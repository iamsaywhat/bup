#include "loger.h"

#include "config.h"
#include "logfs/log.fs.h"
#include "retarget.printf/RetargetPrintf.h"
#include "selftesting.h"
#include "bupdatastorage.h"
#include "kmonshelf.h"
#include "bims.h"


#ifdef flightRegulatorCFB //******************************************************* Если выбран flightRegulatorCFB
  #include "math.model/flightRegulatorCFB/flightRegulatorCFB.h"
#else //*************************************************************************** Если выбран flightController
  #include "math.model/flightController/flightController.h"
#endif //************************************************************************** !flightRegulatorCFB 



/************************************************************************************
  loger_initmsg - Запись в лог первичной информации
************************************************************************************/
void loger_initmsg (void)
{
  // Переключаем вывод в ЛОГ
  printf_switcher(TO_LOG, 0);
  // В начало файла кладём его порядковый номер
  printf("***File # %d***\n", LogFs_getCurrentFileId());
	
  // Версия ПО, включая версию регулятора
  printf("Firmware Version:");
  printf("%u.",   bupFirmwareVersion.majorFirmware);
  printf("%u.",   bupFirmwareVersion.minorFirmware);
  printf("%u ",   bupFirmwareVersion.microFirmware);
  printf("(%u.",  bupFirmwareVersion.optionsMath);
  printf("%u.",   bupFirmwareVersion.majorMath);
  printf("%u)\n", bupFirmwareVersion.minorMath);

  // Выведем загруженное полетное задание
  printf("TD_Lat: %f\n", Bup_getTouchdownLatitude());
  printf("TD_Lon: %f\n", Bup_getTouchdownLongitude());
  printf("TD_Alt: %f\n", Bup_getTouchdownAltitude());
}
/************************************************************************************
    loger_periodprint - Функция, определяющая формат записи в лог сообщений
    на каждый шаг алгоритма.
************************************************************************************/
void loger_periodprint (void)
{
  printf("\nTimestamp, sec: %d\n",       bupDataStorage.controlSecond); // Метку времени в ЛОГ
  printf("SNS_Lat: %llu\n",              SNS_position.Struct.Pos_lat);
  printf("SNS_Lon: %llu\n",              SNS_position.Struct.Pos_lon);
  printf("SNS_Alt: %llu\n",              SNS_position.Struct.Pos_alt);
  printf("SNS_Vel_lat: %d\n",            SNS_position.Struct.Vel_lat);
  printf("SNS_Vel_lon: %d\n",            SNS_position.Struct.Vel_lon);
  printf("SNS_Vel_alt: %d\n",            SNS_position.Struct.Vel_alt);
  printf("SNS_Course: %d\n",             SNS_position.Struct.Course);
  printf("SNS_Heading_true: %d\n",       SNS_orientation.Struct.Heading_true);
  printf("SNS_Heading_mgn: %d\n",        SNS_orientation.Struct.Heading_mgn);
  printf("SNS_Pitch: %d\n",              SNS_orientation.Struct.Pitch);
  printf("SNS_Roll: %d\n",               SNS_orientation.Struct.Roll);
  printf("SWS_TrueSpeed: %f\n",          SWS_getTrueSpeed());
  printf("SWS_InstrumentSpeed: %f\n",    SWS_getInstrumentSpeed());
  printf("SWS_AbsoluteHeight: %f\n",     SWS_getAbsoluteHeight());
  printf("BIML_Pos: %d\n",               (uint8_t)(0.5 + 0.3922*BIM_getStrapPosition(LEFT_BIM)));   // Перевод к процентной шкале с округлением
  printf("BIMR_Pos: %d\n",               (uint8_t)(0.5 + 0.3922*BIM_getStrapPosition(RIGHT_BIM)));  // Перевод к процентной шкале с округлением
  printf("SystemState: %x\n",            systemState);
  printf("Model_Lat, deg: %f\n",         Bup_getLatitude());
  printf("Model_Lon, deg: %f\n",         Bup_getLongitude());
  printf("Model_Alt, m: %f\n",           Bup_getAltitude());
  printf("Model_VelocityLat, m/s: %f\n", Bup_getVelocityLatitude());
  printf("Model_VelocityLon, m/s: %f\n", Bup_getVelocityLongitude());
  printf("Model_VelocityAlt, m/s: %f\n", Bup_getVelocityAltitude());
  printf("Model_HeadingTrue, rad: %f\n", Bup_getHeadingTrue());
  printf("Model_HeadingMgn, rad: %f\n",  Bup_getHeadingMgn());
  printf("Model_Course, rad: %f\n",      Bup_getCourse());
  printf("Model_Pitch, rad: %f\n",       Bup_getPitch());
  printf("Model_Roll, rad: %f\n",        Bup_getRoll());	
	
  // Если карта рельефа в текущей позиции доступна, запишем высоту рельефа
  if (SelfTesting_STATUS(ST_MapAvailability))
    printf("MAP, m: %d\n",               Bup_getReliefHeight());
  else
    printf("MAP, m: NOT_AVAILABLE\n");
	
  #ifdef flightRegulatorCFB	//******************************************************* Если выбран flightRegulatorCFB
    printf("Model_BIM_CMD: %f\n", (double)(rtY.tightenSling*rtY.directionOfRotation));
    printf("Model_TD_CMD: %d\n",  (uint8_t)rtY.cmdTouchDown);
  #else //*************************************************************************** Если выбран flightController
    printf("Model_BIM_CMD: %f\n", (double)rtY.BIM_CMD);
    printf("Model_TD_CMD: %d\n",  (uint8_t)rtY.TD_CMD);
  #endif //************************************************************************** !flightRegulatorCFB
}


/************************************************************************************
  loger_exitmsg - Запись в лог информации о завершении работы БУП
************************************************************************************/
void loger_exitmsg(void)
{
  printf("\nThe flight is over!\n");
  printf("Fin_Lat, deg: %f\n",         Bup_getLatitude());
  printf("Fin_Lon, deg: %f\n",         Bup_getLongitude());
  printf("Fin_Alt, m: %f\n",           Bup_getAltitude());
  printf("Fin_SWSHeight: %f\n",        SWS_getAbsoluteHeight());
  // Если карта рельефа в текущей позиции доступна, запишем высоту рельефа
  if (SelfTesting_STATUS(ST_MapAvailability))
    printf("Fin_MAP, m: %d\n",         Bup_getReliefHeight());
  else
    printf("Fin_MAP, m: NOT_AVAILABLE\n");
}
