/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 *
 * File: Easy_reg.h
 *
 * Code generated for Simulink model 'Easy_reg'.
 *
 * Model version                  : 1.23
 * Simulink Coder version         : 8.14 (R2018a) 06-Feb-2018
 * C/C++ source code generated on : Fri Aug  9 18:34:51 2019
 *
 * Target selection: ert.tlc
 * Embedded hardware selection: ARM Compatible->ARM Cortex
 * Code generation objectives: Unspecified
 * Validation result: Not run
 */

#ifndef RTW_HEADER_Easy_reg_h_
#define RTW_HEADER_Easy_reg_h_
#include <math.h>
#include <string.h>
#include <stddef.h>
#ifndef Easy_reg_COMMON_INCLUDES_
# define Easy_reg_COMMON_INCLUDES_
#include "rtwtypes.h"
#endif                                 /* Easy_reg_COMMON_INCLUDES_ */

#include "Easy_reg_types.h"

/* Macros for accessing real-time model data structure */
#ifndef rtmGetErrorStatus
# define rtmGetErrorStatus(rtm)        ((rtm)->errorStatus)
#endif

#ifndef rtmSetErrorStatus
# define rtmSetErrorStatus(rtm, val)   ((rtm)->errorStatus = (val))
#endif

/* Block states (default storage) for system '<Root>' */
typedef struct {
  real_T Memory_1_PreviousInput;       /* '<S1>/Memory' */
  real_T Memory_2_PreviousInput;       /* '<S1>/Memory' */
} DW_Easy_reg_T;

/* Invariant block signals (default storage) */
typedef struct {
  const real_T Gain5;                  /* '<S1>/Gain5' */
  const real_T Product;                /* '<S1>/Product' */
  const boolean_T LogicalOperator;     /* '<S1>/Logical Operator' */
} ConstB_Easy_reg_T;

/* External inputs (root inport signals with default storage) */
typedef struct {
  real_T TDP_lon;                      /* '<Root>/TDP_lon' */
  real_T TDP_lat;                      /* '<Root>/TDP_lat' */
  real_T TDP_alt;                      /* '<Root>/TDP_alt' */
  real_T Pos_lon;                      /* '<Root>/Pos_lon' */
  real_T Pos_lat;                      /* '<Root>/Pos_lat' */
  real_T Pos_alt;                      /* '<Root>/Pos_alt' */
  real_T ActualCourse;                 /* '<Root>/ActualCourse' */
} ExtU_Easy_reg_T;

/* External outputs (root outports fed by signals with default storage) */
typedef struct {
  real_T BIM_CMD;                      /* '<Root>/BIM_CMD' */
  real_T TD_CMD;                       /* '<Root>/TD_CMD' */
} ExtY_Easy_reg_T;

/* Real-time Model Data Structure */
struct tag_RTM_Easy_reg_T {
  const char_T * volatile errorStatus;
};

/* Block states (default storage) */
extern DW_Easy_reg_T Easy_reg_DW;

/* External inputs (root inport signals with default storage) */
extern ExtU_Easy_reg_T Easy_reg_U;

/* External outputs (root outports fed by signals with default storage) */
extern ExtY_Easy_reg_T Easy_reg_Y;
extern const ConstB_Easy_reg_T Easy_reg_ConstB;/* constant block i/o */

/* Model entry point functions */
extern void Easy_reg_initialize(void);
extern void Easy_reg_step(void);
extern void Easy_reg_terminate(void);

/* Real-time Model object */
extern RT_MODEL_Easy_reg_T *const Easy_reg_M;

/*-
 * These blocks were eliminated from the model due to optimizations:
 *
 * Block '<S1>/Cast' : Unused code path elimination
 * Block '<S1>/Constant' : Unused code path elimination
 * Block '<S1>/Fin_Man_Alt' : Unused code path elimination
 * Block '<S1>/Gain2' : Unused code path elimination
 * Block '<S1>/Gain3' : Unused code path elimination
 * Block '<S1>/Relational Operator' : Unused code path elimination
 * Block '<S1>/Relational Operator1' : Unused code path elimination
 * Block '<S1>/Relational Operator2' : Unused code path elimination
 * Block '<S1>/Scope1' : Unused code path elimination
 * Block '<S1>/Scope10' : Unused code path elimination
 * Block '<S1>/Scope11' : Unused code path elimination
 * Block '<S1>/Scope12' : Unused code path elimination
 * Block '<S1>/Scope14' : Unused code path elimination
 * Block '<S1>/Scope2' : Unused code path elimination
 * Block '<S1>/Scope3' : Unused code path elimination
 * Block '<S1>/Scope4' : Unused code path elimination
 * Block '<S1>/Scope5' : Unused code path elimination
 * Block '<S1>/Scope6' : Unused code path elimination
 * Block '<S1>/Scope7' : Unused code path elimination
 * Block '<S1>/Scope8' : Unused code path elimination
 * Block '<S1>/Wait_RAD' : Unused code path elimination
 * Block '<S1>/Gain' : Eliminated nontunable gain of 1
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
 * '<S2>'   : 'BUP_FOR_GUI/Easy_reg/Distance_calc_KM'
 * '<S3>'   : 'BUP_FOR_GUI/Easy_reg/Distance_calc_deg'
 * '<S4>'   : 'BUP_FOR_GUI/Easy_reg/Heading_true'
 * '<S5>'   : 'BUP_FOR_GUI/Easy_reg/Heading_true1'
 * '<S6>'   : 'BUP_FOR_GUI/Easy_reg/MATLAB Function'
 */
#endif                                 /* RTW_HEADER_Easy_reg_h_ */

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
