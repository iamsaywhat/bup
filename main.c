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
	char p1;
	char p2;


int main(void)
{	 
	uint32_t timeout = 0;                                     // Таймаут-счетчик
	SNS_Orientation_Data_Response_Union  SNS_Orientation;     // Данные ориентации от СНС
	SNS_Position_Data_Response_Union     SNS_Position;        // Данные местоположения от СНС
	SWS_Packet_Type_Union                SWS_Data;            // Данные от СВС
	
	// Настраиваем тактовую частоту процессора	
	InitCLK();
	// Конфигурируем дискреты, при этом все реле гарантированно переводятся в состояние по-умолчанию
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
	
	// Запускаем фул-тест системы
	SelfTestingFull();

//////////////
//	BIM_Supply_ON();
//	
//	CAN_BufferRelease (MDR_CAN1, 16);
//	CAN_BufferRelease (MDR_CAN1, 17);
//	CAN_BufferRelease (MDR_CAN1, 18);
//	
//	while(1)
//	{
//		//SelfTesting_RIGHT_BIM();
//		//SelfTesting_LEFT_BIM();
//		SelfTestingOnline();
//		delay_us(100000);
//		debug_can_full_struct();
//		delay_us(100000);
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
	



	// Проверяем подключение разъема ЗПЗ
	if(!CONNECT_ZPZ_CHECK)
	{
		// Разъем подключен
		// Запускаем драйвер ЗПЗ
		ZPZ_init();
		ZPZ_RetargetPins ();
		
		// Для тестирования БИМов подадим на них питание
		BIM_Supply_ON();
		
		// И уходим на обслуживание режима ЗПЗ
		while(!CONNECT_ZPZ_CHECK)
		{
			// Запускаем модуль обслуживания ЗПЗ
			ZPZ_Service();
			
			// Если ЗПЗ в настоящий момент не занят тяжелым процессом (режим РК)
			if(ZPZ_CheckHighPriorityTask() == ZPZ_SC_MODE)
			{
				// Будем в свободное время будем заниматься самодиагностикой
				SelfTestingBeatTest ();
			}				
		}
	}

  // Здесь начинается боевой режим работы
	
	// Проверим готова ли система к запуску управления (периферия в порядке и шпильки парашютов на месте)
	while(SelfTesting_PreflightDiagnostics() == ST_FAULT)
	{	
		// Что-то пошло не так
		// Запустим самодиагностику, и будем ожидать в недежде готовности системы к полёту
		SelfTestingOnline ();
	}
	
	// Если оказались здесь, значит БУП готов к работе
	
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

  #ifdef LOGS_ENABLE	//******************************************************* Если включено логирование в черный ящик
		// Создаём файл лога
		LogFs_CreateFile();
		// Переключаем вывод в ЛОГ
		printf_switcher(TO_LOG, 0);
	  // В начало файла кладём его порядковый номер
		printf("***File # %d***\n", LogFs_GetNumCurrentFile());
		printf("BUP_init..\n");
		// Выведем загруженное полетное задание
		printf("TD_Lat: %f;\n", GetTouchDownPointLat());
		printf("TD_Lon: %f;\n", GetTouchDownPointLon());
		printf("TD_Alt: %f;\n", GetTouchDownPointAlt());
		printf("BUP is ready!\n");
	#endif //******************************************************************** !LOGS_ENABLE	
	
	// Запустим быструю диагностику системы перед запуском управления
	SelfTestingOnline();
	// Инициализируем мат. модель полета
	M_Model_Init();
	// Сообщаем мат.модели о необходимости обновить данные
	M_Model_Need2UpdateSet();
	
	// Главный рабочий цикл
	while(1)
	{
		// Просит ли матмодель обновить данные?
		if(M_Model_Need2UpdateCheck() == 1)
		{
			// Да, просит. Начинаем готовить данные для матмодели		
			
			#ifdef LOGS_ENABLE  //******************************************************* Если включено логирование в черный ящик
				printf("\nTimestamp, sec: %d\n", ControlSecond); // Метку времени в ЛОГ
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
			// Пытаемся получить данные от СВС, не более 10 попыток
			while(SWS_GetPacket (&SWS_Data) && (timeout != 10)) timeout ++;
			
			#ifdef LOGS_ENABLE  //******************************************************* Если включено логирование в черный ящик
				printf("SNS_Lat: %llu\n",            SNS_Position.Struct.Pos_lat);
				printf("SNS_Lon: %llu\n",            SNS_Position.Struct.Pos_lon);
				printf("SNS_Alt: %llu\n",            SNS_Position.Struct.Pos_alt);
				printf("SNS_Vel_lat: %d\n",          SNS_Position.Struct.Vel_lat);
				printf("SNS_Vel_lon: %d\n",          SNS_Position.Struct.Vel_lon);
				printf("SNS_Vel_alt: %d\n",          SNS_Position.Struct.Vel_alt);
				printf("SNS_Course: %d\n",           SNS_Position.Struct.Course);
				printf("SNS_Heading_true: %d\n",     SNS_Orientation.Struct.Heading_true);
				printf("SNS_Heading_mgn: %d\n",      SNS_Orientation.Struct.Heading_mgn);
				printf("SNS_Pitch: %d\n",            SNS_Orientation.Struct.Pitch);
				printf("SNS_Roll: %d\n",             SNS_Orientation.Struct.Roll);
				printf("SWS_TrueSpeed: %f\n",        SWS_Data.Struct.TrueSpeed);
				printf("SWS_InstrumentSpeed: %f\n",  SWS_Data.Struct.InstrumentSpeed);
				printf("BIML_Pos: %d\n",             (uint8_t)(0.5 + 0.3922*BIM_GetStrapPosition(LEFT_BIM)));   // Перевод к процентной шкале с округлением
				printf("BIMR_Pos: %d\n",             (uint8_t)(0.5 + 0.3922*BIM_GetStrapPosition(RIGHT_BIM)));  // Перевод к процентной шкале с округлением
				printf("SystemState: %x\n",          SystemState);
			#endif //******************************************************************** !LOGS_ENABLE	

			// Отправляем данные математической модели
			M_Model_PrepareData (SNS_Position.Struct, SNS_Orientation.Struct, SWS_Data.Struct);
			
			// Запустим быструю диагностику системы
			SelfTestingOnline();
			// Анализ тестов и управление индикацией в полете
			SelfTesting_OnlineDiagnostics ();
			
			#ifdef TRACE_SYS_STATE	//******************************************************* Если включена трассировка состояния системы в CAN		
				// Трассировка состояния системы в CAN
				SelfTesting_SysState2CAN();	
			#endif
		}		
	}
}
