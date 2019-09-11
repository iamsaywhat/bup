#include "otherlib.h"

#include "MDR32F9Qx_eeprom.h"
#include "MDR32F9Qx_rst_clk.h"
#include "MDR32F9Qx_adc.h"


/**************************************************************************************************************
    delay_us - Функция задержки в мкс.
**************************************************************************************************************/
void delay_us(uint32_t us)
{
	uint32_t i;
	// Загружаемое значение 79 = 1 микросекунда
	SysTick->LOAD = 79;
	// Начальное значение таймера
	SysTick->VAL  = 0;
	// Тактирование от HCLK и включение
	SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk;
	for(i = 0; i < us; i++)
	{
		while (!(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk)); 	
	}
	// Выключаем таймер
	SysTick->CTRL = 0;
}


/**************************************************************************************************************
    Pin_Init - Функция конфигурации отдельных пинов порта
**************************************************************************************************************/
void Pin_Init(MDR_PORT_TypeDef* PORTx, uint32_t Pin_Num, PORT_FUNC_TypeDef Port_Func, PORT_OE_TypeDef Port_OE)
{
	PORT_InitTypeDef PORT_Struct;
	
	PORT_Struct.PORT_Pin = Pin_Num; 
	PORT_Struct.PORT_OE = Port_OE;
	PORT_Struct.PORT_FUNC = Port_Func;
	PORT_Struct.PORT_SPEED = PORT_SPEED_MAXFAST;
	PORT_Struct.PORT_MODE = PORT_MODE_DIGITAL;
	PORT_Struct.PORT_PULL_DOWN=PORT_PULL_DOWN_OFF;
	PORT_Struct.PORT_PULL_UP=PORT_PULL_UP_OFF;
	PORT_Struct.PORT_GFEN = PORT_GFEN_OFF;
	PORT_Struct.PORT_PD = PORT_PD_DRIVER;
	PORT_Struct.PORT_PD_SHM = PORT_PD_SHM_OFF;
	
	PORT_Init(PORTx,&PORT_Struct);
}



/**************************************************************************************************************
    Timer_SetInterruptPeriod - Настройка таймера TIMERx на прерывание каждые ticks тактов.
**************************************************************************************************************/
void Timer_SetInterruptPeriod (MDR_TIMER_TypeDef* TIMERx, unsigned long ticks)
{
	// Фoрмула расчета частоты прерываний:
	// F_interrupt = (Fcpu / TIMER_HCLKdivX)/((Prescaller + 1)*(Period + 1));
	unsigned short Prescaler;
	unsigned short Period;	
	// Проверяем хватит ли нам только Period, для такого интервала
	if(ticks > 65535)
	{
		// Не хватит, используем предделитель
		// Необходимо определить на сколько нужно поделить частоту и округлить в большую сторону (+1)
		Prescaler = (unsigned short)((ticks/65535.0) + 1);
		// Остальное необходимо сгрузить в Period
		Period = (unsigned short)(ticks/Prescaler); 
	}
	else 
	{ 
		// Хватит, поэтому можем использовать только Period
		Prescaler = 1;
		Period = ticks;
	}
	TIMER_CntInitTypeDef  sTIMER_CntInit;
	// Сбрасываем настройки таймера
	TIMER_DeInit(TIMERx);
	// Предделитель на входе таймера
	// TIMER_HCLKdivX Фиксируем на TIMER_HCLKdiv1
	TIMER_BRGInit(TIMERx,TIMER_HCLKdiv1);
	// Начальное значние счетчика
	sTIMER_CntInit.TIMER_IniCounter          = 0;
	// Пределитель
	sTIMER_CntInit.TIMER_Prescaler           = Prescaler - 1;
	// Период (значение до которого CNT будет досчитывать и генерировать прерывание
	sTIMER_CntInit.TIMER_Period              = Period - 1;
	// Направление счета фиксорованное
	sTIMER_CntInit.TIMER_CounterMode         = TIMER_CntMode_ClkFixedDir; 
	// Направление счета вверх
	sTIMER_CntInit.TIMER_CounterDirection    = TIMER_CntDir_Up; 
	// Cобытие переднего фронта на TIM_CLK 
	sTIMER_CntInit.TIMER_EventSource         = TIMER_EvSrc_None;
	// Частота семплирования данных FDTS: 00 – каждый TIM_CLK;
	sTIMER_CntInit.TIMER_FilterSampling      = TIMER_FDTS_TIMER_CLK_div_1;
	// Разрешение мгновенного обновления ARR: 0 – ARR будет перезаписан в момент записи в ARR;
	sTIMER_CntInit.TIMER_ARR_UpdateMode      = TIMER_ARR_Update_Immediately;
	// Инициализируем таймер данной структурой
	TIMER_CntInit (TIMERx,&sTIMER_CntInit);
	// Разрешаем прерывания внутри таймера по достижению CNT значения ARR (Period)
	TIMER_ITConfig(TIMERx,TIMER_STATUS_CNT_ARR,ENABLE);
	// Разрешаем прерывания по достижению CNT значения ARR (Period) на уровне NVIC
	if(TIMERx == MDR_TIMER1)
	{
		NVIC_EnableIRQ(Timer1_IRQn);
	}
	else if (TIMERx == MDR_TIMER2)
	{
		NVIC_EnableIRQ(Timer2_IRQn);
	}
	else if (TIMERx == MDR_TIMER3)
	{
		NVIC_EnableIRQ(Timer3_IRQn);
	}
	// Включаем таймер
	TIMER_Cmd(TIMERx,ENABLE);
}


/**************************************************************************************************************
    InitCLK - Настройка тактирования процессора
    Примечание: Текущая конфигурация соответствует 80 МГц
**************************************************************************************************************/
void InitCLK(void){

    // 8MHZ на отладночной плате 	
    // Настройки по-умолчанию
    RST_CLK_DeInit();
    // Запускаем HSE
    RST_CLK_HSEconfig(RST_CLK_HSE_ON);
    // Ожидаем готовности HSE
    RST_CLK_HSEstatus();
    // Предделитель CPU_C1 = HSE
    RST_CLK_HSEclkPrescaler (RST_CLK_HSEclkDIV1);
    // Конфигурация PLL (на выходе PLL = HSE*10)
    RST_CLK_CPU_PLLconfig(RST_CLK_CPU_PLLsrcHSEdiv1,RST_CLK_CPU_PLLmul10);
    // Включаем PLL
    RST_CLK_CPU_PLLcmd(ENABLE);
    // Проверяем готов ли PLL
    RST_CLK_CPU_PLLstatus ();
    // CPU_C3 = CPU_C2/1
    RST_CLK_CPUclkPrescaler(RST_CLK_CPUclkDIV1);
    // CPU_C2 = PLLCPU
    RST_CLK_CPU_PLLuse(ENABLE);
    // Переключаем таксирование процессора на CPU_C3 = 80MHZ
    RST_CLK_CPUclkSelection(RST_CLK_CPUclkCPU_C3);
    // Разрешаем тактирование EEPROM
    RST_CLK_PCLKcmd(RST_CLK_PCLK_EEPROM, ENABLE);
    // Выставляем задержку памяти программ при чтении = 3 цикла (до 80 МГц)
    EEPROM_SetLatency(EEPROM_Latency_3);
    // Отключаем тактирование EEPROM
    RST_CLK_PCLKcmd(RST_CLK_PCLK_EEPROM, DISABLE);
    // Настройки тактирования ADC
    // ADC_C2 тактируется от CPU_C1
    RST_CLK_ADCclkSelection(RST_CLK_ADCclkCPU_C1);
    // ADC_C3 = ADC_C2/1 
    RST_CLK_ADCclkPrescaler(RST_CLK_ADCclkDIV1);
    // Разрешаем тактирование ADC
    RST_CLK_ADCclkEnable(ENABLE);
			
    // Разрешение тактирования перечисленной периферии
    RST_CLK_PCLKcmd( RST_CLK_PCLK_UART2|
                     RST_CLK_PCLK_UART1|
                     RST_CLK_PCLK_ADC  |
                     RST_CLK_PCLK_PORTA|
                     RST_CLK_PCLK_PORTB|
                     RST_CLK_PCLK_PORTC|
                     RST_CLK_PCLK_PORTD|
                     RST_CLK_PCLK_PORTF|
                     RST_CLK_PCLK_PORTE|
                     // RST_CLK_PCLK_TIMER3|
                     RST_CLK_PCLK_CAN1|
                     RST_CLK_PCLK_CAN2|
                     RST_CLK_PCLK_TIMER2|
                     RST_CLK_PCLK_TIMER1, 
                     ENABLE); 
    // Обновим значение тактовой частоты
    SystemCoreClockUpdate();
}


/**************************************************************************************************************
    ADC_init - Инициализация АЦП
    Примечание: Необходимо доделать, как возникнет необходимость
**************************************************************************************************************/
void ADC_init(void)
{
	PORT_InitTypeDef PORT_Struct;
	ADC_InitTypeDef  sADC;
	ADCx_InitTypeDef sADCx;

	// Настройка пина PORTD.9 как аналоговый вход
	PORT_StructInit(&PORT_Struct);
	PORT_Struct.PORT_Pin          = PORT_Pin_9 || PORT_Pin_10 || PORT_Pin_11 || PORT_Pin_12;
	PORT_Struct.PORT_OE           = PORT_OE_IN;
	PORT_Struct.PORT_MODE         = PORT_MODE_ANALOG;
	PORT_Init(MDR_PORTD, &PORT_Struct);
	// Сбрасываем настройки АЦП	
	ADC_DeInit();
	// Заполняем структуру инициализации по-умолчанию
	ADC_StructInit(&sADC);
	
	// АЦП 1 и 2 работают независимо друг от друга
	sADC.ADC_SynchronousMode      =  ADC_SyncMode_Independent;
	// Задержка синхронного пуска АЦП (у нас не синхронная работа)
	sADC.ADC_StartDelay           =  0;
	// Для работы температурного датчика - не используем
	sADC.ADC_TempSensor           =  ADC_TEMP_SENSOR_Disable;
	sADC.ADC_TempSensorAmplifier  =  ADC_TEMP_SENSOR_AMPLIFIER_Disable;
	sADC.ADC_TempSensorConversion =  ADC_TEMP_SENSOR_CONVERSION_Disable;
	// Включение датчика внутреннего опорного напряжения (можно измерять на 30 канале АЦП)
	sADC.ADC_IntVRefConversion    =  ADC_VREF_CONVERSION_Disable;
	// Подстройка внутреннего опорного напряжения (от 0 до 7)
	sADC.ADC_IntVRefTrimming      =  0;
	// Инициализируем АЦП (общие настройки)
	ADC_Init (&sADC);
	
	// Структура заполняется по-умолчанию
	ADCx_StructInit (&sADCx);
	// Источник тактирования ADC_CLOCK = ADC_C3 = CPU_C1 = HSE = 8 МГц
	sADCx.ADC_ClockSource      = ADC_CLOCK_SOURCE_ADC;
	// Одиночное преобразование
	sADCx.ADC_SamplingMode     = ADC_SAMPLING_MODE_SINGLE_CONV;
	// Переключение между каналами запрещено, используется только выбранный канал
	sADCx.ADC_ChannelSwitching = ADC_CH_SWITCHING_Disable;
	// Номер канала 9 - PORTD.9
	sADCx.ADC_ChannelNumber    = ADC_CH_ADC9;
	// Используемые каналы АЦП (переключение каналов отключено, а мы явно указали нужный канал выше, поэтому 0)
	sADCx.ADC_Channels         =  0;
	
	// Контроль уровня отключен
	sADCx.ADC_LevelControl     = ADC_LEVEL_CONTROL_Disable;
	// Границы уровня 0, тк контроль уронвня отключен
	sADCx.ADC_LowLevel         = 0;
	sADCx.ADC_HighLevel        = 0;
	// Выбор опорного напряжения - внутренний AUCC и AGND
	sADCx.ADC_VRefSource       = ADC_VREF_SOURCE_INTERNAL;
	// Внутренний источник опорного напряжения с температурной компенсацией
	sADCx.ADC_IntVRefSource    = ADC_INT_VREF_SOURCE_EXACT;
	
	// Предделитель тактовой частоты АЦП не используется
	sADCx.ADC_Prescaler        = ADC_CLK_div_None;
	// Дополнительная задержка перед началом преобразования после выбора канала
	sADCx.ADC_DelayGo          = 7;
	// Инициализируем АЦП1
	ADC1_Init (&sADCx);
	// Запускаем АЦП
	ADC1_Cmd (ENABLE);
}	

/**************************************************************************************************************
    GetBatteryCharge - Получить заряд батареи
    Примечание: Необходимо доделать, как возникнет необходимость
**************************************************************************************************************/
uint16_t GetBatteryCharge (void)
{
	uint32_t timeout = 0;    // Таймаут на преобразование
	
	// Запускаем преобразование
	ADC1_Start ();
	// Ожидаем окончание преобразования
	while ((ADC1_GetStatus() & ADC_STATUS_FLG_REG_EOCIF) == 0 && timeout < 0xFFF) timeout++;
	// Выделяем значащие разряды и возвращаем значение
	return ADC1_GetResult () & 0xFFF;
}

