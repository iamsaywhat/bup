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



// Определения и макросы для реализации SLIP протокола
// Разделители в SLIP - Протоколе
typedef enum {
	FEND     = 0xC0,
	FESC     = 0xDB,
	TFEND    = 0xDC,
	TFESC		 = 0xDD,
} SLIP_DIV;



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
uint16_t ZPZ_Request_LOG_FORMAT (uint16_t CRC);
void ZPZ_Response_LOG_FORMAT (uint16_t NumPacket);
/*--------------------------------------------------------------------------------------------Обслуживание команды LOG_FILES-----------*/
uint16_t ZPZ_Request_LOG_FILES (uint16_t CRC);
void ZPZ_Response_LOG_FILES (uint16_t NumPacket);
/*--------------------------------------------------------------------------------------------Обслуживание команды LOG_UPLOAD----------*/
uint16_t ZPZ_Request_LOG_UPLOAD(uint16_t* NUM, uint16_t CRC);
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


/******************************************************************************************************

												Объявление публичных функций модуля ЗПЗ

******************************************************************************************************/

/**************************************************************************************************************
						ZPZ_RetargetPins - Функция переопределения UART2 на другие пины, для работы по ZPZ                *
**************************************************************************************************************/
void ZPZ_RetargetPins (void)
{
	RST_CLK_PCLKcmd(RST_CLK_PCLK_ZPZ_PORT , ENABLE);
	// Переназчаем UART на нужный порт для работы ZPZ
	Pin_Init (ZPZ_PORT, ZPZ_RX, PORT_FUNC_OVERRID, PORT_OE_IN);
	Pin_Init (ZPZ_PORT, ZPZ_TX, PORT_FUNC_OVERRID, PORT_OE_OUT);
}

/***************************************************************************************************************
						ZPZ_init - Запуск процедуры обмена по ZPZ                                                          *
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



/**************************************************************************************************************
						ZPZ_ShortResponse - Отправка простого пакета подтвержений/ошибки к ЗПЗ                            *
**************************************************************************************************************/
void ZPZ_ShortResponse(uint8_t Command, uint16_t Count, uint8_t Error)
{
	ZPZ_Response_Union   ZPZ_Response;
	uint16_t timeout = 0;
	uint8_t  i;
	
	//Заполняем сткруктуру ответа
	ZPZ_Response.Struct.Handler     = HANDLER_BU;   // "UB" - Должно быть "BU", но выдача идём младшим байтом вперед
	ZPZ_Response.Struct.PacketSize  = 4;				    // В размер входят только команда, счетчик, и ошибка
	ZPZ_Response.Struct.Command     = Command;
	ZPZ_Response.Struct.Count       = Count;
	ZPZ_Response.Struct.Error       = Error;
	ZPZ_Response.Struct.CRC         = Crc16(&ZPZ_Response.Buffer[0],8, CRC16_INITIAL_FFFF);
	
	// Если FIFO передатчика заполнен, то ждем пока освободится
	while ((UART_GetFlagStatus (ZPZ_UART, UART_FLAG_TXFF) == SET) && (timeout != 0xFFF)){ timeout++;}
	// Посылаем признак начала пакета
	SendFEND(ZPZ_UART, 0xFFF);
	
	// Теперь остальную часть пакета
	for(i = 0; i < 10; i++)
	{
		UARTSendByte_by_SLIP (ZPZ_UART, 0xFFF, ZPZ_Response.Buffer[i]);
	}
	// Ждем завершения передачи
	while ((UART_GetFlagStatus (ZPZ_UART, UART_FLAG_TXFF) == SET) && (timeout != 0xFFF)) 
		timeout++;
	// Посылаем признак конца пакета
	
	SendFEND(ZPZ_UART, 0xFFF);
}



/**************************************************************************************************************
          ZPZ_WriteIntoCSnUnion - Функция записи производит объединение 4х микросхем SPI-памяти в единое      *
                                  адресное пространство 0x00000 - 0x7FFFF, используя драйвер "1636PP52Y.h".   *
																	Позволяет абстрагироваться от периферийного уровня и управления CSn.        *			
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
          ZPZ_ReadIntoCSnUnion - Функция чтения производит объединение 4х микросхем SPI-памяти в единое       *
                                  адресное пространство 0x00000 - 0x7FFFF, используя драйвер "1636PP52Y.h".   *
																	Позволяет абстрагироваться от периферийного уровня и управления CSn.        *			
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
          ZPZ_ChipEraseCSnUnion - Функция стирания всех установленых микросхем памяти, используется           *
																	драйвер "1636PP52Y.h".                                                      *			
**************************************************************************************************************/
void ZPZ_ChipEraseCSnUnion(void)
{	
	SPI_1636PP52Y_ChipErase (SPI_1636PP52Y_CS1);
	SPI_1636PP52Y_ChipErase (SPI_1636PP52Y_CS2);
	SPI_1636PP52Y_ChipErase (SPI_1636PP52Y_CS3);
	SPI_1636PP52Y_ChipErase (SPI_1636PP52Y_CS4);
}	



/**************************************************************************************************************
					ZPZ_Service - Функция обслуживания команд от ЗПЗ (Основной режим)                                   *			
**************************************************************************************************************/
uint8_t ZPZ_Service (void)
{
	
	ZPZ_BasePacket_Union          ZPZ_Base_Request;  // Сюда складывается приходящий запрос (кроме полей "данные")
	ZPZ_RequestControlBIM_Union   BIM_Control;       // Структура для обслуживания команд BIM_CONTROL и BIM_STATUS
	
	uint32_t timeout;             // Контроль превышения времени обработки
	uint16_t crc, i; 							// Контрольная сумма и счетчик циклов
	uint16_t Log_files = 0;				// Номер запрашиваемого файла (обслуж. команды LOG_UPLOAD)
	uint8_t  SNS_opt[3];					// Буфер для обслуживания команды REQ_SNS_SETTINGS
	uint8_t  CAN_buff[11];        // Буфер для обслуживания команды CAN_TRANSMIT
		
	
	//Вычищаем FIFO от мусора и ждем пока не появится заголовок
	while (UART_GetFlagStatus (ZPZ_UART, UART_FLAG_RXFE) != SET) UART_ReceiveData(ZPZ_UART);	
	
	// Ожидаем заголовок
	while(timeout != 0xFFFF)
	{
		// Сначала ловим FEND по SLIP протоколу
		if (UARTReceiveByte_by_SLIP(ZPZ_UART, 0xFFFF) != FEND) continue; 
		// Если первый байт заголовка не совпал, дальше не ждем, переходим к следующей итерации
		if (UARTReceiveByte_by_SLIP(ZPZ_UART, 0xFFFF) != HANDLER_P) continue; 
		// Если второй символ тоже совпал, значит покидаем данный цикл и переходим к приёму остального пакета
		if (UARTReceiveByte_by_SLIP(ZPZ_UART, 0xFFFF) == HANDLER_C) break;
	}	
	
	// Если был таймаут, значит связи нет, можно отключиться
	if(timeout == 0xFFFF) 
		return ZPZ_TIMEOUT;
		
	// Заголовок обнаружен, далее приём синхронен
	ZPZ_Base_Request.Struct.Handler = HANDLER_PC;
	for(i = 2; i < 7; i++)
	{
		// Принимаем остальное
		ZPZ_Base_Request.Buffer[i] = UARTReceiveByte_by_SLIP(ZPZ_UART, 0xFFFF);
	}
	// Считаем контрольную сумму начала сообщения
	crc = Crc16(&ZPZ_Base_Request.Buffer[0], 7, CRC16_INITIAL_FFFF);
	
	
	// Далее программа ветвится в зависимсти от команды, информационная часть пакета будет 
	// обрабатываться разными подфункциями
	// Сценарии работы при получении всех возможных команд
	switch (ZPZ_Base_Request.Struct.Command)
	{
		case START_DOWNLOAD:
			crc = ZPZ_Request_START_DOWNLOAD (crc);
		  break;
		
		case MAP_DOWNLOAD:
			crc = ZPZ_Request_MAP_DOWNLOAD (crc, ZPZ_Base_Request.Struct.Count);
			break;
		
		case START_UPLOAD:
			crc = ZPZ_Request_START_UPLOAD (crc);
			break;
		
		case MAP_UPLOAD:
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
			crc = ZPZ_Request_LOG_FORMAT(crc);
		  break;
		
		case LOG_FILES:
			crc = ZPZ_Request_LOG_FILES (crc);
		  break;
			
		case LOG_UPLOAD:
			crc = ZPZ_Request_LOG_UPLOAD(&Log_files, crc);
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
		ZPZ_Base_Request.Buffer[i] = UARTReceiveByte_by_SLIP(ZPZ_UART, 0xFFFF);
	}
	// Сверяем контрольные суммы и проверяем, что пакет завершился признаком FEND
	if(crc != ZPZ_Base_Request.Struct.CRC || UARTReceiveByte_by_SLIP(ZPZ_UART, 0xFFFF) != FEND)
	{
		//Если не сошлись - ошибка, отправляем ответ об ошибке
		ZPZ_ShortResponse(ZPZ_Base_Request.Struct.Command, ZPZ_Base_Request.Struct.Count, FAIL);
		return ZPZ_WRONG_CRC;
	}
	
	
	
	
	// Далее опять ветвление в зависимости от типа ответа (команды запроса)
	switch (ZPZ_Base_Request.Struct.Command)
	{
		case START_DOWNLOAD:
		  ZPZ_Response_START_DOWNLOAD (ZPZ_Base_Request.Struct.Count);
			break;
		
		case MAP_DOWNLOAD:
		  ZPZ_Response_MAP_DOWNLOAD  (ZPZ_Base_Request.Struct.Count);
			break;
		
		case START_UPLOAD:
			ZPZ_Response_START_UPLOAD (ZPZ_Base_Request.Struct.Count);
			break;
		
		case MAP_UPLOAD:
			ZPZ_Response_MAP_UPLOAD (ZPZ_Base_Request.Struct.Count);
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
			ZPZ_Response_LOG_FORMAT (ZPZ_Base_Request.Struct.Count);
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
	if(timeout == TimeoutRange) return 0xFF;
  // Иначе принимаем байт
	Byte = UART_ReceiveData(UARTx);
	
	// Если поймали символ FESC, то смотрим каким будет следующий
	if(Byte == FESC)
	{
		// Ждем прихода информации
		while ((UART_GetFlagStatus (UARTx, UART_FLAG_RXFE) == SET) && (timeout != TimeoutRange)) timeout++;
		// Если выход из ожидания по таймауту - возвращаем ошибку и выходим
	  if(timeout == TimeoutRange) return 0xFF;
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
	temp = UARTReceiveByte_by_SLIP(ZPZ_UART, 0xFFFF);
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
	temp = UARTReceiveByte_by_SLIP(ZPZ_UART, 0xFFFF);
	// Сразу же подсчитаем изменения в контрольной сумме
	crc = Crc16(&temp, 1, CRC);
	
	// Далее принимаем полезную часть информации и сразу пишем во внешнюю SPI память
	for(i = 0; i < PACKET0_DATASIZE; i++)
	{
		// Принимаем байт информации
		buffer[i] = UARTReceiveByte_by_SLIP(ZPZ_UART, 0xFFFF);
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
	temp = UARTReceiveByte_by_SLIP(ZPZ_UART, 0xFFFF);
	// Сразу же подсчитаем изменения в контрольной сумме
	crc = Crc16(&temp, 1, CRC);
	
	// Используя макрос получаем адрес расположения текущего пакета
	Address = nMAP_ADR(NumPacket);
	// Далее принимаем полезную часть информации и сразу пишем во внешнюю SPI память
	for(i = 0; i < PACKETnMAP_DATASIZE; i++)
	{
		// Принимаем байт информации
		temp = UARTReceiveByte_by_SLIP(ZPZ_UART, 0xFFFF);
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
	ZPZ_BasePacket_Union ZPZ_Base_Request;
	uint32_t timeout;
	uint16_t i;
	uint8_t  temp = 0;
	
	// Заполняем структуру ответа и сразу начнем считать контрольную сумму
	ZPZ_Base_Request.Struct.Handler     = HANDLER_BU;
	ZPZ_Base_Request.Struct.PacketSize  = 0x003C; 
	ZPZ_Base_Request.Struct.Command     = START_UPLOAD;
	ZPZ_Base_Request.Struct.Count       = NumPacket;
	ZPZ_Base_Request.Struct.CRC         = Crc16(&ZPZ_Base_Request.Buffer[0], 7, CRC16_INITIAL_FFFF);
	
	// Если FIFO занято - ждем освобождения
	while ((UART_GetFlagStatus (ZPZ_UART, UART_FLAG_TXFF) == SET) && (timeout != 0xFFFF)) timeout++;
	// Посылаем признак начала пакета
	SendFEND(ZPZ_UART, 0xFFF);
	
	// Отправим базовую часть пакета
	for(i = 0; i < 7; i++)
	{
		// Посылаем байт информации
		UARTSendByte_by_SLIP (ZPZ_UART, 0xFFFF, ZPZ_Base_Request.Buffer[i]);
	}
	
	// Отправляем пустой байт (как в протоколе) и продолжаем считать контрольную сумму
	temp = 0;
	ZPZ_Base_Request.Struct.CRC = Crc16(&temp, 1, ZPZ_Base_Request.Struct.CRC);
	UARTSendByte_by_SLIP (ZPZ_UART, 0xFFFF, temp);
	
	// Далее будем читать внешнюю SPI память побайтно и сразу отправлять
	for(i = 0; i < PACKET0_DATASIZE; i++)
	{
		// Читаем байт
		ZPZ_ReadIntoCSnUnion(PACKET0_ADR+i, &temp, 1);
		// Пересчитываем с его учетом контрольную сумму
		ZPZ_Base_Request.Struct.CRC = Crc16(&temp, 1, ZPZ_Base_Request.Struct.CRC);
		// Отправляем считанный байт
		UARTSendByte_by_SLIP (ZPZ_UART, 0xFFFF, temp);
	}
	
	// Отправим контрольную сумму
	for(i = 7; i < 9; i++)
	{
		// Отправляем контрольную сумму
		UARTSendByte_by_SLIP (ZPZ_UART, 0xFFFF, ZPZ_Base_Request.Buffer[i]);
	}
	// Ждем завершения передачи
	while ((UART_GetFlagStatus (ZPZ_UART, UART_FLAG_TXFF) == SET) && (timeout != 0xFFF)) timeout++;
	//В конце посылаем признак конца пакета
	SendFEND(ZPZ_UART, 0xFFF);
}



//-----------------------------------------Обслуживание команды MAP_UPLOAD---------------------------------------------------------*/
uint16_t ZPZ_Request_MAP_UPLOAD (uint16_t CRC)
{
	return ZPZ_RequestWithEmptyData(CRC);
}

void ZPZ_Response_MAP_UPLOAD (uint16_t NumPacket)
{
	ZPZ_BasePacket_Union ZPZ_Base_Request;

	uint8_t  temp;
	uint16_t i, timeout = 0;
	uint32_t Address;
	
	// Заполняем структуру ответа и сразу начнем считать контрольную сумму
	ZPZ_Base_Request.Struct.Handler     = HANDLER_BU;
	ZPZ_Base_Request.Struct.PacketSize  = 0x0324;
	ZPZ_Base_Request.Struct.Command     = MAP_UPLOAD;
	ZPZ_Base_Request.Struct.Count       = NumPacket;
	ZPZ_Base_Request.Struct.CRC         = Crc16(&ZPZ_Base_Request.Buffer[0], 7, CRC16_INITIAL_FFFF);
	
		// Если FIFO занято - ждем освобождения
	while ((UART_GetFlagStatus (ZPZ_UART, UART_FLAG_TXFF) == SET) && (timeout != 0xFFFF)) timeout++;
	// Посылаем признак начала пакета
	SendFEND(ZPZ_UART, 0xFFF);
	
	// Отправим базовую часть пакета
	for(i = 0; i < 7; i++)
	{
		// Отправляем байт за байтом основной части пакета
		UARTSendByte_by_SLIP (ZPZ_UART, 0xFFFF, ZPZ_Base_Request.Buffer[i]);
	}

	// Отправляем пустой байт (как в протоколе) и продолжаем считать контрольную сумму
	temp = 0;
	ZPZ_Base_Request.Struct.CRC = Crc16(&temp, 1, ZPZ_Base_Request.Struct.CRC);
	UARTSendByte_by_SLIP (ZPZ_UART, 0xFFFF, temp);

	// Далее будем читать внешнюю SPI память побайтно и сразу отправлять
	for(i = 0; i < PACKETnMAP_DATASIZE; i++)
	{
		// Определяем адрес с помощью макроса
		Address = nMAP_ADR(NumPacket);
		// Читаем байт
		ZPZ_ReadIntoCSnUnion(Address + i, &temp, 1);
		// Пересчитываем с его учетом контрольную сумму
		ZPZ_Base_Request.Struct.CRC = Crc16(&temp, 1, ZPZ_Base_Request.Struct.CRC);
		// Отправляем считанное
		UARTSendByte_by_SLIP (ZPZ_UART, 0xFFFF, temp);
	}
	
	// Отправим контрольную сумму
	for(i = 7; i < 9; i++)
	{
		UARTSendByte_by_SLIP (ZPZ_UART, 0xFFFF, ZPZ_Base_Request.Buffer[i]);
	}
	
	// Ждем завершения передачи
	while ((UART_GetFlagStatus (ZPZ_UART, UART_FLAG_TXFF) == SET) && (timeout != 0xFFF)) timeout++;
	
	//В конце посылаем признак конца пакета
	SendFEND(ZPZ_UART, 0xFFF);
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
	BIM_Data->Struct.Side = UARTReceiveByte_by_SLIP(ZPZ_UART,0xFFFF);
	BIM_Data->Struct.State = UARTReceiveByte_by_SLIP(ZPZ_UART,0xFFFF);
	BIM_Data->Struct.Position = UARTReceiveByte_by_SLIP(ZPZ_UART,0xFFFF);
	
	// Считаем контрольную сумму, используя контрольную сумму основной части пакета CRC 
	crc = Crc16(&BIM_Data->Buffer[0], 3, CRC);
	
	return crc;
}

void ZPZ_Response_BIM_CONTROL (ZPZ_RequestControlBIM_Union* BIM_Data, uint16_t NumPacket)
{
	uint16_t BIM_Side;
	
	// Определяем правым или левым БИМом будем управлять
	if (BIM_Data->Struct.Side == L_BIM) 
		BIM_Side =  DEVICE_101;
	else if (BIM_Data->Struct.Side == R_BIM) 
		BIM_Side = DEVICE_100;
	else 
		BIM_Side = 0xFFFF;
	// Отправляем запрос на управление с параметрами
	BIM_SendRequest (BIM_Side, BIM_Data->Struct.State, BIM_Data->Struct.Position, 7, 255, 255);
	// Получим ответ, чтобы убедиться, что все в порядке
	BIM_ReceiveResponse (BIM_Side);
	// Теперь нужно ответить по ЗПЗ
	ZPZ_ShortResponse(BIM_CONTROL, NumPacket, SUCCES);
}



//-----------------------------------------Обслуживание команды BIM_STATUS---------------------------------------------------------*/
uint16_t ZPZ_Request_BIM_STATUS (uint16_t CRC, ZPZ_RequestControlBIM_Union* BIMControl)	
{
	uint16_t crc = 0;
	// Принимаем байт выбора БИМа
	BIMControl->Struct.State = 0;
	BIMControl->Struct.Position = 0;
	BIMControl->Struct.Side = UARTReceiveByte_by_SLIP(ZPZ_UART, 0xFFFF);
	crc = Crc16(&BIMControl->Struct.Side, 1, CRC);
	
	return crc;
}

void ZPZ_Response_BIM_STATUS(uint8_t Side, uint16_t NumPacket)
{
	ZPZ_ResponseStatusBIM_Union   ZPZ_BIM_Status;
	ZPZ_BasePacket_Union          ZPZ_BaseResponse;
	uint16_t BIM_Side, i;
	
	// Определяем правым или левым БИМом будем управлять
	if (Side == L_BIM) 
		BIM_Side =  DEVICE_101;
	else if (Side == R_BIM) 
		BIM_Side = DEVICE_100;
	// Это случай ошибки, адрес выбран произвольно 
	else 
		BIM_Side = 0xFFFF;
	
	// Спросим состояние БИМов
	BIM_ReceiveResponse (BIM_Side);
	
	// Заполняем информационные поля 
	ZPZ_BIM_Status.Struct.Current  = BIM_GetCurrent (BIM_Side);
	ZPZ_BIM_Status.Struct.Voltage  = BIM_GetVoltage (BIM_Side);
	ZPZ_BIM_Status.Struct.Speed    = BIM_GetSpeed (BIM_Side);
	ZPZ_BIM_Status.Struct.Position = BIM_GetStrapPosition (BIM_Side);
	ZPZ_BIM_Status.Struct.Err      = BIM_GetStatusFlags(BIM_Side);
	ZPZ_BIM_Status.Struct.Line     = LINE_MAIN;
	ZPZ_BIM_Status.Struct.Side     = BIM_Side;
	ZPZ_BIM_Status.Struct.Error    = SUCCES;
	ZPZ_BIM_Status.Struct.ElAngle  = 0;
	
	// Заполняем структуру базовых полей всех пакетов
	ZPZ_BaseResponse.Struct.Handler    = HANDLER_BU;
	ZPZ_BaseResponse.Struct.PacketSize = 0x13;
	ZPZ_BaseResponse.Struct.Command    = BIM_STATUS;
	ZPZ_BaseResponse.Struct.Count      = NumPacket;
	
	// Считаем контрольную сумму
	ZPZ_BaseResponse.Struct.CRC = Crc16(&ZPZ_BaseResponse.Buffer[0], 7, CRC16_INITIAL_FFFF);
	ZPZ_BaseResponse.Struct.CRC = Crc16(&ZPZ_BIM_Status.Buffer[0], 10, ZPZ_BaseResponse.Struct.CRC);
	
	// Сначала отправляем признак-разделитель начала пакета
	SendFEND(ZPZ_UART, 0xFFF);
	// Отправляем первичную часть пакета
	for(i = 0; i < 7; i++)
		UARTSendByte_by_SLIP (ZPZ_UART, 0xFFFF, ZPZ_BaseResponse.Buffer[i]);
	// Отправляем информационную часть пакета
	for(i = 0; i < 10; i++)
		UARTSendByte_by_SLIP (ZPZ_UART, 0xFFFF, ZPZ_BIM_Status.Buffer[i]);
	// И в конце сверху посылаем контрольную сумму
	for(i = 0; i < 2; i++)
		UARTSendByte_by_SLIP (ZPZ_UART, 0xFFFF, ZPZ_BaseResponse.Buffer[i]);
	
	// И в конце опять разделитель
	SendFEND(ZPZ_UART, 0xFFF);
}



//-----------------------------------------Обслуживание команды LOG_FORMAT---------------------------------------------------------*/
uint16_t ZPZ_Request_LOG_FORMAT (uint16_t CRC)
{
	return ZPZ_RequestWithEmptyData(CRC);
}

void ZPZ_Response_LOG_FORMAT (uint16_t NumPacket)
{
	// Запрос на форматирование накопителя черного ящика
	// Отвечаем, что приняли команду и переходим на её выполнение
	ZPZ_ShortResponse(LOG_FORMAT, NumPacket, 0x00);
	// Запускаем форматирование
	LogFs_Formatting();
	// И сообщаем о готовности
	ZPZ_ShortResponse(LOG_FORMAT, NumPacket, 0xFF);
	// После форматирование необходимо обновить информацию о файловой системе
	LogFs_Info();
}



//-----------------------------------------Обслуживание команды LOG_FILES---------------------------------------------------------*/
uint16_t ZPZ_Request_LOG_FILES (uint16_t CRC)
{
	return ZPZ_RequestWithEmptyData(CRC);
}

void ZPZ_Response_LOG_FILES (uint16_t NumPacket)
{
	ZPZ_BasePacket_Union  ZPZ_BaseResponse;
	uint8_t  buffer[4];
	uint16_t files = 0;
	uint8_t  result = 0;
	uint8_t  i, j;
	
	// Заполняем структуру общей части всех пакетов
	ZPZ_BaseResponse.Struct.Handler    = HANDLER_BU;
	ZPZ_BaseResponse.Struct.Command    = LOG_FILES;
	ZPZ_BaseResponse.Struct.Count      = NumPacket;
	// Минимально возможное число байт пакета: 
	// Команда (1б) + Номер пакета (2б) + Кол-во файлов (2б)
	// Остальное зависит от количества файлов в накопителе
	ZPZ_BaseResponse.Struct.PacketSize = 5;
	// Тогда определим количество файлов и прибавим (для каждого файла отводится 4 байта сообщения)
	files = LogFs_GetFileNum();
	ZPZ_BaseResponse.Struct.PacketSize += 4*files;
	
	// Теперь посчитаем контрольную сумму начала пакета (первые 7 байт)
	ZPZ_BaseResponse.Struct.CRC = Crc16(&ZPZ_BaseResponse.Buffer[0], 7, CRC16_INITIAL_FFFF);
	
	// Начнём отправлять сообщение
	// Сначала отправляем признак-разделитель начала пакета
	SendFEND(ZPZ_UART, 0xFFF);
	// Теперь первые 7 байт 
	for(i = 0; i < 7; i++)
		UARTSendByte_by_SLIP (ZPZ_UART, 0xFFFF, ZPZ_BaseResponse.Buffer[i]);
	
	// Готовим к отправке число файлов (2 байта)
	*(uint16_t*)buffer = (uint16_t)files;
	// Подсчитываем контрольную сумму с учетом этого
	ZPZ_BaseResponse.Struct.CRC = Crc16(buffer, 2, 	ZPZ_BaseResponse.Struct.CRC);
	// Отправляем эти два байта с числом файлов
	for(i = 0; i < 2; i++)
		UARTSendByte_by_SLIP (ZPZ_UART, 0xFFFF, buffer[i]);
	
	
	// Определение, упаковка, подсчет контрольной суммы и отправка остальной части пакета (номера файлов и размеры)
	for(j = 0; j < files; j++)
	{	
		// Если первая итерация то, (как бы устанавливаем точку отсчета)
		// Ищем первый (самый старый файл в директории)
		if(j == 0) 
			result = Log_Fs_FindFIle(FIRST_FILE);
		// При последующих итерациях нужно просто прокручивать файлы 
		// Командой "NEXT_FILE"
		else 
			result = Log_Fs_FindFIle(NEXT_FILE);
		
		// Узнаем его номер и сразу упакуем в буфер для отправки
		*(uint16_t*)buffer = Log_Fs_GetFileProperties (FILE_NUMBER);
		// Узнаем его размер и сразу упакуем в буфер для отправки
		*((uint16_t*)buffer+1) = Log_Fs_GetFileProperties (FILE_SIZE);
		// Подсчитываем контрольную сумму с учетом их
		ZPZ_BaseResponse.Struct.CRC = Crc16(buffer, 4, 	ZPZ_BaseResponse.Struct.CRC);
		// Отправляем эти 4 байта информации о файле
		for(i = 0; i < 4; i++)
			UARTSendByte_by_SLIP (ZPZ_UART, 0xFFFF, buffer[i]);
		
		// Дополнительная проверка по выходу из цикла
		// Если все файлы были просмотрены, функция Log_Fs_FindFIle() возвращает FS_ALL_FILES_SCROLLS
		// Проконтролируем это
		if (result ==  FS_ALL_FILES_SCROLLS)
			break;
	}
	
	// После сверху посылаем контрольную сумму
	for(i = 0; i < 2; i++)
		UARTSendByte_by_SLIP (ZPZ_UART, 0xFFFF, ZPZ_BaseResponse.Buffer[i]);
	// И в конце опять разделитель
	SendFEND(ZPZ_UART, 0xFFF);
}



//-----------------------------------------Обслуживание команды LOG_UPLOAD---------------------------------------------------------*/
uint16_t ZPZ_Request_LOG_UPLOAD(uint16_t* NUM, uint16_t CRC)
{
	uint8_t temp = 0;
	uint16_t crc = 0;
	
	// Допринимаем остальные два байта с номером файла
	// который просят отправить
	temp = UARTReceiveByte_by_SLIP(ZPZ_UART, 0xFFFF);
	crc = Crc16(&temp, 1, CRC);
	*NUM = temp;
	temp = UARTReceiveByte_by_SLIP(ZPZ_UART, 0xFFFF);
	crc = Crc16(&temp, 1, crc);
	*NUM |= (uint8_t)(temp << 8);
	
	return crc;
}

uint8_t ZPZ_Response_LOG_UPLOAD(uint16_t File_num, uint16_t NumPacket)
{
	ZPZ_BasePacket_Union  ZPZ_BaseResponse;
	uint8_t  temp[2];
	uint16_t filesize;
	uint16_t packet_count = 0;
	uint32_t i;
	
	// Ищем файл в хранилище по номеру
	// Если функция не найдет файл, то выдаст ошибку, тогда можно не продолжать
	if(Log_Fs_FindFile_ByNum(File_num)!= FS_FINE)
	{
		// Ответим ошибкой
		ZPZ_ShortResponse(LOG_UPLOAD, 0, FAIL);
		return 1;
	}

	// Узнаем размер файла
  filesize = Log_Fs_GetFileProperties(FILE_SIZE);

	
	// Заполняем структуру общей части всех пакетов
	ZPZ_BaseResponse.Struct.Handler    = HANDLER_BU;
	ZPZ_BaseResponse.Struct.Command    = LOG_UPLOAD;
	ZPZ_BaseResponse.Struct.Count      = NumPacket;
	// Размер пакета всегда не меньше 5 байт (Код команды, Номер пакета, Номер файла)
	ZPZ_BaseResponse.Struct.PacketSize = 5;
	
	// Если номер пакета нулевой, то запрашивается количество пакетов, 
	// на которые будет разбит файл с номером File_num
	if(NumPacket == 0)
	{
		// Узнаем на сколько пакетов по 100 байт разобьётся файл 
		packet_count = (uint16_t)((filesize/100.0));
		if (filesize % 100 != 0)
			packet_count++;
		
		ZPZ_BaseResponse.Struct.PacketSize += 2;
		// Теперь посчитаем контрольную сумму начала пакета (первые 7 байт)
	  ZPZ_BaseResponse.Struct.CRC = Crc16(&ZPZ_BaseResponse.Buffer[0], 7, CRC16_INITIAL_FFFF);
		// Начнём отправлять сообщение
	  // Сначала отправляем признак-разделитель начала пакета
	  SendFEND(ZPZ_UART, 0xFFF);
	  // Теперь первые 7 байт 
	  for(i = 0; i < 7; i++)
			UARTSendByte_by_SLIP (ZPZ_UART, 0xFFFF, ZPZ_BaseResponse.Buffer[i]);
		
		// Отправляем номер файла
	  *(uint16_t*)temp = File_num;
	  // Подсчитываем контрольную сумму с учетом этого
	  ZPZ_BaseResponse.Struct.CRC = Crc16(temp, 2, 	ZPZ_BaseResponse.Struct.CRC);
	  // Отправляем номер файла
	  for(i = 0; i < 2; i++)
			UARTSendByte_by_SLIP (ZPZ_UART, 0xFFFF, temp[i]);
		
		// Отправляем количество пакетов файла
	  *(uint16_t*)temp = packet_count;
	  // Подсчитываем контрольную сумму с учетом этого
	  ZPZ_BaseResponse.Struct.CRC = Crc16(temp, 2, 	ZPZ_BaseResponse.Struct.CRC);
	  // Отправляем номер файла
	  for(i = 0; i < 2; i++)
			UARTSendByte_by_SLIP (ZPZ_UART, 0xFFFF, temp[i]);			
	}
	
	// Если не нулевой номер пакета, то уже запращивается информация из файла
	// Причем конкретно пакет (на которые был разбит файл) с номером NumPacket
	else
	{
		// Нужно проверить есть ли вообще такой номер пакета
		
		// Узнаем на сколько пакетов по 100 байт разобьётся файл 
		packet_count = (uint16_t)((filesize/100.0));
		if (filesize % 100 != 0)
			packet_count++;
		
		// Если запрашиваемый номер пакета не существует
		if(NumPacket > packet_count)
		{
			// Ответим ошибкой
			ZPZ_ShortResponse(LOG_UPLOAD, NumPacket, FAIL);
			return 1;
		}
		
		// Такой пакет определен
		// Готовимся к отправке 
		
		// Если запрашиваемый пакет является крайним (либо единственным)
		if(packet_count - NumPacket == 0)
		{
			// Увеличиваем размер на оставшуюся часть файла
			ZPZ_BaseResponse.Struct.PacketSize += filesize - (packet_count - 1) * 100;
			
			// Теперь посчитаем контрольную сумму начала пакета (первые 7 байт)
			ZPZ_BaseResponse.Struct.CRC = Crc16(&ZPZ_BaseResponse.Buffer[0], 7, CRC16_INITIAL_FFFF);
			// Начнём отправлять сообщение
			// Сначала отправляем признак-разделитель начала пакета
			SendFEND(ZPZ_UART, 0xFFF);
			// Теперь первые 7 байт 
			for(i = 0; i < 7; i++)
				UARTSendByte_by_SLIP (ZPZ_UART, 0xFFFF, ZPZ_BaseResponse.Buffer[i]);
			
			// Отправляем номер файла
			*(uint16_t*)temp = File_num;
			// Подсчитываем контрольную сумму с учетом этого
			ZPZ_BaseResponse.Struct.CRC = Crc16(temp, 2, 	ZPZ_BaseResponse.Struct.CRC);
			// Отправляем номер файла
			for(i = 0; i < 2; i++)
				UARTSendByte_by_SLIP (ZPZ_UART, 0xFFFF, temp[i]);
		
			// Нужно совершить мнимое считывание от начала файла до нужной позиции, 
			// чтобы сместить на неё указатель внутри функции чтения файла 
			// То есть нужно прочитать NumPacket - 1 раз по 100 байт
			for (i = 0; i < (NumPacket - 1) * 100; i++)
			{
				// Не используем буферизацию, поэтому читаем побайтово
				LogFs_ReadFile(&temp[0], 1);
			}
			
			// Сместили указатель внутри функции чтения, теперь готовы читать нужную часть 
			for(i = 0; i < (filesize - (packet_count - 1) * 100); i++)
			{
				// Будем читать и отправлять побайтово, не забывая пересчитывать контрольную сумму
				LogFs_ReadFile(&temp[0], 1);
				// Сразу подсчитываем контрольную сумму
			  ZPZ_BaseResponse.Struct.CRC = Crc16(&temp[0], 1, 	ZPZ_BaseResponse.Struct.CRC);
			  // И сразу отправляем
			  UARTSendByte_by_SLIP (ZPZ_UART, 0xFFFF, temp[0]);
			}
		}
		// Пакет не крайний 
		else
		{
			// Увеличиваем размер на 100 байт данных
			ZPZ_BaseResponse.Struct.PacketSize += 100;
			
			// Теперь посчитаем контрольную сумму начала пакета (первые 7 байт)
			ZPZ_BaseResponse.Struct.CRC = Crc16(&ZPZ_BaseResponse.Buffer[0], 7, CRC16_INITIAL_FFFF);
			// Начнём отправлять сообщение
			// Сначала отправляем признак-разделитель начала пакета
			SendFEND(ZPZ_UART, 0xFFF);
			// Теперь первые 7 байт 
			for(i = 0; i < 7; i++)
				UARTSendByte_by_SLIP (ZPZ_UART, 0xFFFF, ZPZ_BaseResponse.Buffer[i]);
			
			// Отправляем номер файла
			*(uint16_t*)temp = File_num;
			// Подсчитываем контрольную сумму с учетом этого
			ZPZ_BaseResponse.Struct.CRC = Crc16(temp, 2, 	ZPZ_BaseResponse.Struct.CRC);
			// Отправляем номер файла
			for(i = 0; i < 2; i++)
				UARTSendByte_by_SLIP (ZPZ_UART, 0xFFFF, temp[i]);
			
			// Далее займёмся отправкой данный пакета
			
			// Нужно совершить мнимое считывание от начала файла до нужной позиции, 
			// чтобы сместить на неё указатель внутри функции чтения файла 
			// То есть нужно прочитать NumPacket - 1 раз по 100 байт
			for (i = 0; i < (NumPacket - 1) * 100; i++)
			{
				// Не используем буферизацию, поэтому читаем побайтово
				LogFs_ReadFile(&temp[0], 1);
			}
			// Сместили указатель внутри функции чтения, теперь готовы читать нужную часть 
			for(i = 0; i < 100; i++)
			{
				// Будем читать и отправлять побайтово, не забывая пересчитывать контрольную сумму
				LogFs_ReadFile(&temp[0], 1);
				// Сразу подсчитываем контрольную сумму
			  ZPZ_BaseResponse.Struct.CRC = Crc16(&temp[0], 1, 	ZPZ_BaseResponse.Struct.CRC);
			  // И сразу отправляем
			  UARTSendByte_by_SLIP (ZPZ_UART, 0xFFFF, temp[0]);
			}
		}
	}
		
	// После сверху посылаем контрольную сумму
	for(i = 0; i < 2; i++)
		UARTSendByte_by_SLIP (ZPZ_UART, 0xFFFF, ZPZ_BaseResponse.Buffer[i]);
	// И в конце опять разделитель
	SendFEND(ZPZ_UART, 0xFFF);
	
	return 0;
}



//-----------------------------------------Обслуживание команды REQ_SWS---------------------------------------------------------*/
uint16_t ZPZ_Request_REQ_SWS (uint16_t CRC)
{
	return ZPZ_RequestWithEmptyData(CRC);
}

uint8_t ZPZ_Response_REQ_SWS (uint16_t NumPacket)
{
	ZPZ_BasePacket_Union  ZPZ_BaseResponse;
	SWS_Packet_Type_Union SWS_data;
	uint32_t timeout = 0;
	uint8_t  i;

	// Первым делом нужно получить данные с СВС
	// Готовимся к работе с СВС
	SWS_RetargetPins ();
	SWS_init ();
	// Принимаем данные. 
	while(SWS_GetPacket (&SWS_data) && (timeout < 0xFFF)) timeout ++;
	// Отключаемся от СВС и освобождаем UART
	SWS_deinit ();
	
	// Проверим был ли это выход по таймауту
	if(timeout == 0xFFF)
	{
		// Если да, то нужно ответить ошибкой
		ZPZ_ShortResponse(REQ_SWS, 0, FAIL);
		// И завершиться с ошибкой
		return 1;
	}
	
	// Если данные приняты верно, то надо продолжать
	// Заполняем структуру общей части всех пакетов
	ZPZ_BaseResponse.Struct.Handler    = HANDLER_BU;
	ZPZ_BaseResponse.Struct.PacketSize = 27;
	ZPZ_BaseResponse.Struct.Command    = REQ_SWS;
	ZPZ_BaseResponse.Struct.Count      = NumPacket;
	
  // Теперь посчитаем контрольную сумму начала пакета (первые 7 байт)
	ZPZ_BaseResponse.Struct.CRC = Crc16(&ZPZ_BaseResponse.Buffer[0], 7, CRC16_INITIAL_FFFF);
	
	// Начнём отправлять сообщение
	// Сначала отправляем признак-разделитель начала пакета
	SendFEND(ZPZ_UART, 0xFFF);
	// Теперь первые 7 байт 
	for(i = 0; i < 7; i++)
		UARTSendByte_by_SLIP (ZPZ_UART, 0xFFFF, ZPZ_BaseResponse.Buffer[i]);
	
	// Теперь будем отправлять информацию от СВС
	// Нас интересует данные начиная со статического давления, кроме резервных полей и CRC
	// Начинаем отправлять с давления, поэтому с 8 до истинной скорости до 28 байты
	for(i = 8; i < 27; i++)
	{
		// Сразу подсчитываем контрольную сумму
		ZPZ_BaseResponse.Struct.CRC = Crc16(&SWS_data.Buffer[i], 1, 	ZPZ_BaseResponse.Struct.CRC);
		// И отправляем по байтам
		UARTSendByte_by_SLIP (ZPZ_UART, 0xFFFF, SWS_data.Buffer[i]);
	}
	
	// Так же отправляем 4 байт состояния СВС (с 52 по 56 байт)
	for(i = 52; i < 56; i++)
	{
		// Сразу подсчитываем контрольную сумму
		ZPZ_BaseResponse.Struct.CRC = Crc16(&SWS_data.Buffer[i], 1, 	ZPZ_BaseResponse.Struct.CRC);
		// И отправляем по байтам
		UARTSendByte_by_SLIP (ZPZ_UART, 0xFFFF, SWS_data.Buffer[i]);
	}
	
	// После сверху посылаем контрольную сумму
	for(i = 0; i < 2; i++)
		UARTSendByte_by_SLIP (ZPZ_UART, 0xFFFF, ZPZ_BaseResponse.Buffer[i]);
	// И в конце опять разделитель
	// Если FIFO передатчика заполнен, то ждем пока освободится
	SendFEND(ZPZ_UART, 0xFFF);
	
	return 0;
}



//-----------------------------------------Обслуживание команды REQ_SNS_POS---------------------------------------------------------*/
uint16_t ZPZ_Request_REQ_SNS_POS (uint16_t CRC)
{
	return ZPZ_RequestWithEmptyData(CRC);
}

uint8_t ZPZ_Response_REQ_SNS_POS (uint16_t NumPacket)
{
	ZPZ_BasePacket_Union                    ZPZ_BaseResponse;
	SNS_Position_Data_Response_Union				SNS_Position_Data_Response;
	SNS_Orientation_Data_Response_Union			SNS_Orientation_Data_Response;
	uint32_t timeout = 0;
	uint8_t  i;
	

	// Запрашиваем данные местоположения
	while(!SNS_GetPositionData(&SNS_Position_Data_Response)&& (timeout != 0xFFFF)) timeout ++;
	// Запрашиваем данные ориентации
	while(!SNS_GetOrientationData(&SNS_Orientation_Data_Response)&& (timeout != 0xFFFF)) timeout ++;

	// Проверим, вдруг выход был по таймауту, тогда нужно ответить ошибкой и завершиться
	if (timeout == 0xFFFF)
	{
	  // Если да, то нужно ответить ошибкой
		ZPZ_ShortResponse(REQ_SNS_POS, 0, FAIL);
		// И завершиться с ошибкой
		return 1;
	}
	// Иначе продолжаем
	// Заполняем структуру общей части всех пакетов
	ZPZ_BaseResponse.Struct.Handler    = HANDLER_BU;
	ZPZ_BaseResponse.Struct.PacketSize = 71;
	ZPZ_BaseResponse.Struct.Command    = REQ_SNS_POS;
	ZPZ_BaseResponse.Struct.Count      = NumPacket;	
	// Теперь посчитаем контрольную сумму начала пакета (первые 7 байт)
	ZPZ_BaseResponse.Struct.CRC = Crc16(&ZPZ_BaseResponse.Buffer[0], 7, CRC16_INITIAL_FFFF);
	// Начнём отправлять сообщение
	// Сначала отправляем признак-разделитель начала пакета
	SendFEND(ZPZ_UART, 0xFFF);
	// Теперь первые 7 байт 
	for(i = 0; i < 7; i++)
		UARTSendByte_by_SLIP (ZPZ_UART, 0xFFFF, ZPZ_BaseResponse.Buffer[i]);
	
	// Высылаем данные местоположения
	// Из полученного пакета с местоположением от СНС выкидываем CRC, тип ответа и счетчик в начале, поэтому i = 4
	for (i = 4; i < SizeAnsPDR; i++)
	{
		// Побайтово досчитываем контрольную сумму
		ZPZ_BaseResponse.Struct.CRC = Crc16(&SNS_Position_Data_Response.Buffer[i], 1, ZPZ_BaseResponse.Struct.CRC);
		// и отправляем
		UARTSendByte_by_SLIP (ZPZ_UART, 0xFFFF,	SNS_Position_Data_Response.Buffer[i]);
	}
	
	// Высылаем данные ориентации
	// из полученного пакета с ориентацией от СНС выкидываем CRC, тип ответа и счетчик в начале, поэтому i = 4
	for (i = 4; i < SizeAnsODR; i++)
	{
		// Побайтово досчитываем контрольную сумму
		ZPZ_BaseResponse.Struct.CRC = Crc16(&SNS_Orientation_Data_Response.Buffer[i], 1, ZPZ_BaseResponse.Struct.CRC);
		// и отправляем
		UARTSendByte_by_SLIP (ZPZ_UART, 0xFFFF,	SNS_Orientation_Data_Response.Buffer[i]);
	}
	
	// После сверху посылаем контрольную сумму
	for(i = 0; i < 2; i++)
		UARTSendByte_by_SLIP (ZPZ_UART, 0xFFFF, ZPZ_BaseResponse.Buffer[i]);
	// И в конце опять разделитель
	SendFEND(ZPZ_UART, 0xFFF);
	
	return 0;
}



//-----------------------------------------Обслуживание команды REQ_SNS_STATE---------------------------------------------------------*/
uint16_t ZPZ_Request_REQ_SNS_STATE (uint16_t CRC)
{
	return ZPZ_RequestWithEmptyData(CRC);
}

uint8_t ZPZ_Response_REQ_SNS_STATE (uint16_t NumPacket)
{
	ZPZ_BasePacket_Union                   ZPZ_BaseResponse;
	SNS_Device_Information_Response_Union  SNS_DevInfo;
	SNS_Available_Data_Response_Union      SNS_AvailableData;
	uint32_t timeout = 0;
	uint8_t  i;
	
	// Получим данные об устройстве
  while(!SNS_GetDeviceInformation(&SNS_DevInfo)&& (timeout != 0xFFFF)) timeout ++;
	// Получим информацию о доступных данных 
  while(!SNS_GetDataState(&SNS_AvailableData)&& (timeout != 0xFFFF)) timeout ++;

	// Проверим, вдруг выход был по таймауту, тогда нужно ответить ошибкой и завершиться
	if (timeout == 0xFFFF)
	{
	  // Если да, то нужно ответить ошибкой
		ZPZ_ShortResponse(REQ_SNS_STATE, 0, FAIL);
		// И завершиться с ошибкой
		return 1;
	}
	
	// Иначе продолжаем
	// Заполняем структуру общей части всех пакетов
	ZPZ_BaseResponse.Struct.Handler    = HANDLER_BU;
	ZPZ_BaseResponse.Struct.PacketSize = 22;
	ZPZ_BaseResponse.Struct.Command    = REQ_SNS_STATE;
	ZPZ_BaseResponse.Struct.Count      = NumPacket;	
	// Теперь посчитаем контрольную сумму начала пакета (первые 7 байт)
	ZPZ_BaseResponse.Struct.CRC = Crc16(&ZPZ_BaseResponse.Buffer[0], 7, CRC16_INITIAL_FFFF);
	// Начнём отправлять сообщение
	// Сначала отправляем признак-разделитель начала пакета
	SendFEND(ZPZ_UART, 0xFFF);
	// Теперь первые 7 байт 
	for(i = 0; i < 7; i++)
		UARTSendByte_by_SLIP (ZPZ_UART, 0xFFFF, ZPZ_BaseResponse.Buffer[i]);
	
	
	// Высылаем данные об устройстве
	// из полученного пакета с информацией об устройсстве от СНС выкидываем CRC, тип ответа и счетчик в начале, поэтому i = 4;
	for (i = 4; i < SizeAnsDIR; i++)
	{
		// Побайтово досчитываем контрольную сумму
		ZPZ_BaseResponse.Struct.CRC = Crc16(&SNS_DevInfo.Buffer[i], 1, ZPZ_BaseResponse.Struct.CRC);
		// и отправляем
		UARTSendByte_by_SLIP (ZPZ_UART, 0xFFFF,	SNS_DevInfo.Buffer[i]);
	}

	// Высылаем инфу о доступных данных
	// из полученного пакета с доступными данными от СНС необходимо только поле Data_State
	// Побайтово досчитываем контрольную сумму
	ZPZ_BaseResponse.Struct.CRC = Crc16(&SNS_AvailableData.Struct.Data_State, 1, ZPZ_BaseResponse.Struct.CRC);
	// и отправляем
	UARTSendByte_by_SLIP (ZPZ_UART, 0xFFFF,	SNS_AvailableData.Struct.Data_State);
	
	// После сверху посылаем контрольную сумму
	for(i = 0; i < 2; i++)
		UARTSendByte_by_SLIP (ZPZ_UART, 0xFFFF, ZPZ_BaseResponse.Buffer[i]);
	// И в конце опять разделитель
	SendFEND(ZPZ_UART, 0xFFF);
	
	return 0;	
}



//-----------------------------------------Обслуживание команды REQ_SNS_SETTINGS---------------------------------------------------------*/
uint16_t ZPZ_Request_REQ_SNS_SETTINGS(uint16_t CRC, uint8_t* buffer)
{
	uint8_t i;
	for(i = 0; i < 3; i++)
	{
		buffer[i] = UARTReceiveByte_by_SLIP(ZPZ_UART, 0xFFFF);
	}
	CRC = Crc16(buffer, 3, CRC);

	return CRC;
}

// Реагируем на запрос на изменение настроек СНС
uint8_t ZPZ_Response_REQ_SNS_SETTINGS (uint8_t* params, uint16_t NumPacket)
{
	ZPZ_BasePacket_Union     ZPZ_BaseResponse;
	uint8_t  i;
		
	// Анализируем пришедшую команду и выполняем действия
	
	// Команда включения горизонтальной коррекции
	if(params[0]== 0x01)
		SNS_EnableHorizontalCorrection();
	
	// Команда отключения горизонтальной коррекции
	else if (params[0] == 0x02) 
		SNS_DisableHorizontalCorrection();
	
	// Команда начала калибровки гироскопа
	if(params[1]== 0x01) 
		SNS_StartGyroCalibration();
	
	// Команда сброса калибровки гироскопа
	else if (params[1] == 0x02)
		SNS_ResetGyroCalibration();
	
	// Команда начала калибровки магнитометра
	if(params[2]== 0x01)
		SNS_StartMagnetometerCalibration();
	
	// Команда сброса калибровки магнитометра
	else if (params[2] == 0x02)
		SNS_ResetMagnetometerCalibration();

	
	// Теперь нужно ответить 
	// Заполняем структуру общей части всех пакетов
	ZPZ_BaseResponse.Struct.Handler    = HANDLER_BU;
	ZPZ_BaseResponse.Struct.PacketSize = 6;
	ZPZ_BaseResponse.Struct.Command    = REQ_SNS_SETTINGS;
	ZPZ_BaseResponse.Struct.Count      = NumPacket;	
	// Теперь посчитаем контрольную сумму начала пакета (первые 7 байт)
	ZPZ_BaseResponse.Struct.CRC = Crc16(&ZPZ_BaseResponse.Buffer[0], 7, CRC16_INITIAL_FFFF);
	// Начнём отправлять сообщение
	// Сначала отправляем признак-разделитель начала пакета
	SendFEND(ZPZ_UART, 0xFFF);
	// Теперь первые 7 байт 
	for(i = 0; i < 7; i++)
		UARTSendByte_by_SLIP (ZPZ_UART, 0xFFFF, ZPZ_BaseResponse.Buffer[i]);
	
	// В ответе нужно продублировать (повторить) настройки (3 байта)
	for (i = 0; i < 3; i++)
	{
		// Побайтово досчитываем контрольную сумму
		ZPZ_BaseResponse.Struct.CRC = Crc16(params, 3, ZPZ_BaseResponse.Struct.CRC);
		// и отправляем
		UARTSendByte_by_SLIP (ZPZ_UART, 0xFFFF,	params[i]);
	}
	
	// После сверху посылаем контрольную сумму
	for(i = 0; i < 2; i++)
		UARTSendByte_by_SLIP (ZPZ_UART, 0xFFFF, ZPZ_BaseResponse.Buffer[i]);
	// И в конце опять разделитель
	SendFEND(ZPZ_UART, 0xFFF);
	
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
	SendFEND(ZPZ_UART, 0xFFF);
	// Теперь первые 7 байт 
	for(i = 0; i < 7; i++)
		UARTSendByte_by_SLIP (ZPZ_UART, 0xFFFF, ZPZ_BaseResponse.Buffer[i]);
	
	// Досчитаем контрольную сумму с учётом buff
	ZPZ_BaseResponse.Struct.CRC = Crc16(buff, 3, ZPZ_BaseResponse.Struct.CRC);
	// Отправляем содержимое buff
	for(i = 0; i < 3; i++)
		UARTSendByte_by_SLIP (ZPZ_UART, 0xFFFF, buff[i]);
	
	// После сверху посылаем контрольную сумму
	for(i = 0; i < 2; i++)
		UARTSendByte_by_SLIP (ZPZ_UART, 0xFFFF, ZPZ_BaseResponse.Buffer[i]);
	// И в конце опять разделитель
	SendFEND(ZPZ_UART, 0xFFF);
}


//-----------------------------------------Обслуживание команды SYSTEM_STATE---------------------------------------------------------*/
uint16_t ZPZ_Request_SYSTEM_STATE (uint16_t CRC)
{
	return ZPZ_RequestWithEmptyData(CRC);
}

void ZPZ_Response_SYSTEM_STATE (uint16_t NumPacket)
{
//	ZPZ_BasePacket_Union     ZPZ_BaseResponse;
//	uint32_t timeout = 0;
//	uint8_t  i;
//	uint8_t  buff[3];
	
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
		buffer[i] = UARTReceiveByte_by_SLIP(ZPZ_UART, 0xFFFF);
	}
	for(i = 3; i < buffer[2]+3; i++)
	{
		buffer[i] = UARTReceiveByte_by_SLIP(ZPZ_UART, 0xFFFF);
	}
	CRC = Crc16(buffer, 3 + buffer[2], CRC);

	return CRC;
}
void ZPZ_Response_CAN_TRANSMIT (uint8_t* buffer, uint16_t NumPacket)
{
	ZPZ_BasePacket_Union     ZPZ_BaseResponse;
	CAN_TxMsgTypeDef         CANTxMsg;
	uint32_t timeout = 0;
	uint8_t  i;
	uint8_t  buff[3];
	uint32_t Buffer_number;

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
	Buffer_number = CAN_GetEmptyTransferBuffer (ZPZ_CAN);
	// Кладём сообщение в нужный буфер и ждем отправки
	CAN_Transmit(ZPZ_CAN, Buffer_number, &CANTxMsg);
	// Ожидаем конца передачи, либо превышения времени ожидания
	while(((CAN_GetBufferStatus(ZPZ_CAN, Buffer_number) & CAN_STATUS_TX_REQ) != RESET) && (timeout != 0xFFFF))
		timeout++;
	
	// Теперь нужно ответить 
	// Заполняем структуру общей части всех пакетов
	ZPZ_BaseResponse.Struct.Handler    = HANDLER_BU;
	// 6 Байт обязательны
	ZPZ_BaseResponse.Struct.PacketSize = 6 + CANTxMsg.DLC; 
	ZPZ_BaseResponse.Struct.Command    = CAN_TRANSMIT;
	ZPZ_BaseResponse.Struct.Count      = NumPacket;	
	// Теперь посчитаем контрольную сумму начала пакета (первые 7 байт)
	ZPZ_BaseResponse.Struct.CRC = Crc16(&ZPZ_BaseResponse.Buffer[0], 7, CRC16_INITIAL_FFFF);
	// Начнём отправлять сообщение
	// Сначала отправляем признак-разделитель начала пакета
	SendFEND(ZPZ_UART, 0xFFF);
	// Теперь первые 7 байт 
	for(i = 0; i < 7; i++)
		UARTSendByte_by_SLIP (ZPZ_UART, 0xFFFF, ZPZ_BaseResponse.Buffer[i]);
	
	// Досчитаем контрольную сумму с учётом buff
	ZPZ_BaseResponse.Struct.CRC = Crc16(buffer, 3 + CANTxMsg.DLC, ZPZ_BaseResponse.Struct.CRC);
	// Отправляем содержимое buffer
	for(i = 0; i < 3 + CANTxMsg.DLC; i++)
		UARTSendByte_by_SLIP (ZPZ_UART, 0xFFFF, buffer[i]);
	
	// После сверху посылаем контрольную сумму
	for(i = 0; i < 2; i++)
		UARTSendByte_by_SLIP (ZPZ_UART, 0xFFFF, ZPZ_BaseResponse.Buffer[i]);
	// И в конце опять разделитель
	SendFEND(ZPZ_UART, 0xFFF);	
}
