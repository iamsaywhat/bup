#include "bup_data_store.h"
#include "HeightMap/Heightmap_conf_and_pd.h"
#include "HeightMap/Heightmap.h"
#include "SelfTesting.h"
#include "config.h"


#define pi_const 3.1415927


/*******************************************************************************************************************
       Версия ПО БУП               
*******************************************************************************************************************/
const BupFirmwareVersion  bupFirmwareVersion = {0,    // Старшая версия ПО
                                                30,   // Младшая версия ПО
																								0,    // Изменения внутри версии
#ifdef flightRegulatorCFB // Если выбран flightRegulatorCFB
	                                              1,    // Oпция регулятора - flightRegulatorCFB
																								0,    // Старшая версия модели регулятора
																								1};   // Младшая версия модели регулятора
#else //******************** Если выбран Easy_reg
	                                              2,    // Опция регуляторв - Easy_reg
																								0,    // Старшая версия модели регулятора
																								1};   // Младшая версия модели регулятора
#endif //************************************************************************** !flightRegulatorCFB 



/*******************************************
    Cлужебные данные БУП               
*******************************************/      
BUP_DATA                             BUP_DataStorage;     // Рабочие данные БУПа
SNS_Orientation_Data_Response_Union  SNS_Orientation;     // Данные ориентации от СНС
SNS_Position_Data_Response_Union     SNS_Position;        // Данные местоположения от СНС
SWS_Packet_Type_Union                SWS_Data;            // Данные от СВС



/**************************************************************************************************************
    BUP_DataInit - Инициализация хранилища данных 
***************************************************************************************************************/
void BUP_DataInit (void)
{
	BUP_DataStorage.TouchdownLatitude  = GetTouchDownPointLat();    // Подгружаем из памяти
	BUP_DataStorage.TouchdownLongitude = GetTouchDownPointLon();   // Подгружаем из памяти
	BUP_DataStorage.TouchdownAltitude  = GetTouchDownPointAlt();    // Подгружаем из памяти
	BUP_DataStorage.Latitude = 0;
	BUP_DataStorage.Longitude = 0;
	BUP_DataStorage.Altitude = 0;
	BUP_DataStorage.HeadingTrue = 0;
	BUP_DataStorage.HeadingMgn = 0;
	BUP_DataStorage.VelocityLatitude = 0;
	BUP_DataStorage.VelocityLongitude = 0;
	BUP_DataStorage.VelocityAltitude = 0;
	BUP_DataStorage.Pitch = 0;
	BUP_DataStorage.Roll = 0;
	BUP_DataStorage.Course = 0;
	BUP_DataStorage.ReliefHeight = 0x7FFF;
	BUP_DataStorage.ControlSecond = 0;
	
	// Определим высоту рельефа в точке приземления
	BUP_DataStorage.ReliefOnTDP = GetHeight_OnThisPoint(BUP_DataStorage.TouchdownLongitude, BUP_DataStorage.TouchdownLatitude, TRIANGULARTION);
	if (BUP_DataStorage.ReliefOnTDP == MAP_NO_SOLUTION)
		BUP_DataStorage.ReliefOnTDP = 0;
	
}


/**************************************************************************************************************
    BUP_DataUpdate - Обновление данных хранилища
***************************************************************************************************************/
void BUP_DataUpdate (void)
{
	// Просим данные у СНС
	BUP_UpdateDataFromSNS ();
	// Просим данные у СВС
	BUP_UpdateDataFromSWS ();
	
	// Далее конвертируем полученные данные и будем хранить их уже в таком виде
	BUP_DataStorage.Latitude           = Rad_12_to_Deg (SNS_Position.Struct.Pos_lat);
	BUP_DataStorage.Longitude          = Rad_12_to_Deg (SNS_Position.Struct.Pos_lon);
	BUP_DataStorage.Altitude           = Meter_12_to_Meter (SNS_Position.Struct.Pos_alt);
	BUP_DataStorage.HeadingTrue        = Rad_6_to_Rad(SNS_Orientation.Struct.Heading_true); 
	BUP_DataStorage.HeadingMgn         = Rad_6_to_Rad(SNS_Orientation.Struct.Heading_mgn); 
	BUP_DataStorage.VelocityLatitude   = Ms_6_to_Ms (SNS_Position.Struct.Vel_lat); 
	BUP_DataStorage.VelocityLongitude  = Ms_6_to_Ms (SNS_Position.Struct.Vel_lon); 
	BUP_DataStorage.VelocityAltitude   = Ms_6_to_Ms (SNS_Position.Struct.Vel_alt); 
	BUP_DataStorage.Pitch              = Rad_6_to_Rad(SNS_Orientation.Struct.Pitch);
	BUP_DataStorage.Roll               = Rad_6_to_Rad(SNS_Orientation.Struct.Roll);
	BUP_DataStorage.Course             = Rad_6_to_Rad(SNS_Position.Struct.Course);
	
	// Рассчитываем по полученным данным высоту рельефа в точке
	BUP_DataStorage.ReliefHeight       = GetHeight_OnThisPoint(BUP_DataStorage.Longitude, BUP_DataStorage.Latitude, TRIANGULARTION); 
	
}

/**************************************************************************************************************
    BUP_UpdateDataFromSWS - Обновление данных СВС.                 
***************************************************************************************************************/
void BUP_UpdateDataFromSWS (void)
{
	uint8_t timeout = 0;  // Таймаут-счетчик
	
	// Пытаемся получить данные от СВС, не более 10 попыток
	while(SWS_GetPacket (&SWS_Data) && (timeout != 10)) timeout ++;
	// Если выход по таймауту, то фиксируем как неисправность
	if (timeout == 10)
		SelfTesting_SET_FAULT(ST_sws);	
}


/**************************************************************************************************************
    BUP_UpdateDataFromSNS - Обновление данных СНС.                 
***************************************************************************************************************/
void BUP_UpdateDataFromSNS (void)
{
	uint8_t timeout1 = 0, timeout2 = 0;  // Таймаут-счетчик

	// Запрашиваем данные местоположения с контролем таймаута (спрашиваем не более 20 раз подряд)
	while(SNS_GetPositionData(&SNS_Position) != SNS_OK && (timeout1 != 20)) timeout1 ++;	

	// Запрашиваем данные ориентации с контролем таймаута (спрашиваем не более 20 раз подряд)
	while(SNS_GetOrientationData(&SNS_Orientation) != SNS_OK && (timeout2 != 20)) timeout2 ++;
	// Если выход по таймауту, то фиксируем как неисправность
	if(timeout1 == 20 || timeout2 == 20)
		SelfTesting_SET_FAULT(ST_sns);	
}


/***************************************************************************
    Rad_12_to_Deg - Преобразование 10E-12 радиан в градусы
***************************************************************************/
double Rad_12_to_Deg(int64_t rad)
{
	double result = 0;
	result = ((rad/1e12)*180.0)/pi_const; 
	return result;
}

/***************************************************************************
    Rad_6_to_Deg - Преобразование 10E-6 радиан в градусы
***************************************************************************/
double Rad_6_to_Deg(int32_t rad)
{
	double result = 0;
	result = ((rad/1e6)*180.0)/pi_const;
	return result;
}

/***************************************************************************
    Rad_to_Deg - Преобразование радиан в градусы
***************************************************************************/
double Rad_to_Deg(double rad)
{
	double result = 0;
	result = (rad*180.0)/pi_const;
	return result;
}

/***************************************************************************
    Rad_6_to_Rad - Преобразование 10E-6 радиан в радианы
***************************************************************************/
double Rad_6_to_Rad(int32_t rad)
{
	double result = 0;
	result = (rad/1e6);
	return result;
}

/***************************************************************************
    Meter_12_to_Meter - Преобразование 10E-12 метров в метры
***************************************************************************/
double Meter_12_to_Meter (int64_t meter)
{
	double result = 0;
	result = ((double)(meter/1e12));
	return result;
}


/***************************************************************************
    Ms_6_to_Ms - Преобразование 10E-6 метров\c в метры\с
***************************************************************************/
double Ms_6_to_Ms (int32_t Ms)
{
	double result = 0;
	result = ((double)(Ms/1e6));
	return result;
}
