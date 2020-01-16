/*********************************************************************************************************

    discrete_io - Модуль дискретного управления/контроля переферии
							
*********************************************************************************************************/
#ifndef _DISCRETE_IO_H_
#define _DISCRETE_IO_H_

#include "bupboard.h"
#include "otherlib.h"

// Макросы для именованного контроля состояний
#define PIN1_CHECK          Pin_read (PIN1)           /* Воткнута ли шпилька 1                (Воткнута = 1)      */
#define BLIND_CHECK         Pin_read (BLIND)          /* Открыт ли замок створки ли шпилька 1 (Закрыт = 0)        */
#define PIN2_DIR_CHECK      Pin_read (PIN2_DIR)       /* Воткнута ли шпилька 2                (Воткнута = 1)      */
#define PIN2_INV_CHECK      Pin_read (PIN2_INV)       /* Не воткнута ли шпилька 2             (Воткнута = 0)      */
#define CONNECT_ZPZ_CHECK   Pin_read (CONNECT_ZPZ)    /* Подключен ли ЗПЗ к разъему           (Подключён = 0)     */
#define LED_READY_CHECK     Pin_read (LED_READY)      /* Состояние индикатора "Готов"         (Вкл = 1, Выкл = 0) */
#define LED_FAULT_CHECK     Pin_read (LED_FAULT)      /* Состояние индикатора "Неисправность" (Вкл = 1, Выкл = 0) */
#define RELAY_BIM_CHECK     Pin_read (RELAY_BIM)      /* Состояние реле питания БИМ           (Вкл = 1, Выкл = 0) */
#define PYRO_CHECK          Pin_read (PYRO)           /* Состояние реле пиропатрона           (Вкл = 1, Выкл = 0) */
#define BLIND_CTRL_CHECK    Pin_read (BLIND_CTRL)     /* Состояние реле замка сворки          (Вкл = 1, Выкл = 0) */

// Макросы для именованного управления пинами
#define LED_READY_ON()      Pin_set(LED_READY)        /* Зажечь светодиод "ГОТОВ" */
#define LED_READY_OFF()     Pin_reset(LED_READY)      /* Погасить светодиод "ГОТОВ" */
#define LED_FAULT_ON()      Pin_set(LED_FAULT)        /* Зажечь светодиод "НЕИСПРАВНОСТЬ" */
#define LED_FAULT_OFF()     Pin_reset(LED_FAULT)      /* Погасить светодиод "НЕИСПРАВНОСТЬ" */
#define PYRO_ON()           Pin_set(PYRO)             /* Зажечь пиропатрон */
#define PYRO_OFF()          Pin_reset(PYRO)           /* Снять напряжение с зажигателя пиропатрона */
#define BLIND_CTRL_ON()     Pin_set(BLIND_CTRL)       /* Открыть замок створки */
#define BLIND_CTRL_OFF()    Pin_reset(BLIND_CTRL)     /* Закрыть замок створки */


/**************************************************************************************************************
    Discrete_RetargetPins - Конфигурирование дискретных пинов
**************************************************************************************************************/
void Discrete_RetargetPins (void);


#endif
