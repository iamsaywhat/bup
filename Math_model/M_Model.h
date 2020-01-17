/***************************************************************************
			
    M_Model - Обёртка алгоритмов управления полетом САУ.
      
    Связывает систему управления полетом с периферией БУПа,
    и предоставляет управление БИМами из под математической 
    модели.
			
    Функционал:
        * Конвертация данных от СНС и СВС в пригодный для матмодели вид;
        * Подстановка данных от СНС и СВС на вход матмодели
        * Инициализация матмодели и необходимой для ее работы периферии
        * Запуск шага расчета матмодели
        * Обработка результатов работы матмодели и выдача управляющих
        воздействий.
				
        ** Данный модуль резервирует под свои нужды таймер и соответству-
        ющий ему обработчик прерываний.
			
        ** Текущая версия модуля поддерживает две версии полетного регулятора:
        1) Easy_reg - Упрощенный алгоритм
        2) flightRegulatorCFB - Полновесный алгоритм
			
        Выбор осуществляется в файле конфигурации проекта config.h.
				
			
***************************************************************************/
#ifndef _M_MODEL_H_
#define _M_MODEL_H_

#include <stdint.h>



/***************************************************************************
  M_Model_Init - Инициализация мат. модели
***************************************************************************/
void M_Model_Init(void);



/***************************************************************************
  M_Model_Step - Совершить шаг расчета
***************************************************************************/
void M_Model_Step(void);



/***************************************************************************
  M_Model_PrepareData - Заполнение входных данных для мат. модели
***************************************************************************/
void M_Model_PrepareData (void);



/***************************************************************************
  M_Model_Control - Выполнение команд мат. модели    
***************************************************************************/
void M_Model_Control (void);



/******************************************************************************
  M_Model_Cmd2BIM - Функция, предоставляющая мат модели управление БИМами
  Параметры:
          Side - Управляющий сигнал в процентах от (-100% до 100%)
                где отрицательная область - сигнал для левого БИМа,
                положительная - сигнал для правого БИМа, а 0 слабления всех
                строп.
*******************************************************************************/
void M_Model_Cmd2BIM (double Side);

#endif
