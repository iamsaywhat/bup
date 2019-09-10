#include "irq.h"


#include "discrete_io.h"
#include "Math_model/M_Model.h"
#include "bup_data_store.h"
#include "MDR32F9Qx_timer.h"
#include "task_manager.h"




/****************************************************************
Timer1_IRQHandler - Обслуживание прерываний от Таймера 1
	
	 - Данный таймер обслуживает математическую модель полета 
	(задаёт период дискретизации)
****************************************************************/
void Timer1_IRQHandler(void)
{
	// Сбрасываем флаг прерывания
	TIMER_ClearFlag(MDR_TIMER1,TIMER_STATUS_CNT_ARR);	
		// Делаем инкремент количества секунд с момента начала управления
	BUP_DataStorage.ControlSecond ++;
	// Перезапускаем цикл задач
	TaskManager_RestartCycle();
}


/****************************************************************
Timer2_IRQHandler - Обслуживание прерываний от Таймера 2
  
   - Данный таймер обслуживает режим ВПЗ модуля ЗПЗ;
   - Функция перенесена в ZPZ.c
****************************************************************/
//void Timer2_IRQHandler(void)
//{
//	// Сбрасываем флаг прерываний
//	TIMER_ClearFlag(ZPZ_TIMER,TIMER_STATUS_CNT_ARR);	
//}


///****************************************************************
//Timer3_IRQHandler - Обслуживание прерываний от Таймера 3
//  
//  НЕ ИСПОЛЬЗУЕТСЯ
//****************************************************************/
//void Timer3_IRQHandler(void)
//{
//}


///****************************************************************
//SysTick_Handler - Обслуживание прерываний от Системного таймера
//  
//  НЕ ИСПОЛЬЗУЕТСЯ
//****************************************************************/
//void SysTick_Handler (void)
//{
//	
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
