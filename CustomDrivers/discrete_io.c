#include "discrete_io.h"

/**************************************************************************************************************
    Discrete_RetargetPins - Конфигурирование дискретных пинов
**************************************************************************************************************/
void Discrete_RetargetPins (void)
{
	/* Настраиваем пины на вход */
	Pin_init (PIN1);
	Pin_init (BLIND);
	Pin_init (PIN2_DIR);
	Pin_init (PIN2_INV);
	Pin_init (CONNECT_ZPZ);
	/* Настраиваем пины на выход */
	Pin_init (LED_READY);
	Pin_init (LED_FAULT);
	Pin_init (PYRO);
	Pin_init (BLIND_CTRL);
	/* Сбрасываем все выходы в ноль */
	Pin_reset (LED_READY);
	Pin_reset (LED_FAULT);
	Pin_reset (PYRO);
	Pin_reset (BLIND_CTRL);
}
