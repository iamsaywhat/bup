/*
 * File: flightRegulatorCFB.c
 *
 * Code generated for Simulink model 'flightRegulatorCFB'.
 *
 * Model version                  : 1.174
 * Simulink Coder version         : 9.0 (R2018b) 24-May-2018
 * C/C++ source code generated on : Thu Jan 30 15:51:49 2020
 *
 * Target selection: ert.tlc
 * Embedded hardware selection: ARM Compatible->ARM Cortex
 * Code generation objectives:
 *    1. Execution efficiency
 *    2. RAM efficiency
 * Validation result: Not run
 */

#include "flightRegulatorCFB.h"

/* Named constants for Chart: '<S4>/paramСalculation' */
#define IN_FinishTest                  ((uint8_T)1U)
#define IN_Pause                       ((uint8_T)2U)
#define IN_runTest                     ((uint8_T)3U)
#define IN_startFlight                 ((uint8_T)4U)

/* Block signals and states (default storage) */
DW rtDW;

/* External inputs (root inport signals with default storage) */
ExtU rtU;

/* External outputs (root outports fed by signals with default storage) */
ExtY rtY;

/* Real-time model */
RT_MODEL rtM_;
RT_MODEL *const rtM = &rtM_;

/* Model step function */
void flightRegulatorCFB_step(void)
{
  real_T rtb_Add;
  int32_T tightenSling_tmp;
  boolean_T guard1 = false;
  boolean_T guard2 = false;

  /* Outputs for Atomic SubSystem: '<Root>/flightRegulatorCFB' */
  /* Switch: '<S1>/Switch' incorporates:
   *  Constant: '<S1>/Constant4'
   *  Inport: '<Root>/HighDigitalTerrain'
   *  Inport: '<Root>/isAvailableDigitalTerrain'
   */
  if (rtU.isAvailableDigitalTerrain > 0.0) {
    rtb_Add = rtU.HighDigitalTerrain;
  } else {
    rtb_Add = 0.0;
  }

  /* End of Switch: '<S1>/Switch' */

  /* Sum: '<S1>/Add' incorporates:
   *  Inport: '<Root>/XYZi'
   */
  rtb_Add = rtU.XYZi[2] - rtb_Add;

  /* Chart: '<S4>/paramСalculation' incorporates:
   *  Inport: '<Root>/arrayLeftRightManeuvers'
   *  Inport: '<Root>/arrayTimes'
   *  Inport: '<Root>/pauseTimeBetweenManeuvers'
   */
  if (rtDW.temporalCounter_i1 < MAX_uint32_T) {
    rtDW.temporalCounter_i1++;
  }

  if (rtDW.is_active_c1_flightRegulatorCFB == 0U) {
    rtDW.is_active_c1_flightRegulatorCFB = 1U;
    rtDW.is_flightControlMode = IN_startFlight;
    rtDW.numberOfManeuvers = 10.0;
    rtDW.cntManeuver = 1.0;
  } else {
    guard1 = false;
    guard2 = false;
    switch (rtDW.is_flightControlMode) {
     case IN_FinishTest:
      break;

     case IN_Pause:
      if (rtDW.temporalCounter_i1 >= rtU.pauseTimeBetweenManeuvers) {
        rtDW.is_flightControlMode = IN_runTest;
        rtDW.temporalCounter_i1 = 0U;

        /* Outport: '<Root>/tightenSling' incorporates:
         *  Inport: '<Root>/arrayTightenSlings'
         */
        rtY.tightenSling = rtU.arrayTightenSlings[(int32_T)rtDW.cntManeuver - 1];
        rtDW.directionOfRotation = rtU.arrayLeftRightManeuvers[(int32_T)
          rtDW.cntManeuver - 1];

        /* Outport: '<Root>/cmdTightenSlings' incorporates:
         *  Inport: '<Root>/arrayLeftRightManeuvers'
         */
        rtY.cmdTightenSlings = (rtDW.directionOfRotation == 2.0);
        rtDW.timeManeuver = rtU.arrayTimes[(int32_T)rtDW.cntManeuver - 1];
      }
      break;

     case IN_runTest:
      if (rtDW.temporalCounter_i1 >= (uint32_T)rtDW.timeManeuver) {
        if (rtDW.cntManeuver < rtDW.numberOfManeuvers) {
          rtDW.cntManeuver++;
          rtDW.is_flightControlMode = IN_Pause;
          rtDW.temporalCounter_i1 = 0U;

          /* Outport: '<Root>/tightenSling' */
          rtY.tightenSling = 0.0;
          rtDW.directionOfRotation = 0.0;

          /* Outport: '<Root>/cmdTightenSlings' */
          rtY.cmdTightenSlings = 0.0;
        } else if (rtDW.cntManeuver == rtDW.numberOfManeuvers) {
          guard1 = true;
        } else {
          guard2 = true;
        }
      } else {
        guard2 = true;
      }
      break;

     default:
      rtDW.is_flightControlMode = IN_runTest;
      rtDW.temporalCounter_i1 = 0U;
      tightenSling_tmp = (int32_T)rtDW.cntManeuver - 1;

      /* Outport: '<Root>/tightenSling' incorporates:
       *  Inport: '<Root>/arrayTightenSlings'
       */
      rtY.tightenSling = rtU.arrayTightenSlings[tightenSling_tmp];
      rtDW.directionOfRotation = rtU.arrayLeftRightManeuvers[tightenSling_tmp];

      /* Outport: '<Root>/cmdTightenSlings' incorporates:
       *  Inport: '<Root>/arrayLeftRightManeuvers'
       */
      rtY.cmdTightenSlings = (rtDW.directionOfRotation == 2.0);
      rtDW.timeManeuver = rtU.arrayTimes[tightenSling_tmp];
      break;
    }

    if (guard2) {
      if (rtDW.directionOfRotation == 0.0) {
        guard1 = true;
      }
    }

    if (guard1) {
      rtDW.is_flightControlMode = IN_FinishTest;
      rtDW.directionOfRotation = 0.0;

      /* Outport: '<Root>/tightenSling' */
      rtY.tightenSling = 0.0;

      /* Outport: '<Root>/cmdTightenSlings' */
      rtY.cmdTightenSlings = 0.0;
    }
  }

  /* End of Chart: '<S4>/paramСalculation' */

  /* Switch: '<S1>/Switch1' incorporates:
   *  Constant: '<S7>/Constant'
   *  RelationalOperator: '<S7>/Compare'
   */
  if (rtb_Add <= 400.0) {
    /* Outport: '<Root>/directionOfRotation' incorporates:
     *  Constant: '<S1>/Constant2'
     */
    rtY.directionOfRotation = 0.0;
  } else {
    /* Outport: '<Root>/directionOfRotation' */
    rtY.directionOfRotation = rtDW.directionOfRotation;
  }

  /* End of Switch: '<S1>/Switch1' */
  /* End of Outputs for SubSystem: '<Root>/flightRegulatorCFB' */

  /* Outport: '<Root>/tz' */
  rtY.tz = 0.0;

  /* Outport: '<Root>/tx' */
  rtY.tx = 0.0;

  /* Outport: '<Root>/modeFlight' */
  rtY.modeFlight = 0.0;

  /* Outport: '<Root>/doingManeuverMode' */
  rtY.doingManeuverMode = 0.0;

  /* Outport: '<Root>/angleAngle ' */
  rtY.angleAngle = 0.0;

  /* Outputs for Atomic SubSystem: '<Root>/flightRegulatorCFB' */
  /* Outport: '<Root>/cmdTouchDown' incorporates:
   *  DataTypeConversion: '<S1>/Data Type Conversion12'
   *  Inport: '<Root>/highStopUPS'
   *  Inport: '<Root>/isVeracityGns'
   *  Logic: '<S5>/Logical Operator'
   *  RelationalOperator: '<S5>/Relational Operator'
   */
  rtY.cmdTouchDown = (uint8_T)((rtb_Add <= rtU.highStopUPS) &&
    (rtU.isVeracityGns != 0.0));

  /* Outport: '<Root>/Period_ms' incorporates:
   *  Gain: '<S1>/Gain1'
   */
  rtY.Period_ms = 1000.0;

  /* End of Outputs for SubSystem: '<Root>/flightRegulatorCFB' */

  /* Outport: '<Root>/txNextPoint' */
  rtY.txNextPoint = 0.0;

  /* Outport: '<Root>/distanceAB' */
  rtY.distanceAB = 0.0;

  /* Outport: '<Root>/distanceB' */
  rtY.distanceB = 0.0;

  /* Outport: '<Root>/diffAngle' */
  rtY.diffAngle = 0.0;

  /* Outport: '<Root>/setAngle' */
  rtY.setAngle = 0.0;

  /* Outport: '<Root>/stateAngleCorrection' */
  rtY.stateAngleCorrection = 0.0;

  /* Outport: '<Root>/stateAngleDoing' */
  rtY.stateAngleDoing = 0.0;

  /* Outport: '<Root>/stateTurn' */
  rtY.stateTurn = 0.0;

  /* Outport: '<Root>/changeControl' */
  rtY.changeControl = 0.0;

  /* Outport: '<Root>/lat1' */
  rtY.lat1 = 0.0;

  /* Outport: '<Root>/lat2' */
  rtY.lat2 = 0.0;

  /* Outport: '<Root>/lon1' */
  rtY.lon1 = 0.0;

  /* Outport: '<Root>/lon2' */
  rtY.lon2 = 0.0;

  /* Outport: '<Root>/orderAngle' */
  rtY.orderAngle = 0.0;

  /* Outport: '<Root>/distance2' */
  rtY.distance2 = 0.0;

  /* Outport: '<Root>/tD' */
  rtY.tD = 0.0;

  /* Outport: '<Root>/executeMode' */
  rtY.executeMode = 0.0;

  /* Outport: '<Root>/updateAngle' */
  rtY.updateAngle = 0.0;

  /* Outport: '<Root>/high' */
  rtY.high = rtb_Add;
}

/* Model initialize function */
void flightRegulatorCFB_initialize(void)
{
  /* (no initialization code required) */
}

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
