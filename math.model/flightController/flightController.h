/*
 * File: flightController.h
 *
 * Code generated for Simulink model 'flightController'.
 *
 * Model version                  : 1.832
 * Simulink Coder version         : 8.14 (R2018a) 06-Feb-2018
 * C/C++ source code generated on : Fri Feb  7 17:19:33 2020
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
  real_T TmpSignalConversionAtSFunctionI[3];/* '<S1>/LogicController' */
  real_T TmpSignalConversionAtSFunctio_g[3];/* '<S1>/LogicController' */
  real_T targetPoint[3];               /* '<S1>/LogicController' */
  real_T touchdownPoint_Offset[3];     /* '<S1>/LogicController' */
  real_T initialPoint[3];              /* '<S1>/LogicController' */
  real_T windForce;                    /* '<S9>/Chart' */
  real_T windCourse;                   /* '<S9>/Chart' */
  real_T desiredCourse;                /* '<S1>/LogicController' */
  real_T LastPos_1_DSTATE;             /* '<S17>/LastPos' */
  real_T LastPos_2_DSTATE;             /* '<S17>/LastPos' */
  real_T PreviousBearing_DSTATE;       /* '<S20>/PreviousBearing' */
  real_T initialGroundSpeed;           /* '<S9>/Chart' */
  real_T currentGroundSpeed;           /* '<S9>/Chart' */
  real_T initialCourse;                /* '<S9>/Chart' */
  real_T currentCourse;                /* '<S9>/Chart' */
  real_T LastLineState;                /* '<S40>/BimTrigger' */
  real_T centralCourse;                /* '<S1>/LogicController' */
  real_T courseToTargetPoint;          /* '<S1>/LogicController' */
  uint32_T time;                       /* '<S1>/LogicController' */
  uint32_T timeout;                    /* '<S1>/LogicController' */
  uint32_T timeout_b;                  /* '<S1>/LogicController' */
  uint8_T enableTargetting;            /* '<S1>/LogicController' */
  uint8_T is_active_c5_flightController;/* '<S9>/Chart' */
  uint8_T is_c5_flightController;      /* '<S9>/Chart' */
  uint8_T is_active_c9_BupSimulinkLibrari;/* '<S40>/BimTrigger' */
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
  boolean_T DsblTg_DSTATE;             /* '<S9>/DsblTg' */
  boolean_T side;                      /* '<S1>/LogicController' */
  boolean_T justReturnedHere;          /* '<S1>/LogicController' */
}
DW;

/* External inputs (root inport signals with default storage) */
typedef struct
{
  real_T TDP_lat;                      /* '<Root>/TDP_lat' */
  real_T TDP_lon;                      /* '<Root>/TDP_lon' */
  real_T TDP_alt;                      /* '<Root>/TDP_alt' */
  int16_T Relief;                      /* '<Root>/relief' */
  uint8_T ReliefAvailable;             /* '<Root>/reliefAvailable' */
  int16_T ReliefOnTDP;                 /* '<Root>/reliefOnTdp' */
  real_T Pos_lat;                      /* '<Root>/Pos_lat' */
  real_T Pos_lon;                      /* '<Root>/Pos_lon' */
  real_T Pos_alt;                      /* '<Root>/Pos_alt' */
  real_T ActualCourse;                 /* '<Root>/trackingCourse' */
  real_T LatitudeVelocity;             /* '<Root>/velocityLatitude' */
  real_T LongitudeVelocity;            /* '<Root>/velocityLongitude' */
  real_T AltitudeVelocity;             /* '<Root>/velocityAltitude' */
  real_T barometricAltitude;           /* '<Root>/barometricAltitude' */
  uint8_T barometricAvailable;         /* '<Root>/barometricAvailable' */
}
ExtU;

/* External outputs (root outports fed by signals with default storage) */
typedef struct
{
  real_T bimCommand;                   /* '<Root>/bimCommand' */
  uint8_T touchdown;                   /* '<Root>/touchdown' */
  real_T horizontalDistance;           /* '<Root>/horizontalDistance' */
  real_T onTargetTime;                 /* '<Root>/onTargetTime' */
  real_T touchdownTime;                /* '<Root>/touchdownTime' */
  int8_T bimSelect;                    /* '<Root>/bimSelect' */
  uint8_T bimForce;                    /* '<Root>/bimForce' */
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
 * Block '<S17>/Display' : Unused code path elimination
 * Block '<S17>/Display1' : Unused code path elimination
 * Block '<S21>/Gain' : Unused code path elimination
 * Block '<S22>/Gain' : Unused code path elimination
 * Block '<S17>/Scope1' : Unused code path elimination
 * Block '<S17>/Scope8' : Unused code path elimination
 * Block '<S17>/Scope9' : Unused code path elimination
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
 * Block '<S1>/Scope9' : Unused code path elimination
 * Block '<S31>/Gain' : Unused code path elimination
 * Block '<S32>/Gain' : Unused code path elimination
 * Block '<S33>/Gain' : Unused code path elimination
 * Block '<S35>/Scope' : Unused code path elimination
 * Block '<S7>/Scope' : Unused code path elimination
 * Block '<S7>/Scope2' : Unused code path elimination
 * Block '<S7>/Scope5' : Unused code path elimination
 * Block '<S7>/Scope6' : Unused code path elimination
 * Block '<S7>/Scope7' : Unused code path elimination
 * Block '<S7>/Scope8' : Unused code path elimination
 * Block '<S8>/Scope' : Unused code path elimination
 * Block '<S9>/Scope2' : Unused code path elimination
 * Block '<S9>/SkipSolution' : Unused code path elimination
 * Block '<S3>/CourseSwitch' : Eliminated due to constant selection input
 * Block '<S17>/Manual Switch2' : Eliminated due to constant selection input
 * Block '<S3>/Manual Switch' : Eliminated due to constant selection input
 * Block '<S3>/Manual Switch1' : Eliminated due to constant selection input
 * Block '<S1>/TD_SysSwitch' : Eliminated due to constant selection input
 * Block '<S35>/Manual Switch' : Eliminated due to constant selection input
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
 * '<S7>'   : 'fullSystemModel/flightController/Targeting'
 * '<S8>'   : 'fullSystemModel/flightController/altitudeWeighting'
 * '<S9>'   : 'fullSystemModel/flightController/feedback'
 * '<S10>'  : 'fullSystemModel/flightController/BimControllaNDriftCalc/BimTriggers'
 * '<S11>'  : 'fullSystemModel/flightController/BimControllaNDriftCalc/DriftEstimator'
 * '<S12>'  : 'fullSystemModel/flightController/BimControllaNDriftCalc/PointMoventPlot'
 * '<S13>'  : 'fullSystemModel/flightController/BimControllaNDriftCalc/PreemptionTDP'
 * '<S14>'  : 'fullSystemModel/flightController/BimControllaNDriftCalc/BimTriggers/BimTrigger'
 * '<S15>'  : 'fullSystemModel/flightController/BimControllaNDriftCalc/PointMoventPlot/MATLAB Function'
 * '<S16>'  : 'fullSystemModel/flightController/BimControllaNDriftCalc/PreemptionTDP/MATLAB Function'
 * '<S17>'  : 'fullSystemModel/flightController/DataAnalyzer/InternalTrackingCourse'
 * '<S18>'  : 'fullSystemModel/flightController/DataAnalyzer/InternalTrackingSpeed'
 * '<S19>'  : 'fullSystemModel/flightController/DataAnalyzer/InternalTrackingCourse/Azimuth'
 * '<S20>'  : 'fullSystemModel/flightController/DataAnalyzer/InternalTrackingCourse/Bearing'
 * '<S21>'  : 'fullSystemModel/flightController/DataAnalyzer/InternalTrackingCourse/Radians to Degrees1'
 * '<S22>'  : 'fullSystemModel/flightController/DataAnalyzer/InternalTrackingCourse/Radians to Degrees2'
 * '<S23>'  : 'fullSystemModel/flightController/DataAnalyzer/InternalTrackingCourse/Azimuth/Azimut'
 * '<S24>'  : 'fullSystemModel/flightController/DataAnalyzer/InternalTrackingCourse/Bearing/Heading_true'
 * '<S25>'  : 'fullSystemModel/flightController/DataAnalyzer/InternalTrackingSpeed/GPSVelocity'
 * '<S26>'  : 'fullSystemModel/flightController/DataAnalyzer/InternalTrackingSpeed/ProjectionSpeed'
 * '<S27>'  : 'fullSystemModel/flightController/DataAnalyzer/InternalTrackingSpeed/GPSVelocity/Velocity'
 * '<S28>'  : 'fullSystemModel/flightController/DataAnalyzer/InternalTrackingSpeed/ProjectionSpeed/Speed'
 * '<S29>'  : 'fullSystemModel/flightController/Targeting/ControlDemode'
 * '<S30>'  : 'fullSystemModel/flightController/Targeting/MATLAB Function'
 * '<S31>'  : 'fullSystemModel/flightController/Targeting/Radians to Degrees1'
 * '<S32>'  : 'fullSystemModel/flightController/Targeting/Radians to Degrees2'
 * '<S33>'  : 'fullSystemModel/flightController/Targeting/Radians to Degrees3'
 * '<S34>'  : 'fullSystemModel/flightController/Targeting/Radians to Degrees4'
 * '<S35>'  : 'fullSystemModel/flightController/Targeting/Roughening'
 * '<S36>'  : 'fullSystemModel/flightController/Targeting/TimeNAngleTurning'
 * '<S37>'  : 'fullSystemModel/flightController/Targeting/ControlDemode/ControlDemode'
 * '<S38>'  : 'fullSystemModel/flightController/Targeting/Roughening/DeadZone'
 * '<S39>'  : 'fullSystemModel/flightController/Targeting/Roughening/DeadZone/DeadZone'
 * '<S40>'  : 'fullSystemModel/flightController/feedback/BimTriggers'
 * '<S41>'  : 'fullSystemModel/flightController/feedback/Chart'
 * '<S42>'  : 'fullSystemModel/flightController/feedback/PointMoventPlot'
 * '<S43>'  : 'fullSystemModel/flightController/feedback/PreemptionTDP'
 * '<S44>'  : 'fullSystemModel/flightController/feedback/BimTriggers/BimTrigger'
 * '<S45>'  : 'fullSystemModel/flightController/feedback/PointMoventPlot/MATLAB Function'
 * '<S46>'  : 'fullSystemModel/flightController/feedback/PreemptionTDP/MATLAB Function'
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
