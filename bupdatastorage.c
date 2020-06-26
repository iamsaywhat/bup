#include "bupdatastorage.h"
#include "heightmap/heightmap.platformdepend.h"
#include "heightmap/heightmap.h"
#include "selftesting.h"
#include "config.h"
#include "radiostation.h"


/*******************************************************************************************************************
  Версия ПО БУП               
*******************************************************************************************************************/
const BupFirmwareVersion  bupFirmwareVersion = {0,         // Старшая версия ПО
                                                32,        // Младшая версия ПО
                                                2,         // Изменения внутри версии
#ifdef flightRegulatorCFB // Если выбран flightRegulatorCFB
                                                1,         // Oпция регулятора - flightRegulatorCFB
                                                0,         // Старшая версия модели регулятора
                                                1};        // Младшая версия модели регулятора
#else //******************** Если выбран flightController
                                                2,         // Опция регуляторв - flightController
                                                1,         // Старшая версия модели регулятора
                                                0};        // Младшая версия модели регулятора
#endif //******************* !flightRegulatorCFB 



/*******************************************
  Cлужебные данные БУП               
*******************************************/      
BupDataStorage bupDataStorage;


/***************************************************************************
  Функции доступа к данным БУП
***************************************************************************/
double   Bup_getTouchdownPointLatitude(void)  { return bupDataStorage.touchdownPointLatitude;}
double   Bup_getTouchdownPointLongitude(void) { return bupDataStorage.touchdownPointLongitude;}
double   Bup_getTouchdownPointAltitude(void)  { return bupDataStorage.touchdownPointAltitude;}
short    Bup_getTouchdownPointRelief(void)    { return bupDataStorage.touchdownPointRelief;}
double   Bup_getRadioPointLatitude(void)      { return bupDataStorage.radioPointLatitude; }
double   Bup_getRadioPointLongitude(void)     { return bupDataStorage.radioPointLongitude; }
uint8_t  Bup_getRadioPointUpdateIndex(void)   { return bupDataStorage.radioPointUpdateIndex; }
short    Bup_getRadioPointRelief(void)        { return bupDataStorage.radioPointRelief; }
double   Bup_getCurrentPointLatitude(void)    { return bupDataStorage.currentPointLatitude;}
double   Bup_getCurrentPointLongitude(void)   { return bupDataStorage.currentPointLongitude;}
double   Bup_getCurrentPointAltitude(void)    { return bupDataStorage.currentPointAltitude;}
short    Bup_getCurrentPointRelief(void)      { return bupDataStorage.currentPointRelief;}
double   Bup_getCurrentHeadingTrue(void)      { return bupDataStorage.currentHeadingTrue;}
double   Bup_getCurrentHeadingMgn(void)       { return bupDataStorage.currentHeadingMgn;}
double   Bup_getCurrentVelocityLatitude(void) { return bupDataStorage.currentVelocityLatitude;}
double   Bup_getCurrentVelocityLongitude(void){ return bupDataStorage.currentVelocityLongitude;}
double   Bup_getCurrentVelocityAltitude(void) { return bupDataStorage.currentVelocityAltitude;}
double   Bup_getCurrentPitch(void)            { return bupDataStorage.currentPitch;}
double   Bup_getCurrentRoll(void)             { return bupDataStorage.currentRoll;}
double   Bup_getCurrentCourse(void)           { return bupDataStorage.currentCourse;}
float    Bup_getBatteryVoltage(void)          { return bupDataStorage.battery50V; }
uint32_t Bup_getControlTime (void)            { return bupDataStorage.controlSecond;}



/**************************************************************************************************************
  Bup_initialize - Инициализация хранилища данных 
***************************************************************************************************************/
void Bup_initialize (void)
{
  /* Данные о запланированной точке приземления */
  bupDataStorage.touchdownPointLatitude  = getTouchdownLatitude();  // Подгружаем из памяти
  bupDataStorage.touchdownPointLongitude = getTouchdownLongitude(); // Подгружаем из памяти
  bupDataStorage.touchdownPointAltitude  = getTouchdownAltitude();  // Подгружаем из памяти
  bupDataStorage.touchdownPointRelief = getHeightOnThisPoint(bupDataStorage.touchdownPointLongitude, 
                                                             bupDataStorage.touchdownPointLatitude,
                                                             TRIANGULARTION);
  if(bupDataStorage.touchdownPointRelief == MAP_NO_SOLUTION)
    bupDataStorage.touchdownPointRelief = 0;
  /* Данные точки, получаемой с радиостанции */
  bupDataStorage.radioPointLatitude = 0;
  bupDataStorage.radioPointLongitude = 0;
  bupDataStorage.radioPointUpdateIndex = 0;
  bupDataStorage.radioPointRelief = 0;
  /* Текущие данные местоположения */
  bupDataStorage.currentPointLatitude = 0;
  bupDataStorage.currentPointLongitude = 0;
  bupDataStorage.currentPointAltitude = 0;
  bupDataStorage.currentPointRelief = 0x7FFF;
  /* Текущие данные с датчиков */
  bupDataStorage.currentHeadingTrue = 0;
  bupDataStorage.currentHeadingMgn = 0;
  bupDataStorage.currentVelocityLatitude = 0;
  bupDataStorage.currentVelocityLongitude = 0;
  bupDataStorage.currentVelocityAltitude = 0;
  bupDataStorage.currentPitch = 0;
  bupDataStorage.currentRoll = 0;
  bupDataStorage.currentCourse = 0;
  /* Прочие системные данные */
  bupDataStorage.controlSecond = 0;
  bupDataStorage.battery50V = 0;
}


/**************************************************************************************************************
  Bup_updateData - Обновление данных хранилища
***************************************************************************************************************/
void Bup_updateData (void)
{
  // Просим данные у СНС
  Bup_updateDataFromSNS ();
  // Просим данные у СВС
  Bup_updateDataFromSWS ();
  // Обновить данные радиостанции
  Bup_updateRadiostationData ();
  
  // Далее конвертируем полученные данные и будем хранить их уже в таком виде
  bupDataStorage.currentPointLatitude      = SNS_getLatitude();
  bupDataStorage.currentPointLongitude     = SNS_getLongitude();
  bupDataStorage.currentPointAltitude      = SNS_getAltitude();
  bupDataStorage.currentHeadingTrue        = SNS_getTrueCourse();
  bupDataStorage.currentHeadingMgn         = SNS_getMagnetCourse();
  bupDataStorage.currentVelocityLatitude   = SNS_getLatitudeVelocity();
  bupDataStorage.currentVelocityLongitude  = SNS_getLongitudeVelocity(); 
  bupDataStorage.currentVelocityAltitude   = SNS_getAltitudeVelocity(); 
  bupDataStorage.currentPitch              = SNS_getPitch();
  bupDataStorage.currentRoll               = SNS_getRoll();
  bupDataStorage.currentCourse             = SNS_getGroundTrack();
  // Рассчитываем по полученным данным высоту рельефа в точке
  bupDataStorage.currentPointRelief = getHeightOnThisPoint(bupDataStorage.currentPointLongitude, 
                                                           bupDataStorage.currentPointLatitude, 
                                                           TRIANGULARTION);
}

/**************************************************************************************************************
  Bup_updateDataFromSWS - Обновление данных СВС.                 
***************************************************************************************************************/
void Bup_updateDataFromSWS (void)
{	
  /* Пытаемся получить данные от СВС, 
    в случае неудача выставляем флаг неисправности */
  if(SWS_update() != SWS_OK)
    SelfTesting_SET_FAULT(ST_sws);	
}


/**************************************************************************************************************
  Bup_updateDataFromSNS - Обновление данных СНС.                 
***************************************************************************************************************/
void Bup_updateDataFromSNS (void)
{
  SNS_Status statusPosition;
  SNS_Status statusOrientation;

  /* Запрашиваем данные местоположения */
  statusPosition = SNS_updatePositionData();	
	
  /* Запрашиваем данные ориентации */
  statusOrientation = SNS_updateOrientationData();
	
  /* Проверяем статусы ответов */
  if(statusPosition != SNS_OK || statusOrientation != SNS_OK)
    SelfTesting_SET_FAULT(ST_sns);	
}

/**************************************************************************************************************
  Bup_updateRadiostationData - Обновление данных от радиостанции                
***************************************************************************************************************/
void Bup_updateRadiostationData (void)
{
  // Спрашиваем радиостанцию, и если есть новые данные, то поднимаем флаг, что они обновились
  if(Radiostation.autoChecker(&bupDataStorage.radioPointLatitude, 
                              &bupDataStorage.radioPointLongitude) == RADIO_GOT_NEW_COORDINATES)
  {
    bupDataStorage.radioPointUpdateIndex++;
    bupDataStorage.radioPointRelief = getHeightOnThisPoint(bupDataStorage.radioPointLongitude, 
                                                           bupDataStorage.radioPointLatitude, 
                                                           TRIANGULARTION);
    if(bupDataStorage.radioPointRelief == MAP_NO_SOLUTION)
      bupDataStorage.radioPointRelief = 0;
  }
}
