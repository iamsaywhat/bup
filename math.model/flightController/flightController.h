/*
 * File: flightController.h
 *
 * Code generated for Simulink model 'flightController'.
 *
 * Model version                  : 1.904
 * Simulink Coder version         : 8.14 (R2018a) 06-Feb-2018
 * C/C++ source code generated on : Mon Aug 17 13:24:13 2020
 *
 * Target selection: ert.tlc
 * Embedded hardware selection: ARM Compatible->ARM Cortex
 * Code generation objectives:
 *    1. Execution efficiency
 *    2. RAM efficiency
 * Validation result: Not run
 */

#ifndef RTW_HEADER_flightController_h_
#define RTW_HEADER_flightController_h_
#include <stddef.h>
#include <float.h>
#include <math.h>
#ifndef flightController_COMMON_INCLUDES_
# define flightController_COMMON_INCLUDES_
#include "rtwtypes.h"
#endif                                 /* flightController_COMMON_INCLUDES_ */

/* Macros for accessing real-time model data structure */
#ifndef rtmGetErrorStatus
# define rtmGetErrorStatus(rtm)        ((rtm)->errorStatus)
#endif

#ifndef rtmSetErrorStatus
# define rtmSetErrorStatus(rtm, val)   ((rtm)->errorStatus = (val))
#endif

/* Forward declaration for rtModel */
typedef struct tag_RTM RT_MODEL;

/* Block signals and states (default storage) for system '<Root>' */
typedef struct
{
  real_T targetPoint[4];               /* '<S1>/TargetSelector' */
  real_T Delay_DSTATE[4];              /* '<S11>/Delay' */
  real_T initialPoint[4];              /* '<S1>/LogicController' */
  real_T windForce;                    /* '<S11>/Chart2' */
  real_T windCourse;                   /* '<S11>/Chart2' */
  real_T desiredCourse;                /* '<S1>/LogicController' */
  real_T LastPos_1_DSTATE;             /* '<S13>/LastPos' */
  real_T LastPos_2_DSTATE;             /* '<S13>/LastPos' */
  real_T PreviousBearing_DSTATE;       /* '<S16>/PreviousBearing' */
  real_T initialGroundSpeed;           /* '<S11>/Chart2' */
  real_T currentGroundSpeed;           /* '<S11>/Chart2' */
  real_T initialCourse;                /* '<S11>/Chart2' */
  real_T currentCourse;                /* '<S11>/Chart2' */
  real_T currentGroundSpeed_d;         /* '<S11>/Chart' */
  real_T currentCourse_h;              /* '<S11>/Chart' */
  real_T LastLineState;                /* '<S37>/BimTrigger' */
  real_T centralCourse;                /* '<S1>/LogicController' */
  real_T courseToTargetPoint;          /* '<S1>/LogicController' */
  int32_T sfEvent;                     /* '<S1>/LogicController' */
  uint32_T passedTime;                 /* '<S11>/Chart2' */
  uint32_T time;                       /* '<S1>/LogicController' */
  uint32_T timeout;                    /* '<S1>/LogicController' */
  uint32_T timeout_b;                  /* '<S1>/LogicController' */
  uint8_T enableTargetting;            /* '<S1>/LogicController' */
  uint8_T is_active_c8_flightController;/* '<S11>/Chart2' */
  uint8_T is_c8_flightController;      /* '<S11>/Chart2' */
  uint8_T is_active_c5_flightController;/* '<S11>/Chart' */
  uint8_T is_c5_flightController;      /* '<S11>/Chart' */
  uint8_T is_active_c9_BupSimulinkLibrari;/* '<S37>/BimTrigger' */
  uint8_T is_active_c6_flightController;/* '<S1>/TargetSelector' */
  uint8_T is_c6_flightController;      /* '<S1>/TargetSelector' */
  uint8_T updateIndex;                 /* '<S1>/TargetSelector' */
  uint8_T is_active_c1_flightController;/* '<S1>/LogicController' */
  uint8_T is_active_Timer;             /* '<S1>/LogicController' */
  uint8_T is_Control;                  /* '<S1>/LogicController' */
  uint8_T is_active_Control;           /* '<S1>/LogicController' */
  uint8_T is_Flight;                   /* '<S1>/LogicController' */
  uint8_T was_Flight;                  /* '<S1>/LogicController' */
  uint8_T is_StepAngle;                /* '<S1>/LogicController' */
  uint8_T is_Touchdown;                /* '<S1>/LogicController' */
  uint8_T is_active_Touchdown;         /* '<S1>/LogicController' */
  uint8_T is_active_EventGenerator;    /* '<S1>/LogicController' */
  uint8_T is_BimSupply;                /* '<S1>/LogicController' */
  uint8_T is_active_BimSupply;         /* '<S1>/LogicController' */
  boolean_T DsblTg_DSTATE;             /* '<S11>/DsblTg' */
  boolean_T status;                    /* '<S1>/TargetSelector' */
  boolean_T side;                      /* '<S1>/LogicController' */
  boolean_T justReturnedHere;          /* '<S1>/LogicController' */
}
DW;

/* External inputs (root inport signals with default storage) */
typedef struct
{
  real_T touchdownPointLatitude;       /* '<Root>/touchdownPoint:Latitude' */
  real_T touchdownPointLongitude;      /* '<Root>/touchdownPoint:Longitude' */
  real_T touchdownPointAltitude;       /* '<Root>/touchdownPoint:Altitude' */
  real_T radioPointLatitude;           /* '<Root>/radioPoint:Latitude' */
  real_T radioPointLongitude;          /* '<Root>/radioPoint:Longitude' */
  real_T radioPointAltitude;           /* '<Root>/radioPoint:Altitude' */
  uint8_T radioUpdateIndex;            /* '<Root>/radioUpdateIndex' */
  int16_T currentPointRelief;          /* '<Root>/currentPointRelief' */
  uint8_T currentPointReliefAvailable; /* '<Root>/currentReliefAvailable' */
  int16_T touchdownPointRelief;        /* '<Root>/touchdownPointRelief' */
  int16_T radioPointRelief;            /* '<Root>/radioPointRelief' */
  uint8_T barometricAvailable;         /* '<Root>/barometricAvailable' */
  uint8_T gpsAvailable;                /* '<Root>/gpsAvailable' */
  real_T currentPointLatitude;         /* '<Root>/currentPoint:Latitude' */
  real_T currentPointLongitude;        /* '<Root>/currentPoint:Longitude' */
  real_T currentPointAltitude;         /* '<Root>/currentPoint:Altitude' */
  real_T trackingCourse;               /* '<Root>/trackingCourse' */
  real_T velocityLatitude;             /* '<Root>/velocityLatitude' */
  real_T velocityLongitude;            /* '<Root>/velocityLongitude' */
  real_T velocityAltitude;             /* '<Root>/velocityAltitude' */
  real_T barometricAltitude;           /* '<Root>/barometricAltitude' */
  real_T barometricAirSpeed;           /* '<Root>/barometricAirSpeed' */
}
ExtU;

/* External outputs (root outports fed by signals with default storage) */
typedef struct
{
  real_T rightStrap;                   /* '<Root>/rightStrap' */
  real_T leftStrap;                    /* '<Root>/leftStrap' */
  uint8_T touchdown;                   /* '<Root>/touchdown' */
  real_T horizontalDistance;           /* '<Root>/horizontalDistance' */
  real_T horizontalTime;               /* '<Root>/horizontalTime' */
  real_T verticalTime;                 /* '<Root>/verticalTime' */
  real_T windForce;                    /* '<Root>/windForce' */
  real_T windCourse;                   /* '<Root>/windCourse' */
  real_T horizontalSpeed;              /* '<Root>/horizontalSpeed' */
}
ExtY;

/* Real-time Model Data Structure */
struct tag_RTM
{
  const char_T * volatile errorStatus;
};

/* Block signals and states (default storage) */
extern DW rtDW;

/* External inputs (root inport signals with default storage) */
extern ExtU rtU;

/* External outputs (root outports fed by signals with default storage) */
extern ExtY rtY;

/* Model entry point functions */
extern void flightController_initialize(void);
extern void flightController_step(void);

/* Real-time Model object */
extern RT_MODEL *const rtM;

/*-
 * These blocks were eliminated from the model due to optimizations:
 *
 * Block '<S3>/Display' : Unused code path elimination
 * Block '<S3>/Display1' : Unused code path elimination
 * Block '<S13>/Display' : Unused code path elimination
 * Block '<S13>/Display1' : Unused code path elimination
 * Block '<S17>/Gain' : Unused code path elimination
 * Block '<S18>/Gain' : Unused code path elimination
 * Block '<S13>/Scope1' : Unused code path elimination
 * Block '<S13>/Scope8' : Unused code path elimination
 * Block '<S13>/Scope9' : Unused code path elimination
 * Block '<S3>/Scope1' : Unused code path elimination
 * Block '<S3>/Scope2' : Unused code path elimination
 * Block '<S3>/Scope3' : Unused code path elimination
 * Block '<S1>/Display' : Unused code path elimination
 * Block '<S6>/Gain' : Unused code path elimination
 * Block '<S1>/Scope' : Unused code path elimination
 * Block '<S1>/Scope1' : Unused code path elimination
 * Block '<S1>/Scope2' : Unused code path elimination
 * Block '<S1>/Scope3' : Unused code path elimination
 * Block '<S1>/Scope4' : Unused code path elimination
 * Block '<S1>/Scope5' : Unused code path elimination
 * Block '<S1>/Scope6' : Unused code path elimination
 * Block '<S1>/Scope7' : Unused code path elimination
 * Block '<S1>/Scope8' : Unused code path elimination
 * Block '<S27>/Gain' : Unused code path elimination
 * Block '<S28>/Gain' : Unused code path elimination
 * Block '<S29>/Gain' : Unused code path elimination
 * Block '<S31>/Scope' : Unused code path elimination
 * Block '<S9>/Scope' : Unused code path elimination
 * Block '<S9>/Scope2' : Unused code path elimination
 * Block '<S9>/Scope5' : Unused code path elimination
 * Block '<S9>/Scope6' : Unused code path elimination
 * Block '<S9>/Scope7' : Unused code path elimination
 * Block '<S9>/Scope8' : Unused code path elimination
 * Block '<S3>/CourseSwitch' : Eliminated due to constant selection input
 * Block '<S13>/Manual Switch2' : Eliminated due to constant selection input
 * Block '<S3>/Manual Switch' : Eliminated due to constant selection input
 * Block '<S3>/Manual Switch1' : Eliminated due to constant selection input
 * Block '<S1>/TD_SysSwitch' : Eliminated due to constant selection input
 * Block '<S31>/Manual Switch' : Eliminated due to constant selection input
 * Block '<S1>/Constant' : Unused code path elimination
 * Block '<S3>/Constant1' : Unused code path elimination
 * Block '<S3>/Constant2' : Unused code path elimination
 */

/*-
 * The generated code includes comments that allow you to trace directly
 * back to the appropriate location in the model.  The basic format
 * is <system>/block_name, where system is the system number (uniquely
 * assigned by Simulink) and block_name is the name of the block.
 *
 * Note that this particular code originates from a subsystem build,
 * and has its own system numbers different from the parent model.
 * Refer to the system hierarchy for this subsystem below, and use the
 * MATLAB hilite_system command to trace the generated code back
 * to the parent model.  For example,
 *
 * hilite_system('fullSystemModel/flightController')    - opens subsystem fullSystemModel/flightController
 * hilite_system('fullSystemModel/flightController/Kp') - opens and selects block Kp
 *
 * Here is the system hierarchy for this model
 *
 * '<Root>' : 'fullSystemModel'
 * '<S1>'   : 'fullSystemModel/flightController'
 * '<S2>'   : 'fullSystemModel/flightController/BimControllaNDriftCalc'
 * '<S3>'   : 'fullSystemModel/flightController/DataAnalyzer'
 * '<S4>'   : 'fullSystemModel/flightController/Degrees to Radians'
 * '<S5>'   : 'fullSystemModel/flightController/LogicController'
 * '<S6>'   : 'fullSystemModel/flightController/Radians to Degrees'
 * '<S7>'   : 'fullSystemModel/flightController/StrapControl'
 * '<S8>'   : 'fullSystemModel/flightController/TargetSelector'
 * '<S9>'   : 'fullSystemModel/flightController/Targeting'
 * '<S10>'  : 'fullSystemModel/flightController/altitudeWeighting'
 * '<S11>'  : 'fullSystemModel/flightController/feedback'
 * '<S12>'  : 'fullSystemModel/flightController/BimControllaNDriftCalc/DriftEstimator'
 * '<S13>'  : 'fullSystemModel/flightController/DataAnalyzer/InternalTrackingCourse'
 * '<S14>'  : 'fullSystemModel/flightController/DataAnalyzer/InternalTrackingSpeed'
 * '<S15>'  : 'fullSystemModel/flightController/DataAnalyzer/InternalTrackingCourse/Azimuth'
 * '<S16>'  : 'fullSystemModel/flightController/DataAnalyzer/InternalTrackingCourse/Bearing'
 * '<S17>'  : 'fullSystemModel/flightController/DataAnalyzer/InternalTrackingCourse/Radians to Degrees1'
 * '<S18>'  : 'fullSystemModel/flightController/DataAnalyzer/InternalTrackingCourse/Radians to Degrees2'
 * '<S19>'  : 'fullSystemModel/flightController/DataAnalyzer/InternalTrackingCourse/Azimuth/Azimut'
 * '<S20>'  : 'fullSystemModel/flightController/DataAnalyzer/InternalTrackingCourse/Bearing/Heading_true'
 * '<S21>'  : 'fullSystemModel/flightController/DataAnalyzer/InternalTrackingSpeed/GPSVelocity'
 * '<S22>'  : 'fullSystemModel/flightController/DataAnalyzer/InternalTrackingSpeed/ProjectionSpeed'
 * '<S23>'  : 'fullSystemModel/flightController/DataAnalyzer/InternalTrackingSpeed/GPSVelocity/Velocity'
 * '<S24>'  : 'fullSystemModel/flightController/DataAnalyzer/InternalTrackingSpeed/ProjectionSpeed/Speed'
 * '<S25>'  : 'fullSystemModel/flightController/Targeting/ControlDemode'
 * '<S26>'  : 'fullSystemModel/flightController/Targeting/MATLAB Function'
 * '<S27>'  : 'fullSystemModel/flightController/Targeting/Radians to Degrees1'
 * '<S28>'  : 'fullSystemModel/flightController/Targeting/Radians to Degrees2'
 * '<S29>'  : 'fullSystemModel/flightController/Targeting/Radians to Degrees3'
 * '<S30>'  : 'fullSystemModel/flightController/Targeting/Radians to Degrees4'
 * '<S31>'  : 'fullSystemModel/flightController/Targeting/Roughening'
 * '<S32>'  : 'fullSystemModel/flightController/Targeting/TimeNAngleTurning'
 * '<S33>'  : 'fullSystemModel/flightController/Targeting/ControlDemode/ControlDemode'
 * '<S34>'  : 'fullSystemModel/flightController/Targeting/Roughening/DeadZone'
 * '<S35>'  : 'fullSystemModel/flightController/Targeting/Roughening/DeadZone/DeadZone'
 * '<S36>'  : 'fullSystemModel/flightController/altitudeWeighting/altitudeClarify'
 * '<S37>'  : 'fullSystemModel/flightController/feedback/BimTriggers'
 * '<S38>'  : 'fullSystemModel/flightController/feedback/Chart'
 * '<S39>'  : 'fullSystemModel/flightController/feedback/Chart2'
 * '<S40>'  : 'fullSystemModel/flightController/feedback/PointMoventPlot'
 * '<S41>'  : 'fullSystemModel/flightController/feedback/PreemptionTDP'
 * '<S42>'  : 'fullSystemModel/flightController/feedback/BimTriggers/BimTrigger'
 * '<S43>'  : 'fullSystemModel/flightController/feedback/PointMoventPlot/MATLAB Function'
 * '<S44>'  : 'fullSystemModel/flightController/feedback/PreemptionTDP/MATLAB Function'
 */

/*-
 * Requirements for '<Root>': flightController
 */
#endif                                 /* RTW_HEADER_flightController_h_ */

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
