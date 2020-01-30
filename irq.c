#include "irq.h"

#include "MDR32F9Qx_timer.h"
#include "Math_model/mathmodelapi.h"
#include "bupdatastorage.h"
#include "taskmanager.h"
#include "ZPZ.h"
#include "otherlib.h"
#include "discreteio.h"


/****************************************************************
  SysTick_Handler - Обслуживание прерываний от 
  Системного таймера. Используется для отсчета
  времени с момента включения в мс. 
****************************************************************/
void SysTick_Handler (void)
{
  SysTick_InterruptFunction();
}

/****************************************************************
  Timer1_IRQHandler - Обслуживание прерываний от Таймера 1.
  Используется для обслуживания математической модель полета 
  (задаёт период дискретизации).
****************************************************************/
void Timer1_IRQHandler(void)
{
  /* Сбрасываем флаг прерывания */
  TIMER_ClearFlag(MDR_TIMER1,TIMER_STATUS_CNT_ARR);
  /* Делаем инкремент количества секунд с момента начала управления */
  bupDataStorage.controlSecond++;
  /* Запускаем обновление данных */
  Bup_updateData();
  /* Отправляем данные математической модели */
  MathModel_prepareData ();
  /* Запускаем шаг расчета модели */	
  MathModel_step();
  /* Выполняем требования матмодели */
  MathModel_control();
  /* Сбрасываем цикл пассивных задач */
  TaskManager_gotoTask (TaskTest, 0);
}

/******************************************************************  
  Timer2_IRQHandler - Обслуживание прерываний от Таймера 2.
  Используется для режимов индикации.
******************************************************************/
void Timer2_IRQHandler(void)
{
  interruptIndication();
}



///****************************************************************
//Timer3_IRQHandler - Обслуживание прерываний от Таймера 3
//  
//  НЕ ИСПОЛЬЗУЕТСЯ
//****************************************************************/
//void Timer3_IRQHandler(void)
//{
//}
///****************************************************************
//DMA_IRQHandler - Обслуживание прерываний от контроллера DMA
//  
//  НЕ ИСПОЛЬЗУЕТСЯ
//****************************************************************/
//void DMA_IRQHandler(void)
//{	
//}
///****************************************************************
//CAN1_IRQHandler - Обслуживание прерываний от контроллера CAN1
//  
//  НЕ ИСПОЛЬЗУЕТСЯ
//****************************************************************/
//void CAN1_IRQHandler (void)
//{
//}
///****************************************************************
//CAN2_IRQHandler - Обслуживание прерываний от контроллера CAN2
//  
//  НЕ ИСПОЛЬЗУЕТСЯ
//****************************************************************/
//void CAN2_IRQHandler (void)
//{
//}
