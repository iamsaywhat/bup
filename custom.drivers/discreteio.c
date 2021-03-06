#include "discreteio.h"

PinConfigType blinkPin;

/**************************************************************************************************************
  Discrete_initialize - Конфигурирование дискретных пинов
**************************************************************************************************************/
void Discrete_initialize (void)
{
  /* Настраиваем пины на вход */
  Pin_initialize (PIN1);
  Pin_initialize (BLIND);
  Pin_initialize (PIN2_DIR);
  Pin_initialize (PIN2_INV);
  Pin_initialize (CONNECT_ZPZ);
  /* Настраиваем пины на выход */
  Pin_initialize (LED_READY);
  Pin_initialize (LED_FAULT);
  Pin_initialize (PYRO);
  Pin_initialize (TOUCHDOWN_PYRO);
  /* Сбрасываем все выходы в ноль */
  Pin_reset (LED_READY);
  Pin_reset (LED_FAULT);
  Pin_reset (PYRO);
  Pin_reset (TOUCHDOWN_PYRO);
}

/**************************************************************************************************************
  runIndication - Запуск мигающей индикации
  Параметры:
            pin    - пин с конфигурацией;
            period - период следования импульсов
**************************************************************************************************************/
void runIndication(PinConfigType* pin, unsigned long period)
{
  blinkPin = *pin;                    /* Копируем настройки индикации */
	Pin_reset(blinkPin);                /* Выключаем индикатор */
  Timer_initialize(BLINK_TIMER, period);    /* Запускаем таймер */
}

/**************************************************************************************************************
  stopIndication - Остановка мигающей индикации
**************************************************************************************************************/
void stopIndication(void)
{
  TIMER_Cmd(BLINK_TIMER, DISABLE);   /* Отключаем таймер */
}

/**************************************************************************************************************
  interruptIndication - Функция обслуживания прерываний от используемого таймера.
  Примечание: 
             Необходимо разместить в базовом обработчике прерывания
**************************************************************************************************************/
void interruptIndication(void)
{
  TIMER_ClearFlag(BLINK_TIMER, TIMER_STATUS_CNT_ARR);	   /* Сбрасываем флаг прерываний таймера */
  if (Pin_read(blinkPin))                                /* Сменяем состояние индикатора на противоположное */
    Pin_reset(blinkPin);
  else 
    Pin_set(blinkPin);                            
}
