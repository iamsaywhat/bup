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
  uint8_t optionsMath;     // Вариант модели регулятора: (1 - fightRegulatorCFB; 2 - Easy_reg)
  uint8_t majorMath;       // Старшая версия модели регулятора
  uint8_t minorMath;       // Младшая версия модели регулятора
}BupFirmwareVersion;


/*******************************************
  Тип структуры служебных данных БУП    
*******************************************/
typedef struct{
  double   touchdownLatitude;          // Широта точки приземления, градус
  double   touchdownLongitude;         // Долгота точки приземления, градус
  double   touchdownAltitude;          // Высота раскрытия посадочного парашюта, метры
  double   latitude;                   // Текущая широта, градус
  double   longitude;                  // Текущая долгота, градус
  double   altitude;                   // Текущая высота над уровнем моря по данным от СНС, метры
  double   headingTrue;                // Истинный курс, радианы
  double   headingMgn;                 // Магнитный курс, радианы
  double   velocityLatitude;           // Скорость по широте, м/c
  double   velocityLongitude;          // Скорость по долготе, м/c
  double   velocityAltitude;           // Скорость снижения, м/c
  double   pitch;                      // Тангаж, радианы
  double   roll;                       // Крен, радианы
  double   course;                     // Путевой курс, радианы
  short    reliefHeight;               // Высота рельефа в текущей точке, метры
  short    reliefOnTDP;                // Высота рельефа в точке приземления, метры
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


/***************************************************************************
  Функции доступа к данным БУП
***************************************************************************/
inline double   Bup_getTouchdownLatitude(void)  { return bupDataStorage.touchdownLatitude;}
inline double   Bup_getTouchdownLongitude(void) { return bupDataStorage.touchdownLongitude;}
inline double   Bup_getTouchdownAltitude(void)  { return bupDataStorage.touchdownAltitude;}
inline double   Bup_getLatitude(void)           { return bupDataStorage.latitude;}
inline double   Bup_getLongitude(void)          { return bupDataStorage.longitude;}
inline double   Bup_getAltitude(void)           { return bupDataStorage.altitude;}
inline double   Bup_getHeadingTrue(void)        { return bupDataStorage.headingTrue;}
inline double   Bup_getHeadingMgn(void)         { return bupDataStorage.headingMgn;}
inline double   Bup_getVelocityLatitude(void)   { return bupDataStorage.velocityLatitude;}
inline double   Bup_getVelocityLongitude(void)  { return bupDataStorage.velocityLongitude;}
inline double   Bup_getVelocityAltitude(void)   { return bupDataStorage.velocityAltitude;}
inline double   Bup_getPitch(void)              { return bupDataStorage.pitch;}
inline double   Bup_getRoll(void)               { return bupDataStorage.roll;}
inline double   Bup_getCourse(void)             { return bupDataStorage.course;}
inline short    Bup_getReliefHeight(void)       { return bupDataStorage.reliefHeight;}
inline short    Bup_getReliefHeightOnTDP(void)  { return bupDataStorage.reliefOnTDP;}
inline uint32_t Bup_getControlTime (void)       { return bupDataStorage.controlSecond;}

#endif
