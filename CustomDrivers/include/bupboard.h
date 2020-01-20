/*******************************************************************************************************

  bupboard - Описание распиновки процессора под плату
	
  PORTA:
  Назначение-----------Пин------------Направление-----Фунция работы--------Режим работы  
  PIN1                 PORT_Pin_0     PORT_OE_IN      PORT_FUNC_PORT       PORT_MODE_DIGITAL
  BLIND                PORT_Pin_1     PORT_OE_IN      PORT_FUNC_PORT       PORT_MODE_DIGITAL
  PIN2_INV             PORT_Pin_2     PORT_OE_IN      PORT_FUNC_PORT       PORT_MODE_DIGITAL
  PIN2_DIR             PORT_Pin_3     PORT_OE_IN      PORT_FUNC_PORT       PORT_MODE_DIGITAL
  RESERVED             PORT_Pin_4     PORT_OE_IN      PORT_FUNC_PORT       PORT_MODE_DIGITAL
  RESERVED             PORT_Pin_5     PORT_OE_IN      PORT_FUNC_PORT       PORT_MODE_DIGITAL
  ---                  PORT_Pin_6     PORT_OE_        PORT_FUNC_           PORT_MODE_
  ---                  PORT_Pin_7     PORT_OE_        PORT_FUNC_           PORT_MODE_
  ---                  PORT_Pin_8     PORT_OE_        PORT_FUNC_           PORT_MODE_
  ---                  PORT_Pin_9     PORT_OE_        PORT_FUNC_           PORT_MODE_
  ---                  PORT_Pin_10    PORT_OE_        PORT_FUNC_           PORT_MODE_
  ---                  PORT_Pin_11    PORT_OE_        PORT_FUNC_           PORT_MODE_
  ---                  PORT_Pin_12    PORT_OE_        PORT_FUNC_           PORT_MODE_
  CONNECT_ZPZ          PORT_Pin_13    PORT_OE_IN      PORT_FUNC_PORT       PORT_MODE_DIGITAL
  RESERVED             PORT_Pin_14    PORT_OE_IN      PORT_FUNC_PORT       PORT_MODE_DIGITAL
  RESERVED             PORT_Pin_15    PORT_OE_IN      PORT_FUNC_PORT       PORT_MODE_DIGITAL
	
	
  PORTB:
  Назначение-----------Пин------------Направление-----Фунция работы--------Режим работы  
  JTAGA_TDO            PORT_Pin_0     PORT_OE_        PORT_FUNC_           PORT_MODE_
  JTAGA_TMS            PORT_Pin_1     PORT_OE_        PORT_FUNC_           PORT_MODE_
  JTAGA_TCK            PORT_Pin_2     PORT_OE_        PORT_FUNC_           PORT_MODE_
  JTAGA_TDI            PORT_Pin_3     PORT_OE_        PORT_FUNC_           PORT_MODE_
  JTAGA_TRST           PORT_Pin_4     PORT_OE_        PORT_FUNC_           PORT_MODE_
  SNS_TX               PORT_Pin_5     PORT_OE_OUT     PORT_FUNC_ALTER      PORT_MODE_DIGITAL
  SNS_RX               PORT_Pin_6     PORT_OE_IN      PORT_FUNC_ALTER      PORT_MODE_DIGITAL
  ---                  PORT_Pin_7     PORT_OE_        PORT_FUNC_PORT       PORT_MODE_
  SPI_1636PP52Y_CS1    PORT_Pin_8     PORT_OE_OUT     PORT_FUNC_PORT       PORT_MODE_DIGITAL
  SPI_1636PP52Y_CS2    PORT_Pin_9     PORT_OE_OUT     PORT_FUNC_PORT       PORT_MODE_DIGITAL
  SPI_1636PP52Y_CS3    PORT_Pin_10    PORT_OE_OUT     PORT_FUNC_PORT       PORT_MODE_DIGITAL
  SPI_1636PP52Y_CS4    PORT_Pin_11    PORT_OE_OUT     PORT_FUNC_PORT       PORT_MODE_DIGITAL
  ---                  PORT_Pin_12    PORT_OE_        PORT_FUNC_           PORT_MODE_
  SPI_1636PP52Y_CLK    PORT_Pin_13    PORT_OE_OUT     PORT_FUNC_ALTER      PORT_MODE_DIGITAL
  SPI_1636PP52Y_RXD    PORT_Pin_14    PORT_OE_IN      PORT_FUNC_ALTER      PORT_MODE_DIGITAL
  SPI_1636PP52Y_TXD    PORT_Pin_15    PORT_OE_OUT     PORT_FUNC_ALTER      PORT_MODE_DIGITAL
	
	
  PORTC:
  Назначение-----------Пин------------Направление-----Фунция работы--------Режим работы  
  ---                  PORT_Pin_0     PORT_OE_        PORT_FUNC_           PORT_MODE_
  ---                  PORT_Pin_1     PORT_OE_        PORT_FUNC_           PORT_MODE_
  ---                  PORT_Pin_2     PORT_OE_        PORT_FUNC_           PORT_MODE_
  ---                  PORT_Pin_3     PORT_OE_        PORT_FUNC_           PORT_MODE_
  ---                  PORT_Pin_4     PORT_OE_        PORT_FUNC_           PORT_MODE_
  ---                  PORT_Pin_5     PORT_OE_        PORT_FUNC_           PORT_MODE_
  RESERVED_CS          PORT_Pin_6     PORT_OE_OUT     PORT_FUNC_PORT       PORT_MODE_DIGITAL
  ---                  PORT_Pin_7     PORT_OE_        PORT_FUNC_           PORT_MODE_
  BIM_CAN_TX           PORT_Pin_8     PORT_OE_OUT     PORT_FUNC_MAIN       PORT_MODE_DIGITAL
  BIM_CAN_RX           PORT_Pin_9     PORT_OE_IN      PORT_FUNC_MAIN       PORT_MODE_DIGITAL
  BIM_CAN_CS1          PORT_Pin_10    PORT_OE_OUT     PORT_FUNC_PORT       PORT_MODE_DIGITAL
  ---                  PORT_Pin_11    PORT_OE_        PORT_FUNC_           PORT_MODE_
  ---                  PORT_Pin_12    PORT_OE_        PORT_FUNC_           PORT_MODE_
  ---                  PORT_Pin_13    PORT_OE_        PORT_FUNC_ALTER      PORT_MODE_DIGITAL
  CAN2_RX              PORT_Pin_14    PORT_OE_IN      PORT_FUNC_OVERRID    PORT_MODE_DIGITAL
  CAN2_TX              PORT_Pin_15    PORT_OE_OUT     PORT_FUNC_OVERRID    PORT_MODE_DIGITAL
	
	
  PORTD:
  Назначение-----------Пин------------Направление-----Фунция работы--------Режим работы  
  JTAGB_TMS            PORT_Pin_0     PORT_OE_        PORT_FUNC_           PORT_MODE_
  JTAGB_TCK            PORT_Pin_1     PORT_OE_        PORT_FUNC_           PORT_MODE_
  JTAGB_TRST           PORT_Pin_2     PORT_OE_        PORT_FUNC_           PORT_MODE_
  JTAGB_TDI            PORT_Pin_3     PORT_OE_        PORT_FUNC_           PORT_MODE_
  JTAGB_TDO            PORT_Pin_4     PORT_OE_        PORT_FUNC_           PORT_MODE_
  ---                  PORT_Pin_5     PORT_OE_        PORT_FUNC_           PORT_MODE_
  SWS_DE               PORT_Pin_6     PORT_OE_OUT     PORT_FUNC_PORT       PORT_MODE_DIGITAL
  SWS_RX               PORT_Pin_7     PORT_OE_IN      PORT_FUNC_OVERRID    PORT_MODE_DIGITAL
  SWS_TX               PORT_Pin_8     PORT_OE_OUT     PORT_FUNC_OVERRID    PORT_MODE_DIGITAL
  RESERVED_ADC9        PORT_Pin_9     PORT_OE_        PORT_FUNC_PORT       PORT_MODE_ANALOG
  BATTERY_50V          PORT_Pin_10    PORT_OE_OUT     PORT_FUNC_PORT       PORT_MODE_ANALOG
  RESERVED_ADC11       PORT_Pin_11    PORT_OE_IN      PORT_FUNC_PORT       PORT_MODE_ANALOG
  RESERVED_ADC12       PORT_Pin_12    PORT_OE_IN      PORT_FUNC_PORT       PORT_MODE_ANALOG
  ---                  PORT_Pin_13    PORT_OE_        PORT_FUNC_           PORT_MODE_
  ---                  PORT_Pin_14    PORT_OE_        PORT_FUNC_           PORT_MODE_
  ---                  PORT_Pin_15    PORT_OE_        PORT_FUNC_           PORT_MODE_
	
	
  PORTE:
  Назначение-----------Пин------------Направление-----Фунция работы--------Режим работы  
  RESERVED             PORT_Pin_0     PORT_OE_OUT     PORT_FUNC_PORT       PORT_MODE_DIGITAL
  RESERVED             PORT_Pin_1     PORT_OE_OUT     PORT_FUNC_PORT       PORT_MODE_DIGITAL
  LED_READY            PORT_Pin_2     PORT_OE_OUT     PORT_FUNC_PORT       PORT_MODE_DIGITAL
  LED_FAULT            PORT_Pin_3     PORT_OE_OUT     PORT_FUNC_PORT       PORT_MODE_DIGITAL
  ---                  PORT_Pin_4     PORT_OE_        PORT_FUNC_           PORT_MODE_
  ---                  PORT_Pin_5     PORT_OE_        PORT_FUNC_           PORT_MODE_
  OSC_IN32             PORT_Pin_6     PORT_OE_IN      PORT_FUNC_PORT       PORT_MODE_ANALOG
  OSC_OUT32            PORT_Pin_7     PORT_OE_OUT     PORT_FUNC_PORT       PORT_MODE_ANALOG
  PYRO                 PORT_Pin_8     PORT_OE_OUT     PORT_FUNC_PORT       PORT_MODE_DIGITAL
  BLIND_CTRL           PORT_Pin_9     PORT_OE_OUT     PORT_FUNC_PORT       PORT_MODE_DIGITAL
  RELAY_BIM            PORT_Pin_10    PORT_OE_OUT     PORT_FUNC_PORT       PORT_MODE_DIGITAL
  RESERVED             PORT_Pin_11    PORT_OE_OUT     PORT_FUNC_PORT       PORT_MODE_DIGITAL
  ---                  PORT_Pin_12    PORT_OE_        PORT_FUNC_           PORT_MODE_
  ---                  PORT_Pin_13    PORT_OE_        PORT_FUNC_           PORT_MODE_
  ---                  PORT_Pin_14    PORT_OE_        PORT_FUNC_           PORT_MODE_
  ---                  PORT_Pin_15    PORT_OE_        PORT_FUNC_           PORT_MODE_
	
	
	
  PORTF:
  Назначение-----------Пин------------Направление-----Фунция работы--------Режим работы  
  ZPZ_RX               PORT_Pin_0     PORT_OE_IN      PORT_FUNC_OVERRID    PORT_MODE_DIGITAL
  ZPZ_TX               PORT_Pin_1     PORT_OE_OUT     PORT_FUNC_OVERRID    PORT_MODE_DIGITAL
  ---                  PORT_Pin_2     PORT_OE_        PORT_FUNC_PORT       PORT_MODE_
  ---                  PORT_Pin_3     PORT_OE_        PORT_FUNC_PORT       PORT_MODE_
  MODE[0]              PORT_Pin_4     PORT_OE_        PORT_FUNC_           PORT_MODE_
  MODE[1]              PORT_Pin_5     PORT_OE_        PORT_FUNC_           PORT_MODE_
  MODE[2]              PORT_Pin_6     PORT_OE_        PORT_FUNC_           PORT_MODE_
  ---                  PORT_Pin_7     PORT_OE_        PORT_FUNC_           PORT_MODE_
  ---                  PORT_Pin_8     PORT_OE_        PORT_FUNC_           PORT_MODE_
  ---                  PORT_Pin_9     PORT_OE_        PORT_FUNC_           PORT_MODE_
  RESERVED             PORT_Pin_10    PORT_OE_OUT     PORT_FUNC_PORT       PORT_MODE_DIGITAL
  SPI_25Q64FV_CSn      PORT_Pin_11    PORT_OE_OUT     PORT_FUNC_PORT       PORT_MODE_DIGITAL
  ---                  PORT_Pin_12    PORT_OE_        PORT_FUNC_           PORT_MODE_
  SPI_25Q64FV_CLK      PORT_Pin_13    PORT_OE_OUT     PORT_FUNC_OVERRID    PORT_MODE_DIGITAL
  SPI_25Q64FV_RXD      PORT_Pin_14    PORT_OE_IN      PORT_FUNC_OVERRID    PORT_MODE_DIGITAL
  SPI_25Q64FV_TXD      PORT_Pin_15    PORT_OE_OUT     PORT_FUNC_OVERRID    PORT_MODE_DIGITAL

*******************************************************************************************************/
#ifndef _BUPBOARD_H_
#define _BUPBOARD_H_

#include "MDR32Fx.h"
#include "MDR32F9Qx_port.h"

/**************************
  Идентификаторы АЦП
**************************/
typedef enum {
  ADC1 = 1,
  ADC2 = 2,
}ADCdev;

/********************************************
  Используемые аппаратные модули
********************************************/
#define BIM_CAN                 MDR_CAN1       /* CAN используемый БИМ */
#define CAN_DEBUG               MDR_CAN1       /* CAN используемый под отладку */
#define PRINTF_CAN              MDR_CAN1       /* CAN используемый под вывод функцией printf */
#define SPI_25Q64FV_Module      MDR_SSP2       /* SPI используемый под flash черного ящика */
#define SPI_1636PP52Y_Module    MDR_SSP1       /* SPI используемый под flash полетного задания */
#define SNS_UART                MDR_UART1      /* UART используемый снс */
#define SWS_UART                MDR_UART1      /* UART используемый свс */
#define ZPZ_UART                MDR_UART2      /* UART используемый загрузчиком */
#define ZPZ_IRQn                UART2_IRQn     /* Используемое загрузчиком прерывание от UART */
#define ZPZ_CAN                 MDR_CAN1       /* CAN используемый загрузчиком */    
#define BLINK_TIMER             MDR_TIMER2     /* Таймер используемый под индикацию */
#define MMODEL_TIMER            MDR_TIMER1     /* Таймер используемый под обслуживания мат. модели регулятора */
#define USED_ADC                ADC1           /* Используемый АЦП под аналоговые сигналы */
#define BATTERY_50V_CH          ADC_CH_ADC10   /* Используемый канал АЦП */



/**************************
  Конфигурация пина
**************************/
typedef struct{
  MDR_PORT_TypeDef* port;         /* Порт */
  uint32_t          pinNum;       /* Номер пина */
  PORT_OE_TypeDef   direction;    /* Направление работы */
  PORT_FUNC_TypeDef function;     /* Функция пина */
  PORT_MODE_TypeDef mode;         /* Аналоговый или цифровой режим работы */
}PinConfigType;




/****************************************PORTA********************************************************************************/
extern const PinConfigType PIN1;               /* Шпилька 1 */
extern const PinConfigType BLIND;              /* Замок створки	 */
extern const PinConfigType PIN2_INV;           /* Шпилька 2 - инверсный сигнал */
extern const PinConfigType PIN2_DIR;           /* Шпилька 2 - прямой сигнал */
extern const PinConfigType CONNECT_ZPZ;        /* Подключение ЗПЗ */
/****************************************PORTB********************************************************************************/
extern const PinConfigType SNS_TX;             /* TX ножка для SNS */
extern const PinConfigType SNS_RX;             /* RX ножка для SNS */
extern const PinConfigType SPI_1636PP52Y_CS1;  /* Выбор микросхемы памяти #1 */
extern const PinConfigType SPI_1636PP52Y_CS2;  /* Выбор микросхемы памяти #2 */
extern const PinConfigType SPI_1636PP52Y_CS3;  /* Выбор микросхемы памяти #3 */
extern const PinConfigType SPI_1636PP52Y_CS4;  /* Выбор микросхемы памяти #4 */
extern const PinConfigType SPI_1636PP52Y_CLK;  /* SCLK линия последовательный тактовый сигнал шины SPI */
extern const PinConfigType SPI_1636PP52Y_RXD;  /* MISO линия - вход для мастера, выход для ведомого */
extern const PinConfigType SPI_1636PP52Y_TXD;  /* MOSI линия - выход для мастера, вход для ведомого */
/****************************************PORTC********************************************************************************/
extern const PinConfigType BIM_CAN_TX;         /* TX ножка порта для CAN1  */
extern const PinConfigType BIM_CAN_RX;         /* RX ножка порта для CAN1  */
extern const PinConfigType BIM_CAN_CS1;        /* Номер CS трансивера CAN1 */
extern const PinConfigType CAN2_RX;            /* RX ножка порта для CAN2  */
extern const PinConfigType CAN2_TX;            /* TX ножка порта для CAN2 */
/****************************************PORTD********************************************************************************/
extern const PinConfigType SWS_DE;             /* Направление передачи RS-485 */
extern const PinConfigType SWS_RX;             /* RX ножка для SWS */
extern const PinConfigType SWS_TX;             /* TX ножка для SWS */
extern const PinConfigType BATTERY_50V;        /* Аналоговый вход контроля напряжения АКБ */
/****************************************PORTE********************************************************************************/
extern const PinConfigType LED_READY;          /* Зеленый диод "Готов" */
extern const PinConfigType LED_FAULT;          /* Красный диод "Неисправность" */
extern const PinConfigType PYRO;               /* Пиропатрон */
extern const PinConfigType BLIND_CTRL;         /* Замок створки */
extern const PinConfigType RELAY_BIM;          /* Реле питания BIM */
/****************************************PORTF********************************************************************************/
extern const PinConfigType ZPZ_RX;             /* RX ножка для ZPZ */
extern const PinConfigType ZPZ_TX;             /* TX ножка для ZPZ */
extern const PinConfigType SPI_25Q64FV_CSn;    /* Выбор микросхемы 25Q64FV памяти */
extern const PinConfigType SPI_25Q64FV_CLK;    /* SCLK линия последовательный тактовый сигнал шины SPI */
extern const PinConfigType SPI_25Q64FV_RXD;    /* MISO линия - вход для мастера, выход для ведомого */
extern const PinConfigType SPI_25Q64FV_TXD;    /* MOSI линия - выход для мастера, вход для ведомого */
/*****************************************************************************************************************************/

#endif
