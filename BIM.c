#include "BIM.h"
#include "otherlib.h"

#include "MDR32F9Qx_port.h"
#include "MDR32F9Qx_rst_clk.h"
#include "MDR32F9Qx_can.h"


/*******************************************************
        Актуальные данные о состоянии БИМОВ            *
*******************************************************/
BIM_Response_UnionType Right_BIM;
BIM_Response_UnionType Left_BIM;



/**************************************************************************************************************
				BIM_RetargetPins - Функция переопределения CAN на пины и порт, на на к которых сидят БИМЫ             *
**************************************************************************************************************/
void BIM_RetargetPins (void)
{
	// Разрешаем тактирование
	RST_CLK_PCLKcmd(RST_CLK_PCLK_BIM_PORT , ENABLE);
	// Переназчаем CAN, на котором сидят БИМы, на нужные ножки
	Pin_Init (BIM_CAN_PORT, BIM_CAN_RX, BIM_CAN_PORT_FUNC, PORT_OE_IN);
	Pin_Init (BIM_CAN_PORT, BIM_CAN_TX, BIM_CAN_PORT_FUNC, PORT_OE_OUT);
	// Пин активации приёмопередатчика
	Pin_Init (BIM_CAN_PORT, BIM_CAN_CS1, PORT_FUNC_PORT,PORT_OE_OUT);
	// Пин подачи питания на БИМы
	Pin_Init (BIM_SupplyPort, BIM_SupplyPin, PORT_FUNC_PORT,PORT_OE_OUT);
	// По-умолчанию питание отключим
	BIM_Supply_OFF(); 
}



/**************************************************************************************************************
				BIM_CAN_init - Инициализация драйвера для работы с БИМами по CAN протоколу     								        *
**************************************************************************************************************/
void BIM_CAN_init (void)
{
	double 	 PSEG, SEG1, SEG2, TQ, BRG; 
	double   systemCoreClockUser = 80000000;   // MHz
	double   BaudRate = BAUDRATE_BIM;				   // Бит/cек
	uint32_t HCLK_division = CAN_HCLKdiv1;
	uint8_t	 i = 0;	
	
	CAN_InitTypeDef        CAN_InitStruct;
	CAN_FilterInitTypeDef  CAN_FilterInitStruct;
	
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
	// Сейчас настроен фильтр на 200h адрес только на 0 буфере
	CAN_FilterInitStruct.Filter_ID = CAN_STDID_TO_EXTID(0x200);
	CAN_FilterInitStruct.Mask_ID = 0xFFFFFFFF;
	CAN_FilterInit (MDR_CAN1, 0, &CAN_FilterInitStruct);

	//Настройка фильтров
	// Сейчас настроен фильтр на 201h адрес только на 1 буфере
	CAN_FilterInitStruct.Filter_ID = CAN_STDID_TO_EXTID(0x201);
	CAN_FilterInitStruct.Mask_ID = 0xFFFFFFFF;
	CAN_FilterInit (MDR_CAN1, 1, &CAN_FilterInitStruct); 	
	
	// Активируем передатчик
  PORT_SetBits (BIM_CAN_PORT, BIM_CAN_CS1); 
}


/**************************************************************************************************************
				BIM_SendRequest - Отправка запроса к БИМу     					  																		        *
**************************************************************************************************************/
uint8_t BIM_SendRequest (uint16_t DeviceID, uint8_t CMD, uint8_t StrapPosition, uint8_t ReqCount, uint8_t SpeedLimit, uint8_t CurrentLimit)
{
	uint16_t timeout = 0, Buffer_number;
	CAN_TxMsgTypeDef BIM_Request; 
	
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
	while(((CAN_GetBufferStatus(BIM_CAN, Buffer_number) & CAN_STATUS_TX_REQ) != RESET) && (timeout != 0xFFF)) timeout++;
	
	// Внезависимости от того, удалось отправить или нет, освобождаем буфер
	CAN_BufferRelease (BIM_CAN, Buffer_number);
	
	// Проверяем был ли таймаут, и если да, то выдаём признак неудачи
	if(timeout == 0xFFF) 
		return 0;
	
	// Если попали сюда, значит передача удалась, вернём результат приёма ответа
	return BIM_ReceiveResponse (DeviceID);
}



/**************************************************************************************************************
				BIM_ReceiveResponse - Получение ответа от к БИМов 			  																		        *
**************************************************************************************************************/	
uint8_t BIM_ReceiveResponse (uint16_t DeviceID)
{
	uint16_t timeout = 0;
	CAN_RxMsgTypeDef RxMsg;
	if(DeviceID == DEVICE_100)		// Принять ответ от БИМа с адресом 100 (Левый)
	{
		while (!CAN_GetRxITStatus(BIM_CAN, 0)	 &&  (timeout !=0x3FFF)) {timeout ++;}
		CAN_GetRawReceivedData (BIM_CAN, 0 , &RxMsg);
		Left_BIM.Buffer[0] =  RxMsg.Data[0];
		Left_BIM.Buffer[1] =  RxMsg.Data[1];
		// Сбрасываем флаг того, что имеется необработаное сообщение
		CAN_ITClearRxTxPendingBit(BIM_CAN, 0, CAN_STATUS_RX_READY);
	}
	else if (DeviceID == DEVICE_101)	// Принять ответ от БИМа с адресом 101 (Правый)
	{
		while (!CAN_GetRxITStatus(BIM_CAN, 1)	 &&  (timeout !=0x3FFF)) {timeout ++;}
		CAN_GetRawReceivedData (BIM_CAN, 1 , &RxMsg);
		Right_BIM.Buffer[0] =  RxMsg.Data[0];
		Right_BIM.Buffer[1] =  RxMsg.Data[1];
	  // Сбрасываем флаг того, что имеется необработаное сообщение
		CAN_ITClearRxTxPendingBit(BIM_CAN, 1, CAN_STATUS_RX_READY);
	}
	// БИМов с другимми адресами нет, возвращаем признак ошибки
	else 
		return 0;  
  // Таймаут, возвращаем признак ошибки	
	if(timeout == 0x3FFF) 
		return 0;		
		
	return 1;
}



/**************************************************************************************************************
						BIM_CheckConnection - Проверка связи с БИМами      													                      *	
**************************************************************************************************************/
uint8_t BIM_CheckConnection (uint16_t DeviceID)
{
	uint8_t status = 0;         // Флаг результата проверки связи
	uint8_t i;                  // Счетчик количесвта опросов
	
	for(i = 0; i < 5; i++)
	{
		// Посылаем запрос на получение состояния БИМа
		status = BIM_SendRequest(DeviceID, BIM_CMD_REQ, 0xAA, 0, 0, 0);
		
		if(status)
			return status;
	}
	return status;
}



/**************************************************************************************************************
             BIM_GetStrapPosition - Получить текущее положение стропы     												            *	
**************************************************************************************************************/
uint8_t BIM_GetStrapPosition (uint16_t DeviceID)
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
             BIM_GetVoltage - Получить текущее значение напряжения     												               *	
**************************************************************************************************************/
uint8_t BIM_GetVoltage (uint16_t DeviceID)
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
              BIM_GetCurrent - Получить текущее значение тока     										   		                  *	
**************************************************************************************************************/
uint8_t BIM_GetCurrent (uint16_t DeviceID)
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
            BIM_GetSpeed - Получить текущее значение скорости              	   		                            *	
**************************************************************************************************************/
uint8_t BIM_GetSpeed (uint16_t DeviceID)
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
             BIM_GetStatusFlags - Получить актуальные флаги состояния устройства   			                      *	
**************************************************************************************************************/
uint8_t BIM_GetStatusFlags (uint16_t DeviceID)
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

