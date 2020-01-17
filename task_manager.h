/***************************************************************************
			
    task_manager - Планировщик задач.
			
    Данный планировщик задаёт порядок и приоритет выполнения необходимых
    задач. Алгоритм работы БУПа в боевом режиме цикличен, и завязан
    по времени на частоту дискретизации мат модели, таким образом
    порядок выполнения задач и их перечень заранее известен.
    Реализован на подобии принципа конечных автоматов, только с 
    более простой и линейной структурой.
			
    Приоритет задачи определяется номером в TaskManagerRun.
    В первую очередь нам гарантированно необходимо обсчитывать
    мат модель и обновлять данные от внешних устройств. Поэтому 
    они первые в списке на выполнение. Остальные задачи являются 
    вторичными и хоть их пропуск нежелателен - он некритичен.
			
    Текущий цикл задач в боевом режиме по приоритетам:
        1) Обсчет матмодели и управление;
        2) Подготовка данные для следующей итерации;
        3) Самодиагностика
        4) Трассировка данных в CAN
        5) Логирование
			
			
    Текущий цикл задач в режиме ЗПЗ по приоритетам:
    1) Опрос СНС и СВС для идентификации положения в пространстве;
    2) Самодиагностика;
			
    После выполнения цикла, планировщик ждет перезапуска.
    Перезапуск планировщика происходит по таймеру в соответствии
    с выбранной частотой дискретизации мат модели;
			
    Тяжелые вторичные задачи разбиты на небольшие этапы, 
    благодаря чему планировщик успевает реагировать на необходимость
    перейти к более приоритетным задачам. Если цикл работы не завершился,
    но матмодель уже требует обслуживания, таймер сбрасывает планировщик
    и те задачи, которые на данном цикле не выполнились - пропускаются.			
			
****************************************************************************/
#ifndef _TASK_MANAGER_H_
#define _TASK_MANAGER_H_

#include <stdint.h>

/****************************************************
  Идентификаторы задач
****************************************************/
typedef enum {
  TaskStep   = 0,      // Расчет матмодели и управление
  TaskUpdate = 1,      // Подготовка данных для следующих итераций
  TaskTest   = 2,      // Самодиагностика      
  TaskDebug  = 3,      // Трассировка в CAN отладочных данных
  TaskLoger  = 4,      // Логирование
}TaskManagerCode;


/*********************************************************************************************************
  TaskManager_RestartCycle - Перезапуск цикла задач планировщика
**********************************************************************************************************/
void TaskManager_RestartCycle(void);


/*********************************************************************************************************
  TaskManagerGo2Task - Переход к задаче номер Code, на этап Stage
**********************************************************************************************************/
void TaskManagerGo2Task (TaskManagerCode Code, uint8_t Stage);


/*********************************************************************************************************
  TaskManagerRun - Запуск планировщика задач на выполнение цикла задач
**********************************************************************************************************/
void TaskManagerRun (void);


/*********************************************************************************************************
  TaskManagerZPZBackgroundRun - Запуск планировщика фоновых процессов в режиме ЗПЗ
**********************************************************************************************************/
void TaskManagerZPZBackgroundRun (void);


#endif
