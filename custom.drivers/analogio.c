#include "analogio.h"
#include "filters.h"
#include "bupboard.h"
#include "MDR32F9Qx_adc.h"

/***************************************************************************
  Analog_initialize - Инициализация аналоговых входов
***************************************************************************/
void Analog_initialize(void)
{
  Pin_initialize(BATTERY_50V);
  ADC_initialize (USED_ADC);
} 

 
/************************************************************** Контроль 50 вольтовой секции АКБ **************/
/* Макрос для пересчета показаний АЦП в вольтаж на АКБ */
#define ConvertToVoltage(x) (float)(x*(3.3/4096)*50/3)       
/* Базовая функция запроса результата преобразования от АЦП */
static uint16_t getBatteryChargeFromADC(void)
{
  return ADC_getResult(USED_ADC, BATTERY_50V_CH);
}
#if defined BAT50V_USE_EMA /* Подключен EMA фильтр */
static float getBatteryChargeEMA (void)
{
  /* Объявления необходимые для работы фильтра */
  static double   EMA = 0;
  static uint8_t  initFlag = 0;
  const  uint16_t coefficient = BAT50V_EMA_COEFF;
  /* Запрос показаний делаем через фильтр */
  EMAFilter (&EMA, &initFlag, coefficient, getBatteryChargeFromADC);
	 
  /* Перед возвратом конвертируем в вольты*/
  return ConvertToVoltage(EMA);
}
#endif
#if defined BAT50V_USE_SMA /* Подключен SMA фильтр */
static float getBatteryChargeSMA(void)
{
  /* Объявления необходимые для работы фильтра */
  static double   SMA = 0;                      /* Выход фильтра */
  static uint8_t  initFlag = 0;                 /* Флаг выполнения инициализации */
  const  uint8_t  samples = BAT50V_SMA_FRAME;   /* Размер выборки */
  static uint32_t buffer[samples];              /* Буфер для хранения выборки */
  /* Запрос показаний делаем через фильтр */
  SMAFilter (buffer, samples, &SMA, &initFlag, getBatteryChargeFromADC);
	
  return ConvertToVoltage(SMA);
}
#endif
#if defined BAT50V_USE_MED /* Подключен MED фильтр */
static float getBatteryChargeMED(void)
{
  /* Объявления необходимые для работы фильтра */
  uint32_t MED = 0; 
  const  uint8_t  samples = BAT50V_MED_FRAME; /* Размер выборки */
  /* Запрос показаний делаем через фильтр */
  MED = medianFilter (samples, getBatteryChargeFromADC);
	
  return ConvertToVoltage(MED);
}
#endif
/* Универсальная функция */
float getBatteryCharge(void)
{
  float result = 0;
	
  #if defined BAT50V_USE_EMA && !defined BAT50V_USE_SMA && !defined BAT50V_USE_MED
    result = getBatteryChargeEMA();
  #elif !defined BAT50V_USE_EMA && defined BAT50V_USE_SMA && !defined BAT50V_USE_MED
    result = getBatteryChargeSMA();
  #elif !defined BAT50V_USE_EMA && !defined BAT50V_USE_SMA && defined BAT50V_USE_MED
    result = getBatteryChargeMED();
  #else
    result = ConvertToVoltage(GetBatteryChargeFromADC());
  #endif
	
  return result;
}
/**************************************************************************************************************/


/************************************************************** Резерв на 11 канале АЦП ***********************/
//const AnalogInput Ch11Reserved = {ADC1, PORT_Pin_11, ADC_CH_ADC11};
/**************************************************************************************************************/


/************************************************************** Резерв на 12 канале АЦП ***********************/
//const AnalogInput Ch12Reserved = {ADC1, PORT_Pin_12, ADC_CH_ADC12};
/**************************************************************************************************************/

