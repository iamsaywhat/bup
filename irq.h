/****************************************************************

  irq.c/h - Модуль обслуживания прерываний
				 
  В текущем проекте используются следующие прерывания:
    - Timer1  - Обслуживает дискретизацию мат.модели;
    - Timer2  - Обслуживает модуль ZPZ;
    - SysTick - Обслуживает модуль otherlib;

****************************************************************/
#ifndef _IRQ_H_
#define _IRQ_H_


/****************************************************************
  SysTick_Handler - Обслуживание прерываний от 
  Системного таймера. Используется для отсчета
  времени с момента включения в мс. 
****************************************************************/
void SysTick_Handler (void);

/****************************************************************
  Timer1_IRQHandler - Обслуживание прерываний от Таймера 1.
  Используется для обслуживания математической модель полета 
  (задаёт период дискретизации).
****************************************************************/
void Timer1_IRQHandler(void);

/******************************************************************  
  Timer2_IRQHandler - Обслуживание прерываний от Таймера 2.
  Используется для режимов индикации.
******************************************************************/
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
