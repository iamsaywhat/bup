/***************************************************************************
			
    bup_data_store - Модуль сбора и хранения служебной информации.
			
    Данный модуль позволяет осуществлять сбор информации от СНС, СВС,
    карты высот, необходимую конвертацию данных к удобному представлению.
			
    Предоставляет функции-интерфейсы для доступа ко всем необходимым 
    служебным данным.
			
    Содержит функции конвертации данных от СНС к удобному виду.
			
    Примечание: Модуль был реализован, из-за того, что нескольким
    модулям бывает нужна одна и та же системная информация. При 
    этом передача аргументом в большинстве случаев неудобна и может
    теоретически может повлечь к снижению производительности 
    (самым неудобным было запись в лог и трассировка параметров в CAN,
    из-за чего фукции записи/отправки приходилось размещать прям посреди
    полезного кода).
    Было решено организовать глобальное хранилище, доступное из
    любого модуля при подключении header-файла. 
		
***************************************************************************/
#ifndef _BUP_DATA_STORE_H_
#define _BUP_DATA_STORE_H_

#include <stdint.h>
#include "SNS.h"
#include "SWS.h"

/*******************************************
    Тип структуры служебных данных БУП    
*******************************************/
typedef struct{
	double   TouchdownLatitude;          // Широта точки приземления, градус
	double   TouchdownLongitude;         // Долгота точки приземления, градус
	double   TouchdownAltitude;          // Высота раскрытия посадочного парашюта, метры
	double   Latitude;                   // Текущая широта, градус
	double   Longitude;                  // Текущая долгота, градус
	double   Altitude;                   // Текущая высота над уровнем моря по данным от СНС, метры
	double   HeadingTrue;                // Истинный курс, радианы
	double   HeadingMgn;                 // Магнитный курс, радианы
	double   VelocityLatitude;           // Скорость по широте, м/c
	double   VelocityLongitude;          // Скорость по долготе, м/c
	double   VelocityAltitude;           // Скорость снижения, м/c
	double   Pitch;                      // Тангаж, радианы
	double   Roll;                       // Крен, радианы
	double   Course;                     // Путевой курс, радианы
	short    ReliefHeight;               // Высота рельефа в текущей точке, метры
	short    ReliefOnTDP;                // Высота рельефа в точке приземления, метры
	uint32_t ControlSecond;              // Системное время БУП
}BUP_DATA;


/*******************************************
       Cлужебные данные БУП               
*******************************************/
extern BUP_DATA                             BUP_DataStorage;     // Рабочие данные БУПа
extern SNS_Orientation_Data_Response_Union  SNS_Orientation;     // Данные ориентации от СНС
extern SNS_Position_Data_Response_Union     SNS_Position;        // Данные местоположения от СНС
extern SWS_Packet_Type_Union                SWS_Data;            // Данные от СВС


/************************************************************************************************************** 
    BUP_DataInit - Инициализация хранилища данных.
            
    Описание: Функция инициализации структуры BUP_DataStorage, производит установку 
    параметров по-умолчанию.
***************************************************************************************************************/
void BUP_DataInit (void);


/**************************************************************************************************************
    BUP_DataUpdate - Обновление данных хранилища.                 
						
    Описание: Функция сбора необходимых для работы БУП в боевом режиме данных и их
    конвертация к удобного виду. Использует функционал драйвера "SNS.h" для получения данных 
    местоположения и ориентации в пространстве, драйвера "SWS.h" для сбора полетных характеристик.
***************************************************************************************************************/
void BUP_DataUpdate (void);


/**************************************************************************************************************
    BUP_UpdateDataFromSNS - Обновление данных СНС.                 
						
    Описание: Обновляет только структуру данных от СНС
***************************************************************************************************************/
void BUP_UpdateDataFromSNS (void);


/**************************************************************************************************************
    BUP_UpdateDataFromSWS - Обновление данных СВС.                 
						
    Описание: Обновляет только структуру данных от СВС
***************************************************************************************************************/
void BUP_UpdateDataFromSWS (void);



/***************************************************************************
    Функции доступа к данным от СНС                                    
***************************************************************************/
inline int64_t  BUP_GetSNS_Latitude (void)                   { return SNS_Position.Struct.Pos_lat;}
inline int64_t  BUP_GetSNS_Longitude (void)                  { return SNS_Position.Struct.Pos_lon;}
inline int64_t  BUP_GetSNS_Altitude (void)                   { return SNS_Position.Struct.Pos_alt;}
inline int32_t  BUP_GetSNS_HeadingTrue (void)                { return SNS_Orientation.Struct.Heading_true;}
inline int32_t  BUP_GetSNS_HeadingMgn (void)                 { return SNS_Orientation.Struct.Heading_mgn;}
inline int32_t  BUP_GetSNS_Course (void)                     { return SNS_Position.Struct.Course;}
inline int32_t  BUP_GetSNS_VelocityLatitude (void)           { return SNS_Position.Struct.Vel_lat;}
inline int32_t  BUP_GetSNS_VelocityLongitude (void)          { return SNS_Position.Struct.Vel_lon;}
inline int32_t  BUP_GetSNS_VelocityAltitude (void)           { return SNS_Position.Struct.Vel_alt;}
inline int32_t  BUP_GetSNS_Pitch (void)                      { return SNS_Orientation.Struct.Pitch;}
inline int32_t  BUP_GetSNS_Roll (void)                       { return SNS_Orientation.Struct.Roll;}
inline uint64_t BUP_GetSNS_Timestamp (void)                  { return SNS_Position.Struct.Time_stamp;}
inline uint8_t  BUP_GetSNS_Quality (void)                    { return SNS_Position.Struct.Quality;}
inline uint8_t  BUP_GetSNS_GLONASS_SatellitesNumber (void)   { return SNS_Position.Struct.Gln_sat_num;}
inline uint8_t  BUP_GetSNS_GPS_SatellitesNumber (void)       { return SNS_Position.Struct.Gps_sat_num;}

/***************************************************************************
    Функции доступа к данным от СВС                                   
***************************************************************************/
inline float BUP_GetSWS_StaticPressure (void)     { return SWS_Data.Struct.StaticPressure;}
inline float BUP_GetSWS_DynamicPressure (void)    { return SWS_Data.Struct.DynamicPressure;}
inline float BUP_GetSWS_AbsoluteHeight (void)     { return SWS_Data.Struct.AbsoluteHeight;}
inline float BUP_GetSWS_InstrumentalSpeed (void)  { return SWS_Data.Struct.InstrumentSpeed;}
inline float BUP_GetSWS_TrueSpeed (void)          { return SWS_Data.Struct.TrueSpeed;}

/***************************************************************************
    Функции доступа к данным БУП
***************************************************************************/
inline double   BUP_Get_TouchdownLatitude(void)  { return BUP_DataStorage.TouchdownLatitude;}
inline double   BUP_Get_TouchdownLongitude(void) { return BUP_DataStorage.TouchdownLongitude;}
inline double   BUP_Get_TouchdownAltitude(void)  { return BUP_DataStorage.TouchdownAltitude;}
inline double   BUP_Get_Latitude(void)           { return BUP_DataStorage.Latitude;}
inline double   BUP_Get_Longitude(void)          { return BUP_DataStorage.Longitude;}
inline double   BUP_Get_Altitude(void)           { return BUP_DataStorage.Altitude;}
inline double   BUP_Get_HeadingTrue(void)        { return BUP_DataStorage.HeadingTrue;}
inline double   BUP_Get_HeadingMgn(void)         { return BUP_DataStorage.HeadingMgn;}
inline double   BUP_Get_VelocityLatitude(void)   { return BUP_DataStorage.VelocityLatitude;}
inline double   BUP_Get_VelocityLongitude(void)  { return BUP_DataStorage.VelocityLongitude;}
inline double   BUP_Get_VelocityAltitude(void)   { return BUP_DataStorage.VelocityAltitude;}
inline double   BUP_Get_Pitch(void)              { return BUP_DataStorage.Pitch;}
inline double   BUP_Get_Roll(void)               { return BUP_DataStorage.Roll;}
inline double   BUP_Get_Course(void)             { return BUP_DataStorage.Course;}
inline short    BUP_Get_ReliefHeight(void)       { return BUP_DataStorage.ReliefHeight;}
inline short    BUP_Get_ReliefHeightOnTDP(void)  { return BUP_DataStorage.ReliefOnTDP;}
inline uint32_t BUP_Get_SystemTime (void)        { return BUP_DataStorage.ControlSecond;}


/***************************************************************************
    Rad_12_to_Deg - Преобразование 10E-12 радиан в градусы
***************************************************************************/
double Rad_12_to_Deg(int64_t rad);

/***************************************************************************
    Rad_6_to_Deg - Преобразование 10E-6 радиан в градусы 
***************************************************************************/
double Rad_6_to_Deg(int32_t rad);

/**************************************************************************
    Rad_to_Deg - Преобразование радиан в градусы 
***************************************************************************/
double Rad_to_Deg(double rad);

/***************************************************************************
    Rad_6_to_Rad - Преобразование 10E-6 радиан в радианы
***************************************************************************/
double Rad_6_to_Rad(int32_t rad);

/***************************************************************************
    Meter_12_to_Meter - Преобразование 10E-12 метров в метры
***************************************************************************/
double Meter_12_to_Meter (int64_t meter);

/***************************************************************************
    Ms_6_to_Ms - Преобразование 10E-6 метров\c в метры\с 
***************************************************************************/
double Ms_6_to_Ms (int32_t Ms);


#endif
