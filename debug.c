#include "debug.h"

#include "MDR32F9Qx_port.h"
#include "MDR32F9Qx_can.h"

// В качестве отладочного CAN используется CAN1
#define CAN_DEBUG MDR_CAN1

// Глобальный экземпляр отладочной структуры
debug_output debug_vars;

/*********************************************************************************************************
      debug_can - Универсальная функция вывода переменных любого типа в CAN
**********************************************************************************************************/
void debug_can(unsigned short id, void* data, unsigned char size)
{
	CAN_TxMsgTypeDef Tx_msg;              // Структура CAN сообщения
  unsigned int i = 0;                   // Счетчик 
	unsigned int Buffer_number;           // Номер свободного буфера
	unsigned short per = 0;               // Таймаут-счетчик
	
	Tx_msg.ID = CAN_STDID_TO_EXTID(id);
	Tx_msg.PRIOR_0 = DISABLE;
	Tx_msg.IDE = CAN_ID_STD;
	Tx_msg.DLC = size;
	Tx_msg.Data[0] = 0;
	Tx_msg.Data[1] = 0;
	// Заполняем сообщение
	for (i = 0; i < size; i++)
	{
		*(((unsigned char*)(Tx_msg.Data)) + i) = *(((unsigned char*)(data))++);
	}
	// Спросим какой из буферов свободен для использования
	Buffer_number = CAN_GetEmptyTransferBuffer (CAN_DEBUG);
	// Кладём сообщение в нужный буфер и ждем отправки
	CAN_Transmit(CAN_DEBUG, Buffer_number, &Tx_msg);
	// Ожидаем конца передачи, либо превышения времени ожидания
	while(((CAN_GetBufferStatus(CAN_DEBUG, Buffer_number) & CAN_STATUS_TX_REQ) != RESET) && (per != 0xFFF)){ per++;}
}

/*********************************************************************************************************
      debug_can_full_struct - Вывод всей структуры отладочной информации
**********************************************************************************************************/
void debug_can_full_struct (void)
{
	debug_can(0x500, &debug_vars.distanceAB, 2);	
	debug_can(0x501, &debug_vars.orderAngle, 1);	
	debug_can(0x502, &debug_vars.diffAngle, 2);
	debug_can(0x503, &debug_vars.setAngle, 2);
	debug_can(0x504, &debug_vars.stateTurn, 1);
	debug_can(0x505, &debug_vars.stateAngleDoing, 1);
	debug_can(0x506, &debug_vars.stateAngleCorrection, 1);	
	debug_can(0x507, &debug_vars.changeControl, 1);	
	debug_can(0x508, &debug_vars.doingManeuverMode, 1);	
	debug_can(0x509, &debug_vars.BIM_CMD, 2);
	debug_can(0x510, &debug_vars.angle, 2);
	debug_can(0x511, &debug_vars.TDP_Lat, 8);
	debug_can(0x512, &debug_vars.TDP_Lon, 8);
	debug_can(0x513, &debug_vars.directionOfRotation, 8);
	debug_can(0x514, &debug_vars.cmdTightenSlings, 8);
	debug_can(0x515, &debug_vars.Alt2model, 8);
	debug_can(0x516, &debug_vars.SNSalt, 8);
	debug_can(0x517, &debug_vars.Lat1, 8);
	debug_can(0x518, &debug_vars.Lat2, 8);
	debug_can(0x519, &debug_vars.Lon1, 8);
	debug_can(0x520, &debug_vars.Lon2, 8);
	debug_can(0x521, &debug_vars.rtU_XYZi_Lat, 8);
	debug_can(0x522, &debug_vars.rtU_XYZi_Lon, 8);
	debug_can(0x523, &debug_vars.rtU_XYZi_Alt, 8);
	debug_can(0x524, &debug_vars.distanceB, 8);
}
