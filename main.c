#include "SWS.h"
#include "SNS.h"
#include "BIM.h"
#include "1636PP52Y.h"
#include "stdint.h"
#include "otherlib.h"
#include "ZPZ.h"
#include "crc16.h"
#include "25Q64FV.h"
#include "Log_FS/Log_FS.h"
#include "RetargetPrintf/RetargetPrintf.h"
#include "discrete_io.h"
#include "Math_model/M_Model.h"
#include "HeightMap/Heightmap_conf_and_pd.h"
#include "debug.h"
#include "SelfTesting.h"


	double temp = 0;
	int16_t temp2 = 0;
	uint8_t buff[200];
  long long cnt = 0;
	

	char CHM = 0;
	double iii = 0;
	short pp = 0;


int main(void)
{	 
	uint32_t i = 0;                                           // Счетчик циклов
	uint32_t timeout = 0;                                     // Таймаут-счетчик
	SNS_Orientation_Data_Response_Union  SNS_Orientation;     // Данные ориентации от СНС
	SNS_Position_Data_Response_Union     SNS_Position;        // Данные местоположения от СНС
	SWS_Packet_Type_Union                SWS_Data;            // Данные от СВС
	
	// Настраиваем тактовую частоту процессора	
	InitCLK();
	// Конфигурируем дискреты, при этом все реле гарантированно отключаются
	Discrete_RetargetPins();
	// Запускаем драйвер памяти для полетного задания
	SPI_1636PP52Y_RetargetPins();
	SPI_1636PP52Y_Init();	
	// Запускаем драйвер внешней памяти "Черного ящика"
	SPI_25Q64FV_RetargetPins();
  SPI_25Q64FV_Init();
	// Запускаем драйвер БИМов (который попутно инициализирует CAN1)
	BIM_RetargetPins();
	BIM_CAN_init ();
	// Запускаем файловую систему
	LogFs_Info();
	// Запускаем диагностику системы
	SelfTestingFull();
	
	
//// Тестирование файловой системы с логов
//	for(i=0; i < 200; i++)
//	{
//		buff[i] = i;
//	}
//	LogFs_Formatting();
//	
//	LogFs_Info();
//	LogFs_CreateFile();
//	LogFs_WriteToCurrentFile(buff, 8);
//	
//	printf_switcher(TO_LOG,0);
//  printf("Hello world!\n");
//	
//	LogFs_Info();
//	LogFs_CreateFile();
//	LogFs_WriteToCurrentFile(buff, 50);
//	
//	LogFs_Info();
//	LogFs_CreateFile();
//	LogFs_WriteToCurrentFile(buff, 200);
//	
//	LogFs_Info();
//	LogFs_CreateFile();
//	LogFs_WriteToCurrentFile(buff, 2);
//		
//	for(i=0; i < 200; i++)
//	{
//		buff[i] = 0;
//	}
//	
//	LogFs_Info();
//	Log_Fs_FindFIle(FIRST_FILE);
//	temp = Log_Fs_GetFileProperties(FILE_SIZE);
//	LogFs_ReadFile(buff, temp);
//	
//	for(i=0; i < 200; i++)
//	{
//		buff[i] = 0;
//	}
//	
//	Log_Fs_FindFIle(NEXT_FILE);
//	temp = Log_Fs_GetFileProperties(FILE_SIZE);
//	LogFs_ReadFile(buff, temp);
//	
//		
//	for(i=0; i < 200; i++)
//	{
//		buff[i] = 0;
//	}
//	
//	Log_Fs_FindFIle(NEXT_FILE);
//	temp = Log_Fs_GetFileProperties(FILE_SIZE);
//	LogFs_ReadFile(buff, temp);
//			
//	for(i=0; i < 200; i++)
//	{
//		buff[i] = 0;
//	}
//	
//	Log_Fs_FindFIle(NEXT_FILE);
//	temp = Log_Fs_GetFileProperties(FILE_SIZE);
//	temp = LogFs_ReadFile(buff, temp);
//	temp = LogFs_ReadFile(buff, temp);

//	
//	//создаем огромный файл
//	LogFs_Info();
//	LogFs_CreateFile();
//	LogFs_WriteToCurrentFile(buff, 2);
//		
//	for(i=0; i < 200; i++)
//	{ 
//		buff[i] = 0;
//	}


	// Проверяем подключение разъема ЗПЗ
	if(!CONNECT_ZPZ_CHECK)
	{
		// Разъем подключен
		// Запускаем драйвер ЗПЗ
		ZPZ_init();
		ZPZ_RetargetPins ();
		// Запускаем световую индикацию режима
		Timer_SetInterruptPeriod (MDR_TIMER2, 0.5*SECOND_TICKS);  // Сделать макросом?
		// Для тестирования БИМов подадим на них питание
		BIM_Supply_ON();
		// И уходим на обслуживание режима ЗПЗ
		while(!CONNECT_ZPZ_CHECK)
		{
			ZPZ_Service();
		}
	}
	// Проверим состояние системы:
	// 1. Обе шпильки должны быть вставлены
	// 2. Работу внешних SPI-flash
	// 3. Загружена ли карта рельефа и полетное задание
	// 4. Работает ли файловая система
	if(SelfTesting_STATUS(ST_pin1)!=ST_OK      || 
		 SelfTesting_STATUS(ST_pin2)!=ST_OK      || 
	   SelfTesting_STATUS(ST_MAP)!=ST_OK       || 
	   SelfTesting_STATUS(ST_1636PP52Y)!=ST_OK ||
	   SelfTesting_STATUS(ST_25Q64FV)!=ST_OK   ||
	   SelfTesting_STATUS(ST_LogFS)!=ST_OK)
	{
		// Если не вставлены, гасим "Готовность", "Неисправность" зажигаем
		// Гасим "Готовность"
		LED_READY_OFF();
		// Зажигаем "Неисправность"
		LED_FAULT_ON();
		// Зависаем в ожидании перезапуска
		while(1);
	}

	// Зажигаю готовность
	LED_READY_ON();
	// Ждем пока стабилизирующий парашют выбросится и извлечет шпильку 1
	while(SelfTesting_PIN1());
	// Выжидаем 10 секунд
	delay_us(10000000);
	// Поджигаем пиропатрон
	PYRO_ON();
	// Пиропатрон включаем только на 3 сек - но тупо ждать нельзя ------- переделать! ***************************  FIX_ME!!!!!!!!
	delay_us(3000000);
	
	// Ждем выброс основного парашюта - извлечения Шпильки 2
	while(SelfTesting_PIN2());
	// Ждем 6 секунд 
	delay_us(6000000);
	// Включаем питание БИМов
	BIM_Supply_ON();
	// Нужно проверить исправность БИМов
	while (SelfTesting_RIGHT_BIM()!= ST_OK || SelfTesting_LEFT_BIM()!= ST_OK)
	{
		// Если хотя бы один из БИМов не отвечает, передергиваем питание и опять проверяем
		// Все 3 раза попытаемся перезапустистить БИМы
		BIM_Supply_OFF();
		delay_us(1000000);
		BIM_Supply_ON();
		delay_us(1000000);
		i++;
		if(i > 3)
		{
			LED_READY_OFF();
			LED_FAULT_ON();
			while(1);
		}
	}
	// Запустим быструю диагностику системы перед запуском управления
	SelfTestingOnline();
	// Инициализируем мат. модель полета
	M_Model_Init();
	// Сообщаем мат.модели о необходимости обновить данные
	M_Model_Need2UpdateSet();
	// К полету готовы, запускаем таймер обслуживания мат модели
	Timer_SetInterruptPeriod (MDR_TIMER1, SECOND_TICKS);  // Сделать макросом?
	
	// Главный рабочий цикл
	while(1)
	{
		// Просит ли матмодель обновить данные?
		if(M_Model_Need2UpdateCheck() == 1)
		{
			// Да, просит. Начинаем готовить данные для матмодели
			printf_switcher(TO_CAN, 0x400);
      printf("Hello world!\n");
			
			// Подлючаемся к СНС
			SNS_RetargetPins();
			SNS_init();

			// Сбросим таймаут
			timeout = 0;
			// Запрашиваем данные местоположения с контролем таймаута (спрашиваем не более 20 раз подряд)
			while(!SNS_GetPositionData(&SNS_Position) && (timeout != 20)) timeout ++;	
			// Сбросим таймаут
			timeout = 0;
			// Запрашиваем данные ориентации с контролем таймаута (спрашиваем не более 20 раз подряд)
			while(!SNS_GetOrientationData(&SNS_Orientation) && (timeout != 20)) timeout ++;
			// Отключаемся от СНС и освобождаем UART порт
			SNS_deinit();
			
			// Подключаемся к СВС
			SWS_RetargetPins();
			SWS_init();
			// Сбросим таймаут
			timeout = 0;
			// Пытаемся получить данные от СВС, не более 20 попыток
			while(SWS_GetPacket (&SWS_Data) && (timeout != 10)) timeout ++;
			// Отключаемся от СВС и освобождаем UART порт
			SWS_deinit ();
						
			// Отправляем данные математической модели
			M_Model_PrepareData (SNS_Position.Struct, SNS_Orientation.Struct, SWS_Data.Struct);
			
			// Запустим быструю диагностику системы
			SelfTestingOnline();
		}		
	}
}
