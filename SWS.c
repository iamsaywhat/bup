#include "SWS.h"
#include "CRC32.h"
#include "otherlib.h"


/**************************************************************************************************************
    Объявления локальных функций модуля
***************************************************************************************************************/
static int16_t UARTReceiveByte (MDR_UART_TypeDef* UARTx, uint32_t TimeoutRange);
static uint8_t UARTSendByte (MDR_UART_TypeDef* UARTx, uint32_t TimeoutRange, uint16_t Byte);


/**************************************************************************************************************
    SWS_RetargetPins - Функция переопределения UART1 на другие пины, для работы SWS
***************************************************************************************************************/
void SWS_RetargetPins (void)
{
	RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTD, ENABLE);
	
	// Здесь гарантированно отключаем используемый UART, от других пинов, на которых он может использоваться 
	Pin_Init (MDR_PORTB, PORT_Pin_6, PORT_FUNC_PORT, PORT_OE_OUT);
	Pin_Init (MDR_PORTB, PORT_Pin_5, PORT_FUNC_PORT, PORT_OE_OUT);
	
	// Переназчаем UART1 на порт D для работы SWS
	Pin_Init (SWS_PORT, SWS_RX, PORT_FUNC_OVERRID,PORT_OE_IN);
	Pin_Init (SWS_PORT, SWS_TX, PORT_FUNC_OVERRID,PORT_OE_OUT);
	// Пин активации приёмопередатчика
	Pin_Init (SWS_PORT, SWS_DE, PORT_FUNC_PORT,PORT_OE_OUT);
}


/**************************************************************************************************************
    SWS_init - Запуск процедуры обмена с SWS
***************************************************************************************************************/
void SWS_init (void)
{
	UART_InitTypeDef UART_InitStructure;

	// Включаем тактирования модуля UART и порта, пины которого использует СВС
	RST_CLK_PCLKcmd(RST_CLK_PCLK_SWS_PORT| RST_CLK_PCLK_SWS_UART, ENABLE);
	// Сброс конфигуряции UART
	UART_DeInit(SWS_UART);
	// Заполнение структуры по-умолчанию
	UART_StructInit (&UART_InitStructure);
	// Установка предделителя блока UART1_CLK = HCLK
	UART_BRGInit (SWS_UART,UART_HCLKdiv1); 
	// Заполняем структуру инициализации	
	UART_InitStructure.UART_BaudRate                = BAUDRATE_SWS;
	UART_InitStructure.UART_WordLength              = UART_WordLength8b;
	UART_InitStructure.UART_StopBits                = UART_StopBits1;
	UART_InitStructure.UART_Parity                  = UART_Parity_No;
	UART_InitStructure.UART_FIFOMode                = UART_FIFO_ON;
	UART_InitStructure.UART_HardwareFlowControl     = UART_HardwareFlowControl_RXE | UART_HardwareFlowControl_TXE;
	// Инициализация UART
	UART_Init (SWS_UART,&UART_InitStructure);
		
	// Включение UART - CВС
	UART_Cmd(SWS_UART,ENABLE);
	// Устанавливаем в 0 пин DE - Режим приёмника
	PORT_ResetBits (SWS_PORT,SWS_DE);
	// Если нужен режим передатчика - устанавливаем в 1;
	//PORT_SetBits (SWS_PORT,SWS_DE); 
}

/**************************************************************************************************************
    SWS_deinit - Деинициализация СВС
***************************************************************************************************************/
void SWS_deinit (void)
{
	// Сброс конфигуряции UART
	UART_DeInit(SWS_UART);
	// Включение UART - CВС
	UART_Cmd(SWS_UART,DISABLE);
	// Устанавливаем в 0 пин DE - Режим приёмника
	PORT_ResetBits (SWS_PORT,SWS_DE);
	// Освобождаем пины по
	Pin_Init (SWS_PORT, SWS_RX, PORT_FUNC_PORT,PORT_OE_OUT);
	Pin_Init (SWS_PORT, SWS_TX, PORT_FUNC_PORT,PORT_OE_OUT);
	// Пин активации приёмопередатчика
	Pin_Init (SWS_PORT, SWS_DE, PORT_FUNC_PORT,PORT_OE_OUT);
}

/**************************************************************************************************************
    SWS_GetPacket - Получение структуры пакета от СВС
***************************************************************************************************************/
uint8_t SWS_GetPacket (SWS_Packet_Type_Union* SWS_Pack)
{
	SWS_Packet_Type_Union  Actual_SWS_Pack;      // Актуальные данные от СВС
	uint32_t timeout = 0;                        // Таймаут счетчик
	uint32_t crc = 0;                            // Контрольная сумма
	uint8_t  i;                                  // Счетчик циклов
		
	// Вычищаем FIFO от мусора
	while ((UART_GetFlagStatus (SWS_UART, UART_FLAG_RXFE) != SET) && (timeout != SWS_MAX_TIMEOUT)) 
	{
		timeout++; 
		UART_ReceiveData(SWS_UART);	
	}
	
	// Подключаемся к СВС
	SWS_RetargetPins();
	SWS_init();
	
	// Ожидаем заголовок
	while(timeout != SWS_MAX_TIMEOUT)
	{
		timeout ++;
		// Ловим первый байт заголовка
		if (UARTReceiveByte(SWS_UART, SWS_MAX_TIMEOUT) != (uint8_t)(SWS_Handler)) continue; 
		// Если первый байт заголовка не совпал, дальше не ждем, переходим к следующей итерации
		// Если совпал, ловим следующий байт заголовка
		if (UARTReceiveByte(SWS_UART, SWS_MAX_TIMEOUT) != (uint8_t)(SWS_Handler >> 8)) continue; 
		// Если совпал, ловим следующий байт заголовка
		if (UARTReceiveByte(SWS_UART, SWS_MAX_TIMEOUT) != (uint8_t)(SWS_Handler >> 16)) continue; 
		// Если совпал, ловим следующий байт заголовка
		// Если совпадёт и 4й байт, то переходим к приёму остального пакета
		if (UARTReceiveByte(SWS_UART, SWS_MAX_TIMEOUT) == (uint8_t)(SWS_Handler >> 24)) break;
	}	
	
	// Если был таймаут, связи нет, выходим
	if(timeout == SWS_MAX_TIMEOUT) 
	{
		// Отключаемся от СВС
		SWS_deinit();
		// Возврашаем ошибку
		return 1;
	}
		
	// Если попали сюда, значит связь есть, заголовок совпал, поэтому продублируем принятый заголовок в структуру
	Actual_SWS_Pack.Struct.Handler = SWS_Handler;
	// Принимаем остальную часть пакета
	for(i = 4; i < 56; i++)
	{
		Actual_SWS_Pack.Buffer[i] = UARTReceiveByte(SWS_UART, SWS_MAX_TIMEOUT);
	}
	
	// Отключаемся от СВС
	SWS_deinit();
		
	// Вычисляем контрольную сумму 
	crc = Calc_Crc32_Array(Actual_SWS_Pack.Buffer,56);
	
	// Сверяем контрольную сумму
	if(crc != Actual_SWS_Pack.Struct.CRC)
	{
		// Если не сошлись возвращаем ошибку
		return 1;
	}
	
	// Проверки сошлись можно вернуть полученные данные
	*SWS_Pack = Actual_SWS_Pack;
	
	// Иначе возвращаем успех
	return 0;
}



/**************************************************************************************************************
    UARTReceiveByte - Функция приёма байта данных по UART с контролем таймаутом
***************************************************************************************************************/
static int16_t UARTReceiveByte (MDR_UART_TypeDef* UARTx, uint32_t TimeoutRange)
{
	uint16_t Byte = 0;
	uint32_t timeout = 0;
	
	// Ждем прихода информации
	while ((UART_GetFlagStatus (UARTx, UART_FLAG_RXFE) == SET) && (timeout != TimeoutRange)) timeout++;
	// Если выход из ожидания по таймауту - возвращаем ошибку и выходим
	if(timeout == TimeoutRange) return 0xFF;
	// Иначе принимаем байт
	Byte = UART_ReceiveData(UARTx);
	return Byte;
}

/**************************************************************************************************************
    UARTSendByte - Функция отправки байта данных по UART с контролем таймаутом
***************************************************************************************************************/
static uint8_t UARTSendByte (MDR_UART_TypeDef* UARTx, uint32_t TimeoutRange, uint16_t Byte)
{
	uint32_t timeout = 0;
	
	// Если FIFO передатчика заполнен, то ждем пока освободится
	while ((UART_GetFlagStatus (UARTx, UART_FLAG_TXFF) == SET) && (timeout != TimeoutRange)) timeout++;
	// Если выход из ожидания по таймауту - возвращаем ошибку и выходим
	if(timeout == TimeoutRange) return 0xFF;
	// Иначе отправляем исходный символ
	UART_SendData(UARTx, Byte);
	
	return 0;
}
