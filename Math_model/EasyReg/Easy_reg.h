/*
 * File: Easy_reg.h
 *
 * Code generated for Simulink model 'Easy_reg'.
 *
 * Model version                  : 1.537
 * Simulink Coder version         : 8.14 (R2018a) 06-Feb-2018
 * C/C++ source code generated on : Wed Nov 27 18:58:35 2019
 *
 * Target selection: ert.tlc
 * Embedded hardware selection: ARM Compatible->ARM Cortex
 * Code generation objectives:
 *    1. Execution efficiency
 *    2. RAM efficiency
 * Validation result: Not run
 */

#ifndef RTW_HEADER_Easy_reg_h_
#define RTW_HEADER_Easy_reg_h_
#include <stddef.h>
#include <math.h>
#ifndef Easy_reg_COMMON_INCLUDES_
# define Easy_reg_COMMON_INCLUDES_
#include "rtwtypes.h"
#endif                                 /* Easy_reg_COMMON_INCLUDES_ */

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
  real_T TmpSignalConversionAtSFunctio_g[3];/* '<S1>/TaskController' */
  real_T TargetPoint[3];               /* '<S1>/TaskController' */
  real_T Point1[3];                    /* '<S1>/TaskController' */
  real_T Point2[3];                    /* '<S1>/TaskController' */
  real_T LUPoint[3];                   /* '<S1>/TaskController' */
  real_T LDPoint[3];                   /* '<S1>/TaskController' */
  real_T RUPoint[3];                   /* '<S1>/TaskController' */
  real_T RDPoint[3];                   /* '<S1>/TaskController' */
  real_T Course2Target;                /* '<S1>/TaskController' */
  real_T LastPos_1_DSTATE;             /* '<S14>/LastPos' */
  real_T LastPos_2_DSTATE;             /* '<S14>/LastPos' */
  real_T PreviousBearing_DSTATE;       /* '<S17>/PreviousBearing' */
  real_T ReferenceCourse;              /* '<S2>/DriftEstimator' */
  real_T LastLineState;                /* '<S7>/BimTrigger' */
  uint32_T timeout;                    /* '<S1>/TaskController' */
  uint32_T time;                       /* '<S1>/TaskController' */
  uint32_T timeout_p;                  /* '<S1>/TaskController' */
  uint8_T TargetEnable;                /* '<S1>/TaskController' */
  uint8_T is_active_c1_Easy_reg;       /* '<S1>/TaskController' */
  uint8_T is_active_Timer;             /* '<S1>/TaskController' */
  uint8_T is_Control;                  /* '<S1>/TaskController' */
  uint8_T is_active_Control;           /* '<S1>/TaskController' */
  uint8_T is_Flight;                   /* '<S1>/TaskController' */
  uint8_T was_Flight;                  /* '<S1>/TaskController' */
  uint8_T is_Box;                      /* '<S1>/TaskController' */
  uint8_T was_Box;                     /* '<S1>/TaskController' */
  uint8_T is_Touchdown;                /* '<S1>/TaskController' */
  uint8_T is_active_Touchdown;         /* '<S1>/TaskController' */
  uint8_T is_active_EventGenerator;    /* '<S1>/TaskController' */
  uint8_T is_BimSupply;                /* '<S1>/TaskController' */
  uint8_T is_active_BimSupply;         /* '<S1>/TaskController' */
  uint8_T is_StepAngle;                /* '<S1>/TaskController' */
  uint8_T BoxEntry;                    /* '<S1>/TaskController' */
  uint8_T is_active_c10_BupSimulinkLibrar;/* '<S2>/DriftEstimator' */
  uint8_T is_c10_BupSimulinkLibraries; /* '<S2>/DriftEstimator' */
  uint8_T is_active_c9_BupSimulinkLibrari;/* '<S7>/BimTrigger' */
  boolean_T DsblTg_DSTATE;             /* '<S2>/DsblTg' */
  boolean_T EnblTg_DSTATE;             /* '<S2>/EnblTg' */
  boolean_T LeftOrRight;               /* '<S1>/TaskController' */
}
DW;

/* External inputs (root inport signals with default storage) */
typedef struct
{
  real_T TDP_lat;                      /* '<Root>/TDP_lat' */
  real_T TDP_lon;                      /* '<Root>/TDP_lon' */
  real_T TDP_alt;                      /* '<Root>/TDP_alt' */
  int16_T Relief;                      /* '<Root>/Relief' */
  uint8_T ReliefAvailable;             /* '<Root>/ReliefAvailable' */
  int16_T ReliefOnTDP;                 /* '<Root>/ReliefOnTDP' */
  real_T Pos_lat;                      /* '<Root>/Pos_lat' */
  real_T Pos_lon;                      /* '<Root>/Pos_lon' */
  real_T Pos_alt;                      /* '<Root>/Pos_alt' */
  real_T ActualCourse;                 /* '<Root>/ActualCourse' */
  real_T LatitudeVelocity;             /* '<Root>/VelocityLatitude' */
  real_T LongitudeVelocity;            /* '<Root>/VelocityLongitude' */
  real_T AltitudeVelocity;             /* '<Root>/VelocityAltitude' */
}
ExtU;

/* External outputs (root outports fed by signals with default storage) */
typedef struct
{
  real_T BIM_CMD;                      /* '<Root>/BIM_CMD' */
  uint8_T TD_CMD;                      /* '<Root>/TD_CMD' */
  real_T DistanceToTDP;                /* '<Root>/DistanceToTDP' */
  real_T TimeToHorTarget;              /* '<Root>/TimeToHorTarget' */
  real_T TimeToTD;                     /* '<Root>/TimeToTD' */
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
extern void Easy_reg_initialize(void);
extern void Easy_reg_step(void);

/* Real-time Model object */
extern RT_MODEL *const rtM;

/*-
 * These blocks were eliminated from the model due to optimizations:
 *
 * Block '<S2>/Scope' : Unused code path elimination
 * Block '<S3>/Constant2' : Unused code path elimination
 * Block '<S3>/Display' : Unused code path elimination
 * Block '<S3>/Display1' : Unused code path elimination
 * Block '<S14>/Display' : Unused code path elimination
 * Block '<S14>/Display1' : Unused code path elimination
 * Block '<S18>/Gain' : Unused code path elimination
 * Block '<S19>/Gain' : Unused code path elimination
 * Block '<S14>/Scope1' : Unused code path elimination
 * Block '<S14>/Scope8' : Unused code path elimination
 * Block '<S14>/Scope9' : Unused code path elimination
 * Block '<S3>/Manual Switch1' : Unused code path elimination
 * Block '<S3>/Scope1' : Unused code path elimination
 * Block '<S3>/Scope2' : Unused code path elimination
 * Block '<S3>/Scope3' : Unused code path elimination
 * Block '<S1>/Display' : Unused code path elimination
 * Block '<S4>/Gain' : Unused code path elimination
 * Block '<S1>/Scope1' : Unused code path elimination
 * Block '<S1>/Scope2' : Unused code path elimination
 * Block '<S1>/Scope4' : Unused code path elimination
 * Block '<S27>/Gain' : Unused code path elimination
 * Block '<S28>/Gain' : Unused code path elimination
 * Block '<S29>/Gain' : Unused code path elimination
 * Block '<S31>/Scope' : Unused code path elimination
 * Block '<S5>/Scope' : Unused code path elimination
 * Block '<S5>/Scope2' : Unused code path elimination
 * Block '<S5>/Scope5' : Unused code path elimination
 * Block '<S5>/Scope6' : Unused code path elimination
 * Block '<S5>/Scope7' : Unused code path elimination
 * Block '<S5>/Scope8' : Unused code path elimination
 * Block '<S3>/CourseSwitch' : Eliminated due to constant selection input
 * Block '<S14>/Manual Switch2' : Eliminated due to constant selection input
 * Block '<S3>/Manual Switch' : Eliminated due to constant selection input
 * Block '<S1>/TD_SysSwitch' : Eliminated due to constant selection input
 * Block '<S31>/Manual Switch' : Eliminated due to constant selection input
 * Block '<S1>/Constant' : Unused code path elimination
 * Block '<S3>/Constant1' : Unused code path elimination
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
 * hilite_system('BUP_FOR_GUI/Easy_reg')    - opens subsystem BUP_FOR_GUI/Easy_reg
 * hilite_system('BUP_FOR_GUI/Easy_reg/Kp') - opens and selects block Kp
 *
 * Here is the system hierarchy for this model
 *
 * '<Root>' : 'BUP_FOR_GUI'
 * '<S1>'   : 'BUP_FOR_GUI/Easy_reg'
 * '<S2>'   : 'BUP_FOR_GUI/Easy_reg/BimControllaNDriftCalc'
 * '<S3>'   : 'BUP_FOR_GUI/Easy_reg/DataAnalyzer'
 * '<S4>'   : 'BUP_FOR_GUI/Easy_reg/Radians to Degrees'
 * '<S5>'   : 'BUP_FOR_GUI/Easy_reg/Targeting'
 * '<S6>'   : 'BUP_FOR_GUI/Easy_reg/TaskController'
 * '<S7>'   : 'BUP_FOR_GUI/Easy_reg/BimControllaNDriftCalc/BimTriggers'
 * '<S8>'   : 'BUP_FOR_GUI/Easy_reg/BimControllaNDriftCalc/DriftEstimator'
 * '<S9>'   : 'BUP_FOR_GUI/Easy_reg/BimControllaNDriftCalc/PointMoventPlot'
 * '<S10>'  : 'BUP_FOR_GUI/Easy_reg/BimControllaNDriftCalc/PreemptionTDP'
 * '<S11>'  : 'BUP_FOR_GUI/Easy_reg/BimControllaNDriftCalc/BimTriggers/BimTrigger'
 * '<S12>'  : 'BUP_FOR_GUI/Easy_reg/BimControllaNDriftCalc/PointMoventPlot/MATLAB Function'
 * '<S13>'  : 'BUP_FOR_GUI/Easy_reg/BimControllaNDriftCalc/PreemptionTDP/MATLAB Function'
 * '<S14>'  : 'BUP_FOR_GUI/Easy_reg/DataAnalyzer/InternalTrackingCourse'
 * '<S15>'  : 'BUP_FOR_GUI/Easy_reg/DataAnalyzer/InternalTrackingSpeed'
 * '<S16>'  : 'BUP_FOR_GUI/Easy_reg/DataAnalyzer/InternalTrackingCourse/Azimuth'
 * '<S17>'  : 'BUP_FOR_GUI/Easy_reg/DataAnalyzer/InternalTrackingCourse/Bearing'
 * '<S18>'  : 'BUP_FOR_GUI/Easy_reg/DataAnalyzer/InternalTrackingCourse/Radians to Degrees1'
 * '<S19>'  : 'BUP_FOR_GUI/Easy_reg/DataAnalyzer/InternalTrackingCourse/Radians to Degrees2'
 * '<S20>'  : 'BUP_FOR_GUI/Easy_reg/DataAnalyzer/InternalTrackingCourse/Azimuth/Azimut'
 * '<S21>'  : 'BUP_FOR_GUI/Easy_reg/DataAnalyzer/InternalTrackingCourse/Bearing/Heading_true'
 * '<S22>'  : 'BUP_FOR_GUI/Easy_reg/DataAnalyzer/InternalTrackingSpeed/GPSVelocity'
 * '<S23>'  : 'BUP_FOR_GUI/Easy_reg/DataAnalyzer/InternalTrackingSpeed/ProjectionSpeed'
 * '<S24>'  : 'BUP_FOR_GUI/Easy_reg/DataAnalyzer/InternalTrackingSpeed/GPSVelocity/Velocity'
 * '<S25>'  : 'BUP_FOR_GUI/Easy_reg/DataAnalyzer/InternalTrackingSpeed/ProjectionSpeed/Speed'
 * '<S26>'  : 'BUP_FOR_GUI/Easy_reg/Targeting/ControlDemode'
 * '<S27>'  : 'BUP_FOR_GUI/Easy_reg/Targeting/Radians to Degrees1'
 * '<S28>'  : 'BUP_FOR_GUI/Easy_reg/Targeting/Radians to Degrees2'
 * '<S29>'  : 'BUP_FOR_GUI/Easy_reg/Targeting/Radians to Degrees3'
 * '<S30>'  : 'BUP_FOR_GUI/Easy_reg/Targeting/Radians to Degrees4'
 * '<S31>'  : 'BUP_FOR_GUI/Easy_reg/Targeting/Roughening'
 * '<S32>'  : 'BUP_FOR_GUI/Easy_reg/Targeting/ControlDemode/ControlDemode'
 * '<S33>'  : 'BUP_FOR_GUI/Easy_reg/Targeting/Roughening/DeadZone'
 * '<S34>'  : 'BUP_FOR_GUI/Easy_reg/Targeting/Roughening/DeadZone/DeadZone'
 */

/*-
 * Requirements for '<Root>': Easy_reg
 */
#endif                                 /* RTW_HEADER_Easy_reg_h_ */

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
