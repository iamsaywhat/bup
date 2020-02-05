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
  #include	"../loger.h"
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
    rtU.xyzPoints[lat] = Bup_getTouchdownLatitude();
    rtU.xyzPoints[lon] = Bup_getTouchdownLongitude();
    rtU.xyzPoints[alt] = 0;
    // Текущие координаты:
    rtU.XYZi[lat] = Bup_getLatitude();
    rtU.XYZi[lon] = Bup_getLongitude();
    rtU.XYZi[alt] = Bup_getAltitude();
    // Путевой курс
    rtU.angle = Bup_getCourse();
    // Статус навигационного решения от СНС
    rtU.isVeracityGns = 1; //SNS_PosData.Quality;
    // Подставляем статус доступности карты
    rtU.isAvailableDigitalTerrain = SelfTesting_STATUS(ST_MapAvailability);
    rtU.HighDigitalTerrain = Bup_getReliefHeight();
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
    rtU.TDP_lon            = Bup_getTouchdownLongitude();
    rtU.TDP_lat            = Bup_getTouchdownLatitude();
    rtU.TDP_alt            = Bup_getTouchdownAltitude();
    rtU.Pos_lon            = Bup_getLongitude();
    rtU.Pos_lat            = Bup_getLatitude();
    rtU.Pos_alt            = Bup_getAltitude();
    rtU.ActualCourse       = Bup_getCourse();
    rtU.Relief             = Bup_getReliefHeight();
    rtU.ReliefOnTDP        = Bup_getReliefHeightOnTDP();
    rtU.ReliefAvailable    = SelfTesting_STATUS(ST_MapAvailability);
    rtU.LatitudeVelocity   = Bup_getVelocityLatitude();
    rtU.LongitudeVelocity  = Bup_getVelocityLongitude();
    rtU.AltitudeVelocity   = Bup_getVelocityAltitude();
  #endif //************************************************************************** !flightRegulatorCFB 
}

/***************************************************************************
  MathModel_control - Выполнение команд мат. модели    
***************************************************************************/
void MathModel_control (void)
{	
  #ifdef flightRegulatorCFB	//******************************************************* Если выбран flightRegulatorCFB	
    // Если команда на посадку не пришла, мы еще в полете, будем управлять
    if(rtY.cmdTouchDown == 0)
    {
      // Управление БИМами по выходу модели
      MathModel_sendBimCommand ((int8_t)(rtY.directionOfRotation), 
                                (uint8_t)(rtY.tightenSling + 0.5));
    }
    // Это команда на посадку 
    else if (rtY.cmdTouchDown == 1)
    {
  #else //*************************************************************************** Если выбран flightController
    // Команды на посадку не было, поэтому будем управлять стропами
    if (rtY.TD_CMD == 0)
      MathModel_sendBimCommand ((int8_t)((rtY.BIM_CMD > 0) - (rtY.BIM_CMD < 0)),
                                (uint8_t)(fabs(rtY.BIM_CMD)+0.5));
		
    // Команда на посадку, завершаем работу
    else if (rtY.TD_CMD == 1)
    {
  #endif //************************************************************************** !flightRegulatorCFB 			
    // Переводим БИМЫ в состояние по умолчанию
    MathModel_sendBimCommand (0, 0);
    // Замок створки открыть
    BLIND_CTRL_ON();
			
    #ifdef LOGS_ENABLE	//************************************************************* Если включено логирование в черный ящик
      // Пишем в лог информацию о завершении полета и последние данные
      loger_exitmsg();
    #endif //************************************************************************** !LOGS_ENABLE

    // Ждем 5 секунд
    delay_us(5000000);
    // Отключаем реле створки замка (нельзя удерживать дольше 10 секунд)
    BLIND_CTRL_OFF();
    // Отключаем БИМы
    BIM_disableSupply();
    // Повисаем в ожидании перезапуска
    while(1);
  }
}

/*************************************************************************************
    MathModel_sendBimCommand - Функция, предоставляющая мат модели управление БИМами
*************************************************************************************/
void MathModel_sendBimCommand (int8_t side, uint8_t forcePersent)
{
  uint8_t force;
	
  // Запретим управление БИМ, если шпилька 1 вставлена или реле выключено
  if(SelfTesting_STATUS(ST_pin1) == ST_OK || SelfTesting_STATUS(ST_POW_BIM) != ST_OK)
    return;

  if(forcePersent > 100)                  // Если процент затяжки больше 100, чего быть не должно,
    force = 255;                          // то ограничиваем сверху
  else                                    // Если диапазон процентов соответствует
    force = (uint8_t)(2.55*forcePersent); // то переводим в шкалу [0...255]
	
  switch (side)
  {
    case -1:  // Отрицательное значение означает, что нужно затянуть левый БИМ в положение SIDE и отпустить правый
    {
      /* Этот каскад условий необходим, чтобы: 
      1) Запретить одновременную работу двигателей, принудительным переходом через ноль
      (если было: левый 50%, а сейчас пришла команда правый 50%, то сначала левый переводится в ноль,
      а только потом, правый переводится в 50%);
      2) Контролировать текущее (реальное) положение двигателей, чтобы избавиться от шелчков, возникающих
      при попытке перевести двигатель в положение в котором он уже находится;		
      */
      if (BIM_getStrapPosition (RIGHT_BIM) != 0)                    // Требуется затянуть левый БИМ, правый при этом должен быть в положении 0 
        BIM_sendRequest (RIGHT_BIM, BIM_CMD_ON, 0, 9, 255, 255);    // Правый не в нуле, поэтому пока только переведём его в ноль
    
      else  // Правый действительно находится в нуле, можно отдать команду на затяжку левому
      {
        if(BIM_getStrapPosition (LEFT_BIM) != force)
          BIM_sendRequest (LEFT_BIM, BIM_CMD_ON, force, 7, 255, 255);    // Устанавливать новое положение, будем только если оно отличается от старого (защита от щелчков)
      }
      break;
		}
		case 0:  // Это значит, что нужно ослабить оба БИМА
    {
      if(BIM_getStrapPosition (LEFT_BIM) != 0)
        BIM_sendRequest (LEFT_BIM, BIM_CMD_ON, 0, 7, 255, 255);
			
      if(BIM_getStrapPosition (RIGHT_BIM) != 0)
        BIM_sendRequest (RIGHT_BIM, BIM_CMD_ON, 0, 9, 255, 255);

      break;
		}
    case 1:  // Положительное значение означает, что нужно затянуть правый БИМ в положение SIDE, и отпустить левый
    {
      /* Этот каскад условий необходим, чтобы: 
      1) Запретить одновременную работу двигателей, принудительным переходом через ноль
      (если было: левый 50%, а сейчас пришла команда правый 50%, то сначала левый переводится в ноль,
      а только потом, правый переводится в 50%);
      2) Контролировать текущее (реальное) положение двигателей, чтобы избавиться от шелчков, возникающих
      при попытке перевести двигатель в положение в котором он уже находится;		
      */
      if(BIM_getStrapPosition (LEFT_BIM) != 0)                   // Требуется затянуть правый БИМ, левый при этом должен быть в положении 0 
        BIM_sendRequest (LEFT_BIM, BIM_CMD_ON, 0, 7, 255, 255);  // Левый не в нуле, поэтому пока только переведём его в ноль
			
      else // Левый действительно находится в нуле, можно отдать команду на затяжку правому
      {
        if(BIM_getStrapPosition (RIGHT_BIM) != force)                   // Устанавливать новое положение, будем только 
        BIM_sendRequest (RIGHT_BIM, BIM_CMD_ON, force, 9, 255, 255);  // если оно отличается от старого (защита от щелчков)
      }
      break;
		}
    case 2:  // Затягиваем обе стропы
    {
      if(BIM_getStrapPosition (LEFT_BIM) != force)
        BIM_sendRequest (LEFT_BIM, BIM_CMD_ON, force, 7, 255, 255);
			
      if(BIM_getStrapPosition (RIGHT_BIM) != force)
        BIM_sendRequest (RIGHT_BIM, BIM_CMD_ON, force, 9, 255, 255);
				
      break;
		}
		default:
      break;
  }
  // БИМы не сразу обновляют своё состояние по CAN, 
  // поэтому заставляем их 5 раз сообщить своё состояние
  for(uint8_t i = 0; i < 5; i++)
  {
    SelfTesting_RIGHT_BIM();
    SelfTesting_LEFT_BIM();
  }
}
