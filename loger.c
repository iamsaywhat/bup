#include "loger.h"

#include "otherlib.h"
#include "config.h"
#include "logfs/log.fs.h"
#include "retarget.printf/RetargetPrintf.h"
#include "selftesting.h"
#include "bupdatastorage.h"
#include "discreteio.h"


typedef enum {
  OPEN,
  CLOSE,
}Session_Status;


Session_Status session = CLOSE;

void logger_openNewSession(void)
{
  if(SelfTesting_STATUS(ST_25Q64FV) == ST_FAULT ||         // Если неисправна флэш черного ящика
    SelfTesting_STATUS(ST_LogFS) == ST_FAULT)              // или файловая система с ошибкой, то писать некуда
    return;                                                // поэтому просто выходим
  
  session = OPEN;
  LogFs_initialize();
  LogFs_createFile();
  printf_switcher(TO_LOG, 0);                              // Переключаем вывод в ЛОГ
  printf("session#%d\n", LogFs_getCurrentFileId());        // В начало файла кладём его порядковый номер
  printf("firmware version:");                             // Версия ПО, включая версию регулятора
  printf("%u.",   bupFirmwareVersion.majorFirmware);
  printf("%u.",   bupFirmwareVersion.minorFirmware);
  printf("%u ",   bupFirmwareVersion.microFirmware);
  printf("(%u.",  bupFirmwareVersion.optionsMath);
  printf("%u.",   bupFirmwareVersion.majorMath);
  printf("%u)\n", bupFirmwareVersion.minorMath);
  logger_point("td",                                       // Загруженная точка приземления
               Bup_getTouchdownPointLatitude(), 
               Bup_getTouchdownPointLongitude(), 
               Bup_getTouchdownPointAltitude());
  
  if(!CONNECT_ZPZ_CHECK)                                   // Если производится запуск в режиме зпз
    logger_warning("load zpz mode..");                     // То сообщаем об этом
  
  if(SelfTesting_STATUS(ST_1636PP52Y) == ST_FAULT)         // Пишем только жесткие неисправности
    logger_error("1636pp52y: fault");
  if(SelfTesting_STATUS(ST_25Q64FV) == ST_FAULT)
    logger_error("25q64fv: fault");
  if(SelfTesting_STATUS(ST_sns) == ST_FAULT)
    logger_error("sns: fault");
  if(SelfTesting_STATUS(ST_sws) == ST_FAULT)
    logger_error("sws: fault");
  if(SelfTesting_STATUS(ST_RADIOSTATION) == ST_FAULT)
    logger_error("radio: disconnected");
  if(SelfTesting_STATUS(ST_Left_BIM) == ST_FAULT)
    logger_error("left-bim: fault");
  if(SelfTesting_STATUS(ST_Right_BIM) == ST_FAULT)
    logger_error("right-bim: fault");
  if(SelfTesting_STATUS(ST_BATTERY50V) == ST_FAULT)
    logger_error("battery: low voltage");
  if(SelfTesting_STATUS(ST_MAP) == ST_FAULT)
    logger_error("map: not loaded");
  
  // Здесь сообщаем, а каком состоянии сейчас остальное
  SelfTesting_STATUS(ST_pin1) == ST_FAULT ? logger_warning("pin1: not inserted")
                                          : logger_warning("pin1: inserted");
  
  SelfTesting_STATUS(ST_pin2) == ST_FAULT ? logger_warning("pin2: not inserted") 
                                          : logger_warning("pin2: inserted");
  
  SelfTesting_STATUS(ST_MapAvailability) == ST_FAULT ? logger_warning("map: out of area")
                                                     : logger_warning("map: at area");
}
void logger_closeSession(void)
{
  session = CLOSE;
}
void logger_warning(char* string)
{
  if(session == OPEN)
    printf("{\"time\":%llu,\"warning\":\"%s\"}\n", getCurrentSystemTime(), string);
}
void logger_error(char* string)
{
  if(session == OPEN)
    printf("{\"time\":%llu,\"error\":\"%s\"}\n", getCurrentSystemTime(), string);
}
void logger_point(char* name, double latitude, double longitute, double altitude)
{
  if(session == OPEN)
    printf("{\"time\":%llu,\"point\":[\"%s\",%f,%f,%.2f]}\n", getCurrentSystemTime(), name, latitude, longitute, altitude);
}
void logger_track(double latitude, double longitude, double altitude)
{
  if(session == OPEN)
    printf("{\"time\":%llu,\"track\":[%d,%f,%f,%.2f]}\n", getCurrentSystemTime(), Bup_getControlTime(), latitude, longitude, altitude);
}
void logger_series_f (char* name, double value)
{
  if(session == OPEN)
    printf("{\"time\":%llu,\"series\":[\"%s\",%d,%f]}\n", getCurrentSystemTime(), name, Bup_getControlTime(), value);
}
void logger_series_f0 (char* name, double value)
{
  if(session == OPEN)
    printf("{\"time\":%llu,\"series\":[\"%s\",%d,%.0f]}\n", getCurrentSystemTime(), name, Bup_getControlTime(), value);
}
void logger_series_f2 (char* name, double value)
{
  if(session == OPEN)
    printf("{\"time\":%llu,\"series\":[\"%s\",%d,%.2f]}\n", getCurrentSystemTime(), name, Bup_getControlTime(), value);
}
void logger_series_lld (char* name, int64_t value)
{
  if(session == OPEN)
    printf("{\"time\":%llu,\"series\":[\"%s\",%d,%lld]}\n", getCurrentSystemTime(), name, Bup_getControlTime(), value);
}
void logger_series_llu (char* name, uint64_t value)
{
  if(session == OPEN)
    printf("{\"time\":%llu,\"series\":[\"%s\",%d,%llu]}\n", getCurrentSystemTime(), name, Bup_getControlTime(), value);
}
