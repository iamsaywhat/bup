#include "SelfTesting.h"

#include "bupboard.h"
#include "discrete_io.h"
#include "analog_io.h"
#include "1636PP52Y.h"
#include "25Q64FV.h"
#include "SWS.h"
#include "SNS.h"
#include "BIM.h"
#include "Log_FS/Log_FS.h"
#include "HeightMap/Heightmap_conf_and_pd.h"
#include "HeightMap/Heightmap.h"
#include "RetargetPrintf/RetargetPrintf.h"
#include "bup_data_store.h"


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
  SelfTesting_MapAvailability (BUP_Get_Latitude(), BUP_Get_Longitude());
  // Проверка напряжения на АКБ
  SelfTesting_Battery50Volt();
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
  if(SelfTesting_STATUS(ST_MAP)       == ST_FAULT 
  || SelfTesting_STATUS(ST_1636PP52Y) == ST_FAULT 
  || SelfTesting_STATUS(ST_25Q64FV)   == ST_FAULT 
  || SelfTesting_STATUS(ST_LogFS)     == ST_FAULT
  || SelfTesting_STATUS(ST_sns)       == ST_FAULT
  || SelfTesting_STATUS(ST_sws)       == ST_FAULT
  || SelfTesting_STATUS(ST_pin1)      == ST_FAULT 	
  || SelfTesting_STATUS(ST_pin2)      == ST_FAULT
  || SelfTesting_STATUS(ST_BATTERY50V)== ST_FAULT)
  {
    // Если хоть один из тестов отрицательный - сбрасываем флаг
    status = ST_FAULT;
  }
		
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
  return (SelfTesting_STATUS_TYPE)status;
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
  if(SelfTesting_STATUS(ST_MAP)       == ST_FAULT 
  || SelfTesting_STATUS(ST_1636PP52Y) == ST_FAULT 
  || SelfTesting_STATUS(ST_25Q64FV)   == ST_FAULT 
  || SelfTesting_STATUS(ST_LogFS)     == ST_FAULT
  || SelfTesting_STATUS(ST_sns)       == ST_FAULT 
  || SelfTesting_STATUS(ST_sws)       == ST_FAULT
  || SelfTesting_STATUS(ST_BATTERY50V)== ST_FAULT)
  {
    // Если хоть один из тестов отрицательный - сбрасываем флаг
    status = ST_FAULT;
  }
		
  // Если БИМы аппаратно запитаны (вынута шпилька1 и включено реле),
  // То не обходимо проверить связь с ними
  if(SelfTesting_STATUS(ST_pin1) != ST_OK && SelfTesting_STATUS(ST_POW_BIM) == ST_OK)
  {
    // Если хоть один из БИМов неисправен
    if(SelfTesting_STATUS(ST_Left_BIM) == ST_FAULT || SelfTesting_STATUS(ST_Right_BIM) == ST_FAULT)
    {
      // Сбрасывает флаг и признаём неисправность
      status = ST_FAULT;
    }
  }
		
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
  return (SelfTesting_STATUS_TYPE)status;
}


/************************************************************************************
  SelfTesting_1636PP52Y - Тестирование 4х внешних SPI-flash от Миландра
				
  Возвращает: ST_OK    - Если память работает
              ST_FAULT - Если память недоступна
************************************************************************************/
SelfTesting_STATUS_TYPE SelfTesting_1636PP52Y(void)
{
  // Проверяем просто по ID микросхемы (если ID не совпадёт - это признак неисправности)
  SelfTesting_SET_OK(ST_1636PP52Y);
  if(SPI_1636PP52Y_ReadID (SPI_1636PP52Y_CS1) != _1636PP52Y_ID)
    SelfTesting_SET_FAULT(ST_1636PP52Y); 
  if(SPI_1636PP52Y_ReadID (SPI_1636PP52Y_CS2) != _1636PP52Y_ID)
    SelfTesting_SET_FAULT(ST_1636PP52Y); 	
  if(SPI_1636PP52Y_ReadID (SPI_1636PP52Y_CS3) != _1636PP52Y_ID)
    SelfTesting_SET_FAULT(ST_1636PP52Y); 
  if(SPI_1636PP52Y_ReadID (SPI_1636PP52Y_CS4) != _1636PP52Y_ID)
    SelfTesting_SET_FAULT(ST_1636PP52Y); 
	
  return (SelfTesting_STATUS_TYPE)SelfTesting_STATUS(ST_1636PP52Y); 
}


/************************************************************************************
  SelfTesting_25Q64FV - Тестирование SPI-flash от WINBOND
				
  Возвращает: ST_OK    - Если память работает
              ST_FAULT - Если память недоступна
************************************************************************************/
SelfTesting_STATUS_TYPE SelfTesting_25Q64FV(void)
{
  // Проверяем просто по ID микросхемы (если ID не совпадёт - это признак неисправности)
  SelfTesting_SET_FAULT(ST_25Q64FV); 
  if(SPI_25Q64FV_ReadID (SPI_25Q64FV_CSn) == _25Q64FV_ID)
    SelfTesting_SET_OK(ST_25Q64FV);
	
  return (SelfTesting_STATUS_TYPE)SelfTesting_STATUS(ST_25Q64FV);
}


/************************************************************************************
  SelfTesting_MapNtask - Проверка наличия карты рельефа и полетного задания
				
  Возвращает: ST_OK    - Если задание и карта загружены
              ST_FAULT - Если карта и задание отсутствует или повреждены
************************************************************************************/
SelfTesting_STATUS_TYPE SelfTesting_MapNtask(void)
{
  if(CheckMap())
    SelfTesting_SET_OK(ST_MAP);
  else 
    SelfTesting_SET_FAULT(ST_MAP);
	
  return (SelfTesting_STATUS_TYPE)SelfTesting_STATUS(ST_MAP);
}


/************************************************************************************
  SelfTesting_LogFs - Проверка готовности файловой системы
				
  Возвращает: ST_OK    - Если файловая система исправна
              ST_FAULT - Если форматирование повреждено
************************************************************************************/
SelfTesting_STATUS_TYPE SelfTesting_LogFs(void)
{
  // Если файловая система ответила FINE - она готова к работе
  if(LogFs_Info() == FS_FINE)
    SelfTesting_SET_OK(ST_LogFS);
  // Иначе разметка файловой системы повреждена, либо проблемы с spi-flash 
  else
    SelfTesting_SET_FAULT(ST_LogFS);
	
  return (SelfTesting_STATUS_TYPE)SelfTesting_STATUS(ST_LogFS);
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
	
  // Если шпилька не вставлена и питание БИМОВ включено
  if(SelfTesting_PIN1() != ST_OK  &&  SelfTesting_POW_BIM() == ST_OK)
  {
    if(BIM_CheckConnection (LEFT_BIM))
      SelfTesting_SET_OK(ST_Left_BIM);
    else 
    SelfTesting_SET_FAULT(ST_Left_BIM);
  }
  // Питание на БИМ отсутствует, проверить их нельзя, будем считать, что исправны
  else 
    SelfTesting_SET_FAULT(ST_Left_BIM);
	
  return (SelfTesting_STATUS_TYPE)SelfTesting_STATUS(ST_Left_BIM);
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
	
  // Если шпилька не вставлена и питание БИМОВ включено
  if(SelfTesting_PIN1() != ST_OK  &&  SelfTesting_POW_BIM() == ST_OK)
  {
    // Можно проверить связь
    if(BIM_CheckConnection (RIGHT_BIM))
      SelfTesting_SET_OK(ST_Right_BIM);
    else 
      SelfTesting_SET_FAULT(ST_Right_BIM);
  }
  // Питание на БИМ отсутствует, проверить их нельзя, будем считать, что исправны
  else
    SelfTesting_SET_FAULT(ST_Right_BIM);
	
  return (SelfTesting_STATUS_TYPE)SelfTesting_STATUS(ST_Right_BIM);
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
	static TimeoutType timeout = {0, 1, TIME_IS_UP};
	
  /* Если хоть один из БИМов неисправен, 
	и с последней попытки восстановить соединение случился таймаут */ 
  if((SelfTesting_STATUS(ST_Right_BIM) != ST_OK || 
      SelfTesting_STATUS(ST_Left_BIM)  != ST_OK) 
      && (timeoutStatus(&timeout) == TIME_IS_UP))
  {
    /* Заводим таймаут на 10 сек */
    setTimeout(&timeout, 10000);
    /* Изменим состояние реле питания БИМ на противоположное */
    if(SelfTesting_STATUS(ST_POW_BIM)) 
      BIM_Supply_OFF();
    else 
      BIM_Supply_ON();
  }
}


/************************************************************************************
  SelfTesting_PIN1 - Проверка состояния шпильки 1
				
  Возвращает: ST_OK    - Если шпилька 1 вставлена
              ST_FAULT - Если шпилька 1 отсутствует
************************************************************************************/
SelfTesting_STATUS_TYPE SelfTesting_PIN1(void)
{
  if(PIN1_CHECK)
    SelfTesting_SET_OK(ST_pin1);
  else
    SelfTesting_SET_FAULT(ST_pin1);
	
  return (SelfTesting_STATUS_TYPE)SelfTesting_STATUS(ST_pin1);
}


/************************************************************************************
  SelfTesting_PIN2 - Проверка состояния шпильки 2
				
  Возвращает: ST_OK    - Если шпилька 2 вставлена
              ST_FAULT - Если шпилька 2 отсутствует
************************************************************************************/
SelfTesting_STATUS_TYPE SelfTesting_PIN2(void)
{
  if(PIN2_DIR_CHECK && !PIN2_INV_CHECK)
    SelfTesting_SET_OK(ST_pin2);
  else
    SelfTesting_SET_FAULT(ST_pin2);
	
  return (SelfTesting_STATUS_TYPE)SelfTesting_STATUS(ST_pin2);
}


/************************************************************************************
  SelfTesting_PYRO - Проверка состояния реле пиропатрона
				
  Возвращает: ST_OK    - Если реле пиропатрона включено
              ST_FAULT - Если реле пиропатрона выключено
************************************************************************************/
SelfTesting_STATUS_TYPE SelfTesting_PYRO(void)
{
  if(PYRO_CHECK)
    SelfTesting_SET_OK(ST_pyro);
  else
    SelfTesting_SET_FAULT(ST_pyro);
	
  return (SelfTesting_STATUS_TYPE)SelfTesting_STATUS(ST_pyro);
}


/************************************************************************************
  SelfTesting_BLIND - Проверка состояния реле створки
				
  Возвращает: ST_OK    - Если реле створки включено
              ST_FAULT - Если реле створки выключено
************************************************************************************/
SelfTesting_STATUS_TYPE SelfTesting_BLIND(void)
{
  if(BLIND_CTRL_CHECK && BLIND_CHECK)
    SelfTesting_SET_OK(ST_blind);
  else
    SelfTesting_SET_FAULT(ST_blind);
	
  return (SelfTesting_STATUS_TYPE)SelfTesting_STATUS(ST_blind);
}


/************************************************************************************
  SelfTesting_SNS - Проверка связи с СНС
				
  Возвращает: ST_OK    - Если связь с СНС есть
              ST_FAULT - Если связь с СНС отсутствует
************************************************************************************/
SelfTesting_STATUS_TYPE SelfTesting_SNS(void)
{
  SNS_Device_Information_Response_Union   SNS_DeviceInformation;
	
  /* Спросим у СНС состояние */
  if(SNS_GetDeviceInformation(&SNS_DeviceInformation) == SNS_OK)
    SelfTesting_SET_OK(ST_sns);     /* Ответ получен */
  else
    SelfTesting_SET_FAULT(ST_sns);  /* Нет ответа */

  return (SelfTesting_STATUS_TYPE)SelfTesting_STATUS(ST_sns);
}


/************************************************************************************
  SelfTesting_SWS - Проверка связи с СВС
				
  Возвращает: ST_OK    - Если связь с СВС есть
              ST_FAULT - Если связь с СВС отсутствует
************************************************************************************/
SelfTesting_STATUS_TYPE SelfTesting_SWS(void)
{
  SWS_Packet_Type_Union SWS_Pack;
	
  if(SWS_GetPacket (&SWS_Pack))
    SelfTesting_SET_FAULT(ST_sws);
  else
    SelfTesting_SET_OK(ST_sws);
	
  return (SelfTesting_STATUS_TYPE)SelfTesting_STATUS(ST_sws);
}


/************************************************************************************
  SelfTesting_POW_BIM - Проверка состояния реле питания БИМ
				
  Возвращает: ST_OK    - Если реле питания БИМ включено
              ST_FAULT - Если реле питания БИМ выключено
************************************************************************************/
SelfTesting_STATUS_TYPE SelfTesting_POW_BIM (void)
{
  if(RELAY_BIM_CHECK)
    SelfTesting_SET_OK(ST_POW_BIM);
  else
    SelfTesting_SET_FAULT(ST_POW_BIM);
	
  return (SelfTesting_STATUS_TYPE)SelfTesting_STATUS(ST_POW_BIM);
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
  // Если в точке (Lat, Lon) карта недоступна,
  if(GetAvailabilityStatus(Lon, Lat) == MAP_NOT_AVAILABLE)
  {
    // То сбрасываем бит доступности
    SelfTesting_SET_FAULT(ST_MapAvailability);
  }
  else
  {
    // Иначе устанавливаем
    SelfTesting_SET_OK(ST_MapAvailability);
  }
	
  return (SelfTesting_STATUS_TYPE)SelfTesting_STATUS(ST_MapAvailability);
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
  BUP_DataStorage.Battery50V = GetBatteryCharge();
  if(BUP_DataStorage.Battery50V < 47.0)
    SelfTesting_SET_FAULT(ST_BATTERY50V);
  else
    SelfTesting_SET_OK(ST_BATTERY50V);
  return (SelfTesting_STATUS_TYPE)SelfTesting_STATUS(ST_BATTERY50V);
}





