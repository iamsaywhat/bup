/***************************************************************************
			
  loger - Модуль, задающий формат записи лога
				
***************************************************************************/
#ifndef _LOG_RECORDER_H_
#define _LOG_RECORDER_H_

#include <stdint.h>


/************************************************************************************
  loger_exitmsg - Запись в лог информации о завершении работы БУП
************************************************************************************/
void logger_openNewSession(void);
void logger_closeSession(void);
void logger_warning (char* string);
void logger_error (char* string);
void logger_point (char* name, double latitude, double longitute, double altitude);
void logger_series (char* name, double value);
void logger_track (double latitude, double longitude, double altitude);
void logger_series_f (char* name, double value);
void logger_series_f0 (char* name, double value);
void logger_series_f2 (char* name, double value);
void logger_series_lld (char* name, int64_t value);
void logger_series_llu (char* name, uint64_t value);


#endif
