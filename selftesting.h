/************************************************************************************

    SelfTesting - Модуль самодиагностики БУП, принятия решений и управления 
    индикацией "Готовности" и "Неисправности".
			 			 
			 
    Состав модуля:
    SelfTesting_1636PP52Y  - Тестирование 4х внешних SPI-flash от Миландра
    SelfTesting_25Q64FV    - Тестирование SPI-flash от WINBOND
    SelfTesting_MapNtask   - Проверка наличия карты рельефа и полетного задания
    SelfTesting_LogFs      - Проверка готовности файловой системы
    SelfTesting_LEFT_BIM   - Проверка исправности левого БИМа
    SelfTesting_RIGHT_BIM  - Проверка исправности правого БИМа
    SelfTesting_PIN1       - Проверка состояния шпильки 1
    SelfTesting_PIN2       - Проверка состояния шпильки 2
    SelfTesting_PYRO       - Проверка состояния реле пиропатрона
    SelfTesting_TDS        - Проверка состояния реле пирозамка посадочной системы
    SelfTesting_SNS        - Проверка связи с СНС
    SelfTesting_SWS        - Проверка связи с СВС
			 
    Разбитовка SystemState:
    0 бит - Map (Состояние карты и полетного задания)
    1 бит - _1636PP52Y (Состояние внешней SPI памяти 1636PP52Y от Миландр)
    2 бит - _25Q64FV (Состояние внешней SPI памяти 25Q64FV от Windbond)
    3 бит - LogFS (Состояние файловой системы)
    4 бит - Left_BIM (Состояние левого БИМа)
    5 бит - Right_BIM (Состояние правого БИМа)
    6 бит - PIN1 (Состояние шпильки 1)
    7 бит - PIN2 (Состояние шпильки 2)
    8 бит - PYRO (Состояние реле пиропатрона)
    9 бит - TDS (Состояние реле пирозамка посадочной системы)
    10 бит - SNS (Состояние СНС)
    11 бит - SWS (Состояние СВС)
    12 бит - Доступность карты рельефа
    13 бит - POW_BIM (Реле питания БИМ)
    14 бит - BATTERY50V (Секция 50 вольт АКБ)
    15 бит - Radiostation (Радиостанция)
					
    Использование:
        Состояние того или иного устройства модуля можно узнать с помощью
            SelfTesting_STATUS(x);
        Принудительная установка состояния в ИСПРАВЕН
            SelfTesting_SET_OK(x);
        Принудительная установка состояния в НЕИСПРАВЕН
            SelfTesting_RESET_OK(x);
            где x - идентификатор устройства/модуля из перечня:
                ST_MAP              - Карта и полётное задание
                ST_1636PP52Y        - Внешняя SPI память от Миландр              
                ST_25Q64FV          - Внешняя SPI память от Winbond            
                ST_LogFS            - Файловая система логирования               
                ST_Left_BIM         - Левый БИМ            
                ST_Right_BIM        - Правый БИМ            
                ST_pin1             - Шпилька 1                          (ST_OK/ST_FAULT  = Воткнута/Извлечена)
                ST_pin2             - Шпилька 2                          (ST_OK/ST_FAULT  = Воткнута/Извлечена)
                ST_pyro             - Реле пиропатрона                   (ST_OK/ST_FAULT  = Включено/Выключено)
                ST_tds              - Реле пирозамка посадочной системы  (ST_OK/ST_FAULT  = Включено/Выключено) 
                ST_sns              - СНС
                ST_sws              - СВС  				
                ST_MapAvailability  - Доступность карты рельефа
                ST_POW_BIM          - Реле питания БИМ и CAN                 (ST_OK/ST_FAULT  = Включено/Выключено)
                ST_BATTERY50V       - Секция 50 вольт АКБ
                ST_RADIOSTATION     - Радиостанция
		 
************************************************************************************/
#ifndef _SELF_TESTING_H_
#define _SELF_TESTING_H_

#include <stdint.h>


/***************************************
  Состояние системы
***************************************/
extern uint16_t systemState;


/***************************************
  Результат тестирования
***************************************/
typedef enum {
	ST_OK = 1,
	ST_FAULT = 0,
	ST_ENABLE = 1,
	ST_DISABLE = 0,
} SelfTesting_STATUS_TYPE;


/*************************************************************************
  Макросы изменения и проверки состояния системы
*************************************************************************/
#define ST_MAP                     (0)
#define ST_1636PP52Y               (1)  
#define ST_25Q64FV                 (2)
#define ST_LogFS                   (3)
#define ST_Left_BIM                (4)
#define ST_Right_BIM               (5)
#define ST_pin1                    (6)
#define ST_pin2                    (7)
#define ST_pyro                    (8)
#define ST_tds                     (9)
#define ST_sns                     (10)
#define ST_sws                     (11)
#define ST_MapAvailability         (12)
#define ST_POW_BIM                 (13)
#define ST_BATTERY50V              (14)
#define ST_RADIOSTATION            (15)
#define SelfTesting_SET_OK(x)      (systemState|=(1<<x))                                   // Результат проверки - ОК
#define SelfTesting_SET_FAULT(x)   (systemState&=~(1<<x))                                  // Результат проверки - FAULT
#define SelfTesting_STATUS(x)      (SelfTesting_STATUS_TYPE)((systemState&(1<<x))>>(x))    // Узнать состояние


/************************************************************************************
  SelfTestingFull - Тестирование всей периферии и програмных модулей,
  управление индикацией

  Примечание: Запускает все ниже описанные функции
************************************************************************************/
void SelfTestingFull (void);


/************************************************************************************
  SelfTestingOnline - Оперативная проверка состояния системы

  Примечание: должна периодически запускаться, например, по таймеру  
************************************************************************************/
void SelfTestingOnline (void);


/************************************************************************************
  SelfTesting_PreflightDiagnostics - Анализ результатов тестирования 
  подсистем, управление световой индикацией и решение модуля о готовности 
  всей системы к запуску системы управления (предполётная диагностика)
  Проверяет:
    1) Загрузку полетного задания и карты
    2) Исправность флэш памяти 1636PP52Y
    3) Исправность флэш памяти 25Q64FV
    4) Работу файловой системы черного ящика
    5) Связь с СНС
    6) Связь с СВС (?)
    7) Состояние шпильки 1
    8) Состояние шпильки 2
				
  Примечание:
    Данный анализ тестов является предполетным, то есть до момента выброски.
    Система признаётся готовой к выброске, если все шпильки вставлены.
************************************************************************************/
SelfTesting_STATUS_TYPE SelfTesting_PreflightDiagnostics (void);


/************************************************************************************
  SelfTesting_OnlineDiagnostics - Анализ результатов тестирования подсистем, 
  управление световой индикацией и решение модуля о готовности всей системы 
  к работе.
  Проверяет:
    1) Загрузку полетного задания и карты
    2) Исправность флэш памяти 1636PP52Y
    3) Исправность флэш памяти 25Q64FV
    4) Работу файловой системы черного ящика
    5) Связь с СНС
    6) Связь с СВС (?)
    7) Связь с БИМами (если они подключены)
				
  Примечание:
    Связь с БИМами проверяется только если они аппаратно подключены 
    (шпилькой и реле). Если же аппаратно они и не могут быть доступны для связи
    то, связь с ними не анализируется (то есть отсутствие связи с БИМ, при их
    аппаратном отключении не признаётся неисправностью).
       
  Возвращает: ST_OK    - Если система готова к полёту
              ST_FAULT - Если система не готова к полёту
************************************************************************************/
SelfTesting_STATUS_TYPE SelfTesting_OnlineDiagnostics (void);

	
/************************************************************************************
  SelfTesting_1636PP52Y - Тестирование 4х внешних SPI-flash от Миландра
				
  Возвращает: ST_OK    - Если память работает
              ST_FAULT - Если память недоступна
************************************************************************************/
SelfTesting_STATUS_TYPE SelfTesting_1636PP52Y(void);


/************************************************************************************
  SelfTesting_25Q64FV - Тестирование SPI-flash от WINBOND
				
  Возвращает: ST_OK    - Если память работает
              ST_FAULT - Если память недоступна
************************************************************************************/
SelfTesting_STATUS_TYPE SelfTesting_25Q64FV(void);


/************************************************************************************
  SelfTesting_MapNtask - Проверка наличия карты рельефа и полетного задания
				
  Возвращает: ST_OK    - Если задание и карта загружены
              ST_FAULT - Если карта и задание отсутствует или повреждены
************************************************************************************/
SelfTesting_STATUS_TYPE SelfTesting_MapNtask(void);


/************************************************************************************
  SelfTesting_LogFs - Проверка готовности файловой системы
				
  Возвращает: ST_OK    - Если файловая система исправна
              ST_FAULT - Если форматирование повреждено
************************************************************************************/
SelfTesting_STATUS_TYPE SelfTesting_LogFs(void);


/************************************************************************************
  SelfTesting_LEFT_BIM - Проверка исправности левого БИМа
				
  Возвращает: ST_OK    - Если БИМ исправен
              ST_FAULT - Если БИМ неисправен, либо связь отсутствует
************************************************************************************/
SelfTesting_STATUS_TYPE SelfTesting_LEFT_BIM(void);


/************************************************************************************
  SelfTesting_RIGHT_BIM - Проверка исправности правого БИМа
				
  Возвращает: ST_OK    - Если БИМ исправен
              ST_FAULT - Если БИМ неисправен, либо связь отсутствует
************************************************************************************/
SelfTesting_STATUS_TYPE SelfTesting_RIGHT_BIM(void);


/************************************************************************************
  SelfTesting_BIMS_TRY_CONNECT - Попытка восстановления связи с БИМами
				
  Примечание: Данная функция будет перезапускать питание БИМ в случае,
              если связь хотя бы с одним из них будет нарушена. Имеется ограничение
              на частоту переключения реле питания, для обеспечения этого в функции
              выполняется таймаут контроль. Во время таймаута никаких действий не 
              предпринимается.
************************************************************************************/
void SelfTesting_BIMS_TRY_CONNECT(void);


/************************************************************************************
  SelfTesting_PIN1 - Проверка состояния шпильки 1
				
  Возвращает: ST_OK    - Если шпилька 1 вставлена
              ST_FAULT - Если шпилька 1 отсутствует
************************************************************************************/
SelfTesting_STATUS_TYPE SelfTesting_PIN1(void);


/************************************************************************************
  SelfTesting_PIN2 - Проверка состояния шпильки 2
				
  Возвращает: ST_OK    - Если шпилька 2 вставлена
              ST_FAULT - Если шпилька 2 отсутствует
************************************************************************************/
SelfTesting_STATUS_TYPE SelfTesting_PIN2(void);


/************************************************************************************
  SelfTesting_PYRO - Проверка состояния реле пиропатрона
				
  Возвращает: ST_OK    - Если реле пиропатрона включено
              ST_FAULT - Если реле пиропатрона выключено
************************************************************************************/
SelfTesting_STATUS_TYPE SelfTesting_PYRO(void);


/************************************************************************************
  SelfTesting_TDS - Проверка состояния реле пирозамка посадочной системы
				
  Возвращает: ST_OK    - Если реле включено
              ST_FAULT - Если реле выключено
************************************************************************************/
SelfTesting_STATUS_TYPE SelfTesting_TDS(void);


/************************************************************************************
  SelfTesting_SNS - Проверка связи с СНС
				
  Возвращает: ST_OK    - Если связь с СНС есть
              ST_FAULT - Если связь с СНС отсутствует
************************************************************************************/
SelfTesting_STATUS_TYPE SelfTesting_SNS(void);


/************************************************************************************
  SelfTesting_SWS - Проверка связи с СВС
				
  Возвращает: ST_OK    - Если связь с СВС есть
              ST_FAULT - Если связь с СВС отсутствует
************************************************************************************/
SelfTesting_STATUS_TYPE SelfTesting_SWS(void);


/************************************************************************************
  SelfTesting_POW_BIM - Проверка состояния реле питания БИМ
				
  Возвращает: ST_OK    - Если реле питания БИМ включено
              ST_FAULT - Если реле питания БИМ выключено
************************************************************************************/
SelfTesting_STATUS_TYPE SelfTesting_POW_BIM (void);


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
SelfTesting_STATUS_TYPE SelfTesting_MapAvailability (double Lat, double Lon);


/************************************************************************************
  SelfTesting_Battery50Volt - Проверка напряжения на 50 вольтовой секции АКБ

  Параметры:
            NONE
								
  Возвращает: ST_OK - Если напряжение в норме
              ST_FAULT - Если напряжение ниже порогового
										
  Примечание: Функция помимо проверки напряжения, так же заполняет 
              соответсвующее поле в хранилище bup_data_store
************************************************************************************/
SelfTesting_STATUS_TYPE SelfTesting_Battery50Volt (void);


/************************************************************************************
  SelfTesting_Radiostation - Проверка связи с радиостанцией

  Параметры:
              NONE
								
  Возвращает: ST_OK - Если связь со станцией исправна
              ST_FAULT - Если неисправна
										
  Примечание:
************************************************************************************/
SelfTesting_STATUS_TYPE SelfTesting_Radiostation (void);
#endif
