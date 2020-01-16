#include "bupboard.h"

/****************************************PORTA********************************************************************************/
const PinConfigType PIN1              = {MDR_PORTA,  PORT_Pin_0,     PORT_OE_IN,      PORT_FUNC_PORT,       PORT_MODE_DIGITAL};
const PinConfigType BLIND             = {MDR_PORTA,  PORT_Pin_1,     PORT_OE_IN,      PORT_FUNC_PORT,       PORT_MODE_DIGITAL};
const PinConfigType PIN2_INV          = {MDR_PORTA,  PORT_Pin_2,     PORT_OE_IN,      PORT_FUNC_PORT,       PORT_MODE_DIGITAL};
const PinConfigType PIN2_DIR          = {MDR_PORTA,  PORT_Pin_3,     PORT_OE_IN,      PORT_FUNC_PORT,       PORT_MODE_DIGITAL};
const PinConfigType CONNECT_ZPZ       = {MDR_PORTA,  PORT_Pin_13,    PORT_OE_IN,      PORT_FUNC_PORT,       PORT_MODE_DIGITAL};
/****************************************PORTB********************************************************************************/
const PinConfigType SNS_TX            = {MDR_PORTB,  PORT_Pin_5,     PORT_OE_OUT,     PORT_FUNC_ALTER,      PORT_MODE_DIGITAL};
const PinConfigType SNS_RX            = {MDR_PORTB,  PORT_Pin_6,     PORT_OE_IN,      PORT_FUNC_ALTER,      PORT_MODE_DIGITAL};
const PinConfigType SPI_1636PP52Y_CS1 = {MDR_PORTB,  PORT_Pin_8,     PORT_OE_OUT,     PORT_FUNC_PORT,       PORT_MODE_DIGITAL};
const PinConfigType SPI_1636PP52Y_CS2 = {MDR_PORTB,  PORT_Pin_9,     PORT_OE_OUT,     PORT_FUNC_PORT,       PORT_MODE_DIGITAL};
const PinConfigType SPI_1636PP52Y_CS3 = {MDR_PORTB,  PORT_Pin_10,    PORT_OE_OUT,     PORT_FUNC_PORT,       PORT_MODE_DIGITAL};
const PinConfigType SPI_1636PP52Y_CS4 = {MDR_PORTB,  PORT_Pin_11,    PORT_OE_OUT,     PORT_FUNC_PORT,       PORT_MODE_DIGITAL};
const PinConfigType SPI_1636PP52Y_CLK = {MDR_PORTB,  PORT_Pin_13,    PORT_OE_OUT,     PORT_FUNC_ALTER,      PORT_MODE_DIGITAL};
const PinConfigType SPI_1636PP52Y_RXD = {MDR_PORTB,  PORT_Pin_14,    PORT_OE_IN,      PORT_FUNC_ALTER,      PORT_MODE_DIGITAL};
const PinConfigType SPI_1636PP52Y_TXD = {MDR_PORTB,  PORT_Pin_15,    PORT_OE_OUT,     PORT_FUNC_ALTER,      PORT_MODE_DIGITAL};
/****************************************PORTC********************************************************************************/
const PinConfigType BIM_CAN_TX        = {MDR_PORTC,  PORT_Pin_8,     PORT_OE_OUT,     PORT_FUNC_MAIN,       PORT_MODE_DIGITAL};
const PinConfigType BIM_CAN_RX        = {MDR_PORTC,  PORT_Pin_9,     PORT_OE_IN,      PORT_FUNC_MAIN,       PORT_MODE_DIGITAL};
const PinConfigType BIM_CAN_CS1       = {MDR_PORTC,  PORT_Pin_10,    PORT_OE_OUT,     PORT_FUNC_PORT,       PORT_MODE_DIGITAL};
const PinConfigType CAN2_RX           = {MDR_PORTC,  PORT_Pin_14,    PORT_OE_IN,      PORT_FUNC_OVERRID,    PORT_MODE_DIGITAL};
const PinConfigType CAN2_TX           = {MDR_PORTC,  PORT_Pin_15,    PORT_OE_OUT,     PORT_FUNC_OVERRID,    PORT_MODE_DIGITAL};
/****************************************PORTD********************************************************************************/
const PinConfigType SWS_DE            = {MDR_PORTD,  PORT_Pin_6,     PORT_OE_OUT,     PORT_FUNC_PORT,       PORT_MODE_DIGITAL}; 
const PinConfigType SWS_RX            = {MDR_PORTD,  PORT_Pin_7,     PORT_OE_IN,      PORT_FUNC_OVERRID,    PORT_MODE_DIGITAL}; 
const PinConfigType SWS_TX            = {MDR_PORTD,  PORT_Pin_8,     PORT_OE_OUT,     PORT_FUNC_OVERRID,    PORT_MODE_DIGITAL}; 
const PinConfigType BATTERY_50V       = {MDR_PORTD,  PORT_Pin_10,    PORT_OE_OUT,     PORT_FUNC_PORT,       PORT_MODE_ANALOG}; 
/****************************************PORTE********************************************************************************/
const PinConfigType LED_READY         = {MDR_PORTE,  PORT_Pin_2,     PORT_OE_OUT,     PORT_FUNC_PORT,       PORT_MODE_DIGITAL};
const PinConfigType LED_FAULT         = {MDR_PORTE,  PORT_Pin_3,     PORT_OE_OUT,     PORT_FUNC_PORT,       PORT_MODE_DIGITAL}; 
const PinConfigType PYRO              = {MDR_PORTE,  PORT_Pin_8,     PORT_OE_OUT,     PORT_FUNC_PORT,       PORT_MODE_DIGITAL}; 
const PinConfigType BLIND_CTRL        = {MDR_PORTE,  PORT_Pin_9,     PORT_OE_OUT,     PORT_FUNC_PORT,       PORT_MODE_DIGITAL}; 
const PinConfigType RELAY_BIM         = {MDR_PORTE,  PORT_Pin_10,    PORT_OE_OUT,     PORT_FUNC_PORT,       PORT_MODE_DIGITAL};
/****************************************PORTF********************************************************************************/
const PinConfigType ZPZ_RX            = {MDR_PORTF,  PORT_Pin_0,     PORT_OE_IN,      PORT_FUNC_OVERRID,    PORT_MODE_DIGITAL};
const PinConfigType ZPZ_TX            = {MDR_PORTF,  PORT_Pin_1,     PORT_OE_OUT,     PORT_FUNC_OVERRID,    PORT_MODE_DIGITAL}; 
const PinConfigType SPI_25Q64FV_CSn   = {MDR_PORTF,  PORT_Pin_11,    PORT_OE_OUT,     PORT_FUNC_PORT,       PORT_MODE_DIGITAL};
const PinConfigType SPI_25Q64FV_CLK   = {MDR_PORTF,  PORT_Pin_13,    PORT_OE_OUT,     PORT_FUNC_OVERRID,    PORT_MODE_DIGITAL};
const PinConfigType SPI_25Q64FV_RXD   = {MDR_PORTF,  PORT_Pin_14,    PORT_OE_IN,      PORT_FUNC_OVERRID,    PORT_MODE_DIGITAL};
const PinConfigType SPI_25Q64FV_TXD   = {MDR_PORTF,  PORT_Pin_15,    PORT_OE_OUT,     PORT_FUNC_OVERRID,    PORT_MODE_DIGITAL};
/*****************************************************************************************************************************/
