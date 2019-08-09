#include "SNS.h"
#include "crc16.h"
#include "otherlib.h"

#include "MDR32F9Qx_port.h"
#include "MDR32F9Qx_rst_clk.h"
#include "MDR32F9Qx_uart.h"


/********************************  ПРИВАТНАЯ ЧАСТЬ МОДУЛЯ  ************************************************************************************/


// Разделители фреймов по SLIP
#define FEND			0xC0		// Начало и конец кадра
#define	TFEND			0xDC		// Если FEND - не разделитель, то посылается как FESC TFEND
#define FESC			0xDB		// Если FEND - не разделитель, то посылается как FESC TFEND
#define TFESC			0xDD		// Если FESC - не разделитель, то посылается как FESC TFESC


/**************************************************************************************************************
						SNS_RetargetPins - Функция переопределения UART1 на другие пины, для работы SNS                   *
						Параметры:  NONE                                                                                  *
***************************************************************************************************************/
void SNS_RetargetPins (void)
{
	RST_CLK_PCLKcmd(RST_CLK_PCLK_SNS_PORT , ENABLE);
	// Переназчаем UART1 на порт B для работы SNS
	Pin_Init (SNS_PORT, SNS_RX, PORT_FUNC_ALTER, PORT_OE_IN);
	Pin_Init (SNS_PORT, SNS_TX, PORT_FUNC_ALTER, PORT_OE_OUT);
}

/**************************************************************************************************************
						SNS_init - Инициализация UART под SNS                                                             *
						Параметры:  NONE                                                                                  *
***************************************************************************************************************/
void SNS_init (void)
{
	UART_InitTypeDef UART_InitStructure;
	
	// Сброс конфигуряции UART1
	UART_DeInit(SNS_UART);
	// Заполнение структуры по-умолчанию
	UART_StructInit (&UART_InitStructure);
	// Установка предделителя блока UART1_CLK = HCLK
	UART_BRGInit (SNS_UART,UART_HCLKdiv1); 
	// Заполняем структуру инициализации	
	UART_InitStructure.UART_BaudRate                = BAUDRATE_SNS;
  UART_InitStructure.UART_WordLength              = UART_WordLength8b;
  UART_InitStructure.UART_StopBits                = UART_StopBits1;
  UART_InitStructure.UART_Parity                  = UART_Parity_No;
  UART_InitStructure.UART_FIFOMode                = UART_FIFO_ON;
  UART_InitStructure.UART_HardwareFlowControl     = UART_HardwareFlowControl_RXE | UART_HardwareFlowControl_TXE;
  // Инициализация UART
  UART_Init (SNS_UART,&UART_InitStructure);
	// Включение UART1 - CНС
  UART_Cmd(SNS_UART,ENABLE);

}

/**************************************************************************************************************
						SNS_deinit - Деинициализация SNS и освобождение UART                                              *
						Параметры:  NONE                                                                                  *
***************************************************************************************************************/
void SNS_deinit (void)
{
	// Сброс конфигуряции UART
	UART_DeInit(SNS_UART);
	// Включение UART1 - CНС
  UART_Cmd(SNS_UART,DISABLE);
	Pin_Init (SNS_PORT, SNS_RX, PORT_FUNC_PORT, PORT_OE_OUT);
	Pin_Init (SNS_PORT, SNS_TX, PORT_FUNC_PORT, PORT_OE_OUT);
}

/**************************************************************************************************************
						SNS_Request - SNS_Request - Запрос к СНС с командой                                               *
						Параметры:  																																											*
											Command - Код команды, отправляемый СНС 																								*
										(СНС реагирует только на команды перечисленные а протоколе) 														*
***************************************************************************************************************/
uint8_t SNS_Request(uint8_t Command)
{
	SNS_Request_Union     SNS_Req;
	uint8_t               i;
	uint32_t 							timeout = 0;

	// Готовим данные для отправки
	SNS_Req.Struct.Command = Command;
	// Вычисляем контрольную сумму пакета:
	// как в описани протокола обмена только для 1го байта - команды
	SNS_Req.Struct.CRC = Crc16(&SNS_Req.Buffer[2], 1, CRC16_INITIAL_FFFF);
  // Начинаем отправку запроса
	UART_SendData(SNS_UART, FEND);	// Отправляем признак начала пакета
	// Ждем окончания передачи
	while ((UART_GetFlagStatus (SNS_UART, UART_FLAG_TXFF) == SET) && (timeout != SNS_MAX_TIMEOUT)) timeout++;
	
	// Проверяем будет ли это выход по таймауту
	if(timeout == SNS_MAX_TIMEOUT) 
		return 0;
	// Сбросим счетчик таймаута, и продолжаем
	timeout = 0;
	
	for(i = 0; i < 3; i++)
	{
		// Если FEND не разделитель, а находится внутри пакета
		// Согласно протоколу обмена заменяем его на два символа
		if(SNS_Req.Buffer[i] == FEND) 
		{
			UART_SendData(SNS_UART, FESC);
			UART_SendData(SNS_UART, TFEND);
		}
		// Если внутри пакета содержится FESC,
		// Согласно протоколу обмена заменяем его на два символа
		else if(SNS_Req.Buffer[i] == FESC) 
		{
			UART_SendData(SNS_UART, FESC);
			UART_SendData(SNS_UART, TFESC);
		}
		else UART_SendData(SNS_UART, SNS_Req.Buffer[i]);
		// Ждем окончания передачи
		while ((UART_GetFlagStatus (SNS_UART, UART_FLAG_TXFF) == SET) && (timeout != SNS_MAX_TIMEOUT)) timeout++;

		// Проверяем будет ли это выход по таймауту
		if(timeout == SNS_MAX_TIMEOUT) 
			return 0;
	  // Сбросим счетчик таймаут
	  timeout = 0;
		
	}
	UART_SendData(SNS_UART, FEND);	// Отправляем признак конца пакета
	// Ждем окончания передачи
	while ((UART_GetFlagStatus (SNS_UART, UART_FLAG_TXFF)== SET) && (timeout != SNS_MAX_TIMEOUT)) timeout++;
	
	// Проверяем будет ли это выход по таймауту
	if(timeout == SNS_MAX_TIMEOUT) 
		return 0;
	
	return 1;
}

/**************************************************************************************************************
						SNS_GetData_by_SLIP - Приём данных от СНС, распаковка кадров по SLIP                              *
						Параметры:                                                                                        *
											  PacketSize - Размер принимаего пакета данных                                          *
											  Buffer     - Буфер, в который будет помещены принимаемые данные 										  *
						Возвращает:                                                                                       *
                        0 - Если ошибка проверки кода ответа и контрольной суммы								  						*
												1 - Если проверка верна		 																														*
***************************************************************************************************************/
void SNS_GetData_by_SLIP (uint8_t PacketSize, uint8_t* Buffer)
{
	uint32_t  timeout = 0;
	uint8_t   i, temp;
	// Ожидаем приход данных (пока буфер приёмника пуст)
	while ((UART_GetFlagStatus (SNS_UART, UART_FLAG_RXFE) == SET) && (timeout != SNS_MAX_TIMEOUT)) timeout++;
	
	// Проверяем будет ли это выход по таймауту
	if(timeout == SNS_MAX_TIMEOUT) 
		return;
	// Сбросим счетчик таймаут
	timeout = 0;
	
	// Делаем считывание символа - Должен быть разделитель
	temp = UART_ReceiveData(SNS_UART);
	for(i = 0; i < PacketSize; i++)
	{	
		// Ожидаем приход данных (пока буфер приёмника пуст)
		while ((UART_GetFlagStatus (SNS_UART, UART_FLAG_RXFE) == SET) && (timeout != SNS_MAX_TIMEOUT)) timeout++;
		
		// Проверяем будет ли это выход по таймауту
	  if(timeout == SNS_MAX_TIMEOUT) 
			return;
	  // Сбросим счетчик таймаут
	  timeout = 0;
		
		// Принимаем символ
		temp = UART_ReceiveData(SNS_UART);
		// Проверяем является ли он FESC
		if(temp == FESC)
		{
			// Ожидаем приход данных (пока буфер приёмника пуст)
			while ((UART_GetFlagStatus (SNS_UART, UART_FLAG_RXFE) == SET) && (timeout != SNS_MAX_TIMEOUT)) timeout++;
			
			// Проверяем будет ли это выход по таймауту
	    if(timeout == SNS_MAX_TIMEOUT) 
				return;
	    // Сбросим счетчик таймаут
	    timeout = 0;
			
			// Если следующий символ будет TFEND, то это закодированный символ FEND
			if(UART_ReceiveData(SNS_UART) == TFEND)
			{
				Buffer[i] = FEND;
				continue;
			}
			// Ожидаем приход данных (пока буфер приёмника пуст)
			while ((UART_GetFlagStatus (SNS_UART, UART_FLAG_RXFE) == SET) && (timeout != SNS_MAX_TIMEOUT)) timeout++;
			
			// Проверяем будет ли это выход по таймауту
	    if(timeout == SNS_MAX_TIMEOUT) 
				return;
	    // Сбросим счетчик таймаут
	    timeout = 0;
			
			// Если следующий символ будет TFESC, то это закодированный символ FESC
			if(UART_ReceiveData(SNS_UART) == TFESC)
			{
				Buffer[i] = FESC;
				continue;
			}
		}
		// Если попали сюда, то все условия не выполнились и можем сразу записать символ в буфер
		Buffer[i] = temp;
	}
	// Ожидаем приход данных (пока буфер приёмника пуст)
	while ((UART_GetFlagStatus (SNS_UART, UART_FLAG_RXFE) == SET) && (timeout != SNS_MAX_TIMEOUT)) timeout++;
	
	// Проверяем будет ли это выход по таймауту
	if(timeout == SNS_MAX_TIMEOUT) 
		return;
	// Сбросим счетчик таймаут
	timeout = 0;
	
	// Ждем развершения пакета символом FEND
	temp = UART_ReceiveData(SNS_UART);
}


/********************************  ПУБЛИЧНАЯ ЧАСТЬ МОДУЛЯ  ************************************************************************************/

/**************************************************************************************************************
								 SNS_GetDeviceInformation - Запрос информации о девайсе от СНС                                *
**************************************************************************************************************/
uint8_t SNS_GetDeviceInformation(SNS_Device_Information_Response_Union*  SNS_DeviceInformation)
{
	SNS_Device_Information_Response_Union  Actual_SNS_DeviceInformation;   // Актуальный ответ от СНС
	uint32_t timeout = 0;                                                  // Таймаут счетчик
	uint16_t  crc;                                                         // Расчетная контрольная сумма
		
	//Вычищаем FIFO от мусора перед запросом
	while ((UART_GetFlagStatus (SNS_UART, UART_FLAG_RXFE) != SET)&& (timeout != SNS_MAX_TIMEOUT)) 
	{
		timeout++; 
	  UART_ReceiveData(SNS_UART);
	}
	
	// Проверяем будет ли это выход по таймауту
	if(timeout == SNS_MAX_TIMEOUT)
	{	
		//Возвращаем признак неудачи
		return 0;
	}
	
	// Сбросим счетчик таймаут
	timeout = 0;
	
	// Подключаемся к СНС
	SNS_RetargetPins();
	SNS_init();
	// Запрашиваем информацию у СНС
	SNS_Request(DIR);
	// Получаем ответ в локальную структуру Actual_SNS_DeviceInformation
	SNS_GetData_by_SLIP(SizeAnsDIR, &Actual_SNS_DeviceInformation.Buffer[0]);
	// Отключаемся от СНС
	SNS_deinit();
	
	// Вычисляем контрольную сумму пакета (без поля CRC)
	crc = Crc16(&Actual_SNS_DeviceInformation.Buffer[2],SizeAnsDIR-2, CRC16_INITIAL_FFFF);
	
	// Проверяем контрольную сумму и код ответа, если не совпадают, то возвращаем 0
	if((Actual_SNS_DeviceInformation.Struct.Response != DIR)||(Actual_SNS_DeviceInformation.Struct.CRC != crc))
	{
		//Возвращаем признак неудачи
		return 0; 
	}
	// Проверки верны, поэтому возвращаем полученные данные
	*SNS_DeviceInformation = Actual_SNS_DeviceInformation;
	
	//Возвращаем признак успеха
	return  1;
}



/**************************************************************************************************************
								 SNS_GetDataState - Запрос доступных данных от СНС                                            *
**************************************************************************************************************/
uint8_t SNS_GetDataState(SNS_Available_Data_Response_Union*  SNS_DataState)
{
	SNS_Available_Data_Response_Union  Actual_SNS_DataState;               // Актуальный ответ от СНС
	uint32_t timeout = 0;                                                  // Таймаут счетчик
	uint16_t  crc;                                                         // Расчетная контрольная сумма
		
	// Вычищаем FIFO от мусора перед запросом
	while ((UART_GetFlagStatus (SNS_UART, UART_FLAG_RXFE) != SET) && (timeout != SNS_MAX_TIMEOUT))
	{		
		timeout++; 
	  UART_ReceiveData(SNS_UART);
	}
	
	// Проверяем будет ли это выход по таймауту
	if(timeout == SNS_MAX_TIMEOUT)
	{		
		//Возвращаем признак неудачи
		return 0;
	}
	// Сбросим счетчик таймаута, и продолжаем
	timeout = 0;
	
	// Подключаемся к СНС
	SNS_RetargetPins();
	SNS_init();
	// Запрашиваем информацию у СНС
	SNS_Request(DSR);
	// Получаем ответ
	SNS_GetData_by_SLIP(SizeAnsDSR, &Actual_SNS_DataState.Buffer[0]);
	// Отключаемся от СНС
	SNS_deinit();
	
	// Вычисляем контрольную сумму пакета (без поля CRC)
	crc = Crc16(&Actual_SNS_DataState.Buffer[2],SizeAnsDSR-2, CRC16_INITIAL_FFFF);
	// Проверяем контрольную сумму и код ответа, если не совпадают, то возвращаем 0
	if((Actual_SNS_DataState.Struct.Response != DSR)||(Actual_SNS_DataState.Struct.CRC != crc))
	{		
		// Возвращаем признак неудачи
		return 0; 
	}
	
	// Проверки верны, поэтому возвращаем полученные данные
	*SNS_DataState = Actual_SNS_DataState;
	
	//Возвращаем признак успеха
	return  1;
}



/**************************************************************************************************************
								 SNS_GetPositionData - Запрос информации о местоположении от СНС                              *
**************************************************************************************************************/
uint8_t SNS_GetPositionData(SNS_Position_Data_Response_Union*  SNS_PositionData)
{
	SNS_Position_Data_Response_Union  Actual_SNS_PositionData;             // Актуальный ответ от СНС
	uint32_t timeout = 0;                                                  // Таймаут счетчик
	uint16_t  crc;                                                         // Расчетная контрольная сумма
	
	// Вычищаем FIFO от мусора перед запросом
	while ((UART_GetFlagStatus (SNS_UART, UART_FLAG_RXFE) != SET)&& (timeout != SNS_MAX_TIMEOUT))
	{ 
		timeout++; 
		UART_ReceiveData(SNS_UART);
	}
	
	// Проверяем будет ли это выход по таймауту
	if(timeout == SNS_MAX_TIMEOUT) 
	{
		// Возвращаем признак неудачи 
		return 0;
	}
	// Сбросим счетчик таймаута, и продолжаем
	timeout = 0;
	
	// Подключаемся к СНС
	SNS_RetargetPins();
	SNS_init();
	// Запрашиваем информацию у СНС
	SNS_Request(PDR);
	// Получаем ответ
	SNS_GetData_by_SLIP(SizeAnsPDR, &Actual_SNS_PositionData.Buffer[0]);
	// Отключаемся от СНС
	SNS_deinit();
	
	// Вычисляем контрольную сумму пакета (без поля CRC)
	crc = Crc16(&Actual_SNS_PositionData.Buffer[2],SizeAnsPDR-2, CRC16_INITIAL_FFFF);
	// Проверяем контрольную сумму и код ответа, если не совпадают, то возвращаем 0
	if((Actual_SNS_PositionData.Struct.Response != PDR)||(Actual_SNS_PositionData.Struct.CRC != crc))
	{
		// Возвращаем признак неудачи
		return 0;  
	}
	
	// Проверки верны, поэтому возвращаем полученные данные
	*SNS_PositionData = Actual_SNS_PositionData;
	
	//Возвращаем признак успеха
	return  1;
}



/**************************************************************************************************************
								 SNS_GetOrientationData - Запрос информации об ориентации от СНС                              *
**************************************************************************************************************/
uint8_t SNS_GetOrientationData(SNS_Orientation_Data_Response_Union*  SNS_OrientationData)
{
	SNS_Orientation_Data_Response_Union  Actual_SNS_OrientationData;          // Актуальный ответ от СНС
	uint32_t timeout = 0;                                                     // Таймаут счетчик
	uint16_t  crc;                                                            // Расчетная контрольная сумма
		
	//Вычищаем FIFO от мусора перед запросом
	while ((UART_GetFlagStatus (SNS_UART, UART_FLAG_RXFE) != SET) && (timeout != SNS_MAX_TIMEOUT))
	{
		timeout++;
		UART_ReceiveData(SNS_UART);
	}
	
	// Проверяем будет ли это выход по таймауту
	if(timeout == SNS_MAX_TIMEOUT) 
	{
		// Возвращаем признак неудачи 
		return 0;
	}
	
	// Сбросим счетчик таймаута, и продолжаем
	timeout = 0;
	
	// Подключаемся к СНС
	SNS_RetargetPins();
	SNS_init();
	// Запрашиваем информацию у СНС
	SNS_Request(ODR);
	// Получаем ответ
	SNS_GetData_by_SLIP(SizeAnsODR, &Actual_SNS_OrientationData.Buffer[0]);
	// Отключаемся от СНС
	SNS_deinit();
	
	// Вычисляем контрольную сумму пакета (без поля CRC)
	crc = Crc16(&Actual_SNS_OrientationData.Buffer[2],SizeAnsODR-2, CRC16_INITIAL_FFFF);
	// Проверяем контрольную сумму и код ответа, если не совпадают, то возвращаем 0
	if((Actual_SNS_OrientationData.Struct.Response != ODR)||(Actual_SNS_OrientationData.Struct.CRC != crc))
	{
		// Возвращаем признак неудачи 
		return 0; 
	}	
	
	// Проверки верны, поэтому возвращаем полученные данные
	*SNS_OrientationData = Actual_SNS_OrientationData;
	
	//Возвращаем признак успеха
	return  1;
}



/**************************************************************************************************************
						SNS_StartGyroCalibration - Команда начала калибровки гироскопа                                    *
***************************************************************************************************************/
uint8_t SNS_StartGyroCalibration (void)
{
	uint8_t result;
	// Подключаемся к СНС
	SNS_RetargetPins();
	SNS_init();
	// Отправляем запрос
	result = SNS_Request(GCE);
	// Отключаемся от СНС
	SNS_deinit();
	
	return result;
}



/**************************************************************************************************************
						SNS_ResetGyroCalibration - Команда сброса калибровки гироскопа                                    *
***************************************************************************************************************/
uint8_t SNS_ResetGyroCalibration (void)
{
	uint8_t result;
	// Подключаемся к СНС
	SNS_RetargetPins();
	SNS_init();
	// Отправляем запрос
	result = SNS_Request(GCR);
	// Отключаемся от СНС
	SNS_deinit();
	
	return result;
}



/**************************************************************************************************************
						SNS_StartMagnetometerCalibration - Команда начала калибровки магнитометра                         *
***************************************************************************************************************/
uint8_t SNS_StartMagnetometerCalibration (void)
{
	uint8_t result;
	// Подключаемся к СНС
	SNS_RetargetPins();
	SNS_init();
	// Отправляем запрос
	result = SNS_Request(MCE);
	// Отключаемся от СНС
	SNS_deinit();
	
	return result;
}



/**************************************************************************************************************
						SNS_ResetMagnetometerCalibration - Команда сброса калибровки магнитометра                         * 
***************************************************************************************************************/
uint8_t SNS_ResetMagnetometerCalibration (void)
{
	uint8_t result;
	// Подключаемся к СНС
	SNS_RetargetPins();
	SNS_init();
	// Отправляем запрос
	result = SNS_Request(MCR);
	// Отключаемся от СНС
	SNS_deinit();
	
	return result;
}



/**************************************************************************************************************
						SNS_EnableHorizontalCorrection - Команда включения горизонтальной коррекции                       *
***************************************************************************************************************/
uint8_t SNS_EnableHorizontalCorrection (void)
{
	uint8_t result;
	// Подключаемся к СНС
	SNS_RetargetPins();
	SNS_init();
	// Отправляем запрос
	result = SNS_Request(HCE);
	// Отключаемся от СНС
	SNS_deinit();
	
	return result;
}



/**************************************************************************************************************
						SNS_DisableHorizontalCorrection - Команда отключения горизонтальной коррекции                     *
***************************************************************************************************************/
uint8_t SNS_DisableHorizontalCorrection (void)
{
	uint8_t result;
	// Подключаемся к СНС
	SNS_RetargetPins();
	SNS_init();
	// Отправляем запрос
	result = SNS_Request(HCD);
	// Отключаемся от СНС
	SNS_deinit();
	
	return result;
}
