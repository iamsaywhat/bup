/****************************************************************

         irq.c/h - Модуль обслуживания прерываний
				 
				 В текущем проекте используются следующие
				 прерывания:
				 
				 - Таймер 1 - Обслуживает дискретизацию модели
				 - Таймер 2 - Обслуживает световую индикацию
				 
****************************************************************/
#ifndef _IRQ_H_
#define _IRQ_H_


extern unsigned int ControlSecond;

/****************************************************************
Timer1_IRQHandler - Обсуживание прерываний от Таймера 2
	
	 - Данный таймер обслуживает математическую модель полета 
	(задаёт период дискретизации)
****************************************************************/
void Timer1_IRQHandler(void);



/****************************************************************
Timer2_IRQHandler - Обсуживание прерываний от Таймера 1
  
   - Данный таймер обслуживает световую индикацию
****************************************************************/
void Timer2_IRQHandler(void);



///****************************************************************
//Timer3_IRQHandler - Обсуживание прерываний от Таймера 3
//  
//  НЕ ИСПОЛЬЗУЕТСЯ
//****************************************************************/
//void Timer3_IRQHandler(void);



///****************************************************************
//DMA_IRQHandler - Обсуживание прерываний от контроллера DMA
//  
//  НЕ ИСПОЛЬЗУЕТСЯ
//****************************************************************/
//void DMA_IRQHandler(void);



///****************************************************************
//CAN1_IRQHandler - Обсуживание прерываний от контроллера CAN1
//  
//  НЕ ИСПОЛЬЗУЕТСЯ
//****************************************************************/
//void CAN1_IRQHandler (void);



///****************************************************************
//CAN2_IRQHandler - Обсуживание прерываний от контроллера CAN2
//  
//  НЕ ИСПОЛЬЗУЕТСЯ
//****************************************************************/
//void CAN2_IRQHandler (void);

#endif
