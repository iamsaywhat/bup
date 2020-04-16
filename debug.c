#include "debug.h"

#include "MDR32F9Qx_port.h"
#include "MDR32F9Qx_can.h"
#include "selftesting.h"
#include "bupdatastorage.h"
#include "kmonshelf.h"
#include "config.h"
#include "otherlib.h"


#ifdef flightRegulatorCFB //******************************************************* Если выбран flightRegulatorCFB
	#include "math.model/flightRegulatorCFB/flightRegulatorCFB.h"
#else //*************************************************************************** Если выбран flightController
	#include "math.model/flightController/flightController.h"
#endif //************************************************************************** !flightRegulatorCFB 


// В качестве отладочного CAN используется CAN1
#define CAN_DEBUG MDR_CAN1

// Глобальный экземпляр отладочной структуры
debug_output debug_vars;

/*********************************************************************************************************
  debug_can - Универсальная функция вывода переменных любого типа в CAN
**********************************************************************************************************/
void debug_can(unsigned short id, void* data, unsigned char size)
{
  CAN_TxMsgTypeDef     Tx_msg;              // Структура CAN сообщения
  unsigned int         i = 0;               // Счетчик 
  unsigned int         Buffer_number;       // Номер свободного буфера
  TimeoutType          timeout;             // Таймаут контроль
		
  Tx_msg.ID = CAN_STDID_TO_EXTID(id);
  Tx_msg.PRIOR_0 = DISABLE;
  Tx_msg.IDE = CAN_ID_STD;
  Tx_msg.DLC = size;
  Tx_msg.Data[0] = 0;
  Tx_msg.Data[1] = 0;
	
  // Заполняем сообщение
  for (i = 0; i < size; i++)
  *((unsigned char*)Tx_msg.Data + i) = *((unsigned char*)data + i);
		
  // Спросим какой из буферов свободен для использования
  Buffer_number = CAN_GetDisabledBuffer (CAN_DEBUG);
  // Кладём сообщение в нужный буфер и ждем отправки
  CAN_Transmit(CAN_DEBUG, Buffer_number, &Tx_msg);
  // Ожидаем конца передачи, либо превышения времени ожидания
  setTimeout (&timeout, 1);
  while(((CAN_GetBufferStatus(CAN_DEBUG, Buffer_number) & CAN_STATUS_TX_REQ) != RESET) && (timeoutStatus(&timeout) != TIME_IS_UP));
  // Вне зависимости от того, удалось отправить или нет, освобождаем буфер
  CAN_BufferRelease (CAN_DEBUG, Buffer_number);
}

/*********************************************************************************************************
  debug_can_full_struct - Вывод всей структуры отладочной информации
**********************************************************************************************************/
void debug_can_full_struct (void)
{
  // Заблокируем попытку передачи, если CAN отключен аппаратно шпилькой или программно реле
  if(SelfTesting_STATUS(ST_pin1) == ST_OK || SelfTesting_STATUS(ST_POW_BIM) != ST_OK)
    return;
	
#ifdef flightRegulatorCFB
  debug_can(0x500, &debug_vars.distanceAB, 2);	
  debug_can(0x501, &debug_vars.orderAngle, 1);	
  debug_can(0x502, &debug_vars.diffAngle, 2);
  debug_can(0x503, &debug_vars.setAngle, 2);
  debug_can(0x504, &debug_vars.stateTurn, 1);
  debug_can(0x505, &debug_vars.stateAngleDoing, 1);
  debug_can(0x506, &debug_vars.stateAngleCorrection, 1);	
  debug_can(0x507, &debug_vars.changeControl, 1);	
  debug_can(0x508, &debug_vars.doingManeuverMode, 1);
  debug_can(0x510, &debug_vars.angle, 2);
  debug_can(0x513, &debug_vars.directionOfRotation, 8);
  debug_can(0x514, &debug_vars.TightenSlings, 8);
  debug_can(0x517, &debug_vars.Lat1, 8);
  debug_can(0x518, &debug_vars.Lat2, 8);
  debug_can(0x519, &debug_vars.Lon1, 8);
  debug_can(0x520, &debug_vars.Lon2, 8);
  debug_can(0x524, &debug_vars.distanceB, 8);
  debug_can(0x525, &debug_vars.distance2, 2);
  debug_can(0x528, &debug_vars.td, 8);
  debug_can(0x529, &debug_vars.tx, 8);
  debug_can(0x530, &debug_vars.tz, 8);
  debug_can(0x531, &debug_vars.flightMode, 1);
#else
  debug_can(0x524, &debug_vars.DistanceToTDP, 8);
  debug_can(0x529, &debug_vars.TimeToHorTarget, 8);
  debug_can(0x530, &debug_vars.TimeToTD, 8);
#endif 
  debug_can(0x511, &debug_vars.TDP_Lat, 8);
  debug_can(0x512, &debug_vars.TDP_Lon, 8);
  debug_can(0x515, &debug_vars.Alt2model, 8);
  debug_can(0x516, &debug_vars.SNSalt, 8);
  debug_can(0x521, &debug_vars.rtU_XYZi_Lat, 8);
  debug_can(0x522, &debug_vars.rtU_XYZi_Lon, 8);
  debug_can(0x523, &debug_vars.rtU_XYZi_Alt, 8);
  debug_can(0x526, &debug_vars.Relief_height, 2);
  debug_can(0x527, &debug_vars.SysState, 2); 
  debug_can(0x532, &debug_vars.radioLatitude, 8);
  debug_can(0x533, &debug_vars.radioLongitude, 8);
  debug_can(0x534, &debug_vars.radioUpdatedIndex, 1);
  debug_can(0x535, &debug_vars.leftBimCommand, 1);
  debug_can(0x536, &debug_vars.rightBimCommand, 1);
}

/*********************************************************************************************************
  debug_prepare_data - Подготовка отладочных данных к выводу
**********************************************************************************************************/
void debug_prepare_data (void)
{
#ifdef flightRegulatorCFB
  debug_vars.distanceAB            = (int16_t)(rtY.distanceAB);
  debug_vars.orderAngle            = (uint8_t)(rtY.orderAngle);
  debug_vars.diffAngle             = (int16_t)(rtY.diffAngle);
  debug_vars.setAngle              = (int16_t)(rtY.setAngle);
  debug_vars.stateTurn             = (uint8_t)(rtY.stateTurn);
  debug_vars.stateAngleDoing       = (uint8_t)(rtY.stateAngleDoing);
  debug_vars.stateAngleCorrection	 = (uint8_t)(rtY.stateAngleCorrection);
  debug_vars.changeControl	       = (uint8_t)(rtY.changeControl);
  debug_vars.doingManeuverMode     = (uint8_t)(rtY.doingManeuverMode);
  debug_vars.angle                 = (int16_t)(rtU.angle);
  debug_vars.directionOfRotation   = (double)(rtY.directionOfRotation);
  debug_vars.TightenSlings         = (double)(rtY.tightenSling);
  debug_vars.Lat1                  = (double)(rtY.lat1);
  debug_vars.Lat2                  = (double)(rtY.lat2);
  debug_vars.Lon1                  = (double)(rtY.lon1);
  debug_vars.Lon2                  = (double)(rtY.lon2);
  debug_vars.distanceB             = (double)(rtY.distanceB);
  debug_vars.distance2             = (uint16_t)(rtY.distance2);
  debug_vars.td                    = (double)(rtY.tD);
  debug_vars.tx                    = (double)(rtY.tx);
  debug_vars.tz                    = (double)(rtY.tz);
  debug_vars.flightMode            = (uint8_t)(rtY.executeMode);
  if(rtY.directionOfRotation == -1)
  {
    debug_vars.leftBimCommand = (uint8_t)rtY.tightenSling;
    debug_vars.rightBimCommand = 0;
  }
  else if(rtY.directionOfRotation == 1)
  {
    debug_vars.rightBimCommand = (uint8_t)rtY.tightenSling;
    debug_vars.leftBimCommand = 0;
  }
  else if(rtY.directionOfRotation == 2)
  {
    debug_vars.rightBimCommand = (uint8_t)rtY.tightenSling;
    debug_vars.leftBimCommand  = (uint8_t)rtY.tightenSling;
  }
  else 
  {
    debug_vars.rightBimCommand = 0;
    debug_vars.leftBimCommand  = 0;
  }
#else
  debug_vars.DistanceToTDP         = (double)rtY.horizontalDistance;            // Дистанция до точки приземления, м
  debug_vars.TimeToHorTarget       = (double)rtY.horizontalTime;                // Время полета то точки приземления по прямой, сек
  debug_vars.TimeToTD              = (double)rtY.verticalTime;                  // Время до открытия парашюта, сек
  debug_vars.leftBimCommand        = (uint8_t)rtY.leftStrap;                    // Команда от матмодели левому Биму
  debug_vars.rightBimCommand       = (uint8_t)rtY.rightStrap;                   // Команда от матмодели правому Биму
#endif  
  debug_vars.TDP_Lat               = Bup_getTouchdownPointLatitude();           // Широта точки приземления
  debug_vars.TDP_Lon               = Bup_getTouchdownPointLongitude();          // Долгота точки приземления
  debug_vars.Alt2model             = Bup_getCurrentPointAltitude();             // Высота преобразованная в метры
  debug_vars.SNSalt                = SNS_position.Struct.Pos_alt;               // Высота в том виде в котором принимаем от СНС
  debug_vars.rtU_XYZi_Lat          = Bup_getCurrentPointLatitude();             // Широта преобразованная в градусы
  debug_vars.rtU_XYZi_Lon          = Bup_getCurrentPointLongitude();            // Долгота преобразованная в градусы
  debug_vars.rtU_XYZi_Alt          = Bup_getCurrentPointAltitude();             // Высота преобразованная в метры
  debug_vars.Relief_height         = Bup_getCurrentPointRelief();               // Высота рельефа под нами в метрах
  debug_vars.SysState              = systemState;                               // Состояние системы (из SelfTesing)
  debug_vars.radioLatitude         = Bup_getRadioPointLatitude();               // Широта точки приземления от радиостанции
  debug_vars.radioLongitude        = Bup_getRadioPointLongitude();              // Долгота точки приземления от радиостанции
  debug_vars.radioUpdatedIndex     = Bup_getRadioPointUpdateIndex();            // Индекс обновления координат от радиостанции
}




