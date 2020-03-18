#include "radiostation.h"

#include "bupboard.h"
#include "otherlib.h"
#include "crc16.h"
#include "MDR32F9Qx_uart.h"
#include <string.h>
#include <stdlib.h>


/**********************************************************************
  В текущей версии ПО радиостанции имеется 
  странный баг, приводящий ее к случайным перезагрузкам.
  В довесок после такой перезагрузки радиостанция на 
  любые запросы отвечает пустыми пакетами, поэтому получить 
  от нее данные не представляется возможным. Однако, если 
  не посылать станции ничего (совсем!) в районе 3 сек,
	что было установлено опытным путем, радиостанция вновь
  сможет верно отвечать на запросы. Данный макрос включает
  обход этого бага, путем блокировки любых транзаций со станцией
  на заданное время, если не удаётся получить ответ;
	
	Возможно позже этот баг починят, и тогда этот макрос можно
  будет просто закоментировать.
***********************************************************************/
#define REBOOT_BUG
#define RADIO_BLOCK_TIMEOUT       3000     // Время блокировки обращений к радиостанции, мc


/**********************************************************************
  Этот баг связан с попыткой обмануть заложенный изготовителем
  порядок работы c интерфейсом, так как такой формат не подходит нам.
  Проблема в том, что станция игнориует запросы с информации, отвечая 
	на них всегда пустыми пакетами. Чтобы запрос прошел, необходимо 
  выполнить некоторую последовательность действий, установленных 
  опытным путем:
    1) послать пустой пакет;
    2) выдержать паузу (~20 мс) 
    3) и только потом посылать запрос информации
  Если данные макросы определены, то включен обход этого бага (костыль).
  Этот баг может уйти позже, когда ПО радиостанции обновят, поэтому
	закоментировав макросы ниже, можно отключить данный костыль.	
***********************************************************************/
#define IGNORE_REQUEST_BUG
#define RADIO_REQUEST_DELAY       20   // Пауза между пустым и информациооным запросом



uint8_t buffer[PACKET_SIZE_LIMIT];
uint8_t sdsCounter = 0;
uint8_t sdsList[20];
uint8_t frameIndex = 0;
TimeoutType globalTimeout = {0, 1, TIME_IS_UP}; 
uint32_t currentBaudrate = RADIO_DEFAULT_BAUDRATE;


#define resetCurrentBaudrate()  currentBaudrate = RADIO_DEFAULT_BAUDRATE
#define setCurrentBaudrate(x)   currentBaudrate = x
#define getCurrentBaudrate()    currentBaudrate

/*****************
  Коды команд
******************/
enum DataType{
  AUDIO         = 2,  // Некодированные звуковые данные
  TETRA_VOCODER = 4,  // Кодированные данные вокодера TETRA
  AT_COMMAND    = 5,  // AT-команды
  DEVICE_DATA   = 6,  // Данные от радиостанции к ООД
  HOST_DATA     = 7,  // Данные от ООД к радиостанции
};

/**************************
  Маркеры SLIP-протокола
**************************/
enum SlipMarkers{
  FEND  = 0xC0,   // Начало кадра
  TFEND	=	0xDC,		// Если FEND - не разделитель, то посылается как FESC TFEND
  FESC  = 0xDB,		// Если FEND - не разделитель, то посылается как FESC TFEND
  TFESC	= 0xDD,		// Если FESC - не разделитель, то посылается как FESC TFESC
};

/***********************
  Базовая часть кадра
************************/
typedef __packed struct{
	uint8_t  index;
	uint8_t  address;
	uint16_t length;
	uint16_t crc;
}RadioBaseFrame;
typedef union { 
  RadioBaseFrame Struct;
  uint8_t Buffer[6];
}RadioBaseFrameType;
/***********************
  Данные кадра
************************/
typedef __packed struct{
  uint8_t  type;
	uint16_t length;
	uint8_t* data;
}RadioDataFrame;
typedef union { 
  RadioDataFrame Struct;
  uint8_t        Buffer[7];
}RadioDataFrameType;



/***** Приватная часть модуля **********************************************************/
static uint16_t    swapUint16               (uint16_t value);
static uint32_t    swapUint32               (uint32_t value);
static uint16_t    receiveByte              (MDR_UART_TypeDef* UARTx);
static int16_t     sendByte                 (MDR_UART_TypeDef* UARTx, uint16_t byte);
static uint16_t    sendFend                 (MDR_UART_TypeDef* UARTx);
static void        Radio_send               (uint8_t index, uint8_t *data, uint8_t size);
static RadioStatus Radio_receive            (uint8_t index, uint8_t* data, uint16_t *size);
static void        Radio_initialize         (uint32_t baudrate);
static void        Radio_deinitialize       (void);
RadioStatus        parseForDeviceName       (uint8_t *data);
RadioStatus        parseForManufacturerName (uint8_t *data);
RadioStatus        parseForListSDS          (uint8_t *data);
RadioStatus        parseForCoordinates      (uint8_t *data, uint8_t *sended, double *latitude, double *longitude);
RadioStatus        parseForDeleteStatus     (uint8_t *data, uint8_t *sended);
char*              findTag                  (char* string, int length, const char* tag, int tagSize);
void               itoa                     (int value, char* result);
static void        someMagic                (void);
static void        setSdsCount              (uint8_t count);
static uint8_t     getSdsId                 (uint8_t index);


/****** Публичная часть модуля  **********************************************************/

/**************************************************************************************************************
  sendEmpty - Запрос-пустышка
  Возвращает:
            Результат выполнения
  Примечание: Может использоваться для проверки связи с рацией,
	          или для удержания радиостанции в режиме rs485 (см РЭ на радиостанцию)
***************************************************************************************************************/
RadioStatus sendEmpty(void)
{
  RadioStatus status = RADIO_FAILED; 
  uint16_t    size = 0;
  TimeoutType timeout;
	setTimeout (&timeout, 100); 
	Radio_initialize(getCurrentBaudrate());
	Radio_send(frameIndex, 0, 0);
	while (timeoutStatus(&timeout) != TIME_IS_UP){
	  if(Radio_receive(frameIndex, buffer, &size) != RADIO_SUCCESS){
		  Radio_send(frameIndex, 0, 0);
		  continue;
    }
		else {		
		  status = RADIO_SUCCESS; // Если попали сюда, то пакет был принят
		  break;                  // И даже пакет был верным
    }
	}
	memset(buffer, 0, size);     
  Radio_deinitialize();
	frameIndex++;
	return status;
}
/**************************************************************************************************************
  checkDeviceName - Проверка имени устройства
  Возвращает:
            Результат выполнения
***************************************************************************************************************/
RadioStatus checkDeviceName(void)
{
  RadioStatus status = RADIO_FAILED;   // Статус выполнения по-умолчанию
  uint16_t    size = 0;                // Размер принятых данных
  uint8_t     data[] = "ATI";          // Команда
  TimeoutType timeout;                 // Таймаут

#if defined REBOOT_BUG	
  if(timeoutStatus(&globalTimeout) != TIME_IS_UP)                  // См. описание макроса REBOOT_BUG
    return RADIO_BLOCKED;
#endif
	
	setTimeout (&timeout, RADIO_TRANSACTION_TIMEOUT);                // Устанавлливаем таймаут
	Radio_initialize(getCurrentBaudrate());                          // Включаем обмен
	
#if defined IGNORE_REQUEST_BUG                                     // См. Описание макроса IGNORE_REQUEST_BUG
  someMagic();                                                     // Немного магии
#endif

	Radio_send(frameIndex, data, sizeof(data)-1);                    // Посылаем команду
	while (timeoutStatus(&timeout) != TIME_IS_UP){                   // И следим за таймаутом
	  if(Radio_receive(frameIndex, buffer, &size) != RADIO_SUCCESS){ // Принимаем ответ
		  Radio_send(frameIndex, 0, 0);                                // Если ответа не последовало или ответ с ошибкой
			continue;                                                    // Запрос необходимо повторить с этим же индексом
    }
		if(parseForDeviceName (buffer) != RADIO_SUCCESS){   // Сюда попали если ответ был принят
			frameIndex++;                                     // Но нужно проверить, тот ли ответ был принят
		  Radio_send(frameIndex, 0, 0);                     // Если ответ не тот, необходимо посылать пустые запросы
			continue;                                         // До тех пор пока не получим нужное
		}
		else {		
		  status = RADIO_SUCCESS; // Если попали сюда, то пакет был принят
		  break;                  // И даже пакет был верным
    }
	}
	memset(buffer, 0, size);    // Приёмный буфер необходимо очистить
  Radio_deinitialize();       // Выключаем обмен
	frameIndex++;               // Инкремент индекса кадра

#if defined REBOOT_BUG		
	if(status == RADIO_FAILED){
	  resetCurrentBaudrate();
	  setTimeout (&globalTimeout, RADIO_BLOCK_TIMEOUT);                // См. описание макроса REBOOT_BUG
  }
#endif

	return status;
}
/**************************************************************************************************************
  checkManufacturerName - Проверка имени производителя
  Возвращает:
            Результат выполнения
***************************************************************************************************************/
RadioStatus checkManufacturerName(void)
{
  RadioStatus status = RADIO_FAILED;   // Статус выполнения по-умолчанию
  uint16_t    size = 0;                // Размер принятых данных
  uint8_t     data[] = "AT+GMI";       // Команда
  TimeoutType timeout;                 // Таймаут

#if defined REBOOT_BUG		
  if(timeoutStatus(&globalTimeout) != TIME_IS_UP)                  // См. описание макроса REBOOT_BUG
    return RADIO_BLOCKED;
#endif
	
	setTimeout (&timeout, RADIO_TRANSACTION_TIMEOUT);                // Устанавлливаем таймаут
	Radio_initialize(getCurrentBaudrate());                          // Включаем обмен
	
#if defined IGNORE_REQUEST_BUG	                                   // См. Описание макроса IGNORE_REQUEST_BUG
  someMagic();                                                     // Немного магии
#endif

	Radio_send(frameIndex, data, sizeof(data)-1);                    // Посылаем команду
	while (timeoutStatus(&timeout) != TIME_IS_UP){                   // И следим за таймаутом
	  if(Radio_receive(frameIndex, buffer, &size) != RADIO_SUCCESS){ // Принимаем ответ
		  Radio_send(frameIndex, 0, 0);                                // Если ответа не последовало или ответ с ошибкой
			continue;                                                    // Запрос необходимо повторить с этим же индексом
    }
		if(parseForManufacturerName (buffer) != RADIO_SUCCESS){    // Сюда попали если ответ был принят
			frameIndex++;                                     // Но нужно проверить, тот ли ответ был принят
		  Radio_send(frameIndex, 0, 0);                     // Если ответ не тот, необходимо посылать пустые запросы
			continue;                                         // До тех пор пока не получим нужное
		}
		else {		
		  status = RADIO_SUCCESS; // Если попали сюда, то пакет был принят
		  break;                  // И даже пакет был верным
    }
	}
	memset(buffer, 0, size);    // Приёмный буфер необходимо очистить
  Radio_deinitialize();       // Выключаем обмен
	frameIndex++;               // Инкремент индекса кадра

#if defined REBOOT_BUG		
  if(status == RADIO_FAILED){
	  resetCurrentBaudrate();
    setTimeout (&globalTimeout, RADIO_BLOCK_TIMEOUT);                // См. описание макроса REBOOT_BUG
  }
#endif
	
	return status;
}
/**************************************************************************************************************
  updateSdsList - Обновить список SDS сообщений
  Возвращает:
            Результат выполнения
***************************************************************************************************************/
RadioStatus updateSdsList(void)
{
  RadioStatus status = RADIO_FAILED;   // Статус выполнения по-умолчанию
  uint16_t    size = 0;                // Размер принятых данных
  uint8_t     data[] = "AT+CSDSCNT?";  // Команда
  TimeoutType timeout;                 // Таймаут

#if defined REBOOT_BUG		
  if(timeoutStatus(&globalTimeout) != TIME_IS_UP)                  // См. описание макроса REBOOT_BUG
    return RADIO_BLOCKED;
#endif
	
	setTimeout (&timeout, RADIO_TRANSACTION_TIMEOUT);                // Устанавлливаем таймаут
	Radio_initialize(getCurrentBaudrate());                          // Включаем обмен
	
#if defined IGNORE_REQUEST_BUG                                     // См. Описание макроса IGNORE_REQUEST_BUG
  someMagic();                                                     // Немного магии
#endif

	Radio_send(frameIndex, data, sizeof(data)-1);                    // Посылаем команду
	while (timeoutStatus(&timeout) != TIME_IS_UP){                   // И следим за таймаутом
	  if(Radio_receive(frameIndex, buffer, &size) != RADIO_SUCCESS){ // Принимаем ответ
		  Radio_send(frameIndex, 0, 0);                                // Если ответа не последовало или ответ с ошибкой
			continue;                                                    // Запрос необходимо повторить с этим же индексом
    }
		if(parseForListSDS (buffer) != RADIO_SUCCESS){      // Сюда попали если ответ был принят
			frameIndex++;                                     // Но нужно проверить, тот ли ответ был принят
		  Radio_send(frameIndex, 0, 0);                     // Если ответ не тот, необходимо посылать пустые запросы
			continue;                                         // До тех пор пока не получим нужное
		}
		else {		
		  status = RADIO_SUCCESS; // Если попали сюда, то пакет был принят
		  break;                  // И даже пакет был верным
    }
	}
	memset(buffer, 0, size);    // Приёмный буфер необходимо очистить
  Radio_deinitialize();       // Выключаем обмен
	frameIndex++;               // Инкремент индекса кадра

#if defined REBOOT_BUG			
  if(status == RADIO_FAILED) {
    resetCurrentBaudrate();
    setTimeout (&globalTimeout, RADIO_BLOCK_TIMEOUT);                // См. описание макроса REBOOT_BUG
  }
#endif
	
	return status;
}
/**************************************************************************************************************
  getCoordinatesFromSds - Поиск координат в SDS сообщении с идентификатором idSds;
  Параметры:
            idSds - Идентификатор SDS сообщения
						latitude - Указатель на переменную широта для сохранения
						longitude - Указатель на переменную долгота для сохранения
  Возвращает:
            Результат выполнения
						Долготу, широту через указатель, если были найдены
***************************************************************************************************************/
RadioStatus getCoordinatesFromSds(int idSds, double *latitude, double *longitude)
{
  uint16_t    size = 0;                  // Размер принятых данных
  uint8_t     data[20] = "AT+CSDSRD=";   // Команда
  TimeoutType timeout;                   // Таймаут
  RadioStatus status = RADIO_FAILED;     // Статус выполнения по-умолчанию
	char id[4];                            //
	itoa(idSds, id);                       // Числовой id, конвертируем в строковый
	memcpy(data+10, id, strlen(id));       // Вставляем id в строку с командой, перекрывая нуль терминатор

#if defined REBOOT_BUG	
  if(timeoutStatus(&globalTimeout) != TIME_IS_UP)                  // См. описание макроса REBOOT_BUG
    return RADIO_BLOCKED;
#endif
	
	setTimeout (&timeout, RADIO_TRANSACTION_TIMEOUT);                // Устанавлливаем таймаут
	Radio_initialize(getCurrentBaudrate());                          // Включаем обмен

#if defined IGNORE_REQUEST_BUG                                     // См. Описание макроса IGNORE_REQUEST_BUG
	someMagic();                                                     // Немного магии
#endif

	Radio_send(frameIndex, data, sizeof(data)-1);                    // Посылаем команду
	while (timeoutStatus(&timeout) != TIME_IS_UP){                   // И следим за таймаутом
	  if(Radio_receive(frameIndex, buffer, &size) != RADIO_SUCCESS){ // Принимаем ответ
		  Radio_send(frameIndex, 0, 0);                                // Если ответа не последовало или ответ с ошибкой
			continue;                                                    // Запрос необходимо повторить с этим же индексом
    }
		if(parseForCoordinates (buffer, data+2, latitude, longitude) == RADIO_FAILED){    // Сюда попали если ответ был принят
			frameIndex++;                                     // Но нужно проверить, тот ли ответ был принят
		  Radio_send(frameIndex, 0, 0);                     // Если ответ не тот, необходимо посылать пустые запросы
			continue;                                         // До тех пор пока не получим нужное
		}
		else {		
		  status = RADIO_SUCCESS; // Если попали сюда, то пакет был принят
		  break;                  // И даже пакет был верным
    }
	}
	memset(buffer, 0, size);    // Приёмный буфер необходимо очистить
  Radio_deinitialize();       // Выключаем обмен
	frameIndex++;               // Инкремент индекса кадра

#if defined REBOOT_BUG		
  if(status == RADIO_FAILED){
    resetCurrentBaudrate();
    setTimeout (&globalTimeout, RADIO_BLOCK_TIMEOUT);                // См. описание макроса REBOOT_BUG
  }
#endif
	
	return status;
}
/**************************************************************************************************************
  deleteSds - Стереть SDS сообщение с идентификатором idSds
  Параметры:
            idSds - Идентификатор SDS сообщения
  Возвращает:
            Результат выполнения
***************************************************************************************************************/
RadioStatus deleteSds(uint8_t idSds)
{
  uint8_t     data[20] = "AT+CSDSRM=";   // Команда
  uint16_t    size = 0;                  // Размер принятых данных
  TimeoutType timeout;                   // Таймаут 
	RadioStatus status = RADIO_FAILED;     // Статус выполнения по-умолчанию
	char id[4];                            //
	itoa(idSds, id);                       // Числовой id, конвертируем в строковый
	memcpy(data+10, id, strlen(id));       // Вставляем id в строку с командой, перекрывая нуль терминатор

#if defined REBOOT_BUG		
  if(timeoutStatus(&globalTimeout) != TIME_IS_UP)                  // См. описание макроса REBOOT_BUG
    return RADIO_BLOCKED;
#endif
	
	setTimeout (&timeout, RADIO_TRANSACTION_TIMEOUT);                // Устанавлливаем таймаут
	Radio_initialize(getCurrentBaudrate());                          // Включаем обмен
	
#if defined IGNORE_REQUEST_BUG                                     // См. Описание макроса IGNORE_REQUEST_BUG
	someMagic();                                                     // Немного магии
#endif
	
	Radio_send(frameIndex, data, sizeof(data)-1);                    // Посылаем команду
	while (timeoutStatus(&timeout) != TIME_IS_UP){                   // И следим за таймаутом
	  if(Radio_receive(frameIndex, buffer, &size) != RADIO_SUCCESS){ // Принимаем ответ
		  Radio_send(frameIndex, 0, 0);                                // Если ответа не последовало или ответ с ошибкой
			continue;                                                    // Запрос необходимо повторить с этим же индексом
    }
		if(parseForDeleteStatus (buffer, data+2) != RADIO_SUCCESS){    // Сюда попали если ответ был принят
			frameIndex++;                                     // Но нужно проверить, тот ли ответ был принят
		  Radio_send(frameIndex, 0, 0);                     // Если ответ не тот, необходимо посылать пустые запросы
			continue;                                         // До тех пор пока не получим нужное
		}
		else {		
		  status = RADIO_SUCCESS; // Если попали сюда, то пакет был принят
		  break;                  // И даже пакет был верным
    }
	}
	memset(buffer, 0, size);    // Приёмный буфер необходимо очистить
  Radio_deinitialize();       // Выключаем обмен
	frameIndex++;               // Инкремент индекса кадра

#if defined REBOOT_BUG		
  if(status == RADIO_FAILED){
    resetCurrentBaudrate();
    setTimeout (&globalTimeout, RADIO_BLOCK_TIMEOUT);                // См. описание макроса REBOOT_BUG
  }
#endif
	
	return status;
}
/**************************************************************************************************************
  setBaudrate - Установить скорость интерфейса
  Параметры:
            baudrate - Скорость в бодах (9600, 115200, 230400, 460800);
  Возвращает:
            Результат выполнения
***************************************************************************************************************/
RadioStatus setBaudrate (uint32_t baudrate)
{
  uint8_t     data[20] = "ATS23=";       // Команда
  uint16_t    size = 0;                  // Размер принятых данных
  TimeoutType timeout;                   // Таймаут 
	RadioStatus status = RADIO_FAILED;     // Статус выполнения по-умолчанию
	char stringBaudrate[7];                // Строковый бодрейт
	itoa(baudrate, stringBaudrate);        // Числовой бодрейт, конвертируем в строковый
	memcpy(data+6, stringBaudrate, strlen(stringBaudrate));       // Вставляем бодрейт в строку с командой, перекрывая нуль терминатор
	
#if defined REBOOT_BUG		
  if(timeoutStatus(&globalTimeout) != TIME_IS_UP)                  // См. описание макроса REBOOT_BUG
    return RADIO_BLOCKED;
#endif
	
	setTimeout (&timeout, RADIO_TRANSACTION_TIMEOUT);                // Устанавлливаем таймаут
	Radio_initialize(getCurrentBaudrate());                          // Включаем обмен
	
#if defined IGNORE_REQUEST_BUG                                     // См. Описание макроса IGNORE_REQUEST_BUG
	someMagic();                                                     // Немного магии
#endif

  Radio_send(frameIndex, data, sizeof(data)-1);                    // Посылаем команду
	while (timeoutStatus(&timeout) != TIME_IS_UP){                   // И следим за таймаутом
	  if(Radio_receive(frameIndex, buffer, &size) != RADIO_SUCCESS){ // Принимаем ответ
		  Radio_send(frameIndex, 0, 0);                                // Если ответа не последовало или ответ с ошибкой
			continue;                                                    // Запрос необходимо повторить с этим же индексом
    }
		else {		
		  status = RADIO_SUCCESS;           // Если попали сюда, то пакет валидным
			setCurrentBaudrate(baudrate);     // Бодрейт всего модуля можно обновить
		  break;
    }
	}
	memset(buffer, 0, size);    // Приёмный буфер необходимо очистить
  Radio_deinitialize();       // Выключаем обмен
	frameIndex++;               // Инкремент индекса кадра

#if defined REBOOT_BUG		
  if(status == RADIO_FAILED){
	  resetCurrentBaudrate();
    setTimeout (&globalTimeout, RADIO_BLOCK_TIMEOUT);                // См. описание макроса REBOOT_BUG
  }
#endif
	
	return status;
}
/**************************************************************************************************************
  deleteAllSds - Стереть все имеющиеся SDS сообщения
  Возвращает:
            Результат выполнения
***************************************************************************************************************/
RadioStatus deleteAllSds (void)
{
  if(updateSdsList() != RADIO_SUCCESS)            // Здесь запрашиваем список SDS сообщений
	  return RADIO_FAILED;                          // Если здесь его не получили, то дальше идти бессмысленно
	for(uint16_t i = 0; i < getSdsCount(); i++){    // Из полученного списка начинаем брать по очереди id
    if(deleteSds(getSdsId(i)) != RADIO_SUCCESS)   // И удалять, пока не пройдем по всем
      return RADIO_FAILED;
	}
	updateSdsList();                                // Обновим список SDS сообщений
	return RADIO_SUCCESS;
}
/**************************************************************************************************************
  findCoordinateInSdsList - Поиск среди SDS сообщений закодированных координат
  Возвращает:
            Результат выполнения
  Приимечание:
             Ищет в списке сообщений, то которое содержит координаты, и извлекает их,
						 остальные удаляёт, как и то, из которого только что были извлечены координаты
***************************************************************************************************************/
RadioStatus findCoordinateInSdsList(double *latitude, double *longitude)
{
  RadioStatus status = RADIO_FAILED;
  for(uint16_t i = 0; i < getSdsCount(); i++){                               // В порядке очереди начинаем
	  status = getCoordinatesFromSds(getSdsId(i), latitude, longitude);        // вычитывать все SDS сообщения
		if(status == RADIO_SUCCESS || status == RADIO_COORDINATES_NOT_FOUND){    // Если координаты успешно прочитаны
      deleteSds(getSdsId(i));                                                // либо сообщение прочитано, но там
      break;                                                                 // нет координат, то его можно удалить
    }
	}                                                                          // В противном случае при других статусах
	return status;                                                             // все равно необходимо будет прочитать сообщение
}                                                                            // Поэтому пока не удаляем

/***** Приватная часть модуля **********************************************************/

/**************************************************************************************************************
  parseForDeviceName - Распарсить ответ на запрос имени устройства и проверить его валидность
  Параметры:
            data - Буфер, в котором размещено принятое сообщение
  Возвращает:
            Валидность ответа;
***************************************************************************************************************/
RadioStatus parseForDeviceName(uint8_t *data)
{
	const char name[] = {0xD0, 0x2D, 0x31, 0x38, 0x37, 0x2D, 0xCF, 0x31, 0x0A}; // "Р-187-П1\n"
  RadioDataFrameType dataFrame;
	dataFrame.Struct.type = data[0];
	dataFrame.Struct.length = *((uint16_t*)(data+1));
	dataFrame.Struct.data = data;
  dataFrame.Struct.length = swapUint16(dataFrame.Struct.length); 
		
  if(dataFrame.Struct.type   != AT_COMMAND ||
	   dataFrame.Struct.length != 0x08       ||
     strncmp(name, (char*)(data+3), sizeof(name)-1) != NULL)
		 return RADIO_FAILED;
  else
		 return RADIO_SUCCESS;
}
/**************************************************************************************************************
  parseForManufacturerName - Распарсить ответ на запрос имени производителя и проверить его валидность
  Параметры:
            data - Буфер, в котором размещено принятое сообщение
  Возвращает:
            Валидность ответа;
***************************************************************************************************************/
RadioStatus parseForManufacturerName(uint8_t *data)
{
	const char name[] = {0xC0, 0xED, 0xE3, 0xF1, 0xF2, 0xF0, 0xE5, 0xEC, 0x0A}; // "Ангстрем\n"
  RadioDataFrameType dataFrame;
	dataFrame.Struct.type = data[0];
	dataFrame.Struct.length = *((uint16_t*)(data+1));
	dataFrame.Struct.data = data;
  dataFrame.Struct.length = swapUint16(dataFrame.Struct.length); 
		
  if(dataFrame.Struct.type   != AT_COMMAND ||
	   dataFrame.Struct.length != 0x08       ||
     strncmp(name, (char*)(data+3), sizeof(name)-1) != NULL)
		 return RADIO_FAILED;
  else
		 return RADIO_SUCCESS;
}
/**************************************************************************************************************
  parseForListSDS - Распарсить ответ на запрос списка сообщений и проверить его валидность.
  Параметры:
            data - Буфер, в котором размещено принятое сообщение
  Возвращает:
            Валидность ответа;
***************************************************************************************************************/
RadioStatus parseForListSDS(uint8_t *data)
{
	const char tag[] = "+CSDSCNT:";
	char delimiter = ',';
  RadioDataFrameType dataFrame;
	dataFrame.Struct.type   = data[0];
	dataFrame.Struct.length = *((uint16_t*)(data+1));
	dataFrame.Struct.data = data;
  dataFrame.Struct.length = swapUint16(dataFrame.Struct.length); 
	
	char* sdsText = findTag((char*)(data), (int)dataFrame.Struct.length, tag, sizeof(tag)-1);
	
	if(dataFrame.Struct.type != AT_COMMAND ||    // Тип команды должен быть AT-COMMAND  
	   sdsText == NULL)                          // Указатель на начало текста сообщения
	  return RADIO_FAILED;                       // не должен быть null, если сообщение валидно
	
	setSdsCount(atoi(sdsText));                   // Сохраняем количество имеющихся сообщений
	/* Согласно полученному количеству сообщений нужно, 
	произвести такое же количество считываний id 
	сообщений, разделенных ',' */
	for(int i = 0; i < getSdsCount(); i++)       
	{
	  /* Ищем ближайший ',' в сообщении, получаем адрес байта 
		следующим сразу за запятой - это и есть id, получаем 
		его числовое значение и опять по-новой.
    Так как мы последовательно от разделителя до разделителя 
		извлекаем id SDS-сообщений, и размер области поиска нужно пересчитывть:
    длина пакета - (адрес прошлого обращения - (адрес с которого начинаются именно данные):
    (length      - (sdsText 	               - (data+3)	*/
	  sdsText = findTag(sdsText, ((int)dataFrame.Struct.length - ((int)sdsText - (int)(data+3))), &delimiter, 1); 
		sdsList[i] = atoi(sdsText);
	}
  return RADIO_SUCCESS;
}
/**************************************************************************************************************
  parseForCoordinates - Распарсить ответ на команду чтения сообщения и проверить его валидность.
	                      В соответсвтии с заданным шаблоном найти в сообщении координаты    
  Параметры:
            data - Буфер, в котором размещено принятое сообщение
						sended - Указатель текст запроса
						latitude - Указатель на внешнюю переменную широта;
						longitude - Указатель на внешнюю переменную долгота;
  Возвращает:
            Валидность ответа;
						Найденые в сообщение широту и долготу(через указатель);
***************************************************************************************************************/
RadioStatus parseForCoordinates(uint8_t *data, uint8_t *sended, double *latitude, double *longitude)
{
	const char commandTag[] = {0xD2, 0xCF, 0x2B, 0x0A};          //"ТП+"
	const char latitudeTag[] = {0xD8, 0xC8, 0xD0, 0x3A, 0x0A};   // "ШИР:"
	const char longitudeTag[] = {0xC4, 0xCE, 0xCB, 0x3A, 0x0A};  // "ДОЛ:"
	char* latAddress;
	char* lonAddress;
	RadioStatus status = RADIO_FAILED;
	
  RadioDataFrameType dataFrame;
	dataFrame.Struct.type   = data[0];
	dataFrame.Struct.length = *((uint16_t*)(data+1));
	dataFrame.Struct.data = data;
  dataFrame.Struct.length = swapUint16(dataFrame.Struct.length); 
	
  // Проверим на наш ли запрос пришел ответ
	if(findTag((char*)data, (int)dataFrame.Struct.length, (char*)sended, strlen((char*)sended)-1) == NULL)
		 return RADIO_FAILED; // Точно не то, что мы ждали
	
  if(findTag((char*)(data), (int)dataFrame.Struct.length, commandTag, sizeof(commandTag)-1) != NULL)
	{
    latAddress = findTag((char*)(data), (int)dataFrame.Struct.length, latitudeTag,  sizeof(latitudeTag)-1);
    lonAddress = findTag((char*)(data), (int)dataFrame.Struct.length, longitudeTag, sizeof(longitudeTag)-1);					 
		if(latAddress != NULL || lonAddress != NULL || dataFrame.Struct.type != AT_COMMAND)
		{
      *latitude = atof(latAddress);
      *longitude = atof(lonAddress);
			status = RADIO_SUCCESS;
		}
		else
      status = RADIO_COORDINATES_NOT_FOUND;
  }
	else 
    status = RADIO_COORDINATES_NOT_FOUND; 
		
  return status;
}
/**************************************************************************************************************
  parseForDeleteStatus - Распарсить ответ на команду удаления и проверить его валидность
  Параметры:
            data   - Буфер, в котором размещено принятое сообщение
						sended - Указатель на строку, содержащую ожидаемый (эталон) ответ
  Возвращает:
            Валидность ответа;
***************************************************************************************************************/
RadioStatus parseForDeleteStatus (uint8_t *data, uint8_t *sended)
{
  RadioDataFrameType       dataFrame;
	dataFrame.Struct.type    = data[0];
	dataFrame.Struct.length  = *((uint16_t*)(data+1));
	dataFrame.Struct.data    = data;
  dataFrame.Struct.length  = swapUint16(dataFrame.Struct.length); 
	
	// По полученному адресу можем узнать содержит ли, буфер с сообщением, образцовую строку sended
	char* address = findTag((char*)data, (int)dataFrame.Struct.length, (char*)sended, strlen((char*)sended)-1);
		
  if(dataFrame.Struct.type != AT_COMMAND || address == NULL)
		 return RADIO_FAILED;
  else
		 return RADIO_SUCCESS;
}
/**************************************************************************************************************
  Radio_initialize - Инициализация UART под радиостанцию
***************************************************************************************************************/
void Radio_initialize (uint32_t baudrate)
{
  UART_InitTypeDef UART_InitStructure;
	
	/* Инициализируем пины UART под радиостанцию */
	Pin_initialize (RADIO_RX);
  Pin_initialize (RADIO_TX);
	
  UART_DeInit(RADIO_UART);                      /* Сброс конфигуряции UART1 */
  UART_StructInit (&UART_InitStructure);        /* Заполнение структуры по-умолчанию */
  UART_BRGInit (RADIO_UART, UART_HCLKdiv1);     /* Установка предделителя блока UART1_CLK = HCLK */
  /* Заполняем структуру инициализации */
  UART_InitStructure.UART_BaudRate                = baudrate;
  UART_InitStructure.UART_WordLength              = UART_WordLength8b;
  UART_InitStructure.UART_StopBits                = UART_StopBits1;
  UART_InitStructure.UART_Parity                  = UART_Parity_No;
  UART_InitStructure.UART_FIFOMode                = UART_FIFO_ON;
  UART_InitStructure.UART_HardwareFlowControl     = UART_HardwareFlowControl_RXE | 
                                                    UART_HardwareFlowControl_TXE;
  UART_Init (RADIO_UART, &UART_InitStructure);  /* Инициализация UART */
  UART_Cmd(RADIO_UART, ENABLE);                 /* Включение UART1 - RADIO */
	
  /* Запоминаем текущий бодрейт */
	setCurrentBaudrate(baudrate);                   
}
/**************************************************************************************************************
  Radio_deinitialize - Деинициализация радиостанции и освобождение UART
***************************************************************************************************************/
void Radio_deinitialize (void)
{
  UART_DeInit(RADIO_UART);       /* Сброс конфигуряции UART */
  UART_Cmd(RADIO_UART, DISABLE); /* Выключение UART1 - RADIO */
  Pin_default (RADIO_RX);        /* Сбрасываем настройку пинов по-умолчанию  */
  Pin_default (RADIO_TX);
}
/**************************************************************************************************************
  Radio_send - Приём пакета данных от радиостанции
  Параметры:
            index - Индекс отправляемого пакета
            data - Команда
            size - Длина команды в байтах			
***************************************************************************************************************/
void Radio_send(uint8_t index, uint8_t *data, uint8_t size)
{
  static TimeoutType sendPause = {0, 1, TIME_IS_UP};
	TimeoutType timeout; 
  RadioBaseFrameType baseFrame;
	RadioDataFrameType dataFrame;
	uint16_t packetSize;
	uint8_t special[2] = {0x0D, 0x0A};
	uint16_t i;
	
	if(timeoutStatus(&sendPause) == TIME_IS_UP)
		setTimeout (&sendPause, RADIO_SEND_PAUSE);
	else
    while(timeoutStatus(&sendPause) != TIME_IS_UP);
  	
  while (UART_GetFlagStatus (RADIO_UART, UART_FLAG_RXFE) != SET)  // Вычищаем FIFO приёмника от мусора
    UART_ReceiveData(RADIO_UART);	
	
	if (size == 0)                     // Как минимум пакет будет состоять из 6 байт
	  packetSize = 6;                  
	else                               // Индекс, адрес, длина(4 байта) + тип данных, длина данных(3 байта) 
	  packetSize = 11 + size;          // + данные(size байт) + спецсимволы:0D,0A(2байта) + crc(2 байта)
	
  // Формируем базовый пакет	
	baseFrame.Struct.index   = index;
	baseFrame.Struct.address = 1;
	baseFrame.Struct.length  = packetSize-6;
	baseFrame.Struct.crc     = CRC16_INITIAL_FFFF;
	// Пакет данных
  dataFrame.Struct.type    = AT_COMMAND;
	dataFrame.Struct.length  = size+2;
	dataFrame.Struct.data    = data;
	
	// Радиостанция в big-endian, а мы в little-endian, поэтому перевернём поля
	baseFrame.Struct.length = swapUint16(baseFrame.Struct.length);
	dataFrame.Struct.length = swapUint16(dataFrame.Struct.length);
	
	// Подсчитываем контрольную сумму базовой части
	baseFrame.Struct.crc = Crc16(&baseFrame.Buffer[0], 4, baseFrame.Struct.crc);
	// Если данные есть, до досчитываем кс с учетом их
  if(size != 0) {
    baseFrame.Struct.crc = Crc16(&dataFrame.Buffer[0], 3,    baseFrame.Struct.crc);
		baseFrame.Struct.crc = Crc16(data,                 size, baseFrame.Struct.crc);
		baseFrame.Struct.crc = Crc16(special,              2,    baseFrame.Struct.crc);
	}
	// Контрольную сумму тоже перевернем в big endian
	baseFrame.Struct.crc = swapUint16(baseFrame.Struct.crc);
	
	// Начинаем отправлять 
  sendFend (RADIO_UART);
	// Базовая часть пакета
	for(i = 0; i < 4; i++)
		sendByte (RADIO_UART, baseFrame.Buffer[i]);
	// Пакет данных	
	if(size != 0) {
    for(i = 0; i < 3; i++)
		  sendByte (RADIO_UART, dataFrame.Buffer[i]);
 	  for(i = 0; i < size; i++)
		  sendByte (RADIO_UART, data[i]);
		for(i = 0; i < 2; i++)
			sendByte(RADIO_UART, special[i]);
  }
	// Контрольная сумма всего пакета
  for(i = 4; i < 6; i++)
    sendByte (RADIO_UART, baseFrame.Buffer[i]);
	
	// Дождемся пока все отправится из FIFO
	setTimeout (&timeout, RADIO_FIFO_TIMEOUT); 
	while ((UART_GetFlagStatus (RADIO_UART, UART_FLAG_TXFE) != SET) 
		    && (timeoutStatus(&timeout) != TIME_IS_UP));
}
/**************************************************************************************************************
  Radio_receive - Приём пакета данных от радиостанции
  Параметры:
            index - Индекс ожидаемого пакета
            data - Буфер, в котором принятый пакет будет размещен
  Возвращает:
            size - Размер принятого пакета (Возвращается через указатель)
            status - статус приёма(успех, или код ошибки) 
***************************************************************************************************************/
RadioStatus Radio_receive(uint8_t index, uint8_t* data, uint16_t *size)
{
  TimeoutType timeout;
	RadioBaseFrameType baseFrame;
	uint16_t crc = CRC16_INITIAL_FFFF;
	uint16_t i;
	
  setTimeout (&timeout, RADIO_RECEIVE_TIMEOUT);  /* Ожидаем начало фрейма 0xC0 */
  while(timeoutStatus(&timeout) != TIME_IS_UP){
    if(receiveByte(RADIO_UART) == FEND)
		  break;
	}
  if(timeout.status == TIME_IS_UP)     /* Если был таймаут, значит связи нет, можно отключиться */
    return RADIO_TIMEOUT;
	
	// Базовая часть пакета
  for(i = 0; i < 4; i++){
    baseFrame.Buffer[i] = receiveByte(RADIO_UART);
    crc = Crc16(&baseFrame.Buffer[i], 1, crc);	
  }
	baseFrame.Struct.length = swapUint16(baseFrame.Struct.length);
	
	// На этом этапе уже можем проверить валидность исключив очевидные варианты
	if(baseFrame.Struct.address != RADIO_ADDRESS)      // Адрес должен быть наш, остальные игнорируем
    return RADIO_WRONG_ADDRESS;
	if(baseFrame.Struct.length > PACKET_SIZE_LIMIT)    // Не используем пакеты больше PACKET_SIZE_LIMIT
    return RADIO_EXCESS_PACKET_SIZE;                 // поэтому если пакет больше, он нам точно неинтересен

	*size = baseFrame.Struct.length;
	// Поле данных
	for(i = 0; i < baseFrame.Struct.length; i++)
	{
    data[i] = receiveByte(RADIO_UART);	// Целиком записываем в буфер
    crc = Crc16(&data[i], 1, crc);	    // И пока только считаем контрольную сумму
	}
	// Контрольная сумма
  for(i = 4; i < 6; i++)
    baseFrame.Buffer[i] = receiveByte(RADIO_UART);
	
	// Проверка контрольной суммы и индекса кадра
	crc = swapUint16(crc);
	if(crc != baseFrame.Struct.crc)
    return RADIO_WRONG_CRC;
	if(baseFrame.Struct.index != index)
	  return RADIO_WRONG_INDEX;
	
	return RADIO_SUCCESS;
}
/**************************************************************************************************************
  sendFend - Отправка Fend разделителя пакетов
  Параметры:
            UARTx - Используемый UART модуль
  Возвращает:
            0x0000, передача прошла успешно
						0xFFFF, если при передаче произошла ошибка или таймаут ожидания;
***************************************************************************************************************/
uint16_t sendFend (MDR_UART_TypeDef* UARTx)
{
  TimeoutType timeout;
	
  setTimeout (&timeout, RADIO_FIFO_TIMEOUT);                  // Если FIFO передатчика заполнен, 
  while ((UART_GetFlagStatus (UARTx, UART_FLAG_TXFF) == SET)  // то ждем пока освободится
  && (timeoutStatus(&timeout) != TIME_IS_UP));
  if(timeout.status == TIME_IS_UP)                            // Если выход из ожидания по таймауту,
    return 0xFFFF;                                            // то возвращаем ошибку и выходим
  UART_SendData(UARTx, FEND);                                 // Иначе отправляем вместо FEND или FESC сначала FESC
	return 0;
}
/**************************************************************************************************************
  sendByte - Отправка байта данных (с кодированием по Slip-протоколу)
  Параметры:
            UARTx - Используемый UART модуль
						byte - Оправляемый байт данных;
  Возвращает:
            0x0000, передача прошла успешно
						0xFFFF, если при передаче произошла ошибка или таймаут ожидания;
***************************************************************************************************************/
int16_t sendByte (MDR_UART_TypeDef* UARTx, uint16_t byte)
{
	TimeoutType timeout;
	
	if(byte == FEND || byte == FESC)                 // Если байт совпадает со спецсимволами
	{                                                // его необходимо закодировать
		setTimeout (&timeout, RADIO_FIFO_TIMEOUT);     // Если FIFO передатчика заполнен, то ждем пока освободится
		while ((UART_GetFlagStatus (UARTx, UART_FLAG_TXFF) == SET) 
		    && (timeoutStatus(&timeout) != TIME_IS_UP));
		if(timeout.status == TIME_IS_UP)               // Если выход из ожидания по таймауту,
		  return 0xFFFF;                               // то возвращаем ошибку и выходим
		UART_SendData(UARTx, FESC);                    // Иначе отправляем вместо FEND или FESC сначала FESC
		while ((UART_GetFlagStatus (UARTx, UART_FLAG_TXFF) == SET) 
		    && (timeoutStatus(&timeout) != TIME_IS_UP));	
		
		if(byte == FEND)                               // Если это спецсимвол FEND, то после FESC посылаем TFEND
		{
			// Если FIFO передатчика заполнен, то ждем пока освободится
			while ((UART_GetFlagStatus (UARTx, UART_FLAG_TXFF) == SET) 
			    && (timeoutStatus(&timeout) != TIME_IS_UP));
			
			if(timeout.status == TIME_IS_UP)    // Если выход из ожидания по таймауту
				return 0xFFFF;                    // то возвращаем ошибку и выходим
			
			UART_SendData(UARTx, TFEND);                                // Иначе отправляем вместо FEND или FESC сначала FESC
		  while ((UART_GetFlagStatus (UARTx, UART_FLAG_TXFF) == SET)  // И ждем пока FIFO освободится
			    && (timeoutStatus(&timeout) != TIME_IS_UP));
		}
		else if (byte == FESC) // Если нужно отправить FESC, то после FESC посылаем еще и TFESC
		{
			while ((UART_GetFlagStatus (UARTx, UART_FLAG_TXFF) == SET)  // Если FIFO передатчика заполнен, 
			    && (timeoutStatus(&timeout) != TIME_IS_UP));            // то ждем пока освободится
			if(timeout.status == TIME_IS_UP)       // Если выход из ожидания по таймауту,
				return 0xFFFF;                       // то возвращаем ошибку и выходим
			UART_SendData(UARTx, TFESC);           // Иначе отправляем вместо FEND или FESC сначала FESC		
		  while ((UART_GetFlagStatus (UARTx, UART_FLAG_TXFF) == SET)  // И ждем пока FIFO освободится
			    && (timeoutStatus(&timeout) != TIME_IS_UP));
		}
	}
	else 
	{
		// Если FIFO передатчика заполнен, то ждем пока освободится
		setTimeout (&timeout, RADIO_FIFO_TIMEOUT);
		while ((UART_GetFlagStatus (UARTx, UART_FLAG_TXFF) == SET) 
		    && (timeoutStatus(&timeout) != TIME_IS_UP));
		// Если выход из ожидания по таймауту - возвращаем ошибку и выходим
		if(timeout.status == TIME_IS_UP)  
			return 0xFFFF;
		// Иначе отправляем исходный символ
		UART_SendData(UARTx, byte);
		// И ждем пока FIFO освободится
		while ((UART_GetFlagStatus (UARTx, UART_FLAG_TXFF) == SET) 
		    && (timeoutStatus(&timeout) != TIME_IS_UP));
	}	
  return 0;
}
/**************************************************************************************************************
  receiveByte - Приём байта данных (с декодированием по Slip-протоколу
  Параметры:
            UARTx - Используемый UART модуль
  Возвращает:
            16разрядное слово, где младший байт - принятый байт, старший = 0, если приём успешен;
						0xFFFF, если при приёме произошла ошибка или таймаут ожидания;
***************************************************************************************************************/
uint16_t receiveByte(MDR_UART_TypeDef* UARTx)
{
	uint16_t    byte = 0;
	TimeoutType timeout; 

	setTimeout (&timeout, RADIO_RECEIVE_TIMEOUT);   // Ждем прихода информации 
	while ((UART_GetFlagStatus (UARTx, UART_FLAG_RXFE) == SET) 
	   && (timeoutStatus(&timeout) != TIME_IS_UP));
	if(timeout.status == TIME_IS_UP)   // Если выход из ожидания по таймауту,
		return 0;                   // то возвращаем ошибку и выходим
	byte = UART_ReceiveData(UARTx);    // Иначе принимаем байт
	if(byte == FESC)	                 // Если поймали символ FESC, то смотрим каким будет следующий
	{
		setTimeout (&timeout, RADIO_BYTE_TIMEOUT);  // Ждем прихода информации
		while ((UART_GetFlagStatus (UARTx, UART_FLAG_RXFE) == SET) 
		    && (timeoutStatus(&timeout) != TIME_IS_UP));
		if(timeout.status == TIME_IS_UP) // Если выход из ожидания по таймауту,
			return 0;                 // то возвращаем ошибку и выходим
		byte = UART_ReceiveData(UARTx);  // Иначе принимаем байт		
		if(byte == TFEND)                // Если принят TFEND
			byte = FEND;                   // Значит это закодированный FEND
		else if (byte == TFESC)          // Если за FESC принимаем TFESC
      byte = FESC;                   // Значит это закодированный FESC
  }
	return byte;
}
/**************************************************************************************************************
  swapUint16 - Изменить порядок байт на обратный в 16х разрядном слове
  Параметры:
            Value - Данные с исходным порядком байт
  Возвращает:
            Value c обратным порядком следования байт
***************************************************************************************************************/
uint16_t swapUint16 (uint16_t value)
{
		return (uint16_t)((value & 0x00FF) << 8 | (value & 0xFF00) >> 8);
}
/**************************************************************************************************************
  swapUint32 - Изменить порядок байт на обратный в 32х разрядном слове
  Параметры:
            Value - Данные с исходным порядком байт
  Возвращает:
            Value c обратным порядком следования байт
***************************************************************************************************************/
uint32_t swapUint32 (uint32_t value)
{
    value = (value & 0x00FF00FF) << 8 | (value & 0xFF00FF00) >> 8;
    value = (value & 0x0000FFFF)<< 16 | (value & 0xFFFF0000) >> 16;
    return value;	
}
/**************************************************************************************************************
  findTag - Поиск тэга в строке
  Параметры:
            string  - Строка или приёмный буфер,где будем искать
						length  - Длина сроки или буфера в байтах
						tag     - Тэг(подстрока) которую ищем
						tagSize - Размер тэга в байтах
  Возвращает:
            Адрес на байта следующим за тэгом;
***************************************************************************************************************/
char* findTag(char* string, int length, const char* tag, int tagSize)
{
  int i, j;
  char* address = 0;
  for(i = 0; i < length; i++)
	{
	  for(j = 0; j < tagSize; j++)
		{
			if(string[i+j] != tag[j])
				break;
		}
		if(j == tagSize)
		{
		  address = &string[i+tagSize];
			break;
    }
	}
	return address;
}
/**************************************************************************************************************
  itoa - Преобразование числа в строку
  Параметры:
            value  - Число
						result - Указатель на строку, массив буфер, результата
***************************************************************************************************************/
void itoa(int value, char* result)
{
	char temp;
	int tempValue;
	int i = 0;
	if(value < 0){
		result[i] = '-';
		value = -value;
		i++;
	}	
  tempValue = value;
	while(tempValue > 0)
	{
		tempValue/=10;
		i++;
	}
	result[i] = '\0';
	while(value > 0)
	{
		temp = value%10;
		value/=10;
		result[i-1] = temp | 0x30;
		i--;
	}
}
/**************************************************************************************************************
  setSdsCount - Установить число SDS сообщений в списке 
  Параметры:
            count - Число доступных сообщений
***************************************************************************************************************/
void setSdsCount (uint8_t count){
  sdsCounter = count;
}
/**************************************************************************************************************
  getSdsCount - Получить число SDS сообщений в списке
  Возвращает:
            Число SDS сообщений в списке
***************************************************************************************************************/
uint8_t getSdsCount (void){
  return sdsCounter;
}
/**************************************************************************************************************
  getSdsId - Получить ID SDS сообщения с индексом index в списке;
  Возвращает:
            Число SDS сообщений в списке
***************************************************************************************************************/
uint8_t getSdsId(uint8_t index){
  if(index < sdsCounter)
    return sdsList[index];
  else 
		return 0;
}

#if defined IGNORE_REQUEST_BUG   
/**************************************************************************************************************
  someMagic - Необходимый порядок действий, для того, чтобы радиостанция правильно выбрала интерфейс
***************************************************************************************************************/
void someMagic(void)
{
  /* Здесь необходимо выполнить танцы с бубном, поскольку неизвестно,
	каким образом радиостанция схватывает rs485?, поэтому опытным путем 
	было выяснено, что перед запросом нужно сделать следующее:
	1) Послать запрос-пустышку;
	2) Получить ответ;
	3) Дать задержку (тут вообще магия, без этого не работает)
	4) Послать нужный запрос; 
	*/
	TimeoutType timeout; 
	uint16_t    size = 0;
	setTimeout (&timeout, 50); 
	Radio_send(frameIndex, 0, 0);
	while (timeoutStatus(&timeout) != TIME_IS_UP){
	  if(Radio_receive(frameIndex, buffer, &size) != RADIO_SUCCESS){
		  Radio_send(frameIndex, 0, 0);
		  continue;
    }
		else
		  break; 
	}
	memset(buffer, 0, size);     
	frameIndex++;
	delay_ms(RADIO_REQUEST_DELAY);
  /* Теперь можно перейти к стандарному 
	исполнению запросов 
	*/
}
#endif
