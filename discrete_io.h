/*********************************************************************************************************

              discrete_io - Модуль дискретного управления/контроля переферии
							
*********************************************************************************************************/

#ifndef _DISCRETE_IO_H_
#define _DISCRETE_IO_H_

// Описание дискретных входов
#define DISCRETE_INPUT_PORT     MDR_PORTA   
#define CLK_DISCR_INPUT_PORT    RST_CLK_PCLK_PORTA 
#define PIN1                    PORT_Pin_0             // Шпилька 1
#define BLIND                   PORT_Pin_1				     // Замок створки	
#define PIN2_INV                PORT_Pin_2             // Шпилька 2 - инверсный сигнал
#define PIN2_DIR                PORT_Pin_3             // Шпилька 2 - прямой сигнал
#define RESERVED1               PORT_Pin_4             // Резерв
#define RESERVED2               PORT_Pin_5             // Резерв
#define CONNECT_ZPZ             PORT_Pin_13            // Подключение ЗПЗ


// Описание дискретных выходов
#define DISCRETE_OUTPUT_PORT    MDR_PORTE
#define CLK_DISCR_OUTPUT_PORT   RST_CLK_PCLK_PORTE
#define RESERVED3               PORT_Pin_0             // Резерв 
#define RESERVED4               PORT_Pin_1             // Резерв
#define LED_READY               PORT_Pin_2             // Зеленый диод "Готов"
#define LED_FAULT               PORT_Pin_3             // Красный диод "Неисправность"
#define PYRO                    PORT_Pin_8             // Пиропатрон
#define BLIND_CTRL              PORT_Pin_9             // Замок створки
#define RELAY_BIM               PORT_Pin_10            // Реле питания BIM
#define RESERVED5               PORT_Pin_11            // Резерв



// Макросы для именованного контроля состояний
#define PIN1_CHECK          PORT_ReadInputDataBit (DISCRETE_INPUT_PORT, PIN1)            // Воткнута ли шпилька 1                (Воткнута = 1)
#define BLIND_CHECK         PORT_ReadInputDataBit (DISCRETE_INPUT_PORT, BLIND)           // Открыт ли замок створки ли шпилька 1 (Закрыт = 0)
#define PIN2_DIR_CHECK      PORT_ReadInputDataBit (DISCRETE_INPUT_PORT, PIN2_DIR)        // Воткнута ли шпилька 2                (Воткнута = 1)
#define PIN2_INV_CHECK      PORT_ReadInputDataBit (DISCRETE_INPUT_PORT, PIN2_INV)        // Не воткнута ли шпилька 2             (Воткнута = 0)
#define CONNECT_ZPZ_CHECK   PORT_ReadInputDataBit (DISCRETE_INPUT_PORT, CONNECT_ZPZ)     // Подключен ли ЗПЗ к разъему           (Подключён = 0)
#define LED_READY_CHECK     PORT_ReadInputDataBit (DISCRETE_OUTPUT_PORT, LED_READY)      // Состояние индикатора "Готов"         (Вкл = 1, Выкл = 0)
#define LED_FAULT_CHECK     PORT_ReadInputDataBit (DISCRETE_OUTPUT_PORT, LED_FAULT)			 // Состояние индикатора "Неисправность" (Вкл = 1, Выкл = 0)
#define RELAY_BIM_CHECK     PORT_ReadInputDataBit (DISCRETE_OUTPUT_PORT, RELAY_BIM)      // Состояние реле питания БИМ           (Вкл = 1, Выкл = 0)
#define PYRO_CHECK          PORT_ReadInputDataBit (DISCRETE_OUTPUT_PORT, PYRO)           // Состояние реле пиропатрона           (Вкл = 1, Выкл = 0)
#define BLIND_CTRL_CHECK    PORT_ReadInputDataBit (DISCRETE_OUTPUT_PORT, BLIND_CTRL)     // Состояние реле замка сворки          (Вкл = 1, Выкл = 0)

// Макросы для именованного управления пинами
#define LED_READY_ON()        PORT_SetBits(DISCRETE_OUTPUT_PORT, LED_READY)              // Зажечь светодиод "ГОТОВ"
#define LED_READY_OFF()       PORT_ResetBits(DISCRETE_OUTPUT_PORT, LED_READY)						 // Погасить светодиод "ГОТОВ"
#define LED_FAULT_ON()        PORT_SetBits(DISCRETE_OUTPUT_PORT, LED_FAULT)              // Зажечь светодиод "НЕИСПРАВНОСТЬ"
#define LED_FAULT_OFF()       PORT_ResetBits(DISCRETE_OUTPUT_PORT, LED_FAULT)            // Погасить светодиод "НЕИСПРАВНОСТЬ"
#define PYRO_ON()             PORT_SetBits(DISCRETE_OUTPUT_PORT, PYRO)                   // Зажечь пиропатрон
#define PYRO_OFF()            PORT_ResetBits(DISCRETE_OUTPUT_PORT, PYRO)                 // Снять напряжение с зажигателя пиропатрона
#define BLIND_CTRL_ON()       PORT_SetBits(DISCRETE_OUTPUT_PORT, BLIND_CTRL)             // Открыть замок створки
#define BLIND_CTRL_OFF()      PORT_ResetBits(DISCRETE_OUTPUT_PORT, BLIND_CTRL)           // Закрыть замок створки


/**************************************************************************************************************
									Discrete_RetargetPins - Конфигурирование дискретных пинов	                                  *
**************************************************************************************************************/
void Discrete_RetargetPins (void);


#endif
