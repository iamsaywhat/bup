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
#else //******************** Если выбран Easy_reg
                                                2,         // Опция регуляторв - Easy_reg
                                                0,         // Старшая версия модели регулятора
                                                2};        // Младшая версия модели регулятора
#endif //******************* !flightRegulatorCFB 



/*******************************************
  Cлужебные данные БУП               
*******************************************/      
BupDataStorage bupDataStorage;



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
  bupDataStorage.radioUpdated = 0;
	
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
    bupDataStorage.radioUpdated = 1;
  
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

