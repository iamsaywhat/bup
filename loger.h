/***************************************************************************
			
  loger - Модуль, задающий формат записи лога
				
***************************************************************************/
#ifndef _LOG_RECORDER_H_
#define _LOG_RECORDER_H_


//typedef enum {
//  Logger
//  Logger_LLU,
//  Logger_F0,
//  Logger_F1,
//  Logger_F2
//}Logger_mods;

/************************************************************************************
  loger_exitmsg - Запись в лог информации о завершении работы БУП
************************************************************************************/
void logger_openNewSession(void);
void logger_warning(char* string);
void logger_error(char* string);
void logger_point(char* name, double latitude, double longitute, double altitude);
void logger_series(char* name, double value);
void logger_track(double latitude, double longitude, double altitude);

#endif
