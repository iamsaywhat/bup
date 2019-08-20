/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 *
 * File: Easy_reg.h
 *
 * Code generated for Simulink model 'Easy_reg'.
 *
 * Model version                  : 1.30
 * Simulink Coder version         : 8.14 (R2018a) 06-Feb-2018
 * C/C++ source code generated on : Fri Aug 16 20:03:27 2019
 *
 * Target selection: ert.tlc
 * Embedded hardware selection: ARM Compatible->ARM Cortex
 * Code generation objectives:
 *    1. RAM efficiency
 *    2. Execution efficiency
 * Validation result: Not run
 */

#ifndef RTW_HEADER_Easy_reg_h_
#define RTW_HEADER_Easy_reg_h_
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

/* Block signals and states (default storage) for system '<S1>/Heading_true' */
typedef struct {
  real_T y;                            /* '<S1>/Heading_true' */
} DW_Heading_true;

/* Block signals and states (default storage) for system '<Root>' */
typedef struct {
  DW_Heading_true sf_Heading_true1;    /* '<S1>/Heading_true1' */
  DW_Heading_true sf_Heading_true;     /* '<S1>/Heading_true' */
  real_T Memory_1_PreviousInput;       /* '<S1>/Memory' */
  real_T Memory_2_PreviousInput;       /* '<S1>/Memory' */
} DW;

/* External inputs (root inport signals with default storage) */
typedef struct {
  real_T TDP_lon;                      /* '<Root>/TDP_lon' */
  real_T TDP_lat;                      /* '<Root>/TDP_lat' */
  real_T TDP_alt;                      /* '<Root>/TDP_alt' */
  real_T Pos_lon;                      /* '<Root>/Pos_lon' */
  real_T Pos_lat;                      /* '<Root>/Pos_lat' */
  real_T Pos_alt;                      /* '<Root>/Pos_alt' */
  real_T ActualCourse;                 /* '<Root>/ActualCourse' */
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
 * Block '<S1>/Gain2' : Unused code path elimination
 * Block '<S1>/Gain3' : Unused code path elimination
 * Block '<S1>/Scope1' : Unused code path elimination
 * Block '<S1>/Scope10' : Unused code path elimination
 * Block '<S1>/Scope2' : Unused code path elimination
 * Block '<S1>/Scope3' : Unused code path elimination
 * Block '<S1>/Scope4' : Unused code path elimination
 * Block '<S1>/Scope5' : Unused code path elimination
 * Block '<S1>/Scope6' : Unused code path elimination
 * Block '<S1>/Scope7' : Unused code path elimination
 * Block '<S1>/Scope8' : Unused code path elimination
 * Block '<S1>/Scope9' : Unused code path elimination
 * Block '<S7>/Constant' : Unused code path elimination
 * Block '<S1>/Base_Gain' : Eliminated nontunable gain of 1
 * Block '<S1>/CourseSwitch' : Eliminated due to constant selection input
 * Block '<S1>/TD_RegSwitch' : Eliminated due to constant selection input
 * Block '<S1>/TD_RegSwitch1' : Eliminated due to constant selection input
 * Block '<S1>/Constant2' : Unused code path elimination
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
 * '<S2>'   : 'BUP_FOR_GUI/Easy_reg/ControlDemode'
 * '<S3>'   : 'BUP_FOR_GUI/Easy_reg/Distance_calc_KM'
 * '<S4>'   : 'BUP_FOR_GUI/Easy_reg/Heading_true'
 * '<S5>'   : 'BUP_FOR_GUI/Easy_reg/Heading_true1'
 * '<S6>'   : 'BUP_FOR_GUI/Easy_reg/TD_Reg'
 * '<S7>'   : 'BUP_FOR_GUI/Easy_reg/TouchDownGen'
 * '<S8>'   : 'BUP_FOR_GUI/Easy_reg/TD_Reg/Distance_calc_deg'
 */
#endif                                 /* RTW_HEADER_Easy_reg_h_ */

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
