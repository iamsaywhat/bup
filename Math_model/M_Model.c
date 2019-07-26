#include "M_Model.h"

#include "../BIM.h"
#include "../discrete_io.h"
#include "../debug.h"
#include "../HeightMap/Heightmap_conf_and_pd.h"

#ifdef flightRegulatorCFB
	#include "flightRegulatorCFB/flightRegulatorCFB.h"
#else
  //#include "EasyReg/EasyReg.h"
	#include "EasyReg/Easy_reg.h"
#endif

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
	BIM_Response_UnionType	BIM_Response;
	
	BIM_SendRequest (RIGHT_BIM, BIM_CMD_ON, 0, 9, 255, 255);
	BIM_SendRequest (LEFT_BIM, BIM_CMD_ON, 0, 7, 255, 255);
	BIM_ReceiveResponse (&BIM_Response, RIGHT_BIM);
	BIM_ReceiveResponse (&BIM_Response, LEFT_BIM);
	
#ifdef flightRegulatorCFB	
	flightRegulatorCFB_initialize();
	rtU.isVeracityGns = 0;
#else
	Easy_reg_initialize();
#endif
}

/***************************************************************************
				M_Model_Step - Совершить шаг расчета                               *
***************************************************************************/
void M_Model_Step(void)
{
#ifdef flightRegulatorCFB
	flightRegulatorCFB_step();
#else
	Easy_reg_step();
#endif
}

/***************************************************************************
				Rad_12_to_Deg - Преобразование 10E-12 радиан в градусы             *
***************************************************************************/
double Rad_12_to_Deg(int64_t rad)
{
	double result = 0;
	result = ((rad/1000000000000.0)*180.0)/pi_const; 
	return result;
}

/***************************************************************************
				Rad_6_to_Deg - Преобразование 10E-6 радиан в градусы               *
***************************************************************************/
double Rad_6_to_Deg(int32_t rad)
{
	double result = 0;
	result = ((rad/1000000.0)*180.0)/pi_const;
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
	result = (rad/1000000.0);
	return result;
}

/***************************************************************************
				Meter_12_to_Meter - Преобразование 10E-12 метров в метры           *
***************************************************************************/
double Meter_12_to_Meter (int64_t meter)
{
	double result = 0;
	result = ((double)(meter/1000000000000.0));
	return result;
}


/***************************************************************************
		M_Model_PrepareData - Заполнение входных данных для мат. модели        *
***************************************************************************/
void M_Model_PrepareData (SNS_Position_Data_Response_Type SNS_PosData, SNS_Orientation_Data_Response_Type SNS_Orientation, SWS_Packet_Type SWS_Data)
{
#ifdef flightRegulatorCFB
	// Координаты точки приземления (подгружаем из памяти)
  rtU.xyzPoints[lat] = GetTouchDownPointLat();
	rtU.xyzPoints[lon] = GetTouchDownPointLon();
  rtU.xyzPoints[alt] = 0;
		
	// Текущие координаты:
	rtU.XYZi[lat] = Rad_12_to_Deg (SNS_PosData.Pos_lat);
	rtU.XYZi[lon] = Rad_12_to_Deg (SNS_PosData.Pos_lon);
	rtU.XYZi[alt] = Meter_12_to_Meter (SNS_PosData.Pos_alt);
	debug_vars.Alt2model = (double)(rtU.XYZi[alt]);
	// Истинный курс
 	rtU.angle = Rad_6_to_Rad(SNS_Orientation.Heading_true);
	
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

	// Вывод отладочной информации 
	debug_vars.SNSalt = (int64_t)(SNS_PosData.Pos_alt);
	debug_vars.rtU_XYZi_Lat = (double)(rtU.XYZi[lat]);
	debug_vars.rtU_XYZi_Lon = (double)(rtU.XYZi[lon]);
	debug_vars.rtU_XYZi_Alt = (double)(rtU.XYZi[alt]);
#else
	Easy_reg_U.TDP_lon = GetTouchDownPointLon();
	Easy_reg_U.TDP_lat = GetTouchDownPointLat();
	Easy_reg_U.TDP_alt = 0;
	Easy_reg_U.Pos_lon = Rad_12_to_Deg (SNS_PosData.Pos_lon);
	Easy_reg_U.Pos_lat = Rad_12_to_Deg (SNS_PosData.Pos_lat);
	Easy_reg_U.Pos_alt = Meter_12_to_Meter (SNS_PosData.Pos_alt);
	Easy_reg_U.ActualCourse = Rad_6_to_Rad(SNS_Orientation.Heading_true);
#endif

	// Данные были обновлены, сообщаем об этом Мат. модели
	M_Model_Need2UpdateReset();
}


/***************************************************************************
      M_Model_Control - Выполнение команд мат. модели                      *
***************************************************************************/
void M_Model_Control (void)
{	
#ifdef flightRegulatorCFB	
	// Если команда на посадку не пришла, мы еще в полете, будем управлять
	if(rtY.cmdTouchDown == ((uint8_t)0))
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
	else if (rtY.cmdTouchDown ==((uint8_t)1))
	{
		// Замок створки открыть
	   BLIND_CTRL_ON();
		// Тут нельзя дольше 10 сек удерживать включение
		// TODO
	}
	
  // Вывод отладочной информации в CAN	
	// Заполняем структуру отладочной информации
  debug_vars.distanceAB = (int16_t)(rtY.distanceAB);
	debug_vars.orderAngle = (uint8_t)(rtY.orderAngle);
	debug_vars.diffAngle = (int16_t)(rtY.diffAngle);
	debug_vars.setAngle =(int16_t)(rtY.setAngle);
	debug_vars.stateTurn =(uint8_t)(rtY.stateTurn);
	debug_vars.stateAngleDoing = (uint8_t)(rtY.stateAngleDoing);
	debug_vars.stateAngleCorrection = (uint8_t)(rtY.stateAngleCorrection);
	debug_vars.changeControl = (uint8_t)(rtY.changeControl);
	debug_vars.doingManeuverMode = (uint8_t)(rtY.doingManeuverMode);
	debug_vars.BIM_CMD = (int16_t)(rtY.cmdTightenSlings*rtY.directionOfRotation);
	debug_vars.angle = (int16_t)(rtU.angle);
	debug_vars.TDP_Lat = GetTouchDownPointLat();
	debug_vars.TDP_Lon = GetTouchDownPointLon();
	debug_vars.directionOfRotation = (double)(rtY.directionOfRotation);
	debug_vars.cmdTightenSlings = (double)(rtY.cmdTightenSlings);
  debug_vars.Lat1 = (double)(rtY.lat1);
	debug_vars.Lat2 = (double)(rtY.lat2);
	debug_vars.Lon1 = (double)(rtY.lon1);
	debug_vars.Lon2 = (double)(rtY.lon2);
	debug_vars.distanceB = (double)(rtY.distanceB);
  // Выводим структуру
	debug_can_full_struct();
#else 
  if (Easy_reg_Y.TD_CMD == 1)
	{
		// Замок створки открыть
		BLIND_CTRL_ON();
		// Тут нельзя дольше 10 сек удерживать включение
		// TODO
	}	
	else if (Easy_reg_Y.TD_CMD == 0)
		M_Model_Cmd2BIM (Easy_reg_Y.BIM_CMD);
#endif
}



/*******************************************************************************
    M_Model_Cmd2BIM -  Функция, предоставляющая мат модели управление БИМами   *
*******************************************************************************/
void M_Model_Cmd2BIM (double Side)
{
	int16_t i;                                // Счетчик циклов
	int16_t SIDE = (int16_t)(Side);           // Отбрасываем дробную часть
	static int16_t LastPosLeft = 0;           // Предыдущее положение левой стропы
	static int16_t LastPosRight = 0;          // Предыдущее положение правой стропы
	BIM_Response_UnionType	BIM_Response;     // Ответы БИМов
	
	// Целое число со знаком SIDE в процентах, знак указывает каким БИМом будем управлять

	// Отрицательное значение означает, что нужно затянуть левый БИМ на SIDE% и отпустить правый
	if(SIDE < 0)
	{
		// Устанавливать новое положение, будем только если оно отличается от старого (защита от щелчков)
		if(LastPosLeft != SIDE)
		{ 
			// SIDE в процентах, переводим в диапазон от 0..255, и избавляемся от минуса
			BIM_SendRequest (LEFT_BIM, BIM_CMD_ON, (uint8_t)(2.55*SIDE*(-1)), 7, 255, 255);
			BIM_ReceiveResponse (&BIM_Response, LEFT_BIM);
			LastPosLeft = SIDE;
		}
		// Устанавливать новое положение, будем только если оно отличается от старого (защита от щелчков)
		if (LastPosRight != 0)
		{
			BIM_SendRequest (RIGHT_BIM, BIM_CMD_ON, 0, 9, 255, 255);
			BIM_ReceiveResponse (&BIM_Response, RIGHT_BIM);
			LastPosRight = 0;
		}
	}
	// Это значит, что нужно ослабить оба БИМА
	else if(SIDE == 0)
	{	
		if(LastPosLeft != 0)
		{
			BIM_SendRequest (LEFT_BIM, BIM_CMD_ON, 0, 7, 255, 255);
			BIM_ReceiveResponse (&BIM_Response, LEFT_BIM);
			LastPosLeft = 0;
		}
		if(LastPosRight != 0)
		{
			BIM_SendRequest (RIGHT_BIM, BIM_CMD_ON, 0, 9, 255, 255);
			BIM_ReceiveResponse (&BIM_Response, RIGHT_BIM);
			LastPosRight = 0;
		}
	}
	// Положительное значание означает, что нужно затянуть правый БИМ на SIDE%, и отпустить левый
	else if(SIDE > 0)
	{
		// Устанавливать новое положение, будем только если оно отличается от старого (защита от щелчков)
		if(LastPosRight != SIDE)
		{ 
			// SIDE в процентах, переводим в диапазон от 0..255
			BIM_SendRequest (RIGHT_BIM, BIM_CMD_ON, (uint8_t)(2.55*SIDE), 9, 255, 255);
			BIM_ReceiveResponse (&BIM_Response, RIGHT_BIM);
			LastPosRight = SIDE;
		}
		// Устанавливать новое положение, будем только если оно отличается от старого (защита от щелчков)
		if(LastPosLeft != 0)
		{
			BIM_SendRequest (LEFT_BIM, BIM_CMD_ON, 0, 7, 255, 255);
			BIM_ReceiveResponse (&BIM_Response, LEFT_BIM);
			LastPosLeft = 0;
		}
	}
	// БИМы не сразу обновляют своё состояние по CAN, поэтому заставляем их 5 раз сообщить своё состояние
	// Чтобы снифер CANа получил актуальную информацию (нам она не очень нужна)
	for(i = 0; i < 5; i++)
	{
		BIM_SendRequest (RIGHT_BIM, BIM_CMD_REQ, 0, 0, 0, 0);
		BIM_ReceiveResponse (&BIM_Response, RIGHT_BIM);
		BIM_SendRequest (LEFT_BIM, BIM_CMD_REQ, 0, 0, 0, 0);
		BIM_ReceiveResponse (&BIM_Response, LEFT_BIM);
	}
}
