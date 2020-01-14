#include "task_manager.h"

#include "config.h"
#include "SelfTesting.h"
#include "bup_data_store.h"
#include "BIM.h"
#include "Math_model/M_Model.h"
#include "Log_FS/Log_FS.h"
#include "RetargetPrintf/RetargetPrintf.h"
#include "debug.h"


#ifdef flightRegulatorCFB //******************************************************* Если выбран flightRegulatorCFB
	#include "Math_model/flightRegulatorCFB/flightRegulatorCFB.h"
#else //*************************************************************************** Если выбран Easy_reg
	#include "Math_model/EasyReg/Easy_reg.h"
#endif //************************************************************************** !flightRegulatorCFB 


/****************************************************
    TaskManagerStatus - Тип управляющей структуры
****************************************************/
typedef struct{
	uint8_t Task_num;                  // Номер текущей задачи
	uint8_t Task_LogerStage;           // Этап выполнения задачи логирования
	uint8_t Task_SelftestingStage;     // Этап выполнения задачи самодиагностики
	uint8_t Task_CanDebugStage;        // Этап выполнения задачи трассировки в CAN
} TaskManagerStatus;


/****************************************************
    TaskManager - Управляющая структура
****************************************************/
TaskManagerStatus TaskManager = {0,0,0,0};


/****************************************************
    Приватные функции задач
****************************************************/
static void task_loger(void);
static void task_selftesting(void);
static void task_can_debug(void);


/*********************************************************************************************************
    TaskManager_RestartCycle - Перезапуск цикла задач планировщика
**********************************************************************************************************/
void TaskManager_RestartCycle(void)
{
	TaskManager.Task_num = 0;
	TaskManager.Task_SelftestingStage = 0;
	TaskManager.Task_CanDebugStage = 0;
	TaskManager.Task_LogerStage = 0;
}

/*********************************************************************************************************
    TaskManagerGo2Task - Переход к задаче номер Code, на этап Stage
**********************************************************************************************************/
void TaskManagerGo2Task (TaskManagerCode Code, uint8_t Stage)
{
	// Сбрасываем планировщик
	TaskManager_RestartCycle();
	
	// Присваиваем номер задачи
	TaskManager.Task_num = Code;
	
	// Если задача с этапами, то присваиваем еще и этап
	if (Code == TaskTest)
		TaskManager.Task_SelftestingStage = Stage;
	else if (Code == TaskDebug)
		TaskManager.Task_CanDebugStage = Stage;
	else if (Code == TaskLoger)
		TaskManager.Task_LogerStage = Stage;		
}


/*********************************************************************************************************
    TaskManagerRun - Запуск планировщика задач на выполнение цикла задач
**********************************************************************************************************/
void TaskManagerRun (void)
{
	switch (TaskManager.Task_num)
	{
		case TaskStep: /* Задача № 0 - Расчет шага матмодели и управление */
		{
			// Запускаем шаг расчета модели				
			M_Model_Step();
			// Выполняем требования матмодели
			M_Model_Control();
			// Шаг расчета был выполнен, требуем обновить данные
			M_Model_Need2UpdateSet();
			
			// Переходим на выполнение следующей задачи
			TaskManager.Task_num ++;
			break;
		}
		case TaskUpdate: /* Задача № 1 - Подготовка данных для следующего шага */
		{
			// Запускаем обновление данных
			BUP_DataUpdate ();
			// Отправляем данные математической модели
			M_Model_PrepareData ();
			
			// Переходим на выполнение следующей задачи
			TaskManager.Task_num ++;
			break;
		}
		case TaskTest: /* Задача № 2 - Самодиагностика */
		{
			task_selftesting();
			break;
		}
		case TaskDebug: /* Задача № 3 - Отладка */
		{
			#ifdef DEBUG_VARS	//*************************************************************** Если активна отладка переменных 
				task_can_debug();
			#else
				TaskManager.Task_num ++;
		  #endif //************************************************************************** !DEBUG_VARS 	
			break;
		}
		case TaskLoger: /* Задача № 4 - Запись логов */
		{
			#ifdef LOGS_ENABLE	//******************************************************* Если включено логирование в черный ящик
				task_loger();
			#else 
				TaskManager.Task_num ++;
			#endif //******************************************************************** !LOGS_ENABLE
			break;
		}
		default: /* Цикл задач завершен */
		{
			// Будем ждать перезапуска по таймеру
			break;
		}
	}
}

/*********************************************************************************************************
    TaskManagerZPZBackgroundRun - Запуск планировщика фоновых процессов в режиме ЗПЗ
**********************************************************************************************************/
void TaskManagerZPZBackgroundRun (void)
{
	switch (TaskManager.Task_num)	
	{
		case 1: /* Задача № 1 - Обновление данных от СНС и СВС */
		{
			BUP_DataUpdate ();
			// Переходим на выполнение следующей задачи
			TaskManager.Task_num ++;
			break;
			
		}
		case 2:	/* Задача № 2 - Самодиагностика */
		{
			task_selftesting();
			break;
		}
		default: /* Цикл задач завершен */
		{
			// Сбрасываем цикл задач
			TaskManager_RestartCycle();
			// И указываем Задачу 1, как стартовую в новом цикле
			TaskManagerGo2Task (TaskUpdate, 0);
			break;
		}
	}
}


/*********************************************************************************************************
    task_selftesting - Задача самодиагностики
**********************************************************************************************************/
void task_selftesting(void)
{
	switch (TaskManager.Task_SelftestingStage)
	{
		case 0: /* Этап 0 - Flash память и полетное задание */
		{
			SelfTesting_1636PP52Y();
			SelfTesting_25Q64FV();
			SelfTesting_MapNtask();
			TaskManager.Task_SelftestingStage++;
			break;
		}
		case 1: /* Этап 1 - Тестирование БИМ */
		{
			SelfTesting_LEFT_BIM();
			SelfTesting_RIGHT_BIM();
			TaskManager.Task_SelftestingStage++;
			break;
		}
		case 2: /* Этап 2 - Контроль соединения БИМ и состояние дискретов */
		{
			SelfTesting_BIMS_TRY_CONNECT();
			SelfTesting_PIN1();
			SelfTesting_PIN2();
			SelfTesting_PYRO();
			SelfTesting_BLIND();
			SelfTesting_POW_BIM ();
			TaskManager.Task_SelftestingStage++;
			break;
		}
		case 3: /* Этап 3 - Проверка СВС */
		{
			SelfTesting_SWS();
			TaskManager.Task_SelftestingStage++;
			break;
		}
		case 4: /* Этап 4 - Проверка СНС */
		{
			SelfTesting_SNS();
			TaskManager.Task_SelftestingStage++;
			break;
		}
		case 5: /* Этап 5 - Доступность карты */
		{
			SelfTesting_MapAvailability (BUP_Get_Latitude(), BUP_Get_Longitude());
			TaskManager.Task_SelftestingStage++;
			break;
		}
		case 6: /* Этап 6 - Проверка напряжения на АКБ */
		{
			SelfTesting_Battery50Volt();
			TaskManager.Task_SelftestingStage++;
		}
		case 7: /* Этап 7 - Управление индикацией по результам тестов */
		{
			SelfTesting_OnlineDiagnostics ();
			TaskManager.Task_SelftestingStage++;
		}
		default: /* Переход к следующей задаче */
		{
			// Сбрасываем этап выполнения текущей задачи и переходим к следующей 
			TaskManager.Task_SelftestingStage = 0;
			TaskManager.Task_num ++;
			break;
		}
	}
}



/*********************************************************************************************************
    task_can_debug - Задача вывода отладочных данных в CAN
**********************************************************************************************************/
void task_can_debug(void)
{
	switch (TaskManager.Task_CanDebugStage)
	{
		case 0: /* Этап 0 - Подготовка данных */
		{
			debug_prepare_data();
			TaskManager.Task_CanDebugStage++;
			break;
		}
		case 1: /* Этап 1 - Отправка данных */
		{
			debug_can_full_struct ();
			TaskManager.Task_CanDebugStage++;
			break;
		}
		default: /* Переход к следующей задаче */
		{
			// Сбрасываем этап выполнения текущей задачи и переходим к следующей 
			TaskManager.Task_LogerStage = 0;
			TaskManager.Task_num ++;
			break;
		}
	}
}


/*********************************************************************************************************
    task_loger - Задача записи логов
**********************************************************************************************************/
void task_loger (void)
{
	switch (TaskManager.Task_LogerStage)
	{
		case 0:
		{
			printf("\nTimestamp, sec: %d\n", BUP_DataStorage.ControlSecond); 
			TaskManager.Task_LogerStage++; break;
		}
	  case 1:
		{
			printf("SNS_Lat: %llu\n", BUP_GetSNS_Latitude()); 
			TaskManager.Task_LogerStage++; break;
		}
		case 2:
		{
			printf("SNS_Lon: %llu\n", BUP_GetSNS_Longitude()); 
			TaskManager.Task_LogerStage++; break;
		}
		case 3:
		{
			printf("SNS_Alt: %llu\n", BUP_GetSNS_Altitude()); 
			TaskManager.Task_LogerStage++; break;
		}
		case 4:
		{
			printf("SNS_Vel_lat: %d\n", BUP_GetSNS_VelocityLatitude()); 
			TaskManager.Task_LogerStage++; break;
		}
		case 5:
		{
			printf("SNS_Vel_lon: %d\n", BUP_GetSNS_VelocityLongitude()); 
			TaskManager.Task_LogerStage++; break;
		}
		case 6:
		{
			printf("SNS_Vel_alt: %d\n", BUP_GetSNS_VelocityAltitude()); 
			TaskManager.Task_LogerStage++; break;
		}
		case 7:
		{
			printf("SNS_Course: %d\n", BUP_GetSNS_Course()); 
			TaskManager.Task_LogerStage++; break;
		}
		case 8:
		{
			printf("SNS_Heading_true: %d\n", BUP_GetSNS_HeadingTrue()); 
			TaskManager.Task_LogerStage++; break;
		}
		case 9:
		{
			printf("SNS_Heading_mgn: %d\n", BUP_GetSNS_HeadingMgn()); 
			TaskManager.Task_LogerStage++; break;
		}
		case 10:
		{
			printf("SNS_Pitch: %d\n", BUP_GetSNS_Pitch()); 
			TaskManager.Task_LogerStage++; break;
		}
		case 11:
		{
			printf("SNS_Roll: %d\n", BUP_GetSNS_Roll()); 
			TaskManager.Task_LogerStage++; break;
		}
		case 12:
		{
			printf("SWS_TrueSpeed: %f\n", BUP_GetSWS_TrueSpeed()); 
			TaskManager.Task_LogerStage++; break;
		}
		case 13:
		{
			printf("SWS_InstrumentSpeed: %f\n", BUP_GetSWS_InstrumentalSpeed()); 
			TaskManager.Task_LogerStage++; break;
		}
		case 14:
		{
			printf("SWS_AbsoluteHeight: %f\n", BUP_GetSWS_AbsoluteHeight());
			TaskManager.Task_LogerStage++; break;
		}
		case 15:
		{
			printf("BIML_Pos: %d\n",(uint8_t)(0.5 + 0.3922*BIM_GetStrapPosition(LEFT_BIM)));  // Перевод к процентной шкале с округлением
			TaskManager.Task_LogerStage++; break;  
		}
		case 16:
		{
			printf("BIMR_Pos: %d\n",(uint8_t)(0.5 + 0.3922*BIM_GetStrapPosition(RIGHT_BIM))); // Перевод к процентной шкале с округлением
			TaskManager.Task_LogerStage++; break;
		}
		case 17:
		{
			printf("SystemState: %x\n", SystemState); 
			TaskManager.Task_LogerStage++; break;
		}
		case 18:
		{
			printf("Model_Lat, deg: %f\n", BUP_Get_Latitude()); 
			TaskManager.Task_LogerStage++; break;
		}
		case 19:
		{
			printf("Model_Lon, deg: %f\n", BUP_Get_Longitude()); 
			TaskManager.Task_LogerStage++; break;
		}
		case 20:
		{
			printf("Model_Alt, m: %f\n", BUP_Get_Altitude()); 
			TaskManager.Task_LogerStage++; break;
		}
		case 21:
		{
			printf("Model_VelocityLat, m/s: %f\n", BUP_Get_VelocityLatitude()); 
			TaskManager.Task_LogerStage++; break;
		}
		case 22:
		{
			printf("Model_VelocityLon, m/s: %f\n", BUP_Get_VelocityLongitude()); 
			TaskManager.Task_LogerStage++; break;
		}
		case 23:
		{
			printf("Model_VelocityAlt, m/s: %f\n", BUP_Get_VelocityAltitude()); 
			TaskManager.Task_LogerStage++; break;
		}
		case 24:
		{
			printf("Model_HeadingTrue, rad: %f\n", BUP_Get_HeadingTrue()); 
			TaskManager.Task_LogerStage++; break;
		}
		case 25:
		{
			printf("Model_HeadingMgn, rad: %f\n", BUP_Get_HeadingMgn()); 
			TaskManager.Task_LogerStage++; break;
		}
		case 26:
		{
			printf("Model_Course, rad: %f\n", BUP_Get_Course()); 
			TaskManager.Task_LogerStage++; break;
		}
		case 27:
		{
			printf("Model_Pitch, rad: %f\n", BUP_Get_Pitch()); 
			TaskManager.Task_LogerStage++; break;
		}
		case 28:
		{
			printf("Model_Roll, rad: %f\n", BUP_Get_Roll()); 
			TaskManager.Task_LogerStage++; break;
		}
		case 29:
		{
			// Если карта рельефа в текущей позиции доступна, запишем высоту рельефа
			if (SelfTesting_STATUS(ST_MapAvailability))
				printf("MAP, m: %d\n", BUP_Get_ReliefHeight());
			else
				printf("MAP, m: NOT_AVAILABLE\n");
			
			TaskManager.Task_LogerStage++; break;
		}
		case 30:
		{
			#ifdef flightRegulatorCFB	//******************************************************* Если выбран flightRegulatorCFB
				printf("Model_BIM_CMD: %f\n", (double)(rtY.tightenSling*rtY.directionOfRotation));
			#else //*************************************************************************** Если выбран Easy_reg
				printf("Model_BIM_CMD: %f\n", (double)(rtY.BIM_CMD));
			#endif //************************************************************************** !flightRegulatorCFB
			TaskManager.Task_LogerStage++; break;
		}
		case 31:
		{
			#ifdef flightRegulatorCFB	//******************************************************* Если выбран flightRegulatorCFB
				printf("Model_TD_CMD: %d\n",  (uint8_t)rtY.cmdTouchDown);
			#else //*************************************************************************** Если выбран Easy_reg
				printf("Model_TD_CMD: %d\n",  (uint8_t)rtY.TD_CMD);
			#endif //************************************************************************** !flightRegulatorCFB
			TaskManager.Task_LogerStage++; break;
		}
		default:
		{
			// Сбрасываем этап выполнения данной задачи
			TaskManager.Task_LogerStage = 0;
			// Автоматически переходим на следующую задачу
			TaskManager.Task_num ++;
			break;
		}
	}
}
