/*******************************************************************************************************************

    ZPZ.c/h - Модуль обслуживания загрузчика полётного задания (ЗПЗ).
								
    Предоставляет пользователю со стороны ПК следующий функционал:
        - Проверка связи с ПК
        - Загрузка карты высоты рельефа и точку приземления
        - Выгрузка карты высоты рельефа и точку приземления
        - Управление БИМами с ПК
        - Проверка состояния шпилек
        - Проверка состояния системы
        - Передача в CAN произвольного сообщения
        - Передача данных с СНС
        - Передача данных с СВС
        - Работа с хранилищем полетных логов
						
    Аппаратно занимает UART2 и MDR_TIMER2.
					 
    ЗПЗ поключается к UART порту по RS-232 интерфейсу.
					 
    Настройки UART:
        Скорость передачи 115200 Бод;
        8 бит данных, 
        1 стоповый бит, 
        Без контроля четности. 
        (115200, 8-N-1)
					 
					 
    Особенности работы с ЗПЗ:
					 
        1) Тип обмена информацией с ЗПЗ - двунаправленный запрос/ответ;
					 
        2) Контрольная сумма: CRC16, маска 0х1021, начальное значение 0xFFFF; 
					 
        3)	Поток данных разделяется на кадры по RFC 1055 (SLIP протокол).
            Каждый кадр начинается изаканчивается символом FEND (0xC0). 
					
            Исходный пакет        Отправляем как
            |ПАКЕТ|     ->     |FEND||ПАКЕТ||FEND|
					 
            Если FEND (0xC0) содержится в данных  пакета, он посылается внутри кадра как FESC TFEND (0xDB 0xDC). 
					 
            Исходный пакет                      Отправляем как
            |ПАКЕТ.....C0.....ПАКЕТ|   ->   |FEND||ПАКЕТ.....[FESC,TFEND].....ПАКЕТ||FEND|
					 
            Если FESC (0xDB) содержится в данных пакета, он посылается внутри кадра как FESC TFESC (0xDB 0xDD). 
					 
            Исходный пакет                      Отправляем как
            |ПАКЕТ.....DB.....ПАКЕТ|   ->   |FEND||ПАКЕТ.....[FESC,TFESC].....ПАКЕТ||FEND|
					 
            Символ FESC с любым символом, за исключением TFEND или TFESC, является ошибкой. 
            Символы TFEND и TFESC являются
            обычными символами, если им не предшествует FESC. 
					 
            4) Данные в протоколе передаются младшим байтом вперед.
					 
            5) Режим "выполнения высокоприоритетной задачи" (ВВПЗ).
            * В модуле предусмотрен специальный режим выполнения высокоприоритетной задачи.
            Формально введено разделение на режим "разовых команд" (РК) и режим "ВВПЗ".
            Предполагается, что выполнение "РК" не требует максимального быстродействия,
            поэтому в этом режиме модуль может делить ресурс с другими процессами.
            Режим "ВВПЗ" предназначен для ресурсоёмкого обмена большим объемом данных
            (в данном случае это загрузка, выгрузка карты высот и полётного задания, и
            выгрузка логов из черного ящиков). При выполнении таких операций, разделение ресурса
            с другими процессами, может привести к снижению производительности, скорости обмена
            данными или даже к ошибкам. Для того, чтобы быть в курсе настоящего режима работы 
            модуля, можно воспользоваться функцией ZPZ_CheckHighPriorityTask ().
            * Данный режим сопровождается специальной световой индикацией (мигающий "Готов");
					 
					 
            С учетом всего вышеперечисленного, предлагается следующий пример использования модуля ЗПЗ:
					
						
						
            if(!CONNECT_ZPZ_CHECK)                            
            {
              // Запускаем драйвер ЗПЗ 
              ZPZ_init();
              while(!CONNECT_ZPZ_CHECK
              {
                // Запускаем модуль обслуживания ЗПЗ
                ZPZ_Service(); <----------------------- Должна выполняться в сквозном режиме (вызываться до тех пор пока это необходимо)        
                if(ZPZ_CheckHighPriorityTask() == ZPZ_SC_MODE)
                {			
                  // Здесь можно расположить небольшие задачи          	
                }
              }
					 
    Используемые драйвера и модули:
					 
        1636PP52Y.h   - Драйвер для внешней SPI памяти (загрузка полётного задания и карты высот)
        crc16.h       - Табличный метод расчета контрольной суммы
        BIM.h         - Драйвер работы с БИМами
        Log_FS.h      - Хранилище полетных логов
        SNS.h         - Драйвер работы с СНС
        SWS.h         - Драйвер работы с СВС
        discrete_io.h - Драйвер управления дискретами и реле 
					 
*******************************************************************************************************************/
#ifndef _ZPZ_H_
#define _ZPZ_H_

#include <stdint.h>

// Конфигурация ZPZ модуля (аппаратная привязка)
#define BAUDRATE_ZPZ              115200                 // Бит/с - скорость обмена с ZPZ по UART
#define ZPZ_BYTE_TIMEOUT          1                      // Таймаут на отправку одного байта, мс
#define ZPZ_RECEIVE_TIMEOUT       1                      // Таймаут на приём одного байта, мс
#define ZPZ_CLRBUF_TIMEOUT        1                      // Таймаут на очистку буфера, мс
#define ZPZ_PACKET_WAIT_TIMEOUT   200                    // Время ожидания запроса, мс
#define ZPZ_HPT_TIMEOUT           5000                   // Таймаут на удержание в режиме Высокоприоритетной задачи, мс


// Заголовки - идентификаторы пакетов
typedef enum {
  HANDLER_TO_BUP     = 0x4350,          // Заголовок пакетов от ЗПЗ к БУП (в символах на самом деле эквивалентно CP)
  HANDLER_TO_SNS     = 0x4750,          // Заголовок пакетов от ЗПЗ к СНС (в символах на самом деле эквивалентно GP)
  HANDLER_TO_SWS     = 0x4150,          // Заголовок пакетов от ЗПЗ к СВС (в символах на самом деле эквивалентно WP)
  HANDLER_FROM_BUP   = 0x5542,          // Заголовок пакетов от БУП к ЗПЗ (в символах на самом деле эквивалентно UB)
  HANDLER_FROM_SNS   = 0x4742,          // Заголовок пакетов от SNS к ЗПЗ (в символах на самом деле эквивалентно GB)
  HANDLER_FROM_SWS   = 0x4142,          // Заголовок пакетов от СВС к ЗПЗ (в символах на самом деле эквивалентно WB)
}ZPZ_Handlers;


// Идентификаторы команд
typedef enum {
	START_DOWNLOAD    = 0x42,       // "B" Команда инициирования процедуры загрузки карты
	MAP_DOWNLOAD      = 0x4D,       // "M" Команда загрузки карты
	START_UPLOAD      = 0x62,       // "b" Команда инициирования процедуры выгрузки карты
	MAP_UPLOAD        = 0x6D,       // "m" Команда выгрузки карты
	CHECK_CONNECT     = 0x50,       // "P" Команда проверки связи
	BIM_CONTROL       = 0x45,       // "E" Команда управления БИМами
	BIM_STATUS        = 0x65,       // "e" Команда статуса БИМов
	LOG_FORMAT        = 0x46,       // "F" Команда форматирорования черного ящика
	LOG_FILES         = 0x49,       // "I" Команда запроса информации о файлах
	LOG_UPLOAD        = 0x66,       // "f" Команда выгрузки файла
	REQ_SWS           = 0x49,       // "I" Команда запроса данных с SWS
	SYSTEM_STATE      = 0x73,       // "s" Команда запроса состояния системы
	CAN_TRANSMIT      = 0x43,       // "C" Команда перенаправления посылки в CAN
	REQ_SNS_POS       = 0x4E,       // "N" Команда запроса данных навигации
	REQ_SNS_STATE     = 0x53,       // "S" Команда запроса состояния СНС
	REQ_SNS_SETTINGS  = 0x73,       // "s" Команда настройки/калибровки СНС   
} ZPZ_CMD;



// Индентификаторы статуса для короткого ответа ЗПЗ
typedef enum {
  SUCCES                     = 0x00,    // Ошибок нет												
  FAIL                       = 0xFF,    // Сбой при обмене
  WRONG_CRC                  = 0x33,    // Ошибка в контрольной сумме принятого пакета
  WITHOUT_CLOSING_FEND       = 0x44,    // Входящий пакет не завершился FEND
  UNKNOWN_COMMAND            = 0x13,    // Команда или заголовок не опознаны
  FORMATING_LOG_FS_STARTED   = 0x01,    // Запущено форматирование "черного ящика"
  FORMATING_LOG_FS_COMPLETED = 0x02,    // Форматирование "черного ящика" завершено
  LOG_FORMAT_SUBCMD_ERROR    = 0x4F,    // Подкоманда команды LOG_FORMAT не опознана 
  LOG_FS_IS_CORRUPTED        = 0x3F,    // Файловая система поверждена, чтение данных невозможно
  LOG_FS_FILE_NOT_FIND       = 0x2F,    // Запрашиваемый файл не найден
  LOG_FS_PACKET_NOT_EXIST    = 0x1F,    // Запрашиваемый пакет из файла не существует
  SWS_IS_UNAVAILABLE         = 0xAB,    // Не удаётся получить данные от СВС
  SNS_IS_UNAVAILABLE         = 0xAA,    // Не удаётся получить данные от СНС
  SNS_SETTINGS_WRONG_PAR     = 0xCC,    // В параметрах настройки СНС присутствуют ошибки
  CAN_SENDING_TIMEOUT        = 0xBB,    // Таймаут при отправке в CAN
  CAN_DISABLE_BY_PIN1        = 0xDD,    // CAN передатчик аппаратно отключен шпилькой 1
  CAN_DISABLE_BY_RELAY       = 0xDC,    // CAN передатчик аппаратно отключен реле питания БИМ
  BIM_CTRL_FAILED            = 0x11,    // Отправка команды БИМам не удалась
  BIM_ID_NOT_EXIST           = 0x22,    // БИМ с таким ID не существует
} ZPZ_Short_State;


// Статусы работы модуля ZPZ
typedef enum {
  ZPZ_OK,                 // Все в порядке
  ZPZ_WRONG_CRC,          // Ошибка в контрольной сумме
  ZPZ_TIMEOUT,            // Превышено время ожидания
  ZPZ_UNKNOWN_CMD,        // Принята неизвестная команда
} ZPZ_STATUS_CODE;

/* Режимы работы  */
typedef enum {
  ZPZ_HPT_MODE,           // Идентификатор режима High Priority Task
  ZPZ_SC_MODE,            // Идентификатор режима Single Command
} ZPZ_MODE;
         

// Определения для работы с БИМами
#define L_BIM          0x4C             // Идентификатор левого БИМа
#define R_BIM          0x52             // Идентификатор правого БИМа

// Определения для работы с файловой системой
#define BYTE_FROM_FILE    1024          // Количество байт разбиения файла для выгрузки


/******************************************************************
  Структура основной части всех пакетов
******************************************************************/
typedef __packed struct{
  uint16_t     Handler;               // Заголовок
  uint16_t     PacketSize;            // Размер пакета
  uint8_t      Command;               // Код команды
  uint16_t     Count;                 // Номер пакета
  uint16_t     CRC;                   // Контрольная сумма
}ZPZ_BasePacket_Type;

typedef union {    
  ZPZ_BasePacket_Type   Struct;
  uint8_t               Buffer[9];
}ZPZ_BasePacket_Union;


/******************************************************************
  Структура простого ответа    
******************************************************************/
typedef __packed struct{
  uint16_t     Handler;        // Заголовок
  uint16_t     PacketSize;     // Размер пакета
  uint8_t      Command;        // Код команды
  uint16_t     Count;          // Номер пакета
  uint8_t      Error;          // Статусный байт
  uint16_t     CRC;            // Контрольная сумма
}ZPZ_Response_Type;

typedef union {    
  ZPZ_Response_Type   Struct;
  uint8_t             Buffer[10];
}ZPZ_Response_Union;
/*****************************************************************/
	


/******************************************************************
  Структура информационного кадра запроса на управление БИМами
******************************************************************/
typedef __packed struct{
  uint8_t     Side;              // Какой из БИМОВ
  uint8_t     State;             // Вкл(0x01)/Выкл(0x00)
  uint8_t     Position;          // Положение стропы
}ZPZ_RequestControlBIM_Type;

typedef union {    
  ZPZ_RequestControlBIM_Type   Struct;
  uint8_t                      Buffer[3];
}ZPZ_RequestControlBIM_Union;
/*****************************************************************/


/******************************************************************
  Структура информационного кадра запроса на управление БИМами
******************************************************************/
typedef __packed struct{
  uint8_t     Side;              // Какой из БИМОВ
  uint16_t    StatusFlags;       // Статус-флаги БИМ (см. протокол работы с БИМ)
  uint8_t     Position;          // Положение двигателя
  uint8_t     Voltage;           // Напряжение двигателя
  int8_t      Current;           // Ток двигателя
  int8_t      Speed;             // Скорость двигателя
}ZPZ_ResponseStatusBIM_Type;

typedef union {    
  ZPZ_ResponseStatusBIM_Type   Struct;
  uint8_t                      Buffer[7];
}ZPZ_ResponseStatusBIM_Union;
/*****************************************************************/

	
/**************************************************************************************************************
  ZPZ_init - Инициализация UART под ZPZ
  Параметры:  NONE
***************************************************************************************************************/
void ZPZ_initialize (void);

/**************************************************************************************************************
  ZPZ_deinit - Деинициализация UART под ZPZ
  Параметры:  NONE
***************************************************************************************************************/
void ZPZ_deinitialize (void);

/**************************************************************************************************************
  ZPZ_ShortResponse - Отправка простого пакета подтвержений/ошибки к ЗПЗ
  Параметры:
            Command - Идентификатор команды
            Count   - Номер сообщения
            Error   - Статус (SUCCES или FAIL)
**************************************************************************************************************/
void ZPZ_ShortResponse(uint8_t Command, uint16_t Count, uint8_t Error);



/**************************************************************************************************************
  ZPZ_writeFlash - Функция записи производит объединение 4х микросхем SPI-памяти в единое
                   адресное пространство 0x00000 - 0x7FFFF, используя драйвер "1636PP52Y.h".
                   Позволяет абстрагироваться от периферийного уровня и управления CSn.	
  Параметры: 
            Address     - Адрес места записи;
            Destination - Указатель на буфер источника; 
            Size        - Размер записываемой области памяти в байтах;
  Возвращает:
            NONE
**************************************************************************************************************/
void ZPZ_writeFlash(uint32_t Address, uint8_t* Source, uint32_t Size);


/**************************************************************************************************************
  ZPZ_readFlash - Функция чтения производит объединение 4х микросхем SPI-памяти в единое
                  адресное пространство 0x00000 - 0x7FFFF, используя драйвер "1636PP52Y.h".
                  Позволяет абстрагироваться от периферийного уровня и управления CSn.
  Параметры:
            Address     - Адрес чтения;
            Destination - Указатель на буфер места назначения;
            Size        - Размер читаемой области памяти в байтах;
  Возвращает:
            NONE											 
**************************************************************************************************************/
void ZPZ_readFlash(uint32_t Address, uint8_t* Destination, uint32_t Size);


/**************************************************************************************************************
  ZPZ_eraseFlash - Форматирование носителя с полетным заданием и картой высот.	
**************************************************************************************************************/
void ZPZ_eraseFlash(void);


/**************************************************************************************************************
  ZPZ_service - Функция обслуживания команд от ЗПЗ (Основной режим)
	
  Примечание: Пока запущена - устройство работает в режиме ЗПЗ
**************************************************************************************************************/
uint8_t ZPZ_service (void);


/**************************************************************************************************************
  ZPZ_getStatus - Проверка занятости модуля высокоприоритетной задачей
  Возвращает: 
            ZPZ_HPT - Если модуль находится в режиме высокоприоритетной задачи
            ZPZ_SC  - Если модуль находится в режиме разовых команд
**************************************************************************************************************/
uint8_t ZPZ_getStatus (void);


#endif
