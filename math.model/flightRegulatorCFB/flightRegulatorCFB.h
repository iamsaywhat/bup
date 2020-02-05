/*
 * File: flightRegulatorCFB.h
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

#ifndef RTW_HEADER_flightRegulatorCFB_h_
#define RTW_HEADER_flightRegulatorCFB_h_
#ifndef flightRegulatorCFB_COMMON_INCLUDES_
# define flightRegulatorCFB_COMMON_INCLUDES_
#include "rtwtypes.h"
#endif                                 /* flightRegulatorCFB_COMMON_INCLUDES_ */

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
  real_T directionOfRotation;          /* '<S4>/paramСalculation' */
  real_T cntManeuver;                  /* '<S4>/paramСalculation' */
  real_T numberOfManeuvers;            /* '<S4>/paramСalculation' */
  real_T timeManeuver;                 /* '<S4>/paramСalculation' */
  uint32_T temporalCounter_i1;         /* '<S4>/paramСalculation' */
  uint8_T is_active_c1_flightRegulatorCFB;/* '<S4>/paramСalculation' */
  uint8_T is_flightControlMode;        /* '<S4>/paramСalculation' */
} DW;

/* External inputs (root inport signals with default storage) */
typedef struct {
  real_T XYZi[3];                      /* '<Root>/XYZi' */
  real_T arrayLeftRightManeuvers[10];  /* '<Root>/arrayLeftRightManeuvers' */
  real_T arrayTightenSlings[10];       /* '<Root>/arrayTightenSlings' */
  real_T arrayTimes[10];               /* '<Root>/arrayTimes' */
  real_T xyzPoints[3];                 /* '<Root>/xyzPoints' */
  real_T isVeracityGns;                /* '<Root>/isVeracityGns' */
  real_T isAvailableDigitalTerrain;    /* '<Root>/isAvailableDigitalTerrain' */
  real_T HighDigitalTerrain;           /* '<Root>/HighDigitalTerrain' */
  real_T WindVelocity;                 /* '<Root>/WindVelocity' */
  real_T LeftEnginehadWork;            /* '<Root>/LeftEnginehadWork' */
  real_T RightEnginehadWork;           /* '<Root>/RightEnginehadWork' */
  real_T isEnginesAvailable;           /* '<Root>/isEnginesAvailable' */
  real_T isNormalEnginesPower;         /* '<Root>/isNormalEnginesPower' */
  real_T svsHigh;                      /* '<Root>/svsHigh' */
  real_T SumOfGns;                     /* '<Root>/SumOfGns' */
  real_T udOr3dofGns;                  /* '<Root>/2dOr3dofGns ' */
  real_T angle;                        /* '<Root>/angle' */
  real_T highStopUPS;                  /* '<Root>/highStopUPS' */
  real_T airVelocityHor;               /* '<Root>/airVelocityHor' */
  uint32_T pauseTimeBetweenManeuvers;  /* '<Root>/pauseTimeBetweenManeuvers' */
} ExtU;

/* External outputs (root outports fed by signals with default storage) */
typedef struct {
  real_T tightenSling;                 /* '<Root>/tightenSling' */
  real_T directionOfRotation;          /* '<Root>/directionOfRotation' */
  real_T tz;                           /* '<Root>/tz' */
  real_T tx;                           /* '<Root>/tx' */
  real_T modeFlight;                   /* '<Root>/modeFlight' */
  real_T doingManeuverMode;            /* '<Root>/doingManeuverMode' */
  real_T angleAngle;                   /* '<Root>/angleAngle ' */
  real_T cmdTightenSlings;             /* '<Root>/cmdTightenSlings' */
  uint8_T cmdTouchDown;                /* '<Root>/cmdTouchDown' */
  real_T Period_ms;                    /* '<Root>/Period_ms' */
  real_T txNextPoint;                  /* '<Root>/txNextPoint' */
  real_T distanceAB;                   /* '<Root>/distanceAB' */
  real_T distanceB;                    /* '<Root>/distanceB' */
  real_T diffAngle;                    /* '<Root>/diffAngle' */
  real_T setAngle;                     /* '<Root>/setAngle' */
  real_T stateAngleCorrection;         /* '<Root>/stateAngleCorrection' */
  real_T stateAngleDoing;              /* '<Root>/stateAngleDoing' */
  real_T stateTurn;                    /* '<Root>/stateTurn' */
  real_T changeControl;                /* '<Root>/changeControl' */
  real_T lat1;                         /* '<Root>/lat1' */
  real_T lat2;                         /* '<Root>/lat2' */
  real_T lon1;                         /* '<Root>/lon1' */
  real_T lon2;                         /* '<Root>/lon2' */
  real_T orderAngle;                   /* '<Root>/orderAngle' */
  real_T distance2;                    /* '<Root>/distance2' */
  real_T tD;                           /* '<Root>/tD' */
  real_T executeMode;                  /* '<Root>/executeMode' */
  real_T updateAngle;                  /* '<Root>/updateAngle' */
  real_T high;                         /* '<Root>/high' */
} ExtY;

/* Real-time Model Data Structure */
struct tag_RTM {
  const char_T * volatile errorStatus;
};

/* Block signals and states (default storage) */
extern DW rtDW;

/* External inputs (root inport signals with default storage) */
extern ExtU rtU;

/* External outputs (root outports fed by signals with default storage) */
extern ExtY rtY;

/* Model entry point functions */
extern void flightRegulatorCFB_initialize(void);
extern void flightRegulatorCFB_step(void);

/* Real-time Model object */
extern RT_MODEL *const rtM;

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
 * hilite_system('testFCB1_0/flightRegulatorCFB')    - opens subsystem testFCB1_0/flightRegulatorCFB
 * hilite_system('testFCB1_0/flightRegulatorCFB/Kp') - opens and selects block Kp
 *
 * Here is the system hierarchy for this model
 *
 * '<Root>' : 'testFCB1_0'
 * '<S1>'   : 'testFCB1_0/flightRegulatorCFB'
 * '<S2>'   : 'testFCB1_0/flightRegulatorCFB/Degrees to Radians3'
 * '<S3>'   : 'testFCB1_0/flightRegulatorCFB/Model Info'
 * '<S4>'   : 'testFCB1_0/flightRegulatorCFB/Subsystem'
 * '<S5>'   : 'testFCB1_0/flightRegulatorCFB/touchDown'
 * '<S6>'   : 'testFCB1_0/flightRegulatorCFB/Subsystem/paramСalculation'
 * '<S7>'   : 'testFCB1_0/flightRegulatorCFB/touchDown/Compare To Constant2'
 */
#endif                                 /* RTW_HEADER_flightRegulatorCFB_h_ */

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
