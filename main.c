#include "../config.h"                           // Файл конфигурации проекта
#include "SWS.h"                                 // Драйвер работы с СВС
#include "SNS.h"                                 // Драйвер работы с СНС
#include "BIM.h"                                 // Драйвер работы с БИМ
#include "1636PP52Y.h"                           // Драйвер работы с внешней SPI flash 1636рр52у от Миландр
#include "25Q64FV.h"                             // Драйвер работы с внешней SPI flash 25Q64FV от Winbond
#include "discrete_io.h"                         // Драйвер работы дискретами (реле, LED..)
#include "ZPZ.h"                                 // Драйвер работы c загрузчиком полетного задания
#include "otherlib.h"                            // Модуль аппаратнозависимых функций общего назначения
#include "irq.h"                                 // Обслуживание прерываний

                           
#include "Log_FS/Log_FS.h"                       // Файловая система для записи логов в "черный ящик"
#include "RetargetPrintf/RetargetPrintf.h"       // Переопределение функции printf для вывода в CAN и записи в черный ящик
#include "Math_model/M_Model.h"                  // Математическая модель системы управления полетом
#include "HeightMap/Heightmap_conf_and_pd.h"     // Аппаратнозависимый подмодуль карты высоты рельефа
#include "SelfTesting.h"                         // Модуль самодиагностики

#include "stdint.h"                              // Стандартные типы Keil

#include "debug.h"                               // Отладочный модуль трассировки внутрисистемных переменных в CAN1

	double temp = 0;
	int16_t temp2 = 0;
	uint8_t buff[200];
  long long cnt = 0;
	

	char CHM = 0;
	double iii = 0;
	short pp = 0;


int main(void)
{	 
	uint32_t i = 0;                                           // Счетчик циклов
	uint32_t timeout = 0;                                     // Таймаут-счетчик
	SNS_Orientation_Data_Response_Union  SNS_Orientation;     // Данные ориентации от СНС
	SNS_Position_Data_Response_Union     SNS_Position;        // Данные местоположения от СНС
	SWS_Packet_Type_Union                SWS_Data;            // Данные от СВС
	
	// Настраиваем тактовую частоту процессора	
	InitCLK();
	// Конфигурируем дискреты, при этом все реле гарантированно отключаются
	Discrete_RetargetPins();
	// Запускаем драйвер памяти для полетного задания
	SPI_1636PP52Y_RetargetPins();
	SPI_1636PP52Y_Init();	
	// Запускаем драйвер внешней памяти "Черного ящика"
	SPI_25Q64FV_RetargetPins();
  SPI_25Q64FV_Init();
	// Запускаем драйвер БИМов (который попутно инициализирует CAN1)
	BIM_RetargetPins();
	BIM_CAN_init ();
	// Запускаем файловую систему
	LogFs_Info();
	// Запускаем диагностику системы
	SelfTestingFull();

//	BIM_Supply_ON();
//	while(1)
//	{
//		BIM_SendRequest (RIGHT_BIM, BIM_CMD_ON, 50, 9, 255, 255);
//		BIM_ReceiveResponse (RIGHT_BIM);
//		BIM_SendRequest (LEFT_BIM, BIM_CMD_ON, 50, 7, 255, 255);
//		BIM_ReceiveResponse (LEFT_BIM);
//		
//		BIM_SendRequest (RIGHT_BIM, BIM_CMD_REQ, 50, 9, 255, 255);
//		BIM_ReceiveResponse (RIGHT_BIM);
//		BIM_SendRequest (RIGHT_BIM, BIM_CMD_REQ, 50, 9, 255, 255);
//		BIM_ReceiveResponse (RIGHT_BIM);
//		BIM_SendRequest (RIGHT_BIM, BIM_CMD_REQ, 50, 9, 255, 255);
//		BIM_ReceiveResponse (RIGHT_BIM);
//		
//		BIM_SendRequest (LEFT_BIM, BIM_CMD_REQ, 50, 7, 255, 255);
//		BIM_ReceiveResponse (LEFT_BIM);
//		BIM_SendRequest (LEFT_BIM, BIM_CMD_REQ, 50, 7, 255, 255);
//		BIM_ReceiveResponse (LEFT_BIM);
//		BIM_SendRequest (LEFT_BIM, BIM_CMD_REQ, 50, 7, 255, 255);
//		BIM_ReceiveResponse (LEFT_BIM);
//		
//		
//		BIM_SendRequest (RIGHT_BIM, BIM_CMD_ON, 0, 9, 255, 255);
//		BIM_ReceiveResponse (RIGHT_BIM);
//		BIM_SendRequest (LEFT_BIM, BIM_CMD_ON, 0, 7, 255, 255);
//		BIM_ReceiveResponse (LEFT_BIM);
//		
//		BIM_SendRequest (RIGHT_BIM, BIM_CMD_REQ, 50, 9, 255, 255);
//		BIM_ReceiveResponse (RIGHT_BIM);
//		BIM_SendRequest (RIGHT_BIM, BIM_CMD_REQ, 50, 9, 255, 255);
//		BIM_ReceiveResponse (RIGHT_BIM);
//		BIM_SendRequest (RIGHT_BIM, BIM_CMD_REQ, 50, 9, 255, 255);
//		BIM_ReceiveResponse (RIGHT_BIM);
//		
//		BIM_SendRequest (LEFT_BIM, BIM_CMD_REQ, 50, 7, 255, 255);
//		BIM_ReceiveResponse (LEFT_BIM);
//		BIM_SendRequest (LEFT_BIM, BIM_CMD_REQ, 50, 7, 255, 255);
//		BIM_ReceiveResponse (LEFT_BIM);
//		BIM_SendRequest (LEFT_BIM, BIM_CMD_REQ, 50, 7, 255, 255);
//		BIM_ReceiveResponse (LEFT_BIM);
//	}	
//			SNS_Available_Data_Response_Union  SNS_DataState;
//			SNS_Device_Information_Response_Union  SNS_DeviceInformation;
//			
//while(1){
//     	while(SNS_GetPositionData(&SNS_Position) != SNS_OK && (timeout != 20)) timeout ++;
//			timeout = 0;
//      while(SNS_GetOrientationData(&SNS_Orientation) != SNS_OK && (timeout != 20)) timeout ++;
//			timeout = 0;
//			while(SNS_GetDataState(&SNS_DataState) != SNS_OK && (timeout != 20)) timeout ++;
//			timeout = 0;
//			while(SNS_GetDeviceInformation(&SNS_DeviceInformation) != SNS_OK && (timeout != 20)) timeout ++;
//			timeout = 0;
//			while(SWS_GetPacket (&SWS_Data) && (timeout != 10)) timeout ++;
//			timeout = 0;
//}
	
//// Тестирование файловой системы с логов
//	for(i=0; i < 200; i++)
//	{
//		buff[i] = i;
//	}
//	LogFs_Formatting();
//	
//	LogFs_Info();
//	LogFs_CreateFile();
//	LogFs_WriteToCurrentFile(buff, 8);
//	
//	printf_switcher(TO_LOG,0);
//  printf("Hello world!\n");
//	
//	LogFs_Info();
//	LogFs_CreateFile();
//	LogFs_WriteToCurrentFile(buff, 50);
//	
//	LogFs_Info();
//	LogFs_CreateFile();
//	LogFs_WriteToCurrentFile(buff, 200);
//	
//	LogFs_Info();
//	LogFs_CreateFile();
//	LogFs_WriteToCurrentFile(buff, 2);
//		
//	for(i=0; i < 200; i++)
//	{
//		buff[i] = 0;
//	}
//	
//	LogFs_Info();
//	Log_Fs_FindFIle(FIRST_FILE);
//	temp = Log_Fs_GetFileProperties(FILE_SIZE);
//	LogFs_ReadFile(buff, temp);
//	
//	for(i=0; i < 200; i++)
//	{
//		buff[i] = 0;
//	}
//	
//	Log_Fs_FindFIle(NEXT_FILE);
//	temp = Log_Fs_GetFileProperties(FILE_SIZE);
//	LogFs_ReadFile(buff, temp);
//	
//		
//	for(i=0; i < 200; i++)
//	{
//		buff[i] = 0;
//	}
//	
//	Log_Fs_FindFIle(NEXT_FILE);
//	temp = Log_Fs_GetFileProperties(FILE_SIZE);
//	LogFs_ReadFile(buff, temp);
//			
//	for(i=0; i < 200; i++)
//	{
//		buff[i] = 0;
//	}
//	
//	Log_Fs_FindFIle(NEXT_FILE);
//	temp = Log_Fs_GetFileProperties(FILE_SIZE);
//	temp = LogFs_ReadFile(buff, temp);
//	temp = LogFs_ReadFile(buff, temp);

//	
//	//создаем огромный файл
//	LogFs_Info();
//	LogFs_CreateFile();
//	LogFs_WriteToCurrentFile(buff, 2);
//		
//	for(i=0; i < 200; i++)
//	{ 
//		buff[i] = 0;
//	}


	// Проверяем подключение разъема ЗПЗ
	if(!CONNECT_ZPZ_CHECK)
	{
		// Разъем подключен
		// Запускаем драйвер ЗПЗ
		ZPZ_init();
		ZPZ_RetargetPins ();
		// Запускаем световую индикацию режима
		Timer_SetInterruptPeriod (MDR_TIMER2, 0.5*SECOND_TICKS);  // Сделать макросом?
		// Для тестирования БИМов подадим на них питание
		BIM_Supply_ON();
		// И уходим на обслуживание режима ЗПЗ
		while(!CONNECT_ZPZ_CHECK)
		{
			ZPZ_Service();
		}
	}
	
	// Проверим состояние системы:
	// 1. Обе шпильки должны быть вставлены
	// 2. Работу внешних SPI-flash
	// 3. Загружена ли карта рельефа и полетное задание
	// 4. Работает ли файловая система
	if(SelfTesting_STATUS(ST_pin1)!=ST_OK      || 
		 SelfTesting_STATUS(ST_pin2)!=ST_OK      || 
	   SelfTesting_STATUS(ST_MAP)!=ST_OK       || 
	   SelfTesting_STATUS(ST_1636PP52Y)!=ST_OK ||
	   SelfTesting_STATUS(ST_25Q64FV)!=ST_OK   ||
	   SelfTesting_STATUS(ST_LogFS)!=ST_OK)
	{		
		// Если не вставлены, гасим "Готовность", "Неисправность" зажигаем
		// Гасим "Готовность"
		LED_READY_OFF();
		// Зажигаем "Неисправность"
		LED_FAULT_ON();
		// Зависаем в ожидании перезапуска
		while(1)
		{
			// Включим CAN
			BIM_Supply_ON();
			// Будем выводить состояние системы в CAN, чтобы установить причину неисправности
			debug_vars.SysState = SystemState;
			debug_can(0x527, &debug_vars.SysState, 2);
			delay_us(1000000);
		}
	}
	#ifdef LOGS_ENABLE	//******************************************************* Если включено логирование в черный ящик
		// Создаём файл лога
		LogFs_CreateFile();
		// Переключаем вывод в ЛОГ
		printf_switcher(TO_LOG, 0);
		printf("BUP_init..\n");
		// Пишем загруженую точку посадки
		printf("TD_Lat: %f;\n", GetTouchDownPointLat());
		printf("TD_Lon: %f;\n", GetTouchDownPointLon());
		printf("BUP is ready!\n");
	#endif //******************************************************************** !LOGS_ENABLE	

	// Зажигаю готовность
	LED_READY_ON();
	// Ждем пока стабилизирующий парашют выбросится и извлечет шпильку 1
	while(SelfTesting_PIN1());
	// Выжидаем 10 секунд
	delay_us(10000000);
	// Поджигаем пиропатрон
	PYRO_ON();
	// Пиропатрон включаем только на 3 сек - но тупо ждать нельзя ------- переделать! ***************************  FIX_ME!!!!!!!!
	delay_us(3000000);
	
	// Ждем выброс основного парашюта - извлечения Шпильки 2
	while(SelfTesting_PIN2());
	// Ждем 5 секунд 
	delay_us(5000000);
	// Включаем питание БИМов
	BIM_Supply_ON();
	// Ждем 1 секунду, чтобы питание БИМов стабилировалось
	delay_us(1000000);
	// Нужно проверить исправность БИМов
	while (SelfTesting_RIGHT_BIM()!= ST_OK || SelfTesting_LEFT_BIM()!= ST_OK)
	{
		// Если хотя бы один из БИМов не отвечает, передергиваем питание и опять проверяем
		// Все 3 раза попытаемся перезапустистить БИМы
		BIM_Supply_OFF();
		delay_us(1000000);
		BIM_Supply_ON();
		delay_us(1000000);
		i++;
		if(i > 3)
		{
			LED_READY_OFF();
			LED_FAULT_ON();
			while(1);
		}
	}
	// Запустим быструю диагностику системы перед запуском управления
	SelfTestingOnline();
	// Инициализируем мат. модель полета
	M_Model_Init();
	// Сообщаем мат.модели о необходимости обновить данные
	M_Model_Need2UpdateSet();
	// К полету готовы, запускаем таймер обслуживания мат модели
	Timer_SetInterruptPeriod (MDR_TIMER1, SECOND_TICKS);  // Сделать макросом?
	
	// Главный рабочий цикл
	while(1)
	{
		// Просит ли матмодель обновить данные?
		if(M_Model_Need2UpdateCheck() == 1)
		{
			// Да, просит. Начинаем готовить данные для матмодели		
			
			#ifdef LOGS_ENABLE  //******************************************************* Если включено логирование в черный ящик
				printf("\nTimestamp: %d\n", ControlSecond); // Метку времени в ЛОГ
			#endif //******************************************************************** !LOGS_ENABLE
			
			// Сбросим таймаут
			timeout = 0;
			// Запрашиваем данные местоположения с контролем таймаута (спрашиваем не более 20 раз подряд)
			while(SNS_GetPositionData(&SNS_Position) != SNS_OK && (timeout != 20)) timeout ++;	
			// Сбросим таймаут
			timeout = 0;
			// Запрашиваем данные ориентации с контролем таймаута (спрашиваем не более 20 раз подряд)
			while(SNS_GetOrientationData(&SNS_Orientation) != SNS_OK && (timeout != 20)) timeout ++;

			// Сбросим таймаут
			timeout = 0;
			// Пытаемся получить данные от СВС, не более 20 попыток
			while(SWS_GetPacket (&SWS_Data) && (timeout != 10)) timeout ++;
			
			#ifdef LOGS_ENABLE  //******************************************************* Если включено логирование в черный ящик
				printf("SNS_Lat: %llu\n", SNS_Position.Struct.Pos_lat);
				printf("SNS_Lon: %llu\n", SNS_Position.Struct.Pos_lon);
				printf("SNS_Alt: %llu\n", SNS_Position.Struct.Pos_alt);
				printf("SNS_Heading_true: %d\n", SNS_Orientation.Struct.Heading_true);
				printf("SNS_Heading_mgn: %d\n", SNS_Orientation.Struct.Heading_mgn);
				printf("SNS_Vel_lat: %d\n", SNS_Position.Struct.Vel_lat);
				printf("SNS_Vel_lon: %d\n", SNS_Position.Struct.Vel_lon);
				printf("SNS_Vel_alt: %d\n", SNS_Position.Struct.Vel_alt);
				printf("SWS_TrueSpeed: %f\n", SWS_Data.Struct.TrueSpeed);
				printf("SWS_InstrumentSpeed: %f\n", SWS_Data.Struct.InstrumentSpeed);
				printf("BIML_Pos: %d\n",(uint8_t)(0.3922*BIM_GetStrapPosition(LEFT_BIM)));
				printf("BIMR_Pos: %d\n",(uint8_t)(0.3922*BIM_GetStrapPosition(RIGHT_BIM)));
				printf("SystemState: %x\n", SystemState);
			#endif //******************************************************************** !LOGS_ENABLE	

			// Отправляем данные математической модели
			M_Model_PrepareData (SNS_Position.Struct, SNS_Orientation.Struct, SWS_Data.Struct);
			
			// Запустим быструю диагностику системы
			SelfTestingOnline();
			
#ifdef TRACE_SYS_STATE	//******************************************************* Если включена трассировка состояния системы в CAN		
			// Трассировка состояния системы в CAN
			SelfTesting_SysState2CAN();	
#endif
		}		
	}
}
