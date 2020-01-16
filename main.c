#include <stdint.h>                              // Стандартные типы Keil

#include "config.h"                              // Файл конфигурации проекта

#include "SWS.h"                                 // Драйвер работы с СВС
#include "SNS.h"                                 // Драйвер работы с СНС
#include "BIM.h"                                 // Драйвер работы с БИМ
#include "1636PP52Y.h"                           // Драйвер работы с внешней SPI flash 1636рр52у от Миландр
#include "25Q64FV.h"                             // Драйвер работы с внешней SPI flash 25Q64FV от Winbond
#include "discrete_io.h"                         // Драйвер работы дискретами (реле, LED..)
#include "analog_io.h"                           // Драйвер работы с аналоговыми сигналами
#include "ZPZ.h"                                 // Драйвер работы c загрузчиком полетного задания
#include "otherlib.h"                            // Модуль аппаратнозависимых функций общего назначения

#include "bup_data_store.h"                      // Хранилище данных
#include "Log_FS/Log_FS.h"                       // Файловая система для записи логов в "черный ящик"
#include "Math_model/M_Model.h"                  // Математическая модель системы управления полетом
#include "SelfTesting.h"                         // Модуль самодиагностики
#include "RetargetPrintf/RetargetPrintf.h"       // Переопределение функции printf для вывода в CAN и записи в черный ящик
#include "task_manager.h"                        // Планировщик задач

#ifdef LOGS_ENABLE  //******************************************************* Если включено логирование в черный ящик
	#include "log_recorder.h"
#endif //******************************************************************** !LOGS_ENABLE

#ifdef DEBUG_VARS	//*************************************************************** Если активна отладка переменных 
	#include "debug.h"                             // Отладочный модуль трассировки внутрисистемных переменных в CAN1
#endif //************************************************************************** !DEBUG_VARS 	


int main(void)
{	 
	
	Clock_init();                   /* Настраиваем тактовую частоту процессора */
	SysTick_init();	                /* Запускаем системный таймер */
  Analog_RetargetPins();	        /* Инициализируем аналоговые входы */
	Discrete_RetargetPins();        /* Конфигурируем дискреты, при этом все реле гарантированно переводятся в состояние по-умолчанию*/
	SPI_1636PP52Y_Init();	          /* Запускаем драйвер памяти для полетного задания */   
	SPI_25Q64FV_Init();             /* Запускаем драйвер внешней памяти "Черного ящика" */
	BIM_CAN_init ();                /* Запускаем драйвер БИМов (который попутно инициализирует CAN1) */
	LogFs_Info();	                  /* Запускаем файловую систему */
	BUP_DataInit();                 /* Инициализируем хранилище данных БУП */
	SelfTestingFull();              /* Запускаем фул-тест системы */
  

//	while(1)
//	{
//		//BUP_DataUpdate ();
//		//GetBatteryCharge();
//		
//		SelfTesting_SNS();
//		BUP_UpdateDataFromSNS ();
//		
//	}


	
	if(!CONNECT_ZPZ_CHECK) /* Проверяем подключение разъема ЗПЗ */
	{
		/* Разъем подключен
		   Запускаем драйвер ЗПЗ */
		ZPZ_init();
		
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
				TaskManagerZPZBackgroundRun ();
			}				
		}
	}

	
	// Здесь начинается боевой режим работы
	
	// Проверим готова ли система к запуску управления (периферия в порядке и шпильки парашютов на месте)
	while(SelfTesting_PreflightDiagnostics() == ST_FAULT)
	{	
		// Что-то пошло не так
		// Запустим самодиагностику, и будем ожидать в надежде готовности системы к полёту
		SelfTestingOnline ();
	}
	
	// Если оказались здесь, значит БУП готов к работе
	
	// Ждем пока стабилизирующий парашют выбросится и извлечет шпильку 1
	while(SelfTesting_PIN1());
	// Выжидаем 10 секунд
	delay_ms(10000);
	// Поджигаем пиропатрон
	PYRO_ON();
	// Пиропатрон включаем только на 3 сек
	delay_ms(3000);
	
	// Ждем выброс основного парашюта - извлечения Шпильки 2
	while(SelfTesting_PIN2());
	// Ждем 5 секунд 
	delay_ms(5000);
	// Включаем питание БИМов
	BIM_Supply_ON();
	// Ждем 1 секунду, чтобы питание БИМов стабилировалось
	delay_ms(1000);
	
	#ifdef LOGS_ENABLE	//******************************************************* Если включено логирование в черный ящик
		// Создаём файл лога
		LogFs_CreateFile();
		// Пишем инициирующую информацию в лог
		loger_initmsg ();
	#endif //******************************************************************** !LOGS_ENABLE	
	
	// Запустим быструю диагностику системы перед запуском управления
	SelfTestingOnline();
	
	// Инициализируем мат. модель полета
	M_Model_Init();
	
	// Сбрасываем/инициализируем список задач
	TaskManager_RestartCycle();
	
	// Перед первым шагом матмодели необходимо подготовить данные
	// Запускаем обновление данных
	BUP_DataUpdate ();
	
	// Переключаем планировщик на первую из пассивных задач (диагностика)
	TaskManagerGo2Task (TaskTest, 0);
	
	// Главный рабочий цикл
	while(1)
	{
		// Начинаем выполнять рабочий цикл, который перезапускается по таймеру
		TaskManagerRun();	
	}
}
