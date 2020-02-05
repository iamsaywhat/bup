/***************************************************************************
			
    MathModelAPI - Обёртка алгоритмов управления полетом САУ.
      
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
        1) flightController - Упрощенный алгоритм
        2) flightRegulatorCFB - Полновесный алгоритм
			
        Выбор осуществляется в файле конфигурации проекта config.h.
				
			
***************************************************************************/
#ifndef _MATH_MODEL_API_H_
#define _MATH_MODEL_API_H_

#include <stdint.h>



/***************************************************************************
  MathModel_initialize - Инициализация мат. модели
***************************************************************************/
void MathModel_initialize(void);



/***************************************************************************
  MathModel_step - Совершить шаг расчета
***************************************************************************/
void MathModel_step(void);



/***************************************************************************
  MathModel_prepareData - Заполнение входных данных для мат. модели
***************************************************************************/
void MathModel_prepareData (void);



/***************************************************************************
  MathModel_control - Выполнение команд мат. модели    
***************************************************************************/
void MathModel_control (void);



/******************************************************************************
  MathModel_sendBimCommand - Функция, предоставляющая мат модели 
  управление БИМами;
  Параметры:
          side         - Выбор стропы (-1, 0, 1, 2) = (левая в forcePersent, 
                         обе в ноль, правая в forcePersent, обе в в forcePersent
          forcePersent - Управляющий сигнал в процентах от (0..100%) 
                         на затяжку строп;
*******************************************************************************/
void MathModel_sendBimCommand (int8_t side, uint8_t forcePersent);

#endif
