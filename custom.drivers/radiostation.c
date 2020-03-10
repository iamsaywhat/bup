#include "radiostation.h"

#include "bupboard.h"
#include "otherlib.h"
#include "crc16.h"
#include "MDR32F9Qx_uart.h"


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

/**************************************************************************************************************
  Radio_RetargetPins - Функция переопределения UART1 на другие пины, для работы радиостанции
  Параметры:  NONE
***************************************************************************************************************/
static void Radio_RetargetPins (void)
{
  /* Инициализируем пины UART под радиостанцию */
  Pin_initialize (RADIO_RX);
  Pin_initialize (RADIO_TX);
}

/**************************************************************************************************************
  Radio_initialize - Инициализация UART под радиостанцию
  Параметры:  NONE
***************************************************************************************************************/
static void Radio_initialize (void)
{
  UART_InitTypeDef UART_InitStructure;
	
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
  Radio_deinitialize - Деинициализация SNS и освобождение UART
  Параметры:  NONE
***************************************************************************************************************/
static void Radio_deinitialize (void)
{
  UART_DeInit(RADIO_UART);       /* Сброс конфигуряции UART */
  UART_Cmd(RADIO_UART, DISABLE); /* Выключение UART1 - RADIO */
  Pin_default (RADIO_RX);        /* Сбрасываем настройку пинов по-умолчанию  */
  Pin_default (RADIO_TX);
}

static uint16_t receiveByte(MDR_UART_TypeDef* UARTx);
static int16_t sendByte (MDR_UART_TypeDef* UARTx, uint16_t byte);
static uint16_t sendFend (MDR_UART_TypeDef* UARTx);


static void sendToRadio(uint8_t *data, uint8_t size)
{
	TimeoutType timeout; 
  RadioBaseFrameType radioData;
	uint16_t packetSize;
	
	if (size == 0)                     // Как минимум пакет будет состоять из 6 байт
	  packetSize = 6;                  
	else                               // Индекс, адрес, длина(4 байта) + тип данных, длина данных(4 байта) 
	  packetSize = 4 + 3 + size + 2;   // + данные + crc(2 байта)
		
	radioData.Struct.index      = 0;
	radioData.Struct.address    = 1;
	radioData.Struct.lenght     = packetSize-6;
	radioData.Struct.dataType   = HOST_DATA;
	radioData.Struct.dataLenght = size;
	radioData.Struct.crc        = 0xFFFF;
	
	// Подсчитываем контрольную сумму
	radioData.Struct.crc = Crc16(&radioData.Buffer[0], 4, radioData.Struct.crc);
  if(size != 0) {
    radioData.Struct.crc = Crc16(&radioData.Buffer[4], 3, radioData.Struct.crc);
		radioData.Struct.crc = Crc16(data, size, radioData.Struct.crc);
	}
	
	// Начинаем отправлять 
  sendFend (RADIO_UART);
	for(uint16_t i = 0; i < 4; i++)
		sendByte (RADIO_UART, radioData.Buffer[i]);
	if(size != 0) {
    for(uint16_t i = 4; i < 7; i++)
		  sendByte (RADIO_UART, radioData.Buffer[i]);
 	  for(uint16_t i = 0; i < size; i++)
		  sendByte (RADIO_UART, data[i]);
  }
  for(uint16_t i = 7; i < 9; i++)
    sendByte (RADIO_UART, radioData.Buffer[i]);
}

static void receiveFromeRadio(uint8_t* data)
{

}
	


void getDeviceName(void)
{
	Radio_RetargetPins();
	Radio_initialize();
}




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

// Отправка символа по SLIP - протоколу
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

// Приём и декодирование пакета по SLIP Протоколу 
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
