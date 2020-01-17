#include "SNS.h"
#include "crc16.h"
#include "otherlib.h"
#include "bupboard.h"
#include "MDR32F9Qx_uart.h"


/********************************  ПРИВАТНАЯ ЧАСТЬ МОДУЛЯ  ************************************************************************************/


/* Разделители фреймов по SLIP */
#define FEND			0xC0		/* Начало и конец кадра */
#define	TFEND			0xDC		/* Если FEND - не разделитель, то посылается как FESC TFEND */
#define FESC			0xDB		/* Если FEND - не разделитель, то посылается как FESC TFEND */
#define TFESC			0xDD		/* Если FESC - не разделитель, то посылается как FESC TFESC */

#define pi_const 3.1415927


/* Глобальный экземпляр даных от СНС */
SNS_Device_Information_Response_Union   SNS_deviceInformation;
SNS_Available_Data_Response_Union       SNS_availableData;
SNS_Position_Data_Response_Union        SNS_position;
SNS_Orientation_Data_Response_Union     SNS_orientation;


/* Приватные функции модуля */
static void       SNS_RetargetPins  (void);
static void       SNS_init          (void);
static void       SNS_deinit        (void);
static SNS_Status SNS_Request       (uint8_t Command);
static double     Rad_12_to_Deg     (int64_t rad);
static double     Rad_6_to_Deg      (int32_t rad);
static double     Rad_to_Deg        (double rad);
static double     Rad_6_to_Rad      (int32_t rad);
static double     Meter_12_to_Meter (int64_t meter);
static double     Ms_6_to_Ms        (int32_t Ms);



/**************************************************************************************************************
  SNS_RetargetPins - Функция переопределения UART1 на другие пины, для работы SNS
  Параметры:  NONE
***************************************************************************************************************/
static void SNS_RetargetPins (void)
{
  /* Переназчаем UART1 на порт B для работы SNS */
  Pin_init (SNS_RX);
  Pin_init (SNS_TX);
}

/**************************************************************************************************************
  SNS_init - Инициализация UART под SNS
  Параметры:  NONE
***************************************************************************************************************/
static void SNS_init (void)
{
  UART_InitTypeDef UART_InitStructure;
	
  UART_DeInit(SNS_UART);                     /* Сброс конфигуряции UART1 */
  UART_StructInit (&UART_InitStructure);     /* Заполнение структуры по-умолчанию */
  UART_BRGInit (SNS_UART,UART_HCLKdiv1);     /* Установка предделителя блока UART1_CLK = HCLK */
  /* Заполняем структуру инициализации */
  UART_InitStructure.UART_BaudRate                = BAUDRATE_SNS;
  UART_InitStructure.UART_WordLength              = UART_WordLength8b;
  UART_InitStructure.UART_StopBits                = UART_StopBits1;
  UART_InitStructure.UART_Parity                  = UART_Parity_No;
  UART_InitStructure.UART_FIFOMode                = UART_FIFO_ON;
  UART_InitStructure.UART_HardwareFlowControl     = UART_HardwareFlowControl_RXE | 
                                                    UART_HardwareFlowControl_TXE;
  UART_Init (SNS_UART,&UART_InitStructure);  /* Инициализация UART */
  UART_Cmd(SNS_UART,ENABLE);                 /* Включение UART1 - CНС */
}

/**************************************************************************************************************
  SNS_deinit - Деинициализация SNS и освобождение UART
  Параметры:  NONE
***************************************************************************************************************/
static void SNS_deinit (void)
{
  UART_DeInit(SNS_UART);      /* Сброс конфигуряции UART */
  UART_Cmd(SNS_UART,DISABLE); /* Выключение UART1 - CНС */
  Pin_default (SNS_RX);       /* Сбрасываем настройску пинов по-умолчанию  */
  Pin_default (SNS_TX);
}

/**************************************************************************************************************
  SNS_Request - SNS_Request - Запрос к СНС с командой
  Параметры:
            Command - Код команды, отправляемый СНС
            (СНС реагирует только на команды перечисленные а протоколе)
***************************************************************************************************************/
static SNS_Status SNS_Request(uint8_t Command)
{
  SNS_Request_Union      SNS_Req;
  uint8_t                i;
  TimeoutType            timeout;

  /* Готовим данные для отправки */
  SNS_Req.Struct.Command = Command;
  /* Вычисляем контрольную сумму пакета: */
  /* как в описани протокола обмена только для 1го байта - команды */
  SNS_Req.Struct.CRC = Crc16(&SNS_Req.Buffer[2], 1, CRC16_INITIAL_FFFF);

  /* Ждем окончания передачи */
  setTimeout (&timeout, SNS_CLR_TIMEOUT);
  while ((UART_GetFlagStatus (SNS_UART, UART_FLAG_TXFF) == SET) && (timeoutStatus(&timeout) != TIME_IS_UP));
	
  /* Проверяем будет ли это выход по таймауту */
  if(timeout.status == TIME_IS_UP)
    return SNS_TIMEOUT;
	
  /* Начинаем отправку запроса */
  /* Отправляем признак начала пакета */
  UART_SendData(SNS_UART, FEND);
	
  /* Ожидаем окончания отправки байта */
  setTimeout (&timeout, SNS_BYTE_TIMEOUT);
  while ((UART_GetFlagStatus (SNS_UART, UART_FLAG_TXFF) == SET) && (timeoutStatus(&timeout) != TIME_IS_UP));
	
  for(i = 0; i < 3; i++)
  {
    /* Если FEND не разделитель, а находится внутри пакета
    * Согласно протоколу обмена заменяем его на два символа */
    if(SNS_Req.Buffer[i] == FEND) 
    {
      UART_SendData(SNS_UART, FESC);
      /* Ждем окончания передачи байта */
      setTimeout (&timeout, SNS_BYTE_TIMEOUT);
      while ((UART_GetFlagStatus (SNS_UART, UART_FLAG_TXFF)== SET) && (timeoutStatus(&timeout) != TIME_IS_UP));
      UART_SendData(SNS_UART, TFEND);
    }
    /* Если внутри пакета содержится FESC,
    * Согласно протоколу обмена заменяем его на два символа */
    else if(SNS_Req.Buffer[i] == FESC) 
    {
      UART_SendData(SNS_UART, FESC);
      /* Ждем окончания передачи байта */
      setTimeout (&timeout, SNS_BYTE_TIMEOUT);
      while ((UART_GetFlagStatus (SNS_UART, UART_FLAG_TXFF)== SET) && (timeoutStatus(&timeout) != TIME_IS_UP));
      UART_SendData(SNS_UART, TFESC);
    }
    else
    {
      UART_SendData(SNS_UART, SNS_Req.Buffer[i]);
    }
    /* Ждем окончания передачи байта */
    setTimeout (&timeout, SNS_BYTE_TIMEOUT);
    while ((UART_GetFlagStatus (SNS_UART, UART_FLAG_TXFF) == SET) && (timeoutStatus(&timeout) != TIME_IS_UP));

    /* Проверяем будет ли это выход по таймауту */
    if(timeout.status == TIME_IS_UP) 
      return SNS_TIMEOUT;
  }
  /* Отправляем признак конца пакета */
  UART_SendData(SNS_UART, FEND);
  /* Ждем окончания передачи байта */
  setTimeout (&timeout, SNS_BYTE_TIMEOUT);
  while ((UART_GetFlagStatus (SNS_UART, UART_FLAG_TXFF)== SET) && (timeoutStatus(&timeout) != TIME_IS_UP));
	
  /* Проверяем будет ли это выход по таймауту */
  if(timeout.status == TIME_IS_UP) 
    return SNS_TIMEOUT;
	
  return SNS_OK;
}

/**************************************************************************************************************
  SNS_GetData_by_SLIP - Приём данных от СНС, распаковка кадров по SLIP
  Параметры:
              PacketSize - Размер принимаего пакета данных;
              Buffer     - Буфер, в который будет помещены принимаемые данные.
  Возвращает:
              0 - Если ошибка проверки кода ответа и контрольной суммы;
              1 - Если проверка верна.
***************************************************************************************************************/
static SNS_Status SNS_GetData_by_SLIP (uint8_t PacketSize, uint8_t* Buffer)
{
  TimeoutType timeout; 
  uint8_t     i;
  uint8_t     temp1, temp2;
	
  /* Ожидаем приход данных (пока буфер приёмника пуст) */
  setTimeout (&timeout, SWS_RESPONSE_TIMEOUT);
  while ((UART_GetFlagStatus (SNS_UART, UART_FLAG_RXFE) == SET) && (timeoutStatus(&timeout) != TIME_IS_UP));
  /* Проверяем будет ли это выход по таймауту */
  if(timeout.status == TIME_IS_UP) 
    return SNS_TIMEOUT;
	
  /* Делаем считывание символа - Должен быть разделитель FEND начала пакета */
  temp1 = UART_ReceiveData(SNS_UART);
	
  /* Переходим к приёму полезной части пакета */
  for(i = 0; i < PacketSize; i++)
  {	
    /* Ожидаем приход данных (пока буфер приёмника пуст) */
    setTimeout (&timeout, SNS_BYTE_TIMEOUT);
    while ((UART_GetFlagStatus (SNS_UART, UART_FLAG_RXFE) == SET) && (timeoutStatus(&timeout) != TIME_IS_UP));
    /* Проверяем будет ли это выход по таймауту */
    if(timeout.status == TIME_IS_UP) 
      return SNS_TIMEOUT;
		
    /* Принимаем символ */
    temp1 = UART_ReceiveData(SNS_UART);
		
    /* Проверяем является ли он FESC */
    if(temp1 == FESC)
    {
      /* Да, значит это закодированный FEND, значит следующий должен быть либо TFEND, либо TFESC */
      /* Подождем этот символ */
      setTimeout (&timeout, SNS_BYTE_TIMEOUT);
      while ((UART_GetFlagStatus (SNS_UART, UART_FLAG_RXFE) == SET) && (timeoutStatus(&timeout) != TIME_IS_UP));
      /* Проверяем будет ли это выход по таймауту */
      if(timeout.status == TIME_IS_UP) 
        return SNS_TIMEOUT;

      /* Принимаем следующий байт */
      temp2 = UART_ReceiveData(SNS_UART);
			
      /* Если этот принятый символ будет TFEND */
      if(temp2 == TFEND)
      {
        Buffer[i] = FEND; /* то вручную записываем в буфер символ FEND (вместо принятых двух) */
        temp2 = 0;        /* Сбросим временный байт */
        continue;         /* Сразу прыгаем на следующую итерацию цикла приёма */
      }
      /* Тогда видимо этот байт FESC */
      else if(temp2 == TFESC)
      {
        Buffer[i] = FESC; /* Да, вручную декодируем эти два байта в FESC */
        temp2 = 0;        /* Сбросим временный байт */
        continue;         /* Сразу прыгаем на следующую итерацию цикла приёма */
      }
      /* Если после FESC пришло и не TFEND, и не TFESC, то пакет испорчен */
      else
        return SNS_CORPACKET;

    }
    /* Если попали сюда, значит это обычный байт, его сразу пишем в буфер */
    Buffer[i] = temp1;
  }
  /* Ожидаем приход данных (пока буфер приёмника пуст) */
  setTimeout (&timeout, SNS_BYTE_TIMEOUT);
  while ((UART_GetFlagStatus (SNS_UART, UART_FLAG_RXFE) == SET) && (timeoutStatus(&timeout) != TIME_IS_UP));
  /* Проверяем будет ли это выход по таймауту */
  if(timeout.status == TIME_IS_UP)
    return SNS_TIMEOUT;
	
  /* Ждем развершения пакета символом FEND */
  temp1 = UART_ReceiveData(SNS_UART);
	
  return SNS_OK;
}


/********************************  ПУБЛИЧНАЯ ЧАСТЬ МОДУЛЯ  ************************************************************************************/

/**************************************************************************************************************
  SNS_updateDeviceInformation - Обновление информации о девайсе от СНС
***************************************************************************************************************/
SNS_Status SNS_updateDeviceInformation(void)
{
  SNS_Device_Information_Response_Union  Actual_SNS_DeviceInformation;   // Актуальный ответ от СНС
  TimeoutType  timeout;                                                  // Таймаут контроль
  uint16_t     crc;                                                      // Расчетная контрольная сумма
  uint8_t      requestCounter = 0;                                       // Счетчик отправленых запросов
		
  // Вычищаем FIFO от мусора перед запросом
  setTimeout (&timeout, SNS_CLR_TIMEOUT);
  while ((UART_GetFlagStatus (SNS_UART, UART_FLAG_RXFE) != SET)&& (timeoutStatus(&timeout) != TIME_IS_UP)) 
    UART_ReceiveData(SNS_UART);
	
  // Проверяем будет ли это выход по таймауту
  if(timeout.status == TIME_IS_UP)
    //Возвращаем признак неудачи
    return SNS_TIMEOUT;
	
  // Подключаемся к СНС
  SNS_RetargetPins();
  SNS_init();
  /* Так как СНС не спешит с ответом, необходимо спросить его несколько раз,
  и надеяться что хоть на какой-то из низ он ответит */
  for(requestCounter = 0; requestCounter < SNS_REQUESTS_CNT; requestCounter++)
  {
    /* Запрашиваем информацию у СНС */
    SNS_Request(DIR);
    /* Пытаемся получить в ответ локальную структуру Actual_SNS_DeviceInformation */
    if(SNS_GetData_by_SLIP(SizeAnsDIR, &Actual_SNS_DeviceInformation.Buffer[0]) == SNS_OK)
    {
      /* Ответ получили, сразу проверим валидность пакета */
      /* Вычисляем контрольную сумму пакета (без поля CRC) */
      crc = Crc16(&Actual_SNS_DeviceInformation.Buffer[2],SizeAnsDIR-2, CRC16_INITIAL_FFFF);
      /* Проверяем контрольную сумму и код ответа */
      if((Actual_SNS_DeviceInformation.Struct.Response == DIR)||(Actual_SNS_DeviceInformation.Struct.CRC == crc))
        /* Если пакет валидный - выходим, если нет - отправляемся на очередную попытку связаться */
        break;	
    }
  }
  /* Отключаемся от СНС */
  SNS_deinit();
	
  /* Проверим количество отправленных запросов, 
  если больше лимита значит связаться ответ так и не был получен */	
  if(requestCounter == SNS_REQUESTS_CNT)
  return SNS_TIMEOUT;
		
  /* Проверки верны, поэтому возвращаем полученные данные */
  SNS_deviceInformation = Actual_SNS_DeviceInformation;
	
  /* Возвращаем признак успеха */
  return  SNS_OK;
}



/**************************************************************************************************************
  SNS_updateDataState - Обновление информации о доступных данных от СНС
***************************************************************************************************************/
SNS_Status SNS_updateDataState(void)
{
  SNS_Available_Data_Response_Union  Actual_SNS_DataState;               // Актуальный ответ от СНС
  TimeoutType  timeout;                                                  // Таймаут контроль
  uint16_t     crc;                                                      // Расчетная контрольная сумма
  uint8_t      requestCounter = 0;                                       // Счетчик отправленых запросов
	
	
  // Вычищаем FIFO от мусора перед запросом
  setTimeout (&timeout, SNS_CLR_TIMEOUT);
  while ((UART_GetFlagStatus (SNS_UART, UART_FLAG_RXFE) != SET)&& (timeoutStatus(&timeout) != TIME_IS_UP)) 
    UART_ReceiveData(SNS_UART);
	
  // Проверяем будет ли это выход по таймауту
  if(timeout.status == TIME_IS_UP)
    //Возвращаем признак неудачи
    return SNS_TIMEOUT;

  // Подключаемся к СНС
  SNS_RetargetPins();
  SNS_init();	
  /* Так как СНС не спешит с ответом, необходимо спросить его несколько раз,
  и надеяться что хоть на какой-то из низ он ответит */
  for(requestCounter = 0; requestCounter < SNS_REQUESTS_CNT; requestCounter++)
  {
    /* Запрашиваем информацию у СНС */
    SNS_Request(DSR);
    /* Пытаемся получить в ответ локальную структуру Actual_SNS_DeviceInformation */
    if(SNS_GetData_by_SLIP(SizeAnsDSR, &Actual_SNS_DataState.Buffer[0]) == SNS_OK)
    {
      /* Ответ получили, сразу проверим валидность пакета */
      /* Вычисляем контрольную сумму пакета (без поля CRC) */
      crc = Crc16(&Actual_SNS_DataState.Buffer[2],SizeAnsDSR-2, CRC16_INITIAL_FFFF);
      /* Проверяем контрольную сумму и код ответа */
      if((Actual_SNS_DataState.Struct.Response == DSR)||(Actual_SNS_DataState.Struct.CRC == crc))
        /* Если пакет валидный - выходим, если нет - отправляемся на очередную попытку связаться */
        break;	
    }
  }
  /* Отключаемся от СНС */
  SNS_deinit();
	
  /* Проверим количество отправленных запросов, 
  если больше лимита значит связаться ответ так и не был получен */	
  if(requestCounter == SNS_REQUESTS_CNT)
    return SNS_TIMEOUT;
	
  /* Проверки верны, сохраняем информацию */
  SNS_availableData = Actual_SNS_DataState;
	
  /* Возвращаем признак успеха */
  return  SNS_OK;
}



/**************************************************************************************************************
  SNS_updatePositionData - Обновление информации о местоположении от СНС
***************************************************************************************************************/
SNS_Status SNS_updatePositionData(void)
{
  SNS_Position_Data_Response_Union  Actual_SNS_PositionData;             // Актуальный ответ от СНС
  TimeoutType  timeout;                                                  // Таймаут контроль
  uint16_t     crc;                                                      // Расчетная контрольная сумма
  uint8_t      requestCounter = 0;                                       // Счетчик отправленых запросов
	
  // Вычищаем FIFO от мусора перед запросом
  setTimeout (&timeout, SNS_CLR_TIMEOUT);
  while ((UART_GetFlagStatus (SNS_UART, UART_FLAG_RXFE) != SET)&& (timeoutStatus(&timeout) != TIME_IS_UP)) 
    UART_ReceiveData(SNS_UART);
	
  // Проверяем будет ли это выход по таймауту
  if(timeout.status == TIME_IS_UP)
    //Возвращаем признак неудачи
    return SNS_TIMEOUT;
	
  // Подключаемся к СНС
  SNS_RetargetPins();
  SNS_init();	
  /* Так как СНС не спешит с ответом, необходимо спросить его несколько раз,
  и надеяться что хоть на какой-то из низ он ответит */
  for(requestCounter = 0; requestCounter < SNS_REQUESTS_CNT; requestCounter++)
  {
    /* Запрашиваем информацию у СНС */
    SNS_Request(PDR);
    /* Пытаемся получить в ответ локальную структуру Actual_SNS_DeviceInformation */
    if(SNS_GetData_by_SLIP(SizeAnsPDR, &Actual_SNS_PositionData.Buffer[0]) == SNS_OK)
    {
      /* Ответ получили, сразу проверим валидность пакета */
      /* Вычисляем контрольную сумму пакета (без поля CRC) */
      crc = Crc16(&Actual_SNS_PositionData.Buffer[2],SizeAnsPDR-2, CRC16_INITIAL_FFFF);
      /* Проверяем контрольную сумму и код ответа */
      if((Actual_SNS_PositionData.Struct.Response == PDR)||(Actual_SNS_PositionData.Struct.CRC == crc))
        /* Если пакет валидный - выходим, если нет - отправляемся на очередную попытку связаться */
        break;				
    }
  }
  // Отключаемся от СНС
  SNS_deinit();
	
  /* Проверим количество отправленных запросов, 
  если больше лимита значит связаться ответ так и не был получен */	
  if(requestCounter == SNS_REQUESTS_CNT)
    return SNS_TIMEOUT;
	
  /* Проверки верны, сохраняем информацию */ 
  SNS_position = Actual_SNS_PositionData;
	
  //Возвращаем признак успеха
  return  SNS_OK;
}



/**************************************************************************************************************
  SNS_updateOrientationData - Обновление информации об ориентации от СНС
***************************************************************************************************************/
SNS_Status SNS_updateOrientationData(void)
{
  SNS_Orientation_Data_Response_Union  Actual_SNS_OrientationData;       // Актуальный ответ от СНС
  TimeoutType  timeout;                                                  // Таймаут контроль
  uint16_t  crc;                                                         // Расчетная контрольная сумма
  uint8_t   requestCounter = 0;                                          // Счетчик отправленых запросов
		
  // Вычищаем FIFO от мусора перед запросом
  setTimeout (&timeout, SNS_CLR_TIMEOUT);
  while ((UART_GetFlagStatus (SNS_UART, UART_FLAG_RXFE) != SET)&& (timeoutStatus(&timeout) != TIME_IS_UP)) 
    UART_ReceiveData(SNS_UART);
	
  // Проверяем будет ли это выход по таймауту
  if(timeout.status == TIME_IS_UP)
    //Возвращаем признак неудачи
    return SNS_TIMEOUT;
	
  // Подключаемся к СНС
  SNS_RetargetPins();
  SNS_init();
  /* Так как СНС не спешит с ответом, необходимо спросить его несколько раз,
  и надеяться что хоть на какой-то из низ он ответит */
  for(requestCounter = 0; requestCounter < SNS_REQUESTS_CNT; requestCounter++)
  {
    /* Запрашиваем информацию у СНС */
    SNS_Request(ODR);
    /* Пытаемся получить в ответ локальную структуру Actual_SNS_DeviceInformation */
    if(SNS_GetData_by_SLIP(SizeAnsODR, &Actual_SNS_OrientationData.Buffer[0]) == SNS_OK)
    {
      /* Ответ получили, сразу проверим валидность пакета */
      /* Вычисляем контрольную сумму пакета (без поля CRC) */
      crc = Crc16(&Actual_SNS_OrientationData.Buffer[2],SizeAnsODR-2, CRC16_INITIAL_FFFF);
      /* Проверяем контрольную сумму и код ответа */
      if((Actual_SNS_OrientationData.Struct.Response == ODR)||(Actual_SNS_OrientationData.Struct.CRC == crc))
      /* Если пакет валидный - выходим, если нет - отправляемся на очередную попытку связаться */
      break; 
    }
  }
  // Отключаемся от СНС
  SNS_deinit();
	
  /* Проверим количество отправленных запросов, 
  если больше лимита значит связаться ответ так и не был получен */	
  if(requestCounter == SNS_REQUESTS_CNT)
    return SNS_TIMEOUT;

  /* Проверки верны, поэтому возвращаем полученные данные */
  SNS_orientation = Actual_SNS_OrientationData;
	
  /* Возвращаем признак успеха */
  return  SNS_OK;
}



/**************************************************************************************************************
  SNS_startGyroCalibration - Команда начала калибровки гироскопа    
***************************************************************************************************************/
SNS_Status SNS_startGyroCalibration (void)
{
  SNS_Status result;
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
  SNS_resetGyroCalibration - Команда сброса калибровки гироскопа    
***************************************************************************************************************/
SNS_Status SNS_resetGyroCalibration (void)
{
  SNS_Status result;
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
  SNS_startMagnetometerCalibration - Команда начала калибровки магнитометра     
***************************************************************************************************************/
SNS_Status SNS_startMagnetometerCalibration (void)
{
  SNS_Status result;
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
  SNS_resetMagnetometerCalibration - Команда сброса калибровки магнитометра    
***************************************************************************************************************/
SNS_Status SNS_resetMagnetometerCalibration (void)
{
  SNS_Status result;
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
  SNS_enableHorizontalCorrection - Команда включения горизонтальной коррекции    
***************************************************************************************************************/
SNS_Status SNS_enableHorizontalCorrection (void)
{
  SNS_Status result;
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
  SNS_disableHorizontalCorrection - Команда отключения горизонтальной коррекции    
***************************************************************************************************************/
SNS_Status SNS_disableHorizontalCorrection (void)
{
  SNS_Status result;
  // Подключаемся к СНС
  SNS_RetargetPins();
  SNS_init();
  // Отправляем запрос
  result = SNS_Request(HCD);
  // Отключаемся от СНС
  SNS_deinit();
	
  return result;
}

/**************************************************************************************************************
  SNS_getLatitude - Получить широту в градусах    
***************************************************************************************************************/
double SNS_getLatitude(void)
{
  return Rad_12_to_Deg (SNS_position.Struct.Pos_lat);
}

/**************************************************************************************************************
  SNS_getLongitude - Получить долготу в градусах    
***************************************************************************************************************/
double SNS_getLongitude(void)
{
  return Rad_12_to_Deg (SNS_position.Struct.Pos_lon);
}

/**************************************************************************************************************
  SNS_getAltitude - Получить высоту в градусах, м   
***************************************************************************************************************/
double SNS_getAltitude(void)
{
  return Meter_12_to_Meter (SNS_position.Struct.Pos_alt);
}

/**************************************************************************************************************
  SNS_getGroundTrack - Получить путевой курс по сигналам, рад  
***************************************************************************************************************/
double SNS_getGroundTrack(void)
{
  return Rad_6_to_Rad (SNS_position.Struct.Course);
}

/**************************************************************************************************************
  SNS_getTrueCourse - Получить истиный курс, рад
***************************************************************************************************************/
double SNS_getTrueCourse(void)
{
  return Rad_6_to_Rad (SNS_orientation.Struct.Heading_true); 
}

/**************************************************************************************************************
  SNS_getMagnetCourse - Получить магнитный курс, рад
***************************************************************************************************************/
double SNS_getMagnetCourse(void)
{
  return Rad_6_to_Rad (SNS_orientation.Struct.Heading_mgn); 
}

/**************************************************************************************************************
  SNS_getLatitudeVelocity - Получить скорость по широте, м/с
***************************************************************************************************************/
double SNS_getLatitudeVelocity(void)
{
  return Ms_6_to_Ms(SNS_position.Struct.Vel_lat); 
}

/**************************************************************************************************************
  SNS_getLongitudeVelocity - Получить скорость по долготе, м/с
***************************************************************************************************************/
double SNS_getLongitudeVelocity(void)
{
  return Ms_6_to_Ms(SNS_position.Struct.Vel_lon); 
}

/**************************************************************************************************************
  SNS_getAltitudeVelocity - Получить скорость по высоте, м/с
***************************************************************************************************************/
double SNS_getAltitudeVelocity(void)
{
  return Ms_6_to_Ms(SNS_position.Struct.Vel_alt); 
}

/**************************************************************************************************************
  SNS_getPitch - Получить угол тангажа, рад
***************************************************************************************************************/
double SNS_getPitch(void)
{
  return Rad_6_to_Rad (SNS_orientation.Struct.Pitch);
}

/**************************************************************************************************************
  SNS_getPitch - Получить угол крена, рад
***************************************************************************************************************/
double SNS_getRoll(void)
{
  return Rad_6_to_Rad (SNS_orientation.Struct.Roll);
}
/**
*************************************************************************
  Rad_12_to_Deg - Преобразование 10E-12 радиан в градусы
***************************************************************************/
static double Rad_12_to_Deg(int64_t rad)
{
  double result = 0;
  result = ((rad/1e12)*180.0)/pi_const; 
  return result;
}

/***************************************************************************
  Rad_6_to_Deg - Преобразование 10E-6 радиан в градусы
***************************************************************************/
static double Rad_6_to_Deg(int32_t rad)
{
  double result = 0;
  result = ((rad/1e6)*180.0)/pi_const;
  return result;
}

/***************************************************************************
  Rad_to_Deg - Преобразование радиан в градусы
***************************************************************************/
static double Rad_to_Deg(double rad)
{
  double result = 0;
  result = (rad*180.0)/pi_const;
  return result;
}
/***************************************************************************
  Rad_6_to_Rad - Преобразование 10E-6 радиан в радианы
***************************************************************************/
static double Rad_6_to_Rad(int32_t rad)
{
  double result = 0;
  result = (rad/1e6);
  return result;
}

/***************************************************************************
  Meter_12_to_Meter - Преобразование 10E-12 метров в метры
***************************************************************************/
static double Meter_12_to_Meter (int64_t meter)
{
  double result = 0;
  result = ((double)(meter/1e12));
  return result;
}

/***************************************************************************
  Ms_6_to_Ms - Преобразование 10E-6 метров\c в метры\с
***************************************************************************/
static double Ms_6_to_Ms (int32_t Ms)
{
  double result = 0;
  result = ((double)(Ms/1e6));
  return result;
}
