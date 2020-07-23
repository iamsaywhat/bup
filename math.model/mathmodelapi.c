#include "mathmodelapi.h"

#include "bims.h"
#include "discreteio.h"
#include "../selftesting.h"
#include "otherlib.h"
#include "../config.h"
#include "../bupdatastorage.h"

#ifdef flightRegulatorCFB //******************************************************* Если выбран flightRegulatorCFB
  #include "flightRegulatorCFB/flightRegulatorCFB.h"
#else //*************************************************************************** Если выбран flightController
  #include "flightController/flightController.h"
#endif //************************************************************************** !flightRegulatorCFB 

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
  MathModel_init - Инициализация мат. модели    
***************************************************************************/
void MathModel_initialize(void)
{
  // Переведем БИМы в состояние 0
  MathModel_sendBimCommand (0, 0);
	
  #ifdef flightRegulatorCFB //******************************************************* Если выбран flightRegulatorCFB
    flightRegulatorCFB_initialize();
  #else //*************************************************************************** Если выбран flightController
    flightController_initialize();
  #endif //************************************************************************** !flightRegulatorCFB
	
  // Запускаем таймер обслуживания мат модели (период дискретизации 1 с)
  Timer_initialize (MMODEL_TIMER, SECOND_TICKS);
}

/***************************************************************************
  MathModel_step - Совершить шаг расчета    
***************************************************************************/
void MathModel_step(void)
{
  #ifdef flightRegulatorCFB //******************************************************* Если выбран flightRegulatorCFB
    flightRegulatorCFB_step();
  #else //*************************************************************************** Если выбран flightController
    flightController_step();
  #endif //************************************************************************** !flightRegulatorCFB
}

/***************************************************************************
  MathModel_prepareData - Заполнение входных данных для мат. модели    
***************************************************************************/
void MathModel_prepareData (void)
{
  #ifdef flightRegulatorCFB //******************************************************* Если выбран flightRegulatorCFB
    // Координаты точки приземления (подгружаем из памяти)
    rtU.xyzPoints[lat] = Bup_getTouchdownPointLatitude();
    rtU.xyzPoints[lon] = Bup_getTouchdownPointLongitude();
    rtU.xyzPoints[alt] = 0;
    // Текущие координаты:
    rtU.XYZi[lat] = Bup_getCurrentPointLatitude();
    rtU.XYZi[lon] = Bup_getCurrentPointLongitude();
    rtU.XYZi[alt] = Bup_getCurrentPointAltitude();
    // Путевой курс
    rtU.angle = Bup_getCurrentCourse();
    // Статус навигационного решения от СНС
    rtU.isVeracityGns = 1; //SNS_PosData.Quality;
    // Подставляем статус доступности карты
    rtU.isAvailableDigitalTerrain = SelfTesting_STATUS(ST_MapAvailability);
    rtU.HighDigitalTerrain = Bup_getCurrentPointRelief();
    rtU.highStopUPS = 300.0;
	
	
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

  #else //*************************************************************************** Если выбран flightController  
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
    /* Данные от свс */
    rtU.barometricAirSpeed      = SWS_getTrueSpeed();          // На время испытаний на стенде устанавливаем константу
    rtU.barometricAltitude      = SWS_getAbsoluteHeight();           // так, как имитатор СВС в настоящий момент отсутсвует  
    rtU.barometricAvailable     = SelfTesting_STATUS(ST_sws);   // Высоту от СВС подставляем, но этим флагом заставляем игнорировать
    /* Данные от радиостанции */
    rtU.radioPointLatitude      = Bup_getRadioPointLatitude();
    rtU.radioPointLongitude     = Bup_getRadioPointLongitude();
    rtU.radioPointAltitude      = Bup_getTouchdownPointAltitude();        // В радиостанция не присылаем высоту открытия парашюта, поэтому клонируем
    rtU.radioUpdateIndex        = Bup_getRadioPointUpdateIndex();
    /* Данные карты */
    rtU.currentPointRelief          = 0; //Bup_getCurrentPointRelief();
    rtU.currentPointReliefAvailable = 0; //SelfTesting_STATUS(ST_MapAvailability);
    rtU.touchdownPointRelief        = 0; //Bup_getTouchdownPointRelief();
    rtU.radioPointRelief            = 0; //Bup_getRadioPointRelief();
  #endif //************************************************************************** !flightRegulatorCFB 
}

/***************************************************************************
  MathModel_control - Выполнение команд мат. модели    
***************************************************************************/
void MathModel_control (void)
{	
  #ifdef flightRegulatorCFB	//******************************************************* Если выбран flightRegulatorCFB	
    // Если команда на посадку не пришла, мы еще в полете, будем управлять
    uint8_t left = 0;
    uint8_t right = 0;
    if(rtY.cmdTouchDown == 0)
    {
      // Управление БИМами по выходу модели
      if(rtY.directionOfRotation == -1)
      {
        left= (uint8_t)rtY.tightenSling;
        right = 0;
      }
      else if(rtY.directionOfRotation == 1)
      {
        right = (uint8_t)rtY.tightenSling;
        left = 0;
      }
      else if(rtY.directionOfRotation == 2)
      {
        right = (uint8_t)rtY.tightenSling;
        left = (uint8_t)rtY.tightenSling;
      }
      MathModel_sendBimCommand (left, right);
    }
    // Это команда на посадку 
    else if (rtY.cmdTouchDown == 1)
    {
  #else //*************************************************************************** Если выбран flightController
    // Команды на посадку не было, поэтому будем управлять стропами
    if (rtY.touchdown == 0)
      MathModel_sendBimCommand ((uint8_t)rtY.leftStrap, (uint8_t)rtY.rightStrap);
		
    // Команда на посадку, завершаем работу
    else if (rtY.touchdown == 1)
    {
  #endif //************************************************************************** !flightRegulatorCFB 			
    // Переводим БИМЫ в состояние по умолчанию
    MathModel_sendBimCommand (0, 0);
    // Замок створки открыть
    TOUCHDOWN_PYRO_ON(); 
    SelfTesting_TDS();
    
    #ifdef LOGS_ENABLE
      logger_warning("the flight is over");
      logger_point("final", Bup_getCurrentPointLatitude(), 
                  Bup_getCurrentPointLongitude(), Bup_getCurrentPointAltitude());
    #endif
    
    delay_ms(10000);         // Ждем 10 секунд
    TOUCHDOWN_PYRO_OFF();    // Отключаем реле створки замка (нельзя удерживать дольше 10 секунд)
    SelfTesting_TDS();
    
    BIM_disable();           // Отключаем БИМы
    SelfTesting_POW_BIM();
    while(1);                // Повисаем в ожидании перезапуска
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
