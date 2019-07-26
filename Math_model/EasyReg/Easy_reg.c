/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 *
 * File: Easy_reg.c
 *
 * Code generated for Simulink model 'Easy_reg'.
 *
 * Model version                  : 1.19
 * Simulink Coder version         : 8.14 (R2018a) 06-Feb-2018
 * C/C++ source code generated on : Fri Jul 26 14:43:45 2019
 *
 * Target selection: ert.tlc
 * Embedded hardware selection: ARM Compatible->ARM Cortex
 * Code generation objectives: Unspecified
 * Validation result: Not run
 */

#include "Easy_reg.h"
#include "Easy_reg_private.h"

/* Block states (default storage) */
DW_Easy_reg_T Easy_reg_DW;

/* External inputs (root inport signals with default storage) */
ExtU_Easy_reg_T Easy_reg_U;

/* External outputs (root outports fed by signals with default storage) */
ExtY_Easy_reg_T Easy_reg_Y;

/* Real-time model */
RT_MODEL_Easy_reg_T Easy_reg_M_;
RT_MODEL_Easy_reg_T *const Easy_reg_M = &Easy_reg_M_;

/*
 * Output and update for atomic system:
 *    '<S1>/Heading_true'
 *    '<S1>/Heading_true1'
 */
void Easy_reg_Heading_true(real_T rtu_u1, real_T rtu_u1_j, real_T rtu_u0, real_T
  rtu_u0_f, real_T *rty_y)
{
  real_T un_idx_0;
  real_T un_idx_1;
  real_T tmp;

  /* SignalConversion: '<S2>/TmpSignal ConversionAt SFunction Inport1' */
  un_idx_0 = rtu_u1 - rtu_u0;
  un_idx_1 = rtu_u1_j - rtu_u0_f;
  tmp = sqrt(un_idx_1 * un_idx_1 + un_idx_0 * un_idx_0);
  *rty_y = un_idx_1 / tmp;
  *rty_y = acos(*rty_y);
  if (acos(un_idx_0 / tmp) > 1.5708) {
    *rty_y = 6.28319 - *rty_y;
  }
}

/* Model step function */
void Easy_reg_step(void)
{
  real_T rtb_y_a;
  real_T rtb_y_p;

  /* Outputs for Atomic SubSystem: '<Root>/Easy_reg' */
  /* MATLAB Function: '<S1>/Heading_true' incorporates:
   *  Inport: '<Root>/Pos_lat'
   *  Inport: '<Root>/Pos_lon'
   *  Inport: '<Root>/TDP_lat'
   *  Inport: '<Root>/TDP_lon'
   */
  Easy_reg_Heading_true(Easy_reg_U.TDP_lon, Easy_reg_U.TDP_lat,
                        Easy_reg_U.Pos_lon, Easy_reg_U.Pos_lat, &rtb_y_a);

  /* MATLAB Function: '<S1>/Heading_true1' incorporates:
   *  Inport: '<Root>/Pos_lat'
   *  Inport: '<Root>/Pos_lon'
   *  Memory: '<S1>/Memory'
   */
  Easy_reg_Heading_true(Easy_reg_U.Pos_lon, Easy_reg_U.Pos_lat,
                        Easy_reg_DW.Memory_1_PreviousInput,
                        Easy_reg_DW.Memory_2_PreviousInput, &rtb_y_p);

  /* Sum: '<S1>/Sum1' */
  rtb_y_a -= rtb_y_p;

  /* MATLAB Function: '<S1>/MATLAB Function' */
  if (rtb_y_a > 3.14) {
    rtb_y_a -= 6.2819;
  } else {
    if (rtb_y_a < -3.14) {
      rtb_y_a += 6.2819;
    }
  }

  /* End of MATLAB Function: '<S1>/MATLAB Function' */

  /* Update for Memory: '<S1>/Memory' incorporates:
   *  Inport: '<Root>/Pos_lat'
   *  Inport: '<Root>/Pos_lon'
   */
  Easy_reg_DW.Memory_1_PreviousInput = Easy_reg_U.Pos_lon;
  Easy_reg_DW.Memory_2_PreviousInput = Easy_reg_U.Pos_lat;

  /* Gain: '<S1>/Gain1' */
  Easy_reg_Y.BIM_CMD = 57.295779513082323 * rtb_y_a;

  /* Saturate: '<S1>/Saturation' */
  if (Easy_reg_Y.BIM_CMD > 50.0) {
    /* Gain: '<S1>/Gain1' incorporates:
     *  Outport: '<Root>/BIM_CMD'
     */
    Easy_reg_Y.BIM_CMD = 50.0;
  } else {
    if (Easy_reg_Y.BIM_CMD < -50.0) {
      /* Gain: '<S1>/Gain1' incorporates:
       *  Outport: '<Root>/BIM_CMD'
       */
      Easy_reg_Y.BIM_CMD = -50.0;
    }
  }

  /* End of Saturate: '<S1>/Saturation' */

  /* Outport: '<Root>/TD_CMD' incorporates:
   *  Constant: '<S1>/Constant'
   *  DataTypeConversion: '<S1>/Cast'
   *  Inport: '<Root>/Pos_alt'
   *  RelationalOperator: '<S1>/Relational Operator'
   */
  Easy_reg_Y.TD_CMD = (uint8_T)(Easy_reg_U.Pos_alt <= 300.0);

  /* End of Outputs for SubSystem: '<Root>/Easy_reg' */
}

/* Model initialize function */
void Easy_reg_initialize(void)
{
  /* Registration code */

  /* initialize error status */
  rtmSetErrorStatus(Easy_reg_M, (NULL));

  /* states (dwork) */
  (void) memset((void *)&Easy_reg_DW, 0,
                sizeof(DW_Easy_reg_T));

  /* external inputs */
  (void)memset((void *)&Easy_reg_U, 0, sizeof(ExtU_Easy_reg_T));

  /* external outputs */
  (void) memset((void *)&Easy_reg_Y, 0,
                sizeof(ExtY_Easy_reg_T));
}

/* Model terminate function */
void Easy_reg_terminate(void)
{
  /* (no terminate code required) */
}

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
