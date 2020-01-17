#include "bup_data_store.h"
#include "HeightMap/Heightmap_conf_and_pd.h"
#include "HeightMap/Heightmap.h"
#include "SelfTesting.h"
#include "config.h"


/*******************************************************************************************************************
  Версия ПО БУП               
*******************************************************************************************************************/
const BupFirmwareVersion  bupFirmwareVersion = {0,         // Старшая версия ПО
                                                30,        // Младшая версия ПО
                                                5,         // Изменения внутри версии
#ifdef flightRegulatorCFB // Если выбран flightRegulatorCFB
                                                1,         // Oпция регулятора - flightRegulatorCFB
                                                0,         // Старшая версия модели регулятора
                                                1};        // Младшая версия модели регулятора
#else //******************** Если выбран Easy_reg
                                                2,         // Опция регуляторв - Easy_reg
                                                0,         // Старшая версия модели регулятора
                                                1};        // Младшая версия модели регулятора
#endif //******************* !flightRegulatorCFB 



/*******************************************
  Cлужебные данные БУП               
*******************************************/      
BUP_DATA BUP_DataStorage;



/**************************************************************************************************************
  BUP_DataInit - Инициализация хранилища данных 
***************************************************************************************************************/
void BUP_DataInit (void)
{
  BUP_DataStorage.TouchdownLatitude  = GetTouchDownPointLat();  // Подгружаем из памяти
  BUP_DataStorage.TouchdownLongitude = GetTouchDownPointLon();  // Подгружаем из памяти
  BUP_DataStorage.TouchdownAltitude  = GetTouchDownPointAlt();  // Подгружаем из памяти
  BUP_DataStorage.Latitude = 0;
  BUP_DataStorage.Longitude = 0;
  BUP_DataStorage.Altitude = 0;
  BUP_DataStorage.HeadingTrue = 0;
  BUP_DataStorage.HeadingMgn = 0;
  BUP_DataStorage.VelocityLatitude = 0;
  BUP_DataStorage.VelocityLongitude = 0;
  BUP_DataStorage.VelocityAltitude = 0;
  BUP_DataStorage.Pitch = 0;
  BUP_DataStorage.Roll = 0;
  BUP_DataStorage.Course = 0;
  BUP_DataStorage.ReliefHeight = 0x7FFF;
  BUP_DataStorage.ControlSecond = 0;
	
  // Определим высоту рельефа в точке приземления
  BUP_DataStorage.ReliefOnTDP = GetHeight_OnThisPoint(BUP_DataStorage.TouchdownLongitude, 
                                                      BUP_DataStorage.TouchdownLatitude,
                                                      TRIANGULARTION);
  if(BUP_DataStorage.ReliefOnTDP == MAP_NO_SOLUTION)
    BUP_DataStorage.ReliefOnTDP = 0;
}


/**************************************************************************************************************
  BUP_DataUpdate - Обновление данных хранилища
***************************************************************************************************************/
void BUP_DataUpdate (void)
{
  // Просим данные у СНС
  BUP_UpdateDataFromSNS ();
  // Просим данные у СВС
  BUP_UpdateDataFromSWS ();
	
  // Далее конвертируем полученные данные и будем хранить их уже в таком виде
  BUP_DataStorage.Latitude           = SNS_getLatitude();
  BUP_DataStorage.Longitude          = SNS_getLongitude();
  BUP_DataStorage.Altitude           = SNS_getAltitude();
  BUP_DataStorage.HeadingTrue        = SNS_getTrueCourse();
  BUP_DataStorage.HeadingMgn         = SNS_getMagnetCourse();
  BUP_DataStorage.VelocityLatitude   = SNS_getLatitudeVelocity();
  BUP_DataStorage.VelocityLongitude  = SNS_getLongitudeVelocity(); 
  BUP_DataStorage.VelocityAltitude   = SNS_getAltitudeVelocity(); 
  BUP_DataStorage.Pitch              = SNS_getPitch();
  BUP_DataStorage.Roll               = SNS_getRoll();
  BUP_DataStorage.Course             = SNS_getGroundTrack();
	
  // Рассчитываем по полученным данным высоту рельефа в точке
  BUP_DataStorage.ReliefHeight       = GetHeight_OnThisPoint(BUP_DataStorage.Longitude, 
                                                             BUP_DataStorage.Latitude, 
                                                             TRIANGULARTION); 
}

/**************************************************************************************************************
  BUP_UpdateDataFromSWS - Обновление данных СВС.                 
***************************************************************************************************************/
void BUP_UpdateDataFromSWS (void)
{	
  /* Пытаемся получить данные от СВС, 
    в случае неудача выставляем флаг неисправности */
  if(SWS_update() != SWS_OK)
    SelfTesting_SET_FAULT(ST_sws);	
}


/**************************************************************************************************************
  BUP_UpdateDataFromSNS - Обновление данных СНС.                 
***************************************************************************************************************/
void BUP_UpdateDataFromSNS (void)
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
