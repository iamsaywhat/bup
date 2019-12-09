#include "M_Model.h"

#include "BIM.h"
#include "discrete_io.h"
#include "../SelfTesting.h"
#include "otherlib.h"
#include "../config.h"
#include "../bup_data_store.h"

#ifdef flightRegulatorCFB //******************************************************* Если выбран flightRegulatorCFB
	#include "flightRegulatorCFB/flightRegulatorCFB.h"
#else //*************************************************************************** Если выбран Easy_reg
	#include "EasyReg/Easy_reg.h"
#endif //************************************************************************** !flightRegulatorCFB 

#ifdef DEBUG_VARS //*************************************************************** Если активна отладка переменных 
	#include "../debug.h"
#endif //************************************************************************** !DEBUG_VARS

#ifdef LOGS_ENABLE	//************************************************************* Если включено логирование в черный ящик
   #include	"../log_recorder.h"
#endif //************************************************************************** !LOGS_ENABLE


#define lat 0
#define lon 1
#define alt 2

// Глобальный для этого модуля флаг
uint8_t M_Model_Need2UpdateFlag = 0;


/***************************************************************************
    M_Model_Init - Инициализация мат. модели    
***************************************************************************/
void M_Model_Init(void)
{
	// Переведем БИМы в состояние 0
	M_Model_Cmd2BIM (0.0);
	
	#ifdef flightRegulatorCFB //******************************************************* Если выбран flightRegulatorCFB
		flightRegulatorCFB_initialize();
	#else //*************************************************************************** Если выбран Easy_reg
		Easy_reg_initialize();
	#endif //************************************************************************** !flightRegulatorCFB
	
	// Запускаем таймер обслуживания мат модели (период дискретизации 1 с)
	Timer_SetInterruptPeriod (MMODEL_TIMER, SECOND_TICKS);
}

/***************************************************************************
    M_Model_Step - Совершить шаг расчета    
***************************************************************************/
void M_Model_Step(void)
{
	#ifdef flightRegulatorCFB //******************************************************* Если выбран flightRegulatorCFB
		flightRegulatorCFB_step();
	#else //*************************************************************************** Если выбран Easy_reg
		Easy_reg_step();
	#endif //************************************************************************** !flightRegulatorCFB
}



/***************************************************************************
    M_Model_PrepareData - Заполнение входных данных для мат. модели    
***************************************************************************/
void M_Model_PrepareData (void)
{
	#ifdef flightRegulatorCFB //******************************************************* Если выбран flightRegulatorCFB
		// Координаты точки приземления (подгружаем из памяти)
		rtU.xyzPoints[lat] = BUP_Get_TouchdownLatitude();
		rtU.xyzPoints[lon] = BUP_Get_TouchdownLongitude();
		rtU.xyzPoints[alt] = 0;
		// Текущие координаты:
		rtU.XYZi[lat] = BUP_Get_Latitude();
		rtU.XYZi[lon] = BUP_Get_Longitude();
		rtU.XYZi[alt] = BUP_Get_Altitude();
		// Путевой курс
		rtU.angle = BUP_Get_Course();
		// Статус навигационного решения от СНС
		rtU.isVeracityGns = 1; //SNS_PosData.Quality;
		// Подставляем статус доступности карты
		rtU.isAvailableDigitalTerrain = SelfTesting_STATUS(ST_MapAvailability);
		rtU.HighDigitalTerrain = BUP_Get_ReliefHeight();
		rtU.highStopUPS = 300.0;
	
//  // Высота над рельефом
//  rtU.HighAboveTheGround = 0; 
//	// Истинная скорость от СВС
//  rtU.WindVelocity = SWS_Data.TrueSpeed; 
//  // Высота над уровлем моря
//  rtU.HighAboveTheSea = SWS_Data.AbsoluteHeight;
//  // Количество доступных спутников: глонасс + gps	
//  rtU.SumOfGns = SNS_PosData.Gln_sat_num + SNS_PosData.Gps_sat_num;  
//  // Было ли получено 2D решение	
//  rtU.udOr3dofGns = ((SNS_PosData.Quality & 2) == 0)? 1.0: -1.0;
//	                 //((geo.gnss_quality & rrp::gnssVALUE_2D) == 0)? 1.0 : -1.0;               
//	// Статус-флаги
//  // Есть ли информация от карты рельефа: "1" - Да, "-1" - Нет.
//  rtU.isAvailableDigitalTerrain = -1; //(условие)? 1.0:-1.0; ;	
//	// Есть ли питание на БИМах????????????????????????????????????????????????
//	rtU.isNormalEnginesPower = 1.0; 
//	// Работают ли БИМы: "1" - да, "-1" - нет.
//	rtU.isEnginesAvailable = 1.0;
//  // Работает ли левый БИМ: "1" - да, "-1" - нет.	
//	rtU.LeftEnginehadWork = 1.0; 
//  // Работает ли правый БИМ: "1" - да, "-1" - нет.		
//  rtU.RightEnginehadWork = 1.0;  

	#else //*************************************************************************** Если выбран Easy_reg
		rtU.TDP_lon            = BUP_Get_TouchdownLongitude();
		rtU.TDP_lat            = BUP_Get_TouchdownLatitude();
		rtU.TDP_alt            = BUP_Get_TouchdownAltitude();
		rtU.Pos_lon            = BUP_Get_Longitude();
		rtU.Pos_lat            = BUP_Get_Latitude();
		rtU.Pos_alt            = BUP_Get_Altitude();
		rtU.ActualCourse       = BUP_Get_Course();
		rtU.Relief             = BUP_Get_ReliefHeight();
		rtU.ReliefOnTDP        = BUP_Get_ReliefHeightOnTDP();
		rtU.ReliefAvailable    = SelfTesting_STATUS(ST_MapAvailability);
		rtU.LatitudeVelocity   = BUP_Get_VelocityLatitude();
		rtU.LongitudeVelocity  = BUP_Get_VelocityLongitude();
		rtU.AltitudeVelocity   = BUP_Get_VelocityAltitude();
	#endif //************************************************************************** !flightRegulatorCFB 

	// Данные были обновлены, сообщаем об этом Мат. модели
	M_Model_Need2UpdateReset();
	
}


/***************************************************************************
    M_Model_Control - Выполнение команд мат. модели    
***************************************************************************/
void M_Model_Control (void)
{	
	#ifdef flightRegulatorCFB	//******************************************************* Если выбран flightRegulatorCFB	
		// Если команда на посадку не пришла, мы еще в полете, будем управлять
		if(rtY.cmdTouchDown == 0)
		{
			// Управление БИМами по выходу модели
			M_Model_Cmd2BIM (rtY.tightenSling*rtY.directionOfRotation);
		}
		// Это команда на посадку 
		else if (rtY.cmdTouchDown == 1)
		{
	#else //*************************************************************************** Если выбран Easy_reg
		// Команды на посадку не было, поэтому будем управлять стропами
		if (rtY.TD_CMD == 0)
			M_Model_Cmd2BIM (rtY.BIM_CMD);
		
		// Команда на посадку, завершаем работу
		else if (rtY.TD_CMD == 1)
		{
	#endif //************************************************************************** !flightRegulatorCFB 			
			// Переводим БИМЫ в состояние по умолчанию
			M_Model_Cmd2BIM (0.0);
			// Замок створки открыть
			BLIND_CTRL_ON();
			
			#ifdef LOGS_ENABLE	//************************************************************* Если включено логирование в черный ящик
			  // Пишем в лог информацию о завершении полета и последние данные
			  loger_exitmsg();
			#endif //************************************************************************** !LOGS_ENABLE

			// Ждем 5 секунд
			delay_us(5000000);
			// Отключаем реле створки замка (нельзя удерживать дольше 10 секунд)
			BLIND_CTRL_OFF();
			// Отключаем БИМы
			BIM_Supply_OFF();
			// Повисаем в ожидании перезапуска
			while(1);
		}
}



/*******************************************************************************
    M_Model_Cmd2BIM -  Функция, предоставляющая мат модели управление БИМами
*******************************************************************************/
void M_Model_Cmd2BIM (double Side)
{
	int16_t i;          // Счетчик циклов
	int16_t SIDE = 0;   // Положение стропы (отриц - левая, полож - правая)
	
	// Целое число со знаком SIDE в процентах, знак указывает каким БИМом будем управлять
	// Конвертируем проценты в диапазон [0...255] и отбрасываем дробную часть
	SIDE = (int16_t)(2.55*Side);
	
	// Запретим управление БИМ, если шпилька 1 вставлена (несмотря на то, что это реализованно аппаратно)
	if(SelfTesting_STATUS(ST_pin1) == ST_OK || SelfTesting_STATUS(ST_POW_BIM) != ST_OK)
	{
		// Это необходимо потому что, при износе переключателей, иногда при вставленой шпильке аппаратное отключение не происходит
		// Так же для общности будем отказываться управлять, если реле питания БИМ выключено (хоть такое происходить и не должно)
		return;
	}
	
	// Отрицательное значение означает, что нужно затянуть левый БИМ в положение SIDE и отпустить правый
	if(SIDE < 0)
	{
		// Делаем SIDE положительным и приводим к uint8_t
		SIDE = (uint8_t)((-1)*SIDE);
		
		/* Этот каскад условий необходим, чтобы: 
		1) Запретить одновременную работу двигателей, принудительным переходом через ноль
		   (если было: левый 50%, а сейчас пришла команда правый 50%, то сначала левый переводится в ноль,
		   а только потом, правый переводится в 50%);
		2) Контролировать текущее (реальное) положение двигателей, чтобы избавиться от шелчков, возникающих
		   при попытке перевести двигатель в положение в котором он уже находится;		
		*/
		
		// Требуется затянуть левый БИМ, правый при этом должен быть в положении 0 
		if (BIM_GetStrapPosition (RIGHT_BIM) != 0)
		{
            // Правый не в нуле, поэтому пока только переведём его в ноль
			BIM_SendRequest (RIGHT_BIM, BIM_CMD_ON, 0, 9, 255, 255);
		}
		// Правый действительно находится в нуле, можно отдать команду на затяжку левому
		else
		{
			if(BIM_GetStrapPosition (LEFT_BIM) != SIDE)
			{
			   // Устанавливать новое положение, будем только если оно отличается от старого (защита от щелчков)
			   BIM_SendRequest (LEFT_BIM, BIM_CMD_ON, SIDE, 7, 255, 255);
			}
		}
	}
	// Это значит, что нужно ослабить оба БИМА
	else if(SIDE == 0)
	{	
		if(BIM_GetStrapPosition (LEFT_BIM) != 0)
		{
			BIM_SendRequest (LEFT_BIM, BIM_CMD_ON, 0, 7, 255, 255);
		}
		if(BIM_GetStrapPosition (RIGHT_BIM) != 0)
		{
			BIM_SendRequest (RIGHT_BIM, BIM_CMD_ON, 0, 9, 255, 255);
		}
	}
	// Положительное значение означает, что нужно затянуть правый БИМ в положение SIDE, и отпустить левый
	else if(SIDE > 0)
	{
		// SIDE приводим к uint8_t
		SIDE = (uint8_t)(SIDE);
		
		/* Этот каскад условий необходим, чтобы: 
		1) Запретить одновременную работу двигателей, принудительным переходом через ноль
		   (если было: левый 50%, а сейчас пришла команда правый 50%, то сначала левый переводится в ноль,
		   а только потом, правый переводится в 50%);
		2) Контролировать текущее (реальное) положение двигателей, чтобы избавиться от шелчков, возникающих
		   при попытке перевести двигатель в положение в котором он уже находится;		
		*/
		
		// Требуется затянуть правый БИМ, левый при этом должен быть в положении 0 
		if(BIM_GetStrapPosition (LEFT_BIM) != 0)
		{
            // Левый не в нуле, поэтому пока только переведём его в ноль
			BIM_SendRequest (LEFT_BIM, BIM_CMD_ON, 0, 7, 255, 255);
		}
		// Левый действительно находится в нуле, можно отдать команду на затяжку правому
		else
		{
		   if(BIM_GetStrapPosition (RIGHT_BIM) != SIDE)
		   {
			    // Устанавливать новое положение, будем только если оно отличается от старого (защита от щелчков)
			    BIM_SendRequest (RIGHT_BIM, BIM_CMD_ON, SIDE, 9, 255, 255);
		   }
		}
	}
	// БИМы не сразу обновляют своё состояние по CAN, поэтому заставляем их 5 раз сообщить своё состояние
	for(i = 0; i < 5; i++)
	{
		SelfTesting_RIGHT_BIM();
		SelfTesting_LEFT_BIM();
	}
}
