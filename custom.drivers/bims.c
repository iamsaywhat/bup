#include "bims.h"
#include "otherlib.h"
#include "MDR32F9Qx_can.h"


/*******************************************************
    Актуальные данные о состоянии БИМОВ 
*******************************************************/
BIM_Response_UnionType Right_BIM;
BIM_Response_UnionType Left_BIM;


/**************************************************************************************************************
    Объявления локальных функций модуля
***************************************************************************************************************/
static Bim_status BIM_ReceiveResponse (uint16_t DeviceID);


/**************************************************************************************************************
    BIM_RetargetPins - Функция переопределения CAN на пины и порт, на на к которых сидят БИМЫ
**************************************************************************************************************/
static void BIM_RetargetPins (void)
{
	// Переназчаем CAN, на котором сидят БИМы, на нужные ножки
	Pin_initialize (BIM_CAN_RX);
	Pin_initialize (BIM_CAN_TX);
	// Пин активации приёмопередатчика
	Pin_initialize (BIM_CAN_CS1);
	// Пин подачи питания на БИМы
	Pin_initialize (RELAY_BIM);
	// По-умолчанию питание отключим
	BIM_disable(); 
}


/**************************************************************************************************************
    BIM_CAN_initialize - Инициализация драйвера для работы с БИМами по CAN протоколу
**************************************************************************************************************/
void BIM_CAN_initialize (void)
{
	double 	 PSEG, SEG1, SEG2, TQ, BRG; 
	double   systemCoreClockUser = 80000000;   // MHz
	double   BaudRate = BAUDRATE_BIM;          // Бит/cек
	uint32_t HCLK_division = CAN_HCLKdiv1;
	uint8_t	 i = 0;	
	
	CAN_InitTypeDef        CAN_InitStruct;
	CAN_FilterInitTypeDef  CAN_FilterInitStruct;
	
	// Конфигурируем пины
	BIM_RetargetPins(); 
	// Сброс всех настроек
	CAN_DeInit(BIM_CAN);
	// Частота работы = HCLK/1
	CAN_BRGInit(BIM_CAN,HCLK_division);
	// Заполняем структуру настройками по-умолчанию
	CAN_StructInit(&CAN_InitStruct);
	// Настройка тактирования и таймингов	
	// Перечисляем конфигурацию
	CAN_InitStruct.CAN_ROP = DISABLE;
	CAN_InitStruct.CAN_SAP = DISABLE;
	CAN_InitStruct.CAN_STM = DISABLE;
	CAN_InitStruct.CAN_ROM = DISABLE;
	CAN_InitStruct.CAN_PSEG = CAN_PSEG_Mul_5TQ; 
	CAN_InitStruct.CAN_SEG1 = CAN_SEG1_Mul_8TQ;
	CAN_InitStruct.CAN_SEG2 = CAN_SEG2_Mul_2TQ;
	CAN_InitStruct.CAN_SJW = CAN_SJW_Mul_1TQ;
	CAN_InitStruct.CAN_SB = CAN_SB_1_SAMPLE;
	// Рассчитываем  CAN_BRP
	BRG = (1 << HCLK_division);
	PSEG = ((CAN_InitStruct.CAN_PSEG >> CAN_BITTMNG_PSEG_Pos)+1);
	SEG1 = ((CAN_InitStruct.CAN_SEG1 >> CAN_BITTMNG_SEG1_Pos)+1);
	SEG2 = ((CAN_InitStruct.CAN_SEG2 >> CAN_BITTMNG_SEG2_Pos)+1);
	// Определяем количество Time Quanta в битовом интервале 
	TQ = 1 + SEG1 + SEG2 + PSEG;
	CAN_InitStruct.CAN_BRP  = (uint8_t)(systemCoreClockUser/BRG/(TQ*BaudRate))-1;
	// Допустимое количество ошибок TEC или REC 
	CAN_InitStruct.CAN_OVER_ERROR_MAX = 255;
	// Иницилизируем структуру
	CAN_Init(BIM_CAN,&CAN_InitStruct);
	
	// Включаем CAN
	CAN_Cmd(BIM_CAN,ENABLE);
		
	// Запрещаем все прерывания от CAN1
	CAN_ITConfig(BIM_CAN, CAN_IT_GLBINTEN | CAN_IT_RXINTEN | CAN_IT_TXINTEN | CAN_IT_ERRINTEN | CAN_IT_ERROVERINTEN, DISABLE);
	
	
	// Разрешаем работу буферы 0-15 на прием 
	for(i = 0; i < 16; i++)
	{	
		// Работа i-го буфера на приём, перезапись разрешена
		CAN_Receive(BIM_CAN, i, ENABLE);
		// Разрешаем прерывание, изменение состояния буфера генерирует прерывание
		CAN_RxITConfig(BIM_CAN, (1<<i), ENABLE);
		// Включаем буфера на передачу
		BIM_CAN->BUF_CON[i+16] = CAN_BUF_CON_EN;
		// Разрешаем прерывания по передаче из буферов 16-31
		CAN_TxITConfig(BIM_CAN, 1<<(i+16), ENABLE);
		
		// Настроили буфер на передачу и отключаем (освобождаем его)
		CAN_BufferRelease (BIM_CAN, i+16);
		
		// Здесь присутсвует нюанс работы с SPL драйвером от Миландр:
		// Одни функции работы с CAN в качестве номера буфера принимают просто номера от 0 - 31
		// Другие предлагают либо использование макросов типа CAN_BUFFER_x = 1 << x
	}
	
	//Настройка фильтров
  // Резервируем один из буферов (RIGHT_BIM_BUFFER) под данные с адресом ответа правого бима (FROM_RIGHT_BIM)
	CAN_FilterInitStruct.Filter_ID = CAN_STDID_TO_EXTID(FROM_RIGHT_BIM);
	CAN_FilterInitStruct.Mask_ID = 0xFFFFFFFF;
	CAN_FilterInit (MDR_CAN1, RIGHT_BIM_BUFFER, &CAN_FilterInitStruct);

	//Настройка фильтров
  // Резервируем один из буферов (LEFT_BIM_BUFFER) под данные с адресом ответа правого бима (FROM_LEFT_BIM)
	CAN_FilterInitStruct.Filter_ID = CAN_STDID_TO_EXTID(FROM_LEFT_BIM);
	CAN_FilterInitStruct.Mask_ID = 0xFFFFFFFF;
	CAN_FilterInit (MDR_CAN1, LEFT_BIM_BUFFER, &CAN_FilterInitStruct); 	
	
	// Активируем передатчик
	Pin_set (BIM_CAN_CS1); 
}


/**************************************************************************************************************
    BIM_sendRequest - Отправка запроса к БИМу
**************************************************************************************************************/
Bim_status BIM_sendRequest (Bim_devices DeviceID, uint8_t CMD, uint8_t StrapPosition, uint8_t ReqCount, uint8_t SpeedLimit, uint8_t CurrentLimit)
{
	uint16_t         Buffer_number;
	CAN_TxMsgTypeDef BIM_Request; 
	TimeoutType      timeout; 
	
	BIM_Request.ID = CAN_STDID_TO_EXTID(DeviceID);  // Адрес БИМ
	BIM_Request.PRIOR_0 = DISABLE;                  // Без приоритета
	BIM_Request.IDE = CAN_ID_STD;                   // Тип - адреса стандартный
	BIM_Request.DLC = 5;                            // Количество байт данных в запросе	
	// Заполняем данные	
	BIM_Request.Data[0] = CMD;                      // Команда
	BIM_Request.Data[0]|= StrapPosition << 8;       // Позиция стропы
	BIM_Request.Data[0]|= ReqCount << 16;           // Счетчик опроса
	BIM_Request.Data[0]|= SpeedLimit << 24;         // Ограничение скорости
	BIM_Request.Data[1] = CurrentLimit;             // Ограничение тока
	
	// Спросим какой из буферов свободен для использования
	Buffer_number = CAN_GetDisabledBuffer (BIM_CAN);
	// Кладём сообщение в нужный буфер и ждем отправки
	CAN_Transmit(BIM_CAN, Buffer_number, &BIM_Request);
	// Ожидаем конца передачи, либо превышения времени ожидания
	setTimeout (&timeout, BIM_SEND_TIMEOUT);
	while(((CAN_GetBufferStatus(BIM_CAN, Buffer_number) & CAN_STATUS_TX_REQ) != RESET) && (timeoutStatus(&timeout) != TIME_IS_UP));
	
	// Внезависимости от того, удалось отправить или нет, освобождаем буфер
	CAN_BufferRelease (BIM_CAN, Buffer_number);
	
	// Проверяем был ли таймаут, и если да, то выдаём признак неудачи
	if(timeout.status == TIME_IS_UP)
		return BIM_ERROR;
	
	// Если попали сюда, значит передача удалась, вернём результат приёма ответа
	return BIM_ReceiveResponse (DeviceID);
}

/**************************************************************************************************************
    BIM_controlCommand - Команда управления положением БИМа
**************************************************************************************************************/
Bim_status BIM_controlCommand (Bim_devices DeviceID, uint8_t StrapPosition)
{
  BIM_sendRequest (DeviceID, BIM_CMD_REQ, 0, 77, 255, 255);  // Спросим состояние БИМ
  
  if (BIM_getStrapPosition(DeviceID) == StrapPosition)
    return BIM_ALREADY_ON_POSITION;
  return BIM_sendRequest (DeviceID, BIM_CMD_ON, StrapPosition, 66, 255, 255);
}

/**************************************************************************************************************
    BIM_stopCommand - Команда на остановку БИМА
**************************************************************************************************************/
Bim_status BIM_stopCommand (Bim_devices DeviceID)
{
  return BIM_sendRequest (DeviceID, BIM_CMD_OFF, 0, 11, 255, 255);
}

/**************************************************************************************************************
    BIM_updateCommand - Обновить данные о состоянии БИМа
**************************************************************************************************************/
Bim_status BIM_updateCommand (Bim_devices DeviceID)
{
  return BIM_sendRequest (DeviceID, BIM_CMD_REQ, 0, 55, 255, 255);
}

/**************************************************************************************************************
    BIM_checkConnection - Проверка связи с БИМами
**************************************************************************************************************/
Bim_status BIM_checkConnection (Bim_devices DeviceID)
{
  Bim_status status = BIM_ERROR;  // Флаг результата проверки связи
  uint8_t i;                      // Счетчик количесвта опросов
	
  for(i = 0; i < 5; i++)
  {
    // Посылаем запрос на получение состояния БИМа
    status = BIM_sendRequest(DeviceID, BIM_CMD_REQ, 0xAA, 0, 0, 0);
  }
  return status;
}



/**************************************************************************************************************
    BIM_getStrapPosition - Получить текущее положение стропы	
**************************************************************************************************************/
uint8_t BIM_getStrapPosition (Bim_devices DeviceID)
{
	// Спрашивается положение стропы левого БИМа
	if(DeviceID == LEFT_BIM)
	{
		return Left_BIM.Struct.StrapPosition;
	}
	// Спрашивается положение стропы правого БИМа
	else if (DeviceID == RIGHT_BIM)
	{
		return Right_BIM.Struct.StrapPosition;
	}
	// ID устройства неопознан
	return 0;
}



/**************************************************************************************************************
    BIM_getVoltage - Получить текущее значение напряжения
**************************************************************************************************************/
uint8_t BIM_getVoltage (Bim_devices DeviceID)
{
	// Спрашивается положение стропы левого БИМа
	if(DeviceID == LEFT_BIM)
	{
		return Left_BIM.Struct.Voltage;
	}
	// Спрашивается положение стропы правого БИМа
	else if (DeviceID == RIGHT_BIM)
	{
		return Right_BIM.Struct.Voltage;
	}
	// ID устройства неопознан
	return 0;
}



/**************************************************************************************************************
    BIM_getCurrent - Получить текущее значение тока
**************************************************************************************************************/
int8_t BIM_getCurrent (Bim_devices DeviceID)
{
	// Спрашивается положение стропы левого БИМа
	if(DeviceID == LEFT_BIM)
	{
		return Left_BIM.Struct.Current;
	}
	// Спрашивается положение стропы правого БИМа
	else if (DeviceID == RIGHT_BIM)
	{
		return Right_BIM.Struct.Current;
	}
	// ID устройства неопознан
	return 0;
}



/**************************************************************************************************************
    BIM_getSpeed - Получить текущее значение скорости
**************************************************************************************************************/
int8_t BIM_getSpeed (Bim_devices DeviceID)
{
	// Спрашивается положение стропы левого БИМа
	if(DeviceID == LEFT_BIM)
	{
		return Left_BIM.Struct.Speed;
	}
	// Спрашивается положение стропы правого БИМа
	else if (DeviceID == RIGHT_BIM)
	{
		return Right_BIM.Struct.Speed;
	}
	// ID устройства неопознан
	return 0;
}



/**************************************************************************************************************
    BIM_getStatusFlags - Получить актуальные флаги состояния устройства
**************************************************************************************************************/
uint16_t BIM_getStatusFlags (Bim_devices DeviceID)
{
	// Спрашивается положение стропы левого БИМа
	if(DeviceID == LEFT_BIM)
	{
		return Left_BIM.Struct.StatusFlags;
	}
	// Спрашивается положение стропы правого БИМа
	else if (DeviceID == RIGHT_BIM)
	{
		return Right_BIM.Struct.StatusFlags;
	}
	// ID устройства неопознан
	return 0;
}



/**************************************************************************************************************
    BIM_ReceiveResponse - Функция приёма ответа на запрос от БИМов 

    Параметры: 
                BIM_Response  - Структура ответа;
                DeviceID      - Адрес БИМа на CAN шине, от которого ждем ответ.												

    Возвращает: 0 - Если ошибка при приёме (таймаут или DeviceID не найден)
                1 - Если отправлено успешно 		
**************************************************************************************************************/
static Bim_status BIM_ReceiveResponse (Bim_devices DeviceID)
{
	TimeoutType      timeout;
	CAN_RxMsgTypeDef RxMsg;
	
	setTimeout (&timeout, BIM_RECEIVE_TIMEOUT);
	if(DeviceID == RIGHT_BIM)		// Принять ответ от правого БИМа
	{
		while (!CAN_GetRxITStatus(BIM_CAN, RIGHT_BIM_BUFFER)	&& (timeoutStatus(&timeout) != TIME_IS_UP));
		if(CAN_GetRxITStatus(BIM_CAN, RIGHT_BIM_BUFFER))                   
		{
			CAN_GetRawReceivedData (BIM_CAN, RIGHT_BIM_BUFFER, &RxMsg);     // Забираем пакет данных, только если
			Right_BIM.Buffer[0] =  RxMsg.Data[0];                     // Выход из цикла ожидания выше был
			Right_BIM.Buffer[1] =  RxMsg.Data[1];                     // По приходу данных, по таймауту ничего не делаем
		}
		// Сбрасываем флаг того, что имеется необработаное сообщение
		CAN_ITClearRxTxPendingBit(BIM_CAN, RIGHT_BIM_BUFFER, CAN_STATUS_RX_READY);
	}
	else if (DeviceID == LEFT_BIM)	// Принять ответ от левого БИМа
	{
		while (!CAN_GetRxITStatus(BIM_CAN, LEFT_BIM_BUFFER)	&& (timeoutStatus(&timeout) != TIME_IS_UP));
		if(CAN_GetRxITStatus(BIM_CAN, LEFT_BIM_BUFFER))
		{
			CAN_GetRawReceivedData (BIM_CAN, LEFT_BIM_BUFFER, &RxMsg);   // Забираем пакет данных, только если
			Left_BIM.Buffer[0] =  RxMsg.Data[0];                    // Выход из цикла ожидания выше был
			Left_BIM.Buffer[1] =  RxMsg.Data[1];                    // По приходу данных, по таймауту ничего не делаем
		}
		// Сбрасываем флаг того, что имеется необработаное сообщение
		CAN_ITClearRxTxPendingBit(BIM_CAN, LEFT_BIM_BUFFER, CAN_STATUS_RX_READY);
	}
	// БИМов с другимми адресами нет, возвращаем признак ошибки
	else 
		return BIM_ERROR;  
    // Таймаут, возвращаем признак ошибки	
	if(timeout.status == TIME_IS_UP)
		return BIM_ERROR;		
		
	return BIM_DONE;
}
