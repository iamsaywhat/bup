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
#include "SWS.h"


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
  double   TouchdownLatitude;          // Широта точки приземления, градус
  double   TouchdownLongitude;         // Долгота точки приземления, градус
  double   TouchdownAltitude;          // Высота раскрытия посадочного парашюта, метры
  double   Latitude;                   // Текущая широта, градус
  double   Longitude;                  // Текущая долгота, градус
  double   Altitude;                   // Текущая высота над уровнем моря по данным от СНС, метры
  double   HeadingTrue;                // Истинный курс, радианы
  double   HeadingMgn;                 // Магнитный курс, радианы
  double   VelocityLatitude;           // Скорость по широте, м/c
  double   VelocityLongitude;          // Скорость по долготе, м/c
  double   VelocityAltitude;           // Скорость снижения, м/c
  double   Pitch;                      // Тангаж, радианы
  double   Roll;                       // Крен, радианы
  double   Course;                     // Путевой курс, радианы
  short    ReliefHeight;               // Высота рельефа в текущей точке, метры
  short    ReliefOnTDP;                // Высота рельефа в точке приземления, метры
  float    Battery50V;                 // Напряжение на секции 50 Вольт АКБ
  uint32_t ControlSecond;              // Системное время БУП
}BUP_DATA;


/*******************************************
  Версия ПО БУП               
*******************************************/
extern const BupFirmwareVersion  bupFirmwareVersion;


/*******************************************
  Cлужебные данные БУП               
*******************************************/                        
extern BUP_DATA BUP_DataStorage;


/************************************************************************************************************** 
  BUP_DataInit - Инициализация хранилища данных.
            
  Описание: Функция инициализации структуры BUP_DataStorage, производит установку 
  параметров по-умолчанию.
***************************************************************************************************************/
void BUP_DataInit (void);


/**************************************************************************************************************
  BUP_DataUpdate - Обновление данных хранилища.                 
						
  Описание: Функция сбора необходимых для работы БУП в боевом режиме данных и их
  конвертация к удобного виду. Использует функционал драйвера "SNS.h" для получения данных 
  местоположения и ориентации в пространстве, драйвера "SWS.h" для сбора полетных характеристик.
***************************************************************************************************************/
void BUP_DataUpdate (void);


/**************************************************************************************************************
  BUP_UpdateDataFromSNS - Обновление данных СНС.                 
  
  Описание: Обновляет только структуру данных от СНС
***************************************************************************************************************/
void BUP_UpdateDataFromSNS (void);


/**************************************************************************************************************
  BUP_UpdateDataFromSWS - Обновление данных СВС.                 
  
  Описание: Обновляет только структуру данных от СВС
***************************************************************************************************************/
void BUP_UpdateDataFromSWS (void);


/***************************************************************************
  Функции доступа к данным БУП
***************************************************************************/
inline double   BUP_Get_TouchdownLatitude(void)  { return BUP_DataStorage.TouchdownLatitude;}
inline double   BUP_Get_TouchdownLongitude(void) { return BUP_DataStorage.TouchdownLongitude;}
inline double   BUP_Get_TouchdownAltitude(void)  { return BUP_DataStorage.TouchdownAltitude;}
inline double   BUP_Get_Latitude(void)           { return BUP_DataStorage.Latitude;}
inline double   BUP_Get_Longitude(void)          { return BUP_DataStorage.Longitude;}
inline double   BUP_Get_Altitude(void)           { return BUP_DataStorage.Altitude;}
inline double   BUP_Get_HeadingTrue(void)        { return BUP_DataStorage.HeadingTrue;}
inline double   BUP_Get_HeadingMgn(void)         { return BUP_DataStorage.HeadingMgn;}
inline double   BUP_Get_VelocityLatitude(void)   { return BUP_DataStorage.VelocityLatitude;}
inline double   BUP_Get_VelocityLongitude(void)  { return BUP_DataStorage.VelocityLongitude;}
inline double   BUP_Get_VelocityAltitude(void)   { return BUP_DataStorage.VelocityAltitude;}
inline double   BUP_Get_Pitch(void)              { return BUP_DataStorage.Pitch;}
inline double   BUP_Get_Roll(void)               { return BUP_DataStorage.Roll;}
inline double   BUP_Get_Course(void)             { return BUP_DataStorage.Course;}
inline short    BUP_Get_ReliefHeight(void)       { return BUP_DataStorage.ReliefHeight;}
inline short    BUP_Get_ReliefHeightOnTDP(void)  { return BUP_DataStorage.ReliefOnTDP;}
inline uint32_t BUP_Get_SystemTime (void)        { return BUP_DataStorage.ControlSecond;}

#endif
