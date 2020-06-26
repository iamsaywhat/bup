#include "config.h"
#include "selftesting.h"

#include "bupboard.h"
#include "discreteio.h"
#include "analogio.h"
#include "1636pp52y.h"
#include "25q64fv.h"
#include "sws.h"
#include "kmonshelf.h"
#include "bims.h"
#include "logfs/log.fs.h"
#include "heightmap/heightmap.platformdepend.h"
#include "heightmap/heightmap.h"
#include "bupdatastorage.h"
#include "radiostation.h"

#ifdef LOGS_ENABLE
#include "loger.h"
#endif


/***************************************
  Состояние системы
***************************************/
uint16_t systemState = 0;


/************************************************************************************
  SelfTestingFull - Тестирование всей периферии и програмных модулей,
  управление индикацией

  Примечание: Запускает все ниже описанные функции
************************************************************************************/
void SelfTestingFull (void)
{
  // Проверка 4х внешних SPI-flash от Миландра
  SelfTesting_1636PP52Y();
  // Проверка SPI-flash от WINBOND
  SelfTesting_25Q64FV();
  // Загружена ли карта и полетное задание
  SelfTesting_MapNtask();
  // Проверка файловой системы
  SelfTesting_LogFs();
  // Проверка состояния шпильки 1
  SelfTesting_PIN1();
  // Проверка состояния шпильки 2
  SelfTesting_PIN2();
  // Проверка состояния реле пиропатрона
  SelfTesting_PYRO();
  // Проверка состояния шторки
  SelfTesting_BLIND();
  // Проверка состояния реле питания БИМ и CAN
  SelfTesting_POW_BIM ();
  // Проверка левого БИМа
  SelfTesting_LEFT_BIM();
  // Проверка правого БИМа
  SelfTesting_RIGHT_BIM();
  // Проверка связи с SNS
  SelfTesting_SNS();
  // Проверка связи с SWS
  SelfTesting_SWS();
  // Проверка напряжения на АКБ
  SelfTesting_Battery50Volt();
  // Проверка связи с радиостанцией
  SelfTesting_Radiostation();
  // Управление индикацией, по результатам тестов периферии
  SelfTesting_OnlineDiagnostics();
}


/************************************************************************************
  SelfTestingOnline - Оперативная проверка состояния системы

  Примечание: должна периодически запускаться, например, по таймеру  
************************************************************************************/
void SelfTestingOnline (void)
{
  // Проверка 4х внешних SPI-flash от Миландра
  SelfTesting_1636PP52Y();
  // Проверка SPI-flash от WINBOND
  SelfTesting_25Q64FV();
  // Загружена ли карта и полетное задание
  SelfTesting_MapNtask();
  // Проверка левого БИМа
  SelfTesting_LEFT_BIM();
  // Проверка правого БИМа
  SelfTesting_RIGHT_BIM();
  // Проконтролируем необходимость восстановления связи с БИМ
  SelfTesting_BIMS_TRY_CONNECT();
  // Проверка состояния шпильки 1
  SelfTesting_PIN1();
  // Проверка состояния шпильки 2
  SelfTesting_PIN2();
  // Проверка состояния реле пиропатрона
  SelfTesting_PYRO();
  // Проверка состояния шторки
  SelfTesting_BLIND();
  // Проверка состояния реле питания БИМ и CAN
  SelfTesting_POW_BIM ();
  // Проверка связи с СВС
  SelfTesting_SWS();
  // Проверка связи с СНС
  SelfTesting_SNS();
  // Проверка доступности карты
  SelfTesting_MapAvailability (Bup_getCurrentPointLatitude(), Bup_getCurrentPointLongitude());
  // Проверка напряжения на АКБ
  SelfTesting_Battery50Volt();
  // Проверка связи с радиостанцией
  SelfTesting_Radiostation();
}


/************************************************************************************
  SelfTesting_PreflightDiagnostics - Анализ результатов тестирования 
  подсистем, управление световой индикацией и решение модуля о готовности 
  всей системы к запуску системы управления (предполётная диагностика)
************************************************************************************/
SelfTesting_STATUS_TYPE SelfTesting_PreflightDiagnostics (void)
{
  SelfTesting_STATUS_TYPE status = ST_OK;   // Статус анализа тестов (по-умолчанию OK)
	
  // Основная часть тестов, по которым будем судить о неисправности
  // Если хоть один из тестов отрицательный - сбрасываем флаг
  status &= SelfTesting_STATUS(ST_MAP);
  status &= SelfTesting_STATUS(ST_1636PP52Y); 
  status &= SelfTesting_STATUS(ST_25Q64FV); 
  status &= SelfTesting_STATUS(ST_LogFS);
  status &= SelfTesting_STATUS(ST_sns);
  status &= SelfTesting_STATUS(ST_sws);
  status &= SelfTesting_STATUS(ST_pin1); 	
  status &= SelfTesting_STATUS(ST_pin2);
//  status &= SelfTesting_STATUS(ST_BATTERY50V);
  		
  // По результатам тестирования принимаем решение о режиме индикации
  if(status == ST_OK)
  {
    // Гасим "Готовность", Зажигаем "Неисправность"
    LED_READY_ON();
    LED_FAULT_OFF();
  }
  else
  {
    // Гасим "Готовность", Зажигаем "Неисправность"
    LED_READY_OFF();
    LED_FAULT_ON();
  }
  // Так же возвращаем флаг результатов анализа тестов
  return status;
}


/************************************************************************************
  SelfTesting_OnlineDiagnostics - Анализ результатов тестирования подсистем, 
  управление световой индикацией и решение модуля о готовности всей системы 
  к работе.
************************************************************************************/
SelfTesting_STATUS_TYPE SelfTesting_OnlineDiagnostics (void)
{
  SelfTesting_STATUS_TYPE status = ST_OK;   // Статус анализа тестов (по-умолчанию OK)
	
  // Основная часть тестов, по которым будем судить о неисправности
  // Если хоть один из тестов отрицательный - status будет false
  status &= SelfTesting_STATUS(ST_MAP);
  status &= SelfTesting_STATUS(ST_1636PP52Y); 
  status &= SelfTesting_STATUS(ST_25Q64FV); 
  status &= SelfTesting_STATUS(ST_LogFS);
  status &= SelfTesting_STATUS(ST_sns);
  status &= SelfTesting_STATUS(ST_sws);
  status &= SelfTesting_STATUS(ST_Left_BIM); 	
  status &= SelfTesting_STATUS(ST_Right_BIM);  
//  status &= SelfTesting_STATUS(ST_BATTERY50V);
  		
  // По результатам тестирования принимаем решение о режиме индикации
  if(status == ST_OK)
  {
    // Гасим "Готовность", Зажигаем "Неисправность"
    LED_READY_ON();
    LED_FAULT_OFF();
  }
  else
  {
    // Гасим "Готовность", Зажигаем "Неисправность"
    LED_READY_OFF();
    LED_FAULT_ON();
  }
  // Так же возвращаем флаг результатов анализа тестов
  return status;
}


/************************************************************************************
  SelfTesting_1636PP52Y - Тестирование 4х внешних SPI-flash от Миландра
				
  Возвращает: ST_OK    - Если память работает
              ST_FAULT - Если память недоступна
************************************************************************************/
SelfTesting_STATUS_TYPE SelfTesting_1636PP52Y(void)
{
  SelfTesting_STATUS_TYPE previous = SelfTesting_STATUS(ST_1636PP52Y);
  
  // Проверяем просто по ID микросхемы (если ID не совпадёт - это признак неисправности)
  SelfTesting_SET_OK(ST_1636PP52Y);
  if(SPI_1636PP52Y_readID (SPI_1636PP52Y_CS1) != _1636PP52Y_ID)
    SelfTesting_SET_FAULT(ST_1636PP52Y); 
  if(SPI_1636PP52Y_readID (SPI_1636PP52Y_CS2) != _1636PP52Y_ID)
    SelfTesting_SET_FAULT(ST_1636PP52Y); 	
  if(SPI_1636PP52Y_readID (SPI_1636PP52Y_CS3) != _1636PP52Y_ID)
    SelfTesting_SET_FAULT(ST_1636PP52Y); 
  if(SPI_1636PP52Y_readID (SPI_1636PP52Y_CS4) != _1636PP52Y_ID)
    SelfTesting_SET_FAULT(ST_1636PP52Y); 

  if(SelfTesting_STATUS(ST_1636PP52Y) != previous)
  {
    if(SelfTesting_STATUS(ST_1636PP52Y) == ST_OK)
    {
      #ifdef LOGS_ENABLE
        logger_warning("1636pp52y: fault");
      #endif
    }
    else
    {
      #ifdef LOGS_ENABLE
        logger_warning("1636pp52y: ready");
      #endif
    }
  }
  
  return SelfTesting_STATUS(ST_1636PP52Y); 
}


/************************************************************************************
  SelfTesting_25Q64FV - Тестирование SPI-flash от WINBOND
				
  Возвращает: ST_OK    - Если память работает
              ST_FAULT - Если память недоступна
************************************************************************************/
SelfTesting_STATUS_TYPE SelfTesting_25Q64FV(void)
{
  SelfTesting_STATUS_TYPE previous = SelfTesting_STATUS(ST_25Q64FV);
  
  // Проверяем просто по ID микросхемы (если ID не совпадёт - это признак неисправности)
  SelfTesting_SET_FAULT(ST_25Q64FV); 
  if(SPI_25Q64FV_readID (SPI_25Q64FV_CSn) == _25Q64FV_ID)
    SelfTesting_SET_OK(ST_25Q64FV);
 
  if(SelfTesting_STATUS(ST_25Q64FV) != previous)
  {
    if(SelfTesting_STATUS(ST_25Q64FV) == ST_OK)
    {
      #ifdef LOGS_ENABLE
        logger_warning("25Q64FV: ready!");
      #endif
    }
    else
    {
      #ifdef LOGS_ENABLE
        logger_error("25Q64FV: fault!");   
      #endif
    }
  }
	
  return SelfTesting_STATUS(ST_25Q64FV);
}


/************************************************************************************
  SelfTesting_MapNtask - Проверка наличия карты рельефа и полетного задания
				
  Возвращает: ST_OK    - Если задание и карта загружены
              ST_FAULT - Если карта и задание отсутствует или повреждены
************************************************************************************/
SelfTesting_STATUS_TYPE SelfTesting_MapNtask(void)
{
  SelfTesting_STATUS_TYPE previous = SelfTesting_STATUS(ST_MAP);
  
  if(checkMap())
    SelfTesting_SET_OK(ST_MAP);
  else 
    SelfTesting_SET_FAULT(ST_MAP);

  if(SelfTesting_STATUS(ST_MAP) != previous)
  {
    if(SelfTesting_STATUS(ST_MAP) == ST_OK)
    {
      #ifdef LOGS_ENABLE
        logger_warning("map: loaded");
      #endif
    }
    else
    {
      #ifdef LOGS_ENABLE
        logger_error("map: not found");
      #endif
    }
  }
  
  return SelfTesting_STATUS(ST_MAP);
}


/************************************************************************************
  SelfTesting_LogFs - Проверка готовности файловой системы
				
  Возвращает: ST_OK    - Если файловая система исправна
              ST_FAULT - Если форматирование повреждено
************************************************************************************/
SelfTesting_STATUS_TYPE SelfTesting_LogFs(void)
{
  // Если файловая система ответила FINE - она готова к работе
  if(LogFs_initialize() == FS_FINE)
    SelfTesting_SET_OK(ST_LogFS);
  // Иначе разметка файловой системы повреждена, либо проблемы с spi-flash 
  else
    SelfTesting_SET_FAULT(ST_LogFS);
	
  return SelfTesting_STATUS(ST_LogFS);
}


/************************************************************************************
  SelfTesting_LEFT_BIM - Проверка исправности левого БИМа
				
  Возвращает: ST_OK    - Если БИМ исправен
              ST_FAULT - Если БИМ неисправен, либо связь отсутствует
************************************************************************************/
SelfTesting_STATUS_TYPE SelfTesting_LEFT_BIM(void)
{
  // Нет смысла проверять БИМ, если аппаратно на них не подано питание
  // (то есть вставлена Шпилька1 и/или реле питания БИМ выключено)
  // Есть аппаратная проблема связанная повисанием БИМов при скачках напржения
  // при которой он отвечает на запросы, но больше не управляется
  // Поэтому необходимо анализировать флаги его состояния, чтобы по ним определять
  // его состояние. Но здесь возникает вторая проблема, часто даже при штатной работе
  // БИМ может выставить флаг на очень короткий период времени, о какой-либо неисправности, 
  // хотя фактически он продолжает работать в нормальном режиме. Получается, что анализировать 
  // флаги нужно, но необходимо различать ложные флаги (мгновенные), от реальных неисправностей
  // блокирующих работу. Поэтому будем считать неисправность подтвержденной, если она
  // продолжает подтверждаться в течение некоторого времени.
  
  static TimeoutType timeout = {0, 0, TIME_IS_UP};   // Для фиксации неисправности во времени
  static uint8_t fault = 0;	                         // Флаг того, что была зафиксирована неисправность и начался отсчет.
  SelfTesting_STATUS_TYPE previous = SelfTesting_STATUS(ST_Left_BIM);
  
  // Если шпилька не вставлена и питание БИМОВ включено
  if(SelfTesting_PIN1() != ST_OK  &&  SelfTesting_POW_BIM() == ST_OK)
  {
    if(BIM_checkConnection (LEFT_BIM) == BIM_DONE)  {     // Если ответ от БИМА получен
      uint16_t flags = BIM_getStatusFlags(LEFT_BIM);      // Спрашиваем его флаги
      uint8_t summary = 0;                                // Суммируем по ИЛИ все флаги несправностей
      summary |= CHECK_SENSOR_FAULT(flags); 
      summary |= CHECK_OVERCURRENT(flags); 
      summary |= CHECK_OVERVOLT(flags); 
      summary |= CHECK_UNDER_VOLT(flags); 
      summary |= CHECK_OVERTEMPERATURE(flags); 
      summary |= CHECK_OVERLOAD(flags); 
      summary |= CHECK_POSITION_ERR(flags); 
      summary |= CHECK_HALT_OK(flags); 
//      summary |= !CHECK_READY(flags);
			
      if (!fault && summary) {                // Текущие флаги сообщают о первой неисправности, 
        fault = 1;                            // Фиксируем неиправность
        setTimeout(&timeout, 5000);           // Начинаем отсчет подтвержения неисправности
      }
      else if (!summary)                      // В данный момент неисправность ушла
      {
        fault = 0;                            // Снимаем фиксацию
        timeout.start = 0;                    // Сбрасываем таймаут
        timeout.stop = 0;              
        timeout.status = TIME_IS_UP;
      }
      if (fault && timeoutStatus(&timeout) == TIME_IS_UP)     // Фиксация неисправности не была снята и время фиксации вышло
        SelfTesting_SET_FAULT(ST_Left_BIM);                 // Подтверждаем неисправность
      else
        SelfTesting_SET_OK(ST_Left_BIM);                    // Иначе поддверждаем исправность
    }
    else 
      SelfTesting_SET_FAULT(ST_Left_BIM);                   // Если ответ не был получен, то точно неисправен
  }
  // Питание на БИМ отсутствует, проверить их нельзя, будем считать, что исправны
  else {
    if (fault)                              // За исключением случая, когда БИМ
      SelfTesting_SET_FAULT(ST_Left_BIM);   // выключены по причине по неисправности и ждут перезапуска
    else
      SelfTesting_SET_OK(ST_Left_BIM);
  }

  if(SelfTesting_STATUS(ST_Left_BIM) != previous)
  {
    if(SelfTesting_STATUS(ST_Left_BIM) == ST_OK)
    {
      #ifdef LOGS_ENABLE
        logger_warning("left-bim: ready");
      #endif
    }
    else
    {
      #ifdef LOGS_ENABLE
        logger_error("left-bim: fault");
      #endif
    }
  }
  
  return SelfTesting_STATUS(ST_Left_BIM);
}


/************************************************************************************
  SelfTesting_RIGHT_BIM - Проверка исправности правого БИМа
				
  Возвращает: ST_OK    - Если БИМ исправен
              ST_FAULT - Если БИМ неисправен, либо связь отсутствует
************************************************************************************/
SelfTesting_STATUS_TYPE SelfTesting_RIGHT_BIM(void)
{	
  // Нет смысла проверять БИМ, если аппаратно на них не подано питание
  // (то есть вставлена Шпилька1 и/или реле питания БИМ выключено)
  // Есть аппаратная проблема связанная повисанием БИМов при скачках напржения
  // при которой он отвечает на запросы, но больше не управляется
  // Поэтому необходимо анализировать флаги его состояния, чтобы по ним определять
  // его состояние. Но здесь возникает вторая проблема, часто даже при штатной работе
  // БИМ может выставить флаг на очень короткий период времени, о какой-либо неисправности, 
  // хотя фактически он продолжает работать в нормальном режиме. Получается, что анализировать 
  // флаги нужно, но необходимо различать ложные флаги (мгновенные), от реальных неисправностей
  // блокирующих работу. Поэтому будем считать неисправность подтвержденной, если она
  // продолжает подтверждаться в течение некоторого времени.
  
  static TimeoutType timeout = {0, 0, TIME_IS_UP};   // Для фиксации неисправности во времени
  static uint8_t fault = 0;	                         // Флаг того, что была зафиксирована неисправность и начался отсчет.
  SelfTesting_STATUS_TYPE previous = SelfTesting_STATUS(ST_Right_BIM);
  
  // Если шпилька не вставлена и питание БИМОВ включено
  if(SelfTesting_PIN1() != ST_OK  &&  SelfTesting_POW_BIM() == ST_OK)
  {
    if(BIM_checkConnection (RIGHT_BIM) == BIM_DONE) {      // Если ответ от БИМА получен
      uint16_t flags = BIM_getStatusFlags(RIGHT_BIM);      // Спрашиваем его флаги
      uint8_t summary = 0;                                 // Суммируем по ИЛИ все флаги несправностей
      summary |= CHECK_SENSOR_FAULT(flags); 
      summary |= CHECK_OVERCURRENT(flags); 
      summary |= CHECK_OVERVOLT(flags); 
      summary |= CHECK_UNDER_VOLT(flags); 
      summary |= CHECK_OVERTEMPERATURE(flags); 
      summary |= CHECK_OVERLOAD(flags); 
      summary |= CHECK_POSITION_ERR(flags); 
      summary |= CHECK_HALT_OK(flags); 
//      summary |= !CHECK_READY(flags);
			
        if (!fault && summary) {                // Текущие флаги сообщают о первой неисправности, 
          fault = 1;                            // Фиксируем неиправность
          setTimeout(&timeout, 4000);           // Начинаем отсчет подтвержения неисправности
        }
        else if (!summary)                      // В данный момент неисправность ушла
        {
          fault = 0;                            // Снимаем фиксацию
          timeout.start = 0;                    // Сбрасываем таймаут
          timeout.stop = 0;              
          timeout.status = TIME_IS_UP;
        }
        if(fault && timeoutStatus(&timeout) == TIME_IS_UP)   // Фиксация неисправности не была снята и время фиксации вышло
          SelfTesting_SET_FAULT(ST_Right_BIM);               // Подтверждаем неисправность
        else
          SelfTesting_SET_OK(ST_Right_BIM);                  // Иначе поддверждаем исправность
    }
    else 
      SelfTesting_SET_FAULT(ST_Right_BIM);                   // Если ответ не был получен, то точно неисправен
  }
  // Питание на БИМ отсутствует, проверить их нельзя, будем считать, что исправны
  else {
    if (fault)                               // За исключением случая, когда БИМ
      SelfTesting_SET_FAULT(ST_Right_BIM);   // выключены по причине по неисправности и ждут перезапуска
    else
      SelfTesting_SET_OK(ST_Right_BIM);
  }

  if(SelfTesting_STATUS(ST_Right_BIM) != previous)
  {
    if(SelfTesting_STATUS(ST_Right_BIM) == ST_OK)
    {
      #ifdef LOGS_ENABLE
        logger_warning("right-bim: ready");
      #endif
    }
    else
    {
      #ifdef LOGS_ENABLE
        logger_error("right-bim: fault");
      #endif
    }
  }
  
  return SelfTesting_STATUS(ST_Right_BIM);
}


/************************************************************************************
  SelfTesting_BIMS_TRY_CONNECT - Попытка восстановления связи с БИМами
				
  Примечание: Данная функция будет перезапускать питание БИМ в случае,
              если связь хотя бы с одним из них будет нарушена. Имеется ограничение
              на частоту переключения реле питания, для обеспечения этого в функции
              выполняется таймаут контроль. Во время таймаута никаких действий не 
              предпринимается.
************************************************************************************/
void SelfTesting_BIMS_TRY_CONNECT(void)
{
  static TimeoutType timeout = {0, 0, TIME_IS_UP};   // Контроль таймаутом с момента последнего вкл/откл реле
  static uint8_t needToReset = 0;                    // Флаг ожидания перезапуска
	
  if(needToReset)                                    // Нужно перезапустить Реле
  {
    if(timeoutStatus(&timeout) == TIME_IS_UP)        // Необходимая пауза между включения и отключения выдержана
    {
      setTimeout(&timeout, 10000);                   // Взводим новый таймаут
      BIM_enableSupply();                            // Включаем БИМЫ
      needToReset = 0;                               // Сбрасываем флаг ожидания перезапуска
    }
  }
  else if ((SelfTesting_STATUS(ST_POW_BIM) == ST_ENABLE) &&    // Если питание на бимах есть гарантированно
           (SelfTesting_PIN1() == ST_DISABLE))           
  {
    if(SelfTesting_STATUS(ST_Right_BIM) == ST_FAULT ||         // Но хотя бы один из них неисправен
       SelfTesting_STATUS(ST_Left_BIM) == ST_FAULT)            
    {
      if (timeoutStatus(&timeout) == TIME_IS_UP)               // И время с момента поледнего перезапуска прошло
      {
        BIM_disableSupply();                                   // Отключаем реле питания
        setTimeout(&timeout, 10000);                           // Взводим новый таймаут
        needToReset = 1;                                       // Взводим флаг необходимости перезауска
        #ifdef LOGS_ENABLE
          logger_error("bims is faulty! trying to restart power");
        #endif
      }
    }			
  }
}


/************************************************************************************
  SelfTesting_PIN1 - Проверка состояния шпильки 1
				
  Возвращает: ST_OK    - Если шпилька 1 вставлена
              ST_FAULT - Если шпилька 1 отсутствует
************************************************************************************/
SelfTesting_STATUS_TYPE SelfTesting_PIN1(void)
{
  SelfTesting_STATUS_TYPE previous = SelfTesting_STATUS(ST_pin1);

  if(PIN1_CHECK)
    SelfTesting_SET_OK(ST_pin1);
  else
    SelfTesting_SET_FAULT(ST_pin1);

  if(SelfTesting_STATUS(ST_pin1) != previous)
  {
    if(SelfTesting_STATUS(ST_pin1) == ST_OK)
    {
      #ifdef LOGS_ENABLE
        logger_warning("pin1: has been inserted");
      #endif
    }
    else
    {
      #ifdef LOGS_ENABLE
        logger_warning("pin1: has been removed");   
      #endif
    }      
  } 
  
  return SelfTesting_STATUS(ST_pin1);
}


/************************************************************************************
  SelfTesting_PIN2 - Проверка состояния шпильки 2
				
  Возвращает: ST_OK    - Если шпилька 2 вставлена
              ST_FAULT - Если шпилька 2 отсутствует
************************************************************************************/
SelfTesting_STATUS_TYPE SelfTesting_PIN2(void)
{
  SelfTesting_STATUS_TYPE previous = SelfTesting_STATUS(ST_pin2);
  
  if(PIN2_DIR_CHECK && !PIN2_INV_CHECK)
    SelfTesting_SET_OK(ST_pin2);
  else
    SelfTesting_SET_FAULT(ST_pin2);
 
  if(SelfTesting_STATUS(ST_pin2) != previous)
  {
    if(SelfTesting_STATUS(ST_pin2) == ST_OK)
    {
      #ifdef LOGS_ENABLE
        logger_warning("pin2: has been inserted!");
      #endif
    }
    else
    {
      #ifdef LOGS_ENABLE
        logger_warning("pin2: has been removed!");
      #endif
    }
  }
  
  return SelfTesting_STATUS(ST_pin2);
}


/************************************************************************************
  SelfTesting_PYRO - Проверка состояния реле пиропатрона
				
  Возвращает: ST_OK    - Если реле пиропатрона включено
              ST_FAULT - Если реле пиропатрона выключено
************************************************************************************/
SelfTesting_STATUS_TYPE SelfTesting_PYRO(void)
{
  SelfTesting_STATUS_TYPE previous = SelfTesting_STATUS(ST_pyro);

  if(PYRO_CHECK)
    SelfTesting_SET_OK(ST_pyro);
  else
    SelfTesting_SET_FAULT(ST_pyro);

  if(SelfTesting_STATUS(ST_pyro) != previous)
  {
    if(SelfTesting_STATUS(ST_pyro) == ST_OK)
    {
      #ifdef LOGS_ENABLE
        logger_warning("pyro: has been enabled!");
      #endif
    }
    else
    {
      #ifdef LOGS_ENABLE
        logger_warning("pyro: has been disabled!"); 
      #endif      
    }
  }
  
  return SelfTesting_STATUS(ST_pyro);
}


/************************************************************************************
  SelfTesting_BLIND - Проверка состояния реле створки
				
  Возвращает: ST_OK    - Если реле створки включено
              ST_FAULT - Если реле створки выключено
************************************************************************************/
SelfTesting_STATUS_TYPE SelfTesting_BLIND(void)
{
  SelfTesting_STATUS_TYPE previous = SelfTesting_STATUS(ST_blind);
  
  if(BLIND_CTRL_CHECK && BLIND_CHECK)
    SelfTesting_SET_OK(ST_blind);
  else
    SelfTesting_SET_FAULT(ST_blind);
 
  if(SelfTesting_STATUS(ST_blind) != previous)
  {
    if(SelfTesting_STATUS(ST_blind) == ST_OK)
    {
      #ifdef LOGS_ENABLE
        logger_warning("blind: has been enabled");
      #endif
    }
    else
    {
      #ifdef LOGS_ENABLE
        logger_warning("blind: has been disabled");
      #endif
    }
  }
  
  return SelfTesting_STATUS(ST_blind);
}


/************************************************************************************
  SelfTesting_SNS - Проверка связи с СНС
				
  Возвращает: ST_OK    - Если связь с СНС есть
              ST_FAULT - Если связь с СНС отсутствует
************************************************************************************/
SelfTesting_STATUS_TYPE SelfTesting_SNS(void)
{
  SelfTesting_STATUS_TYPE previous = SelfTesting_STATUS(ST_sns);
  
  /* Спросим у СНС состояние */
  if(SNS_updateDeviceInformation() == SNS_OK)
    SelfTesting_SET_OK(ST_sns);     /* Ответ получен */
  else
    SelfTesting_SET_FAULT(ST_sns);  /* Нет ответа */

  if(SelfTesting_STATUS(ST_sns) != previous)
  {
    if(SelfTesting_STATUS(ST_sns) == ST_FAULT)
    {
      #ifdef LOGS_ENABLE
        logger_error("sns: connection lost!");
      #endif
    }
    else
    {
      #ifdef LOGS_ENABLE
        logger_warning("sns: connection restored!");
      #endif
    }
  }
  
  return SelfTesting_STATUS(ST_sns);
}


/************************************************************************************
  SelfTesting_SWS - Проверка связи с СВС
				
  Возвращает: ST_OK    - Если связь с СВС есть
              ST_FAULT - Если связь с СВС отсутствует
************************************************************************************/
SelfTesting_STATUS_TYPE SelfTesting_SWS(void)
{
  SelfTesting_STATUS_TYPE previous = SelfTesting_STATUS(ST_sws);
  
  if(SWS_update())
    SelfTesting_SET_FAULT(ST_sws);
  else
    SelfTesting_SET_OK(ST_sws);

  if(SelfTesting_STATUS(ST_sws) != previous)
  {
    if(SelfTesting_STATUS(ST_sws) == ST_FAULT)
    {
      #ifdef LOGS_ENABLE
        logger_error("sws: connection lost!");
      #endif
    }
    else
    {
      #ifdef LOGS_ENABLE
        logger_warning("sws: connection restored!");
      #endif
    }
  }
  
  return SelfTesting_STATUS(ST_sws);
}


/************************************************************************************
  SelfTesting_POW_BIM - Проверка состояния реле питания БИМ
				
  Возвращает: ST_OK    - Если реле питания БИМ включено
              ST_FAULT - Если реле питания БИМ выключено
************************************************************************************/
SelfTesting_STATUS_TYPE SelfTesting_POW_BIM (void)
{
  SelfTesting_STATUS_TYPE previous = SelfTesting_STATUS(ST_POW_BIM);
  
  if(RELAY_BIM_CHECK)
    SelfTesting_SET_OK(ST_POW_BIM);
  else
    SelfTesting_SET_FAULT(ST_POW_BIM);

  if(SelfTesting_STATUS(ST_POW_BIM) != previous)
  {  
    if(SelfTesting_STATUS(ST_POW_BIM) == ST_ENABLE)
    {
      #ifdef LOGS_ENABLE
        logger_warning("bims-relay: has been enabled!");
      #endif
    }
    else
    {
      #ifdef LOGS_ENABLE
        logger_warning("bims-relay: has been disabled!");
      #endif
    }
  }
  
  return SelfTesting_STATUS(ST_POW_BIM);
}
	

/************************************************************************************
  SelfTesting_MapAvailability - Проверка доступноти карты рельефа в 
  точке с координатами (Lat, Lon).

  Параметры:
            Lat - Широта в градусах
            Lon - Долгота в грудусах

  Возвращает: ST_OK - Если карта доступна в данной точке
              ST_FAULT - Если карта доступна в данной точке
										
  Примечание: Функция предполагает использование с координатами ТЕКУЩЕГО
              местоположения, которые необходимо использовать извне.
************************************************************************************/
SelfTesting_STATUS_TYPE SelfTesting_MapAvailability (double Lat, double Lon)
{
  SelfTesting_STATUS_TYPE previous = SelfTesting_STATUS(ST_MapAvailability);
  
  if(getAvailabilityStatus(Lon, Lat) == MAP_NOT_AVAILABLE) // Если в точке (Lat, Lon) карта недоступна,
    SelfTesting_SET_FAULT(ST_MapAvailability);             // То сбрасываем бит доступности
  else                                                     // Иначе устанавливаем
    SelfTesting_SET_OK(ST_MapAvailability);
	
  if(SelfTesting_STATUS(ST_MapAvailability) != previous)
  {
    if(SelfTesting_STATUS(ST_MapAvailability) == ST_OK)
    {
      #ifdef LOGS_ENABLE
        logger_warning("map: available");
      #endif
    }
    else
    {
      #ifdef LOGS_ENABLE
        logger_error("map: out of area");
      #endif
    }
  }
  
  return SelfTesting_STATUS(ST_MapAvailability);
}


/************************************************************************************
  SelfTesting_Battery50Volt - Проверка напряжения на 50 вольтовой секции АКБ

  Параметры:
                NONE
								
  Возвращает: ST_OK - Если напряжение в норме
              ST_FAULT - Если напряжение ниже порогового
										
  Примечание: Функция помимо проверки напряжения, так же заполняет 
              соответсвующее поле в хранилище bup_data_store
************************************************************************************/
SelfTesting_STATUS_TYPE SelfTesting_Battery50Volt (void)
{
  SelfTesting_STATUS_TYPE previous = SelfTesting_STATUS(ST_BATTERY50V);
  
  bupDataStorage.battery50V = getBatteryCharge();
  if(bupDataStorage.battery50V < 47.0)
    SelfTesting_SET_FAULT(ST_BATTERY50V);
  else
    SelfTesting_SET_OK(ST_BATTERY50V);
 
  if(SelfTesting_STATUS(ST_BATTERY50V) != previous)
  { 
    if(SelfTesting_STATUS(ST_BATTERY50V) == ST_FAULT)
    {
      #ifdef LOGS_ENABLE
        logger_error("battery: low voltage");
      #endif
    }
  }
    
  return SelfTesting_STATUS(ST_BATTERY50V);
}

/************************************************************************************
  SelfTesting_Radiostation - Проверка связи с радиостанцией

  Параметры:
              NONE
								
  Возвращает: ST_OK - Если напряжение в норме
              ST_FAULT - Если напряжение ниже порогового
										
  Примечание:
************************************************************************************/
SelfTesting_STATUS_TYPE SelfTesting_Radiostation (void)
{
  SelfTesting_STATUS_TYPE previous = SelfTesting_STATUS(ST_RADIOSTATION);
  RadioStatus status;
  
  if(Radiostation.currentBaudrate() == RADIO_DEFAULT_BAUDRATE)
    status = Radiostation.setBaudrate(230400);
  else
    status = Radiostation.sendEmpty();
  if(status == RADIO_SUCCESS)
    SelfTesting_SET_OK(ST_RADIOSTATION);
  else
    SelfTesting_SET_FAULT(ST_RADIOSTATION);
   
  if(SelfTesting_STATUS(ST_RADIOSTATION) != previous)
  {
    if(SelfTesting_STATUS(ST_RADIOSTATION) == ST_OK)
    {
      #ifdef LOGS_ENABLE
        logger_warning("radio: connection restored!");
      #endif
    }
    else
    {
      #ifdef LOGS_ENABLE
        logger_warning("radio: connection lost!");
      #endif
    }
  }

  
  return SelfTesting_STATUS(ST_RADIOSTATION);
}
