#include "M_Model.h"

#include "../BIM.h"
#include "../discrete_io.h"
#include "../HeightMap/Heightmap_conf_and_pd.h"
#include "../HeightMap/Heightmap.h"
#include "../SelfTesting.h"
#include "../otherlib.h"
#include "../config.h"

#ifdef flightRegulatorCFB //******************************************************* Если выбран flightRegulatorCFB
	#include "flightRegulatorCFB/flightRegulatorCFB.h"
#else //*************************************************************************** Если выбран Easy_reg
	#include "EasyReg/Easy_reg.h"
#endif //************************************************************************** !flightRegulatorCFB 

#ifdef DEBUG_VARS	//*************************************************************** Если активна отладка переменных 
	#include "../debug.h"
#endif //************************************************************************** !DEBUG_VARS

#ifdef LOGS_ENABLE  //************************************************************* Если включено логирование в черный ящик
	#include "../RetargetPrintf/RetargetPrintf.h"
#endif //************************************************************************** !LOGS_ENABLE	


#define lat 0
#define lon 1
#define alt 2
#define pi_const 3.1415927

// Глобальный для этого модуля флаг
uint8_t need_to_update_data = 0;

/***************************************************************************
			M_Model_Need2UpdateSet - Потребовать обновить данные для Мат.Модели  
***************************************************************************/
void M_Model_Need2UpdateSet(void)
{
	need_to_update_data = 1;
}

/***************************************************************************
			M_Model_Need2UpdateReset - Данные для Мат.Модели обновлять 
			                           не требуется
***************************************************************************/
void M_Model_Need2UpdateReset(void)
{
	need_to_update_data = 0;
}

/***************************************************************************
				M_Model_Need2UpdateCheck - Проверка необходимости 
				обновить данные для Мат.Модели
				
				Возвращает: 1 - Требуется обновление данных 
				            0 - Обновление данных не требуется
***************************************************************************/
uint8_t M_Model_Need2UpdateCheck(void)
{
	return need_to_update_data;
}


/***************************************************************************
				M_Model_Init - Инициализация мат. модели                           *
***************************************************************************/
void M_Model_Init(void)
{
	BIM_SendRequest (RIGHT_BIM, BIM_CMD_ON, 0, 9, 255, 255);
	BIM_SendRequest (LEFT_BIM, BIM_CMD_ON, 0, 7, 255, 255);
	BIM_ReceiveResponse (RIGHT_BIM);
	BIM_ReceiveResponse (LEFT_BIM);
	
	#ifdef flightRegulatorCFB //******************************************************* Если выбран flightRegulatorCFB
		flightRegulatorCFB_initialize();
	#else //*************************************************************************** Если выбран Easy_reg
		Easy_reg_initialize();
	#endif
}

/***************************************************************************
				M_Model_Step - Совершить шаг расчета                               *
***************************************************************************/
void M_Model_Step(void)
{
	#ifdef flightRegulatorCFB //******************************************************* Если выбран flightRegulatorCFB
		flightRegulatorCFB_step();
	#else //*************************************************************************** Если выбран Easy_reg
		Easy_reg_step();
	#endif
}

/***************************************************************************
				Rad_12_to_Deg - Преобразование 10E-12 радиан в градусы             *
***************************************************************************/
double Rad_12_to_Deg(int64_t rad)
{
	double result = 0;
	result = ((rad/1e12)*180.0)/pi_const; 
	return result;
}

/***************************************************************************
				Rad_6_to_Deg - Преобразование 10E-6 радиан в градусы               *
***************************************************************************/
double Rad_6_to_Deg(int32_t rad)
{
	double result = 0;
	result = ((rad/1e6)*180.0)/pi_const;
	return result;
}

/***************************************************************************
				Rad_to_Deg - Преобразование радиан в градусы                      *
***************************************************************************/
double Rad_to_Deg(double rad)
{
	double result = 0;
	result = (rad*180.0)/pi_const;
	return result;
}

/***************************************************************************
				Rad_6_to_Rad - Преобразование 10E-6 радиан в радианы               *
***************************************************************************/
double Rad_6_to_Rad(int32_t rad)
{
	double result = 0;
	result = (rad/1e6);
	return result;
}

/***************************************************************************
				Meter_12_to_Meter - Преобразование 10E-12 метров в метры           *
***************************************************************************/
double Meter_12_to_Meter (int64_t meter)
{
	double result = 0;
	result = ((double)(meter/1e12));
	return result;
}


/***************************************************************************
		M_Model_PrepareData - Заполнение входных данных для мат. модели        *
***************************************************************************/
void M_Model_PrepareData (SNS_Position_Data_Response_Type SNS_PosData, SNS_Orientation_Data_Response_Type SNS_Orientation, SWS_Packet_Type SWS_Data)
{
	double SNS_Lat        = Rad_12_to_Deg (SNS_PosData.Pos_lat);
	double SNS_Lon        = Rad_12_to_Deg (SNS_PosData.Pos_lon);
	double SNS_Alt        = Meter_12_to_Meter (SNS_PosData.Pos_alt);
	short Map_Alt         = GetHeight_OnThisPoint(SNS_Lon, SNS_Lat, TRIANGULARTION);
	double HeadingTrue    = Rad_6_to_Rad(SNS_Orientation.Heading_true);
	
	
	// Проверим доступна ли карта в текущей геолокации (если в данной точке карта недоступна, в Map_Alt будет лежать 0x7FFF)
	// Сразу же обновляем соответствующий флаг в модуля самодиагностики
	if(Map_Alt == 0x7FFF)
		SelfTesting_SET_FAULT(ST_MapAvailability);
	else
		SelfTesting_SET_OK(ST_MapAvailability);
	
	#ifdef LOGS_ENABLE  //******************************************************* Если включено логирование в черный ящик
		printf("Model_Lat: %f\n", SNS_Lat);
		printf("Model_Lon: %f\n", SNS_Lon);
		printf("Model_Alt: %f\n", SNS_Alt);
		printf("Model_Course: %f\n", HeadingTrue);
		if (SelfTesting_STATUS(ST_MapAvailability))
			printf("MAP: %d\n", Map_Alt);
		else
			printf("MAP: NOT_AVAILABLE\n");
	#endif //******************************************************************** !LOGS_ENABLE	
	
	
	#ifdef DEBUG_VARS	//*************************************************************** Если активна отладка переменн
		// Вывод общесистемной отладочной информации 
		debug_vars.SNSalt            = SNS_PosData.Pos_alt;      // Высота в том виде в котором принимаем от СНС
		debug_vars.rtU_XYZi_Lat      = SNS_Lat;                  // Широта преобразованная в градусы
		debug_vars.rtU_XYZi_Lon      = SNS_Lon;                  // Долгота преобразованная в градусы
		debug_vars.rtU_XYZi_Alt      = SNS_Alt;                  // Высота преобразованная в метры
		debug_vars.Relief_height     = Map_Alt;                  // Высота рельефа под нами в метрах
		debug_vars.Alt2model         = SNS_Alt;                  // Высота преобразованная в метры
		debug_vars.SysState          = SystemState;              // Состояние системы (из SelfTesing)
	#endif //************************************************************************** !DEBUG_VARS
	
	
	#ifdef flightRegulatorCFB //******************************************************* Если выбран flightRegulatorCFB
		// Координаты точки приземления (подгружаем из памяти)
		rtU.xyzPoints[lat] = GetTouchDownPointLat();
		rtU.xyzPoints[lon] = GetTouchDownPointLon();
		rtU.xyzPoints[alt] = 0;
		// Текущие координаты:
		rtU.XYZi[lat] = SNS_Lat;
		rtU.XYZi[lon] = SNS_Lon;
		rtU.XYZi[alt] = SNS_Alt;
		// Истинный курс
		rtU.angle = HeadingTrue;
		// Статус навигационного решения от СНС
		rtU.isVeracityGns = 1; //SNS_PosData.Quality;
	
	
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
		Easy_reg_U.TDP_lon        = GetTouchDownPointLon();
		Easy_reg_U.TDP_lat        = GetTouchDownPointLat();
		Easy_reg_U.TDP_alt        = GetTouchDownPoinAlt();
		Easy_reg_U.Pos_lon        = SNS_Lon;
		Easy_reg_U.Pos_lat        = SNS_Lat;
		Easy_reg_U.Pos_alt        = SNS_Alt;
		Easy_reg_U.ActualCourse   = HeadingTrue;
	#endif //************************************************************************** !flightRegulatorCFB 

	// Данные были обновлены, сообщаем об этом Мат. модели
	M_Model_Need2UpdateReset();
}


/***************************************************************************
      M_Model_Control - Выполнение команд мат. модели                      *
***************************************************************************/
void M_Model_Control (void)
{	
	#ifdef flightRegulatorCFB	//******************************************************* Если выбран flightRegulatorCFB
	
		#ifdef LOGS_ENABLE  //******************************************************* Если включено логирование в черный ящик
			printf("Model_BIM_CMD: %f\n", rtY.tightenSling*rtY.directionOfRotation);
			printf("Model_TD_CMD: %d\n", rtY.cmdTouchDown);
		#endif //******************************************************************* !LOGS_ENABLE	
	
		// Если команда на посадку не пришла, мы еще в полете, будем управлять
		if(rtY.cmdTouchDown == 0)
		{
			// Управление БИМами по выходу модели
			// Обе стропы отпустить
			if(rtY.directionOfRotation == 0.0)
			{
				M_Model_Cmd2BIM (rtY.tightenSling*rtY.directionOfRotation);
			}
			// Левая стропа
			else if(rtY.directionOfRotation == -1.0)
			{
				M_Model_Cmd2BIM (rtY.tightenSling*rtY.directionOfRotation);
			}
			// Правая стропа
			else if(rtY.directionOfRotation == 1.0)		
			{
				M_Model_Cmd2BIM (rtY.tightenSling*rtY.directionOfRotation);
			}
		}
		// Это команда на посадку 
		else if (rtY.cmdTouchDown == 1)
		{
			// Замок створки открыть
			BLIND_CTRL_ON();
			// Отключаем БИМы
			BIM_Supply_OFF();
			// Ждем 5 секунд
			delay_us(5000000);
			// Отключаем реле створки замка (нельзя удерживать дольше 10 секунд)
			BLIND_CTRL_OFF();
			// Повисаем в ожидании перезапуска
			while(1);
		}
	
		#ifdef DEBUG_VARS	//*************************************************************** Если активна отладка переменных 
			// Заполняем структуру отладочной информации (для flightRegulatorCFB)
			debug_vars.distanceAB              = (int16_t)(rtY.distanceAB);
			debug_vars.orderAngle              = (uint8_t)(rtY.orderAngle);
			debug_vars.diffAngle               = (int16_t)(rtY.diffAngle);
			debug_vars.setAngle                = (int16_t)(rtY.setAngle);
			debug_vars.stateTurn               = (uint8_t)(rtY.stateTurn);
			debug_vars.stateAngleDoing         = (uint8_t)(rtY.stateAngleDoing);
			debug_vars.stateAngleCorrection    = (uint8_t)(rtY.stateAngleCorrection);
			debug_vars.changeControl           = (uint8_t)(rtY.changeControl);
			debug_vars.doingManeuverMode       = (uint8_t)(rtY.doingManeuverMode);
			debug_vars.angle                   = (int16_t)(rtU.angle);
			debug_vars.directionOfRotation     = (double)(rtY.directionOfRotation);
			debug_vars.TightenSlings           = (double)(rtY.tightenSling);
			debug_vars.Lat1                    = (double)(rtY.lat1);
			debug_vars.Lat2                    = (double)(rtY.lat2);
			debug_vars.Lon1                    = (double)(rtY.lon1);
			debug_vars.Lon2                    = (double)(rtY.lon2);
			debug_vars.distanceB               = (double)(rtY.distanceB);
			debug_vars.distance2               = (uint16_t)(rtY.distance2);
			debug_vars.BIM_CMD                 = (int16_t)(rtY.tightenSling*rtY.directionOfRotation);
		#endif //************************************************************************** !DEBUG_VARS
	
	#else //*************************************************************************** Если выбран Easy_reg

		#ifdef LOGS_ENABLE  //******************************************************* Если включено логирование в черный ящик
			printf("Model_BIM_CMD: %f\n", Easy_reg_Y.BIM_CMD);
			printf("Model_TD_CMD: %d\n", (uint8_t)Easy_reg_Y.TD_CMD);
		#endif //******************************************************************** !LOGS_ENABLE	
		// Возможно пришла команда на посадку?
		if (Easy_reg_Y.TD_CMD == 1)
		{
			// Замок створки открыть
			BLIND_CTRL_ON();
			// Отключаем БИМы
			BIM_Supply_OFF();
			// Ждем 5 секунд
			delay_us(5000000);
			// Отключаем реле створки замка (нельзя удерживать дольше 10 секунд)
			BLIND_CTRL_OFF();
			// Повисаем в ожидании перезапуска
			while(1);
		}	
		// Команды на посадку не было, поэтому будем управлять стропами
		else if (Easy_reg_Y.TD_CMD == 0)
			M_Model_Cmd2BIM (Easy_reg_Y.BIM_CMD);

		#ifdef DEBUG_VARS	//*************************************************************** Если активна отладка переменных 
			debug_vars.BIM_CMD = (int16_t)(Easy_reg_Y.BIM_CMD);
		#endif //************************************************************************** !DEBUG_VARS 
	
	#endif //************************************************************************** !flightRegulatorCFB 

	#ifdef DEBUG_VARS	//*************************************************************** Если активна отладка переменных 
		// Заполняем структуру отладочной информации (общесистемной)
		debug_vars.TDP_Lat = GetTouchDownPointLat();
		debug_vars.TDP_Lon = GetTouchDownPointLon();
		// Отправляем отладочные данные в CAN
		debug_can_full_struct();
	#endif //************************************************************************** !DEBUG_VARS 	
}



/*******************************************************************************
    M_Model_Cmd2BIM -  Функция, предоставляющая мат модели управление БИМами   *
*******************************************************************************/
void M_Model_Cmd2BIM (double Side)
{
	int16_t i;                                // Счетчик циклов
	int16_t SIDE = 0;                         // Положение стропы (отриц - левая, полож - правая)
	
	// Целое число со знаком SIDE в процентах, знак указывает каким БИМом будем управлять
	// Конвертируем проценты в диапазон [0...255] и отбрасываем дробную часть
	SIDE = (int16_t)(2.55*Side);
	
	// Отрицательное значение означает, что нужно затянуть левый БИМ в положение SIDE и отпустить правый
	if(SIDE < 0)
	{
		// Делаем SIDE положительным и приводим к uint8_t
		SIDE = (uint8_t)((-1)*SIDE);
		
		// Устанавливать новое положение, будем только если оно отличается от старого (защита от щелчков)
		if(BIM_GetStrapPosition (LEFT_BIM) != SIDE)
		{ 
			BIM_SendRequest (LEFT_BIM, BIM_CMD_ON, SIDE, 7, 255, 255);
			BIM_ReceiveResponse (LEFT_BIM);
		}
		// Устанавливать новое положение, будем только если оно отличается от старого (защита от щелчков)
		if (BIM_GetStrapPosition (RIGHT_BIM) != 0)
		{
			BIM_SendRequest (RIGHT_BIM, BIM_CMD_ON, 0, 9, 255, 255);
			BIM_ReceiveResponse (RIGHT_BIM);
		}
	}
	// Это значит, что нужно ослабить оба БИМА
	else if(SIDE == 0)
	{	
		if(BIM_GetStrapPosition (LEFT_BIM) != 0)
		{
			BIM_SendRequest (LEFT_BIM, BIM_CMD_ON, 0, 7, 255, 255);
			BIM_ReceiveResponse (LEFT_BIM);
		}
		if(BIM_GetStrapPosition (RIGHT_BIM) != 0)
		{
			BIM_SendRequest (RIGHT_BIM, BIM_CMD_ON, 0, 9, 255, 255);
			BIM_ReceiveResponse (RIGHT_BIM);
		}
	}
	// Положительное значание означает, что нужно затянуть правый БИМ в положение SIDE, и отпустить левый
	else if(SIDE > 0)
	{
		// SIDE приводим к uint8_t
		SIDE = (uint8_t)(SIDE);
		
		// Устанавливать новое положение, будем только если оно отличается от старого (защита от щелчков)
		if(BIM_GetStrapPosition (RIGHT_BIM) != SIDE)
		{ 
			BIM_SendRequest (RIGHT_BIM, BIM_CMD_ON, SIDE, 9, 255, 255);
			BIM_ReceiveResponse (RIGHT_BIM);
		}
		// Устанавливать новое положение, будем только если оно отличается от старого (защита от щелчков)
		if(BIM_GetStrapPosition (LEFT_BIM) != 0)
		{
			BIM_SendRequest (LEFT_BIM, BIM_CMD_ON, 0, 7, 255, 255);
			BIM_ReceiveResponse (LEFT_BIM);
		}
	}
	// БИМы не сразу обновляют своё состояние по CAN, поэтому заставляем их 5 раз сообщить своё состояние
	for(i = 0; i < 5; i++)
	{
		BIM_SendRequest (RIGHT_BIM, BIM_CMD_REQ, 0, 0, 0, 0);
		BIM_ReceiveResponse (RIGHT_BIM);
		BIM_SendRequest (LEFT_BIM, BIM_CMD_REQ, 0, 0, 0, 0);
		BIM_ReceiveResponse (LEFT_BIM);
	}
}
