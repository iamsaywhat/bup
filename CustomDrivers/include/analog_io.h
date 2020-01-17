/*********************************************************************************************************

    analog_io - Модуль аналового управления/контроля периферии
							
*********************************************************************************************************/
#ifndef _ANALOG_IO_H_
#define _ANALOG_IO_H_

#include "stdint.h"
#include "otherlib.h"

/***************************************************************************
  Analog_RetargetPins - Инициализация аналоговых входов
***************************************************************************/
void Analog_RetargetPins(void);

/***************************************************************************
  Battery50V - Аналоговый вход подключенный к секции 50 вольт АКБ;
		
  GetBatteryCharge - Возвращает напряжение на секции АКБ.
		
  С помощью макроса BAT50V_USE_x, 
    где x = EMA  - Использовать EMA Фильтр;
            SMA  - Использовать SMA Фильтр;
            MED  - Использовать MED Фильтр;
            NONE - Не использовать фильтры. 
						
  Можно подключить к данному аналоговому сигналу один из фильтров.
  Фильтры имеют параметры:
    EMA_COEFF = 1...10 000 - Параметр EMA фильтра определяющий степень 
                сглаживания аналогового сигнала (1 - максимальное 
                сглаживание,10 000 - минимальное);
    SMA_FRAME - Параметр SMA фильтра, определяющий размер буфера 
                выборки;
    MED_FRAME - Параметр MED фильтра, определяющий размер буфера 
                выборки;
***************************************************************************/
#define BAT50V_EMA_COEFF 100                                              //
#define BAT50V_SMA_FRAME 10                                               //
#define BAT50V_MED_FRAME 10                                               //
#define BAT50V_USE_MED                                                    //
float GetBatteryCharge(void);                                             //
/**************************************************************************/


/***************************************************************************
  11 Канал АЦП в резерве
***************************************************************************/
//extern const AnalogInput Ch11Reserved;
/**************************************************************************/

/***************************************************************************
  12 Канал АЦП в резерве
***************************************************************************/
//extern const AnalogInput Ch12Reserved;
/**************************************************************************/

#endif
