/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 *
 * File: Easy_reg.c
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

#include "Easy_reg.h"

/* Block signals and states (default storage) */
DW Easy_reg_DW;

/* External inputs (root inport signals with default storage) */
ExtU Easy_reg_U;

/* External outputs (root outports fed by signals with default storage) */
ExtY Easy_reg_Y;

/* Real-time model */
RT_MODEL Easy_reg_M_;
RT_MODEL *const Easy_reg_M = &Easy_reg_M_;
extern real_T rt_roundd_snf(real_T u);
static void Heading_true(real_T rtu_u1, real_T rtu_u1_b, real_T rtu_u0, real_T
  rtu_u0_m, DW_Heading_true *localDW);

/*
 * Output and update for atomic system:
 *    '<S1>/Heading_true'
 *    '<S1>/Heading_true1'
 */
static void Heading_true(real_T rtu_u1, real_T rtu_u1_b, real_T rtu_u0, real_T
  rtu_u0_m, DW_Heading_true *localDW)
{
  real_T un_idx_0;
  real_T un_idx_1;
  real_T y_tmp;

  /* SignalConversion: '<S4>/TmpSignal ConversionAt SFunction Inport1' */
  un_idx_0 = rtu_u1 - rtu_u0;
  un_idx_1 = rtu_u1_b - rtu_u0_m;
  y_tmp = sqrt(un_idx_1 * un_idx_1 + un_idx_0 * un_idx_0);
  localDW->y = un_idx_1 / y_tmp;
  localDW->y = acos(localDW->y);
  if (acos(un_idx_0 / y_tmp) > 1.5708) {
    localDW->y = 6.28319 - localDW->y;
  }
}

real_T rt_roundd_snf(real_T u)
{
  real_T y;
  if (fabs(u) < 4.503599627370496E+15) {
    if (u >= 0.5) {
      y = floor(u + 0.5);
    } else if (u > -0.5) {
      y = u * 0.0;
    } else {
      y = ceil(u - 0.5);
    }
  } else {
    y = u;
  }

  return y;
}

/* Model step function */
void Easy_reg_step(void)
{
  boolean_T rtb_LogicalOperator1;
  real_T dif_idx_1;
  real_T dif_idx_0;

  /* Outputs for Atomic SubSystem: '<Root>/Easy_reg' */
  /* MATLAB Function: '<S6>/Distance_calc_deg' incorporates:
   *  Inport: '<Root>/Pos_lat'
   *  Inport: '<Root>/Pos_lon'
   *  Inport: '<Root>/TDP_lat'
   *  Inport: '<Root>/TDP_lon'
   */
  dif_idx_0 = Easy_reg_U.TDP_lon - Easy_reg_U.Pos_lon;
  dif_idx_1 = Easy_reg_U.TDP_lat - Easy_reg_U.Pos_lat;

  /* Logic: '<S6>/Logical Operator1' incorporates:
   *  Constant: '<S6>/TD_RAD'
   *  Constant: '<S6>/TD_reg_disable_alt'
   *  Inport: '<Root>/Pos_alt'
   *  MATLAB Function: '<S6>/Distance_calc_deg'
   *  RelationalOperator: '<S6>/Relational Operator1'
   *  RelationalOperator: '<S6>/Relational Operator2'
   */
  rtb_LogicalOperator1 = ((sqrt(dif_idx_0 * dif_idx_0 + dif_idx_1 * dif_idx_1) <=
    0.007) && (Easy_reg_U.Pos_alt > 650.0));

  /* MATLAB Function: '<S1>/Heading_true' incorporates:
   *  Inport: '<Root>/Pos_lat'
   *  Inport: '<Root>/Pos_lon'
   *  Inport: '<Root>/TDP_lat'
   *  Inport: '<Root>/TDP_lon'
   */
  Heading_true(Easy_reg_U.TDP_lon, Easy_reg_U.TDP_lat, Easy_reg_U.Pos_lon,
               Easy_reg_U.Pos_lat, &Easy_reg_DW.sf_Heading_true);

  /* MATLAB Function: '<S1>/Heading_true1' incorporates:
   *  Inport: '<Root>/Pos_lat'
   *  Inport: '<Root>/Pos_lon'
   *  Memory: '<S1>/Memory'
   */
  Heading_true(Easy_reg_U.Pos_lon, Easy_reg_U.Pos_lat,
               Easy_reg_DW.Memory_1_PreviousInput,
               Easy_reg_DW.Memory_2_PreviousInput, &Easy_reg_DW.sf_Heading_true1);

  /* Sum: '<S1>/Sum1' incorporates:
   *  Product: '<S6>/Product'
   *  Sum: '<S1>/Sum'
   */
  dif_idx_0 = (Easy_reg_DW.sf_Heading_true.y - (real_T)rtb_LogicalOperator1 *
               1.5707963267948966) - Easy_reg_DW.sf_Heading_true1.y;

  /* MATLAB Function: '<S1>/ControlDemode' */
  if (dif_idx_0 > 3.14) {
    dif_idx_0 -= 6.2819;
  } else {
    if (dif_idx_0 < -3.14) {
      dif_idx_0 += 6.2819;
    }
  }

  /* End of MATLAB Function: '<S1>/ControlDemode' */

  /* Switch: '<S1>/Switch' */
  if (rtb_LogicalOperator1) {
    /* Gain: '<S1>/TD_Gain' incorporates:
     *  Gain: '<S1>/Gain1'
     */
    dif_idx_0 = 57.295779513082323 * dif_idx_0 * 3.0;

    /* Saturate: '<S1>/TD_Sat' */
    if (dif_idx_0 > 20.0) {
      dif_idx_0 = 20.0;
    } else {
      if (dif_idx_0 < -20.0) {
        dif_idx_0 = -20.0;
      }
    }

    /* End of Saturate: '<S1>/TD_Sat' */
  } else {
    /* Gain: '<S1>/Gain1' */
    dif_idx_0 *= 57.295779513082323;

    /* Saturate: '<S1>/Base_Sat' */
    if (dif_idx_0 > 50.0) {
      dif_idx_0 = 50.0;
    } else {
      if (dif_idx_0 < -50.0) {
        dif_idx_0 = -50.0;
      }
    }

    /* End of Saturate: '<S1>/Base_Sat' */
  }

  /* End of Switch: '<S1>/Switch' */

  /* Gain: '<S1>/Gain' */
  dif_idx_0 *= 0.1;

  /* Rounding: '<S1>/Rounding Function' */
  dif_idx_0 = rt_roundd_snf(dif_idx_0);

  /* Outport: '<Root>/TD_CMD' incorporates:
   *  Inport: '<Root>/Pos_alt'
   *  Inport: '<Root>/TDP_alt'
   *  ManualSwitch: '<S1>/TD_SysSwitch'
   *  RelationalOperator: '<S7>/Relational Operator'
   */
  Easy_reg_Y.TD_CMD = (Easy_reg_U.Pos_alt <= Easy_reg_U.TDP_alt);

  /* Update for Memory: '<S1>/Memory' incorporates:
   *  Inport: '<Root>/Pos_lat'
   *  Inport: '<Root>/Pos_lon'
   */
  Easy_reg_DW.Memory_1_PreviousInput = Easy_reg_U.Pos_lon;
  Easy_reg_DW.Memory_2_PreviousInput = Easy_reg_U.Pos_lat;

  /* Outport: '<Root>/BIM_CMD' incorporates:
   *  Gain: '<S1>/Gain4'
   */
  Easy_reg_Y.BIM_CMD = 10.0 * dif_idx_0;

  /* End of Outputs for SubSystem: '<Root>/Easy_reg' */
}

/* Model initialize function */
void Easy_reg_initialize(void)
{
  /* (no initialization code required) */
}

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
