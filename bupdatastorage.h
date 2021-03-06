/***************************************************************************
			
    bup_data_store - Модуль сбора и хранения служебной информации.
			
    Данный модуль позволяет осуществлять сбор информации от СНС, СВС,
    карты высот, необходимую конвертацию данных к удобному представлению.
			
    Предоставляет функции-интерфейсы для доступа ко всем необходимым 
    служебным данным.
			
    Содержит функции конвертации данных от СНС к удобному виду.
			
    Примечание: Модуль был реализован, из-за того, что нескольким
    модулям бывает нужна одна и та же системная информация. При 
    этом передача аргументом в большинстве случаев неудобна и может
    теоретически может повлечь к снижению производительности 
    (самым неудобным было запись в лог и трассировка параметров в CAN,
    из-за чего фукции записи/отправки приходилось размещать прям посреди
    полезного кода).
    Было решено организовать глобальное хранилище, доступное из
    любого модуля при подключении header-файла. 
		
***************************************************************************/
#ifndef _BUP_DATA_STORE_H_
#define _BUP_DATA_STORE_H_

#include <stdint.h>
#include "kmonshelf.h"
#include "sws.h"


/*******************************************
  Версия ПО БУП
    Firmware: 
      major.minor.micro
    Math:
      option.major.minor
*******************************************/
typedef struct{
  uint8_t majorFirmware;   // Старшая версия ПО
  uint8_t minorFirmware;   // Младшая версия ПО
  uint8_t microFirmware;   // Небольшие изменения
  uint8_t majorMath;       // Старшая версия модели регулятора
  uint8_t minorMath;       // Младшая версия модели регулятора
}BupFirmwareVersion;


/*******************************************
  Тип структуры служебных данных БУП    
*******************************************/
typedef struct{
  double   touchdownPointLatitude;     // Широта точки приземления, градус
  double   touchdownPointLongitude;    // Долгота точки приземления, градус
  double   touchdownPointAltitude;     // Высота раскрытия посадочного парашюта, метры
  short    touchdownPointRelief;       // Высота рельефа в точке приземления, метры
  double   radioPointLatitude;         // Широта точки приземления полученной от радиостанции, градус
  double   radioPointLongitude;        // Долгота точки приземления полученной от радиостанции, градус
  uint8_t  radioPointUpdateIndex;      // Индекс обновления точки полученные от радиостанции
  short    radioPointRelief;           // Высота рельефа в точке полученной от радиостанции, метры
  double   currentPointLatitude;       // Текущая широта, градус
  double   currentPointLongitude;      // Текущая долгота, градус
  double   currentPointAltitude;       // Текущая высота над уровнем моря по данным от СНС, метры
  short    currentPointRelief;         // Высота рельефа в текущей точке, метры
  double   currentHeadingTrue;         // Истинный курс, радианы
  double   currentHeadingMgn;          // Магнитный курс, радианы
  double   currentVelocityLatitude;    // Скорость по широте, м/c
  double   currentVelocityLongitude;   // Скорость по долготе, м/c
  double   currentVelocityAltitude;    // Скорость снижения, м/c
  double   currentPitch;               // Тангаж, радианы
  double   currentRoll;                // Крен, радианы
  double   currentCourse;              // Путевой курс, радианы
  float    battery50V;                 // Напряжение на секции 50 Вольт АКБ
  uint32_t controlSecond;              // Системное время БУП
}BupDataStorage;


/*******************************************
  Версия ПО БУП               
*******************************************/
extern const BupFirmwareVersion  bupFirmwareVersion;


/*******************************************
  Cлужебные данные БУП               
*******************************************/                        
extern BupDataStorage bupDataStorage;


/************************************************************************************************************** 
  Bup_initialize - Инициализация хранилища данных.
            
  Описание: Функция инициализации структуры BUP_DataStorage, производит установку 
  параметров по-умолчанию.
***************************************************************************************************************/
void Bup_initialize (void);


/**************************************************************************************************************
  Bup_updateData - Обновление данных хранилища.                 
						
  Описание: Функция сбора необходимых для работы БУП в боевом режиме данных и их
  конвертация к удобного виду. Использует функционал драйвера "SNS.h" для получения данных 
  местоположения и ориентации в пространстве, драйвера "SWS.h" для сбора полетных характеристик.
***************************************************************************************************************/
void Bup_updateData (void);


/**************************************************************************************************************
  Bup_updateDataFromSNS - Обновление данных СНС.                 
  
  Описание: Обновляет только структуру данных от СНС
***************************************************************************************************************/
void Bup_updateDataFromSNS (void);


/**************************************************************************************************************
  Bup_updateDataFromSWS - Обновление данных СВС.                 
  
  Описание: Обновляет только структуру данных от СВС
***************************************************************************************************************/
void Bup_updateDataFromSWS (void);


/**************************************************************************************************************
  Bup_updateRadiostationData - Обновление данных от радиостанции
  
  Описание: Получает данные от радиостанции, если получены координаты расчитывает так же высоту рельефа
***************************************************************************************************************/
void Bup_updateRadiostationData (void);


/**************************************************************************************************************
  Bup_loadTouchdownPoint - Загрузка точки приземления из flash полетного задания в ОЗУ
  
  Описание: Подгружает точку приземления из flash полетного задания, а так же высчисляет высоту рельефа
***************************************************************************************************************/
void Bup_loadTouchdownPoint (void);


/***************************************************************************
  Функции доступа к данным БУП
***************************************************************************/
inline double   Bup_getTouchdownPointLatitude(void);
inline double   Bup_getTouchdownPointLongitude(void);
inline double   Bup_getTouchdownPointAltitude(void);
inline short    Bup_getTouchdownPointRelief(void);
inline double   Bup_getCurrentPointLatitude(void);
inline double   Bup_getCurrentPointLongitude(void);
inline double   Bup_getCurrentPointAltitude(void);
inline short    Bup_getCurrentPointRelief(void);
inline double   Bup_getRadioPointLatitude(void);
inline double   Bup_getRadioPointLongitude(void);
inline uint8_t  Bup_getRadioPointUpdateIndex(void);
inline short    Bup_getRadioPointRelief(void);
inline double   Bup_getCurrentHeadingTrue(void);
inline double   Bup_getCurrentHeadingMgn(void);
inline double   Bup_getCurrentVelocityLatitude(void);
inline double   Bup_getCurrentVelocityLongitude(void);
inline double   Bup_getCurrentVelocityAltitude(void);
inline double   Bup_getCurrentPitch(void);
inline double   Bup_getCurrentRoll(void);
inline double   Bup_getCurrentCourse(void);
inline float    Bup_getBatteryVoltage(void);     
inline uint32_t Bup_getControlTime (void);


#endif
