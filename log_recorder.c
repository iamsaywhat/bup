#include "log_recorder.h"

#include "config.h"
#include "Log_FS.h"
#include "RetargetPrintf/RetargetPrintf.h"
#include "SelfTesting.h"
#include "bup_data_store.h"
#include "BIM.h"


#ifdef flightRegulatorCFB //******************************************************* Если выбран flightRegulatorCFB
	#include "Math_model/flightRegulatorCFB/flightRegulatorCFB.h"
#else //*************************************************************************** Если выбран Easy_reg
	#include "Math_model/EasyReg/Easy_reg.h"
#endif //************************************************************************** !flightRegulatorCFB 



/************************************************************************************
        loger_initmsg - Запись в лог первичной информации
************************************************************************************/
void loger_initmsg (void)
{
	// Переключаем вывод в ЛОГ
	printf_switcher(TO_LOG, 0);
	// В начало файла кладём его порядковый номер
	printf("***File # %d***\n", LogFs_GetNumCurrentFile());
	printf("BUP_init..\n");
	// Выведем загруженное полетное задание
	printf("TD_Lat: %f;\n", BUP_Get_TouchdownLatitude());
	printf("TD_Lon: %f;\n", BUP_Get_TouchdownLongitude());
	printf("TD_Alt: %f;\n", BUP_Get_TouchdownAltitude());
	printf("BUP is ready!\n");
}
/************************************************************************************
        loger_periodprint - Функция, определяющая формат записи в лог сообщений
				на каждый шаг алгоритма.
************************************************************************************/
void loger_periodprint (void)
{
	printf("\nTimestamp, sec: %d\n",       BUP_DataStorage.ControlSecond); // Метку времени в ЛОГ
	printf("SNS_Lat: %llu\n",              BUP_GetSNS_Latitude());
	printf("SNS_Lon: %llu\n",              BUP_GetSNS_Longitude());
	printf("SNS_Alt: %llu\n",              BUP_GetSNS_Altitude());
	printf("SNS_Vel_lat: %d\n",            BUP_GetSNS_VelocityLatitude());
	printf("SNS_Vel_lon: %d\n",            BUP_GetSNS_VelocityLongitude());
	printf("SNS_Vel_alt: %d\n",            BUP_GetSNS_VelocityAltitude());
	printf("SNS_Course: %d\n",             BUP_GetSNS_Course());
	printf("SNS_Heading_true: %d\n",       BUP_GetSNS_HeadingTrue());
	printf("SNS_Heading_mgn: %d\n",        BUP_GetSNS_HeadingMgn());
	printf("SNS_Pitch: %d\n",              BUP_GetSNS_Pitch());
	printf("SNS_Roll: %d\n",               BUP_GetSNS_Roll());
	printf("SWS_TrueSpeed: %f\n",          BUP_GetSWS_TrueSpeed());
	printf("SWS_InstrumentSpeed: %f\n",    BUP_GetSWS_InstrumentalSpeed());
	printf("BIML_Pos: %d\n",               (uint8_t)(0.5 + 0.3922*BIM_GetStrapPosition(LEFT_BIM)));   // Перевод к процентной шкале с округлением
	printf("BIMR_Pos: %d\n",               (uint8_t)(0.5 + 0.3922*BIM_GetStrapPosition(RIGHT_BIM)));  // Перевод к процентной шкале с округлением
	printf("SystemState: %x\n",            SystemState);
	printf("Model_Lat, deg: %f\n",         BUP_Get_Latitude());
	printf("Model_Lon, deg: %f\n",         BUP_Get_Longitude());
	printf("Model_Alt, m: %f\n",           BUP_Get_Altitude());
	printf("Model_VelocityLat, m/s: %f\n", BUP_Get_VelocityLatitude());
	printf("Model_VelocityLon, m/s: %f\n", BUP_Get_VelocityLongitude());
	printf("Model_VelocityAlt, m/s: %f\n", BUP_Get_VelocityAltitude());
	printf("Model_HeadingTrue, rad: %f\n", BUP_Get_HeadingTrue());
	printf("Model_HeadingMgn, rad: %f\n",  BUP_Get_HeadingMgn());
	printf("Model_Course, rad: %f\n",      BUP_Get_Course());
	printf("Model_Pitch, rad: %f\n",       BUP_Get_Pitch());
	printf("Model_Roll, rad: %f\n",        BUP_Get_Roll());	
	
	// Если карта рельефа в текущей позиции доступна, запишем высоту рельефа
	if (SelfTesting_STATUS(ST_MapAvailability))
		printf("MAP, m: %d\n",               BUP_Get_ReliefHeight());
	else
		printf("MAP, m: NOT_AVAILABLE\n");
	
	#ifdef flightRegulatorCFB	//******************************************************* Если выбран flightRegulatorCFB
		printf("Model_BIM_CMD: %f\n", (double)(rtY.tightenSling*rtY.directionOfRotation));
		printf("Model_TD_CMD: %d\n",  (uint8_t)rtY.cmdTouchDown);
	#else //*************************************************************************** Если выбран Easy_reg
		printf("Model_BIM_CMD: %f\n", (double)Easy_reg_Y.BIM_CMD);
		printf("Model_TD_CMD: %d\n",  (uint8_t)Easy_reg_Y.TD_CMD);
	#endif //************************************************************************** !flightRegulatorCFB
}
