#include <stdint.h>                              // Стандартные типы Keil

#include "config.h"                              // Файл конфигурации проекта

#include "sws.h"                                 // Драйвер работы с СВС
#include "kmonshelf.h"                           // Драйвер работы с СНС
#include "bims.h"                                // Драйвер работы с БИМ
#include "1636pp52y.h"                           // Драйвер работы с внешней SPI flash 1636рр52у от Миландр
#include "25q64fv.h"                             // Драйвер работы с внешней SPI flash 25Q64FV от Winbond
#include "discreteio.h"                          // Драйвер работы дискретами (реле, LED..)
#include "analogio.h"                            // Драйвер работы с аналоговыми сигналами
#include "zpz.h"                                 // Драйвер работы c загрузчиком полетного задания
#include "otherlib.h"                            // Модуль аппаратнозависимых функций общего назначения

#include "bupdatastorage.h"                      // Хранилище данных
#include "logfs/log.fs.h"                        // Файловая система для записи логов в "черный ящик"
#include "math.model/mathmodelapi.h"             // Математическая модель системы управления полетом
#include "selftesting.h"                         // Модуль самодиагностики
#include "retarget.printf/RetargetPrintf.h"      // Переопределение функции printf для вывода в CAN и записи в черный ящик
#include "taskmanager.h"                         // Планировщик задач

#ifdef LOGS_ENABLE  //******************************************************* Если включено логирование в черный ящик
	#include "loger.h"
#endif //******************************************************************** !LOGS_ENABLE

#ifdef DEBUG_VARS	//*************************************************************** Если активна отладка переменных 
//	#include "debug.h"                             // Отладочный модуль трассировки внутрисистемных переменных в CAN
#endif //************************************************************************** !DEBUG_VARS 	

#include "radiostation.h"



int main(void)
{	 
  Clock_initialize();                            /* Настраиваем тактовую частоту процессора */
  SysTick_initialize();                          /* Запускаем системный таймер */
  Analog_initialize();                           /* Инициализируем аналоговые входы */
  Discrete_initialize();                         /* Конфигурируем дискреты, при этом все реле гарантированно переводятся в состояние по-умолчанию*/
  /* Необходимо дать СНС время на запуск */
  TimeoutType timeout;                           /* Таймаут на включение СНС */
  PinConfigType pin = LED_READY;                 /* Мигать будем светодиодом "Готов" */
  runIndication(&pin, 0.2*SECOND_TICKS);	       /* Запускаем индикацию на время инициализации и тестирования */
  setTimeout (&timeout, SNS_TURN_ON_DELAY);      /* Устанавливаем таймаут */ 
  /* Инициализируем внешнюю периферию пока СНС включается */
  SPI_1636PP52Y_initialize();                    /* Запускаем драйвер памяти для полетного задания */   
  SPI_25Q64FV_initialize();                      /* Запускаем драйвер внешней памяти "Черного ящика" */
  BIM_CAN_initialize ();                         /* Запускаем драйвер БИМов (который попутно инициализирует CAN1) */
  LogFs_initialize();                            /* Запускаем файловую систему */
  Bup_initialize();                              /* Инициализируем хранилище данных БУП */
  //while(timeoutStatus(&timeout) != TIME_IS_UP);  /* Дожидаемся запуска СНС */     
  SelfTestingFull();                             /* Запускаем фул-тест системы */
  stopIndication();                              /* Отключаем мигание */

	
	uint8_t buffer[] = "AT+GMI";
	
	int a = 2213;
	char b[10];
	
	itoa(a, b);
	
	
	
	const uint32_t  delay = 30;
	
//	setTimeout (&timeout, delay); 
//	while (timeoutStatus(&timeout) != TIME_IS_UP);
double latitude;
double longitude;
	
	while(1)
	{
		//BUP_DataUpdate ();
		//GetBatteryCharge();
		
		//SelfTesting_SNS();
		//BUP_UpdateDataFromSNS ();
		
	//////////////////////////////////////////////
//	  sendEmpty();
//		delay_ms(delay);
//		getDeviceName();
//		
//		sendEmpty();
//		delay_ms(delay);
//		getManufacturerName();

//		sendEmpty();
//		delay_ms(delay);
//		getListSDS();
//			
//		sendEmpty();
//		delay_ms(delay);
//		getListSDS();
//////////////////////////////////////////////////////
//	  sendEmpty();
//	  setTimeout (&timeout, delay); 
//	  while (timeoutStatus(&timeout) != TIME_IS_UP);
//		getDeviceName();
//		
//		sendEmpty();
//	  setTimeout (&timeout, delay); 
//	  while (timeoutStatus(&timeout) != TIME_IS_UP);
//		getManufacturerName();

//		sendEmpty();
//	  setTimeout (&timeout, delay); 
//	  while (timeoutStatus(&timeout) != TIME_IS_UP);
//		getListSDS();
//			
//		sendEmpty();
//	  setTimeout (&timeout, delay); 
//	  while (timeoutStatus(&timeout) != TIME_IS_UP);
//		getListSDS();

/////////////////////////////////////////////////////		
	updateSdsList();	
  findCoordinateInSdsList(&latitude, &longitude);	



//		checkDeviceName();
//		checkManufacturerName();
//		updateSdsList();
//		updateSdsList();
  
	}

  if(!CONNECT_ZPZ_CHECK)                             /* Проверяем подключение разъема ЗПЗ */
  {
    ZPZ_initialize();                                /* Запускаем драйвер ЗПЗ */
    BIM_enableSupply();                              /* Для тестирования БИМов подадим на них питание */
    while(!CONNECT_ZPZ_CHECK)                        /* И уходим на обслуживание режима ЗПЗ */
    {
      ZPZ_service();                                 /* Запускаем модуль обслуживания ЗПЗ */
      if(ZPZ_getStatus() == ZPZ_SC_MODE)             /* Если ЗПЗ в настоящий момент не занят тяжелым процессом (режим РК) */
        TaskManager_runZpzBackgroundMode ();         /* Будем в свободное время будем заниматься самодиагностикой */		
    }
  }
	
  /* Здесь начинается боевой режим работы */
  /* Проверим готова ли система к запуску управления 
  (периферия в порядке и шпильки парашютов на месте) */
  while(SelfTesting_PreflightDiagnostics() == ST_FAULT)
  {	
    /* Что-то пошло не так
    Запустим самодиагностику, 
    и будем ожидать в надежде готовности 
    системы к полёту */
    SelfTestingOnline ();
  }
	
  /* Если оказались здесь, значит БУП готов к работе */
  while(SelfTesting_PIN1());          /* Ждем пока стабилизирующий парашют выбросится и извлечет шпильку 1 */
  delay_ms(10000);                    /* Выжидаем 10 секунд */
  PYRO_ON();                          /* Поджигаем пиропатрон */
  delay_ms(3000);                     /* Пиропатрон включаем только на 3 сек */
  while(SelfTesting_PIN2());          /* Ждем выброс основного парашюта - извлечения Шпильки 2 */
  delay_ms(5000);                     /* Ждем 5 секунд */
  BIM_enableSupply();                 /* Включаем питание БИМов */
  delay_ms(1000);                     /* Ждем 1 секунду, чтобы питание БИМов стабилировалось */
	
  #ifdef LOGS_ENABLE	//******************************************************* Если включено логирование в черный ящик
    LogFs_createFile();               /* Создаём файл лога */
    loger_initmsg ();                 /* Пишем инициирующую информацию в лог */
  #endif //******************************************************************** !LOGS_ENABLE	
	
  SelfTestingOnline();                /* Запустим быструю диагностику системы перед запуском управления */
  MathModel_initialize();             /* Инициализируем мат. модель полета */
  TaskManager_restartCycle();         /* Сбрасываем/инициализируем список задач */
	
  /* Перед первым шагом матмодели необходимо подготовить данные */
  Bup_updateData ();                  /* Запускаем обновление данных */
  TaskManager_gotoTask (TaskTest, 0); /* Переключаем планировщик на первую из пассивных задач (диагностика) */
	
  while(1)                            /* Главный рабочий цикл */
  {                                   /* Начинаем выполнять рабочий цикл, */
    TaskManager_run();	              /* который перезапускается по таймеру */
  }
}
