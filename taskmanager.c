#include "taskmanager.h"

#include "config.h"
#include "selftesting.h"
#include "bupdatastorage.h"
#include "bims.h"
#include "math.model/mathmodelapi.h"
#include "logfs/log.fs.h"
#include "retarget.printf/RetargetPrintf.h"
#include "debug.h"
#include "loger.h"


#ifdef flightRegulatorCFB //******************************************************* Если выбран flightRegulatorCFB
	#include "math.model/flightRegulatorCFB/flightRegulatorCFB.h"
#else //*************************************************************************** Если выбран flightController
	#include "math.model/flightController/flightController.h"
#endif //************************************************************************** !flightRegulatorCFB 


/****************************************************
  TaskManagerStatus - Тип управляющей структуры
****************************************************/
typedef struct{
  uint8_t Task_num;                  // Номер текущей задачи
  uint8_t Task_LogerStage;           // Этап выполнения задачи логирования
  uint8_t Task_SelftestingStage;     // Этап выполнения задачи самодиагностики
  uint8_t Task_CanDebugStage;        // Этап выполнения задачи трассировки в CAN
}TaskManagerStatus;


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
  TaskManager_restartCycle - Перезапуск цикла задач планировщика
**********************************************************************************************************/
void TaskManager_restartCycle(void)
{
  TaskManager.Task_num = 0;
  TaskManager.Task_SelftestingStage = 0;
  TaskManager.Task_CanDebugStage = 0;
  TaskManager.Task_LogerStage = 0;
}

/*********************************************************************************************************
  TaskManager_gotoTask - Переход к задаче номер Code, на этап Stage
**********************************************************************************************************/
void TaskManager_gotoTask (TaskManagerCode Code, uint8_t Stage)
{
  // Сбрасываем планировщик
  TaskManager_restartCycle();
	
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
  TaskManager_run - Запуск планировщика задач на выполнение цикла задач
**********************************************************************************************************/
void TaskManager_run (void)
{
  switch (TaskManager.Task_num)
  {
    case TaskStep: /* Задача № 0 - Расчет шага матмодели и управление */
    {
      // Запускаем шаг расчета модели				
      MathModel_step();
      // Выполняем требования матмодели
      MathModel_control();
			
      // Переходим на выполнение следующей задачи
      TaskManager.Task_num ++;
      break;
    }
    case TaskUpdate: /* Задача № 1 - Подготовка данных для следующего шага */
    {
      // Запускаем обновление данных
      Bup_updateData ();
      // Отправляем данные математической модели
      MathModel_prepareData ();
			
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
  TaskManager_runZpzBackgroundMode - Запуск планировщика фоновых процессов в режиме ЗПЗ
**********************************************************************************************************/
void TaskManager_runZpzBackgroundMode (void)
{
  switch (TaskManager.Task_num)	
  {
    case 1: /* Задача № 1 - Обновление данных от СНС и СВС */
    {
      Bup_updateData ();
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
      TaskManager_restartCycle();
      // И указываем Задачу 1, как стартовую в новом цикле
      TaskManager_gotoTask (TaskUpdate, 0);
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
      SelfTesting_MapAvailability (Bup_getCurrentPointLatitude(), Bup_getCurrentPointLongitude());
      TaskManager.Task_SelftestingStage++;
      break;
    }
    case 6: /* Этап 6 - Проверка напряжения на АКБ */
    {
      SelfTesting_Battery50Volt();
      TaskManager.Task_SelftestingStage++;
    }
    case 7: /* Этап 7 - Проверка радиостанции */
    {
      SelfTesting_Radiostation();
      TaskManager.Task_SelftestingStage++;
    }
    case 8: /* Этап 8 - Управление индикацией по результам тестов */
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
      logger_series("timestamp", Bup_getControlTime());
      TaskManager.Task_LogerStage++; break;
    }
    case 1:
    {
      logger_series("sns_lat", SNS_position.Struct.Pos_lat);       
      TaskManager.Task_LogerStage++; break;
    }
    case 2:
    {
      logger_series("sns_lon", SNS_position.Struct.Pos_lon); 
      TaskManager.Task_LogerStage++; break;
    }
    case 3:
    {
      logger_series("sns_alt", SNS_position.Struct.Pos_alt); 
      TaskManager.Task_LogerStage++; break;
    }
    case 4:
    {
      logger_series("sns_vel_lat", SNS_position.Struct.Vel_lat); 
      TaskManager.Task_LogerStage++; break;
    }
    case 5:
    {
      logger_series("sns_vel_lon", SNS_position.Struct.Vel_lon); 
      TaskManager.Task_LogerStage++; break;
    }
    case 6:
    {
      logger_series("sns_vel_alt", SNS_position.Struct.Vel_alt); 
      TaskManager.Task_LogerStage++; break;
    }
    case 7:
    {
      logger_series("sns_course", SNS_position.Struct.Course); 
      TaskManager.Task_LogerStage++; break;
    }
    case 8:
    {
      logger_series("sns_heading_true", SNS_orientation.Struct.Heading_true);       
      TaskManager.Task_LogerStage++; break;
    }
    case 9:
    {
      logger_series("sns_heading_mgn", SNS_orientation.Struct.Heading_mgn);  
      TaskManager.Task_LogerStage++; break;
    }
    case 10:
    {
      logger_series("sns_pitch", SNS_orientation.Struct.Pitch);  
      TaskManager.Task_LogerStage++; break;
    }
    case 11:
    {
      logger_series("sns_roll", SNS_orientation.Struct.Roll); 
      TaskManager.Task_LogerStage++; break;
    }
    case 12:
    {
      logger_series("sns_true_speed", SWS_getTrueSpeed()); 
      TaskManager.Task_LogerStage++; break;
    }
    case 13:
    {
      logger_series("sns_inst_speed", SWS_getInstrumentSpeed()); 
      TaskManager.Task_LogerStage++; break;
    }
    case 14:
    {
      logger_series("sns_abs_height", SWS_getAbsoluteHeight());
      TaskManager.Task_LogerStage++; break;
    }
    case 15:
    {
      logger_series("bim_l_pos", (uint8_t)(0.5 + 0.3922*BIM_getStrapPosition(LEFT_BIM))); // Перевод к процентной шкале с округлением
      TaskManager.Task_LogerStage++; break;  
    }
    case 16:
    {
      logger_series("bim_r_pos", (uint8_t)(0.5 + 0.3922*BIM_getStrapPosition(RIGHT_BIM))); // Перевод к процентной шкале с округлением
      TaskManager.Task_LogerStage++; break;
    }
    case 17:
    {
      logger_track(Bup_getCurrentPointLatitude(), Bup_getCurrentPointLongitude(), Bup_getCurrentPointAltitude());
      TaskManager.Task_LogerStage++; break;
    }
    case 18:
    {
      logger_series("velocity_lat", Bup_getCurrentVelocityLatitude());
      TaskManager.Task_LogerStage++; break;
    }
    case 19:
    {
      logger_series("velocity_lon", Bup_getCurrentVelocityLongitude());      
      TaskManager.Task_LogerStage++; break;
    }
    case 20:
    {
      logger_series("velocity_alt", Bup_getCurrentVelocityAltitude());
      TaskManager.Task_LogerStage++; break;
    }
    case 21:
    {
      logger_series("heading_true", Bup_getCurrentHeadingTrue());
      TaskManager.Task_LogerStage++; break;
    }
    case 22:
    {
      logger_series("heading_mgn", Bup_getCurrentHeadingMgn()); 
      TaskManager.Task_LogerStage++; break;
    }
    case 23:
    {
      logger_series("course", Bup_getCurrentCourse());
      TaskManager.Task_LogerStage++; break;
    }
    case 24:
    {
      logger_series("pitch", Bup_getCurrentPitch());
      TaskManager.Task_LogerStage++; break;
    }
    case 25:
    {
      logger_series("roll", Bup_getCurrentRoll());
      TaskManager.Task_LogerStage++; break;
    }
    case 26:
    {
      // Если карта рельефа в текущей позиции доступна, запишем высоту рельефа
      if (SelfTesting_STATUS(ST_MapAvailability))
        logger_series("map", Bup_getCurrentPointRelief());
      TaskManager.Task_LogerStage++; break;
    }
    case 27:
    {
      #ifdef flightRegulatorCFB	//******************************************************* Если выбран flightRegulatorCFB
      uint8_t left = 0;
      uint8_t right = 0;
      if(rtY.directionOfRotation == -1)
      {
        left= (uint8_t)rtY.tightenSling;
        right = 0;
      }
      else if(rtY.directionOfRotation == 1)
      {
        right = (uint8_t)rtY.tightenSling;
        left = 0;
      }
      else if(rtY.directionOfRotation == 2)
      {
        right = (uint8_t)rtY.tightenSling;
        left = (uint8_t)rtY.tightenSling;
      }
      logger_series("bim_left_cmd", left);
      logger_series("bim_right_cmd", right);
      #else //*************************************************************************** Если выбран Easy_reg
      logger_series("bim_left_cmd", (uint8_t)rtY.leftStrap);
      logger_series("bim_right_cmd", (uint8_t)rtY.rightStrap);
      #endif //************************************************************************** !flightRegulatorCFB
      TaskManager.Task_LogerStage++; break;
    }
    case 28:
    {
      #ifdef flightRegulatorCFB	//******************************************************* Если выбран flightRegulatorCFB
      logger_series("td_cmd", (uint8_t)rtY.cmdTouchDown);
      #else //*************************************************************************** Если выбран Easy_reg
      logger_series("td_cmd", (uint8_t)rtY.touchdown);
      #endif //************************************************************************** !flightRegulatorCFB
      TaskManager.Task_LogerStage++; break;
    }
    case 29:
    {
      static uint8_t lastUpdateIndex = 0;
      if(lastUpdateIndex != Bup_getRadioPointUpdateIndex())
      {
        logger_point("radio_point", Bup_getRadioPointLatitude(), Bup_getRadioPointLongitude(), Bup_getTouchdownPointAltitude());
        lastUpdateIndex = Bup_getRadioPointUpdateIndex();
      }
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
