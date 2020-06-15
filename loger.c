#include "loger.h"

#include "otherlib.h"
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





void logger_openNewSession(void)
{
  LogFs_initialize();
  LogFs_createFile();
  printf_switcher(TO_LOG, 0);                              // Переключаем вывод в ЛОГ
  printf("Session#%d\n", LogFs_getCurrentFileId());        // В начало файла кладём его порядковый номер
  printf("Firmware Version:");                             // Версия ПО, включая версию регулятора
  printf("%u.",   bupFirmwareVersion.majorFirmware);
  printf("%u.",   bupFirmwareVersion.minorFirmware);
  printf("%u ",   bupFirmwareVersion.microFirmware);
  printf("(%u.",  bupFirmwareVersion.optionsMath);
  printf("%u.",   bupFirmwareVersion.majorMath);
  printf("%u)\n", bupFirmwareVersion.minorMath);
}
void logger_warning(char* string)
{
  printf("{\"time\":%llu,\"warning\":\"%s\"}\n", getCurrentSystemTime(), string);
}
void logger_error(char* string)
{
  printf("{\"time\":%llu,\"error\":\"%s\"}\n", getCurrentSystemTime(), string);
}
void logger_point(char* name, double latitude, double longitute, double altitude)
{
  printf("{\"time\":%llu,\"point\":[\"%s\",%f,%f,%.2f]}\n", getCurrentSystemTime(), name, latitude, longitute, altitude);
}
void logger_series(char* name, double value)
{
  printf("{\"time\":%llu,\"series\":[\"%s\",%d,%f]}\n", getCurrentSystemTime(), name, Bup_getControlTime(), value);
}
void logger_track(double latitude, double longitude, double altitude)
{
  printf("{\"time\":%llu,\"track\":[%d,%f,%f,%.2f]}\n", getCurrentSystemTime(), Bup_getControlTime(), latitude, longitude, altitude);
}









///************************************************************************************
//  loger_initmsg - Запись в лог первичной информации
//************************************************************************************/
//void loger_initmsg (void)
//{
//  // Переключаем вывод в ЛОГ
//  printf_switcher(TO_LOG, 0);
//  // В начало файла кладём его порядковый номер
//  printf("***File # %d***\n", LogFs_getCurrentFileId());
//	
//  // Версия ПО, включая версию регулятора
//  printf("Firmware Version:");
//  printf("%u.",   bupFirmwareVersion.majorFirmware);
//  printf("%u.",   bupFirmwareVersion.minorFirmware);
//  printf("%u ",   bupFirmwareVersion.microFirmware);
//  printf("(%u.",  bupFirmwareVersion.optionsMath);
//  printf("%u.",   bupFirmwareVersion.majorMath);
//  printf("%u)\n", bupFirmwareVersion.minorMath);

//  // Выведем загруженное полетное задание
//  printf("TD_Lat: %f\n", Bup_getTouchdownPointLatitude());
//  printf("TD_Lon: %f\n", Bup_getTouchdownPointLongitude());
//  printf("TD_Alt: %f\n", Bup_getTouchdownPointAltitude());
//}
///************************************************************************************
//    loger_periodprint - Функция, определяющая формат записи в лог сообщений
//    на каждый шаг алгоритма.
//************************************************************************************/
//void loger_periodprint (void)
//{
//  printf("\nTimestamp, sec: %d\n",       Bup_getControlTime()); // Метку времени в ЛОГ
//  printf("SNS_Lat: %llu\n",              SNS_position.Struct.Pos_lat);
//  printf("SNS_Lon: %llu\n",              SNS_position.Struct.Pos_lon);
//  printf("SNS_Alt: %llu\n",              SNS_position.Struct.Pos_alt);
//  printf("SNS_Vel_lat: %d\n",            SNS_position.Struct.Vel_lat);
//  printf("SNS_Vel_lon: %d\n",            SNS_position.Struct.Vel_lon);
//  printf("SNS_Vel_alt: %d\n",            SNS_position.Struct.Vel_alt);
//  printf("SNS_Course: %d\n",             SNS_position.Struct.Course);
//  printf("SNS_Heading_true: %d\n",       SNS_orientation.Struct.Heading_true);
//  printf("SNS_Heading_mgn: %d\n",        SNS_orientation.Struct.Heading_mgn);
//  printf("SNS_Pitch: %d\n",              SNS_orientation.Struct.Pitch);
//  printf("SNS_Roll: %d\n",               SNS_orientation.Struct.Roll);
//  printf("SWS_TrueSpeed: %f\n",          SWS_getTrueSpeed());
//  printf("SWS_InstrumentSpeed: %f\n",    SWS_getInstrumentSpeed());
//  printf("SWS_AbsoluteHeight: %f\n",     SWS_getAbsoluteHeight());
//  printf("BIML_Pos: %d\n",               (uint8_t)(0.5 + 0.3922*BIM_getStrapPosition(LEFT_BIM)));   // Перевод к процентной шкале с округлением
//  printf("BIMR_Pos: %d\n",               (uint8_t)(0.5 + 0.3922*BIM_getStrapPosition(RIGHT_BIM)));  // Перевод к процентной шкале с округлением
//  printf("SystemState: %x\n",            systemState);
//  printf("Model_Lat, deg: %f\n",         Bup_getCurrentPointLatitude());
//  printf("Model_Lon, deg: %f\n",         Bup_getCurrentPointLongitude());
//  printf("Model_Alt, m: %f\n",           Bup_getCurrentPointAltitude());
//  printf("Model_VelocityLat, m/s: %f\n", Bup_getCurrentVelocityLatitude());
//  printf("Model_VelocityLon, m/s: %f\n", Bup_getCurrentVelocityLongitude());
//  printf("Model_VelocityAlt, m/s: %f\n", Bup_getCurrentVelocityAltitude());
//  printf("Model_HeadingTrue, rad: %f\n", Bup_getCurrentHeadingTrue());
//  printf("Model_HeadingMgn, rad: %f\n",  Bup_getCurrentHeadingMgn());
//  printf("Model_Course, rad: %f\n",      Bup_getCurrentCourse());
//  printf("Model_Pitch, rad: %f\n",       Bup_getCurrentPitch());
//  printf("Model_Roll, rad: %f\n",        Bup_getCurrentRoll());	
//	
//  // Если карта рельефа в текущей позиции доступна, запишем высоту рельефа
//  if (SelfTesting_STATUS(ST_MapAvailability))
//    printf("MAP, m: %d\n",               Bup_getCurrentPointRelief());
//  else
//    printf("MAP, m: NOT_AVAILABLE\n");
//	
//  #ifdef flightRegulatorCFB	//******************************************************* Если выбран flightRegulatorCFB
//    uint8_t left = 0;
//    uint8_t right = 0;
//    if(rtY.directionOfRotation == -1)
//    {
//      left= (uint8_t)rtY.tightenSling;
//      right = 0;
//    }
//    else if(rtY.directionOfRotation == 1)
//    {
//      right = (uint8_t)rtY.tightenSling;
//      left = 0;
//    }
//    else if(rtY.directionOfRotation == 2)
//    {
//      right = (uint8_t)rtY.tightenSling;
//      left = (uint8_t)rtY.tightenSling;
//    }
//    printf("Model_Left_Bim: %d\n", left);
//    printf("Model_Right_Bim: %d\n", right);
//    printf("Model_TD_CMD: %d\n",  (uint8_t)rtY.cmdTouchDown);
//  #else //*************************************************************************** Если выбран flightController
//    printf("Model_Left_Bim: %d\n", (uint8_t)rtY.leftStrap);
//    printf("Model_Right_Bim: %d\n", (uint8_t)rtY.rightStrap);
//    printf("Model_TD_CMD: %d\n",  (uint8_t)rtY.touchdown);
//  #endif //************************************************************************** !flightRegulatorCFB
//}


///************************************************************************************
//  loger_exitmsg - Запись в лог информации о завершении работы БУП
//************************************************************************************/
//void loger_exitmsg(void)
//{
//  printf("\nThe flight is over!\n");
//  printf("Fin_Lat, deg: %f\n",         Bup_getCurrentPointLatitude());
//  printf("Fin_Lon, deg: %f\n",         Bup_getCurrentPointLongitude());
//  printf("Fin_Alt, m: %f\n",           Bup_getCurrentPointAltitude());
//  printf("Fin_SWSHeight: %f\n",        SWS_getAbsoluteHeight());
//  // Если карта рельефа в текущей позиции доступна, запишем высоту рельефа
//  if (SelfTesting_STATUS(ST_MapAvailability))
//    printf("Fin_MAP, m: %d\n",         Bup_getCurrentPointRelief());
//  else
//    printf("Fin_MAP, m: NOT_AVAILABLE\n");
//}
