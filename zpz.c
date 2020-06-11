#include "zpz.h"

#include "MDR32F9Qx_uart.h"
#include "MDR32F9Qx_can.h"
#include "1636PP52Y.h"
#include "crc16.h"
#include "bims.h"
#include "logfs/log.fs.h"
#include "kmonshelf.h"
#include "sws.h"
#include "discreteio.h"
#include "otherlib.h"
#include "selftesting.h"   
#include "bupdatastorage.h"
#include "heightMap/mapflashlayout.h"
#include "string.h"


#define BUFFER_SIZE 800
static uint8_t buffer [BUFFER_SIZE];

/****************************************************************** 
    Определения и макросы для реализации SLIP протокола
******************************************************************/
typedef enum {
  FEND     = 0xC0,
  FESC     = 0xDB,
  TFEND    = 0xDC,
  TFESC    = 0xDD,
}SLIP_DIV;

/****************************************************************** 
  Флаг занятости модуля высокоприоритетной задачей
******************************************************************/
ZPZ_MODE HighPriorityTask = ZPZ_SC_MODE; /* По-умолчанию - Режим разовой команды */


/******************************************************************************************************

  Объявление приватных функций обслуживания команд от ЗПЗ

*******************************************************************************************************/

/*--------------------------------------------------------------------------------------------Обслуживание простых запросов-----------*/
static uint16_t ZPZ_RequestWithEmptyData(uint16_t CRC); 
/*--------------------------------------------------------------------------------------------Обслуживание команды CHECK_CONNECT-------*/
static uint16_t ZPZ_Request_CHECK_CONNECT(uint16_t CRC);
static void     ZPZ_Response_CHECK_CONNECT(uint16_t NumPacket);
/*--------------------------------------------------------------------------------------------Обслуживание команды START_DOWNLOAD------*/
static uint16_t ZPZ_Request_START_DOWNLOAD(uint16_t CRC);
static void     ZPZ_Response_START_DOWNLOAD(uint16_t NumPacket);
/*--------------------------------------------------------------------------------------------Обслуживание команды MAP_DOWNLOAD--------*/
static uint16_t ZPZ_Request_MAP_DOWNLOAD(uint16_t CRC);
static void     ZPZ_Response_MAP_DOWNLOAD(uint16_t NumPacket);
/*--------------------------------------------------------------------------------------------Обслуживание команды START_UPLOAD--------*/
static uint16_t ZPZ_Request_START_UPLOAD(uint16_t CRC);
static void     ZPZ_Response_START_UPLOAD(uint16_t NumPacket); 
/*--------------------------------------------------------------------------------------------Обслуживание команды MAP_UPLOAD----------*/
static uint16_t ZPZ_Request_MAP_UPLOAD(uint16_t CRC);
static void     ZPZ_Response_MAP_UPLOAD(uint16_t NumPacket);
/*--------------------------------------------------------------------------------------------Обслуживание команды BIM_CONTROL---------*/
static uint16_t ZPZ_Request_BIM_CONTROL(uint16_t CRC);
static void     ZPZ_Response_BIM_CONTROL(uint16_t NumPacket);
/*--------------------------------------------------------------------------------------------Обслуживание команды BIM_STATUS----------*/
static uint16_t ZPZ_Request_BIM_STATUS(uint16_t CRC);
static void     ZPZ_Response_BIM_STATUS(uint16_t NumPacket);
/*--------------------------------------------------------------------------------------------Обслуживание команды LOG_FORMAT----------*/
static uint16_t ZPZ_Request_LOG_FORMAT(uint16_t CRC);
static void     ZPZ_Response_LOG_FORMAT(uint16_t NumPacket);
/*--------------------------------------------------------------------------------------------Обслуживание команды LOG_FILES-----------*/
static uint16_t ZPZ_Request_LOG_FILES(uint16_t CRC);
static void     ZPZ_Response_LOG_FILES(uint16_t NumPacket);
/*--------------------------------------------------------------------------------------------Обслуживание команды LOG_UPLOAD----------*/
static uint16_t ZPZ_Request_LOG_UPLOAD(uint16_t CRC);
static void     ZPZ_Response_LOG_UPLOAD(uint16_t File_num);
/*--------------------------------------------------------------------------------------------Обслуживание команды REQ_SWS-------------*/
static uint16_t ZPZ_Request_REQ_SWS(uint16_t CRC);
static void     ZPZ_Response_REQ_SWS(uint16_t NumPacket);
/*--------------------------------------------------------------------------------------------Обслуживание команды REQ_SNS_POS---------*/
static uint16_t ZPZ_Request_REQ_SNS_POS(uint16_t CRC);
static void     ZPZ_Response_REQ_SNS_POS(uint16_t NumPacket);
/*--------------------------------------------------------------------------------------------Обслуживание команды REQ_SNS_STATE-------*/
static uint16_t ZPZ_Request_REQ_SNS_STATE (uint16_t CRC);
static void     ZPZ_Response_REQ_SNS_STATE (uint16_t NumPacket);
/*--------------------------------------------------------------------------------------------Обслуживание команды REQ_SNS_SETTINGS----*/
static uint16_t ZPZ_Request_REQ_SNS_SETTINGS(uint16_t CRC);
static void     ZPZ_Response_REQ_SNS_SETTINGS(uint16_t NumPacket);
/*--------------------------------------------------------------------------------------------Обслуживание команды SYSTEM_STATE--------*/
static uint16_t ZPZ_Request_SYSTEM_STATE(uint16_t CRC);
static void     ZPZ_Response_SYSTEM_STATE(uint16_t NumPacket);
/*--------------------------------------------------------------------------------------------Обслуживание команды CAN_TRANSMIT--------*/
static uint16_t ZPZ_Request_CAN_TRANSMIT(uint16_t CRC);
static void     ZPZ_Response_CAN_TRANSMIT(uint16_t NumPacket);
/*--------------------------------------------------------------------------------------------Обёртка UART функций со SLIP-------------*/
static uint16_t UARTReceiveByte_by_SLIP(MDR_UART_TypeDef* UARTx);
static int16_t  UARTSendByte_by_SLIP (MDR_UART_TypeDef* UARTx, uint16_t Byte);
static int16_t  SendFEND (MDR_UART_TypeDef* UARTx);
/*--------------------------------------------------------------------------------------------Режим "ВВПЗ"-----------------------------*/
static void     ZPZ_startHighPriorityTask (void);
static void     ZPZ_finishHighPriorityTask (void);
/*-------------------------------------------------------------------------------------------------------------------------------------*/

/******************************************************************************************************

  Объявление публичных функций модуля ЗПЗ

******************************************************************************************************/

/**************************************************************************************************************
  ZPZ_RetargetPins - Функция переопределения UART2 на другие пины, для работы по ZPZ
**************************************************************************************************************/
static void ZPZ_RetargetPins (void)
{
  // Переназчаем UART на нужный порт для работы ZPZ
  Pin_initialize (ZPZ_RX);
  Pin_initialize (ZPZ_TX);
}

/***************************************************************************************************************
  ZPZ_initialize - Запуск процедуры обмена по ZPZ
***************************************************************************************************************/
void ZPZ_initialize (void)
{
  UART_InitTypeDef UART_InitStructure;
	
  // Конфигурируем пины
  ZPZ_RetargetPins();
  // Сброс конфигуряции UART
  UART_DeInit(ZPZ_UART);
  // Заполнение структуры по-умолчанию
  UART_StructInit (&UART_InitStructure);
  // Установка предделителя блока UART1_CLK = HCLK
  UART_BRGInit (ZPZ_UART,UART_HCLKdiv1); 
  // Заполняем структуру инициализации	
  UART_InitStructure.UART_BaudRate                = BAUDRATE_ZPZ;
  UART_InitStructure.UART_WordLength              = UART_WordLength8b;
  UART_InitStructure.UART_StopBits                = UART_StopBits1;
  UART_InitStructure.UART_Parity                  = UART_Parity_No;
  UART_InitStructure.UART_FIFOMode                = UART_FIFO_ON;
  UART_InitStructure.UART_HardwareFlowControl     = UART_HardwareFlowControl_RXE | UART_HardwareFlowControl_TXE;
  // Инициализация UART
  UART_Init (ZPZ_UART,&UART_InitStructure);
  // Разрешение прерываний по приёму
  //UART_ITConfig(ZPZ_UART,UART_IT_RX,ENABLE);
  // Включение UART1 - CНС
  UART_Cmd(ZPZ_UART,ENABLE);
  // Сбрасываем флаги прерываний по приёму и передаче
  //UART_ClearITPendingBit(MDR_UART2,UART_IT_RX|UART_IT_TX);
  // Глобальное разрешение прерываний от ZPZ
  //NVIC_EnableIRQ(ZPZ_IRQn);
}


/***************************************************************************************************************
  ZPZ_deinitialize - Деинициализация ЗПЗ
***************************************************************************************************************/
void ZPZ_deinitialize (void)
{
  // Сброс конфигуряции UART
  UART_DeInit(ZPZ_UART);
  // Отключение UART
  UART_Cmd(ZPZ_UART, DISABLE);
  Pin_default (ZPZ_RX);
  Pin_default (ZPZ_TX);
}


/**************************************************************************************************************
  ZPZ_ShortResponse - Отправка простого пакета подтвержений/ошибки к ЗПЗ
**************************************************************************************************************/
void ZPZ_ShortResponse(uint8_t Command, uint16_t Count, uint8_t Error)
{
  ZPZ_Response_Union   ZPZ_Response;
  TimeoutType          timeout;
	
  //Заполняем сткруктуру ответа
  ZPZ_Response.Struct.Handler     = HANDLER_FROM_BUP;   // "UB" - Должно быть "BU", но выдача идём младшим байтом вперед
  ZPZ_Response.Struct.PacketSize  = 4;                  // В размер входят только команда, счетчик, и ошибка
  ZPZ_Response.Struct.Command     = Command;
  ZPZ_Response.Struct.Count       = Count;
  ZPZ_Response.Struct.Error       = Error;
  ZPZ_Response.Struct.CRC         = Crc16(&ZPZ_Response.Buffer[0],8, CRC16_INITIAL_FFFF);
	
  // Если FIFO передатчика заполнен, то ждем пока освободится
  setTimeout (&timeout, ZPZ_CLRBUF_TIMEOUT);
  while ((UART_GetFlagStatus (ZPZ_UART, UART_FLAG_TXFF) == SET) && (timeoutStatus(&timeout) != TIME_IS_UP));
	
  // Посылаем признак начала пакета
  SendFEND(ZPZ_UART);
	
  // Теперь остальную часть пакета
  for(uint8_t i = 0; i < 10; i++)
  {
    UARTSendByte_by_SLIP (ZPZ_UART, ZPZ_Response.Buffer[i]);
  }
  // Ждем завершения передачи
  setTimeout (&timeout, ZPZ_CLRBUF_TIMEOUT);
  while ((UART_GetFlagStatus (ZPZ_UART, UART_FLAG_TXFF) == SET) && (timeoutStatus(&timeout) != TIME_IS_UP));
  
  // Посылаем признак конца пакета
  SendFEND(ZPZ_UART);
}



/**************************************************************************************************************
  ZPZ_writeFlash - Функция записи производит объединение 4х микросхем SPI-памяти в единое 
                   адресное пространство 0x00000 - 0x7FFFF, используя драйвер "1636PP52Y.h".
                   Позволяет абстрагироваться от периферийного уровня и управления CSn.			
**************************************************************************************************************/
void ZPZ_writeFlash(uint32_t Address, uint8_t* Source, uint32_t Size)
{	
  uint32_t  Bytecount = 0;

  while(Bytecount < Size)
  {
    if(((Address + Bytecount) < 0x20000))
      SPI_1636PP52Y_writeByte (SPI_1636PP52Y_CS1, Address + Bytecount, *Source++);
			
    else if (((Address + Bytecount) >= 0x20000)  &&  ((Address + Bytecount) < 0x40000))
      SPI_1636PP52Y_writeByte (SPI_1636PP52Y_CS2, (Address - 0x20000 + Bytecount), *Source++);
				
    else if (((Address + Bytecount) >= 0x40000)  &&  ((Address + Bytecount) < 0x60000))
      SPI_1636PP52Y_writeByte (SPI_1636PP52Y_CS3, (Address - 0x40000 + Bytecount), *Source++);
			
    else if (((Address + Bytecount) >= 0x60000)  &&  ((Address + Bytecount) < 0x80000))
      SPI_1636PP52Y_writeByte (SPI_1636PP52Y_CS4, (Address - 0x60000 + Bytecount), *Source++);

    Bytecount ++;
  }
}



/**************************************************************************************************************
  ZPZ_readIntoCSnUnion - Функция чтения производит объединение 4х микросхем SPI-памяти в единое
                         адресное пространство 0x00000 - 0x7FFFF, используя драйвер "1636PP52Y.h".
                         Позволяет абстрагироваться от периферийного уровня и управления CSn.			
**************************************************************************************************************/
void ZPZ_readFlash(uint32_t Address, uint8_t* Destination, uint32_t Size)
{	
  uint32_t  Bytecount = 0;
	
  while(Bytecount < Size)
  {
    if(((Address + Bytecount) < 0x20000))
      SPI_1636PP52Y_readArray (SPI_1636PP52Y_CS1, Address + Bytecount, Destination++, 1);
			
    else if (((Address + Bytecount) >= 0x20000)  &&  ((Address + Bytecount) < 0x40000))
      SPI_1636PP52Y_readArray (SPI_1636PP52Y_CS2, Address + Bytecount, Destination++, 1);
				
    else if (((Address + Bytecount) >= 0x40000)  &&  ((Address + Bytecount) < 0x60000))
      SPI_1636PP52Y_readArray (SPI_1636PP52Y_CS3, Address + Bytecount, Destination++, 1);
			
    else if (((Address + Bytecount) >= 0x60000)  &&  ((Address + Bytecount) < 0x80000))
      SPI_1636PP52Y_readArray (SPI_1636PP52Y_CS4, Address + Bytecount, Destination++, 1);

    Bytecount ++;
  }
}



/**************************************************************************************************************
  ZPZ_eraseFlash - Функция стирания всех установленых микросхем памяти, используется
                          драйвер "1636PP52Y.h".		
**************************************************************************************************************/
void ZPZ_eraseFlash(void)
{	
  SPI_1636PP52Y_eraseChip (SPI_1636PP52Y_CS1);
  SPI_1636PP52Y_eraseChip (SPI_1636PP52Y_CS2);
  SPI_1636PP52Y_eraseChip (SPI_1636PP52Y_CS3);
  SPI_1636PP52Y_eraseChip (SPI_1636PP52Y_CS4);
}	



/**************************************************************************************************************
  ZPZ_service - Функция обслуживания команд от ЗПЗ (Основной режим)
**************************************************************************************************************/
uint8_t ZPZ_service (void)
{
  ZPZ_BasePacket_Union  ZPZ_Base_Request;         /* Сюда складывается приходящий запрос (кроме полей "данные") */
  TimeoutType           timeout;                  /* Контроль превышения времени обработки */
  uint16_t              crc;                      /* Контрольная сумма */
  static TimeoutType    timeoutHighPriorityTask;  /* Таймаут на удержание в режиме ВПЗ */
	

//  while (UART_GetFlagStatus (ZPZ_UART, UART_FLAG_RXFE) != SET)     /* Вычищаем FIFO от мусора и ждем пока не появится заголовок */
//    UART_ReceiveData(ZPZ_UART);	
	
  if(timeoutStatus(&timeoutHighPriorityTask) == TIME_IS_UP)         /* Здесь проверяем нужно ли вернуть модуль в режим одиночной команды */
    ZPZ_finishHighPriorityTask();
		
  setTimeout (&timeout, ZPZ_PACKET_WAIT_TIMEOUT);                   /* Ожидаем заголовок в течение заданного времени */
  while(timeoutStatus(&timeout) != TIME_IS_UP)
  {
    if (UARTReceiveByte_by_SLIP(ZPZ_UART) != FEND) continue;        /* Сначала ловим FEND по SLIP протоколу. Если первый байт заголовка не совпал, */
    ZPZ_Base_Request.Buffer[0] = UARTReceiveByte_by_SLIP(ZPZ_UART); /* дальше не ждем, переходим к следующей итерации. Если второй символ тоже совпал, */
    ZPZ_Base_Request.Buffer[1] = UARTReceiveByte_by_SLIP(ZPZ_UART); /* значит покидаем данный цикл и переходим к приёму остального пакета */
    if(ZPZ_Base_Request.Struct.Handler != HANDLER_TO_BUP &&         /* Проверим принадлежит ли handler описанному перечню,*/
       ZPZ_Base_Request.Struct.Handler != HANDLER_TO_SNS &&         /* Иначе в буфер может упасть мусор */
       ZPZ_Base_Request.Struct.Handler != HANDLER_TO_SWS) continue;
    break;
  }	
  if(timeout.status == TIME_IS_UP)                                  /* Если был таймаут, значит связи нет, можно отключиться */
    return ZPZ_TIMEOUT;
		
  /* Если оказались тут, значит удалось поймать начало пакета и не поймать таймаут */	
  for(uint16_t i = 2; i < 7; i++)
    ZPZ_Base_Request.Buffer[i] = UARTReceiveByte_by_SLIP(ZPZ_UART); /* Принимаем остальное */
 
  crc = Crc16(&ZPZ_Base_Request.Buffer[0], 7, CRC16_INITIAL_FFFF);  /* Считаем контрольную сумму начала сообщения */
  
	
  /* Далее программа ветвиться в зависимости от типа заголовка и команды, 
  информационная часть пакета будет обрабатываться разными подфункциями. */
	
  /* Сценарии работы при получении всех возможных заголовков и команд */
  switch (ZPZ_Base_Request.Struct.Handler)
  {
    case HANDLER_TO_BUP: /* Заголовок запроса к БУПу */
    {
      switch (ZPZ_Base_Request.Struct.Command)
      {
        case START_DOWNLOAD: /* Начало загрузки полетного задания */
        {
          ZPZ_startHighPriorityTask ();                           /* Процесс ресурсоемкий, будем выполнять его в режиме "ВВПЗ" */
          setTimeout (&timeoutHighPriorityTask, ZPZ_HPT_TIMEOUT); /* Обновляем таймаут контроль */
          crc = ZPZ_Request_START_DOWNLOAD (crc);                 /* Принимаем данные запроса */
          break;
        }
        case MAP_DOWNLOAD: /* Продолжение загрузки полетного задания */
        {
          setTimeout (&timeoutHighPriorityTask, ZPZ_HPT_TIMEOUT); /* Обновляем таймаут контроль */
          crc = ZPZ_Request_MAP_DOWNLOAD (crc);                   /* Принимаем данные запроса */
          break;
        }
        case START_UPLOAD: /* Начало выгрузки полетного задания */
        {
          ZPZ_startHighPriorityTask ();                           /* Процесс ресурсоемкий, будем выполнять его в режиме "ВВПЗ" */
          setTimeout (&timeoutHighPriorityTask, ZPZ_HPT_TIMEOUT); /* Обновляем таймаут контроль */
          crc = ZPZ_Request_START_UPLOAD (crc);                   /* Принимаем данные запроса */
          break;
        }
        case MAP_UPLOAD: /* Продолжение выгрузки полетного задания */
        {
          setTimeout (&timeoutHighPriorityTask, ZPZ_HPT_TIMEOUT);  /* Обновляем таймаут контроль */
          crc = ZPZ_Request_MAP_UPLOAD (crc);                      /* Принимаем данные запроса */
          break;
        }
        case CHECK_CONNECT: /* Проверка связи по протоколу */
        {
          crc = ZPZ_Request_CHECK_CONNECT (crc);
          break;
        }
        case BIM_CONTROL: /* Управление БИМ'ами */
        {
          crc = ZPZ_Request_BIM_CONTROL (crc);
          break;
        }
        case BIM_STATUS: /* Запрос информации о состоянии БИМ'ов */
        {
          crc = ZPZ_Request_BIM_STATUS (crc);	
          break;
        }
        case LOG_FORMAT: /* Команда форматирования черного ящика */
        {
          crc = ZPZ_Request_LOG_FORMAT(crc);
          break;
        }
        case LOG_FILES: /* Запрос информации о содержании черного ящика */
        {
          crc = ZPZ_Request_LOG_FILES (crc);
          break;
        }
        case LOG_UPLOAD: /* Выгрузка файла лога */
        {
          if(ZPZ_Base_Request.Struct.Count == 0)                  /* Процесс ресурсоемкий, будем выполнять его в режиме "ВВПЗ" */
            ZPZ_startHighPriorityTask ();                         /* Определять начало процесса выгрузки логов будем по 0-му номеру пакета */
          setTimeout (&timeoutHighPriorityTask, ZPZ_HPT_TIMEOUT); /* Обновляем таймаут контроль */
          crc = ZPZ_Request_LOG_UPLOAD(crc);                      /* Принимаем данные запроса */
          break;
        }
        case SYSTEM_STATE: /* Запрос состояния системы */
        {
          crc = ZPZ_Request_SYSTEM_STATE (crc);
          break;
        }
        case CAN_TRANSMIT: /* Запрос передачи сообщения в CAN */
        {
          crc = ZPZ_Request_CAN_TRANSMIT (crc);
          break;
        }
        default: /* Команда запроса неизвестна */
        {
          ZPZ_ShortResponse(ZPZ_Base_Request.Struct.Command,
                            ZPZ_Base_Request.Struct.Count, 
                            UNKNOWN_COMMAND);   
          return ZPZ_UNKNOWN_CMD; 
        }
      }
      break;
		}
    case HANDLER_TO_SNS: /* Заголовок запроса к СНС */
    {
      switch (ZPZ_Base_Request.Struct.Command)
      {
        case REQ_SNS_POS: /* Запрос навигационной информации от СНС */
        {
          crc = ZPZ_Request_REQ_SNS_POS(crc);
          break;
        }
        case REQ_SNS_STATE: /* Запрос состояние устройства СНС */
        {
          crc = ZPZ_Request_REQ_SNS_STATE(crc);
          break;
        }
        case REQ_SNS_SETTINGS: /* Запрос на изменение настроек СНС  */
        {
          crc = ZPZ_Request_REQ_SNS_SETTINGS(crc);
          break;
        }
        default: /* Команда запроса неизвестна */
        {
          ZPZ_ShortResponse(ZPZ_Base_Request.Struct.Command,
                            ZPZ_Base_Request.Struct.Count, 
                            UNKNOWN_COMMAND);   
          return ZPZ_UNKNOWN_CMD; 
        }
      }
      break;
    }
    case HANDLER_TO_SWS: /* Заголовок запроса к СВС */
    {
      switch (ZPZ_Base_Request.Struct.Command)
      {
        case REQ_SWS: /* Запрос информации от СВС */
        {
          crc = ZPZ_Request_REQ_SWS(crc);
          break;
        }
        default: /* Команда запроса неизвестна */
        {
          ZPZ_ShortResponse(ZPZ_Base_Request.Struct.Command,
                            ZPZ_Base_Request.Struct.Count, 
                            UNKNOWN_COMMAND);   
          return ZPZ_UNKNOWN_CMD; 
        }
      }
      break;
    }
  }
 
	
  /* Все функции возвращаются сюда, 
  здесь принимаем контрольную сумму
  и проверяем валидность пакета */
  for(uint16_t i = 7; i < 9; i++)
  {
    ZPZ_Base_Request.Buffer[i] = UARTReceiveByte_by_SLIP(ZPZ_UART);                    /* Принимаем контрольную сумму сообщения */
  }
  if(crc != ZPZ_Base_Request.Struct.CRC)                                               /* Сверяем контрольные суммы */
  {
    ZPZ_ShortResponse(ZPZ_Base_Request.Struct.Command,                                 /* Если не сошлись - ошибка */
                      ZPZ_Base_Request.Struct.Count, 
                      WRONG_CRC);   
    return ZPZ_WRONG_CRC;                                                              /* Отправляем ответ об ошибке */
  }
  if(crc != ZPZ_Base_Request.Struct.CRC || UARTReceiveByte_by_SLIP(ZPZ_UART) != FEND)  /* Проверяем, что пакет завершился признаком FEND */
  {
    ZPZ_ShortResponse(ZPZ_Base_Request.Struct.Command,                                 /* Если не завершился FEND, */
		                  ZPZ_Base_Request.Struct.Count, 
											WITHOUT_CLOSING_FEND);
    return ZPZ_WRONG_CRC;                                                              /* Отправляем отчет с ошибкой */
  }
	
	
  /* Если оказались здесь - пакет валидный
  нужно ответить нужным типом на запрос 
  соответствующим образом	*/
  switch (ZPZ_Base_Request.Struct.Handler)
  {
    case HANDLER_TO_BUP: /* Заголовок запроса к БУПу */
    {
      switch (ZPZ_Base_Request.Struct.Command)
      {
        case START_DOWNLOAD: /* Начало загрузки полетного задания */
        {
          ZPZ_Response_START_DOWNLOAD (ZPZ_Base_Request.Struct.Count);
          break;
        }
        case MAP_DOWNLOAD: /* Продолжение загрузки полетного задания */
        {
          ZPZ_Response_MAP_DOWNLOAD  (ZPZ_Base_Request.Struct.Count);   /* Формируем ответ */
          if(ZPZ_Base_Request.Struct.Count == NUMBER_OF_MAP_PACKET)     /* Если это конец загрузки карты и задания (определяем как 400й пакет) */
            ZPZ_finishHighPriorityTask ();                              /* Завершаем режим "ВВПЗ" */
          break;
        }
        case START_UPLOAD: /* Начало выгрузки полетного задания */
        {
          ZPZ_Response_START_UPLOAD (ZPZ_Base_Request.Struct.Count);
          break;
        }
        case MAP_UPLOAD: /* Продолжение выгрузки полетного задания */
        {
          ZPZ_Response_MAP_UPLOAD (ZPZ_Base_Request.Struct.Count);      /* Формируем ответ */
          if(ZPZ_Base_Request.Struct.Count == NUMBER_OF_MAP_PACKET)     /* Если это конец выгрузки карты и задания (определяем как 400й пакет) */
            ZPZ_finishHighPriorityTask ();                              /* Завершаем режим "ВВПЗ" */
          break;
        }
        case CHECK_CONNECT: /* Проверка связи по протоколу */
        {
          ZPZ_Response_CHECK_CONNECT (ZPZ_Base_Request.Struct.Count);
          break;
        }
        case BIM_CONTROL: /* Управление БИМ'ами */
        {
          ZPZ_Response_BIM_CONTROL (ZPZ_Base_Request.Struct.Count);
          break;
        }
        case BIM_STATUS: /* Запрос информации о состоянии БИМ'ов */
        {
          ZPZ_Response_BIM_STATUS(ZPZ_Base_Request.Struct.Count);
          break;
        }
        case LOG_FORMAT: /* Команда форматирования черного ящика */
        {
          ZPZ_Response_LOG_FORMAT (ZPZ_Base_Request.Struct.Count);
          break;
        }
        case LOG_FILES: /* Запрос информации о содержании черного ящика */
        {
          ZPZ_Response_LOG_FILES (ZPZ_Base_Request.Struct.Count);
          break;
        }
        case LOG_UPLOAD: /* Выгрузка файла лога */
        {
          ZPZ_Response_LOG_UPLOAD(ZPZ_Base_Request.Struct.Count);
          break;
        }
        case SYSTEM_STATE:  /* Запрос состояния системы */
        {
          ZPZ_Response_SYSTEM_STATE (ZPZ_Base_Request.Struct.Count);
          break;
        }
        case CAN_TRANSMIT: /* Запрос передачи сообщения в CAN */
        {
          ZPZ_Response_CAN_TRANSMIT (ZPZ_Base_Request.Struct.Count);
          break;
        }
        default:
          break;
      }
      break;
		}
    case HANDLER_TO_SNS: /* Заголовок запроса к СНС */
    {
      switch (ZPZ_Base_Request.Struct.Command)
      {
        case REQ_SNS_POS: /* Запрос навигационной информации от СНС */
        {
          ZPZ_Response_REQ_SNS_POS(ZPZ_Base_Request.Struct.Count);
          break;
        }
        case REQ_SNS_STATE: /* Запрос состояние устройства СНС */
        {
          ZPZ_Response_REQ_SNS_STATE(ZPZ_Base_Request.Struct.Count);
          break;
        }
        case REQ_SNS_SETTINGS: /* Запрос на изменение настроек СНС  */
        {
          ZPZ_Response_REQ_SNS_SETTINGS (ZPZ_Base_Request.Struct.Count);
          break;
        }
      }
      break;
    }
    case HANDLER_TO_SWS: /* Заголовок запроса к СВС */
    {
      switch (ZPZ_Base_Request.Struct.Command)
      {
        case REQ_SWS: /* Запрос информации от СВС */
        {
          ZPZ_Response_REQ_SWS(ZPZ_Base_Request.Struct.Count);
          break;
        }
      }
      break;
    }
  }

  return ZPZ_OK;
}



/*--------------------------------------------------------------------------------------------Обёртка UART функций со SLIP-------------*/
// Отправка FEND разделителя, с таймаутом на время отправки 
static int16_t SendFEND(MDR_UART_TypeDef* UARTx)
{
	TimeoutType timeout;
	
	// Если FIFO передатчика заполнен, то ждем пока освободится
	setTimeout (&timeout, ZPZ_CLRBUF_TIMEOUT);
	while ((UART_GetFlagStatus (UARTx, UART_FLAG_TXFF) == SET) && (timeoutStatus(&timeout) != TIME_IS_UP));
	UART_SendData(ZPZ_UART, FEND);
	// Если выход из ожидания по таймауту - возвращаем ошибку и выходим
	if(timeout.status == TIME_IS_UP)
		return 0xFF;
	else 
		return 0;
}

// Отправка символа по SLIP - протоколу
static int16_t UARTSendByte_by_SLIP (MDR_UART_TypeDef* UARTx, uint16_t Byte)
{
	TimeoutType timeout;
	
	if(Byte == FEND || Byte == FESC)
	{
		// Если FIFO передатчика заполнен, то ждем пока освободится
		setTimeout (&timeout, ZPZ_CLRBUF_TIMEOUT);
		while ((UART_GetFlagStatus (UARTx, UART_FLAG_TXFF) == SET) && (timeoutStatus(&timeout) != TIME_IS_UP));
		// Если выход из ожидания по таймауту - возвращаем ошибку и выходим
		if(timeout.status == TIME_IS_UP)
		  return 0xFF;
		// Иначе отправляем вместо FEND или FESC сначала FESC
		UART_SendData(UARTx, FESC);
		// И ждем пока FIFO освободится
		while ((UART_GetFlagStatus (UARTx, UART_FLAG_TXFF) == SET) && (timeoutStatus(&timeout) != TIME_IS_UP));
		
		// Если нужно отправить FEND, то после FESC посылаем TFEND
		if(Byte == FEND)
		{
			// Если FIFO передатчика заполнен, то ждем пока освободится
			while ((UART_GetFlagStatus (UARTx, UART_FLAG_TXFF) == SET) && (timeoutStatus(&timeout) != TIME_IS_UP));
			// Если выход из ожидания по таймауту - возвращаем ошибку и выходим
			if(timeout.status == TIME_IS_UP)   
				return 0xFF;
			// Иначе отправляем вместо FEND или FESC сначала FESC
			UART_SendData(UARTx, TFEND);
			// И ждем пока FIFO освободится
		  while ((UART_GetFlagStatus (UARTx, UART_FLAG_TXFF) == SET) && (timeoutStatus(&timeout) != TIME_IS_UP));
		}
		// Если нужно отправить FESC, то после FESC посылаем еще и TFESC
		else if (Byte == FESC)
		{
			// Если FIFO передатчика заполнен, то ждем пока освободится
			while ((UART_GetFlagStatus (UARTx, UART_FLAG_TXFF) == SET) && (timeoutStatus(&timeout) != TIME_IS_UP));
			// Если выход из ожидания по таймауту - возвращаем ошибку и выходим
			if(timeout.status == TIME_IS_UP)   
				return 0xFF;
			// Иначе отправляем вместо FEND или FESC сначала FESC
			UART_SendData(UARTx, TFESC);
			// И ждем пока FIFO освободится
		  while ((UART_GetFlagStatus (UARTx, UART_FLAG_TXFF) == SET) && (timeoutStatus(&timeout) != TIME_IS_UP));
		}
	}
	else 
	{
		// Если FIFO передатчика заполнен, то ждем пока освободится
		setTimeout (&timeout, ZPZ_CLRBUF_TIMEOUT);
		while ((UART_GetFlagStatus (UARTx, UART_FLAG_TXFF) == SET) && (timeoutStatus(&timeout) != TIME_IS_UP));
		// Если выход из ожидания по таймауту - возвращаем ошибку и выходим
		if(timeout.status == TIME_IS_UP)  
			return 0xFF;
		// Иначе отправляем исходный символ
		UART_SendData(UARTx, Byte);
		// И ждем пока FIFO освободится
		while ((UART_GetFlagStatus (UARTx, UART_FLAG_TXFF) == SET) && (timeoutStatus(&timeout) != TIME_IS_UP));
	}
		
		return 0;
}

// Приём и декодирование пакета по SLIP Протоколу 
static uint16_t UARTReceiveByte_by_SLIP(MDR_UART_TypeDef* UARTx)
{
	uint16_t    Byte = 0;
	TimeoutType timeout; 
	
	// Ждем прихода информации
	setTimeout (&timeout, ZPZ_RECEIVE_TIMEOUT);
	while ((UART_GetFlagStatus (UARTx, UART_FLAG_RXFE) == SET) && (timeoutStatus(&timeout) != TIME_IS_UP));
	// Если выход из ожидания по таймауту - возвращаем ошибку и выходим
	if(timeout.status == TIME_IS_UP) 
		return 0xFF;
	// Иначе принимаем байт
	Byte = UART_ReceiveData(UARTx);
	
	// Если поймали символ FESC, то смотрим каким будет следующий
	if(Byte == FESC)
	{
		// Ждем прихода информации
		setTimeout (&timeout, ZPZ_BYTE_TIMEOUT);
		while ((UART_GetFlagStatus (UARTx, UART_FLAG_RXFE) == SET) && (timeoutStatus(&timeout) != TIME_IS_UP));
		// Если выход из ожидания по таймауту - возвращаем ошибку и выходим
		if(timeout.status == TIME_IS_UP)  
			return 0xFF;
		// Иначе принимаем байт
		Byte = UART_ReceiveData(UARTx);
		
		// Если за FESC принимаем TFEND
		if(Byte == TFEND)
		{
			// Значит это закодированный FEND
			Byte = FEND;
		}
		
		// Если за FESC принимаем TFESC
		else if (Byte == TFESC)
		{
			// Значит это закодированный FESC
			Byte = FESC;
		}
	}
	return Byte;
}



/*-----------------------------------------Обслуживание простых запросов-------------------------------------------------------------*/
static uint16_t ZPZ_RequestWithEmptyData(uint16_t CRC) 
{
	uint16_t crc;
	
	// Это простой запрос:
	// Тут следует только один пустой байт (=0), перед CRC
	// Просто примем его прям тут и посчитаем CRC с его учетом
	buffer[0] = UARTReceiveByte_by_SLIP(ZPZ_UART);
	crc = Crc16(buffer, 1, CRC);
	
	return crc;
}



//-----------------------------------------Обслуживание команды CHECK_CONNECT---------------------------------------------------------*/
static uint16_t ZPZ_Request_CHECK_CONNECT (uint16_t CRC)
{
	return ZPZ_RequestWithEmptyData(CRC);
}

void ZPZ_Response_CHECK_CONNECT (uint16_t NumPacket)
{
	ZPZ_ShortResponse(CHECK_CONNECT, NumPacket, SUCCES);
}



//-----------------------------------------Обслуживание команды START_DOWNLOAD---------------------------------------------------------*/
static uint16_t ZPZ_Request_START_DOWNLOAD (uint16_t CRC) 
{
	uint16_t crc;
	
	/* Приём пустого байта перед информацией (фиктивное считывание) */
	buffer[0] = UARTReceiveByte_by_SLIP(ZPZ_UART);
	/* Сразу же подсчитаем изменения в контрольной сумме */
	crc = Crc16(buffer, 1, CRC);
	/* Далее принимаем полезную часть информации и считаем crc */
	for(uint16_t i = 0; i < SIZE_OF_0_PACKET_DATA; i++)
	{
		/* Принимаем байт информации и пишем в буфер */
		buffer[i] = UARTReceiveByte_by_SLIP(ZPZ_UART);
		/* Пересчитываем контрольную сумму с учетом этого байта */
		crc = Crc16(&buffer[i], 1, crc);
	}
	
	/* Возвращаем контрольную сумму обратно в основную функцию для дорасчета */
	return crc;
}

static void ZPZ_Response_START_DOWNLOAD (uint16_t NumPacket)
{
	/* Если попали сюда, значит пакет валиден и его 
	   можно записать во flash */
	/* Очищаем память под полетное задание */
	ZPZ_eraseFlash(); 
	/* Записываем точку приземления и масштабы карты высот */
	ZPZ_writeFlash(ADDRESS_0_PACKET, buffer, SIZE_OF_0_PACKET_DATA);
	/* Отвечаем об успешной команде */
	ZPZ_ShortResponse(START_DOWNLOAD, NumPacket, SUCCES);
}



//-----------------------------------------Обслуживание команды MAP_DOWNLOAD---------------------------------------------------------*/
static uint16_t ZPZ_Request_MAP_DOWNLOAD (uint16_t CRC)
{
	uint16_t crc;
		
	/* Ожидаем приёма пустого байта перед информацией */
	buffer[0] = UARTReceiveByte_by_SLIP(ZPZ_UART);
	
	/* Сразу же подсчитаем изменения в контрольной сумме */
	crc = Crc16(buffer, 1, CRC);
		
	/* Далее принимаем полезную часть информации и сразу пишем ее во временный буфер */
	for(uint16_t i = 0; i < SIZE_OF_n_PACKET_DATA; i++)
	{
		/* Принимаем байт информации */
		buffer[i] = UARTReceiveByte_by_SLIP(ZPZ_UART);
		/* И сразу же подсчитывать контрольную сумму */
		crc = Crc16(&buffer[i], 1, crc);
	}
	
	return crc;
}

static void ZPZ_Response_MAP_DOWNLOAD (uint16_t NumPacket)
{
	uint32_t Address;
	
	/* Если мы попали сюда, значит пакет находящийся в буфере 
	валиден, его можно переложить на flash-память */
  for(uint16_t count = 0; count < SIZE_OF_n_PACKET_DATA; count++)
	{
	  /* Определяем адрес места записи с помощью макроса */
		Address = ADDRESS_n_PACKET_MAP(NumPacket);
		/* Пишем байт во flash */
		ZPZ_writeFlash (Address + count, &buffer[count], 1);
	}
	/* Если принятый пакет последний и мы попали сюда,
     значит все пакеты валидны, укладываем последний
     пакет в память и сверху признак валидности и 
		 и успешного статуса загрузки карты */
	if(NumPacket == NUMBER_OF_MAP_PACKET)
	{
		/* Берем адрес признака */
		Address = ADDRESS_MAP_TAG;
		/* Буфер теперь можно перезаписать,
       запишем в него признак	*/
		*((uint32_t*)buffer) = MAP_TAG; 
		/* А признак теперь уложим в конец*/
		ZPZ_writeFlash(Address, buffer, 4);
	}		
	ZPZ_ShortResponse(MAP_DOWNLOAD, NumPacket, SUCCES);
}



//-----------------------------------------Обслуживание команды START_UPLOAD---------------------------------------------------------*/
static uint16_t ZPZ_Request_START_UPLOAD (uint16_t CRC)
{
	return ZPZ_RequestWithEmptyData(CRC);
}

static void ZPZ_Response_START_UPLOAD (uint16_t NumPacket) 
{
	ZPZ_Response_Union   ZPZ_Response;     // Стандартный ответ к ЗПЗ           
	
	/* Заполняем структуру ответа и сразу начнем считать контрольную сумму */
	ZPZ_Response.Struct.Handler     = HANDLER_FROM_BUP;     /* Заголовок BU                        */
	ZPZ_Response.Struct.PacketSize  = 0x003C;               /* Размер пакета 60 байт               */
	ZPZ_Response.Struct.Command     = START_UPLOAD;         /* Команда: Инициация выгрузки карты   */
	ZPZ_Response.Struct.Count       = NumPacket;            /* Номер пакета берем из запроса       */
	ZPZ_Response.Struct.Error       = SUCCES;               /* Статус выполнения: без ошибок       */
	/* Считаем контрольную сумму базовой части пакета */
	ZPZ_Response.Struct.CRC         = Crc16(&ZPZ_Response.Buffer[0], 8, CRC16_INITIAL_FFFF);
		
	/* Посылаем признак начала пакета */
	SendFEND(ZPZ_UART);
	
	/* Отправим базовую часть пакета */
	for(uint16_t i = 0; i < 8; i++)
	{
		/* Посылаем байт информации */
		UARTSendByte_by_SLIP (ZPZ_UART, ZPZ_Response.Buffer[i]);
	}
	
	/* Далее будем читать внешнюю SPI память побайтно и сразу отправлять */
	for(uint16_t i = 0; i < SIZE_OF_0_PACKET_DATA; i++)
	{
		/* Читаем байт */
		ZPZ_readFlash(ADDRESS_0_PACKET+i, buffer, 1);
		/* Пересчитываем с его учетом контрольную сумму */
		ZPZ_Response.Struct.CRC = Crc16(buffer, 1, ZPZ_Response.Struct.CRC);
		/* Отправляем считанный байт */
		UARTSendByte_by_SLIP (ZPZ_UART, *buffer);
	}
	
	/* Отправим контрольную сумму */
	for(uint16_t i = 8; i < 10; i++)
	{
		/* Отправляем контрольную сумму */
		UARTSendByte_by_SLIP (ZPZ_UART, ZPZ_Response.Buffer[i]);
	}
	
	/* В конце посылаем признак конца пакета */
	SendFEND(ZPZ_UART);
}



//-----------------------------------------Обслуживание команды MAP_UPLOAD---------------------------------------------------------*/
static uint16_t ZPZ_Request_MAP_UPLOAD (uint16_t CRC)
{
	return ZPZ_RequestWithEmptyData(CRC);
}

static void ZPZ_Response_MAP_UPLOAD (uint16_t NumPacket)
{
	ZPZ_Response_Union      ZPZ_Response;       // Стандартный ответ к ЗПЗ
	uint32_t                Address;            // Адрес чтения из SPI Flash
	
	/* Заполняем структуру ответа и сразу начнем считать контрольную сумму */
	ZPZ_Response.Struct.Handler     = HANDLER_FROM_BUP; // Заголовок BU
	ZPZ_Response.Struct.PacketSize  = 0x0324;           // Размер пакета 804 байта
	ZPZ_Response.Struct.Command     = MAP_UPLOAD;       // Команда-запрос: Выгрузка карты
	ZPZ_Response.Struct.Count       = NumPacket;        // Номер пакета повторяем из запроса
	ZPZ_Response.Struct.Error       = SUCCES;           // Статус выполянения: без ошибок
	
	/* Вычисляем контрольную сумму базовой части пакета */
	ZPZ_Response.Struct.CRC         = Crc16(&ZPZ_Response.Buffer[0], 8, CRC16_INITIAL_FFFF);
		
	/* Посылаем признак начала пакета */
	SendFEND(ZPZ_UART);
	
	/* Отправим базовую часть пакета */
	for(uint16_t i = 0; i < 8; i++)
	{
		/* Отправляем байт за байтом основной части пакета */
		UARTSendByte_by_SLIP (ZPZ_UART, ZPZ_Response.Buffer[i]);
	}

	/* Далее будем читать внешнюю SPI память побайтно и сразу отправлять */
	for(uint16_t i = 0; i < SIZE_OF_n_PACKET_DATA; i++)
	{
		/* Определяем адрес с помощью макроса */
		Address = ADDRESS_n_PACKET_MAP(NumPacket);
		/* Читаем байт */
		ZPZ_readFlash (Address + i, buffer, 1);
		/* Пересчитываем с его учетом контрольную сумму */
		ZPZ_Response.Struct.CRC = Crc16(buffer, 1, ZPZ_Response.Struct.CRC);
		/* Отправляем считанное */
		UARTSendByte_by_SLIP (ZPZ_UART, *buffer);
	}
	
	/* Отправим контрольную сумму */
	for(uint16_t i = 8; i < 10; i++)
	{
		UARTSendByte_by_SLIP (ZPZ_UART, ZPZ_Response.Buffer[i]);
	}
	
	/* В конце посылаем признак конца пакета */
	SendFEND(ZPZ_UART);
}



//-----------------------------------------Обслуживание команды BIM_CONTROL---------------------------------------------------------*/
static uint16_t ZPZ_Request_BIM_CONTROL (uint16_t CRC)
{
	uint16_t crc;

	/* Должны принять 3 байта, какие пока неважно */
	for(uint8_t i = 0; i < 3; i++)
	{
		buffer[i] = UARTReceiveByte_by_SLIP(ZPZ_UART);
	}
	/* Считаем контрольную сумму, используя контрольную сумму основной части пакета CRC */
	crc = Crc16(buffer, 3, CRC);
	
	return crc;
}

static void ZPZ_Response_BIM_CONTROL (uint16_t NumPacket)
{
	uint16_t    BIM_Side;
	Bim_status  status;
	ZPZ_RequestControlBIM_Union BIM_Data;
	
	/* Разберем буфер */
	memcpy(&BIM_Data.Buffer, buffer, 3);
	
	/* Для отправки запроса БИМ необходимо, чтобы БИМ был запитан аппаратно */
	/* Если шпилька 1 вставлена */
	if (SelfTesting_PIN1() == ST_OK)
	{
		/* Ответим, что аппаратно питания на CAN нет, значит дальше не идем */
		ZPZ_ShortResponse(BIM_CONTROL, NumPacket, CAN_DISABLE_BY_PIN1);
		/* И завершаемся с ошибкой */
		return;
	}
	/* Если реле питания БИМ выкл */
	if (SelfTesting_POW_BIM() != ST_OK)
	{
		/* Ответим, что аппаратно питания на CAN нет, значит дальше не идем */
		ZPZ_ShortResponse(BIM_CONTROL, NumPacket, CAN_DISABLE_BY_RELAY);
		/* И завершаемся с ошибкой */
		return;
	}
		
	/* Определяем правым или левым БИМом будем управлять */
	if (BIM_Data.Struct.Side == L_BIM) 
		BIM_Side =  DEVICE_101;
	else if (BIM_Data.Struct.Side == R_BIM) 
		BIM_Side = DEVICE_100;
	else 
	{
		/* Такого ID не существует */
		ZPZ_ShortResponse(BIM_CONTROL, NumPacket, BIM_ID_NOT_EXIST);
		/* И завершаемся с ошибкой */
		return;
	}
  // Если БИМ сейчас в движении, то команда на вращение в противоположном направлении
  // Ломает его (резкое изменение напрвления видимо заставляет отдавать противо-ЭДС в сеть
  // И бим повисает по превышению питающего напряжения. Поэтому какой-бы не была команда от ЗПЗ
  // Проверяем вращается ли БИМ сейчас (по ненулевой скорости) и если да, то стопорим его, и
  // посылаем новую команду.
  
  if(BIM_Data.Struct.State == BIM_CMD_REQ)
    status = BIM_updateCommand(BIM_Side);
  else if (BIM_Data.Struct.State == BIM_CMD_OFF)
    status = BIM_stopCommand(BIM_Side);
  else if (BIM_Data.Struct.State == BIM_CMD_ON)
    status = BIM_controlCommand(BIM_Side, BIM_Data.Struct.Position);
	
	/* Теперь нужно ответить по ЗПЗ */		
	if (status == BIM_ERROR)
		ZPZ_ShortResponse(BIM_CONTROL, NumPacket, BIM_CTRL_FAILED);
  else 
    ZPZ_ShortResponse(BIM_CONTROL, NumPacket, SUCCES);
}



//-----------------------------------------Обслуживание команды BIM_STATUS---------------------------------------------------------*/
static uint16_t ZPZ_Request_BIM_STATUS (uint16_t CRC)	
{
	uint16_t crc = 0;
	
	buffer[0] = UARTReceiveByte_by_SLIP(ZPZ_UART);
	crc = Crc16(buffer, 1, CRC);
	
	return crc;
}

static void ZPZ_Response_BIM_STATUS(uint16_t NumPacket)
{
	ZPZ_ResponseStatusBIM_Union   ZPZ_BIM_Status;      // Структура с информацией от БИМ
	ZPZ_Response_Union            ZPZ_Response;        // Стандартный ответ к ЗПЗ
	uint16_t                      BIM_Side, i;         // Выбор левого, правого БИМ; счетчик циклов
	Bim_status                    status;              // Статус обмена с БИМ
	
	/* Разбираем буфер */
	uint8_t Side = buffer[0];
	
	/* Для отправки запроса БИМ необходимо, чтобы БИМ был запитан аппаратно */
	/* Если шпилька 1 вставлена */
	if (SelfTesting_PIN1() == ST_OK)
	{
		/* Ответим, что аппаратно питания на CAN нет, значит дальше не идем */
		ZPZ_ShortResponse(BIM_STATUS, NumPacket, CAN_DISABLE_BY_PIN1);
		/* И завершаемся с ошибкой */
		return;
	}
	/* Если реле питания БИМ выкл */
	if (SelfTesting_POW_BIM() != ST_OK)
	{
		/* Ответим, что аппаратно питания на CAN нет, значит дальше не идем */
		ZPZ_ShortResponse(BIM_STATUS, NumPacket, CAN_DISABLE_BY_RELAY);
		/* И завершаемся с ошибкой */
		return;
	}
	
	/* Определяем правым или левым БИМом будем управлять */
	if (Side == L_BIM) 
		BIM_Side =  DEVICE_101;
	else if (Side == R_BIM) 
		BIM_Side = DEVICE_100;
	/* Это случай ошибки, адрес выбран произвольно */
	else 
	{
		/* Такого ID не существует */
		ZPZ_ShortResponse(BIM_STATUS, NumPacket, BIM_ID_NOT_EXIST);
		/* И завершаемся с ошибкой */
		return;
	}
	
	/* Спросим состояние БИМов */
	status = BIM_updateCommand(BIM_Side);
	/* Проверим состоялся ли обмен, и если нет, то ответим ошибкой */
	if(status == BIM_ERROR)
	{	
		ZPZ_ShortResponse(BIM_STATUS, NumPacket, BIM_CTRL_FAILED);
		return;
	}
	
	/* Заполняем информационные поля информацией от БИМ */
	ZPZ_BIM_Status.Struct.Side          = Side;
	ZPZ_BIM_Status.Struct.StatusFlags   = BIM_getStatusFlags(BIM_Side);
	ZPZ_BIM_Status.Struct.Position      = BIM_getStrapPosition (BIM_Side);
	ZPZ_BIM_Status.Struct.Voltage       = BIM_getVoltage (BIM_Side);
	ZPZ_BIM_Status.Struct.Current       = BIM_getCurrent (BIM_Side);
	ZPZ_BIM_Status.Struct.Speed         = BIM_getSpeed (BIM_Side);
	
	
	/* Заполняем структуру базовых полей всех пакетов */
	ZPZ_Response.Struct.Handler    = HANDLER_FROM_BUP;  // Заголовок BU
	ZPZ_Response.Struct.PacketSize = 11;                // Размер пакета 11 байт
	ZPZ_Response.Struct.Command    = BIM_STATUS;        // Команда-запрос информации от БИМов
	ZPZ_Response.Struct.Count      = NumPacket;         // Номер пакета повторяем из запроса
	ZPZ_Response.Struct.Error      = SUCCES;            // Статус выполнения: без ошибок
	
	/* Считаем контрольную сумму */
	ZPZ_Response.Struct.CRC = Crc16(&ZPZ_Response.Buffer[0], 8, CRC16_INITIAL_FFFF);
	ZPZ_Response.Struct.CRC = Crc16(&ZPZ_BIM_Status.Buffer[0], 7, ZPZ_Response.Struct.CRC);
	
	/* Сначала отправляем признак-разделитель начала пакета */
	SendFEND(ZPZ_UART);
	
	/* Отправляем первичную часть пакета (первые 8 байт) */
	for(i = 0; i < 8; i++)
		UARTSendByte_by_SLIP (ZPZ_UART, ZPZ_Response.Buffer[i]);
	
	/* Отправляем информационную часть пакета */
	for(i = 0; i < 7; i++)
		UARTSendByte_by_SLIP (ZPZ_UART, ZPZ_BIM_Status.Buffer[i]);
	
	/* И в конце сверху посылаем контрольную сумму */
	for(i = 8; i < 10; i++)
		UARTSendByte_by_SLIP (ZPZ_UART, ZPZ_Response.Buffer[i]);
	
	/* И в конце опять разделитель */
	SendFEND(ZPZ_UART);
}



//-----------------------------------------Обслуживание команды LOG_FORMAT---------------------------------------------------------*/
static uint16_t ZPZ_Request_LOG_FORMAT (uint16_t CRC)
{
	uint16_t crc;
	
	/* Принимаем байт подкоманды: Запуск форматирования или Статус форматирования */
	buffer[0] = UARTReceiveByte_by_SLIP(ZPZ_UART);
	/* Подсчитываем контрольную сумму */
	crc = Crc16(buffer, 1, CRC);
	
	return crc;
}

static void ZPZ_Response_LOG_FORMAT (uint16_t NumPacket)
{
	ZPZ_Response_Union   ZPZ_Response;    // Стандартный ответ к ЗПЗ
	uint8_t              Subcommand;      // Код подкоманды
	
	/* Берем 0й байт из буфера - это наша подкоманда */
	Subcommand = buffer[0];

	/* Проверим верная ли пришла подкоманда: */
	if(Subcommand != 0x02 && Subcommand != 0x01)
	{
		/* Такой подкоманды не реализовано, коротко отвечаем с соответствующим признаком */
		ZPZ_ShortResponse(LOG_FORMAT, NumPacket, LOG_FORMAT_SUBCMD_ERROR);
		return;
	}
	
	/* Заполняем структуру ответа */
	ZPZ_Response.Struct.Handler    = HANDLER_FROM_BUP;    // Заголовок BU
	ZPZ_Response.Struct.PacketSize = 0x05;                // Размер пакета 5 байт
	ZPZ_Response.Struct.Command    = LOG_FORMAT;          // Команда форматирования черного ящика
	ZPZ_Response.Struct.Count      = NumPacket;           // Номер пакета берем из запроса
	ZPZ_Response.Struct.Error      = SUCCES;              // Статус выполнения: Без ошибок
		
	/* Подсчитываем контрольную сумму основной части пакета */
	ZPZ_Response.Struct.CRC = Crc16(&ZPZ_Response.Buffer[0], 8, CRC16_INITIAL_FFFF);
	
	/* Сначала отправляем признак-разделитель начала пакета */
	SendFEND(ZPZ_UART);
	
	/* Отправляем первичную часть пакета - 8 байт */
	for(uint8_t i = 0; i < 8; i++)
		UARTSendByte_by_SLIP (ZPZ_UART, ZPZ_Response.Buffer[i]);
	
	/* Запрос на форматирование накопителя черного ящика */
	if(Subcommand == 0x02)
	{
		/* Отвечаем, что приняли команду и переходим на её выполнение */
		buffer[0] = FORMATING_LOG_FS_STARTED;
		ZPZ_Response.Struct.CRC = Crc16(buffer, 1, ZPZ_Response.Struct.CRC);
		UARTSendByte_by_SLIP (ZPZ_UART, *buffer);
		
		/* После сверху посылаем контрольную сумму */
		for(uint8_t i = 8; i < 10; i++)
			UARTSendByte_by_SLIP (ZPZ_UART, ZPZ_Response.Buffer[i]);
		/* И в конце опять разделитель */
		SendFEND(ZPZ_UART);
		
		/* Запускаем форматирование */
		LogFs_format();
		/* После форматирование необходимо обновить информацию о файловой системе */
		LogFs_initialize();
		
		return;
	}
	/* Запрос статуса форматирования */
	else if (Subcommand == 0x01)
	{
		/* Отвечаем, что форматирование завершено или в данный момент не выполняется */
		buffer[0]= FORMATING_LOG_FS_COMPLETED;
		ZPZ_Response.Struct.CRC = Crc16(buffer, 1, ZPZ_Response.Struct.CRC);
		UARTSendByte_by_SLIP (ZPZ_UART, *buffer);
		
		/* После сверху посылаем контрольную сумму */
		for(uint8_t i = 8; i < 10; i++)
			UARTSendByte_by_SLIP (ZPZ_UART, ZPZ_Response.Buffer[i]);
		/* И в конце опять разделитель */
		SendFEND(ZPZ_UART);
		
		return;
	}
}



//-----------------------------------------Обслуживание команды LOG_FILES---------------------------------------------------------*/
static uint16_t ZPZ_Request_LOG_FILES (uint16_t CRC)
{
	return ZPZ_RequestWithEmptyData(CRC);
}

static void ZPZ_Response_LOG_FILES (uint16_t NumPacket)
{
	ZPZ_Response_Union    ZPZ_Response;    // Стандартный ответ к ЗПЗ
	uint16_t              files = 0;       // Количество файлов в хранилище
	uint8_t               result = 0;      // Коды ошибок файловой системы
	
	/* Проверим целлостность и разметку файловой системы */
	if(SelfTesting_LogFs() != ST_OK)
	{
		/* Файловая система поверждена, чтение данных невозможно */
		ZPZ_ShortResponse(LOG_FILES, NumPacket, LOG_FS_IS_CORRUPTED);
		return;
	}
	
	/* Заполняем структуру общей части всех пакетов */
	ZPZ_Response.Struct.Handler    = HANDLER_FROM_BUP;     // Заголовок BU
	ZPZ_Response.Struct.Command    = LOG_FILES;            // Команда: Запрос информации о содержимом
	ZPZ_Response.Struct.Count      = NumPacket;            // Номер пакета берем из запроса
	ZPZ_Response.Struct.Error      = SUCCES;               // Статус выполнения: Без ошибок
		
	/* Минимально возможное число байт пакета: 
	   Команда (1б) + Номер пакета (2б) + Error (1б) + Кол-во файлов (2б) = 6 байт
	   Остальное зависит от количества файлов в накопителе */
	ZPZ_Response.Struct.PacketSize = 6;
	/* Тогда определим количество файлов и прибавим (для каждого файла отводится 6 байт сообщения) */
	files = LogFs_getFileNumber();
	ZPZ_Response.Struct.PacketSize += 6*files;
	
	/* Теперь посчитаем контрольную сумму начала пакета (первые 8 байт) */
	ZPZ_Response.Struct.CRC = Crc16(&ZPZ_Response.Buffer[0], 8, CRC16_INITIAL_FFFF);
	
	/* Начнём отправлять сообщение */
	/* Сначала отправляем признак-разделитель начала пакета */
	SendFEND(ZPZ_UART);
	/* Теперь первые 8 байт */
	for(uint8_t i = 0; i < 8; i++)
		UARTSendByte_by_SLIP (ZPZ_UART, ZPZ_Response.Buffer[i]);
	
	/* Готовим к отправке число файлов (2 байта) */
	*(uint16_t*)buffer = (uint16_t)files;
	/* Подсчитываем контрольную сумму с учетом этого */
	ZPZ_Response.Struct.CRC = Crc16(buffer, 2, 	ZPZ_Response.Struct.CRC);
	/* Отправляем эти два байта с числом файлов */
	for(uint8_t i = 0; i < 2; i++)
		UARTSendByte_by_SLIP (ZPZ_UART, buffer[i]);
	
	
	/* Определение, упаковка, подсчет контрольной суммы и отправка остальной части пакета (номера файлов и размеры) */
	for(uint32_t j = 0; j < files; j++)
	{	
		/* Если первая итерация то, (как бы устанавливаем точку отсчета) */
		/* Ищем первый (самый старый файл в директории) */
		if(j == 0) 
			result = LogFs_findFile(FIRST_FILE);
		/* При последующих итерациях нужно просто прокручивать файлы 
		   Командой "NEXT_FILE" */
		else 
			result = LogFs_findFile(NEXT_FILE);
		
		/* Узнаем его номер файла и сразу упакуем в буфер для отправки */
		*(uint16_t*)buffer = LogFs_getFileProperties(FILE_NUMBER);
		/* Подсчитываем контрольную сумму */
		ZPZ_Response.Struct.CRC = Crc16(buffer, 2, 	ZPZ_Response.Struct.CRC);
		/* Отправляем эти 2 байта информации о номере файла */
		for(uint8_t i = 0; i < 2; i++)
			UARTSendByte_by_SLIP (ZPZ_UART, buffer[i]);
		
		/* Узнаем его размер и сразу упакуем в буфер для отправки */
		*((uint32_t*)buffer) = LogFs_getFileProperties (FILE_SIZE);
		/* Подсчитываем контрольную сумму */
		ZPZ_Response.Struct.CRC = Crc16(buffer, 4, 	ZPZ_Response.Struct.CRC);
		/* Отправляем эти 4 байта информации о размере файла */
		for(uint8_t i = 0; i < 4; i++)
			UARTSendByte_by_SLIP (ZPZ_UART, buffer[i]);
		
		/* Дополнительная проверка по выходу из цикла
		   Если все файлы были просмотрены, функция LogFs_findFile() возвращает FS_ALL_FILES_SCROLLS */
		/* Проконтролируем это */
		if (result ==  FS_ALL_FILES_SCROLLS)
			break;
	}
	
	// После сверху посылаем контрольную сумму
	for(uint8_t i = 8; i < 10; i++)
		UARTSendByte_by_SLIP (ZPZ_UART, ZPZ_Response.Buffer[i]);
	
	// И в конце опять разделитель
	SendFEND(ZPZ_UART);
}



//-----------------------------------------Обслуживание команды LOG_UPLOAD---------------------------------------------------------*/
static uint16_t ZPZ_Request_LOG_UPLOAD(uint16_t CRC)
{
	uint16_t crc = 0;
	
	/* Допринимаем остальные два байта с номером файла
	   который просят отправить */
	for(uint8_t i = 0; i < 2; i++)
	  buffer[i] = UARTReceiveByte_by_SLIP(ZPZ_UART);
	/* Подсчитываем crc */
	crc = Crc16(buffer, 2, CRC);
	
	return crc;
}

static void ZPZ_Response_LOG_UPLOAD(uint16_t NumPacket)
{
	ZPZ_Response_Union    ZPZ_Response;      // Стандартный ответ к ЗПЗ
	uint32_t              filesize;          // Размер файла
	uint16_t              packet_count = 0;  // Количество пакетов на которые будет разбит файл
	uint32_t              offset;            // Смещение (в байтах) от начала файла
	uint16_t              fileNumber;        // Номер запрашиваемого файла
	
	/* Заберем из буфера номер запрашиваемого файла */
	fileNumber = *((uint16_t*)buffer);
	
	/* Ищем файл в хранилище по номеру
	   Если функция не найдет файл, то выдаст ошибку, тогда можно не продолжать */
	if(LogFs_findFileByNum(fileNumber)!= FS_FINE)
	{
		/* Ответим ошибкой: Файл с таким номером не найден */
		ZPZ_ShortResponse(LOG_UPLOAD, 0, LOG_FS_FILE_NOT_FIND);
		return;
	}

	/* Узнаем размер файла */
  filesize = LogFs_getFileProperties(FILE_SIZE);

	/* Заполняем структуру общей части всех пакетов */
	ZPZ_Response.Struct.Handler    = HANDLER_FROM_BUP; // Заголовок BU
	ZPZ_Response.Struct.Command    = LOG_UPLOAD;       // Команда: выгрузка файла
	ZPZ_Response.Struct.Count      = NumPacket;        // Номер пакета при разбиении файла
	ZPZ_Response.Struct.Error      = SUCCES;           // Статус выполнения: Без ошибок
	
	/* Размер пакета всегда не меньше 6 байт (Код команды, Номер пакета, Статус, Номер файла) */
	ZPZ_Response.Struct.PacketSize = 6;
	
	/* Если номер пакета нулевой, то запрашивается количество пакетов, 
	   на которые будет разбит файл с номером fileNumber */
	if(NumPacket == 0)
	{
		/* Узнаем на сколько пакетов по BYTE_FROM_FILE байт разобьётся файл */
		packet_count = (uint16_t)((filesize/BYTE_FROM_FILE));
		if (filesize % BYTE_FROM_FILE != 0)
			packet_count++;
		
	  ZPZ_Response.Struct.PacketSize += 2;
	  /* Теперь посчитаем контрольную сумму начала пакета (первые 8 байт) */
	  ZPZ_Response.Struct.CRC = Crc16(&ZPZ_Response.Buffer[0], 8, CRC16_INITIAL_FFFF);

		/* Начнём отправлять сообщение */
	  /* Сначала отправляем признак-разделитель начала пакета */
	  SendFEND(ZPZ_UART);
	  /* Теперь первые 8 байт */
	  for(uint8_t i = 0; i < 8; i++)
			UARTSendByte_by_SLIP (ZPZ_UART, ZPZ_Response.Buffer[i]);
		
		/* Отправляем номер файла */
	  *(uint16_t*)buffer = fileNumber; 
	  /* Подсчитываем контрольную сумму с учетом этого */
	  ZPZ_Response.Struct.CRC = Crc16(buffer, 2, ZPZ_Response.Struct.CRC);
	  /* Отправляем номер файла */
	  for(uint8_t i = 0; i < 2; i++)
			UARTSendByte_by_SLIP (ZPZ_UART, buffer[i]);
		
		/* Отправляем количество пакетов файла */
	  *(uint16_t*)buffer = packet_count;
	  /* Подсчитываем контрольную сумму с учетом этого */
	  ZPZ_Response.Struct.CRC = Crc16(buffer, 2, 	ZPZ_Response.Struct.CRC);
	  /* Отправляем номер файла */
	  for(uint8_t i = 0; i < 2; i++)
			UARTSendByte_by_SLIP (ZPZ_UART, buffer[i]);			
	}
	
	/* Если не нулевой номер пакета, то уже запращивается информация из файла
	   Причем конкретно пакет (на которые был разбит файл) с номером NumPacket */
	else
	{
		/* Нужно проверить есть ли вообще такой номер пакета */
		/* Узнаем на сколько пакетов по BYTE_FROM_FILE байт разобьётся файл */
		packet_count = (uint16_t)((filesize/BYTE_FROM_FILE));
		if (filesize % BYTE_FROM_FILE != 0)
			packet_count++;
		
		/* Если запрашиваемый номер пакета не существует */
		if(NumPacket > packet_count)
		{
			/* Ответим ошибкой */
			ZPZ_ShortResponse(LOG_UPLOAD, NumPacket, LOG_FS_PACKET_NOT_EXIST);
			return;
		}
		
		/* Такой пакет определен, готовимся к отправке */
		
		/* Если запрашиваемый пакет является крайним (либо единственным) */
		if(packet_count - NumPacket == 0)
		{
			/* Увеличиваем размер на оставшуюся часть файла */
			ZPZ_Response.Struct.PacketSize += filesize - (packet_count - 1) * BYTE_FROM_FILE;
			
			/* Теперь посчитаем контрольную сумму начала пакета (первые 8 байт) */
			ZPZ_Response.Struct.CRC = Crc16(&ZPZ_Response.Buffer[0], 8, CRC16_INITIAL_FFFF);
			
			/* Для выгрузки нужного куска, необходимо вычислить смещение в байтах от начала файла */
			offset = (NumPacket-1)* BYTE_FROM_FILE;
			
			/* Начнём отправлять сообщение */
			/* Сначала отправляем признак-разделитель начала пакета */
			SendFEND(ZPZ_UART);
			/* Теперь первые 8 байт  */
			for(uint8_t i = 0; i < 8; i++)
				UARTSendByte_by_SLIP (ZPZ_UART, ZPZ_Response.Buffer[i]);
			
			/* Отправляем номер файла */
			*(uint16_t*)buffer = fileNumber;
			/* Подсчитываем контрольную сумму с учетом этого */
			ZPZ_Response.Struct.CRC = Crc16(buffer, 2, 	ZPZ_Response.Struct.CRC);
			/* Отправляем номер файла */
			for(uint8_t i = 0; i < 2; i++)
				UARTSendByte_by_SLIP (ZPZ_UART, buffer[i]);
					
			/* Указатель внутри функции чтения уже установлен ранее, теперь готовы читать нужную часть  */
			for(uint32_t i = 0; i < (filesize - (packet_count - 1) * BYTE_FROM_FILE); i++)
			{
				/* Будем читать и отправлять побайтово, не забывая пересчитывать контрольную сумму */
				LogFs_readFile(buffer, offset + i, 1);
				/* Сразу подсчитываем контрольную сумму */
			  ZPZ_Response.Struct.CRC = Crc16(buffer, 1, 	ZPZ_Response.Struct.CRC);
			  /* И сразу отправляем */
			  UARTSendByte_by_SLIP (ZPZ_UART, buffer[0]);
			}
		}
		/* Пакет не крайний  */
		else
		{
			/* Увеличиваем размер на BYTE_FROM_FILE байт данных */
			ZPZ_Response.Struct.PacketSize += BYTE_FROM_FILE;
			
			/* Теперь посчитаем контрольную сумму начала пакета (первые 8 байт) */
			ZPZ_Response.Struct.CRC = Crc16(&ZPZ_Response.Buffer[0], 8, CRC16_INITIAL_FFFF);
			
			/* Для выгрузки нужного куска, необходимо вычислить смещение в байтах от начала файла */
			offset = (NumPacket-1)* BYTE_FROM_FILE;
			
			/* Начнём отправлять сообщение */
			/* Сначала отправляем признак-разделитель начала пакета */
			SendFEND(ZPZ_UART);
			/* Теперь первые 8 байт  */
			for(uint8_t i = 0; i < 8; i++)
				UARTSendByte_by_SLIP (ZPZ_UART, ZPZ_Response.Buffer[i]);
			
			/* Отправляем номер файла */
			*(uint16_t*)buffer = fileNumber;
			/* Подсчитываем контрольную сумму с учетом этого */
			ZPZ_Response.Struct.CRC = Crc16(buffer, 2, 	ZPZ_Response.Struct.CRC);
			/* Отправляем номер файла  */
			for(uint8_t i = 0; i < 2; i++)
				UARTSendByte_by_SLIP (ZPZ_UART, buffer[i]);
			
			/* Далее займёмся отправкой данных пакета */
			
			/* Указатель внутри функции чтения уже установлен ранее, теперь готовы читать нужную часть  */
			for(uint32_t i = 0; i < BYTE_FROM_FILE; i++)
			{
				/* Будем читать и отправлять побайтово, не забывая пересчитывать контрольную сумму */
				LogFs_readFile(buffer, offset + i, 1);
				/* Сразу подсчитываем контрольную сумму */
				ZPZ_Response.Struct.CRC = Crc16(buffer, 1, ZPZ_Response.Struct.CRC);
				/* И сразу отправляем */
				UARTSendByte_by_SLIP (ZPZ_UART, buffer[0]);
			}
		}
	}
		
	/* После сверху посылаем контрольную сумму */
	for(uint8_t i = 8; i < 10; i++)
		UARTSendByte_by_SLIP (ZPZ_UART, ZPZ_Response.Buffer[i]);
		
	/* И в конце опять разделитель */
	SendFEND(ZPZ_UART);
}



//-----------------------------------------Обслуживание команды REQ_SWS---------------------------------------------------------*/
static uint16_t ZPZ_Request_REQ_SWS (uint16_t CRC)
{
	return ZPZ_RequestWithEmptyData(CRC);
}

static void ZPZ_Response_REQ_SWS (uint16_t NumPacket)
{
	ZPZ_Response_Union    ZPZ_Response;    // Стандартный ответ к ЗПЗ
	TimeoutType           timeout;         // Таймаут контроль

	/* Принимаем данные */ 
	setTimeout (&timeout, 40);
	while(SWS_update () && (timeoutStatus(&timeout) != TIME_IS_UP));
	/* Проверим был ли это выход по таймауту */
	if(timeout.status == TIME_IS_UP)
	{
		// Если да, то нужно ответить ошибкой, так как СВС недоступен
		ZPZ_ShortResponse(REQ_SWS, 0, SWS_IS_UNAVAILABLE);
		// И завершиться с ошибкой
		return;
	}
	
	/* Если данные приняты верно, то надо продолжать */
	/* Заполняем структуру общей части всех пакетов */
	ZPZ_Response.Struct.Handler    = HANDLER_FROM_SWS;// Заголовок BU
	ZPZ_Response.Struct.PacketSize = 28;              // Размер пакета 28 байт
	ZPZ_Response.Struct.Command    = REQ_SWS;         // Команда-запрос данных от СВС
	ZPZ_Response.Struct.Count      = NumPacket;       // Номер ответа берём из запроса
	ZPZ_Response.Struct.Error      = SUCCES;          // Статус выполнения: без ошибок
	
	/* Теперь посчитаем контрольную сумму начала пакета (первые 8 байт) */
	ZPZ_Response.Struct.CRC = Crc16(&ZPZ_Response.Buffer[0], 8, CRC16_INITIAL_FFFF);
	
	/* Начнём отправлять сообщение */
	/* Сначала отправляем признак-разделитель начала пакета */
	SendFEND(ZPZ_UART);
	
	/* Теперь первые 8 байт */
	for(uint8_t i = 0; i < 8; i++)
		UARTSendByte_by_SLIP (ZPZ_UART, ZPZ_Response.Buffer[i]);
	
	/* Теперь будем отправлять информацию от СВС
	   Нас интересует данные начиная со статического давления, кроме резервных полей и CRC
	   Начинаем отправлять с давления (8 байт), до истинной скорости (до 28 байта) */
	for(uint8_t i = 8; i < 28; i++)
	{
		/* Сразу подсчитываем контрольную сумму */
		ZPZ_Response.Struct.CRC = Crc16(&SWS_data.Buffer[i], 1, ZPZ_Response.Struct.CRC);
		/* И отправляем по байтам */
		UARTSendByte_by_SLIP (ZPZ_UART, SWS_data.Buffer[i]);
	}
	
	/* Так же отправляем 4 байта состояния СВС (с 48 по 52 байт) */
	for(uint8_t i = 48; i < 52; i++)
	{
		/* Сразу подсчитываем контрольную сумму */
		ZPZ_Response.Struct.CRC = Crc16(&SWS_data.Buffer[i], 1, ZPZ_Response.Struct.CRC);
		// И отправляем по байтам
		UARTSendByte_by_SLIP (ZPZ_UART, SWS_data.Buffer[i]);
	}
	
	/* После сверху посылаем контрольную сумму */
	for(uint8_t i = 8; i < 10; i++)
		UARTSendByte_by_SLIP (ZPZ_UART, ZPZ_Response.Buffer[i]);
	/* И в конце опять разделитель */
	/* Если FIFO передатчика заполнен, то ждем пока освободится */
	SendFEND(ZPZ_UART);
}



//-----------------------------------------Обслуживание команды REQ_SNS_POS---------------------------------------------------------*/
static uint16_t ZPZ_Request_REQ_SNS_POS (uint16_t CRC)
{
	return ZPZ_RequestWithEmptyData(CRC);
}

static void ZPZ_Response_REQ_SNS_POS (uint16_t NumPacket)
{
	ZPZ_Response_Union    ZPZ_Response;  // Стандартный ответ к ЗПЗ
	
	/* Обновляем данные СНС */
	Bup_updateData();
	
	/* Проверим исправен ли СНС (функция выше должна обновить состояние) */
	if (SelfTesting_STATUS(ST_sns) == ST_FAULT)
	{
		/* Если нет, то нужно ответить ошибкой: Не удаётся получить данные от СНС */
		ZPZ_ShortResponse(REQ_SNS_POS, NumPacket, SNS_IS_UNAVAILABLE);
		/* И завершиться с ошибкой */
		return;
	}		
	
	/* Иначе продолжаем
	   Заполняем структуру общей части всех пакетов */
	ZPZ_Response.Struct.Handler    = HANDLER_FROM_SNS;    // Заголовок BU
	ZPZ_Response.Struct.PacketSize = 72;                  // Размер пакета 72 байта
	ZPZ_Response.Struct.Command    = REQ_SNS_POS;         // Команда-запрос информации об местоположении от СНС
	ZPZ_Response.Struct.Count      = NumPacket;	          // Повторяем номер из запроса
	ZPZ_Response.Struct.Error      = SUCCES;              // Статус выполнения: без ошибок
	
	/* Теперь посчитаем контрольную сумму начала пакета (первые 8 байт) */
	ZPZ_Response.Struct.CRC = Crc16(&ZPZ_Response.Buffer[0], 8, CRC16_INITIAL_FFFF);
	
	/* Начнём отправлять сообщение */
	/* Сначала отправляем признак-разделитель начала пакета */
	SendFEND(ZPZ_UART);
	/* Теперь первые 8 байт */
	for(uint8_t i = 0; i < 8; i++)
		UARTSendByte_by_SLIP (ZPZ_UART, ZPZ_Response.Buffer[i]);
	
	/* Высылаем данные местоположения */
	/* Из полученного пакета с местоположением от СНС выкидываем CRC, 
	   тип ответа и счетчик в начале, поэтому i = 4 */
	for(uint8_t i = 4; i < SizeAnsPDR; i++)
	{
		/* Побайтово досчитываем контрольную сумму */
		ZPZ_Response.Struct.CRC = Crc16(&SNS_position.Buffer[i], 1, ZPZ_Response.Struct.CRC);
		/* И отправляем */
		UARTSendByte_by_SLIP (ZPZ_UART, SNS_position.Buffer[i]);
	}
	
	/* Высылаем данные ориентации
	   из полученного пакета с ориентацией от СНС выкидываем CRC, 
		 тип ответа и счетчик в начале, поэтому i = 4 */
	for(uint8_t i = 4; i < SizeAnsODR; i++)
	{
		/* Побайтово досчитываем контрольную сумму */
		ZPZ_Response.Struct.CRC = Crc16(&SNS_orientation.Buffer[i], 1, ZPZ_Response.Struct.CRC);
		/* И отправляем */
		UARTSendByte_by_SLIP (ZPZ_UART, SNS_orientation.Buffer[i]);
	}
	
	/* После сверху посылаем контрольную сумму */
	for(uint8_t i = 8; i < 10; i++)
		UARTSendByte_by_SLIP (ZPZ_UART, ZPZ_Response.Buffer[i]);
		
	/* И в конце опять разделитель */
	SendFEND(ZPZ_UART);
}



//-----------------------------------------Обслуживание команды REQ_SNS_STATE---------------------------------------------------------*/
static uint16_t ZPZ_Request_REQ_SNS_STATE (uint16_t CRC)
{
	return ZPZ_RequestWithEmptyData(CRC);
}

static void ZPZ_Response_REQ_SNS_STATE (uint16_t NumPacket)
{
	ZPZ_Response_Union ZPZ_Response;          // Стандартный ответ к ЗПЗ
	TimeoutType        timeout;               // Таймаут-контроль
	
	/* Получим данные об устройстве */
	setTimeout (&timeout, 100);
	while(SNS_updateDeviceInformation() != SNS_OK && (timeoutStatus(&timeout) != TIME_IS_UP));
	
	/* Проверим, вдруг выход был по таймауту, тогда нужно ответить ошибкой и завершиться */
	if(timeout.status == TIME_IS_UP)
	{
	  /* Если да, то нужно ответить ошибкой */
		ZPZ_ShortResponse(REQ_SNS_STATE, NumPacket, SNS_IS_UNAVAILABLE);
		/* И завершиться с ошибкой */
		return;
	}
	
	/* Получим информацию о доступных данных */
	setTimeout (&timeout, 100);
	while(SNS_updateDataState() != SNS_OK && (timeoutStatus(&timeout) != TIME_IS_UP));
	/* Проверим, вдруг выход был по таймауту, тогда нужно ответить ошибкой и завершиться */
	if(timeout.status == TIME_IS_UP)
	{
	  /* Если да, то нужно ответить ошибкой */
		ZPZ_ShortResponse(REQ_SNS_STATE, NumPacket, SNS_IS_UNAVAILABLE);
		/* И завершиться с ошибкой */
		return;
	}
	
	/* Иначе продолжаем */
	/* Заполняем структуру общей части всех пакетов */
	ZPZ_Response.Struct.Handler    = HANDLER_FROM_SNS;   // Заголовок BU
	ZPZ_Response.Struct.PacketSize = 23;                 // Размер пакета 23 байта
	ZPZ_Response.Struct.Command    = REQ_SNS_STATE;      // Команда-запрос состояния СНС
	ZPZ_Response.Struct.Count      = NumPacket;	         // Повторяем номер как в запросе
	ZPZ_Response.Struct.Error      = SUCCES;             // Статус выполнения команда: без ошибок
	
	/* Теперь посчитаем контрольную сумму начала пакета (первые 8 байт) */
	ZPZ_Response.Struct.CRC = Crc16(&ZPZ_Response.Buffer[0], 8, CRC16_INITIAL_FFFF);
	
	/* Начнём отправлять сообщение */
	/* Сначала отправляем признак-разделитель начала пакета */
	SendFEND(ZPZ_UART);
	/* Теперь первые 8 байт */
	for(uint8_t i = 0; i < 8; i++)
		UARTSendByte_by_SLIP (ZPZ_UART, ZPZ_Response.Buffer[i]);
	
	
	/* Высылаем данные об устройстве
	   из полученного пакета с информацией об устройсстве от СНС выкидываем CRC, 
	   тип ответа и счетчик в начале, поэтому i = 4; */
	for(uint8_t i = 4; i < SizeAnsDIR; i++)
	{
		/* Побайтово досчитываем контрольную сумму */
		ZPZ_Response.Struct.CRC = Crc16(&SNS_deviceInformation.Buffer[i], 1, ZPZ_Response.Struct.CRC);
		/* И отправляем */
		UARTSendByte_by_SLIP (ZPZ_UART, SNS_deviceInformation.Buffer[i]);
	}

	/* Высылаем инфу о доступных данных 
	   из полученного пакета с доступными данными от СНС необходимо только поле Data_State
	   Побайтово досчитываем контрольную сумму */
	ZPZ_Response.Struct.CRC = Crc16(&SNS_availableData.Struct.Data_State, 1, ZPZ_Response.Struct.CRC);
	/* И отправляем */
	UARTSendByte_by_SLIP (ZPZ_UART, SNS_availableData.Struct.Data_State);
	
	/* После сверху посылаем контрольную сумму */
	for(uint8_t i = 8; i < 10; i++)
		UARTSendByte_by_SLIP (ZPZ_UART, ZPZ_Response.Buffer[i]);
	
	/* И в конце опять разделитель */
	SendFEND(ZPZ_UART);
}



//-----------------------------------------Обслуживание команды REQ_SNS_SETTINGS---------------------------------------------------------*/
static uint16_t ZPZ_Request_REQ_SNS_SETTINGS(uint16_t CRC)
{
	uint16_t crc = 0;
	
	for(uint8_t i = 0; i < 3; i++)
	{
		buffer[i] = UARTReceiveByte_by_SLIP(ZPZ_UART);
	}
	crc = Crc16(buffer, 3, CRC);

	return crc;
}

/* Реагируем на запрос на изменение настроек СНС */
static void ZPZ_Response_REQ_SNS_SETTINGS (uint16_t NumPacket)
{
	ZPZ_Response_Union   ZPZ_Response;          // Стандартный ответ к ЗПЗ
		
	/* Анализируем пришедшую команду и выполняем действия */
	
	/* Команда включения горизонтальной коррекции */
	if(buffer[0]== 0x01)
		SNS_enableHorizontalCorrection();
	
	/* Команда отключения горизонтальной коррекции */
	else if (buffer[0] == 0x02) 
		SNS_disableHorizontalCorrection();
	
	/* Параметр не опознан */
	else 
	{
		/* Ответим, что ошибка в параметрах команды */
		ZPZ_ShortResponse(REQ_SNS_STATE, NumPacket, SNS_SETTINGS_WRONG_PAR);
		/* И завершаемся с ошибкой */
		return;
	}
		
	/* Команда начала калибровки гироскопа */
	if(buffer[1]== 0x01) 
		SNS_startGyroCalibration();
	
	/* Команда сброса калибровки гироскопа */
	else if (buffer[1] == 0x02)
		SNS_resetGyroCalibration();
	
	/* Параметр не опознан */
	else
	{
		/* Ответим, что ошибка в параметрах команды */
		ZPZ_ShortResponse(REQ_SNS_STATE, NumPacket, SNS_SETTINGS_WRONG_PAR);
		/* И завершаемся с ошибкой */
		return;
	}
	
	/* Команда начала калибровки магнитометра */
	if(buffer[2]== 0x01)
		SNS_startMagnetometerCalibration();
	
	/* Команда сброса калибровки магнитометра */
	else if (buffer[2] == 0x02)
		SNS_resetMagnetometerCalibration();

	/* Параметр не опознан */
	else
	{
		/* Ответим, что ошибка в параметрах команды */
		ZPZ_ShortResponse(REQ_SNS_STATE, NumPacket, SNS_SETTINGS_WRONG_PAR);
		/* И завершаемся с ошибкой */
		return;
	}
	
	
	/* Теперь нужно ответить  */
	/* Заполняем структуру общей части всех пакетов  */
	ZPZ_Response.Struct.Handler    = HANDLER_FROM_SNS;    // Заголовок BU
	ZPZ_Response.Struct.PacketSize = 7;                   // Размер пакета 7 байт
	ZPZ_Response.Struct.Command    = REQ_SNS_SETTINGS;    // Команда-запрос Настройки СНС
	ZPZ_Response.Struct.Count      = NumPacket;	          // Номер пакета повторяем из запроса
	ZPZ_Response.Struct.Error      = SUCCES;              // Статус выполнения: без ошибок
	

	/* Теперь посчитаем контрольную сумму начала пакета (первые 8 байт) */
	ZPZ_Response.Struct.CRC = Crc16(&ZPZ_Response.Buffer[0], 8, CRC16_INITIAL_FFFF);
	/* Начнём отправлять сообщение */
	/* Сначала отправляем признак-разделитель начала пакета */
	SendFEND(ZPZ_UART);
	/* Теперь первые 8 байт  */
	for(uint8_t i = 0; i < 8; i++)
		UARTSendByte_by_SLIP (ZPZ_UART, ZPZ_Response.Buffer[i]);
	
	/* В ответе нужно продублировать (повторить) настройки (3 байта) */
	for(uint8_t i = 0; i < 3; i++)
	{
		/* Побайтово досчитываем контрольную сумму */
		ZPZ_Response.Struct.CRC = Crc16(buffer, 3, ZPZ_Response.Struct.CRC);
		/* И отправляем */
		UARTSendByte_by_SLIP (ZPZ_UART, buffer[i]);
	}
	
	/* После сверху посылаем контрольную сумму */
	for(uint8_t i = 8; i < 10; i++)
		UARTSendByte_by_SLIP (ZPZ_UART, ZPZ_Response.Buffer[i]);
	
	/* И в конце опять разделитель */
	SendFEND(ZPZ_UART);
}


//-----------------------------------------Обслуживание команды SYSTEM_STATE---------------------------------------------------------*/
static uint16_t ZPZ_Request_SYSTEM_STATE (uint16_t CRC)
{
	return ZPZ_RequestWithEmptyData(CRC);
}

static void ZPZ_Response_SYSTEM_STATE (uint16_t NumPacket)
{
	ZPZ_Response_Union   ZPZ_Response;   /* Стандартный ответ к ЗПЗ */
		
	/* Теперь нужно ответить  */
	/* Заполняем структуру общей части всех пакетов */
	ZPZ_Response.Struct.Handler    = HANDLER_FROM_BUP;    // Заголовок BU       
	ZPZ_Response.Struct.PacketSize = 13;                  // Размер пакета в байтах  
	ZPZ_Response.Struct.Command    = SYSTEM_STATE;        // Команда: передача по CAN
	ZPZ_Response.Struct.Count      = NumPacket;	          // Номер пакета повторяем из запроса
	ZPZ_Response.Struct.Error      = SUCCES;              // Статус: без ошибок
	
	/* Теперь посчитаем контрольную сумму начала пакета (первые 8 байт) */
	ZPZ_Response.Struct.CRC = Crc16(&ZPZ_Response.Buffer[0], 8, CRC16_INITIAL_FFFF);
	
	/* Кладём состояние системы в буфер */
  *((uint16_t*)buffer)            = systemState;
  /* Кладём заряд батареи */
  *((float*)((uint8_t*)buffer+2)) = Bup_getBatteryVoltage(); 
	/* Кладём версию прошивки */
  *((uint8_t*)buffer+6)           = bupFirmwareVersion.microFirmware;
  *((uint8_t*)buffer+7)           = bupFirmwareVersion.minorFirmware;
  *((uint8_t*)buffer+8)           = bupFirmwareVersion.majorFirmware;
	
	/* Теперь посчитаем контрольную сумму с учетом отправляемого */
	ZPZ_Response.Struct.CRC = Crc16(buffer, 9, ZPZ_Response.Struct.CRC);
	
	/* Начнём отправлять сообщение */
	/* Сначала отправляем признак-разделитель начала пакета */
	SendFEND(ZPZ_UART);
	
	/* Теперь первые 8 байт  */
	for(uint8_t i = 0; i < 8; i++)
		UARTSendByte_by_SLIP (ZPZ_UART, ZPZ_Response.Buffer[i]);
	
	/* Отправляем информационную часть посылки */
	for(uint8_t i = 0; i < 9; i++)
		UARTSendByte_by_SLIP (ZPZ_UART, buffer[i]);
	
	/* После сверху посылаем контрольную сумму */
	for(uint8_t i = 8; i < 10; i++)
		UARTSendByte_by_SLIP (ZPZ_UART, ZPZ_Response.Buffer[i]);
	/* И в конце опять разделитель */
	SendFEND(ZPZ_UART);	
}



//-----------------------------------------Обслуживание команды CAN_TRANSMIT---------------------------------------------------------*/
static uint16_t ZPZ_Request_CAN_TRANSMIT (uint16_t CRC)
{
	/* Первые два байта - Адрес
	   Третий байт - размер
	   Остальные n байт - Данные */

	for(uint8_t i = 0; i < 3; i++)
	{
		buffer[i] = UARTReceiveByte_by_SLIP(ZPZ_UART);
	}
	for(uint8_t i = 3; i < buffer[2]+3; i++)
	{
		buffer[i] = UARTReceiveByte_by_SLIP(ZPZ_UART);
	}
	CRC = Crc16(buffer, 3 + buffer[2], CRC);

	return CRC;
}

static void ZPZ_Response_CAN_TRANSMIT (uint16_t NumPacket)
{
	ZPZ_Response_Union  ZPZ_Response;  // Стандартный ответ к ЗПЗ
	CAN_TxMsgTypeDef    CANTxMsg;
	TimeoutType         timeout;
	uint32_t            Buffer_number;
	
	/* Для отправки необходимо, чтобы CAN был запитан аппаратно  */
	/* Если шпилька 1 вставлена */
	if (SelfTesting_PIN1() == ST_OK)
	{
		/* Ответим, что аппаратно питания на CAN нет, значит дальше не идем */
		ZPZ_ShortResponse(CAN_TRANSMIT, NumPacket, CAN_DISABLE_BY_PIN1);
		/* И завершаемся с ошибкой */
		return;
	}
	/* Если реле питания БИМ выкл */
	if (SelfTesting_POW_BIM() != ST_OK)
	{
		/* Ответим, что аппаратно питания на CAN нет, значит дальше не идем */
		ZPZ_ShortResponse(CAN_TRANSMIT, NumPacket, CAN_DISABLE_BY_RELAY);
		/* И завершаемся с ошибкой */
		return;
	}
	
	/* Адрес - первые два байта */
	CANTxMsg.ID = CAN_STDID_TO_EXTID(*((uint16_t*)(buffer)));
	/* Длина сообщения начинается со второго байта */
	CANTxMsg.DLC = *((uint8_t*)(buffer+2));
	/* ID - Стандартный (не расширеный) */
	CANTxMsg.IDE = CAN_ID_STD;
	/* Отправка без приоритета */
	CANTxMsg.PRIOR_0 = DISABLE;
	/* Данные идут после 2 байт адреса и 1 байта длины (с 3-его байта) */
	CANTxMsg.Data[0] = *((uint32_t*)(buffer + 3));
	CANTxMsg.Data[1] = *((uint32_t*)(buffer + 7));

	/* Начинаем отправлять в CAN */
	/* Спросим какой из буферов CAN свободен для использования */
	Buffer_number = CAN_GetDisabledBuffer (ZPZ_CAN);
	/* Кладём сообщение в нужный буфер и ждем отправки */
	CAN_Transmit(ZPZ_CAN, Buffer_number, &CANTxMsg);
	/* Ожидаем конца передачи, либо превышения времени ожидания */
	setTimeout (&timeout, 4);
	while(((CAN_GetBufferStatus(ZPZ_CAN, Buffer_number) & CAN_STATUS_TX_REQ) != RESET) && (timeoutStatus(&timeout) != TIME_IS_UP)); 
	
	/* Вне зависимости от того, удалось отправить или нет, освобождаем буфер */
	CAN_BufferRelease (ZPZ_CAN, Buffer_number);
	
	/* Если превышено время на ожидание отправки  */
	if(timeout.status == TIME_IS_UP)
	{
		/* Ответим, что истекло время на отправку */
		ZPZ_ShortResponse(CAN_TRANSMIT, NumPacket, CAN_SENDING_TIMEOUT);
		/* И завершаемся с ошибкой */
		return;
	}

	/* Теперь нужно ответить */
	/* Заполняем структуру общей части всех пакетов */
	ZPZ_Response.Struct.Handler    = HANDLER_FROM_BUP;    // Заголовок BU
	/* 6 Байт обязательны + Длина сообщения  */ 
	ZPZ_Response.Struct.PacketSize = 6 + CANTxMsg.DLC;    // Размер пакета в байтах  
	ZPZ_Response.Struct.Command    = CAN_TRANSMIT;        // Команда: передача по CAN
	ZPZ_Response.Struct.Count      = NumPacket;	          // Номер пакета повторяем из запроса
	ZPZ_Response.Struct.Error      = SUCCES;              
	/* Теперь посчитаем контрольную сумму начала пакета (первые 8 байт)*/
	ZPZ_Response.Struct.CRC = Crc16(&ZPZ_Response.Buffer[0], 8, CRC16_INITIAL_FFFF);
	/* Начнём отправлять сообщение*/
	/* Сначала отправляем признак-разделитель начала пакета */
	SendFEND(ZPZ_UART);
	/* Теперь первые 8 байт */
	for(uint8_t i = 0; i < 8; i++)
		UARTSendByte_by_SLIP (ZPZ_UART, ZPZ_Response.Buffer[i]);
	
	/* Досчитаем контрольную сумму с учётом buff */
	ZPZ_Response.Struct.CRC = Crc16(buffer, 3 + CANTxMsg.DLC, ZPZ_Response.Struct.CRC);
	/* Отправляем содержимое buffer */
	for(uint8_t i = 0; i < 3 + CANTxMsg.DLC; i++)
		UARTSendByte_by_SLIP (ZPZ_UART, buffer[i]);
	
	/* После сверху посылаем контрольную сумму */
	for(uint8_t i = 8; i < 10; i++)
		UARTSendByte_by_SLIP (ZPZ_UART, ZPZ_Response.Buffer[i]);
	/* И в конце опять разделитель */
	SendFEND(ZPZ_UART);	
}


//-----------------------------------------Обслуживание режима "ВВПЗ"----------------------------------------------------------------*/
    
/****************************************************************** 
    Запуск режима выполнения высокоприоритетной задачи
******************************************************************/
static void ZPZ_startHighPriorityTask (void)
{
  PinConfigType pin = LED_READY;          /* Мигать будем светодиодом "Готов" */
  HighPriorityTask = ZPZ_HPT_MODE;	      /* Выставляем флаг режима High Priority Task */
  runIndication(&pin, 0.2*SECOND_TICKS);	/* Запускаем индикацию */
  LED_FAULT_OFF();	                      /* Выключаем "Неисправность" */
}
/****************************************************************** 
    Завершение режима выполнения высокоприоритетной задачи
******************************************************************/
static void ZPZ_finishHighPriorityTask (void)
{
	stopIndication();               /* Выключаем индикацию */
	HighPriorityTask = ZPZ_SC_MODE;	/* Выставляем флаг режима по-умолчанию - Single Command */
}

/****************************************************************** 
    Проверка занятости модуля высокоприоритетной задачей 
******************************************************************/
uint8_t ZPZ_getStatus (void)
{
	return HighPriorityTask;
}

