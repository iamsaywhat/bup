#include "ZPZ.h"

#include "MDR32F9Qx_port.h"
#include "MDR32F9Qx_rst_clk.h"
#include "MDR32F9Qx_uart.h"
#include "MDR32F9Qx_can.h"
#include "1636PP52Y.h"
#include "crc16.h"
#include "BIM.h"
#include "Log_FS/Log_FS.h"
#include "SNS.h"
#include "SWS.h"
#include "discrete_io.h"
#include "otherlib.h"
#include "SelfTesting.h"   
#include "bup_data_store.h"
#include "HeightMap/map_flash_layout.h"

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
} SLIP_DIV;

/****************************************************************** 
    Флаг занятости модуля высокоприоритетной задачей
******************************************************************/
uint8_t HighPriorityTask = ZPZ_SC_MODE;

/****************************************************************** 
    Счетчик таймаута при выполнении высокоприоритетной задачи
******************************************************************/
uint8_t TimeoutCounter = 0;


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
/*--------------------------------------------------------------------------------------------Обслуживание команды PIN_STATE-----------*/
static uint16_t ZPZ_Request_PIN_STATE(uint16_t CRC);
static void     ZPZ_Response_PIN_STATE(uint16_t NumPacket);
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
static void     ZPZ_StartHighPriorityTask (void);
static void     ZPZ_FinishHighPriorityTask (void);
static void     ZPZ_CheckpointHighPriorityTask (void);
void Timer2_IRQHandler(void);
/*-------------------------------------------------------------------------------------------------------------------------------------*/

/******************************************************************************************************

    Объявление публичных функций модуля ЗПЗ

******************************************************************************************************/

/**************************************************************************************************************
    ZPZ_RetargetPins - Функция переопределения UART2 на другие пины, для работы по ZPZ
**************************************************************************************************************/
void ZPZ_RetargetPins (void)
{
	RST_CLK_PCLKcmd(RST_CLK_PCLK_ZPZ_PORT , ENABLE);
	// Переназчаем UART на нужный порт для работы ZPZ
	Pin_init (ZPZ_PORT, ZPZ_RX, PORT_FUNC_OVERRID, PORT_OE_IN);
	Pin_init (ZPZ_PORT, ZPZ_TX, PORT_FUNC_OVERRID, PORT_OE_OUT);
}

/***************************************************************************************************************
    ZPZ_init - Запуск процедуры обмена по ZPZ
***************************************************************************************************************/
void ZPZ_init (void)
{
	UART_InitTypeDef UART_InitStructure;

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
    ZPZ_deinit - Деинициализация ЗПЗ
***************************************************************************************************************/
void ZPZ_deinit (void)
{
	// Сброс конфигуряции UART
	UART_DeInit(ZPZ_UART);
	// Отключение UART
	UART_Cmd(ZPZ_UART, DISABLE);
	Pin_init (ZPZ_PORT, ZPZ_RX, PORT_FUNC_PORT, PORT_OE_OUT);
	Pin_init (ZPZ_PORT, ZPZ_TX, PORT_FUNC_PORT, PORT_OE_OUT);
	// Отключаем таймер
	TIMER_Cmd(ZPZ_TIMER, DISABLE);
}


/**************************************************************************************************************
    ZPZ_ShortResponse - Отправка простого пакета подтвержений/ошибки к ЗПЗ
**************************************************************************************************************/
void ZPZ_ShortResponse(uint8_t Command, uint16_t Count, uint8_t Error)
{
	ZPZ_Response_Union   ZPZ_Response;
	TimeoutType          timeout;
	
	//Заполняем сткруктуру ответа
	ZPZ_Response.Struct.Handler     = HANDLER_BU;   // "UB" - Должно быть "BU", но выдача идём младшим байтом вперед
	ZPZ_Response.Struct.PacketSize  = 4;            // В размер входят только команда, счетчик, и ошибка
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
    ZPZ_WriteIntoCSnUnion - Функция записи производит объединение 4х микросхем SPI-памяти в единое 
                            адресное пространство 0x00000 - 0x7FFFF, используя драйвер "1636PP52Y.h".
                            Позволяет абстрагироваться от периферийного уровня и управления CSn.			
**************************************************************************************************************/
void ZPZ_WriteIntoCSnUnion(uint32_t Address, uint8_t* Source, uint32_t Size)
{	
	uint32_t  Bytecount = 0;

	while(Bytecount < Size)
	{
			if(((Address + Bytecount) < 0x20000))
				SPI_1636PP52Y_ByteProgram (SPI_1636PP52Y_CS1, Address + Bytecount, *Source++);
			
			else if (((Address + Bytecount) >= 0x20000)  &&  ((Address + Bytecount) < 0x40000))
				SPI_1636PP52Y_ByteProgram (SPI_1636PP52Y_CS2, (Address - 0x20000 + Bytecount), *Source++);
				
			else if (((Address + Bytecount) >= 0x40000)  &&  ((Address + Bytecount) < 0x60000))
				SPI_1636PP52Y_ByteProgram (SPI_1636PP52Y_CS3, (Address - 0x40000 + Bytecount), *Source++);
			
			else if (((Address + Bytecount) >= 0x60000)  &&  ((Address + Bytecount) < 0x80000))
				SPI_1636PP52Y_ByteProgram (SPI_1636PP52Y_CS4, (Address - 0x60000 + Bytecount), *Source++);

			Bytecount ++;
	}
}



/**************************************************************************************************************
    ZPZ_ReadIntoCSnUnion - Функция чтения производит объединение 4х микросхем SPI-памяти в единое
                            адресное пространство 0x00000 - 0x7FFFF, используя драйвер "1636PP52Y.h".
                            Позволяет абстрагироваться от периферийного уровня и управления CSn.			
**************************************************************************************************************/
void ZPZ_ReadIntoCSnUnion(uint32_t Address, uint8_t* Destination, uint32_t Size)
{	
	uint32_t  Bytecount = 0;
	
	while(Bytecount < Size)
	{
			if(((Address + Bytecount) < 0x20000))
			  SPI_1636PP52Y_ReadArray (SPI_1636PP52Y_CS1, Address + Bytecount, Destination++, 1);
			
			else if (((Address + Bytecount) >= 0x20000)  &&  ((Address + Bytecount) < 0x40000))
			  SPI_1636PP52Y_ReadArray (SPI_1636PP52Y_CS2, Address + Bytecount, Destination++, 1);
				
			else if (((Address + Bytecount) >= 0x40000)  &&  ((Address + Bytecount) < 0x60000))
			  SPI_1636PP52Y_ReadArray (SPI_1636PP52Y_CS3, Address + Bytecount, Destination++, 1);
			
			else if (((Address + Bytecount) >= 0x60000)  &&  ((Address + Bytecount) < 0x80000))
			  SPI_1636PP52Y_ReadArray (SPI_1636PP52Y_CS4, Address + Bytecount, Destination++, 1);

			Bytecount ++;
	}
}



/**************************************************************************************************************
    ZPZ_ChipEraseCSnUnion - Функция стирания всех установленых микросхем памяти, используется
                            драйвер "1636PP52Y.h".		
**************************************************************************************************************/
void ZPZ_ChipEraseCSnUnion(void)
{	
	SPI_1636PP52Y_ChipErase (SPI_1636PP52Y_CS1);
	SPI_1636PP52Y_ChipErase (SPI_1636PP52Y_CS2);
	SPI_1636PP52Y_ChipErase (SPI_1636PP52Y_CS3);
	SPI_1636PP52Y_ChipErase (SPI_1636PP52Y_CS4);
}	



/**************************************************************************************************************
    ZPZ_Service - Функция обслуживания команд от ЗПЗ (Основной режим)
**************************************************************************************************************/
uint8_t ZPZ_Service (void)
{
	
	ZPZ_BasePacket_Union  ZPZ_Base_Request;    // Сюда складывается приходящий запрос (кроме полей "данные")
	TimeoutType           timeout;             // Контроль превышения времени обработки
	uint16_t              crc;                 // Контрольная сумма		
	
	//Вычищаем FIFO от мусора и ждем пока не появится заголовок
	// while (UART_GetFlagStatus (ZPZ_UART, UART_FLAG_RXFE) != SET) UART_ReceiveData(ZPZ_UART);	
	
	// Ожидаем заголовок в течение заданного времени
	setTimeout (&timeout, 200);
	while(timeoutStatus(&timeout) != TIME_IS_UP)
	{
		// Сначала ловим FEND по SLIP протоколу
		if (UARTReceiveByte_by_SLIP(ZPZ_UART) != FEND) continue; 
		// Если первый байт заголовка не совпал, дальше не ждем, переходим к следующей итерации
		if (UARTReceiveByte_by_SLIP(ZPZ_UART) != HANDLER_P) continue; 
		// Если второй символ тоже совпал, значит покидаем данный цикл и переходим к приёму остального пакета
		if (UARTReceiveByte_by_SLIP(ZPZ_UART) == HANDLER_C) break;
	}	
	// Если был таймаут, значит связи нет, можно отключиться
	if(timeout.status == TIME_IS_UP)
		return ZPZ_TIMEOUT;
		
	// Заголовок обнаружен, далее приём синхронен
	ZPZ_Base_Request.Struct.Handler = HANDLER_PC;
	for(uint16_t i = 2; i < 7; i++)
	{
		// Принимаем остальное
		ZPZ_Base_Request.Buffer[i] = UARTReceiveByte_by_SLIP(ZPZ_UART);
	}
	// Считаем контрольную сумму начала сообщения
	crc = Crc16(&ZPZ_Base_Request.Buffer[0], 7, CRC16_INITIAL_FFFF);
	
	
	// Далее программа ветвится в зависимости от команды, информационная часть пакета будет 
	// обрабатываться разными подфункциями
	// Сценарии работы при получении всех возможных команд
	switch (ZPZ_Base_Request.Struct.Command)
	{
		case START_DOWNLOAD: // Начало загрузки полетного задания 
			
			// Этот процесс ресурсоемкий, поэтому будем выполнять его в режиме "ВВПЗ"
			ZPZ_StartHighPriorityTask ();
			// Принимаем данные запроса
			crc = ZPZ_Request_START_DOWNLOAD (crc);
		  break;
		
		case MAP_DOWNLOAD: // Продолжение загрузки полетного задания 
			
			// Отмечаемся в чекпоинт-функции "ВВПЗ", что мы не повисли
			ZPZ_CheckpointHighPriorityTask ();
			// Принимаем данные запроса
			crc = ZPZ_Request_MAP_DOWNLOAD (crc);
			break;
		
		case START_UPLOAD: // Начало выгрузки полетного задания 
			
		  // Этот процесс ресурсоемкий, поэтому будем выполнять его в режиме "ВВПЗ
			ZPZ_StartHighPriorityTask ();
			// Принимаем данные запроса
			crc = ZPZ_Request_START_UPLOAD (crc);
			break;
		
		case MAP_UPLOAD: // Продолжение выгрузки полетного задания 
			
			// Отмечаемся в чекпоинт-функции, что мы не повисли
			ZPZ_CheckpointHighPriorityTask ();
			// Принимаем данные запроса
			crc = ZPZ_Request_MAP_UPLOAD (crc);
			break;
		
		case CHECK_CONNECT:
			
			crc = ZPZ_Request_CHECK_CONNECT (crc);
			break;
		
		case BIM_CONTROL:
			
			crc = ZPZ_Request_BIM_CONTROL (crc);
		  break;
		
		case BIM_STATUS: 	
			
			crc = ZPZ_Request_BIM_STATUS (crc);	
			break;
		
		case LOG_FORMAT:
			
			crc = ZPZ_Request_LOG_FORMAT(crc);
		  break;
		
		case LOG_FILES:
			
			crc = ZPZ_Request_LOG_FILES (crc);
		  break;
			
		case LOG_UPLOAD:
		    
			// Этот процесс ресурсоемкий, поэтому будем выполнять его в режиме "ВВПЗ"
			// Определять начало процесса выгрузки логов будем по 0-му номеру пакета
			if(ZPZ_Base_Request.Struct.Count == 0)
			{
				// Запускаем режим "ВВПЗ" (а завершится он сам по таймауту)
				ZPZ_StartHighPriorityTask ();
			}
			else
				ZPZ_CheckpointHighPriorityTask ();
		  
			// Принимаем данные запроса
			crc = ZPZ_Request_LOG_UPLOAD(crc);
			break;
		
		case REQ_SWS:
			
			crc = ZPZ_Request_REQ_SWS(crc);
			break;
		
		case REQ_SNS_POS:
			
            crc = ZPZ_Request_REQ_SNS_POS(crc);
			break;
		
		case REQ_SNS_STATE:
			
			crc = ZPZ_Request_REQ_SNS_STATE(crc);
			break;
		
		case REQ_SNS_SETTINGS:
			
			crc = ZPZ_Request_REQ_SNS_SETTINGS(crc);
			break;
		
		case PIN_STATE:
			
			crc = ZPZ_Request_PIN_STATE(crc);
			break;
		
		case SYSTEM_STATE:
			
			crc = ZPZ_Request_SYSTEM_STATE (crc);
			break;
		
		case CAN_TRANSMIT:
			
			crc = ZPZ_Request_CAN_TRANSMIT (crc);
		  break;
		
		default:
			break;
	}
	

	
	// Обработка данных пакета завершена, все функции возращаются сюда
	// Принимаем контрольную сумму сообщения
	for(uint16_t i = 7; i < 9; i++)
	{
		ZPZ_Base_Request.Buffer[i] = UARTReceiveByte_by_SLIP(ZPZ_UART);
	}
	// Сверяем контрольные суммы
	if(crc != ZPZ_Base_Request.Struct.CRC)
	{
		//Если не сошлись - ошибка, отправляем ответ об ошибке
		ZPZ_ShortResponse(ZPZ_Base_Request.Struct.Command, ZPZ_Base_Request.Struct.Count, WRONG_CRC);
		return ZPZ_WRONG_CRC;
	}
	// Проверяем, что пакет завершился признаком FEND
	if(crc != ZPZ_Base_Request.Struct.CRC || UARTReceiveByte_by_SLIP(ZPZ_UART) != FEND)
	{
		//Если не завершился FEND, отправляем отчет с ошибкой
		ZPZ_ShortResponse(ZPZ_Base_Request.Struct.Command, ZPZ_Base_Request.Struct.Count, WITHOUT_CLOSING_FEND);
		return ZPZ_WRONG_CRC;
	}
	
	
	
	// Далее опять ветвление в зависимости от типа ответа (команды запроса)
	switch (ZPZ_Base_Request.Struct.Command)
	{
		case START_DOWNLOAD:
			
		  ZPZ_Response_START_DOWNLOAD (ZPZ_Base_Request.Struct.Count);
			break;
		
		case MAP_DOWNLOAD: // Продолжение загрузки полетного задания
		
			// Формируем ответ
			ZPZ_Response_MAP_DOWNLOAD  (ZPZ_Base_Request.Struct.Count);
			// Если это конец загрузки карты и задания (определяем как 400й пакет)
			if(ZPZ_Base_Request.Struct.Count == 400)
			{
				// Завершаем режим "ВВПЗ" командой:
				ZPZ_FinishHighPriorityTask ();
			}	
			break;
		
		case START_UPLOAD:
			ZPZ_Response_START_UPLOAD (ZPZ_Base_Request.Struct.Count);
			break;
		
		case MAP_UPLOAD: // Продолжение выгрузки полетного задания
		
			// Формируем ответ
			ZPZ_Response_MAP_UPLOAD (ZPZ_Base_Request.Struct.Count);
			// Если это конец выгрузки карты и задания (определяем как 400й пакет)
			if(ZPZ_Base_Request.Struct.Count == 400)
			{
				// Завершаем режим "ВВПЗ" командой:
				ZPZ_FinishHighPriorityTask ();
			}	
			break;
		
		case CHECK_CONNECT:
		  ZPZ_Response_CHECK_CONNECT (ZPZ_Base_Request.Struct.Count);
			break;
		
		case BIM_CONTROL:
			ZPZ_Response_BIM_CONTROL (ZPZ_Base_Request.Struct.Count);
			break;
		
		case BIM_STATUS: 
			ZPZ_Response_BIM_STATUS(ZPZ_Base_Request.Struct.Count);
			break;
		
		case LOG_FORMAT:
			ZPZ_Response_LOG_FORMAT (ZPZ_Base_Request.Struct.Count);
			break;
		
		case LOG_FILES:
			ZPZ_Response_LOG_FILES (ZPZ_Base_Request.Struct.Count);
			break;
		
		case LOG_UPLOAD:
			ZPZ_Response_LOG_UPLOAD(ZPZ_Base_Request.Struct.Count);
			break;
		
		case REQ_SWS:
			ZPZ_Response_REQ_SWS(ZPZ_Base_Request.Struct.Count);
			break;
		
		case REQ_SNS_POS:
			ZPZ_Response_REQ_SNS_POS(ZPZ_Base_Request.Struct.Count);
			break;
		
		case REQ_SNS_STATE:
			ZPZ_Response_REQ_SNS_STATE(ZPZ_Base_Request.Struct.Count);
			break;
		
		case REQ_SNS_SETTINGS:
			ZPZ_Response_REQ_SNS_SETTINGS (ZPZ_Base_Request.Struct.Count);
			break;
		
		case PIN_STATE:
			ZPZ_Response_PIN_STATE (ZPZ_Base_Request.Struct.Count);
			break; 
		
		case SYSTEM_STATE:
			ZPZ_Response_SYSTEM_STATE (ZPZ_Base_Request.Struct.Count);
			break;
		
		case CAN_TRANSMIT:
			ZPZ_Response_CAN_TRANSMIT (ZPZ_Base_Request.Struct.Count);
			break;
		
		default:
			break;
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
	ZPZ_ChipEraseCSnUnion(); 
	/* Записываем точку приземления и масштабы карты высот */
	ZPZ_WriteIntoCSnUnion(ADDRESS_0_PACKET, buffer, SIZE_OF_0_PACKET_DATA);
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
		ZPZ_WriteIntoCSnUnion(Address + count, &buffer[count], 1);
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
		ZPZ_WriteIntoCSnUnion(Address, buffer, 4);
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
	ZPZ_Response.Struct.Handler     = HANDLER_BU;           /* Заголовок BU                        */
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
		ZPZ_ReadIntoCSnUnion(ADDRESS_0_PACKET+i, buffer, 1);
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
	ZPZ_Response.Struct.Handler     = HANDLER_BU;       // Заголовок BU
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
		ZPZ_ReadIntoCSnUnion(Address + i, buffer, 1);
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
	uint16_t BIM_Side;
	uint8_t  status = 0;
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
	
	/* Отправляем запрос на управление с параметрами */
	status = BIM_SendRequest (BIM_Side, BIM_Data.Struct.State, BIM_Data.Struct.Position, 7, 255, 255);
	
	/* Теперь нужно ответить по ЗПЗ */
	if(status)
		ZPZ_ShortResponse(BIM_CONTROL, NumPacket, SUCCES);
	else 
		ZPZ_ShortResponse(BIM_CONTROL, NumPacket, BIM_CTRL_FAILED);
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
	uint8_t                       status = 0;          // Статус обмена с БИМ
	
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
	status = BIM_SendRequest(BIM_Side, BIM_CMD_REQ, 0, 0, 0, 0);
	/* Проверим состоялся ли обмен, и если нет, то ответим ошибкой */
	if(!status)
	{	
		ZPZ_ShortResponse(BIM_STATUS, NumPacket, BIM_CTRL_FAILED);
		return;
	}
	
	/* Заполняем информационные поля информацией от БИМ */
	ZPZ_BIM_Status.Struct.Side          = Side;
	ZPZ_BIM_Status.Struct.StatusFlags   = BIM_GetStatusFlags(BIM_Side);
	ZPZ_BIM_Status.Struct.Position      = BIM_GetStrapPosition (BIM_Side);
	ZPZ_BIM_Status.Struct.Voltage       = BIM_GetVoltage (BIM_Side);
	ZPZ_BIM_Status.Struct.Current       = BIM_GetCurrent (BIM_Side);
	ZPZ_BIM_Status.Struct.Speed         = BIM_GetSpeed (BIM_Side);
	
	
	/* Заполняем структуру базовых полей всех пакетов */
	ZPZ_Response.Struct.Handler    = HANDLER_BU;        // Заголовок BU
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
	ZPZ_Response.Struct.Handler    = HANDLER_BU;    // Заголовок BU
	ZPZ_Response.Struct.PacketSize = 0x05;          // Размер пакета 5 байт
	ZPZ_Response.Struct.Command    = LOG_FORMAT;    // Команда форматирования черного ящика
	ZPZ_Response.Struct.Count      = NumPacket;     // Номер пакета берем из запроса
	ZPZ_Response.Struct.Error      = SUCCES;        // Статус выполнения: Без ошибок
		
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
		LogFs_Formatting();
		/* После форматирование необходимо обновить информацию о файловой системе */
		LogFs_Info();
		
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
	ZPZ_Response.Struct.Handler    = HANDLER_BU;     // Заголовок BU
	ZPZ_Response.Struct.Command    = LOG_FILES;      // Команда: Запрос информации о содержимом
	ZPZ_Response.Struct.Count      = NumPacket;      // Номер пакета берем из запроса
	ZPZ_Response.Struct.Error      = SUCCES;         // Статус выполнения: Без ошибок
		
	/* Минимально возможное число байт пакета: 
	   Команда (1б) + Номер пакета (2б) + Error (1б) + Кол-во файлов (2б) = 6 байт
	   Остальное зависит от количества файлов в накопителе */
	ZPZ_Response.Struct.PacketSize = 6;
	/* Тогда определим количество файлов и прибавим (для каждого файла отводится 6 байт сообщения) */
	files = LogFs_GetFileNum();
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
			result = Log_Fs_FindFile(FIRST_FILE);
		/* При последующих итерациях нужно просто прокручивать файлы 
		   Командой "NEXT_FILE" */
		else 
			result = Log_Fs_FindFile(NEXT_FILE);
		
		/* Узнаем его номер файла и сразу упакуем в буфер для отправки */
		*(uint16_t*)buffer = Log_Fs_GetFileProperties (FILE_NUMBER);
		/* Подсчитываем контрольную сумму */
		ZPZ_Response.Struct.CRC = Crc16(buffer, 2, 	ZPZ_Response.Struct.CRC);
		/* Отправляем эти 2 байта информации о номере файла */
		for(uint8_t i = 0; i < 2; i++)
			UARTSendByte_by_SLIP (ZPZ_UART, buffer[i]);
		
		/* Узнаем его размер и сразу упакуем в буфер для отправки */
		*((uint32_t*)buffer) = Log_Fs_GetFileProperties (FILE_SIZE);
		/* Подсчитываем контрольную сумму */
		ZPZ_Response.Struct.CRC = Crc16(buffer, 4, 	ZPZ_Response.Struct.CRC);
		/* Отправляем эти 4 байта информации о размере файла */
		for(uint8_t i = 0; i < 4; i++)
			UARTSendByte_by_SLIP (ZPZ_UART, buffer[i]);
		
		/* Дополнительная проверка по выходу из цикла
		   Если все файлы были просмотрены, функция Log_Fs_FindFile() возвращает FS_ALL_FILES_SCROLLS */
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
	if(Log_Fs_FindFile_ByNum(fileNumber)!= FS_FINE)
	{
		/* Ответим ошибкой: Файл с таким номером не найден */
		ZPZ_ShortResponse(LOG_UPLOAD, 0, LOG_FS_FILE_NOT_FIND);
		return;
	}

	/* Узнаем размер файла */
  filesize = Log_Fs_GetFileProperties(FILE_SIZE);

	/* Заполняем структуру общей части всех пакетов */
	ZPZ_Response.Struct.Handler    = HANDLER_BU;       // Заголовок BU
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
				LogFs_ReadFile(buffer, offset + i, 1);
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
				LogFs_ReadFile(buffer, offset + i, 1);
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
	SWS_Packet_Type_Union SWS_data;        // Структура ответа от СВС
	TimeoutType           timeout;         // Таймаут контроль

	/* Принимаем данные */ 
	setTimeout (&timeout, 40);
	while(SWS_GetPacket (&SWS_data) && (timeoutStatus(&timeout) != TIME_IS_UP));
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
	ZPZ_Response.Struct.Handler    = HANDLER_BU;      // Заголовок BU
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
	BUP_UpdateDataFromSNS ();
	
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
	ZPZ_Response.Struct.Handler    = HANDLER_BU;          // Заголовок BU
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
		ZPZ_Response.Struct.CRC = Crc16(&SNS_Orientation.Buffer[i], 1, ZPZ_Response.Struct.CRC);
		/* И отправляем */
		UARTSendByte_by_SLIP (ZPZ_UART, SNS_Position.Buffer[i]);
	}
	
	/* Высылаем данные ориентации
	   из полученного пакета с ориентацией от СНС выкидываем CRC, 
		 тип ответа и счетчик в начале, поэтому i = 4 */
	for(uint8_t i = 4; i < SizeAnsODR; i++)
	{
		/* Побайтово досчитываем контрольную сумму */
		ZPZ_Response.Struct.CRC = Crc16(&SNS_Orientation.Buffer[i], 1, ZPZ_Response.Struct.CRC);
		/* И отправляем */
		UARTSendByte_by_SLIP (ZPZ_UART, SNS_Orientation.Buffer[i]);
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
	ZPZ_Response_Union                     ZPZ_Response;          // Стандартный ответ к ЗПЗ
	SNS_Device_Information_Response_Union  SNS_DevInfo;           // Информация о СНС
	SNS_Available_Data_Response_Union      SNS_AvailableData;     // Доступные данные 
	TimeoutType                            timeout;               // Таймаут-контроль
	
	/* Получим данные об устройстве */
	setTimeout (&timeout, 100);
	while(SNS_GetDeviceInformation(&SNS_DevInfo) != SNS_OK && (timeoutStatus(&timeout) != TIME_IS_UP));
	
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
	while(SNS_GetDataState(&SNS_AvailableData) != SNS_OK && (timeoutStatus(&timeout) != TIME_IS_UP));
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
	ZPZ_Response.Struct.Handler    = HANDLER_BU;         // Заголовок BU
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
		ZPZ_Response.Struct.CRC = Crc16(&SNS_DevInfo.Buffer[i], 1, ZPZ_Response.Struct.CRC);
		/* И отправляем */
		UARTSendByte_by_SLIP (ZPZ_UART, SNS_DevInfo.Buffer[i]);
	}

	/* Высылаем инфу о доступных данных 
	   из полученного пакета с доступными данными от СНС необходимо только поле Data_State
	   Побайтово досчитываем контрольную сумму */
	ZPZ_Response.Struct.CRC = Crc16(&SNS_AvailableData.Struct.Data_State, 1, ZPZ_Response.Struct.CRC);
	/* И отправляем */
	UARTSendByte_by_SLIP (ZPZ_UART, SNS_AvailableData.Struct.Data_State);
	
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
		SNS_EnableHorizontalCorrection();
	
	/* Команда отключения горизонтальной коррекции */
	else if (buffer[0] == 0x02) 
		SNS_DisableHorizontalCorrection();
	
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
		SNS_StartGyroCalibration();
	
	/* Команда сброса калибровки гироскопа */
	else if (buffer[1] == 0x02)
		SNS_ResetGyroCalibration();
	
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
		SNS_StartMagnetometerCalibration();
	
	/* Команда сброса калибровки магнитометра */
	else if (buffer[2] == 0x02)
		SNS_ResetMagnetometerCalibration();

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
	ZPZ_Response.Struct.Handler    = HANDLER_BU;          // Заголовок BU
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



//-----------------------------------------Обслуживание команды PIN_STATE---------------------------------------------------------*/
static uint16_t ZPZ_Request_PIN_STATE(uint16_t CRC)
{
	return ZPZ_RequestWithEmptyData(CRC);
}

static void ZPZ_Response_PIN_STATE (uint16_t NumPacket)
{
	ZPZ_BasePacket_Union     ZPZ_BaseResponse;
	
	/* Узнаем состояние шпилек */
	buffer[0] = 0; /* Первый байт пустой */
	buffer[1] = 0; 
	buffer[2] = 0; 
	/* Определим состояние дискретов */
	buffer[1] = ((~PIN1_CHECK)&0x01)| 
	            (((~PIN2_DIR_CHECK)&0x01) << 1)|
							(BLIND_CHECK << 2)|
							(((~PIN2_INV_CHECK)&0x01) << 3);
	buffer[2] = PYRO_CHECK |
	            (BLIND_CTRL_CHECK << 1)|
	            (RELAY_BIM_CHECK << 2)|
	            (LED_FAULT_CHECK << 6)|
	            (LED_READY_CHECK << 7);
	/* Теперь нужно ответить */
	/* Заполняем структуру общей части всех пакетов */
	ZPZ_BaseResponse.Struct.Handler    = HANDLER_BU;
	ZPZ_BaseResponse.Struct.PacketSize = 6;
	ZPZ_BaseResponse.Struct.Command    = PIN_STATE;
	ZPZ_BaseResponse.Struct.Count      = NumPacket;	
	/* Теперь посчитаем контрольную сумму начала пакета (первые 7 байт) */
	ZPZ_BaseResponse.Struct.CRC = Crc16(&ZPZ_BaseResponse.Buffer[0], 7, CRC16_INITIAL_FFFF);
	/* Начнём отправлять сообщение*/
	/* Сначала отправляем признак-разделитель начала пакета */
	SendFEND(ZPZ_UART);
	/* Теперь первые 7 байт */
	for(uint8_t i = 0; i < 7; i++)
		UARTSendByte_by_SLIP (ZPZ_UART, ZPZ_BaseResponse.Buffer[i]);
	
	/* Досчитаем контрольную сумму с учётом buff */
	ZPZ_BaseResponse.Struct.CRC = Crc16(buffer, 3, ZPZ_BaseResponse.Struct.CRC);
	/* Отправляем содержимое buffer*/
	for(uint8_t i = 0; i < 3; i++)
		UARTSendByte_by_SLIP (ZPZ_UART, buffer[i]);
	
	/* После сверху посылаем контрольную сумму */
	for(uint8_t i = 0; i < 2; i++)
		UARTSendByte_by_SLIP (ZPZ_UART, ZPZ_BaseResponse.Buffer[i]);
	
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
	ZPZ_Response.Struct.Handler    = HANDLER_BU;          // Заголовок BU       
	ZPZ_Response.Struct.PacketSize = 13;                  // Размер пакета в байтах  
	ZPZ_Response.Struct.Command    = SYSTEM_STATE;        // Команда: передача по CAN
	ZPZ_Response.Struct.Count      = NumPacket;	          // Номер пакета повторяем из запроса
	ZPZ_Response.Struct.Error      = SUCCES;              // Статус: без ошибок
	
	/* Теперь посчитаем контрольную сумму начала пакета (первые 8 байт) */
	ZPZ_Response.Struct.CRC = Crc16(&ZPZ_Response.Buffer[0], 8, CRC16_INITIAL_FFFF);
	
	/* Кладём состояние системы в буфер */
  *((uint16_t*)buffer)            = SystemState;
  /* Кладём заряд батареи */
  *((float*)((uint8_t*)buffer+2)) = BUP_DataStorage.Battery50V; 
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
	ZPZ_Response.Struct.Handler    = HANDLER_BU;          // Заголовок BU
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
    Обработчик прерываний от таймера ZPZ
******************************************************************/
void Timer2_IRQHandler(void)
{
	/* Сбрасываем флаг прерываний */
	TIMER_ClearFlag(ZPZ_TIMER,TIMER_STATUS_CNT_ARR);	
	/* Делаем инкремент таймаут-счетчика */
	TimeoutCounter++;
	
	/* Сменяем состояние индикатора "READY" */
	if (LED_READY_CHECK) 
		LED_READY_OFF();
	else 
		LED_READY_ON();
	
	/* Здесь реализуем таймаут контроль режима ВВПЗ */
	/* Повисание или разрыв связи будем определять так: */
	if(ZPZ_CheckHighPriorityTask() && TimeoutCounter > 10)
	{
		/* Завершим режим ВППЗ и перейдем в режим РК */
		ZPZ_FinishHighPriorityTask ();
	}
}


/****************************************************************** 
    Запуск режима выполнения высокоприоритетной задачи
******************************************************************/
static void ZPZ_StartHighPriorityTask (void)
{
	/* Выставляем флаг режима High Priority Task */
	HighPriorityTask = ZPZ_HPT_MODE;
	/* Сбрасываем таймаут */
	TimeoutCounter  = 0;
	/* Выключаем "Неисправность" */
	LED_FAULT_OFF();
	/* Запускаем таймер */
	Timer_init(ZPZ_TIMER, 0.2*SECOND_TICKS);
}


/****************************************************************** 
    Завершение режима выполнения высокоприоритетной задачи
******************************************************************/
static void ZPZ_FinishHighPriorityTask (void)
{
	/* Отключаем таймер */
	TIMER_Cmd(ZPZ_TIMER, DISABLE);
	/* Выключаем "Готов" */
	LED_READY_OFF();
	/* Выставляем флаг режима по-умолчанию - Single Command */
	HighPriorityTask = ZPZ_SC_MODE;
	/* Сбрасываем таймаут */
	TimeoutCounter  = 0;
}

/****************************************************************** 
    Чекпоинт режима выполнения высокоприоритетной задачи      

    Примечание: Эту функция используется для защиты от 
    повисаний режима выполнения высокоприоритетной задачи.
    Если данная функция не вызывается некоторое время, то
    данный режим завершается, автоматически.
******************************************************************/
static void ZPZ_CheckpointHighPriorityTask (void)
{
	/* При чекпоинте нам просто необходимо сбросить счетчик таймаута */
	/* Принцип работы WDT */
	/* Если счетчик переполнится, то режим "ВВЗ" завершится */
	TimeoutCounter  = 0;
}


/****************************************************************** 
    Проверка занятости модуля высокоприоритетной задачей 
******************************************************************/
uint8_t ZPZ_CheckHighPriorityTask (void)
{
	return HighPriorityTask;
}
