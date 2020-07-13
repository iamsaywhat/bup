/***************************************************************************	
  config.h - Файл конфигурации проекта	
***************************************************************************/
#ifndef _CONFIG_H_
#define _CONFIG_H_

//#define flightRegulatorCFB       // Использовать flightRegulatorCFB регулятор
#define DEBUG_VARS               // Включить модуль отладки (модуль debug.c)
#define LOGS_ENABLE              // Включить модуль логирования в черный ящик


/* Учет версий ПО */
#define FIRMWARE_VERSION_MAJOR    0     // Старшая версия ПО
#define FIRMWARE_VERSION_MINOR    32    // Младшая версия ПО
#define FIRMWARE_VERSION_MICRO    5     // Изменения

#ifdef flightRegulatorCFB
  #define MATH_OPTION             1     // Опция регулятора  
  #define MATH_VERSION_MAJOR      0     // Cтаршая версия регулятора
  #define MATH_VERSION_MINOR      1     // Младшая версия регулятора
#else
  #define MATH_OPTION             2     // Опция регулятора  
  #define MATH_VERSION_MAJOR      1     // Cтаршая версия регулятора
  #define MATH_VERSION_MINOR      1     // Младшая версия регулятора
#endif

#endif
