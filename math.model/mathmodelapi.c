#include "mathmodelapi.h"

#include "bims.h"
#include "discreteio.h"
#include "../selftesting.h"
#include "otherlib.h"
#include "../config.h"
#include "../bupdatastorage.h"
#include "flightController/flightController.h"

#ifdef DEBUG_VARS //*************************************************************** Если активна отладка переменных 
  #include "../debug.h"
#endif //************************************************************************** !DEBUG_VARS

#ifdef LOGS_ENABLE	//************************************************************* Если включено логирование в черный ящик
  #include	"../logger/logger.h"
#endif //************************************************************************** !LOGS_ENABLE

#define lat 0
#define lon 1
#define alt 2


/***************************************************************************
  MathModel_touchdownStatus - Завершил ли алгоритм управления работу
***************************************************************************/
uint8_t MathModel_touchdownStatus(void)
{
    return (uint8_t)rtY.touchdown; 
}

/***************************************************************************
  MathModel_init - Инициализация мат. модели    
***************************************************************************/
void MathModel_initialize(void)
{
  // Переведем БИМы в состояние 0
  MathModel_sendBimCommand (0, 0);
  flightController_initialize();
  // Запускаем таймер обслуживания мат модели (период дискретизации 1 с)
  Timer_initialize (MMODEL_TIMER, SECOND_TICKS);
}

/***************************************************************************
  MathModel_step - Совершить шаг расчета    
***************************************************************************/
void MathModel_step(void)
{
    flightController_step();
}

/***************************************************************************
  MathModel_prepareData - Заполнение входных данных для мат. модели    
***************************************************************************/
void MathModel_prepareData (void)
{
    /* Точка запланированного приземления */
    rtU.touchdownPointLatitude  = Bup_getTouchdownPointLatitude();
    rtU.touchdownPointLongitude = Bup_getTouchdownPointLongitude();
    rtU.touchdownPointAltitude  = Bup_getTouchdownPointAltitude();
    /* Данные от спутника */
    rtU.currentPointLatitude    = Bup_getCurrentPointLatitude();
    rtU.currentPointLongitude   = Bup_getCurrentPointLongitude();
    rtU.currentPointAltitude    = Bup_getCurrentPointAltitude();
    rtU.velocityLatitude        = Bup_getCurrentVelocityLatitude();
    rtU.velocityLongitude       = Bup_getCurrentVelocityLongitude();
    rtU.velocityAltitude        = Bup_getCurrentVelocityAltitude();
    rtU.trackingCourse          = Bup_getCurrentCourse();
    rtU.gpsAvailable            = SelfTesting_STATUS(ST_sns);
    /* Данные от свс */
    rtU.barometricAirSpeed      = SWS_getTrueSpeed();           // На время испытаний на стенде устанавливаем константу
    rtU.barometricAltitude      = SWS_getAbsoluteHeight();      // так, как имитатор СВС в настоящий момент отсутсвует  
    rtU.barometricAvailable     = SelfTesting_STATUS(ST_sws);   // Высоту от СВС подставляем, но этим флагом заставляем игнорировать
    /* Данные от радиостанции */
    rtU.radioPointLatitude      = Bup_getRadioPointLatitude();
    rtU.radioPointLongitude     = Bup_getRadioPointLongitude();
    rtU.radioPointAltitude      = Bup_getTouchdownPointAltitude();        // В радиостанция не присылаем высоту открытия парашюта, поэтому клонируем
    rtU.radioUpdateIndex        = Bup_getRadioPointUpdateIndex();
    /* Данные карты */
    rtU.currentPointRelief          = Bup_getCurrentPointRelief();
    rtU.currentPointReliefAvailable = SelfTesting_STATUS(ST_MapAvailability);
    rtU.touchdownPointRelief        = Bup_getTouchdownPointRelief();
    rtU.radioPointRelief            = Bup_getRadioPointRelief();
}

/***************************************************************************
  MathModel_control - Выполнение команд мат. модели    
***************************************************************************/
void MathModel_control (void)
{	
  if (rtY.touchdown == 0)                 // Команды на посадку не было, поэтому будем управлять стропами
    MathModel_sendBimCommand ((uint8_t)rtY.leftStrap, (uint8_t)rtY.rightStrap);
  else                                   // Команда на посадку, завершаем работу
  { 
    Timer_deinitialize (MMODEL_TIMER);   // Отключаем таймер матмодели
    MathModel_sendBimCommand (0, 0);     // Переводим БИМЫ в состояние по умолчанию
    TOUCHDOWN_PYRO_ON();                 // Замок створки открыть
  }
}

/*************************************************************************************
    MathModel_sendBimCommand - Функция, предоставляющая мат модели управление БИМами
*************************************************************************************/
void MathModel_sendBimCommand (uint8_t left, uint8_t right)
{
  // Запретим управление БИМ, если шпилька 1 вставлена или реле выключено
  if(SelfTesting_STATUS(ST_pin1) == ST_OK || SelfTesting_STATUS(ST_POW_BIM) != ST_OK)
    return;
  
  if(left > 100)                    // Если процент затяжки больше 100, чего быть не должно,
    left = 100;                     // то ограничиваем сверху
  if(right > 100)
    right = 100;
  left = BIM_persentToPosition(left);      // Переводим в байтовый диапазон 
  right = BIM_persentToPosition(right);    // [0...255]
  
  /* Этот каскад условий необходим, чтобы: 
  1) Запретить одновременную работу двигателей, принудительным переходом через ноль
  (если было: левый 50%, а сейчас пришла команда правый 50%, то сначала левый переводится в ноль,
  а только потом, правый переводится в 50%);		
  */
  
  if(left > 0 && right == 0) // нужно затянуть левый БИМ в положение left и отпустить правый в 0
  {
    if (BIM_getStrapPosition (RIGHT_BIM) != 0)     // Требуется затянуть левый БИМ, правый при этом должен быть в положении 0 
      BIM_controlCommand (RIGHT_BIM, 0);           // Правый не в нуле, поэтому пока только переведём его в ноль
    else                                           // Правый действительно находится в нуле, 
      BIM_controlCommand(LEFT_BIM, left);          // можно отдать команду на затяжку левому
  }
  else if (right > 0 && left == 0) // нужно затянуть правый БИМ в положение right и отпустить левый в 0
  {
    if(BIM_getStrapPosition (LEFT_BIM) != 0)       // Требуется затянуть правый БИМ, левый при этом должен быть в положении 0 
      BIM_controlCommand(LEFT_BIM, 0);             // Левый не в нуле, поэтому пока только переведём его в ноль	
    else                                           // Левый действительно находится в нуле, 
      BIM_controlCommand(RIGHT_BIM, right);        // можно отдать команду на затяжку правому
  }
  else if (right > 0 && left > 0)                  // нужно затянуть оба БИМа
  { 
    BIM_controlCommand(RIGHT_BIM, right); 
    BIM_controlCommand(LEFT_BIM, left);
  }
  else // Нужно оба бима опустить в 0
  {
    BIM_controlCommand(LEFT_BIM, 0);
    BIM_controlCommand(RIGHT_BIM, 0);
  }
}
