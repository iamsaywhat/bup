#ifndef _DEBUG_H_
#define _DEBUG_H_

#include <stdint.h>
#include "config.h"

// Список выводимых переменных
typedef struct 
{
#ifdef flightRegulatorCFB          // Переменные для отладки flightRegulatorCFB
	int16_t distanceAB;              // Зарезервирован адрес CAN 0x500
	uint8_t orderAngle;              // Зарезервирован адрес CAN 0x501
	int16_t diffAngle;               // Зарезервирован адрес CAN 0x502
	int16_t setAngle;                // Зарезервирован адрес CAN 0x503
	uint8_t stateTurn;               // Зарезервирован адрес CAN 0x504
	uint8_t stateAngleDoing;         // Зарезервирован адрес CAN 0x505
	uint8_t stateAngleCorrection;    // Зарезервирован адрес CAN 0x506
	uint8_t changeControl;           // Зарезервирован адрес CAN 0x507
	uint8_t doingManeuverMode;       // Зарезервирован адрес CAN 0x508
	int16_t angle;                   // Зарезервирован адрес CAN 0x510
	double directionOfRotation;      // Зарезервирован адрес CAN 0x513
	double cmdTightenSlings;         // Зарезервирован адрес CAN 0x514
	double Lat1;                     // Зарезервирован адрес CAN 0x517
	double Lat2;                     // Зарезервирован адрес CAN 0x518
	double Lon1;                     // Зарезервирован адрес CAN 0x519
	double Lon2;                     // Зарезервирован адрес CAN 0x520
	double distanceB;                // Зарезервирован адрес CAN 0x524
	uint16_t distance2;              // Зарезервирован адрес CAN 0x525
	
#else                              // Переменные для отладки Easy_reg
  /* Здесь можно добавить 
	   необходимые переменные, относящиеся к Easy_reg
	*/
#endif                             // Общесистемные переменные
	int16_t BIM_CMD;                 // Зарезервирован адрес CAN 0x509
	double TDP_Lat;                  // Зарезервирован адрес CAN 0x511
	double TDP_Lon;                  // Зарезервирован адрес CAN 0x512
	int64_t SNSalt;                  // Зарезервирован адрес CAN 0x516
	double Alt2model;                // Зарезервирован адрес CAN 0x515
	double rtU_XYZi_Lat;             // Зарезервирован адрес CAN 0x521
	double rtU_XYZi_Lon;             // Зарезервирован адрес CAN 0x522
	double rtU_XYZi_Alt;             // Зарезервирован адрес CAN 0x523
	int16_t Relief_height;           // Зарезервирован адрес CAN 0x526
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

