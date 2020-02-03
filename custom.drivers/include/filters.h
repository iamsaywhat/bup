/******************************************************************

    filters - Модуль програмных фильтров для аналоговых сигналов;
		
    Данный модуль содержит в себе следующие решения:
		
    * EMA (exponential moving average) - Экспоненциальное 
    скользящее среднее. 
    * Реализует следующий алгоритм: 
    EMA[t] = a*p[t] + (1-a)*EMA[t-1]
    * Параметром фильтра является: коэффициент a = 0...1;
    * Наиболее быстрый и менее ресурсозатратный алгоритм, 
    Уменьшение коэффициента делает, снижает влияние случайных 
    выбросов и сглаживает измеряемый сигнал, но вносит в 
    измерения серьёзную задержку. Больше подходит для 
    измерения медленных процессов.
		
    * SMA (simple moving average) - Простое скользящее среднее
    * Реализует следующий алгоритм: 
    SMA[t] = (p[t] + p[t-1] + ... + p[t-n+1])/n
    SMA[t] = SMA[t-1] + p[t-n]/n + p[t]/n
    * Параметром фильтра является: размер выборки: n - значений;
    * Для работы требует выделение буфера под хранение выборки,
    Увеличение размера выборки повышает качество, но занимает
    больше ресурсов и снижает быстродействие. В момент 
    инициализации делает N измерений, а при последующих 
    обращениях требует только одно измерение.
		
    * MED (Median filter) - Медианный фильтр
    * Реализует следующий алгоритм:
    В каждый момент времени осуществляет выборка из N значений
    (N обращений к датчику), далее эта выборка упорядочивается
    в порядке возрастания, и как результат берется медиана
    (середина) данной выборки.
    * Параметром фильтра является: размер выборки: N - значений;
    * Не требует хранение выборки от вызова к случаю,
    отличная реакция на случайные выборосы датчика, увеличение
    выборки повышает качество фильтрации, но снижает 
    быстродействие, так как за каждый проход требуется сделать 
    N измерений аналоговой величины.
																
*******************************************************************/
#ifndef _FILTERS_H_
#define _FILTERS_H_

#include "stdint.h"

/******************************************************************
    EMAFilter - Экспоненциально взвешенное скользящее 
                среднее (метод фильтрации)
    Параметры:  
                EMA      - Указатель на промежуточный результат
                initFlag - Указатель на флаг инициализации;
                coef     - Коэфициент фильтрации (0..10 000)
                getValue - Указатель на функцию опроса датчика
    Примечание:
                Для того, чтобы данный фильтр можно было 
                использовать для фильтрации разных величин,
                пользователь должен самостоятельно выделить
                место под данные для фильтра, и передать
                на них указатели;
    (!)Внимание:	
                Под каждую фильтруемую величину должены быть
                выделены отдельные ЕMA, initflag,
                а так же эти данные должны храниться от 
                вызова к вызову фильтра (требуется объявить 
                глобально, либо static). 		
    Пример использования:
		 
double SomeFunction (void)
{  
  static double   EMA = 0;
  static uint8_t  initFlag = 0;
  uint16_t (*getValue)(void) = GET_VALUE_NAME_FUNCION;
  ExpMovingAverageFilter (&EMA, &initFlag,10, getValue);
  return EMA;
}	
*******************************************************************/
void EMAFilter (double* EMA, uint8_t* initFlag,
                uint16_t coef, uint16_t (*getValue)(void));

/******************************************************************
    SMAFilter - Простое скользящее среднее (метод фильтрации)
    Параметры:  
                buffer     - Буфер, c которым работает фильтр
                bufferSize - Размер выборки
                SMA        - Указатель на промежуточный результат
                initFlag   - Указатель на флаг инициализации;
                getValue   - Указатель на функцию опроса датчика
    Примечание:
                Для того, чтобы данный фильтр можно было 
                использовать для фильтрации разных величин,
                пользователь должен самостоятельно выделить
                место под данные для фильтра, и передать
                на них указатели;
    (!)Внимание:	
                Под каждую фильтруемую величину должены быть
                выделены отдельные buffer, SMA, initflag,
                а так же эти данные должны храниться от 
                вызова к вызову фильтра (требуется объявить 
                глобально, либо static). 		
    Пример использования:
		 
double SomeFunction (void)
{ 
  static double SMA = 0;           // Выход фильтра
  static uint8_t  initFlag = 0;    // Флаг выполнения инициализации 
  const  uint8_t  samples = 50;    // Размер выборки 
  static uint32_t buffer[samples]; // Буфер для хранения выборки
	
  uint16_t (*getValue)(void) = GET_VALUE_NAME_FUNCION;
	
  SimpleMovingAverageFilter (buffer, samples, &SMA, &initFlag, getValue);
  return SMA;
}	
*******************************************************************/
void SMAFilter (uint32_t* buffer, uint8_t bufferSize, 
                double* SMA,      uint8_t* initFlag, 
                uint16_t (*getValue)(void));

/******************************************************************
    medianFilter - Медианный фильтр
    Параметры:  
                frameSize - Размер выборки  
                getValue  - Указатель на функцию получения 
                            фильтруемого сигнала
    Примечание:
                Реализация медианной фильтрации. Для работы
                с фильтром необходимо предоставить указатель
                на функцию без аргументов, которая осуществляет 
                опрос датчика, и указать размер выборки. 
    (!)Внимание:  
                Увеличение размера выборки снижает 
                быстродействие.
*******************************************************************/
uint32_t medianFilter (const uint8_t frameSize, uint16_t (*getValue)(void));

#endif