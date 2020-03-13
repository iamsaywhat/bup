#include "radiostation.h"

#include "bupboard.h"
#include "otherlib.h"
#include "crc16.h"
#include "MDR32F9Qx_uart.h"
#include <string.h>
#include <stdlib.h>

uint8_t buffer[200];
uint8_t frameIndex = 0;

enum DataType{
  AUDIO         = 2,
  TETRA_VOCODER = 4,
  AT_COMMAND    = 5,
  DEVICE_DATA   = 6,
  HOST_DATA     = 7,
};

enum SlipMarkers{
  FEND  = 0xC0,   /* Начало кадра */
  TFEND	=	0xDC,		/* Если FEND - не разделитель, то посылается как FESC TFEND */
  FESC  = 0xDB,		/* Если FEND - не разделитель, то посылается как FESC TFEND */
  TFESC	= 0xDD,		/* Если FESC - не разделитель, то посылается как FESC TFESC */
};


/***** Приватная часть модуля **********************************************************/
static uint16_t    swapUint16 (uint16_t value);
static uint32_t    swapUint32 (uint32_t value);
static uint16_t    receiveByte(MDR_UART_TypeDef* UARTx);
static int16_t     sendByte (MDR_UART_TypeDef* UARTx, uint16_t byte);
static uint16_t    sendFend (MDR_UART_TypeDef* UARTx);
static void        Radio_send (uint8_t index, uint8_t *data, uint8_t size);
static RadioStatus Radio_receive (uint8_t index, uint8_t* data, uint16_t *size);
static void        Radio_initialize (void);
static void        Radio_deinitialize (void);

RadioStatus parseForDeviceName(uint8_t *data);
RadioStatus parseForManufacturerName(uint8_t *data);
RadioStatus parseForListSDS(uint8_t *data);
RadioStatus parseForCoordinates(uint8_t *data, double *latitude, double *longitude);
char* findTag(char* string, int length, const char* tag, int tagSize);


/****** Публичная часть модуля  **********************************************************/
void sendEmpty(void)
{
  uint16_t    size = 0;
  TimeoutType timeout;
	setTimeout (&timeout, 100); 
	Radio_initialize();
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
  Radio_deinitialize();
	frameIndex++;
}
void getDeviceName(void)
{
  uint16_t    size = 0;          // Размер принятых данных
  uint8_t     data[] = "ATI";    // Команда
  TimeoutType timeout;           // Таймаут
	 
	setTimeout (&timeout, 150);                                      // Устанавлливаем таймаут
	Radio_initialize();                                              // Включаем обмен
	Radio_send(frameIndex, data, sizeof(data)-1);                    // Посылаем команду
	while (timeoutStatus(&timeout) != TIME_IS_UP){                   // И следим за таймаутом
	  if(Radio_receive(frameIndex, buffer, &size) != RADIO_SUCCESS){  // Принимаем ответ
		  Radio_send(frameIndex, 0, 0);                                // Если ответа не последовало или ответ с ошибкой
			continue;                                                    // Запрос необходимо повторить с этим же индексом
    }
		if(parseForDeviceName (buffer) != RADIO_SUCCESS){    // Сюда попали если ответ был принят
			frameIndex++;                                     // Но нужно проверить, тот ли ответ был принят
		  Radio_send(frameIndex, 0, 0);                     // Если ответ не тот, необходимо посылать пустые запросы
			continue;                                         // До тех пор пока не получим нужное
		}
		else                  // Если попали сюда, то пакет был принят
		  break;              // И даже пакет был верным
	}
	memset(buffer, 0, size);    // Приёмный буфер необходимо очистить
  Radio_deinitialize();       // Выключаем обмен
	frameIndex++;               // Инкремент индекса кадра
}

void getManufacturerName(void)
{
  uint16_t    size = 0;           // Размер принятых данных
  uint8_t     data[] = "AT+GMI";  // Команда
  TimeoutType timeout;            // Таймаут
	 
	setTimeout (&timeout, 150);                                      // Устанавлливаем таймаут
	Radio_initialize();                                              // Включаем обмен
	Radio_send(frameIndex, data, sizeof(data)-1);                    // Посылаем команду
	while (timeoutStatus(&timeout) != TIME_IS_UP){                   // И следим за таймаутом
	  if(Radio_receive(frameIndex, buffer, &size) != RADIO_SUCCESS){  // Принимаем ответ
		  Radio_send(frameIndex, 0, 0);                                // Если ответа не последовало или ответ с ошибкой
			continue;                                                    // Запрос необходимо повторить с этим же индексом
    }
		if(parseForManufacturerName (buffer) != RADIO_SUCCESS){    // Сюда попали если ответ был принят
			frameIndex++;                                     // Но нужно проверить, тот ли ответ был принят
		  Radio_send(frameIndex, 0, 0);                     // Если ответ не тот, необходимо посылать пустые запросы
			continue;                                         // До тех пор пока не получим нужное
		}
		else                  // Если попали сюда, то пакет был принят
		  break;              // И даже пакет был верным
	}
	memset(buffer, 0, size);    // Приёмный буфер необходимо очистить
  Radio_deinitialize();       // Выключаем обмен
	frameIndex++;               // Инкремент индекса кадра
}

void getListSDS(void)
{
  uint16_t    size = 0;                // Размер принятых данных
  uint8_t     data[] = "AT+CSDSCNT?";  // Команда
  TimeoutType timeout;                 // Таймаут
	 
	setTimeout (&timeout, 150);                                      // Устанавлливаем таймаут
	Radio_initialize();                                              // Включаем обмен
	Radio_send(frameIndex, data, sizeof(data)-1);                    // Посылаем команду
	while (timeoutStatus(&timeout) != TIME_IS_UP){                   // И следим за таймаутом
	  if(Radio_receive(frameIndex, buffer, &size) != RADIO_SUCCESS){  // Принимаем ответ
		  Radio_send(frameIndex, 0, 0);                                // Если ответа не последовало или ответ с ошибкой
			continue;                                                    // Запрос необходимо повторить с этим же индексом
    }
		if(parseForListSDS (buffer) != RADIO_SUCCESS){    // Сюда попали если ответ был принят
			frameIndex++;                                     // Но нужно проверить, тот ли ответ был принят
		  Radio_send(frameIndex, 0, 0);                     // Если ответ не тот, необходимо посылать пустые запросы
			continue;                                         // До тех пор пока не получим нужное
		}
		else                  // Если попали сюда, то пакет был принят
		  break;              // И даже пакет был верным
	}
	memset(buffer, 0, size);    // Приёмный буфер необходимо очистить
  Radio_deinitialize();       // Выключаем обмен
	frameIndex++;               // Инкремент индекса кадра
}
void getSDS(void)
{
  uint16_t    size = 0;                // Размер принятых данных
  uint8_t     data[] = "AT+CSDSRD=4";  // Команда
  TimeoutType timeout;                 // Таймаут
	double latitude;
	double longitude;
	 
	setTimeout (&timeout, 150);                                      // Устанавлливаем таймаут
	Radio_initialize();                                              // Включаем обмен
	Radio_send(frameIndex, data, sizeof(data)-1);                    // Посылаем команду
	while (timeoutStatus(&timeout) != TIME_IS_UP){                   // И следим за таймаутом
	  if(Radio_receive(frameIndex, buffer, &size) != RADIO_SUCCESS){  // Принимаем ответ
		  Radio_send(frameIndex, 0, 0);                                // Если ответа не последовало или ответ с ошибкой
			continue;                                                    // Запрос необходимо повторить с этим же индексом
    }
		if(parseForCoordinates (buffer, &latitude, &longitude) != RADIO_SUCCESS){    // Сюда попали если ответ был принят
			frameIndex++;                                     // Но нужно проверить, тот ли ответ был принят
		  Radio_send(frameIndex, 0, 0);                     // Если ответ не тот, необходимо посылать пустые запросы
			continue;                                         // До тех пор пока не получим нужное
		}
		else                  // Если попали сюда, то пакет был принят
		  break;              // И даже пакет был верным
	}
	memset(buffer, 0, size);    // Приёмный буфер необходимо очистить
  Radio_deinitialize();       // Выключаем обмен
	frameIndex++;               // Инкремент индекса кадра
}




/***** Приватная часть модуля **********************************************************/
RadioStatus parseForDeviceName(uint8_t *data)
{
	const char name[] = {0xD0, 0x2D, 0x31, 0x38, 0x37, 0x2D, 0xCF, 0x31, 0x0A};
  RadioDataFrameType dataFrame;
	dataFrame.Struct.type   = data[0];
	dataFrame.Struct.length = *((uint16_t*)(data+1));
	dataFrame.Struct.data = data;
  dataFrame.Struct.length = swapUint16(dataFrame.Struct.length); 
		
  if(dataFrame.Struct.type   != AT_COMMAND ||
	   dataFrame.Struct.length != 0x08       ||
     strncmp(name, (char*)(data+3), 8) != 0)
		 return RADIO_FAILED;
  else
		 return RADIO_SUCCESS;
}
RadioStatus parseForManufacturerName(uint8_t *data)
{
	const char name[] = {0xC0, 0xED, 0xE3, 0xF1, 0xF2, 0xF0, 0xE5, 0xEC};
  RadioDataFrameType dataFrame;
	dataFrame.Struct.type   = data[0];
	dataFrame.Struct.length = *((uint16_t*)(data+1));
	dataFrame.Struct.data = data;
  dataFrame.Struct.length = swapUint16(dataFrame.Struct.length); 
		
  if(dataFrame.Struct.type   != AT_COMMAND ||
	   dataFrame.Struct.length != 0x08       ||
     strncmp(name, (char*)(data+3), 8) != 0)
		 return RADIO_FAILED;
  else
		 return RADIO_SUCCESS;
}
RadioStatus parseForListSDS(uint8_t *data)
{
	const char name[] = {0xC0, 0xED, 0xE3, 0xF1, 0xF2, 0xF0, 0xE5, 0xEC};
  RadioDataFrameType dataFrame;
	dataFrame.Struct.type   = data[0];
	dataFrame.Struct.length = *((uint16_t*)(data+1));
	dataFrame.Struct.data = data;
  dataFrame.Struct.length = swapUint16(dataFrame.Struct.length); 
		
  if(dataFrame.Struct.type   != AT_COMMAND ||
	   dataFrame.Struct.length != 0x08       ||
     strncmp(name, (char*)(data+3), 8) != 0)
		 return RADIO_FAILED;
  else
		 return RADIO_SUCCESS;
}
RadioStatus parseForCoordinates(uint8_t *data, double *latitude, double *longitude)
{
	const char commandTag[] = {0xD2, 0xCF, 0x2B};
	const char latitudeTag[] = {0xD8, 0xC8, 0xD0, 0x3A};
	const char longitudeTag[] = {0xC4, 0xCE, 0xCB, 0x3A};
	char* latAddress;
	char* lonAddress;
	RadioStatus status = RADIO_FAILED;
	
  RadioDataFrameType dataFrame;
	dataFrame.Struct.type   = data[0];
	dataFrame.Struct.length = *((uint16_t*)(data+1));
	dataFrame.Struct.data = data;
  dataFrame.Struct.length = swapUint16(dataFrame.Struct.length); 
	
  if(findTag((char*)(data), (int)dataFrame.Struct.length, commandTag, sizeof(commandTag) != NULL))
	{
    latAddress = findTag((char*)(data), (int)dataFrame.Struct.length, 
		                   latitudeTag,  sizeof(latitudeTag));
	  
    lonAddress = findTag((char*)(data), dataFrame.Struct.length, 
		                   longitudeTag, sizeof(longitudeTag));
											 
		if(latAddress != NULL || lonAddress != NULL || dataFrame.Struct.type != AT_COMMAND)
		{
      *latitude = atof(latAddress);
      *longitude = atof(lonAddress);
			status = RADIO_SUCCESS;
		}
		else
      status = RADIO_FAILED;
  }
  return status;
}

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
  Radio_initialize - Инициализация UART под радиостанцию
***************************************************************************************************************/
void Radio_initialize (void)
{
  UART_InitTypeDef UART_InitStructure;
	
	/* Инициализируем пины UART под радиостанцию */
	Pin_initialize (RADIO_RX);
  Pin_initialize (RADIO_TX);
	
  UART_DeInit(RADIO_UART);                      /* Сброс конфигуряции UART1 */
  UART_StructInit (&UART_InitStructure);        /* Заполнение структуры по-умолчанию */
  UART_BRGInit (RADIO_UART, UART_HCLKdiv1);     /* Установка предделителя блока UART1_CLK = HCLK */
  /* Заполняем структуру инициализации */
  UART_InitStructure.UART_BaudRate                = RADIO_BAUDRATE;
  UART_InitStructure.UART_WordLength              = UART_WordLength8b;
  UART_InitStructure.UART_StopBits                = UART_StopBits1;
  UART_InitStructure.UART_Parity                  = UART_Parity_No;
  UART_InitStructure.UART_FIFOMode                = UART_FIFO_ON;
  UART_InitStructure.UART_HardwareFlowControl     = UART_HardwareFlowControl_RXE | 
                                                    UART_HardwareFlowControl_TXE;
  UART_Init (RADIO_UART, &UART_InitStructure);  /* Инициализация UART */
  UART_Cmd(RADIO_UART, ENABLE);                 /* Включение UART1 - RADIO */
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
	TimeoutType timeout; 
  RadioBaseFrameType baseFrame;
	RadioDataFrameType dataFrame;
	uint16_t packetSize;
	uint8_t special[2] = {0x0D, 0x0A};
	uint16_t i;
		
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
	
//	sendFend (RADIO_UART);
	
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
		return 0xFFFF;                   // то возвращаем ошибку и выходим
	byte = UART_ReceiveData(UARTx);    // Иначе принимаем байт
	if(byte == FESC)	                 // Если поймали символ FESC, то смотрим каким будет следующий
	{
		setTimeout (&timeout, RADIO_BYTE_TIMEOUT);  // Ждем прихода информации
		while ((UART_GetFlagStatus (UARTx, UART_FLAG_RXFE) == SET) 
		    && (timeoutStatus(&timeout) != TIME_IS_UP));
		if(timeout.status == TIME_IS_UP) // Если выход из ожидания по таймауту,
			return 0xFFFF;                 // то возвращаем ошибку и выходим
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