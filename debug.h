#ifndef _DEBUG_H_
#define _DEBUG_H_

#include <stdint.h>

// Список выводимых переменных
typedef struct 
{
	int16_t distanceAB;
	uint8_t orderAngle;
	int16_t diffAngle;
	int16_t setAngle;
	uint8_t stateTurn;
	uint8_t stateAngleDoing;
	uint8_t stateAngleCorrection;
	uint8_t changeControl;
	uint8_t doingManeuverMode;
	int16_t BIM_CMD;
	int16_t angle;
	double TDP_Lat;
	double TDP_Lon;
	double directionOfRotation;
	double cmdTightenSlings;
	double Alt2model;
	int64_t SNSalt;
	double Lat1;
	double Lat2;
	double Lon1;
	double Lon2;
	double rtU_XYZi_Lat;
	double rtU_XYZi_Lon;
	double rtU_XYZi_Alt;
	double distanceB;
} debug_output;

// Глобальный экземпляр отладочной структуры
extern debug_output debug_vars;

/*********************************************************************************************************
      debug_can - Универсальная функция вывода переменных любого типа в CAN
      Параметры:
                id    - Адрес на CAN шине
                *data - Указатель на переменную любого типа
                size  - Размер в байтах переменной
**********************************************************************************************************/
void debug_can(unsigned short id, void* data, unsigned char size);


/*********************************************************************************************************
      debug_can_full_struct - Вывод всей структуры отладочной информации
**********************************************************************************************************/
void debug_can_full_struct (void);

#endif

