#include "ZPZ.h"

#include "MDR32F9Qx_port.h"
#include "MDR32F9Qx_rst_clk.h"
#include "MDR32F9Qx_uart.h"
#include "MDR32F9Qx_can.h"
#include "1636PP52Y.h"
#include "crc16.h"
#include "BIM.h"
#include "Log_FS.h"
#include "SNS.h"
#include "SWS.h"
#include "discrete_io.h"
#include "otherlib.h"
#include "SelfTesting.h"   
#include "bup_data_store.h"


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
uint16_t ZPZ_RequestWithEmptyData(uint16_t CRC); 
/*--------------------------------------------------------------------------------------------Обслуживание команды CHECK_CONNECT-------*/
uint16_t ZPZ_Request_CHECK_CONNECT (uint16_t CRC);
void ZPZ_Response_CHECK_CONNECT (uint16_t NumPacket);
/*--------------------------------------------------------------------------------------------Обслуживание команды START_DOWNLOAD------*/
uint16_t ZPZ_Request_START_DOWNLOAD (uint16_t CRC);
void ZPZ_Response_START_DOWNLOAD (uint16_t NumPacket);
/*--------------------------------------------------------------------------------------------Обслуживание команды MAP_DOWNLOAD--------*/
uint16_t ZPZ_Request_MAP_DOWNLOAD (uint16_t CRC, uint16_t NumPacket);
void ZPZ_Response_MAP_DOWNLOAD (uint16_t NumPacket);
/*--------------------------------------------------------------------------------------------Обслуживание команды START_UPLOAD--------*/
uint16_t ZPZ_Request_START_UPLOAD (uint16_t CRC);
void ZPZ_Response_START_UPLOAD (uint16_t NumPacket); 
/*--------------------------------------------------------------------------------------------Обслуживание команды MAP_UPLOAD----------*/
uint16_t ZPZ_Request_MAP_UPLOAD (uint16_t CRC);
void ZPZ_Response_MAP_UPLOAD (uint16_t NumPacket);
/*--------------------------------------------------------------------------------------------Обслуживание команды BIM_CONTROL---------*/
uint16_t ZPZ_Request_BIM_CONTROL (uint16_t CRC, ZPZ_RequestControlBIM_Union* BIM_Data);
void ZPZ_Response_BIM_CONTROL (ZPZ_RequestControlBIM_Union* BIM_Data, uint16_t NumPacket);
/*--------------------------------------------------------------------------------------------Обслуживание команды BIM_STATUS----------*/
uint16_t ZPZ_Request_BIM_STATUS (uint16_t CRC, ZPZ_RequestControlBIM_Union* BIMControl);
void ZPZ_Response_BIM_STATUS(uint8_t Side, uint16_t NumPacket);
/*--------------------------------------------------------------------------------------------Обслуживание команды LOG_FORMAT----------*/
uint16_t ZPZ_Request_LOG_FORMAT (uint16_t CRC, uint8_t* Subcommand);
void ZPZ_Response_LOG_FORMAT (uint16_t NumPacket, uint8_t Subcommand);
/*--------------------------------------------------------------------------------------------Обслуживание команды LOG_FILES-----------*/
uint16_t ZPZ_Request_LOG_FILES (uint16_t CRC);
void ZPZ_Response_LOG_FILES (uint16_t NumPacket);
/*--------------------------------------------------------------------------------------------Обслуживание команды LOG_UPLOAD----------*/
uint16_t ZPZ_Request_LOG_UPLOAD(uint16_t CRC, uint16_t* NUM);
uint8_t ZPZ_Response_LOG_UPLOAD(uint16_t File_num, uint16_t NumPacket);
/*--------------------------------------------------------------------------------------------Обслуживание команды REQ_SWS-------------*/
uint16_t ZPZ_Request_REQ_SWS (uint16_t CRC);
uint8_t ZPZ_Response_REQ_SWS (uint16_t NumPacket);
/*--------------------------------------------------------------------------------------------Обслуживание команды REQ_SNS_POS---------*/
uint16_t ZPZ_Request_REQ_SNS_POS (uint16_t CRC);
uint8_t ZPZ_Response_REQ_SNS_POS (uint16_t NumPacket);
/*--------------------------------------------------------------------------------------------Обслуживание команды REQ_SNS_STATE-------*/
uint16_t ZPZ_Request_REQ_SNS_STATE (uint16_t CRC);
uint8_t ZPZ_Response_REQ_SNS_STATE (uint16_t NumPacket);
/*--------------------------------------------------------------------------------------------Обслуживание команды REQ_SNS_SETTINGS----*/
uint16_t ZPZ_Request_REQ_SNS_SETTINGS(uint16_t CRC, uint8_t* buffer);
uint8_t ZPZ_Response_REQ_SNS_SETTINGS (uint8_t* params, uint16_t NumPacket);
/*--------------------------------------------------------------------------------------------Обслуживание команды PIN_STATE-----------*/
uint16_t ZPZ_Request_PIN_STATE(uint16_t CRC);
void ZPZ_Response_PIN_STATE (uint16_t NumPacket);
/*--------------------------------------------------------------------------------------------Обслуживание команды SYSTEM_STATE--------*/
uint16_t ZPZ_Request_SYSTEM_STATE (uint16_t CRC);
void ZPZ_Response_SYSTEM_STATE (uint16_t NumPacket);
/*--------------------------------------------------------------------------------------------Обслуживание команды CAN_TRANSMIT--------*/
uint16_t ZPZ_Request_CAN_TRANSMIT (uint16_t CRC, uint8_t* buffer);
void ZPZ_Response_CAN_TRANSMIT (uint8_t* buffer, uint16_t NumPacket);
/*--------------------------------------------------------------------------------------------Обёртка UART функций со SLIP-------------*/
uint16_t UARTReceiveByte_by_SLIP(MDR_UART_TypeDef* UARTx, uint32_t TimeoutRange);
int16_t UARTSendByte_by_SLIP (MDR_UART_TypeDef* UARTx, uint32_t TimeoutRange, uint16_t Byte);
int16_t SendFEND (MDR_UART_TypeDef* UARTx, uint32_t TimeoutRange);
/*--------------------------------------------------------------------------------------------Режим "ВВПЗ"-----------------------------*/
void ZPZ_StartHighPriorityTask (void);
void ZPZ_FinishHighPriorityTask (void);
void ZPZ_CheckpointHighPriorityTask (void);
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
	Pin_Init (ZPZ_PORT, ZPZ_RX, PORT_FUNC_OVERRID, PORT_OE_IN);
	Pin_Init (ZPZ_PORT, ZPZ_TX, PORT_FUNC_OVERRID, PORT_OE_OUT);
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
	Pin_Init (ZPZ_PORT, ZPZ_RX, PORT_FUNC_PORT, PORT_OE_OUT);
	Pin_Init (ZPZ_PORT, ZPZ_TX, PORT_FUNC_PORT, PORT_OE_OUT);
	// Отключаем таймер
	TIMER_Cmd(ZPZ_TIMER, DISABLE);
}


/**************************************************************************************************************
    ZPZ_ShortResponse - Отправка простого пакета подтвержений/ошибки к ЗПЗ
**************************************************************************************************************/
void ZPZ_ShortResponse(uint8_t Command, uint16_t Count, uint8_t Error)
{
	ZPZ_Response_Union   ZPZ_Response;
	uint16_t timeout = 0;
	uint8_t  i;
	
	//Заполняем сткруктуру ответа
	ZPZ_Response.Struct.Handler     = HANDLER_BU;   // "UB" - Должно быть "BU", но выдача идём младшим байтом вперед
	ZPZ_Response.Struct.PacketSize  = 4;            // В размер входят только команда, счетчик, и ошибка
	ZPZ_Response.Struct.Command     = Command;
	ZPZ_Response.Struct.Count       = Count;
	ZPZ_Response.Struct.Error       = Error;
	ZPZ_Response.Struct.CRC         = Crc16(&ZPZ_Response.Buffer[0],8, CRC16_INITIAL_FFFF);
	
	// Если FIFO передатчика заполнен, то ждем пока освободится
	while ((UART_GetFlagStatus (ZPZ_UART, UART_FLAG_TXFF) == SET) && (timeout != 0xFFF)){ timeout++;}
	// Посылаем признак начала пакета
	SendFEND(ZPZ_UART, ZPZ_SEND_BYTE_TIMEOUT);
	
	// Теперь остальную часть пакета
	for(i = 0; i < 10; i++)
	{
		UARTSendByte_by_SLIP (ZPZ_UART, ZPZ_SEND_BYTE_TIMEOUT, ZPZ_Response.Buffer[i]);
	}
	// Ждем завершения передачи
	while ((UART_GetFlagStatus (ZPZ_UART, UART_FLAG_TXFF) == SET) && (timeout != 0xFFF)) 
		timeout++;
	// Посылаем признак конца пакета
	
	SendFEND(ZPZ_UART, ZPZ_SEND_BYTE_TIMEOUT);
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
	
	ZPZ_BasePacket_Union          ZPZ_Base_Request;  // Сюда складывается приходящий запрос (кроме полей "данные")
	ZPZ_RequestControlBIM_Union   BIM_Control;       // Структура для обслуживания команд BIM_CONTROL и BIM_STATUS
	
	uint32_t timeout = 0;             // Контроль превышения времени обработки
	uint16_t crc, i;                  // Контрольная сумма и счетчик циклов
	uint16_t Log_files = 0;           // Номер запрашиваемого файла (обслуж. команды LOG_UPLOAD)
	uint8_t  SNS_opt[3];              // Буфер для обслуживания команды REQ_SNS_SETTINGS
	uint8_t  CAN_buff[11];            // Буфер для обслуживания команды CAN_TRANSMIT
	uint8_t  LOG_FS_Subcommand = 0;   // Подкоманда для форматирования "черного ящика"
		
	
	//Вычищаем FIFO от мусора и ждем пока не появится заголовок
	//while (UART_GetFlagStatus (ZPZ_UART, UART_FLAG_RXFE) != SET) UART_ReceiveData(ZPZ_UART);	
	
	// Ожидаем заголовок. Пытаемся 30 раз словить начало пакета.
	while(timeout !=  100)
	{
		timeout ++;
		// Сначала ловим FEND по SLIP протоколу
		if (UARTReceiveByte_by_SLIP(ZPZ_UART, ZPZ_RECEIVE_BYTE_TIMEOUT) != FEND) continue; 
		// Если первый байт заголовка не совпал, дальше не ждем, переходим к следующей итерации
		if (UARTReceiveByte_by_SLIP(ZPZ_UART, ZPZ_RECEIVE_BYTE_TIMEOUT) != HANDLER_P) continue; 
		// Если второй символ тоже совпал, значит покидаем данный цикл и переходим к приёму остального пакета
		if (UARTReceiveByte_by_SLIP(ZPZ_UART, ZPZ_RECEIVE_BYTE_TIMEOUT) == HANDLER_C) break;
	}	
	
	// Если был таймаут, значит связи нет, можно отключиться
	if(timeout == 100) 
		return ZPZ_TIMEOUT;
		
	// Заголовок обнаружен, далее приём синхронен
	ZPZ_Base_Request.Struct.Handler = HANDLER_PC;
	for(i = 2; i < 7; i++)
	{
		// Принимаем остальное
		ZPZ_Base_Request.Buffer[i] = UARTReceiveByte_by_SLIP(ZPZ_UART, ZPZ_RECEIVE_BYTE_TIMEOUT);
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
			crc = ZPZ_Request_MAP_DOWNLOAD (crc, ZPZ_Base_Request.Struct.Count);
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
			
			crc = ZPZ_Request_BIM_CONTROL (crc, &BIM_Control);
		  break;
		
		case BIM_STATUS: 	
			
			crc = ZPZ_Request_BIM_STATUS (crc, &BIM_Control);	
			break;
		
		case LOG_FORMAT:
			
			crc = ZPZ_Request_LOG_FORMAT(crc, &LOG_FS_Subcommand);
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
			crc = ZPZ_Request_LOG_UPLOAD(crc, &Log_files);
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
			
			crc = ZPZ_Request_REQ_SNS_SETTINGS(crc, SNS_opt);
			break;
		
		case PIN_STATE:
			
			crc = ZPZ_Request_PIN_STATE(crc);
			break;
		
		case SYSTEM_STATE:
			
			crc = ZPZ_Request_SYSTEM_STATE (crc);
			break;
		
		case CAN_TRANSMIT:
			
			crc = ZPZ_Request_CAN_TRANSMIT (crc, CAN_buff);
		  break;
		
		default:
			break;
	}
	

	
	// Обработка данных пакета завершена, все функции возращаются сюда
	// Принимаем контрольную сумму сообщения
	for(i = 7; i < 9; i++)
	{
		ZPZ_Base_Request.Buffer[i] = UARTReceiveByte_by_SLIP(ZPZ_UART, ZPZ_RECEIVE_BYTE_TIMEOUT);
	}
	// Сверяем контрольные суммы
	if(crc != ZPZ_Base_Request.Struct.CRC)
	{
		//Если не сошлись - ошибка, отправляем ответ об ошибке
		ZPZ_ShortResponse(ZPZ_Base_Request.Struct.Command, ZPZ_Base_Request.Struct.Count, WRONG_CRC);
		return ZPZ_WRONG_CRC;
	}
	// Проверяем, что пакет завершился признаком FEND
	if(crc != ZPZ_Base_Request.Struct.CRC || UARTReceiveByte_by_SLIP(ZPZ_UART, ZPZ_RECEIVE_BYTE_TIMEOUT) != FEND)
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
			ZPZ_Response_BIM_CONTROL (&BIM_Control, ZPZ_Base_Request.Struct.Count);
			break;
		
		case BIM_STATUS: 
			ZPZ_Response_BIM_STATUS(BIM_Control.Struct.Side,ZPZ_Base_Request.Struct.Count);
			break;
		
		case LOG_FORMAT:
			ZPZ_Response_LOG_FORMAT (ZPZ_Base_Request.Struct.Count, LOG_FS_Subcommand);
			break;
		
		case LOG_FILES:
			ZPZ_Response_LOG_FILES (ZPZ_Base_Request.Struct.Count);
			break;
		
		case LOG_UPLOAD:
			ZPZ_Response_LOG_UPLOAD(Log_files, ZPZ_Base_Request.Struct.Count);
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
			ZPZ_Response_REQ_SNS_SETTINGS (SNS_opt, ZPZ_Base_Request.Struct.Count);
			break;
		
		case PIN_STATE:
			ZPZ_Response_PIN_STATE (ZPZ_Base_Request.Struct.Count);
			break; 
		
		case SYSTEM_STATE:
			ZPZ_Response_SYSTEM_STATE (ZPZ_Base_Request.Struct.Count);
			break;
		
		case CAN_TRANSMIT:
			ZPZ_Response_CAN_TRANSMIT (CAN_buff, ZPZ_Base_Request.Struct.Count);
			break;
		
		default:
			break;
	}
	
	return ZPZ_OK;
	
}



/*--------------------------------------------------------------------------------------------Обёртка UART функций со SLIP-------------*/
// Отправка FEND разделителя, с таймаутом на время отправки 
int16_t SendFEND(MDR_UART_TypeDef* UARTx, uint32_t TimeoutRange)
{
	uint32_t timeout = 0;
	// Если FIFO передатчика заполнен, то ждем пока освободится
	while ((UART_GetFlagStatus (UARTx, UART_FLAG_TXFF) == SET) && (timeout != TimeoutRange)) timeout++;
	UART_SendData(ZPZ_UART, FEND);
	// Если выход из ожидания по таймауту - возвращаем ошибку и выходим
	if(timeout == TimeoutRange) 
		return 0xFF;
	else 
		return 0;
}

// Отправка символа по SLIP - протоколу
int16_t UARTSendByte_by_SLIP (MDR_UART_TypeDef* UARTx, uint32_t TimeoutRange, uint16_t Byte)
{
	uint32_t timeout = 0;
	
	if(Byte == FEND || Byte == FESC)
	{
		// Если FIFO передатчика заполнен, то ждем пока освободится
		while ((UART_GetFlagStatus (UARTx, UART_FLAG_TXFF) == SET) && (timeout != TimeoutRange)) timeout++;
		// Если выход из ожидания по таймауту - возвращаем ошибку и выходим
		if(timeout == TimeoutRange) return 0xFF;
		// Иначе отправляем вместо FEND или FESC сначала FESC
		UART_SendData(UARTx, FESC);
		// И ждем пока FIFO освободится
		while ((UART_GetFlagStatus (UARTx, UART_FLAG_TXFF) == SET) && (timeout != TimeoutRange)) timeout++;
		
		// Если нужно отправить FEND, то после FESC посылаем TFEND
		if(Byte == FEND)
		{
			// Если FIFO передатчика заполнен, то ждем пока освободится
			while ((UART_GetFlagStatus (UARTx, UART_FLAG_TXFF) == SET) && (timeout != TimeoutRange)) timeout++;
			// Если выход из ожидания по таймауту - возвращаем ошибку и выходим
			if(timeout == TimeoutRange) 
				return 0xFF;
			// Иначе отправляем вместо FEND или FESC сначала FESC
			UART_SendData(UARTx, TFEND);
			// И ждем пока FIFO освободится
		  while ((UART_GetFlagStatus (UARTx, UART_FLAG_TXFF) == SET) && (timeout != TimeoutRange)) timeout++;
		}
		// Если нужно отправить FESC, то после FESC посылаем еще и TFESC
		else if (Byte == FESC)
		{
			// Если FIFO передатчика заполнен, то ждем пока освободится
			while ((UART_GetFlagStatus (UARTx, UART_FLAG_TXFF) == SET) && (timeout != TimeoutRange)) timeout++;
			// Если выход из ожидания по таймауту - возвращаем ошибку и выходим
			if(timeout == TimeoutRange) 
				return 0xFF;
			// Иначе отправляем вместо FEND или FESC сначала FESC
			UART_SendData(UARTx, TFESC);
			// И ждем пока FIFO освободится
		  while ((UART_GetFlagStatus (UARTx, UART_FLAG_TXFF) == SET) && (timeout != TimeoutRange)) timeout++;
		}
	}
	else 
	{
		// Если FIFO передатчика заполнен, то ждем пока освободится
		while ((UART_GetFlagStatus (UARTx, UART_FLAG_TXFF) == SET) && (timeout != TimeoutRange)) timeout++;
		// Если выход из ожидания по таймауту - возвращаем ошибку и выходим
		if(timeout == TimeoutRange) 
			return 0xFF;
		// Иначе отправляем исходный символ
		UART_SendData(UARTx, Byte);
		// И ждем пока FIFO освободится
		while ((UART_GetFlagStatus (UARTx, UART_FLAG_TXFF) == SET) && (timeout != TimeoutRange)) timeout++;
	}
		
		return 0;
}

// Приём и декодирование пакета по SLIP Протоколу 
uint16_t UARTReceiveByte_by_SLIP(MDR_UART_TypeDef* UARTx, uint32_t TimeoutRange)
{
	uint16_t Byte = 0;
	uint32_t timeout = 0;
	
	// Ждем прихода информации
	while ((UART_GetFlagStatus (UARTx, UART_FLAG_RXFE) == SET) && (timeout != TimeoutRange)) timeout++;
	// Если выход из ожидания по таймауту - возвращаем ошибку и выходим
	if(timeout == TimeoutRange) 
		return 0xFF;
	// Иначе принимаем байт
	Byte = UART_ReceiveData(UARTx);
	
	// Если поймали символ FESC, то смотрим каким будет следующий
	if(Byte == FESC)
	{
		// Ждем прихода информации
		while ((UART_GetFlagStatus (UARTx, UART_FLAG_RXFE) == SET) && (timeout != TimeoutRange)) timeout++;
		// Если выход из ожидания по таймауту - возвращаем ошибку и выходим
		if(timeout == TimeoutRange) 
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
uint16_t ZPZ_RequestWithEmptyData(uint16_t CRC) 
{
	uint8_t temp;
	uint16_t crc;
	// Это простой запрос:
	// Тут следует только один пустой байт (=0), перед CRC
	// Просто примем его прям тут и посчитаем CRC с его учетом
	temp = UARTReceiveByte_by_SLIP(ZPZ_UART, ZPZ_RECEIVE_BYTE_TIMEOUT);
	crc = Crc16(&temp, 1, CRC);
	
	return crc;
}



//-----------------------------------------Обслуживание команды CHECK_CONNECT---------------------------------------------------------*/
uint16_t ZPZ_Request_CHECK_CONNECT (uint16_t CRC)
{
	return ZPZ_RequestWithEmptyData(CRC);
}

void ZPZ_Response_CHECK_CONNECT (uint16_t NumPacket)
{
	ZPZ_ShortResponse(CHECK_CONNECT, NumPacket, SUCCES);
}



//-----------------------------------------Обслуживание команды START_DOWNLOAD---------------------------------------------------------*/
uint16_t ZPZ_Request_START_DOWNLOAD (uint16_t CRC) 
{
	uint16_t crc;
	uint8_t  temp;
	uint16_t i;
	uint8_t  buffer[PACKET0_DATASIZE];
	
	// Приём пустого байта перед информацией (фиктивное считывание)
	temp = UARTReceiveByte_by_SLIP(ZPZ_UART, ZPZ_RECEIVE_BYTE_TIMEOUT);
	// Сразу же подсчитаем изменения в контрольной сумме
	crc = Crc16(&temp, 1, CRC);
	
	// Далее принимаем полезную часть информации и сразу пишем во внешнюю SPI память
	for(i = 0; i < PACKET0_DATASIZE; i++)
	{
		// Принимаем байт информации
		buffer[i] = UARTReceiveByte_by_SLIP(ZPZ_UART, ZPZ_RECEIVE_BYTE_TIMEOUT);
		// Пересчитываем контрольную сумму
		crc = Crc16(&buffer[i], 1, crc);
	}
	
	// Очищаем память под полетное задание
	ZPZ_ChipEraseCSnUnion();
	// Записываем точку приземления и масштабы карты высот
	ZPZ_WriteIntoCSnUnion(PACKET0_ADR, buffer, PACKET0_DATASIZE);
	
	// Возвращаем контрольную сумму обратно в основную функцию для дорасчета
	return crc;
}

void ZPZ_Response_START_DOWNLOAD (uint16_t NumPacket)
{
	ZPZ_ShortResponse(START_DOWNLOAD, NumPacket, SUCCES);
}



//-----------------------------------------Обслуживание команды MAP_DOWNLOAD---------------------------------------------------------*/
uint16_t ZPZ_Request_MAP_DOWNLOAD (uint16_t CRC, uint16_t NumPacket)
{
	uint16_t crc;
	uint8_t  temp;
	uint16_t i;
	uint32_t Address;
	uint8_t  buff [4];
	

	// Ожидаем приёма пустого байта перед информацией
	temp = UARTReceiveByte_by_SLIP(ZPZ_UART, ZPZ_RECEIVE_BYTE_TIMEOUT);
	// Сразу же подсчитаем изменения в контрольной сумме
	crc = Crc16(&temp, 1, CRC);
	
	// Используя макрос получаем адрес расположения текущего пакета
	Address = nMAP_ADR(NumPacket);
	// Далее принимаем полезную часть информации и сразу пишем во внешнюю SPI память
	for(i = 0; i < PACKETnMAP_DATASIZE; i++)
	{
		// Принимаем байт информации
		temp = UARTReceiveByte_by_SLIP(ZPZ_UART, ZPZ_RECEIVE_BYTE_TIMEOUT);
		// Записываем во внешнюю SPI память по байтово
		ZPZ_WriteIntoCSnUnion(Address + i, &temp, 1);
		// Пересчитываем контрольную сумму
		crc = Crc16(&temp, 1, crc);
	}
	// Если это последний пакет с картой, запишем в память сразу после карты
	// Признак того, что карта загружена, как и полетное задание
	// Который будем использовать при самодиагностике (отсутсвие задание - это неисправность/неготовность)
	if(NumPacket == 400)
	{
		Address+=800;
		*((uint32_t*)buff) = 0x4F4F4F4F; 
		ZPZ_WriteIntoCSnUnion(Address, buff, 4);
	}		
	
	return crc;
}

void ZPZ_Response_MAP_DOWNLOAD (uint16_t NumPacket)
{
	ZPZ_ShortResponse(MAP_DOWNLOAD, NumPacket, SUCCES);
}



//-----------------------------------------Обслуживание команды START_UPLOAD---------------------------------------------------------*/
uint16_t ZPZ_Request_START_UPLOAD (uint16_t CRC)
{
	return ZPZ_RequestWithEmptyData(CRC);
}

void ZPZ_Response_START_UPLOAD (uint16_t NumPacket) 
{
	ZPZ_Response_Union   ZPZ_Response;     // Стандартный ответ к ЗПЗ           
	uint16_t             i;                // Счетчик циклов
	uint8_t              temp = 0;         // Временное хранилище читаемого из SPI Flash 
	
	// Заполняем структуру ответа и сразу начнем считать контрольную сумму
	ZPZ_Response.Struct.Handler     = HANDLER_BU;           // Заголовок BU
	ZPZ_Response.Struct.PacketSize  = 0x003C;               // Размер пакета 60 байт
	ZPZ_Response.Struct.Command     = START_UPLOAD;         // Команда: Инициация выгрузки карты
	ZPZ_Response.Struct.Count       = NumPacket;            // Номер пакета берем из запроса
	ZPZ_Response.Struct.Error       = SUCCES;               // Статус выполнения: без ошибок
	// Считаем контрольную сумму базовой части пакета
	ZPZ_Response.Struct.CRC         = Crc16(&ZPZ_Response.Buffer[0], 8, CRC16_INITIAL_FFFF);
		
	// Посылаем признак начала пакета
	SendFEND(ZPZ_UART, ZPZ_SEND_BYTE_TIMEOUT);
	
	// Отправим базовую часть пакета
	for(i = 0; i < 8; i++)
	{
		// Посылаем байт информации
		UARTSendByte_by_SLIP (ZPZ_UART, ZPZ_SEND_BYTE_TIMEOUT, ZPZ_Response.Buffer[i]);
	}
	
	// Далее будем читать внешнюю SPI память побайтно и сразу отправлять
	for(i = 0; i < PACKET0_DATASIZE; i++)
	{
		// Читаем байт
		ZPZ_ReadIntoCSnUnion(PACKET0_ADR+i, &temp, 1);
		// Пересчитываем с его учетом контрольную сумму
		ZPZ_Response.Struct.CRC = Crc16(&temp, 1, ZPZ_Response.Struct.CRC);
		// Отправляем считанный байт
		UARTSendByte_by_SLIP (ZPZ_UART, ZPZ_SEND_BYTE_TIMEOUT, temp);
	}
	
	// Отправим контрольную сумму
	for(i = 8; i < 10; i++)
	{
		// Отправляем контрольную сумму
		UARTSendByte_by_SLIP (ZPZ_UART, ZPZ_SEND_BYTE_TIMEOUT, ZPZ_Response.Buffer[i]);
	}
	
	//В конце посылаем признак конца пакета
	SendFEND(ZPZ_UART, ZPZ_SEND_BYTE_TIMEOUT);
}



//-----------------------------------------Обслуживание команды MAP_UPLOAD---------------------------------------------------------*/
uint16_t ZPZ_Request_MAP_UPLOAD (uint16_t CRC)
{
	return ZPZ_RequestWithEmptyData(CRC);
}

void ZPZ_Response_MAP_UPLOAD (uint16_t NumPacket)
{
	ZPZ_Response_Union      ZPZ_Response;       // Стандартный ответ к ЗПЗ
	uint8_t                 temp;               // Временное хранилище читаемого из SPI Flash 
	uint16_t                i;                  // Счетчик циклов
	uint32_t                Address;            // Адрес чтения из SPI Flash
	
	// Заполняем структуру ответа и сразу начнем считать контрольную сумму
	ZPZ_Response.Struct.Handler     = HANDLER_BU;       // Заголовок BU
	ZPZ_Response.Struct.PacketSize  = 0x0324;           // Размер пакета 804 байта
	ZPZ_Response.Struct.Command     = MAP_UPLOAD;       // Команда-запрос: Выгрузка карты
	ZPZ_Response.Struct.Count       = NumPacket;        // Номер пакета повторяем из запроса
	ZPZ_Response.Struct.Error       = SUCCES;           // Статус выполянения: без ошибок
	
	// Вычисляем контрольную сумму базовой части пакета
	ZPZ_Response.Struct.CRC         = Crc16(&ZPZ_Response.Buffer[0], 8, CRC16_INITIAL_FFFF);
		
	// Посылаем признак начала пакета
	SendFEND(ZPZ_UART, ZPZ_SEND_BYTE_TIMEOUT);
	
	// Отправим базовую часть пакета
	for(i = 0; i < 8; i++)
	{
		// Отправляем байт за байтом основной части пакета
		UARTSendByte_by_SLIP (ZPZ_UART, ZPZ_SEND_BYTE_TIMEOUT, ZPZ_Response.Buffer[i]);
	}

	// Далее будем читать внешнюю SPI память побайтно и сразу отправлять
	for(i = 0; i < PACKETnMAP_DATASIZE; i++)
	{
		// Определяем адрес с помощью макроса
		Address = nMAP_ADR(NumPacket);
		// Читаем байт
		ZPZ_ReadIntoCSnUnion(Address + i, &temp, 1);
		// Пересчитываем с его учетом контрольную сумму
		ZPZ_Response.Struct.CRC = Crc16(&temp, 1, ZPZ_Response.Struct.CRC);
		// Отправляем считанное
		UARTSendByte_by_SLIP (ZPZ_UART, ZPZ_SEND_BYTE_TIMEOUT, temp);
	}
	
	// Отправим контрольную сумму
	for(i = 8; i < 10; i++)
	{
		UARTSendByte_by_SLIP (ZPZ_UART, ZPZ_SEND_BYTE_TIMEOUT, ZPZ_Response.Buffer[i]);
	}
	
	//В конце посылаем признак конца пакета
	SendFEND(ZPZ_UART, ZPZ_SEND_BYTE_TIMEOUT);
}



//-----------------------------------------Обслуживание команды BIM_CONTROL---------------------------------------------------------*/
uint16_t ZPZ_Request_BIM_CONTROL (uint16_t CRC, ZPZ_RequestControlBIM_Union* BIM_Data)
{
	uint16_t crc;

	// Здесь необходимо принять только 3 байта согласно протоколу:
	// Side  0x4C ('L') левый БИМ		
	//       0x52 ('R') правый БИМ		
	// State 0x00 выключить двигатель					
	//       0x01 включить двигатель					
	// Position 0…255 (0=0%, 255=100%) значение положения стропы		
	
	// Принимаем информационную часть
	BIM_Data->Struct.Side = UARTReceiveByte_by_SLIP(ZPZ_UART,ZPZ_RECEIVE_BYTE_TIMEOUT);
	BIM_Data->Struct.State = UARTReceiveByte_by_SLIP(ZPZ_UART,ZPZ_RECEIVE_BYTE_TIMEOUT);
	BIM_Data->Struct.Position = UARTReceiveByte_by_SLIP(ZPZ_UART,ZPZ_RECEIVE_BYTE_TIMEOUT);
	
	// Считаем контрольную сумму, используя контрольную сумму основной части пакета CRC 
	crc = Crc16(&BIM_Data->Buffer[0], 3, CRC);
	
	return crc;
}

void ZPZ_Response_BIM_CONTROL (ZPZ_RequestControlBIM_Union* BIM_Data, uint16_t NumPacket)
{
	uint16_t BIM_Side;
	uint8_t  status = 0;
	
	// Для отправки запроса БИМ необходимо, чтобы БИМ был запитан аппаратно 
	// Если шпилька 1 вставлена
	if (SelfTesting_PIN1() == ST_OK)
	{
		// Ответим, что аппаратно питания на CAN нет, значит дальше не идем
		ZPZ_ShortResponse(BIM_CONTROL, NumPacket, CAN_DISABLE_BY_PIN1);
		// И завершаемся с ошибкой
		return;
	}
	// Если реле питания БИМ выкл
	if (SelfTesting_POW_BIM() != ST_OK)
	{
		// Ответим, что аппаратно питания на CAN нет, значит дальше не идем
		ZPZ_ShortResponse(BIM_CONTROL, NumPacket, CAN_DISABLE_BY_RELAY);
		// И завершаемся с ошибкой
		return;
	}
		
	// Определяем правым или левым БИМом будем управлять
	if (BIM_Data->Struct.Side == L_BIM) 
		BIM_Side =  DEVICE_101;
	else if (BIM_Data->Struct.Side == R_BIM) 
		BIM_Side = DEVICE_100;
	else 
	{
		// Такого ID не существует
		ZPZ_ShortResponse(BIM_CONTROL, NumPacket, BIM_ID_NOT_EXIST);
		// И завершаемся с ошибкой
		return;
	}
	
	// Отправляем запрос на управление с параметрами
	status = BIM_SendRequest (BIM_Side, BIM_Data->Struct.State, BIM_Data->Struct.Position, 7, 255, 255);
	
	// Теперь нужно ответить по ЗПЗ
	if(status)
		ZPZ_ShortResponse(BIM_CONTROL, NumPacket, SUCCES);
	else 
		ZPZ_ShortResponse(BIM_CONTROL, NumPacket, BIM_CTRL_FAILED);
}



//-----------------------------------------Обслуживание команды BIM_STATUS---------------------------------------------------------*/
uint16_t ZPZ_Request_BIM_STATUS (uint16_t CRC, ZPZ_RequestControlBIM_Union* BIMControl)	
{
	uint16_t crc = 0;
	
	// Данные поля просто устанавливаем в ноль, они не используются
	BIMControl->Struct.State       = 0; 
	BIMControl->Struct.Position    = 0;
	
	// Принимаем байт выбора БИМа
	BIMControl->Struct.Side        = UARTReceiveByte_by_SLIP(ZPZ_UART, ZPZ_RECEIVE_BYTE_TIMEOUT);
	// Подсчитываем контрольную сумму с учетом принятого байта
	crc = Crc16(&BIMControl->Struct.Side, 1, CRC);
	
	return crc;
}

void ZPZ_Response_BIM_STATUS(uint8_t Side, uint16_t NumPacket)
{
	ZPZ_ResponseStatusBIM_Union   ZPZ_BIM_Status;      // Структура с информацией от БИМ
	ZPZ_Response_Union            ZPZ_Response;        // Стандартный ответ к ЗПЗ
	uint16_t                      BIM_Side, i;         // Выбор левого, правого БИМ; счетчик циклов
	uint8_t                       status = 0;          // Статус обмена с БИМ
	
	// Для отправки запроса БИМ необходимо, чтобы БИМ был запитан аппаратно 
	// Если шпилька 1 вставлена
	if (SelfTesting_PIN1() == ST_OK)
	{
		// Ответим, что аппаратно питания на CAN нет, значит дальше не идем
		ZPZ_ShortResponse(BIM_STATUS, NumPacket, CAN_DISABLE_BY_PIN1);
		// И завершаемся с ошибкой
		return;
	}
	// Если реле питания БИМ выкл
	if (SelfTesting_POW_BIM() != ST_OK)
	{
		// Ответим, что аппаратно питания на CAN нет, значит дальше не идем
		ZPZ_ShortResponse(BIM_STATUS, NumPacket, CAN_DISABLE_BY_RELAY);
		// И завершаемся с ошибкой
		return;
	}
	
	// Определяем правым или левым БИМом будем управлять
	if (Side == L_BIM) 
		BIM_Side =  DEVICE_101;
	else if (Side == R_BIM) 
		BIM_Side = DEVICE_100;
	// Это случай ошибки, адрес выбран произвольно 
	else 
	{
		// Такого ID не существует
		ZPZ_ShortResponse(BIM_STATUS, NumPacket, BIM_ID_NOT_EXIST);
		// И завершаемся с ошибкой
		return;
	}
	
	// Спросим состояние БИМов
	status = BIM_SendRequest(BIM_Side, BIM_CMD_REQ, 0, 0, 0, 0);
	// Проверим состоялся ли обмен, и если нет, то ответим ошибкой
	if(!status)
	{	
		ZPZ_ShortResponse(BIM_STATUS, NumPacket, BIM_CTRL_FAILED);
		return;
	}
	
	// Заполняем информационные поля информацией от БИМ
	ZPZ_BIM_Status.Struct.Side          = Side;
	ZPZ_BIM_Status.Struct.StatusFlags   = BIM_GetStatusFlags(BIM_Side);
	ZPZ_BIM_Status.Struct.Position      = BIM_GetStrapPosition (BIM_Side);
	ZPZ_BIM_Status.Struct.Voltage       = BIM_GetVoltage (BIM_Side);
	ZPZ_BIM_Status.Struct.Current       = BIM_GetCurrent (BIM_Side);
	ZPZ_BIM_Status.Struct.Speed         = BIM_GetSpeed (BIM_Side);
	
	
	// Заполняем структуру базовых полей всех пакетов
	ZPZ_Response.Struct.Handler    = HANDLER_BU;        // Заголовок BU
	ZPZ_Response.Struct.PacketSize = 11;                // Размер пакета 11 байт
	ZPZ_Response.Struct.Command    = BIM_STATUS;        // Команда-запрос информации от БИМов
	ZPZ_Response.Struct.Count      = NumPacket;         // Номер пакета повторяем из запроса
	ZPZ_Response.Struct.Error      = SUCCES;            // Статус выполнения: без ошибок
	
	// Считаем контрольную сумму
	ZPZ_Response.Struct.CRC = Crc16(&ZPZ_Response.Buffer[0], 8, CRC16_INITIAL_FFFF);
	ZPZ_Response.Struct.CRC = Crc16(&ZPZ_BIM_Status.Buffer[0], 7, ZPZ_Response.Struct.CRC);
	
	// Сначала отправляем признак-разделитель начала пакета
	SendFEND(ZPZ_UART, ZPZ_SEND_BYTE_TIMEOUT);
	
	// Отправляем первичную часть пакета (первые 8 байт)
	for(i = 0; i < 8; i++)
		UARTSendByte_by_SLIP (ZPZ_UART, ZPZ_SEND_BYTE_TIMEOUT, ZPZ_Response.Buffer[i]);
	
	// Отправляем информационную часть пакета
	for(i = 0; i < 10; i++)
		UARTSendByte_by_SLIP (ZPZ_UART, ZPZ_SEND_BYTE_TIMEOUT, ZPZ_BIM_Status.Buffer[i]);
	
	// И в конце сверху посылаем контрольную сумму
	for(i = 8; i < 10; i++)
		UARTSendByte_by_SLIP (ZPZ_UART, ZPZ_SEND_BYTE_TIMEOUT, ZPZ_Response.Buffer[i]);
	
	// И в конце опять разделитель
	SendFEND(ZPZ_UART, ZPZ_SEND_BYTE_TIMEOUT);
}



//-----------------------------------------Обслуживание команды LOG_FORMAT---------------------------------------------------------*/
uint16_t ZPZ_Request_LOG_FORMAT (uint16_t CRC, uint8_t* Subcommand)
{
	uint16_t crc;
	
	// Принимаем байт подкоманды: Запуск форматирования или Статус форматирования
	*Subcommand = UARTReceiveByte_by_SLIP(ZPZ_UART, ZPZ_RECEIVE_BYTE_TIMEOUT);
	// Подсчитываем контрольную сумму
	crc = Crc16(Subcommand, 1, CRC);
	
	return crc;
}

void ZPZ_Response_LOG_FORMAT (uint16_t NumPacket, uint8_t Subcommand)
{
	ZPZ_Response_Union   ZPZ_Response;    // Стандартный ответ к ЗПЗ
	uint8_t              temp;            // Дополнительный байт статуса
	uint8_t              i;               // Счетчик циклов
	
	// Проверим верная ли пришла подкоманда:
	if(Subcommand != 0x02 && Subcommand != 0x01)
	{
		// Такой подкоманды не реализовано, коротко отвечаем с соответствующим признаком
		ZPZ_ShortResponse(LOG_FORMAT, NumPacket, LOG_FORMAT_SUBCMD_ERROR);
		return;
	}
	
	// Заполняем структуру ответа
	ZPZ_Response.Struct.Handler    = HANDLER_BU;    // Заголовок BU
	ZPZ_Response.Struct.PacketSize = 0x05;          // Размер пакета 5 байт
	ZPZ_Response.Struct.Command    = LOG_FORMAT;    // Команда форматирования черного ящика
	ZPZ_Response.Struct.Count      = NumPacket;     // Номер пакета берем из запроса
	ZPZ_Response.Struct.Error      = SUCCES;        // Статус выполнения: Без ошибок
		
	// Подсчитываем контрольную сумму основной части пакета
	ZPZ_Response.Struct.CRC = Crc16(&ZPZ_Response.Buffer[0], 8, CRC16_INITIAL_FFFF);
	
	// Сначала отправляем признак-разделитель начала пакета
	SendFEND(ZPZ_UART, ZPZ_SEND_BYTE_TIMEOUT);
	
	// Отправляем первичную часть пакета - 8 байт
	for(i = 0; i < 8; i++)
		UARTSendByte_by_SLIP (ZPZ_UART, ZPZ_SEND_BYTE_TIMEOUT, ZPZ_Response.Buffer[i]);
	
	// Запрос на форматирование накопителя черного ящика
	if(Subcommand == 0x02)
	{
		// Отвечаем, что приняли команду и переходим на её выполнение
		temp = FORMATING_LOG_FS_STARTED;
		ZPZ_Response.Struct.CRC = Crc16(&temp, 1, ZPZ_Response.Struct.CRC);
		UARTSendByte_by_SLIP (ZPZ_UART, ZPZ_SEND_BYTE_TIMEOUT, temp);
		
		// После сверху посылаем контрольную сумму
		for(i = 8; i < 10; i++)
			UARTSendByte_by_SLIP (ZPZ_UART, ZPZ_SEND_BYTE_TIMEOUT, ZPZ_Response.Buffer[i]);
		// И в конце опять разделитель
		SendFEND(ZPZ_UART, ZPZ_SEND_BYTE_TIMEOUT);
		
		// Запускаем форматирование
		LogFs_Formatting();
		// После форматирование необходимо обновить информацию о файловой системе
		LogFs_Info();
		
		return;
	}
	// Запрос статуса форматирования
	else if (Subcommand == 0x01)
	{
		// Отвечаем, что форматирование завершено или в данный момент не выполняется
		temp = FORMATING_LOG_FS_COMPLETED;
		ZPZ_Response.Struct.CRC = Crc16(&temp, 1, ZPZ_Response.Struct.CRC);
		UARTSendByte_by_SLIP (ZPZ_UART, ZPZ_SEND_BYTE_TIMEOUT, temp);
		
		// После сверху посылаем контрольную сумму
		for(i = 8; i < 10; i++)
			UARTSendByte_by_SLIP (ZPZ_UART, ZPZ_SEND_BYTE_TIMEOUT, ZPZ_Response.Buffer[i]);
		// И в конце опять разделитель
		SendFEND(ZPZ_UART, ZPZ_SEND_BYTE_TIMEOUT);
		
		return;
	}
}



//-----------------------------------------Обслуживание команды LOG_FILES---------------------------------------------------------*/
uint16_t ZPZ_Request_LOG_FILES (uint16_t CRC)
{
	return ZPZ_RequestWithEmptyData(CRC);
}

void ZPZ_Response_LOG_FILES (uint16_t NumPacket)
{
	ZPZ_Response_Union    ZPZ_Response;    // Стандартный ответ к ЗПЗ
	uint8_t               buffer[4];       // Временное хранилище данных
	uint16_t              files = 0;       // Количество файлов в хранилище
	uint8_t               result = 0;      // Коды ошибок файловой системы
	uint8_t               i, j;            // Счетчики циклов
	
	// Проверим целлостность и разметку файловой системы
	if(SelfTesting_LogFs() != ST_OK)
	{
		// Файловая система поверждена, чтение данных невозможно
		ZPZ_ShortResponse(LOG_FILES, NumPacket, LOG_FS_IS_CORRUPTED);
		return;
	}
	
	// Заполняем структуру общей части всех пакетов
	ZPZ_Response.Struct.Handler    = HANDLER_BU;     // Заголовок BU
	ZPZ_Response.Struct.Command    = LOG_FILES;      // Команда: Запрос информации о содержимом
	ZPZ_Response.Struct.Count      = NumPacket;      // Номер пакета берем из запроса
	ZPZ_Response.Struct.Error      = SUCCES;         // Статус выполнения: Без ошибок
		
	// Минимально возможное число байт пакета: 
	// Команда (1б) + Номер пакета (2б) + Error (1б) + Кол-во файлов (2б) = 6 байт
	// Остальное зависит от количества файлов в накопителе
	ZPZ_Response.Struct.PacketSize = 6;
	// Тогда определим количество файлов и прибавим (для каждого файла отводится 6 байт сообщения)
	files = LogFs_GetFileNum();
	ZPZ_Response.Struct.PacketSize += 6*files;
	
	// Теперь посчитаем контрольную сумму начала пакета (первые 8 байт)
	ZPZ_Response.Struct.CRC = Crc16(&ZPZ_Response.Buffer[0], 8, CRC16_INITIAL_FFFF);
	
	// Начнём отправлять сообщение
	// Сначала отправляем признак-разделитель начала пакета
	SendFEND(ZPZ_UART, ZPZ_SEND_BYTE_TIMEOUT);
	// Теперь первые 8 байт 
	for(i = 0; i < 8; i++)
		UARTSendByte_by_SLIP (ZPZ_UART, ZPZ_SEND_BYTE_TIMEOUT, ZPZ_Response.Buffer[i]);
	
	// Готовим к отправке число файлов (2 байта)
	*(uint16_t*)buffer = (uint16_t)files;
	// Подсчитываем контрольную сумму с учетом этого
	ZPZ_Response.Struct.CRC = Crc16(buffer, 2, 	ZPZ_Response.Struct.CRC);
	// Отправляем эти два байта с числом файлов
	for(i = 0; i < 2; i++)
		UARTSendByte_by_SLIP (ZPZ_UART, ZPZ_SEND_BYTE_TIMEOUT, buffer[i]);
	
	
	// Определение, упаковка, подсчет контрольной суммы и отправка остальной части пакета (номера файлов и размеры)
	for(j = 0; j < files; j++)
	{	
		// Если первая итерация то, (как бы устанавливаем точку отсчета)
		// Ищем первый (самый старый файл в директории)
		if(j == 0) 
			result = Log_Fs_FindFile(FIRST_FILE);
		// При последующих итерациях нужно просто прокручивать файлы 
		// Командой "NEXT_FILE"
		else 
			result = Log_Fs_FindFile(NEXT_FILE);
		
		// Узнаем его номер файла и сразу упакуем в буфер для отправки
		*(uint16_t*)buffer = Log_Fs_GetFileProperties (FILE_NUMBER);
		// Подсчитываем контрольную сумму
		ZPZ_Response.Struct.CRC = Crc16(buffer, 2, 	ZPZ_Response.Struct.CRC);
		// Отправляем эти 2 байта информации о номере файла
		for(i = 0; i < 2; i++)
			UARTSendByte_by_SLIP (ZPZ_UART, ZPZ_SEND_BYTE_TIMEOUT, buffer[i]);
		
		// Узнаем его размер и сразу упакуем в буфер для отправки
		*((uint32_t*)buffer) = Log_Fs_GetFileProperties (FILE_SIZE);
		// Подсчитываем контрольную сумму 
		ZPZ_Response.Struct.CRC = Crc16(buffer, 4, 	ZPZ_Response.Struct.CRC);
		// Отправляем эти 4 байта информации о размере файла
		for(i = 0; i < 4; i++)
			UARTSendByte_by_SLIP (ZPZ_UART, ZPZ_SEND_BYTE_TIMEOUT, buffer[i]);
		
		// Дополнительная проверка по выходу из цикла
		// Если все файлы были просмотрены, функция Log_Fs_FindFile() возвращает FS_ALL_FILES_SCROLLS
		// Проконтролируем это
		if (result ==  FS_ALL_FILES_SCROLLS)
			break;
	}
	
	// После сверху посылаем контрольную сумму
	for(i = 8; i < 10; i++)
		UARTSendByte_by_SLIP (ZPZ_UART, ZPZ_SEND_BYTE_TIMEOUT, ZPZ_Response.Buffer[i]);
	// И в конце опять разделитель
	SendFEND(ZPZ_UART, ZPZ_SEND_BYTE_TIMEOUT);
}



//-----------------------------------------Обслуживание команды LOG_UPLOAD---------------------------------------------------------*/
uint16_t ZPZ_Request_LOG_UPLOAD(uint16_t CRC, uint16_t* NUM)
{
	uint8_t temp = 0;       // Временная переменная
	uint16_t crc = 0;       // Контрольная сумма
	
	// Допринимаем остальные два байта с номером файла
	// который просят отправить
	temp = UARTReceiveByte_by_SLIP(ZPZ_UART, ZPZ_RECEIVE_BYTE_TIMEOUT);
	crc = Crc16(&temp, 1, CRC);
	*NUM = temp;
	temp = UARTReceiveByte_by_SLIP(ZPZ_UART, ZPZ_RECEIVE_BYTE_TIMEOUT);
	crc = Crc16(&temp, 1, crc);
	*NUM |= (uint8_t)(temp << 8);
	
	return crc;
}

uint8_t ZPZ_Response_LOG_UPLOAD(uint16_t File_num, uint16_t NumPacket)
{
	ZPZ_Response_Union    ZPZ_Response;      // Стандартный ответ к ЗПЗ
	uint8_t               temp[2];           // Временный буфер для чтения и отправки
	uint32_t              filesize;          // Размер файла
	uint16_t              packet_count = 0;  // Количество пакетов на которые будет разбит файл
	uint32_t              i;                 // Счетчик циклов
	uint32_t              offset;            // Смещение (в байтах) от начала файла
	
	// Ищем файл в хранилище по номеру
	// Если функция не найдет файл, то выдаст ошибку, тогда можно не продолжать
	if(Log_Fs_FindFile_ByNum(File_num)!= FS_FINE)
	{
		// Ответим ошибкой: Файл с таким номером не найден
		ZPZ_ShortResponse(LOG_UPLOAD, 0, LOG_FS_FILE_NOT_FIND);
		return 1;
	}

	// Узнаем размер файла
  filesize = Log_Fs_GetFileProperties(FILE_SIZE);

	
	// Заполняем структуру общей части всех пакетов
	ZPZ_Response.Struct.Handler    = HANDLER_BU;       // Заголовок BU
	ZPZ_Response.Struct.Command    = LOG_UPLOAD;       // Команда: выгрузка файла
	ZPZ_Response.Struct.Count      = NumPacket;        // Номер пакета при разбиении файла
	ZPZ_Response.Struct.Error      = SUCCES;           // Статус выполнения: Без ошибок
	
	// Размер пакета всегда не меньше 6 байт (Код команды, Номер пакета, Статус, Номер файла)
	ZPZ_Response.Struct.PacketSize = 6;
	
	// Если номер пакета нулевой, то запрашивается количество пакетов, 
	// на которые будет разбит файл с номером File_num
	if(NumPacket == 0)
	{
		// Узнаем на сколько пакетов по BYTE_FROM_FILE байт разобьётся файл 
		packet_count = (uint16_t)((filesize/BYTE_FROM_FILE));
		if (filesize % BYTE_FROM_FILE != 0)
			packet_count++;
		
	  ZPZ_Response.Struct.PacketSize += 2;
	  // Теперь посчитаем контрольную сумму начала пакета (первые 8 байт)
	  ZPZ_Response.Struct.CRC = Crc16(&ZPZ_Response.Buffer[0], 8, CRC16_INITIAL_FFFF);

		
		// Начнём отправлять сообщение
	  // Сначала отправляем признак-разделитель начала пакета
	  SendFEND(ZPZ_UART, ZPZ_SEND_BYTE_TIMEOUT);
	  // Теперь первые 8 байт 
	  for(i = 0; i < 8; i++)
			UARTSendByte_by_SLIP (ZPZ_UART, ZPZ_SEND_BYTE_TIMEOUT, ZPZ_Response.Buffer[i]);
		
		// Отправляем номер файла
	  *(uint16_t*)temp = File_num;
	  // Подсчитываем контрольную сумму с учетом этого
	  ZPZ_Response.Struct.CRC = Crc16(temp, 2, 	ZPZ_Response.Struct.CRC);
	  // Отправляем номер файла
	  for(i = 0; i < 2; i++)
			UARTSendByte_by_SLIP (ZPZ_UART, ZPZ_SEND_BYTE_TIMEOUT, temp[i]);
		
		// Отправляем количество пакетов файла
	  *(uint16_t*)temp = packet_count;
	  // Подсчитываем контрольную сумму с учетом этого
	  ZPZ_Response.Struct.CRC = Crc16(temp, 2, 	ZPZ_Response.Struct.CRC);
	  // Отправляем номер файла
	  for(i = 0; i < 2; i++)
			UARTSendByte_by_SLIP (ZPZ_UART, ZPZ_SEND_BYTE_TIMEOUT, temp[i]);			
	}
	
	// Если не нулевой номер пакета, то уже запращивается информация из файла
	// Причем конкретно пакет (на которые был разбит файл) с номером NumPacket
	else
	{
		// Нужно проверить есть ли вообще такой номер пакета
		
		// Узнаем на сколько пакетов по BYTE_FROM_FILE байт разобьётся файл 
		packet_count = (uint16_t)((filesize/BYTE_FROM_FILE));
		if (filesize % BYTE_FROM_FILE != 0)
			packet_count++;
		
		// Если запрашиваемый номер пакета не существует
		if(NumPacket > packet_count)
		{
			// Ответим ошибкой
			ZPZ_ShortResponse(LOG_UPLOAD, NumPacket, LOG_FS_PACKET_NOT_EXIST);
			return 1;
		}
		
		// Такой пакет определен
		// Готовимся к отправке 
		
		// Если запрашиваемый пакет является крайним (либо единственным)
		if(packet_count - NumPacket == 0)
		{
			// Увеличиваем размер на оставшуюся часть файла
			ZPZ_Response.Struct.PacketSize += filesize - (packet_count - 1) * BYTE_FROM_FILE;
			
			// Теперь посчитаем контрольную сумму начала пакета (первые 8 байт)
			ZPZ_Response.Struct.CRC = Crc16(&ZPZ_Response.Buffer[0], 8, CRC16_INITIAL_FFFF);
			
			// Для выгрузки нужного куска, необходимо вычислить смещение в байтах от начала файла
			offset = (NumPacket-1)* BYTE_FROM_FILE;
			
			// Начнём отправлять сообщение
			// Сначала отправляем признак-разделитель начала пакета
			SendFEND(ZPZ_UART, ZPZ_SEND_BYTE_TIMEOUT);
			// Теперь первые 8 байт 
			for(i = 0; i < 8; i++)
				UARTSendByte_by_SLIP (ZPZ_UART, ZPZ_SEND_BYTE_TIMEOUT, ZPZ_Response.Buffer[i]);
			
			// Отправляем номер файла
			*(uint16_t*)temp = File_num;
			// Подсчитываем контрольную сумму с учетом этого
			ZPZ_Response.Struct.CRC = Crc16(temp, 2, 	ZPZ_Response.Struct.CRC);
			// Отправляем номер файла
			for(i = 0; i < 2; i++)
				UARTSendByte_by_SLIP (ZPZ_UART, ZPZ_SEND_BYTE_TIMEOUT, temp[i]);
					
			// Указатель внутри функции чтения уже установлен ранее, теперь готовы читать нужную часть 
			for(i = 0; i < (filesize - (packet_count - 1) * BYTE_FROM_FILE); i++)
			{
				// Будем читать и отправлять побайтово, не забывая пересчитывать контрольную сумму
				LogFs_ReadFile(&temp[0], offset + i, 1);
				// Сразу подсчитываем контрольную сумму
			  ZPZ_Response.Struct.CRC = Crc16(&temp[0], 1, 	ZPZ_Response.Struct.CRC);
			  // И сразу отправляем
			  UARTSendByte_by_SLIP (ZPZ_UART, ZPZ_SEND_BYTE_TIMEOUT, temp[0]);
			}
		}
		// Пакет не крайний 
		else
		{
			// Увеличиваем размер на BYTE_FROM_FILE байт данных
			ZPZ_Response.Struct.PacketSize += BYTE_FROM_FILE;
			
			// Теперь посчитаем контрольную сумму начала пакета (первые 8 байт)
			ZPZ_Response.Struct.CRC = Crc16(&ZPZ_Response.Buffer[0], 8, CRC16_INITIAL_FFFF);
			
			// Для выгрузки нужного куска, необходимо вычислить смещение в байтах от начала файла
			offset = (NumPacket-1)* BYTE_FROM_FILE;
			
			// Начнём отправлять сообщение
			// Сначала отправляем признак-разделитель начала пакета
			SendFEND(ZPZ_UART, ZPZ_SEND_BYTE_TIMEOUT);
			// Теперь первые 8 байт 
			for(i = 0; i < 8; i++)
				UARTSendByte_by_SLIP (ZPZ_UART, ZPZ_SEND_BYTE_TIMEOUT, ZPZ_Response.Buffer[i]);
			
			// Отправляем номер файла
			*(uint16_t*)temp = File_num;
			// Подсчитываем контрольную сумму с учетом этого
			ZPZ_Response.Struct.CRC = Crc16(temp, 2, 	ZPZ_Response.Struct.CRC);
			// Отправляем номер файла
			for(i = 0; i < 2; i++)
				UARTSendByte_by_SLIP (ZPZ_UART, ZPZ_SEND_BYTE_TIMEOUT, temp[i]);
			
			// Далее займёмся отправкой данных пакета
			
			// Указатель внутри функции чтения уже установлен ранее, теперь готовы читать нужную часть 
			for(i = 0; i < BYTE_FROM_FILE; i++)
			{
				// Будем читать и отправлять побайтово, не забывая пересчитывать контрольную сумму
				LogFs_ReadFile(&temp[0], offset + i, 1);
				// Сразу подсчитываем контрольную сумму
				ZPZ_Response.Struct.CRC = Crc16(&temp[0], 1, 	ZPZ_Response.Struct.CRC);
				// И сразу отправляем
				UARTSendByte_by_SLIP (ZPZ_UART, ZPZ_SEND_BYTE_TIMEOUT, temp[0]);
			}
		}
	}
		
	// После сверху посылаем контрольную сумму
	for(i = 8; i < 10; i++)
		UARTSendByte_by_SLIP (ZPZ_UART, ZPZ_SEND_BYTE_TIMEOUT, ZPZ_Response.Buffer[i]);
	// И в конце опять разделитель
	SendFEND(ZPZ_UART, ZPZ_SEND_BYTE_TIMEOUT);
	
	return 0;
}



//-----------------------------------------Обслуживание команды REQ_SWS---------------------------------------------------------*/
uint16_t ZPZ_Request_REQ_SWS (uint16_t CRC)
{
	return ZPZ_RequestWithEmptyData(CRC);
}

uint8_t ZPZ_Response_REQ_SWS (uint16_t NumPacket)
{
	ZPZ_Response_Union    ZPZ_Response;    // Стандартный ответ к ЗПЗ
	SWS_Packet_Type_Union SWS_data;        // Структура ответа от СВС
	uint32_t timeout = 0;                  // Таймаут счетчик
	uint8_t  i;                            // Счетчик циклов

	// Принимаем данные. 
	while(SWS_GetPacket (&SWS_data) && (timeout < 0xFFF)) timeout ++;
	
	// Проверим был ли это выход по таймауту
	if(timeout == 0xFFF)
	{
		// Если да, то нужно ответить ошибкой, так как СВС недоступен
		ZPZ_ShortResponse(REQ_SWS, 0, SWS_IS_UNAVAILABLE);
		// И завершиться с ошибкой
		return 1;
	}
	
	// Если данные приняты верно, то надо продолжать
	// Заполняем структуру общей части всех пакетов
	ZPZ_Response.Struct.Handler    = HANDLER_BU;      // Заголовок BU
	ZPZ_Response.Struct.PacketSize = 28;              // Размер пакета 28 байт
	ZPZ_Response.Struct.Command    = REQ_SWS;         // Команда-запрос данных от СВС
	ZPZ_Response.Struct.Count      = NumPacket;       // Номер ответа берём из запроса
	ZPZ_Response.Struct.Error      = SUCCES;          // Статус выполнения: без ошибок
	
  // Теперь посчитаем контрольную сумму начала пакета (первые 8 байт)
	ZPZ_Response.Struct.CRC = Crc16(&ZPZ_Response.Buffer[0], 8, CRC16_INITIAL_FFFF);
	
	// Начнём отправлять сообщение
	// Сначала отправляем признак-разделитель начала пакета
	SendFEND(ZPZ_UART, ZPZ_SEND_BYTE_TIMEOUT);
	// Теперь первые 8 байт 
	for(i = 0; i < 8; i++)
		UARTSendByte_by_SLIP (ZPZ_UART, ZPZ_SEND_BYTE_TIMEOUT, ZPZ_Response.Buffer[i]);
	
	// Теперь будем отправлять информацию от СВС
	// Нас интересует данные начиная со статического давления, кроме резервных полей и CRC
	// Начинаем отправлять с давления (8 байт), до истинной скорости (до 28 байта)
	for(i = 8; i < 28; i++)
	{
		// Сразу подсчитываем контрольную сумму
		ZPZ_Response.Struct.CRC = Crc16(&SWS_data.Buffer[i], 1, ZPZ_Response.Struct.CRC);
		// И отправляем по байтам
		UARTSendByte_by_SLIP (ZPZ_UART, ZPZ_SEND_BYTE_TIMEOUT, SWS_data.Buffer[i]);
	}
	
	// Так же отправляем 4 байта состояния СВС (с 48 по 52 байт)
	for(i = 48; i < 52; i++)
	{
		// Сразу подсчитываем контрольную сумму
		ZPZ_Response.Struct.CRC = Crc16(&SWS_data.Buffer[i], 1, ZPZ_Response.Struct.CRC);
		// И отправляем по байтам
		UARTSendByte_by_SLIP (ZPZ_UART, ZPZ_SEND_BYTE_TIMEOUT, SWS_data.Buffer[i]);
	}
	
	// После сверху посылаем контрольную сумму
	for(i = 8; i < 10; i++)
		UARTSendByte_by_SLIP (ZPZ_UART, ZPZ_SEND_BYTE_TIMEOUT, ZPZ_Response.Buffer[i]);
	// И в конце опять разделитель
	// Если FIFO передатчика заполнен, то ждем пока освободится
	SendFEND(ZPZ_UART, ZPZ_SEND_BYTE_TIMEOUT);
	
	return 0;
}



//-----------------------------------------Обслуживание команды REQ_SNS_POS---------------------------------------------------------*/
uint16_t ZPZ_Request_REQ_SNS_POS (uint16_t CRC)
{
	return ZPZ_RequestWithEmptyData(CRC);
}

uint8_t ZPZ_Response_REQ_SNS_POS (uint16_t NumPacket)
{
	ZPZ_Response_Union    ZPZ_Response;                    // Стандартный ответ к ЗПЗ
	uint8_t               i;                               // Счетчик циклов
	
	// Обновляем данные СНС
	BUP_UpdateDataFromSNS ();
	
	// Проверим исправен ли СНС (функция выше должна обновить состояние)
	if (SelfTesting_STATUS(ST_sns) == ST_FAULT)
	{
		// Если нет, то нужно ответить ошибкой: Не удаётся получить данные от СНС
		ZPZ_ShortResponse(REQ_SNS_POS, NumPacket, SNS_IS_UNAVAILABLE);
		// И завершиться с ошибкой
		return 1;
	}		
	
	// Иначе продолжаем
	// Заполняем структуру общей части всех пакетов
	ZPZ_Response.Struct.Handler    = HANDLER_BU;          // Заголовок BU
	ZPZ_Response.Struct.PacketSize = 72;                  // Размер пакета 72 байта
	ZPZ_Response.Struct.Command    = REQ_SNS_POS;         // Команда-запрос информации об местоположении от СНС
	ZPZ_Response.Struct.Count      = NumPacket;	          // Повторяем номер из запроса
	ZPZ_Response.Struct.Error      = SUCCES;              // Статус выполнения: без ошибок
	
	// Теперь посчитаем контрольную сумму начала пакета (первые 8 байт)
	ZPZ_Response.Struct.CRC = Crc16(&ZPZ_Response.Buffer[0], 8, CRC16_INITIAL_FFFF);
	
	// Начнём отправлять сообщение
	// Сначала отправляем признак-разделитель начала пакета
	SendFEND(ZPZ_UART, ZPZ_SEND_BYTE_TIMEOUT);
	// Теперь первые 8 байт 
	for(i = 0; i < 8; i++)
		UARTSendByte_by_SLIP (ZPZ_UART, ZPZ_SEND_BYTE_TIMEOUT, ZPZ_Response.Buffer[i]);
	
	// Высылаем данные местоположения
	// Из полученного пакета с местоположением от СНС выкидываем CRC, тип ответа и счетчик в начале, поэтому i = 4
	for (i = 4; i < SizeAnsPDR; i++)
	{
		// Побайтово досчитываем контрольную сумму
		ZPZ_Response.Struct.CRC = Crc16(&SNS_Orientation.Buffer[i], 1, ZPZ_Response.Struct.CRC);
		// И отправляем
		UARTSendByte_by_SLIP (ZPZ_UART, ZPZ_SEND_BYTE_TIMEOUT,	SNS_Position.Buffer[i]);
	}
	
	// Высылаем данные ориентации
	// из полученного пакета с ориентацией от СНС выкидываем CRC, тип ответа и счетчик в начале, поэтому i = 4
	for (i = 4; i < SizeAnsODR; i++)
	{
		// Побайтово досчитываем контрольную сумму
		ZPZ_Response.Struct.CRC = Crc16(&SNS_Orientation.Buffer[i], 1, ZPZ_Response.Struct.CRC);
		// И отправляем
		UARTSendByte_by_SLIP (ZPZ_UART, ZPZ_SEND_BYTE_TIMEOUT,	SNS_Orientation.Buffer[i]);
	}
	
	// После сверху посылаем контрольную сумму
	for(i = 8; i < 10; i++)
		UARTSendByte_by_SLIP (ZPZ_UART, ZPZ_SEND_BYTE_TIMEOUT, ZPZ_Response.Buffer[i]);
	// И в конце опять разделитель
	SendFEND(ZPZ_UART, ZPZ_SEND_BYTE_TIMEOUT);
	
	return 0;
}



//-----------------------------------------Обслуживание команды REQ_SNS_STATE---------------------------------------------------------*/
uint16_t ZPZ_Request_REQ_SNS_STATE (uint16_t CRC)
{
	return ZPZ_RequestWithEmptyData(CRC);
}

uint8_t ZPZ_Response_REQ_SNS_STATE (uint16_t NumPacket)
{
	ZPZ_Response_Union                     ZPZ_Response;          // Стандартный ответ к ЗПЗ
	SNS_Device_Information_Response_Union  SNS_DevInfo;           // Информация о СНС
	SNS_Available_Data_Response_Union      SNS_AvailableData;     // Доступные данные 
	uint32_t timeout = 0;                                         // Таймаут-счетчик
	uint8_t  i;                                                   // Счетчик циклов
	
	// Получим данные об устройстве
	while(SNS_GetDeviceInformation(&SNS_DevInfo) != SNS_OK && (timeout != 30)) timeout ++;
	// Проверим, вдруг выход был по таймауту, тогда нужно ответить ошибкой и завершиться
	if (timeout == 30)
	{
	  // Если да, то нужно ответить ошибкой
		ZPZ_ShortResponse(REQ_SNS_STATE, NumPacket, SNS_IS_UNAVAILABLE);
		// И завершиться с ошибкой
		return 1;
	}
	
	// Получим информацию о доступных данных 
	timeout = 0;
	while(SNS_GetDataState(&SNS_AvailableData) != SNS_OK && (timeout != 30)) timeout ++;
	// Проверим, вдруг выход был по таймауту, тогда нужно ответить ошибкой и завершиться
	if (timeout == 30)
	{
	  // Если да, то нужно ответить ошибкой
		ZPZ_ShortResponse(REQ_SNS_STATE, NumPacket, SNS_IS_UNAVAILABLE);
		// И завершиться с ошибкой
		return 1;
	}
	
	// Иначе продолжаем
	// Заполняем структуру общей части всех пакетов
	ZPZ_Response.Struct.Handler    = HANDLER_BU;         // Заголовок BU
	ZPZ_Response.Struct.PacketSize = 23;                 // Размер пакета 23 байта
	ZPZ_Response.Struct.Command    = REQ_SNS_STATE;      // Команда-запрос состояния СНС
	ZPZ_Response.Struct.Count      = NumPacket;	         // Повторяем номер как в запросе
	ZPZ_Response.Struct.Error      = SUCCES;             // Статус выполнения команда: без ошибок
	
	// Теперь посчитаем контрольную сумму начала пакета (первые 8 байт)
	ZPZ_Response.Struct.CRC = Crc16(&ZPZ_Response.Buffer[0], 8, CRC16_INITIAL_FFFF);
	
	// Начнём отправлять сообщение
	// Сначала отправляем признак-разделитель начала пакета
	SendFEND(ZPZ_UART, ZPZ_SEND_BYTE_TIMEOUT);
	// Теперь первые 8 байт 
	for(i = 0; i < 8; i++)
		UARTSendByte_by_SLIP (ZPZ_UART, ZPZ_SEND_BYTE_TIMEOUT, ZPZ_Response.Buffer[i]);
	
	
	// Высылаем данные об устройстве
	// из полученного пакета с информацией об устройсстве от СНС выкидываем CRC, тип ответа и счетчик в начале, поэтому i = 4;
	for (i = 4; i < SizeAnsDIR; i++)
	{
		// Побайтово досчитываем контрольную сумму
		ZPZ_Response.Struct.CRC = Crc16(&SNS_DevInfo.Buffer[i], 1, ZPZ_Response.Struct.CRC);
		// И отправляем
		UARTSendByte_by_SLIP (ZPZ_UART, ZPZ_SEND_BYTE_TIMEOUT,	SNS_DevInfo.Buffer[i]);
	}

	// Высылаем инфу о доступных данных
	// из полученного пакета с доступными данными от СНС необходимо только поле Data_State
	// Побайтово досчитываем контрольную сумму
	ZPZ_Response.Struct.CRC = Crc16(&SNS_AvailableData.Struct.Data_State, 1, ZPZ_Response.Struct.CRC);
	// И отправляем
	UARTSendByte_by_SLIP (ZPZ_UART, ZPZ_SEND_BYTE_TIMEOUT,	SNS_AvailableData.Struct.Data_State);
	
	// После сверху посылаем контрольную сумму
	for(i = 8; i < 10; i++)
		UARTSendByte_by_SLIP (ZPZ_UART, ZPZ_SEND_BYTE_TIMEOUT, ZPZ_Response.Buffer[i]);
	// И в конце опять разделитель
	SendFEND(ZPZ_UART, ZPZ_SEND_BYTE_TIMEOUT);
	
	return 0;	
}



//-----------------------------------------Обслуживание команды REQ_SNS_SETTINGS---------------------------------------------------------*/
uint16_t ZPZ_Request_REQ_SNS_SETTINGS(uint16_t CRC, uint8_t* buffer)
{
	uint8_t i;
	uint16_t crc = 0;
	
	for(i = 0; i < 3; i++)
	{
		buffer[i] = UARTReceiveByte_by_SLIP(ZPZ_UART, ZPZ_RECEIVE_BYTE_TIMEOUT);
	}
	crc = Crc16(buffer, 3, CRC);

	return crc;
}

// Реагируем на запрос на изменение настроек СНС
uint8_t ZPZ_Response_REQ_SNS_SETTINGS (uint8_t* params, uint16_t NumPacket)
{
	ZPZ_Response_Union   ZPZ_Response;          // Стандартный ответ к ЗПЗ
	uint8_t  i;                                 // Счетчик циклов
		
	// Анализируем пришедшую команду и выполняем действия
	
	// Команда включения горизонтальной коррекции
	if(params[0]== 0x01)
		SNS_EnableHorizontalCorrection();
	
	// Команда отключения горизонтальной коррекции
	else if (params[0] == 0x02) 
		SNS_DisableHorizontalCorrection();
	
	// Параметр не опознан
	else 
	{
		// Ответим, что ошибка в параметрах команды
		ZPZ_ShortResponse(REQ_SNS_STATE, NumPacket, SNS_SETTINGS_WRONG_PAR);
		// И завершаемся с ошибкой
		return 1;
	}
		
	// Команда начала калибровки гироскопа
	if(params[1]== 0x01) 
		SNS_StartGyroCalibration();
	
	// Команда сброса калибровки гироскопа
	else if (params[1] == 0x02)
		SNS_ResetGyroCalibration();
	
	// Параметр не опознан
	else
	{
		// Ответим, что ошибка в параметрах команды
		ZPZ_ShortResponse(REQ_SNS_STATE, NumPacket, SNS_SETTINGS_WRONG_PAR);
		// И завершаемся с ошибкой
		return 1;
	}
	
	// Команда начала калибровки магнитометра
	if(params[2]== 0x01)
		SNS_StartMagnetometerCalibration();
	
	// Команда сброса калибровки магнитометра
	else if (params[2] == 0x02)
		SNS_ResetMagnetometerCalibration();

	// Параметр не опознан
	else
	{
		// Ответим, что ошибка в параметрах команды
		ZPZ_ShortResponse(REQ_SNS_STATE, NumPacket, SNS_SETTINGS_WRONG_PAR);
		// И завершаемся с ошибкой
		return 1;
	}
	
	
	// Теперь нужно ответить 
	// Заполняем структуру общей части всех пакетов 
	ZPZ_Response.Struct.Handler    = HANDLER_BU;          // Заголовок BU
	ZPZ_Response.Struct.PacketSize = 7;                   // Размер пакета 7 байт
	ZPZ_Response.Struct.Command    = REQ_SNS_SETTINGS;    // Команда-запрос Настройки СНС
	ZPZ_Response.Struct.Count      = NumPacket;	          // Номер пакета повторяем из запроса
	ZPZ_Response.Struct.Error      = SUCCES;              // Статус выполнения: без ошибок
	

	// Теперь посчитаем контрольную сумму начала пакета (первые 8 байт)
	ZPZ_Response.Struct.CRC = Crc16(&ZPZ_Response.Buffer[0], 8, CRC16_INITIAL_FFFF);
	// Начнём отправлять сообщение
	// Сначала отправляем признак-разделитель начала пакета
	SendFEND(ZPZ_UART, ZPZ_SEND_BYTE_TIMEOUT);
	// Теперь первые 8 байт 
	for(i = 0; i < 8; i++)
		UARTSendByte_by_SLIP (ZPZ_UART, ZPZ_SEND_BYTE_TIMEOUT, ZPZ_Response.Buffer[i]);
	
	// В ответе нужно продублировать (повторить) настройки (3 байта)
	for (i = 0; i < 3; i++)
	{
		// Побайтово досчитываем контрольную сумму
		ZPZ_Response.Struct.CRC = Crc16(params, 3, ZPZ_Response.Struct.CRC);
		// И отправляем
		UARTSendByte_by_SLIP (ZPZ_UART, ZPZ_SEND_BYTE_TIMEOUT,	params[i]);
	}
	
	// После сверху посылаем контрольную сумму
	for(i = 8; i < 10; i++)
		UARTSendByte_by_SLIP (ZPZ_UART, ZPZ_SEND_BYTE_TIMEOUT, ZPZ_Response.Buffer[i]);
	// И в конце опять разделитель
	SendFEND(ZPZ_UART, ZPZ_SEND_BYTE_TIMEOUT);
	
	return 0;	
}



//-----------------------------------------Обслуживание команды PIN_STATE---------------------------------------------------------*/
uint16_t ZPZ_Request_PIN_STATE(uint16_t CRC)
{
	return ZPZ_RequestWithEmptyData(CRC);
}

void ZPZ_Response_PIN_STATE (uint16_t NumPacket)
{
	ZPZ_BasePacket_Union     ZPZ_BaseResponse;
	uint8_t  i;
	uint8_t  buff[3];
	
	// Узнаем состояние шпилек
	buff[0] = 0; // Первый байт пустой
	buff[1] = 0; 
	buff[2] = 0; 
	// Определим состояние дискретов
	buff[1] = ((~PIN1_CHECK)&0x01)| (((~PIN2_DIR_CHECK)&0x01) << 1)|(BLIND_CHECK << 2)|(((~PIN2_INV_CHECK)&0x01) << 3);
	buff[2] = PYRO_CHECK |(BLIND_CTRL_CHECK << 1)|(RELAY_BIM_CHECK << 2)|(LED_FAULT_CHECK << 6)|(LED_READY_CHECK << 7);
	
	// Теперь нужно ответить 
	// Заполняем структуру общей части всех пакетов
	ZPZ_BaseResponse.Struct.Handler    = HANDLER_BU;
	ZPZ_BaseResponse.Struct.PacketSize = 6;
	ZPZ_BaseResponse.Struct.Command    = PIN_STATE;
	ZPZ_BaseResponse.Struct.Count      = NumPacket;	
	// Теперь посчитаем контрольную сумму начала пакета (первые 7 байт)
	ZPZ_BaseResponse.Struct.CRC = Crc16(&ZPZ_BaseResponse.Buffer[0], 7, CRC16_INITIAL_FFFF);
	// Начнём отправлять сообщение
	// Сначала отправляем признак-разделитель начала пакета
	SendFEND(ZPZ_UART, ZPZ_SEND_BYTE_TIMEOUT);
	// Теперь первые 7 байт 
	for(i = 0; i < 7; i++)
		UARTSendByte_by_SLIP (ZPZ_UART, ZPZ_SEND_BYTE_TIMEOUT, ZPZ_BaseResponse.Buffer[i]);
	
	// Досчитаем контрольную сумму с учётом buff
	ZPZ_BaseResponse.Struct.CRC = Crc16(buff, 3, ZPZ_BaseResponse.Struct.CRC);
	// Отправляем содержимое buff
	for(i = 0; i < 3; i++)
		UARTSendByte_by_SLIP (ZPZ_UART, ZPZ_SEND_BYTE_TIMEOUT, buff[i]);
	
	// После сверху посылаем контрольную сумму
	for(i = 0; i < 2; i++)
		UARTSendByte_by_SLIP (ZPZ_UART, ZPZ_SEND_BYTE_TIMEOUT, ZPZ_BaseResponse.Buffer[i]);
	// И в конце опять разделитель
	SendFEND(ZPZ_UART, ZPZ_SEND_BYTE_TIMEOUT);
}


//-----------------------------------------Обслуживание команды SYSTEM_STATE---------------------------------------------------------*/
uint16_t ZPZ_Request_SYSTEM_STATE (uint16_t CRC)
{
	return ZPZ_RequestWithEmptyData(CRC);
}

void ZPZ_Response_SYSTEM_STATE (uint16_t NumPacket)
{
	ZPZ_Response_Union       ZPZ_Response;          // Стандартный ответ к ЗПЗ
	uint8_t                  i;                     // Счетчик циклов
	uint8_t                  buff[2];               // Буфер - временное хранилище
		
	// Теперь нужно ответить 
	// Заполняем структуру общей части всех пакетов
	ZPZ_Response.Struct.Handler    = HANDLER_BU;          // Заголовок BU       
	ZPZ_Response.Struct.PacketSize = 6;                   // Размер пакета в байтах  
	ZPZ_Response.Struct.Command    = SYSTEM_STATE;        // Команда: передача по CAN
	ZPZ_Response.Struct.Count      = NumPacket;	          // Номер пакета повторяем из запроса
	ZPZ_Response.Struct.Error      = SUCCES;              // Статус: без ошибок
	
	// Теперь посчитаем контрольную сумму начала пакета (первые 8 байт)
	ZPZ_Response.Struct.CRC = Crc16(&ZPZ_Response.Buffer[0], 8, CRC16_INITIAL_FFFF);
	
	// Кладём состояние системы в буфер
	*((uint16_t*)buff) = SystemState;
	// Теперь посчитаем контрольную сумму с учетом отправляемого
	ZPZ_Response.Struct.CRC = Crc16(buff, 2, ZPZ_Response.Struct.CRC);
	
	// Начнём отправлять сообщение
	// Сначала отправляем признак-разделитель начала пакета
	SendFEND(ZPZ_UART, ZPZ_SEND_BYTE_TIMEOUT);
	// Теперь первые 8 байт 
	for(i = 0; i < 8; i++)
		UARTSendByte_by_SLIP (ZPZ_UART, ZPZ_SEND_BYTE_TIMEOUT, ZPZ_Response.Buffer[i]);
	
	// Отправляем информационную часть посылки
	for(i = 0; i < 2; i++)
		UARTSendByte_by_SLIP (ZPZ_UART, ZPZ_SEND_BYTE_TIMEOUT, buff[i]);
	
	// После сверху посылаем контрольную сумму
	for(i = 8; i < 10; i++)
		UARTSendByte_by_SLIP (ZPZ_UART, ZPZ_SEND_BYTE_TIMEOUT, ZPZ_Response.Buffer[i]);
	// И в конце опять разделитель
	SendFEND(ZPZ_UART, ZPZ_SEND_BYTE_TIMEOUT);	
}



//-----------------------------------------Обслуживание команды CAN_TRANSMIT---------------------------------------------------------*/
uint16_t ZPZ_Request_CAN_TRANSMIT (uint16_t CRC, uint8_t* buffer)
{
	// Первые два байта - Адрес
	// Третий байт - размер
	// Остальные n байт - Данные
	uint8_t i;
	for(i = 0; i < 3; i++)
	{
		buffer[i] = UARTReceiveByte_by_SLIP(ZPZ_UART, ZPZ_RECEIVE_BYTE_TIMEOUT);
	}
	for(i = 3; i < buffer[2]+3; i++)
	{
		buffer[i] = UARTReceiveByte_by_SLIP(ZPZ_UART, ZPZ_RECEIVE_BYTE_TIMEOUT);
	}
	CRC = Crc16(buffer, 3 + buffer[2], CRC);

	return CRC;
}
void ZPZ_Response_CAN_TRANSMIT (uint8_t* buffer, uint16_t NumPacket)
{
	ZPZ_Response_Union       ZPZ_Response;          // Стандартный ответ к ЗПЗ
	CAN_TxMsgTypeDef         CANTxMsg;
	uint32_t timeout = 0;
	uint8_t  i;
	uint8_t  buff[3];
	uint32_t Buffer_number;
	
	// Для отправки необходимо, чтобы CAN был запитан аппаратно 
	// Если шпилька 1 вставлена
	if (SelfTesting_PIN1() == ST_OK)
	{
		// Ответим, что аппаратно питания на CAN нет, значит дальше не идем
		ZPZ_ShortResponse(CAN_TRANSMIT, NumPacket, CAN_DISABLE_BY_PIN1);
		// И завершаемся с ошибкой
		return;
	}
	// Если реле питания БИМ выкл
	if (SelfTesting_POW_BIM() != ST_OK)
	{
		// Ответим, что аппаратно питания на CAN нет, значит дальше не идем
		ZPZ_ShortResponse(CAN_TRANSMIT, NumPacket, CAN_DISABLE_BY_RELAY);
		// И завершаемся с ошибкой
		return;
	}
	
	
	// Адрес - первые два байта
	CANTxMsg.ID = CAN_STDID_TO_EXTID(*((uint16_t*)(buffer)));
	// Длина сообщения начинается со второго байта
	CANTxMsg.DLC = *((uint8_t*)(buffer+2));
	// ID - Стандартный (не расширеный)
	CANTxMsg.IDE = CAN_ID_STD;
	// Отправка без приоритета
	CANTxMsg.PRIOR_0 = DISABLE;
	// Данные идут после 2 байт адреса и 1 байта длины (с 3-его байта)
	CANTxMsg.Data[0] = *((uint32_t*)(buffer + 3));
	CANTxMsg.Data[1] = *((uint32_t*)(buffer + 7));

	// Начинаем отправлять в CAN
	// Спросим какой из буферов CAN свободен для использования
	Buffer_number = CAN_GetDisabledBuffer (ZPZ_CAN);
	// Кладём сообщение в нужный буфер и ждем отправки
	CAN_Transmit(ZPZ_CAN, Buffer_number, &CANTxMsg);
	// Ожидаем конца передачи, либо превышения времени ожидания
	while(((CAN_GetBufferStatus(ZPZ_CAN, Buffer_number) & CAN_STATUS_TX_REQ) != RESET) && (timeout != 0xFFF)) timeout++;
	
	// Вне зависимости от того, удалось отправить или нет, освобождаем буфер
	CAN_BufferRelease (ZPZ_CAN, Buffer_number);
	
	// Если превышено время на ожидание отправки
	if (timeout == 0xFFF)
	{
		// Ответим, что истекло время на отправку
		ZPZ_ShortResponse(CAN_TRANSMIT, NumPacket, CAN_SENDING_TIMEOUT);
		// И завершаемся с ошибкой
		return;
	}

	// Теперь нужно ответить 
	// Заполняем структуру общей части всех пакетов
	ZPZ_Response.Struct.Handler    = HANDLER_BU;          // Заголовок BU
	// 6 Байт обязательны + Длина сообщения          
	ZPZ_Response.Struct.PacketSize = 6 + CANTxMsg.DLC;    // Размер пакета в байтах  
	ZPZ_Response.Struct.Command    = CAN_TRANSMIT;        // Команда: передача по CAN
	ZPZ_Response.Struct.Count      = NumPacket;	          // Номер пакета повторяем из запроса
	ZPZ_Response.Struct.Error      = SUCCES;              
	// Теперь посчитаем контрольную сумму начала пакета (первые 8 байт)
	ZPZ_Response.Struct.CRC = Crc16(&ZPZ_Response.Buffer[0], 8, CRC16_INITIAL_FFFF);
	// Начнём отправлять сообщение
	// Сначала отправляем признак-разделитель начала пакета
	SendFEND(ZPZ_UART, ZPZ_SEND_BYTE_TIMEOUT);
	// Теперь первые 8 байт 
	for(i = 0; i < 8; i++)
		UARTSendByte_by_SLIP (ZPZ_UART, ZPZ_SEND_BYTE_TIMEOUT, ZPZ_Response.Buffer[i]);
	
	// Досчитаем контрольную сумму с учётом buff
	ZPZ_Response.Struct.CRC = Crc16(buffer, 3 + CANTxMsg.DLC, ZPZ_Response.Struct.CRC);
	// Отправляем содержимое buffer
	for(i = 0; i < 3 + CANTxMsg.DLC; i++)
		UARTSendByte_by_SLIP (ZPZ_UART, ZPZ_SEND_BYTE_TIMEOUT, buffer[i]);
	
	// После сверху посылаем контрольную сумму
	for(i = 8; i < 10; i++)
		UARTSendByte_by_SLIP (ZPZ_UART, ZPZ_SEND_BYTE_TIMEOUT, ZPZ_Response.Buffer[i]);
	// И в конце опять разделитель
	SendFEND(ZPZ_UART, ZPZ_SEND_BYTE_TIMEOUT);	
}


//-----------------------------------------Обслуживание режима "ВВПЗ"----------------------------------------------------------------*/
    
/******************************************************************  
    Обработчик прерываний от таймера ZPZ
******************************************************************/
void Timer2_IRQHandler(void)
{
	// Сбрасываем флаг прерываний
	TIMER_ClearFlag(ZPZ_TIMER,TIMER_STATUS_CNT_ARR);	
	// Делаем инкремент таймаут-счетчика
	TimeoutCounter++;
	
	// Сменяем состояние индикатора "READY"
	if (LED_READY_CHECK) 
		LED_READY_OFF();
	else 
		LED_READY_ON();
	
	// Здесь реализуем таймаут контроль режима ВВПЗ
	// Повисание или разрыв связи будем определять так:
	if(ZPZ_CheckHighPriorityTask() && TimeoutCounter > 20)
	{
		// Завершим режим ВППЗ и перейдем в режим РК
		ZPZ_FinishHighPriorityTask ();
	}
}


/****************************************************************** 
    Запуск режима выполнения высокоприоритетной задачи
******************************************************************/
void ZPZ_StartHighPriorityTask (void)
{
	// Выставляем флаг режима High Priority Task
	HighPriorityTask = ZPZ_HPT_MODE;
	// Сбрасываем таймаут
	TimeoutCounter  = 0;
	// Выключаем "Неисправность"
	LED_FAULT_OFF();
	// Запускаем таймер
	Timer_SetInterruptPeriod(ZPZ_TIMER, 0.2*SECOND_TICKS);
}


/****************************************************************** 
    Завершение режима выполнения высокоприоритетной задачи
******************************************************************/
void ZPZ_FinishHighPriorityTask (void)
{
	// Отключаем таймер
	TIMER_Cmd(ZPZ_TIMER, DISABLE);
	// Выключаем "Готов"
	LED_READY_OFF();
	// Выставляем флаг режима по-умолчанию - Single Command
	HighPriorityTask = ZPZ_SC_MODE;
	// Сбрасываем таймаут
	TimeoutCounter  = 0;
}

/****************************************************************** 
    Чекпоинт режима выполнения высокоприоритетной задачи      

    Примечание: Эту функция используется для защиты от 
    повисаний режима выполнения высокоприоритетной задачи.
    Если данная функция не вызывается некоторое время, то
    данный режим завершается, автоматически.
******************************************************************/
void ZPZ_CheckpointHighPriorityTask (void)
{
	// При чекпоинте нам просто необходимо сбросить счетчик таймаута
	// Принцип работы WDT
	// Если счетчик переполнится, то режим "ВВЗ" завершится
	TimeoutCounter  = 0;
}


/****************************************************************** 
    Проверка занятости модуля высокоприоритетной задачей 
******************************************************************/
uint8_t ZPZ_CheckHighPriorityTask (void)
{
	return HighPriorityTask;
}
