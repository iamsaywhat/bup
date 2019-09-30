/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 *
 * File: Easy_reg.h
 *
 * Code generated for Simulink model 'Easy_reg'.
 *
 * Model version                  : 1.116
 * Simulink Coder version         : 8.14 (R2018a) 06-Feb-2018
 * C/C++ source code generated on : Mon Sep 30 15:38:34 2019
 *
 * Target selection: ert.tlc
 * Embedded hardware selection: ARM Compatible->ARM Cortex
 * Code generation objectives: Unspecified
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

/* Block signals and states (default storage) for system '<S4>/Heading_true' */
typedef struct {
  real_T y;                            /* '<S4>/Heading_true' */
} DW_Heading_true;

/* Block signals and states (default storage) for system '<S2>/Simulink.Targeting' */
typedef struct {
  DW_Heading_true sf_Heading_true1;    /* '<S4>/Heading_true1' */
  DW_Heading_true sf_Heading_true;     /* '<S4>/Heading_true' */
  real_T Memory_PreviousInput[3];      /* '<S4>/Memory' */
  real_T ManualSwitch;                 /* '<S14>/Manual Switch' */
} DW_SimulinkTargeting;

/* Block signals and states (default storage) for system '<Root>' */
typedef struct {
  DW_SimulinkTargeting SimulinkTargeting_e;/* '<S2>/Simulink.Targeting' */
  real_T BoxLDPoint[3];                /* '<S1>/Chart' */
  real_T AngleManPoint[3];             /* '<S1>/Chart' */
  real_T BoxLUPoint[3];                /* '<S1>/Chart' */
  real_T BoxRUPoint[3];                /* '<S1>/Chart' */
  real_T BoxRDPoint[3];                /* '<S1>/Chart' */
  real_T TargetPoint[3];               /* '<S1>/Chart' */
  uint8_T TD_CMD;                      /* '<S1>/Chart' */
  uint8_T is_active_c1_Easy_reg;       /* '<S1>/Chart' */
  uint8_T is_Task_Control;             /* '<S1>/Chart' */
  uint8_T is_Task_Flight;              /* '<S1>/Chart' */
  uint8_T is_Task_TargetReplace;       /* '<S1>/Chart' */
  uint8_T is_Task_Touchdown;           /* '<S1>/Chart' */
  uint8_T BoxEntry;                    /* '<S1>/Chart' */
} DW;

/* External inputs (root inport signals with default storage) */
typedef struct {
  real_T TDP_lat;                      /* '<Root>/TDP_lat' */
  real_T TDP_lon;                      /* '<Root>/TDP_lon' */
  real_T TDP_alt;                      /* '<Root>/TDP_alt' */
  real_T Pos_lat;                      /* '<Root>/Pos_lat' */
  real_T Pos_lon;                      /* '<Root>/Pos_lon' */
  real_T Pos_alt;                      /* '<Root>/Pos_alt' */
  real_T ActualCourse;                 /* '<Root>/ActualCourse' */
  int16_T Relief;                      /* '<Root>/Relief' */
  uint8_T ReliefAvailable;             /* '<Root>/ReliefAvailable' */
} ExtU;

/* External outputs (root outports fed by signals with default storage) */
typedef struct {
  real_T BIM_CMD;                      /* '<Root>/BIM_CMD' */
  real_T TD_CMD;                       /* '<Root>/TD_CMD' */
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
 * Block '<S9>/Gain' : Unused code path elimination
 * Block '<S10>/Gain' : Unused code path elimination
 * Block '<S11>/Gain' : Unused code path elimination
 * Block '<S12>/Gain' : Unused code path elimination
 * Block '<S14>/Scope' : Unused code path elimination
 * Block '<S4>/Scope10' : Unused code path elimination
 * Block '<S4>/Scope13' : Unused code path elimination
 * Block '<S4>/Scope2' : Unused code path elimination
 * Block '<S4>/Scope3' : Unused code path elimination
 * Block '<S4>/Scope4' : Unused code path elimination
 * Block '<S4>/Scope5' : Unused code path elimination
 * Block '<S1>/Display' : Unused code path elimination
 * Block '<S1>/Display1' : Unused code path elimination
 * Block '<S1>/Display2' : Unused code path elimination
 * Block '<S1>/Scope' : Unused code path elimination
 * Block '<S4>/Base_Gain' : Eliminated nontunable gain of 1
 * Block '<S4>/CourseSwitch' : Eliminated due to constant selection input
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
 * '<S2>'   : 'BUP_FOR_GUI/Easy_reg/Chart'
 * '<S3>'   : 'BUP_FOR_GUI/Easy_reg/Velocity'
 * '<S4>'   : 'BUP_FOR_GUI/Easy_reg/Chart/Simulink.Targeting'
 * '<S5>'   : 'BUP_FOR_GUI/Easy_reg/Chart/Simulink.Targeting/Azimut'
 * '<S6>'   : 'BUP_FOR_GUI/Easy_reg/Chart/Simulink.Targeting/ControlDemode'
 * '<S7>'   : 'BUP_FOR_GUI/Easy_reg/Chart/Simulink.Targeting/Heading_true'
 * '<S8>'   : 'BUP_FOR_GUI/Easy_reg/Chart/Simulink.Targeting/Heading_true1'
 * '<S9>'   : 'BUP_FOR_GUI/Easy_reg/Chart/Simulink.Targeting/Radians to Degrees'
 * '<S10>'  : 'BUP_FOR_GUI/Easy_reg/Chart/Simulink.Targeting/Radians to Degrees1'
 * '<S11>'  : 'BUP_FOR_GUI/Easy_reg/Chart/Simulink.Targeting/Radians to Degrees2'
 * '<S12>'  : 'BUP_FOR_GUI/Easy_reg/Chart/Simulink.Targeting/Radians to Degrees3'
 * '<S13>'  : 'BUP_FOR_GUI/Easy_reg/Chart/Simulink.Targeting/Radians to Degrees4'
 * '<S14>'  : 'BUP_FOR_GUI/Easy_reg/Chart/Simulink.Targeting/Roughening'
 * '<S15>'  : 'BUP_FOR_GUI/Easy_reg/Chart/Simulink.Targeting/Roughening/dead-zone'
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
