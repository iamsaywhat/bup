#include "otherlib.h"

#include "MDR32F9Qx_port.h"
#include "MDR32F9Qx_eeprom.h"
#include "MDR32F9Qx_rst_clk.h"
#include "MDR32F9Qx_adc.h"


/********************************************
  Системное время с момента включения, мс
********************************************/
uint64_t systemTime = 0;



/*---------------------------- Приватные функции драйвера ---------------------------------------------------------*/

/**************************************************************************************************************
  ADC_switchChannel - Переключение текущего канала ADCx на Channel
**************************************************************************************************************/
static void ADC_switchChannel (ADCdev ADCx, uint32_t Channel);




/*---------------------------- Публичные функции драйвера ----------------------------------------------------------*/

/****************************************************************************************
  SysTick_initialize - Инициализация SysTick таймера с периодом прерывания 1 мс
  при частоте тактирования 80МГц
****************************************************************************************/
void SysTick_initialize(void)
{
  /* Загружаемое значение 80 000 - 1 = 1 милисекунда */
  SysTick->LOAD = 80000-1;
  /* Начальное значение таймера */
  SysTick->VAL  = 0;
  /* Тактирование от HCLK и включение и разрешение прерываний */
  SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | 
                  SysTick_CTRL_ENABLE_Msk    |
                  SysTick_CTRL_TICKINT_Msk   ;
	/* Системный таймер имеет наивысший приоритер среди прерываний */
  NVIC_SetPriority(SysTick_IRQn, 0);
}

/****************************************************************************************
  SysTick_InterruptFunction - Функция для размещения в функции прерывания SysTick
****************************************************************************************/
void SysTick_InterruptFunction(void)
{
  systemTime++;
}

/****************************************************************************************
  getCurrentSystemTime - Узнать системное время (милисекунды с момента включения)
  Возвращает:
              Системное время в милисекундах с момента включения
****************************************************************************************/
uint64_t getCurrentSystemTime(void)
{
  return systemTime;
}

/****************************************************************************************
  setTimeout - Установить таймаут в милисекундах
  Параметры:
            timeout     - Указатель на структуру
            millisecond - Величина таймаута в милисекундах
****************************************************************************************/
void setTimeout (TimeoutType* timeout, uint32_t millisecond)
{
  timeout->start  = getCurrentSystemTime();
  timeout->stop   = timeout->start + millisecond;
  timeout->status = TIME_IS_NOT_UP;
}

/****************************************************************************************
  timeoutStatus - Проверить истекло ли время ожидания
  Параметры:
            timeout - Указатель на структуру
  Возвращает:
            Статус таймаута
****************************************************************************************/
TimeoutStatus timeoutStatus(TimeoutType* timeout)
{
  uint64_t currentTime = getCurrentSystemTime();
	
  if(timeout->stop > timeout->start)
  {
    if(currentTime > timeout->stop)
    {
      timeout->status = TIME_IS_UP;
      return TIME_IS_UP;
    }
  }
  else
  {
    if((currentTime > timeout->stop) && (currentTime < timeout->start))
    {
      timeout->status = TIME_IS_UP;
      return TIME_IS_UP;
    }
  }
  timeout->status = TIME_IS_NOT_UP;
  return TIME_IS_NOT_UP;
}

/**************************************************************************************************************
  delay_ms - Функция задержки в мс
  Параметры:
            ms - Величина задержки в милисекундах
**************************************************************************************************************/
void delay_ms(uint32_t ms)
{
  uint64_t currentTime = getCurrentSystemTime();
  while(getCurrentSystemTime() < currentTime + ms);
}

/**************************************************************************************************************
    delay_us - Функция задержки в мкс
    Параметры:
                us - Величина задержки в микросекундах
**************************************************************************************************************/
void delay_us(uint32_t us)
{	
  uint32_t      currentValue;    /* Счет таймера, от которого считаем задержку */
  uint32_t      exitValue;       /* Счет таймера на котором нужно выйти */
  const uint8_t tickCount = 50;  /* Число тиков на 1 микросекунду времени */
  /* Вообще 1 микросекунда это 80 тиков, 
  но так как мы тратим так же время на другие операции,
  мы опаздываем примерно на 30 тиков суммарно, поэтому
  компенсируем это */
  for(uint32_t i = 0; i < us; i++)
  {
    currentValue = SysTick->VAL;
    if(currentValue - tickCount > SysTick->LOAD)
      exitValue = SysTick->LOAD - tickCount;
    else
      exitValue = currentValue - tickCount;
			
    if(exitValue > currentValue)
      while(!((SysTick->VAL > currentValue)&&(SysTick->VAL < exitValue)));
    else
      while((SysTick->VAL > exitValue)&&(SysTick->VAL < currentValue));
  }
}

/**************************************************************************************************************
  Pin_initialize - Функция конфигурации отдельных пинов порта
**************************************************************************************************************/
void Pin_initialize(PinConfigType pin)
{
  PORT_InitTypeDef PORT_Struct;
	
  PORT_Struct.PORT_Pin       = pin.pinNum; 
  PORT_Struct.PORT_OE        = pin.direction;
  PORT_Struct.PORT_FUNC      = pin.function;
  PORT_Struct.PORT_MODE      = pin.mode;
	
  if(pin.direction == PORT_OE_IN)
    PORT_Struct.PORT_PULL_DOWN = PORT_PULL_DOWN_ON;
  else
    PORT_Struct.PORT_PULL_DOWN = PORT_PULL_DOWN_OFF;
		
  PORT_Struct.PORT_SPEED     = PORT_SPEED_MAXFAST;
  PORT_Struct.PORT_PULL_UP   = PORT_PULL_UP_OFF;
  PORT_Struct.PORT_GFEN      = PORT_GFEN_OFF;
  PORT_Struct.PORT_PD        = PORT_PD_DRIVER;
  PORT_Struct.PORT_PD_SHM    = PORT_PD_SHM_OFF;
	
  PORT_Init(pin.port, &PORT_Struct);
}

/**************************************************************************************************************
  Pin_deinit - Функция конфигурации пина по-умолчанию
**************************************************************************************************************/
void Pin_default(PinConfigType pin)
{
  PORT_InitTypeDef PORT_Struct;
	
  PORT_Struct.PORT_Pin       = pin.pinNum; 
  PORT_Struct.PORT_OE        = PORT_OE_OUT;
  PORT_Struct.PORT_FUNC      = PORT_FUNC_PORT;
  PORT_Struct.PORT_MODE      = PORT_MODE_DIGITAL;
	
  PORT_Struct.PORT_SPEED     = PORT_SPEED_MAXFAST;
  PORT_Struct.PORT_PULL_DOWN = PORT_PULL_DOWN_OFF;
  PORT_Struct.PORT_PULL_UP   = PORT_PULL_UP_OFF;
  PORT_Struct.PORT_GFEN      = PORT_GFEN_OFF;
  PORT_Struct.PORT_PD        = PORT_PD_DRIVER;
  PORT_Struct.PORT_PD_SHM    = PORT_PD_SHM_OFF;
	
  PORT_Init(pin.port, &PORT_Struct);
}

/**************************************************************************************************************
  Pin_set - Установить выход пина в 1
**************************************************************************************************************/
void Pin_set(PinConfigType pin)
{
  PORT_SetBits(pin.port, pin.pinNum); 
} 

/**************************************************************************************************************
  Pin_reset - Установить выход пина в 0
**************************************************************************************************************/
void Pin_reset(PinConfigType pin)
{
  PORT_ResetBits(pin.port, pin.pinNum);
}

/**************************************************************************************************************
  Pin_read - Чтение состояния входа пина
**************************************************************************************************************/
uint8_t Pin_read(PinConfigType pin)
{
  return PORT_ReadInputDataBit(pin.port, pin.pinNum);
}




/**************************************************************************************************************
  Timer_initialize - Настройка таймера TIMERx на прерывание каждые ticks тактов.
**************************************************************************************************************/
void Timer_initialize (MDR_TIMER_TypeDef* TIMERx, unsigned long ticks)
{
  /* 
  Фoрмула расчета частоты прерываний:
  F_interrupt = (Fcpu / TIMER_HCLKdivX)/((Prescaller + 1)*(Period + 1)); 
  */
  unsigned short Prescaler;
  unsigned short Period;	
  /* Проверяем хватит ли нам только Period, для такого интервала */
  if(ticks > 65535)
  {
    /* Не хватит, используем предделитель
    Необходимо определить на сколько нужно поделить частоту и округлить в большую сторону (+1) */
    Prescaler = (unsigned short)((ticks/65535.0) + 1);
    /* Остальное необходимо сгрузить в Period */
    Period = (unsigned short)(ticks/Prescaler); 
  }
  else 
  { 
    /* Хватит, поэтому можем использовать только Period */
    Prescaler = 1;
    Period = ticks;
  }
	
  TIMER_CntInitTypeDef  sTIMER_CntInit;
  
  /* Сбрасываем настройки таймера */
  TIMER_DeInit(TIMERx);	  

  /* Предделитель на входе таймера */
  /* TIMER_HCLKdivX Фиксируем на TIMER_HCLKdiv1 */                                                                     
  TIMER_BRGInit(TIMERx,TIMER_HCLKdiv1);
	
  sTIMER_CntInit.TIMER_IniCounter       = 0;                            /* Начальное значние счетчика */
  sTIMER_CntInit.TIMER_Prescaler        = Prescaler - 1;                /* Пределитель */
  sTIMER_CntInit.TIMER_Period           = Period - 1;                   /* Период (значение до которого CNT будет досчитывать 
	                                                                         и генерировать прерывание) */
  sTIMER_CntInit.TIMER_CounterMode      = TIMER_CntMode_ClkFixedDir;    /* Направление счета фиксорованное */
  sTIMER_CntInit.TIMER_CounterDirection = TIMER_CntDir_Up;              /* Направление счета вверх */
  sTIMER_CntInit.TIMER_EventSource      = TIMER_EvSrc_None;             /* Cобытие переднего фронта на TIM_CLK */
  sTIMER_CntInit.TIMER_FilterSampling   = TIMER_FDTS_TIMER_CLK_div_1; 	/* Частота семплирования данных FDTS: 00 – каждый TIM_CLK; */
  sTIMER_CntInit.TIMER_ARR_UpdateMode   = TIMER_ARR_Update_Immediately; /* Разрешение мгновенного обновления ARR: 
	                                                                         0 – ARR будет перезаписан в момент записи в ARR; */
  /* Инициализируем таймер данной структурой */
  TIMER_CntInit (TIMERx,&sTIMER_CntInit);  	                            
	
  /* Разрешаем прерывания внутри таймера по достижению CNT значения ARR (Period) */
  TIMER_ITConfig(TIMERx,TIMER_STATUS_CNT_ARR,ENABLE);
	
  /* Разрешаем прерывания по достижению CNT значения ARR (Period) на уровне NVIC */
  /* Приоритет у обычных таймеров в системе наивысший после SysTick */
  if(TIMERx == MDR_TIMER1)
  {
    NVIC_EnableIRQ(Timer1_IRQn);
    NVIC_SetPriority(Timer1_IRQn, 1);
  }
  else if (TIMERx == MDR_TIMER2)
  {
    NVIC_EnableIRQ(Timer2_IRQn);
    NVIC_SetPriority(Timer2_IRQn, 1);
  }
  else if (TIMERx == MDR_TIMER3)
  {
    NVIC_EnableIRQ(Timer3_IRQn);
    NVIC_SetPriority(Timer3_IRQn, 1);
  }

  /* Включаем таймер */
  TIMER_Cmd(TIMERx,ENABLE);
}


/**************************************************************************************************************
  Clock_initialize - Настройка тактирования процессора
  Примечание: Текущая конфигурация соответствует 80 МГц
**************************************************************************************************************/
void Clock_initialize(void)
{
  /* 8MHZ на отладочной плате */  
  RST_CLK_DeInit();                                /* Настройки по-умолчанию */
  RST_CLK_HSEconfig(RST_CLK_HSE_ON);               /* Запускаем HSE */
  RST_CLK_HSEstatus();                             /* Ожидаем готовности HSE */
  RST_CLK_HSEclkPrescaler (RST_CLK_HSEclkDIV1);    /* Предделитель CPU_C1 = HSE */
  RST_CLK_CPU_PLLconfig(RST_CLK_CPU_PLLsrcHSEdiv1, /* Конфигурация PLL (на выходе PLL = HSE*10) */
                        RST_CLK_CPU_PLLmul10);     
  RST_CLK_CPU_PLLcmd(ENABLE);                      /* Включаем PLL */
  RST_CLK_CPU_PLLstatus ();                        /* Проверяем готов ли PLL */
  RST_CLK_CPUclkPrescaler(RST_CLK_CPUclkDIV1);     /* CPU_C3 = CPU_C2/1 */
  RST_CLK_CPU_PLLuse(ENABLE);                      /* CPU_C2 = PLLCPU */
  RST_CLK_CPUclkSelection(RST_CLK_CPUclkCPU_C3);   /* Переключаем таксирование процессора на CPU_C3 = 80MHZ */
  RST_CLK_PCLKcmd(RST_CLK_PCLK_EEPROM, ENABLE);    /* Разрешаем тактирование EEPROM */
  EEPROM_SetLatency(EEPROM_Latency_3);             /* Выставляем задержку памяти программ при чтении = 3 цикла (до 80 МГц) */
  RST_CLK_PCLKcmd(RST_CLK_PCLK_EEPROM, DISABLE);   /* Отключаем тактирование EEPROM */
    
  /* Разрешение тактирования перечисленной периферии */
  RST_CLK_PCLKcmd(RST_CLK_PCLK_UART2|
                  RST_CLK_PCLK_UART1|
                  RST_CLK_PCLK_ADC  |
                  RST_CLK_PCLK_PORTA|
                  RST_CLK_PCLK_PORTB|
                  RST_CLK_PCLK_PORTC|
                  RST_CLK_PCLK_PORTD|
                  RST_CLK_PCLK_PORTF|
                  RST_CLK_PCLK_PORTE|
                  RST_CLK_PCLK_TIMER3|
                  RST_CLK_PCLK_CAN1|
                  RST_CLK_PCLK_CAN2|
                  RST_CLK_PCLK_TIMER2|
                  RST_CLK_PCLK_TIMER1|
                  RST_CLK_PCLK_SSP1  |
                  RST_CLK_PCLK_SSP2, 
                  ENABLE); 
  /* Обновим значение тактовой частоты */
  SystemCoreClockUpdate();
}


/**************************************************************************************************************
  ADC_initialize - Инициализация АЦП
  Параметры:
            ADCx - Используемый АЦП (ADC1/ADC2)
**************************************************************************************************************/
void ADC_initialize(ADCdev ADCx)
{
  ADC_InitTypeDef  sADC;          /* Общие настройки АЦП */
  ADCx_InitTypeDef sADCx;         /* Настройки одного из АЦП */
	
  /* Настройки тактирования ADC на шине */
  RST_CLK_ADCclkSelection(RST_CLK_ADCclkCPU_C1);       /* ADC_C2 тактируется от CPU_C1 */
  RST_CLK_ADCclkPrescaler(RST_CLK_ADCclkDIV1);         /* ADC_C3 = ADC_C2/1 */
  RST_CLK_ADCclkEnable(ENABLE);                        /* Разрешаем тактирование ADC */

  /* Общие настройки АЦП */
  ADC_DeInit();                                                        /* Сбрасываем настройки АЦП */
  ADC_StructInit(&sADC);                                               /* Заполняем структуру инициализации по-умолчанию */
  sADC.ADC_SynchronousMode      =  ADC_SyncMode_Independent;           /* АЦП 1 и 2 работают независимо (асинхронно) друг от друга */
  sADC.ADC_StartDelay           =  0;                                  /* Задержка синхронного пуска двух АЦП (у нас асинхронная работа) */	
  sADC.ADC_TempSensor           =  ADC_TEMP_SENSOR_Enable;             /* Для работы температурный датчик - не используем: Бит TS_EN */
  sADC.ADC_TempSensorAmplifier  =  ADC_TEMP_SENSOR_AMPLIFIER_Disable;  /* Запрет работы выходного усилителя: Бит TS_BUF*/
  sADC.ADC_TempSensorConversion =  ADC_TEMP_SENSOR_CONVERSION_Disable; /* Запрет выбора датчика температуры: SEL_TS */
  sADC.ADC_IntVRefConversion    =  ADC_VREF_CONVERSION_Disable;        /* Запрет выбора источника опорного напряжения для оцифровки: бит SEL_VREF */
  sADC.ADC_IntVRefTrimming      =  0;                                  /* Подстройка внутреннего опорного напряжения (от 0 до 7) */
  ADC_Init (&sADC);                                                    /* Инициализируем АЦП (общие настройки) */
	
  /* Настройки одного из АЦП */
  ADCx_StructInit (&sADCx);                                   /* Структура заполняется по-умолчанию */
  sADCx.ADC_ClockSource      = ADC_CLOCK_SOURCE_ADC;          /* Источник тактирования ADC_CLOCK = ADC_C3 = CPU_C1 = HSE = 8 МГц */
  sADCx.ADC_SamplingMode     = ADC_SAMPLING_MODE_SINGLE_CONV; /* Одиночное преобразование */
  sADCx.ADC_ChannelSwitching = ADC_CH_SWITCHING_Disable;      /* Переключение между каналами запрещено, используется только выбранный канал */
  sADCx.ADC_ChannelNumber    = 0;                             /* Номер канала: можно было бы сразу тут выбрать канал, например, ADC_CH_ADC1
                                                                 но выбирать канал будем в функции запроса результата, поэтому безразлично */
  sADCx.ADC_Channels         = 0;                             /* Используемые каналы АЦП (переключение каналов отключено, 
                                                                 а мы явно указали нужный канал выше, поэтому 0) */
  sADCx.ADC_LevelControl     = ADC_LEVEL_CONTROL_Disable;     /* Контроль уровня отключен */
  sADCx.ADC_LowLevel         = 0;                             /* Границы уровня = 0, тк контроль уровня отключен */
  sADCx.ADC_HighLevel        = 0;                             /* Границы уровня = 0, тк контроль уровня отключен */
  sADCx.ADC_VRefSource       = ADC_VREF_SOURCE_INTERNAL;      /* Выбор опорного напряжения - внутренний AUCC и AGND */
  sADCx.ADC_IntVRefSource    = ADC_INT_VREF_SOURCE_EXACT;     /* Внутренний источник опорного напряжения с температурной компенсацией */
  sADCx.ADC_Prescaler        = ADC_CLK_div_None;              /* Предделитель тактовой частоты АЦП не используется */
  sADCx.ADC_DelayGo          = 0x07;                          /* Дополнительная задержка перед началом преобразования */
	
  /* Выбор АЦП */
  switch(ADCx)
  {	
    case ADC1:
      ADC1_Init (&sADCx);   /* Инициализируем АЦП1 */
      ADC1_Cmd (ENABLE);    /* Запускаем АЦП */
      break;
    case ADC2:
      ADC2_Init (&sADCx);   /* Инициализируем АЦП2 */
      ADC2_Cmd (ENABLE);    /* Запускаем АЦП */
      break;
    default:
      return;
  }
}


/**************************************************************************************************************
  ADC_getResult - Получить результат преобразования АЦП
  Параметры:
            ADCx    - Используемый АЦП (ADC1/ADC2)
            Channel - Номер канала (ADC_CH_ADCx, где x = 0..15)
**************************************************************************************************************/
uint16_t ADC_getResult (ADCdev ADCx, uint32_t Channel)
{
  TimeoutType timeout;     /* Таймаут на преобразование */
  uint16_t result  = 0;    /* Результат преобразований */
	
  /* Переключаем канал на тот, с котого хотим считать данные */
  ADC_switchChannel(ADCx, Channel);
	
  /* Выбор АЦП */
  switch(ADCx)
  {	
    case ADC1:
      /* Запускаем преобразование */
      ADC1_Start ();
      /* Ожидаем окончание преобразования */
      setTimeout (&timeout, 1);
      while ((ADC1_GetStatus() & ADC_STATUS_FLG_REG_EOCIF) == 0 && (timeoutStatus(&timeout) != TIME_IS_UP));
      /* Выделяем значащие разряды (до 12 го) и возвращаем значение */
      result = ADC1_GetResult () & 0xFFF;
      break;
    case ADC2:
      /* Запускаем преобразование */
      ADC2_Start ();
      /* Ожидаем окончание преобразования */
      setTimeout (&timeout, 1);
      while ((ADC2_GetStatus() & ADC_STATUS_FLG_REG_EOCIF) == 0 && (timeoutStatus(&timeout) != TIME_IS_UP));
      /* Выделяем значащие разряды (до 12 го) и возвращаем значение */
      result = ADC2_GetResult () & 0xFFF;
      break;
  default:
    result = 0;
    break;
  }
  return result;
}


/**************************************************************************************************************
  ADC_switchChannel - Переключение текущего канала ADCx на Channel
**************************************************************************************************************/
static void ADC_switchChannel (ADCdev ADCx, uint32_t Channel)
{
  uint32_t Temp; // Временное хранение регистра
	
  /* Выбор АЦП */
  switch(ADCx)
  {	
    case ADC1:
      Temp = MDR_ADC->ADC1_CFG;                  /* Сохраняем текущие настройки АЦП */
      Temp &= ~(ADC1_CFG_REG_CHS_Msk);           /* Сбрасываем установленные биты выбора канала*/
      Temp += Channel << ADC1_CFG_REG_CHS_Pos;   /* Устанавливаем нужный канал */
      MDR_ADC->ADC1_CFG = Temp;                  /* Сохраняем регистр с новым выбором канала */
      break;
    case ADC2:
      Temp = MDR_ADC->ADC2_CFG;                  /* Сохраняем текущие настройки АЦП */
      Temp &= ~(ADC2_CFG_REG_CHS_Msk);           /* Сбрасываем установленные биты выбора канала*/
      Temp += Channel << ADC2_CFG_REG_CHS_Pos;   /* Устанавливаем нужный канал */
      MDR_ADC->ADC2_CFG = Temp;                  /* Сохраняем регистр с новым выбором канала */
      break;
    default:
      /*  */
      break;
  }
}

