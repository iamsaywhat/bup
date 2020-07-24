/***************************************************************************	
  config.h - Файл конфигурации проекта	
***************************************************************************/
#ifndef _CONFIG_H_
#define _CONFIG_H_

#define DEBUG_VARS               // Включить модуль отладки (модуль debug.c)
#define LOGS_ENABLE              // Включить модуль логирования в черный ящик

/* Учет версий ПО */
#define FIRMWARE_VERSION_MAJOR    0     // Старшая версия ПО
#define FIRMWARE_VERSION_MINOR    32    // Младшая версия ПО
#define FIRMWARE_VERSION_MICRO    7     // Изменения
#define MATH_OPTION               2     // Опция регулятора  
#define MATH_VERSION_MAJOR        1     // Cтаршая версия регулятора
#define MATH_VERSION_MINOR        1     // Младшая версия регулятора

#define PIN1_DELAY   14000     // Задержка на детонацию пирозамков
#define PIN2_DELAY   30000     // Задержка на раскрытие планера

#endif
