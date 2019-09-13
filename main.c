#include <stdint.h>                              // Стандартные типы Keil

#include "../config.h"                           // Файл конфигурации проекта
#include "bup_data_store.h"
#include "SWS.h"                                 // Драйвер работы с СВС
#include "SNS.h"                                 // Драйвер работы с СНС
#include "BIM.h"                                 // Драйвер работы с БИМ
#include "1636PP52Y.h"                           // Драйвер работы с внешней SPI flash 1636рр52у от Миландр
#include "25Q64FV.h"                             // Драйвер работы с внешней SPI flash 25Q64FV от Winbond
#include "discrete_io.h"                         // Драйвер работы дискретами (реле, LED..)
#include "ZPZ.h"                                 // Драйвер работы c загрузчиком полетного задания
#include "otherlib.h"                            // Модуль аппаратнозависимых функций общего назначения


#include "Log_FS/Log_FS.h"                       // Файловая система для записи логов в "черный ящик"
#include "Math_model/M_Model.h"                  // Математическая модель системы управления полетом
#include "SelfTesting.h"                         // Модуль самодиагностики
#include "RetargetPrintf/RetargetPrintf.h"       // Переопределение функции printf для вывода в CAN и записи в черный ящик
#include "task_manager.h"                        // Планировщик задач

#ifdef LOGS_ENABLE  //******************************************************* Если включено логирование в черный ящик
	#include "log_recorder.h"
#endif //******************************************************************** !LOGS_ENABLE

#ifdef DEBUG_VARS	//*************************************************************** Если активна отладка переменных 
	#include "debug.h"                             // Отладочный модуль трассировки внутрисистемных переменных в CAN1
#endif //************************************************************************** !DEBUG_VARS 	





//	double temp = 0;
//	int16_t temp2 = 0;
//	uint8_t buff[200];
//  long long cnt = 0;
//	

//	char CHM = 0;
//	double iii = 0;
//	short pp = 0;
//	char p1;
//	char p2;

uint32_t size = 0;
uint32_t num = 0;
uint32_t i = 0;

int main(void)
{	 
	// Настраиваем тактовую частоту процессора	
	InitCLK();
	// Конфигурируем дискреты, при этом все реле гарантированно переводятся в состояние по-умолчанию
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
	
	// Инициализируем хранилище данных БУП
	BUP_DataInit();
	
	// Запускаем фул-тест системы
	SelfTestingFull();
  
	
	
	
//	Log_Fs_FindFile(FIRST_FILE);
//  // Узнаем размер открытого файла (команда FILE_SIZE)
//  size = Log_Fs_GetFileProperties(FILE_SIZE);
//  // Так же можно узнать порядковый номер открытого файла (команда FILE_NUMBER)
//  num = Log_Fs_GetFileProperties(FILE_NUMBER);
//  // И прочитаем файл в буфер
//  //LogFs_ReadFile(buffer, 0, size);
//	

//    // Перейдем к следующему файлу (Внимание! Теперь функция с командой NEXT_FILE)
//    // То есть этой функцией мы можем последовательно от самого старого к самому новому файлу переключаться между ними)
//    // Так же можно проверять открылся ли файл
//    while (Log_Fs_FindFile(NEXT_FILE) != FS_ERROR)
//    {
//        // Узнаем размер открытого файла (команда FILE_SIZE)
//        size = Log_Fs_GetFileProperties(FILE_SIZE);
//        // Продемонстрируем, что функцией LogFs_ReadFile, можно читать файл в за несколько обращений
//        // Будем читать по байту size раз (размер файла в байтах)
////        for (i = 0; i < size; i++)
////        {
////            //LogFs_ReadFile(&buffer[i], i, 1);
////        }
//    }
//		
//		while(1);
	

//	BIM_Supply_ON();
//	
//	BUP_DataUpdate ();
//	// Отправляем данные математической модели
//	M_Model_PrepareData ();
//	
//	M_Model_Init();
//	printf_switcher(TO_LOG, 0);
//	while(1)
//	{
//		TaskManagerRun();
//	}

//////////////
//	BIM_Supply_ON();
//	
//	CAN_BufferRelease (MDR_CAN1, 16);
//	CAN_BufferRelease (MDR_CAN1, 17);
//	CAN_BufferRelease (MDR_CAN1, 18);
//	
//	while(1)
//	{
//		//SelfTesting_RIGHT_BIM();
//		//SelfTesting_LEFT_BIM();
//		SelfTestingOnline();
//		delay_us(100000);
//		debug_can_full_struct();
//		delay_us(100000);
//	}

//			SNS_Available_Data_Response_Union  SNS_DataState;
//			SNS_Device_Information_Response_Union  SNS_DeviceInformation;
//			
//while(1){
//     	while(SNS_GetPositionData(&SNS_Position) != SNS_OK && (timeout != 20)) timeout ++;
//			timeout = 0;
//      while(SNS_GetOrientationData(&SNS_Orientation) != SNS_OK && (timeout != 20)) timeout ++;
//			timeout = 0;
//			while(SNS_GetDataState(&SNS_DataState) != SNS_OK && (timeout != 20)) timeout ++;
//			timeout = 0;
//			while(SNS_GetDeviceInformation(&SNS_DeviceInformation) != SNS_OK && (timeout != 20)) timeout ++;
//			timeout = 0;
//			while(SWS_GetPacket (&SWS_Data) && (timeout != 10)) timeout ++;
//			timeout = 0;
//}
	



	// Проверяем подключение разъема ЗПЗ
	if(!CONNECT_ZPZ_CHECK)
	{
		// Разъем подключен
		// Запускаем драйвер ЗПЗ
		ZPZ_init();
		ZPZ_RetargetPins ();
		
		// Для тестирования БИМов подадим на них питание
		BIM_Supply_ON();
		
		// И уходим на обслуживание режима ЗПЗ
		while(!CONNECT_ZPZ_CHECK)
		{
			// Запускаем модуль обслуживания ЗПЗ
			ZPZ_Service();
			
			// Если ЗПЗ в настоящий момент не занят тяжелым процессом (режим РК)
			if(ZPZ_CheckHighPriorityTask() == ZPZ_SC_MODE)
			{
				// Будем в свободное время будем заниматься самодиагностикой
				TaskManagerZPZBackgroundRun ();
			}				
		}
	}

	
	// Здесь начинается боевой режим работы
	
	// Проверим готова ли система к запуску управления (периферия в порядке и шпильки парашютов на месте)
	while(SelfTesting_PreflightDiagnostics() == ST_FAULT)
	{	
		// Что-то пошло не так
		// Запустим самодиагностику, и будем ожидать в надежде готовности системы к полёту
		SelfTestingOnline ();
	}
	
	// Если оказались здесь, значит БУП готов к работе
	
	// Ждем пока стабилизирующий парашют выбросится и извлечет шпильку 1
	while(SelfTesting_PIN1());
	// Выжидаем 10 секунд
	delay_us(10000000);
	// Поджигаем пиропатрон
	PYRO_ON();
	// Пиропатрон включаем только на 3 сек
	delay_us(3000000);
	
	// Ждем выброс основного парашюта - извлечения Шпильки 2
	while(SelfTesting_PIN2());
	// Ждем 5 секунд 
	delay_us(5000000);
	// Включаем питание БИМов
	BIM_Supply_ON();
	// Ждем 1 секунду, чтобы питание БИМов стабилировалось
	delay_us(1000000);
	
	#ifdef LOGS_ENABLE	//******************************************************* Если включено логирование в черный ящик
		// Создаём файл лога
		LogFs_CreateFile();
		// Пишем инициирующую информацию в лог
		loger_initmsg ();
	#endif //******************************************************************** !LOGS_ENABLE	
	
	// Запустим быструю диагностику системы перед запуском управления
	SelfTestingOnline();
	
	// Инициализируем мат. модель полета
	M_Model_Init();
	
	// Сбрасываем/инициализируем список задач
	TaskManager_RestartCycle();
	
	// Перед первым шагом матмодели необходимо подготовить данные
	// Запускаем обновление данных
	BUP_DataUpdate ();
	// Отправляем данные математической модели
	M_Model_PrepareData ();
	
	// Переключаем планировщик на первую из пассивных задач (диагностика)
	TaskManagerGo2Task (TaskTest, 0);
	
	// Главный рабочий цикл
	while(1)
	{
		// Начинаем выполнять рабочий цикл, который перезапускается по таймеру
		TaskManagerRun();	
	}
}
