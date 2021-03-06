#include "filters.h"

static double EMAFilterBase (double actual,    double previous,   uint16_t coef);
static void   SMAFilterBase (uint32_t* frame,  uint8_t frameSize, double* SMA, uint32_t actual);
static void   shakerSort    (uint32_t* buffer, uint8_t samples);

/*--------------------------------------------------------------------------------------- Публичные функции модуля --*/
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
  uint16_t (*getValue)(void) = GET_VALUE_NAME_FUNCION();
  ExpMovingAverageFilter (&EMA, &initFlag,10, getValue);
  return EMA;
}	
*******************************************************************/
void EMAFilter (double* EMA,  uint8_t* initFlag,
                uint16_t coef, uint16_t (*getValue)(void))
{
  /* Получаем значение от фунции */
  double p = getValue();	
  /* Проверяем была ли инициализация */
  if(!(*initFlag))
  {	
    /* Инициализация прошла */
    *initFlag = 1;
    /* Во избежания "разогрева датчика"
    на выход сразу подаём первое 
    нефильтрованное значение	*/
    *EMA = p;
  }
  /* Если инициализация была */
  else
  {
    /* То теперь применяем алгоритм фильтрации */
    *EMA = EMAFilterBase (p, *EMA, coef);
  }
}

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
	
  uint16_t (*getValue)(void) = GET_VALUE_NAME_FUNCION();
	
  SimpleMovingAverageFilter (buffer, samples, &SMA, &initFlag, getValue);
  return SMA;
}	
*******************************************************************/
void SMAFilter (uint32_t* buffer, uint8_t bufferSize, 
                double* SMA,      uint8_t* initFlag, 
                uint16_t (*getValue)(void))
{
  uint32_t actualValue = 0;  /* Текущее считанное значение */
  uint8_t  i;
  /* Если еще не прошли инициализацию */ 
  if(!(*initFlag))
  {
    /* То нужно собрать первичную выборку */
    for(i = 0; i < bufferSize; i++)
    {	
      /* Собираем первичную выборку */
      buffer[i] = getValue();
      /* Сразу же считаем среднее по всей выборке */
      *SMA = *SMA + (double)(buffer[i])/bufferSize;
    }
    /* Отмечаем, что прошли инициализацию */
    *initFlag = 1;
  }
  /* Инициализацию уже проходили */ 
  else 
  {
    /* Берем новое значение */
    actualValue = getValue();
    /* Фильтруем */ 
    SMAFilterBase (buffer, bufferSize, SMA, actualValue);
  }
}

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
uint32_t medianFilter (const uint8_t frameSize, uint16_t (*getValue)(void))
{
  uint8_t  i;                  /* Счетчик циклов */
  uint32_t buffer[frameSize];  /* Буфер для хранения выборки */
	
  /* Заполняем буфер выборки */
  for (i = 0; i < frameSize; i++)
  {
    buffer[i] = getValue();
  }
  /* Сортируем буфера по возрастанию */
  shakerSort(buffer, frameSize);
  /* Возвращаем медиану выборки */
  return buffer[frameSize/2];
}

/*--------------------------------------------------------------------------------------- Приватные функции модуля --*/
/******************************************************************
    EMAFilterBase - Базовая функция для EMA фильтра
    Параметры:  
                actual   - Текущее значение величины 
                previous - Предыдущее значение величины
                coef     - Коэфициент фильтрации (0..10 000)
    Примечание:
                В данной функции реализован сам алгоритм фильтрации;
                Используется функцией EMAFilter()	 
*******************************************************************/
static double EMAFilterBase (double actual, double previous, uint16_t coef)
{
  double _coef;   /* Коэффициент сглаживания */
  double result;  /* Результат фильтрации */
	
  /* Проверим попадает ли коэффциент в диапазон
  если нет подрезаем, и нормируем */
  if(coef > 10000)
    _coef = 1.0;
  else
    _coef = coef/10000.0;
        
  /* Вычисляем результат */
  result = (_coef*actual + (1.0-_coef)*previous);
  return result;
}

/******************************************************************
    SMAFilterBase - Базовая функция для SMA фильтра
    Параметры:
                frame       - Указатель буфер с выборкой
                frameSize   - Размер выборки
                SMA         - Результат работы фильтра
                actual      - Актуальное значение величины			 
    Примечание:
                В данной функции реализован сам алгоритм фильтрации;
                Используется функцией SMAFilter()
*******************************************************************/
static void SMAFilterBase (uint32_t* frame, uint8_t frameSize, double* SMA, uint32_t actual)
{
  uint8_t  i;          /* Счетчик циклов */
	
  /* Вычитаем из него вклад самого старого ззначения выборки */
  *SMA = *SMA - (double)(frame[frameSize-1])/frameSize;
	
  /* Далее необходимо передвинуть данные выборки */
  for(i = frameSize-1; i > 0; i--)
  {
    frame[i] = frame[i-1];
  }
  /* А в освободившееся место помещаем текущую величину */
  frame[0] = actual;
  /* Добавляем вклад новой величины */
  *SMA = *SMA + (double)(frame[0])/frameSize;
}

/******************************************************************
    shakerSort - Шейкерная сортировка массива величин 
                по увеличению значений (исп. в медианной 
                фильтрации)
    Параметры:  
                buffer  - Указатель на массив  
                samples - Размер массива
    Примечание:
                Используется медианным фультром
*******************************************************************/
static void shakerSort (uint32_t* buffer, uint8_t samples)
{
  uint8_t  i;                     /* Счетчик циклов */
  uint8_t  left = 0;              /* Левая граница сортируемой области массива */
  uint8_t  right = samples - 1;   /* Правая граница сортируемой области массива */
  uint8_t  flag = 1;              /* Флаг наличия перемещений */
  uint32_t temp;                  /* Временное хранение данных */
	
  /* Выполнение цикла пока левая граница не сомкнётся с правой 
  или пока в массиве имеются перемещения */
  while ((left < right) && flag > 0)
  {
    flag = 0;
    /* Двигаемся слева направо */
    for (i = left; i < right; i++)  
    {
      /* Если следующий элемент меньше текущего */
      if (buffer[i]>buffer[i + 1]) 
      {             
        /* Меняем их местами */
        temp = buffer[i];
        buffer[i] = buffer[i + 1];
        buffer[i + 1] = temp;
        /* Делаем отметку о том, что 
        перемещения в этом цикле были */
        flag = 1;      
      }
    }
    /* Cдвигаем правую границу на предыдущий элемент */
    right--; 
    /* Двигаемся справа налево */
    for (i = right; i > left; i--)  
    {
      /* Если предыдущий элемент больше текущего */
      if (buffer[i - 1]>buffer[i]) 
      {            
        /* Меняем их местами */
        temp = buffer[i];
        buffer[i] = buffer[i - 1];
        buffer[i - 1] = temp;
        /* Делаем отметку о том, что 
        перемещения в этом цикле были */
        flag = 1;
      }
    }
  /* Сдвигаем правую границу на следующий элемент */
  left++; 
  }
}
