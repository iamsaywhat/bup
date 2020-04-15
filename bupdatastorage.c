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
                                                31,        // Младшая версия ПО
                                                3,         // Изменения внутри версии
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
double   Bup_getTouchdownLatitude(void)  { return bupDataStorage.touchdownLatitude;}
double   Bup_getTouchdownLongitude(void) { return bupDataStorage.touchdownLongitude;}
double   Bup_getTouchdownAltitude(void)  { return bupDataStorage.touchdownAltitude;}
double   Bup_getLatitude(void)           { return bupDataStorage.latitude;}
double   Bup_getLongitude(void)          { return bupDataStorage.longitude;}
double   Bup_getAltitude(void)           { return bupDataStorage.altitude;}
double   Bup_getHeadingTrue(void)        { return bupDataStorage.headingTrue;}
double   Bup_getHeadingMgn(void)         { return bupDataStorage.headingMgn;}
double   Bup_getVelocityLatitude(void)   { return bupDataStorage.velocityLatitude;}
double   Bup_getVelocityLongitude(void)  { return bupDataStorage.velocityLongitude;}
double   Bup_getVelocityAltitude(void)   { return bupDataStorage.velocityAltitude;}
double   Bup_getPitch(void)              { return bupDataStorage.pitch;}
double   Bup_getRoll(void)               { return bupDataStorage.roll;}
double   Bup_getCourse(void)             { return bupDataStorage.course;}
short    Bup_getReliefHeight(void)       { return bupDataStorage.reliefHeight;}
short    Bup_getReliefHeightOnTDP(void)  { return bupDataStorage.reliefOnTDP;}
float    Bup_getBatteryVoltage(void)     { return bupDataStorage.battery50V; }
uint32_t Bup_getControlTime (void)       { return bupDataStorage.controlSecond;}
double   Bup_getRadioLatitude(void)      { return bupDataStorage.radioLatitude; }
double   Bup_getRadioLongitude(void)     { return bupDataStorage.radioLongitude; }
uint8_t  Bup_getRadioUpdateIndex(void)   { return bupDataStorage.radioUpdateIndex; }


/**************************************************************************************************************
  Bup_initialize - Инициализация хранилища данных 
***************************************************************************************************************/
void Bup_initialize (void)
{
  bupDataStorage.touchdownLatitude  = getTouchdownLatitude();  // Подгружаем из памяти
  bupDataStorage.touchdownLongitude = getTouchdownLongitude(); // Подгружаем из памяти
  bupDataStorage.touchdownAltitude  = getTouchdownAltitude();  // Подгружаем из памяти
  bupDataStorage.latitude = 0;
  bupDataStorage.longitude = 0;
  bupDataStorage.altitude = 0;
  bupDataStorage.headingTrue = 0;
  bupDataStorage.headingMgn = 0;
  bupDataStorage.velocityLatitude = 0;
  bupDataStorage.velocityLongitude = 0;
  bupDataStorage.velocityAltitude = 0;
  bupDataStorage.pitch = 0;
  bupDataStorage.roll = 0;
  bupDataStorage.course = 0;
  bupDataStorage.reliefHeight = 0x7FFF;
  bupDataStorage.controlSecond = 0;
  bupDataStorage.radioLatitude = 0;
  bupDataStorage.radioLongitude = 0;
  bupDataStorage.radioUpdateIndex = 0;
	
  // Определим высоту рельефа в точке приземления
  bupDataStorage.reliefOnTDP = getHeightOnThisPoint(bupDataStorage.touchdownLongitude, 
                                                     bupDataStorage.touchdownLatitude,
                                                     TRIANGULARTION);
  if(bupDataStorage.reliefOnTDP == MAP_NO_SOLUTION)
    bupDataStorage.reliefOnTDP = 0;
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
  
  // Далее конвертируем полученные данные и будем хранить их уже в таком виде
  bupDataStorage.latitude           = SNS_getLatitude();
  bupDataStorage.longitude          = SNS_getLongitude();
  bupDataStorage.altitude           = SNS_getAltitude();
  bupDataStorage.headingTrue        = SNS_getTrueCourse();
  bupDataStorage.headingMgn         = SNS_getMagnetCourse();
  bupDataStorage.velocityLatitude   = SNS_getLatitudeVelocity();
  bupDataStorage.velocityLongitude  = SNS_getLongitudeVelocity(); 
  bupDataStorage.velocityAltitude   = SNS_getAltitudeVelocity(); 
  bupDataStorage.pitch              = SNS_getPitch();
  bupDataStorage.roll               = SNS_getRoll();
  bupDataStorage.course             = SNS_getGroundTrack();
  
  // Спрашиваем радиостанцию, и если есть новые данные, то поднимаем флаг, что они обновились
  if(Radiostation.autoChecker(&bupDataStorage.radioLatitude, 
                              &bupDataStorage.radioLongitude) == RADIO_GOT_NEW_COORDINATES)
    bupDataStorage.radioUpdateIndex++;
  
  // Рассчитываем по полученным данным высоту рельефа в точке
  bupDataStorage.reliefHeight       = getHeightOnThisPoint(bupDataStorage.longitude, 
                                                           bupDataStorage.latitude, 
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

