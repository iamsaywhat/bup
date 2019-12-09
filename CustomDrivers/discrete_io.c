#include "discrete_io.h"

#include "otherlib.h"
#include "MDR32F9Qx_rst_clk.h"
#include "MDR32F9Qx_port.h"

/**************************************************************************************************************
    Discrete_RetargetPins - Конфигурирование дискретных пинов
**************************************************************************************************************/
void Discrete_RetargetPins (void)
{
	// Разрешаем тактирование портов
	RST_CLK_PCLKcmd(CLK_DISCR_INPUT_PORT,  ENABLE);
	RST_CLK_PCLKcmd(CLK_DISCR_OUTPUT_PORT, ENABLE);
	// Настраиваем пины на вход
	Pin_Init (DISCRETE_INPUT_PORT, PIN1,        PORT_FUNC_PORT, PORT_OE_IN);
	Pin_Init (DISCRETE_INPUT_PORT, BLIND,       PORT_FUNC_PORT, PORT_OE_IN);
	Pin_Init (DISCRETE_INPUT_PORT, PIN2_DIR,    PORT_FUNC_PORT, PORT_OE_IN);
	Pin_Init (DISCRETE_INPUT_PORT, PIN2_INV,    PORT_FUNC_PORT, PORT_OE_IN);
	Pin_Init (DISCRETE_INPUT_PORT, RESERVED1,   PORT_FUNC_PORT, PORT_OE_IN);
	Pin_Init (DISCRETE_INPUT_PORT, RESERVED2,   PORT_FUNC_PORT, PORT_OE_IN);
	Pin_Init (DISCRETE_INPUT_PORT, CONNECT_ZPZ, PORT_FUNC_PORT, PORT_OE_IN);
	// Настраиваем пины на выход
	Pin_Init (DISCRETE_OUTPUT_PORT, RESERVED3, PORT_FUNC_PORT, PORT_OE_OUT);
	Pin_Init (DISCRETE_OUTPUT_PORT, RESERVED4, PORT_FUNC_PORT, PORT_OE_OUT);
	Pin_Init (DISCRETE_OUTPUT_PORT, LED_READY, PORT_FUNC_PORT, PORT_OE_OUT);
	Pin_Init (DISCRETE_OUTPUT_PORT, LED_FAULT, PORT_FUNC_PORT, PORT_OE_OUT);
	Pin_Init (DISCRETE_OUTPUT_PORT, PYRO,      PORT_FUNC_PORT, PORT_OE_OUT);
	Pin_Init (DISCRETE_OUTPUT_PORT, BLIND_CTRL,PORT_FUNC_PORT, PORT_OE_OUT);
	Pin_Init (DISCRETE_OUTPUT_PORT, RESERVED5, PORT_FUNC_PORT, PORT_OE_OUT);
	// Сбрасываем все выходы в ноль
	PORT_ResetBits (DISCRETE_OUTPUT_PORT, RESERVED3);
	PORT_ResetBits (DISCRETE_OUTPUT_PORT, RESERVED4);
	PORT_ResetBits (DISCRETE_OUTPUT_PORT, LED_READY);
	PORT_ResetBits (DISCRETE_OUTPUT_PORT, LED_FAULT);
	PORT_ResetBits (DISCRETE_OUTPUT_PORT, PYRO);
	PORT_ResetBits (DISCRETE_OUTPUT_PORT, BLIND_CTRL);
	PORT_ResetBits (DISCRETE_OUTPUT_PORT, RESERVED5);
}
