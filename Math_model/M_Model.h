/***************************************************************************
			
			M_Model - Обёртка алгоритмов управления полетом САУ.
      
			Связывает систему управления полетом с периферией БУПа,
			и предоставляет управление БИМами.
			
			В текущей версии можно выбирать тип регулятора с помощью
			условной компиляции, закоментировав/раскоментировать строку
			  #define flightRegulatorCFB
			
***************************************************************************/
#ifndef _M_MODEL_H_
#define _M_MODEL_H_

#include "../SNS.h"
#include "../SWS.h"
#include "stdint.h"



/***************************************************************************
			M_Model_Need2UpdateSet - Потребовать обновить данные для Мат.Модели  
***************************************************************************/
void M_Model_Need2UpdateSet(void);



/***************************************************************************
			M_Model_Need2UpdateReset - Данные для Мат.Модели обновлять 
			                           не требуется
***************************************************************************/
void M_Model_Need2UpdateReset(void);



/***************************************************************************
				M_Model_Need2UpdateCheck - Проверка необходимости 
				обновить данные для Мат.Модели
				
				Возвращает: 1 - Требуется обновление данных 
				            0 - Обновление данных не требуется
***************************************************************************/
unsigned char M_Model_Need2UpdateCheck(void);



/***************************************************************************
				M_Model_Init - Инициализация мат. модели                           *
***************************************************************************/
void M_Model_Init(void);



/***************************************************************************
				M_Model_Step - Совершить шаг расчета                               *
***************************************************************************/
void M_Model_Step(void);



/***************************************************************************
				Rad_12_to_Deg - Преобразование 10E-12 радиан в градусы             *
***************************************************************************/
double Rad_12_to_Deg(int64_t rad);


/***************************************************************************
				Rad_6_to_Deg - Преобразование 10E-6 радиан в градусы               *
***************************************************************************/
double Rad_6_to_Deg(int32_t rad);



/***************************************************************************
				Rad_to_Deg - Преобразование радиан в градусы                      *
***************************************************************************/
double Rad_to_Deg(double rad);



/***************************************************************************
				Rad_6_to_Rad - Преобразование 10E-6 радиан в радианы              *
***************************************************************************/
double Rad_6_to_Rad(int32_t rad);




/***************************************************************************
				Meter_12_to_Meter - Преобразование 10E-12 метров в метры           *
***************************************************************************/
double Meter_12_to_Meter (int64_t meter);



/***************************************************************************
		M_Model_PrepareData - Заполнение входных данных для мат. модели        *
***************************************************************************/
void M_Model_PrepareData (SNS_Position_Data_Response_Type SNS_PosData, SNS_Orientation_Data_Response_Type SNS_Orientation, SWS_Packet_Type SWS_Data);



/***************************************************************************
      M_Model_Control - Выполнение команд мат. модели                      *
***************************************************************************/
void M_Model_Control (void);



/******************************************************************************
                                                                              *
      M_Model_Cmd2BIM - Функция, предоставляющая мат модели управление БИМами *
			                                                                        *
			Параметры:                                                              *
								Side - Управляющий сигнал в процентах от (-100% до 100%)      *
								где отрицательная область - сигнал для левого БИМа,           *
								положительная - сигнал для правого БИМа, а 0 слабления всех   *
								строп.                                                        *
								                                                              *
*******************************************************************************/
void M_Model_Cmd2BIM (double Side);

#endif
