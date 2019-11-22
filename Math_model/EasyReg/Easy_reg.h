/*
 * File: Easy_reg.h
 *
 * Code generated for Simulink model 'Easy_reg'.
 *
 * Model version                  : 1.418
 * Simulink Coder version         : 8.14 (R2018a) 06-Feb-2018
 * C/C++ source code generated on : Thu Nov 21 16:08:32 2019
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

/* Block signals and states (default storage) for system '<S13>/Bearing' */
typedef struct {
  real_T PreviousBearing_DSTATE;       /* '<S16>/PreviousBearing' */
} DW_Bearing;

/* Block signals and states (default storage) for system '<Root>' */
typedef struct {
  DW_Bearing Bearing_e;                /* '<S14>/Bearing' */
  DW_Bearing Bearing_o;                /* '<S13>/Bearing' */
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
  real_T LastPos_3_DSTATE;             /* '<S14>/LastPos' */
  real_T LastLineState;                /* '<S2>/BimTrigger' */
  uint32_T time;                       /* '<S1>/TaskController' */
  uint32_T timeout;                    /* '<S1>/TaskController' */
  uint8_T TargetEnable;                /* '<S1>/TaskController' */
  uint8_T is_active_c1_Easy_reg;       /* '<S1>/TaskController' */
  uint8_T is_active_Timer;             /* '<S1>/TaskController' */
  uint8_T is_Control;                  /* '<S1>/TaskController' */
  uint8_T is_active_Control;           /* '<S1>/TaskController' */
  uint8_T is_active_TargetCourse;      /* '<S1>/TaskController' */
  uint8_T is_Flight;                   /* '<S1>/TaskController' */
  uint8_T was_Flight;                  /* '<S1>/TaskController' */
  uint8_T is_Box;                      /* '<S1>/TaskController' */
  uint8_T was_Box;                     /* '<S1>/TaskController' */
  uint8_T is_Touchdown;                /* '<S1>/TaskController' */
  uint8_T is_active_Touchdown;         /* '<S1>/TaskController' */
  uint8_T is_active_EventGenerator;    /* '<S1>/TaskController' */
  uint8_T is_BimSupply;                /* '<S1>/TaskController' */
  uint8_T is_active_BimSupply;         /* '<S1>/TaskController' */
  uint8_T BoxEntry;                    /* '<S1>/TaskController' */
  uint8_T is_active_c9_BupSimulinkLibrari;/* '<S2>/BimTrigger' */
  boolean_T Delay2_DSTATE;             /* '<S1>/Delay2' */
} DW;

/* External inputs (root inport signals with default storage) */
typedef struct {
  real_T TDP_lat;                      /* '<Root>/TDP_lat' */
  real_T TDP_lon;                      /* '<Root>/TDP_lon' */
  real_T TDP_alt;                      /* '<Root>/TDP_alt' */
  real_T Pos_lat;                      /* '<Root>/Pos_lat' */
  real_T Pos_lon;                      /* '<Root>/Pos_lon' */
  real_T Pos_alt;                      /* '<Root>/Pos_alt' */
  int16_T Relief;                      /* '<Root>/Relief' */
  uint8_T ReliefAvailable;             /* '<Root>/ReliefAvailable' */
  int16_T ReliefOnTDP;                 /* '<Root>/ReliefOnTDP' */
  real_T ActualCourse;                 /* '<Root>/ActualCourse' */
} ExtU;

/* External outputs (root outports fed by signals with default storage) */
typedef struct {
  real_T BIM_CMD;                      /* '<Root>/BIM_CMD' */
  uint8_T TD_CMD;                      /* '<Root>/TD_CMD' */
  real_T DistanceToTDP;                /* '<Root>/DistanceToTDP' */
  real_T TimeToHorTarget;              /* '<Root>/TimeToHorTarget' */
  real_T TimeToTD;                     /* '<Root>/TimeToTD' */
} ExtY;

/* Real-time Model Data Structure */
struct tag_RTM {
  const char_T * volatile errorStatus;
};

/* Block signals and states (default storage) */
extern DW Easy_reg_DW;

/* External inputs (root inport signals with default storage) */
extern ExtU Easy_reg_U;

/* External outputs (root outports fed by signals with default storage) */
extern ExtY Easy_reg_Y;

/* Model entry point functions */
extern void Easy_reg_initialize(void);
extern void Easy_reg_step(void);

/* Real-time Model object */
extern RT_MODEL *const Easy_reg_M;

/*-
 * These blocks were eliminated from the model due to optimizations:
 *
 * Block '<S4>/Display' : Unused code path elimination
 * Block '<S4>/Display1' : Unused code path elimination
 * Block '<S14>/Display' : Unused code path elimination
 * Block '<S14>/Display1' : Unused code path elimination
 * Block '<S20>/Gain' : Unused code path elimination
 * Block '<S21>/Gain' : Unused code path elimination
 * Block '<S14>/Scope1' : Unused code path elimination
 * Block '<S14>/Scope8' : Unused code path elimination
 * Block '<S14>/Scope9' : Unused code path elimination
 * Block '<S4>/Scope' : Unused code path elimination
 * Block '<S4>/Scope1' : Unused code path elimination
 * Block '<S4>/Scope2' : Unused code path elimination
 * Block '<S4>/Scope3' : Unused code path elimination
 * Block '<S1>/Delay3' : Unused code path elimination
 * Block '<S1>/Scope1' : Unused code path elimination
 * Block '<S1>/Scope4' : Unused code path elimination
 * Block '<S35>/Gain' : Unused code path elimination
 * Block '<S36>/Gain' : Unused code path elimination
 * Block '<S37>/Gain' : Unused code path elimination
 * Block '<S39>/Scope' : Unused code path elimination
 * Block '<S9>/Scope' : Unused code path elimination
 * Block '<S9>/Scope2' : Unused code path elimination
 * Block '<S9>/Scope5' : Unused code path elimination
 * Block '<S9>/Scope6' : Unused code path elimination
 * Block '<S9>/Scope7' : Unused code path elimination
 * Block '<S9>/Scope8' : Unused code path elimination
 * Block '<S4>/CourseSwitch' : Eliminated due to constant selection input
 * Block '<S14>/Manual Switch2' : Eliminated due to constant selection input
 * Block '<S4>/Manual Switch' : Eliminated due to constant selection input
 * Block '<S4>/Manual Switch1' : Eliminated due to constant selection input
 * Block '<S1>/TD_SysSwitch' : Eliminated due to constant selection input
 * Block '<S39>/Manual Switch' : Eliminated due to constant selection input
 * Block '<S1>/Constant' : Unused code path elimination
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
 * '<S2>'   : 'BUP_FOR_GUI/Easy_reg/BimTriggers'
 * '<S3>'   : 'BUP_FOR_GUI/Easy_reg/BimTriggers13'
 * '<S4>'   : 'BUP_FOR_GUI/Easy_reg/DataAnalyzer'
 * '<S5>'   : 'BUP_FOR_GUI/Easy_reg/DriftEstimator'
 * '<S6>'   : 'BUP_FOR_GUI/Easy_reg/InternalTrackingSpeed'
 * '<S7>'   : 'BUP_FOR_GUI/Easy_reg/PointMoventPlot'
 * '<S8>'   : 'BUP_FOR_GUI/Easy_reg/PreemptionTDP'
 * '<S9>'   : 'BUP_FOR_GUI/Easy_reg/Targeting'
 * '<S10>'  : 'BUP_FOR_GUI/Easy_reg/TaskController'
 * '<S11>'  : 'BUP_FOR_GUI/Easy_reg/BimTriggers/BimTrigger'
 * '<S12>'  : 'BUP_FOR_GUI/Easy_reg/BimTriggers13/BimTrigger'
 * '<S13>'  : 'BUP_FOR_GUI/Easy_reg/DataAnalyzer/Course2TDP'
 * '<S14>'  : 'BUP_FOR_GUI/Easy_reg/DataAnalyzer/InternalTrackingCourse'
 * '<S15>'  : 'BUP_FOR_GUI/Easy_reg/DataAnalyzer/InternalTrackingSpeed'
 * '<S16>'  : 'BUP_FOR_GUI/Easy_reg/DataAnalyzer/Course2TDP/Bearing'
 * '<S17>'  : 'BUP_FOR_GUI/Easy_reg/DataAnalyzer/Course2TDP/Bearing/Heading_true'
 * '<S18>'  : 'BUP_FOR_GUI/Easy_reg/DataAnalyzer/InternalTrackingCourse/Azimuth'
 * '<S19>'  : 'BUP_FOR_GUI/Easy_reg/DataAnalyzer/InternalTrackingCourse/Bearing'
 * '<S20>'  : 'BUP_FOR_GUI/Easy_reg/DataAnalyzer/InternalTrackingCourse/Radians to Degrees1'
 * '<S21>'  : 'BUP_FOR_GUI/Easy_reg/DataAnalyzer/InternalTrackingCourse/Radians to Degrees2'
 * '<S22>'  : 'BUP_FOR_GUI/Easy_reg/DataAnalyzer/InternalTrackingCourse/Azimuth/Azimut'
 * '<S23>'  : 'BUP_FOR_GUI/Easy_reg/DataAnalyzer/InternalTrackingCourse/Bearing/Heading_true'
 * '<S24>'  : 'BUP_FOR_GUI/Easy_reg/DataAnalyzer/InternalTrackingSpeed/GPSVelocity'
 * '<S25>'  : 'BUP_FOR_GUI/Easy_reg/DataAnalyzer/InternalTrackingSpeed/ProjectionSpeed'
 * '<S26>'  : 'BUP_FOR_GUI/Easy_reg/DataAnalyzer/InternalTrackingSpeed/GPSVelocity/Velocity'
 * '<S27>'  : 'BUP_FOR_GUI/Easy_reg/DataAnalyzer/InternalTrackingSpeed/ProjectionSpeed/Speed'
 * '<S28>'  : 'BUP_FOR_GUI/Easy_reg/InternalTrackingSpeed/GPSVelocity'
 * '<S29>'  : 'BUP_FOR_GUI/Easy_reg/InternalTrackingSpeed/ProjectionSpeed'
 * '<S30>'  : 'BUP_FOR_GUI/Easy_reg/InternalTrackingSpeed/GPSVelocity/Velocity'
 * '<S31>'  : 'BUP_FOR_GUI/Easy_reg/InternalTrackingSpeed/ProjectionSpeed/Speed'
 * '<S32>'  : 'BUP_FOR_GUI/Easy_reg/PointMoventPlot/MATLAB Function'
 * '<S33>'  : 'BUP_FOR_GUI/Easy_reg/PreemptionTDP/MATLAB Function'
 * '<S34>'  : 'BUP_FOR_GUI/Easy_reg/Targeting/ControlDemode1'
 * '<S35>'  : 'BUP_FOR_GUI/Easy_reg/Targeting/Radians to Degrees1'
 * '<S36>'  : 'BUP_FOR_GUI/Easy_reg/Targeting/Radians to Degrees2'
 * '<S37>'  : 'BUP_FOR_GUI/Easy_reg/Targeting/Radians to Degrees3'
 * '<S38>'  : 'BUP_FOR_GUI/Easy_reg/Targeting/Radians to Degrees4'
 * '<S39>'  : 'BUP_FOR_GUI/Easy_reg/Targeting/Roughening'
 * '<S40>'  : 'BUP_FOR_GUI/Easy_reg/Targeting/ControlDemode1/ControlDemode'
 * '<S41>'  : 'BUP_FOR_GUI/Easy_reg/Targeting/Roughening/DeadZone'
 * '<S42>'  : 'BUP_FOR_GUI/Easy_reg/Targeting/Roughening/DeadZone/DeadZone'
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
