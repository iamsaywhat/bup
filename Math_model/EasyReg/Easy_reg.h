/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 *
 * File: Easy_reg.h
 *
 * Code generated for Simulink model 'Easy_reg'.
 *
 * Model version                  : 1.265
 * Simulink Coder version         : 8.14 (R2018a) 06-Feb-2018
 * C/C++ source code generated on : Tue Nov  5 17:10:06 2019
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
typedef struct {
  real_T TmpSignalConversionAtSFunctio_g[3];/* '<S1>/TaskController' */
  real_T TargetPoint[3];               /* '<S1>/TaskController' */
  real_T Point1[3];                    /* '<S1>/TaskController' */
  real_T Point2[3];                    /* '<S1>/TaskController' */
  real_T LUPoint[3];                   /* '<S1>/TaskController' */
  real_T LDPoint[3];                   /* '<S1>/TaskController' */
  real_T RUPoint[3];                   /* '<S1>/TaskController' */
  real_T RDPoint[3];                   /* '<S1>/TaskController' */
  real_T SFunction_o8;                 /* '<S1>/TaskController' */
  real_T SFunction_o9;                 /* '<S1>/TaskController' */
  real_T SFunction_o10;                /* '<S1>/TaskController' */
  real_T SFunction_o11;                /* '<S1>/TaskController' */
  real_T LastPos_1_PreviousInput;      /* '<S8>/LastPos' */
  real_T LastPos_2_PreviousInput;      /* '<S8>/LastPos' */
  real_T Memory1_PreviousInput;        /* '<S8>/Memory1' */
  uint32_T time;                       /* '<S1>/TaskController' */
  uint32_T timeout;                    /* '<S1>/TaskController' */
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
  uint8_T is_active_EventGen;          /* '<S1>/TaskController' */
  uint8_T BoxEntry;                    /* '<S1>/TaskController' */
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
 * Block '<S3>/Display' : Unused code path elimination
 * Block '<S3>/Display1' : Unused code path elimination
 * Block '<S8>/Scope8' : Unused code path elimination
 * Block '<S8>/Scope9' : Unused code path elimination
 * Block '<S3>/Scope' : Unused code path elimination
 * Block '<S3>/Scope1' : Unused code path elimination
 * Block '<S3>/Scope2' : Unused code path elimination
 * Block '<S3>/Scope3' : Unused code path elimination
 * Block '<S16>/Gain' : Unused code path elimination
 * Block '<S17>/Gain' : Unused code path elimination
 * Block '<S18>/Gain' : Unused code path elimination
 * Block '<S20>/Scope' : Unused code path elimination
 * Block '<S4>/Scope1' : Unused code path elimination
 * Block '<S4>/Scope5' : Unused code path elimination
 * Block '<S4>/Scope6' : Unused code path elimination
 * Block '<S4>/Scope7' : Unused code path elimination
 * Block '<S4>/Scope8' : Unused code path elimination
 * Block '<S3>/CourseSwitch' : Eliminated due to constant selection input
 * Block '<S8>/Manual Switch2' : Eliminated due to constant selection input
 * Block '<S3>/Manual Switch' : Eliminated due to constant selection input
 * Block '<S3>/Manual Switch1' : Eliminated due to constant selection input
 * Block '<S1>/TD_SysSwitch' : Eliminated due to constant selection input
 * Block '<S4>/Base_Gain' : Eliminated nontunable gain of 1
 * Block '<S20>/Manual Switch' : Eliminated due to constant selection input
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
 * '<S2>'   : 'BUP_FOR_GUI/Easy_reg/CourseToTarget'
 * '<S3>'   : 'BUP_FOR_GUI/Easy_reg/DataAnalyzer'
 * '<S4>'   : 'BUP_FOR_GUI/Easy_reg/Targeting'
 * '<S5>'   : 'BUP_FOR_GUI/Easy_reg/TaskController'
 * '<S6>'   : 'BUP_FOR_GUI/Easy_reg/CourseToTarget/TargetCourse'
 * '<S7>'   : 'BUP_FOR_GUI/Easy_reg/DataAnalyzer/Course2TDP'
 * '<S8>'   : 'BUP_FOR_GUI/Easy_reg/DataAnalyzer/InternalTrackingCourse'
 * '<S9>'   : 'BUP_FOR_GUI/Easy_reg/DataAnalyzer/InternalTrackingSpeed'
 * '<S10>'  : 'BUP_FOR_GUI/Easy_reg/DataAnalyzer/Course2TDP/TDPCourse'
 * '<S11>'  : 'BUP_FOR_GUI/Easy_reg/DataAnalyzer/InternalTrackingCourse/Azimut'
 * '<S12>'  : 'BUP_FOR_GUI/Easy_reg/DataAnalyzer/InternalTrackingCourse/Heading_true'
 * '<S13>'  : 'BUP_FOR_GUI/Easy_reg/DataAnalyzer/InternalTrackingSpeed/Speed'
 * '<S14>'  : 'BUP_FOR_GUI/Easy_reg/DataAnalyzer/InternalTrackingSpeed/Velocity'
 * '<S15>'  : 'BUP_FOR_GUI/Easy_reg/Targeting/ControlDemode'
 * '<S16>'  : 'BUP_FOR_GUI/Easy_reg/Targeting/Radians to Degrees1'
 * '<S17>'  : 'BUP_FOR_GUI/Easy_reg/Targeting/Radians to Degrees2'
 * '<S18>'  : 'BUP_FOR_GUI/Easy_reg/Targeting/Radians to Degrees3'
 * '<S19>'  : 'BUP_FOR_GUI/Easy_reg/Targeting/Radians to Degrees4'
 * '<S20>'  : 'BUP_FOR_GUI/Easy_reg/Targeting/Roughening'
 * '<S21>'  : 'BUP_FOR_GUI/Easy_reg/Targeting/Roughening/dead-zone'
 * '<S22>'  : 'BUP_FOR_GUI/Easy_reg/TaskController/Simulink.Targeting'
 * '<S23>'  : 'BUP_FOR_GUI/Easy_reg/TaskController/Simulink.TrackingCourse'
 * '<S24>'  : 'BUP_FOR_GUI/Easy_reg/TaskController/Simulink.Targeting/ControlDemode'
 * '<S25>'  : 'BUP_FOR_GUI/Easy_reg/TaskController/Simulink.Targeting/Radians to Degrees2'
 * '<S26>'  : 'BUP_FOR_GUI/Easy_reg/TaskController/Simulink.Targeting/Radians to Degrees3'
 * '<S27>'  : 'BUP_FOR_GUI/Easy_reg/TaskController/Simulink.Targeting/Radians to Degrees4'
 * '<S28>'  : 'BUP_FOR_GUI/Easy_reg/TaskController/Simulink.Targeting/Roughening'
 * '<S29>'  : 'BUP_FOR_GUI/Easy_reg/TaskController/Simulink.Targeting/Roughening/dead-zone'
 * '<S30>'  : 'BUP_FOR_GUI/Easy_reg/TaskController/Simulink.TrackingCourse/Azimut'
 * '<S31>'  : 'BUP_FOR_GUI/Easy_reg/TaskController/Simulink.TrackingCourse/Heading_true'
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
