/*
 * File: flightRegulatorCFB.h
 *
 * Code generated for Simulink model 'flightRegulatorCFB'.
 *
 * Model version                  : 1.1334
 * Simulink Coder version         : 8.14 (R2018a) 06-Feb-2018
 * C/C++ source code generated on : Thu Jul 25 18:36:38 2019
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
#include <stddef.h>
#include "rtwtypes.h"
#include <float.h>
#include <math.h>
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

#ifndef DEFINED_TYPEDEF_FOR_flightMode1ModeType_
#define DEFINED_TYPEDEF_FOR_flightMode1ModeType_

typedef enum {
  flightMode1ModeType_None = 0,        /* Default value */
  flightMode1ModeType_flightStart,
  flightMode1ModeType_flightTurning,
  flightMode1ModeType_flightBox,
  flightMode1ModeType_finishLine,
  flightMode1ModeType_flightLine
} flightMode1ModeType;

#endif

#ifndef DEFINED_TYPEDEF_FOR_pointsModeType_
#define DEFINED_TYPEDEF_FOR_pointsModeType_

typedef enum {
  pointsModeType_None = 0,             /* Default value */
  pointsModeType_startBox,
  pointsModeType_goToPoint1,
  pointsModeType_pausePoint1,
  pointsModeType_goToPoint2,
  pointsModeType_pausePoint4,
  pointsModeType_goToPoint3,
  pointsModeType_pausePoint3,
  pointsModeType_goToPoint4,
  pointsModeType_pausePoint5
} pointsModeType;

#endif

#ifndef DEFINED_TYPEDEF_FOR_waitingEnginesModeType_
#define DEFINED_TYPEDEF_FOR_waitingEnginesModeType_

typedef enum {
  waitingEnginesModeType_None = 0,     /* Default value */
  waitingEnginesModeType_enableEngines,
  waitingEnginesModeType_disableEngines
} waitingEnginesModeType;

#endif

#ifndef DEFINED_TYPEDEF_FOR_TurnModeType_
#define DEFINED_TYPEDEF_FOR_TurnModeType_

typedef enum {
  TurnModeType_None = 0,               /* Default value */
  TurnModeType_wait,
  TurnModeType_turnRight,
  TurnModeType_turnLeft
} TurnModeType;

#endif

#ifndef DEFINED_TYPEDEF_FOR_angleCorrectionModeType_
#define DEFINED_TYPEDEF_FOR_angleCorrectionModeType_

typedef enum {
  angleCorrectionModeType_None = 0,    /* Default value */
  angleCorrectionModeType_startAngleCorrection,
  angleCorrectionModeType_rightAngle,
  angleCorrectionModeType_waitZeroDistance,
  angleCorrectionModeType_leftAngle
} angleCorrectionModeType;

#endif

#ifndef DEFINED_TYPEDEF_FOR_angleDoingModeType_
#define DEFINED_TYPEDEF_FOR_angleDoingModeType_

typedef enum {
  angleDoingModeType_None = 0,         /* Default value */
  angleDoingModeType_angleWaitCorrection,
  angleDoingModeType_angleCorrection,
  angleDoingModeType_waitOrderAngle
} angleDoingModeType;

#endif

/* Block signals and states (default storage) for system '<Root>' */
typedef struct {
  real_T TmpSignalConversionAtSFunctionI[3];/* '<S3>/parameter&#x421;alculation' */
  real_T TmpSignalConversionAtSFunctio_j[3];/* '<S3>/parameter&#x421;alculation' */
  real_T boxPoints[8];                 /* '<S3>/parameter&#x421;alculation' */
  real_T outAngle;                     /* '<S4>/holdingAngle' */
  real_T outAngle_f;                   /* '<S4>/distanceRegulator' */
  real_T orderAngle;                   /* '<S4>/distanceRegulator' */
  real_T diffAngle;                    /* '<S4>/angleRegulator' */
  real_T diffAngleDegAbs;              /* '<S4>/angleRegulator' */
  real_T orderAngle_a;                 /* '<S4>/angleRegulator' */
  real_T outSetAngle;                  /* '<S4>/angleRegulator' */
  real_T outDistanceB;                 /* '<S3>/parameter&#x421;alculation' */
  real_T azimut;                       /* '<S3>/parameter&#x421;alculation' */
  real_T outDistanceAB;                /* '<S3>/parameter&#x421;alculation' */
  real_T orderAngle_g;                 /* '<S3>/parameter&#x421;alculation' */
  real_T outAngle_h;                   /* '<S3>/parameter&#x421;alculation' */
  real_T isOffFlightTurning;           /* '<S3>/parameter&#x421;alculation' */
  real_T tD;                           /* '<S3>/parameter&#x421;alculation' */
  real_T azimutB;                      /* '<S3>/parameter&#x421;alculation' */
  real_T phi1;                         /* '<S3>/parameter&#x421;alculation' */
  real_T phi2;                         /* '<S3>/parameter&#x421;alculation' */
  real_T la1;                          /* '<S3>/parameter&#x421;alculation' */
  real_T la2;                          /* '<S3>/parameter&#x421;alculation' */
  real_T isFlightBox;                  /* '<S3>/parameter&#x421;alculation' */
  real_T outDistance2;                 /* '<S3>/parameter&#x421;alculation' */
  real_T outPrecision;                 /* '<S3>/parameter&#x421;alculation' */
  real_T localOutAngle;                /* '<S4>/distanceRegulator' */
  real_T angleAdded;                   /* '<S4>/distanceRegulator' */
  real_T lastDirectionOfRotation;      /* '<S4>/angleRegulator' */
  real_T timeWaitEngines;              /* '<S4>/angleRegulator' */
  real_T enableEngines;                /* '<S4>/angleRegulator' */
  real_T pTightenSling;                /* '<S4>/angleRegulator' */
  real_T radEarth;                     /* '<S3>/parameter&#x421;alculation' */
  real_T laC;                          /* '<S3>/parameter&#x421;alculation' */
  real_T phiC;                         /* '<S3>/parameter&#x421;alculation' */
  real_T angleCorridor;                /* '<S3>/parameter&#x421;alculation' */
  real_T last_phi2;                    /* '<S3>/parameter&#x421;alculation' */
  real_T last_la2;                     /* '<S3>/parameter&#x421;alculation' */
  real_T flatXC;                       /* '<S3>/parameter&#x421;alculation' */
  real_T flatYC;                       /* '<S3>/parameter&#x421;alculation' */
  real_T flatX1;                       /* '<S3>/parameter&#x421;alculation' */
  real_T flatY1;                       /* '<S3>/parameter&#x421;alculation' */
  real_T updatePoint;                  /* '<S3>/parameter&#x421;alculation' */
  real_T changePoint;                  /* '<S3>/parameter&#x421;alculation' */
  real_T laB;                          /* '<S3>/parameter&#x421;alculation' */
  real_T phiB;                         /* '<S3>/parameter&#x421;alculation' */
  real_T distanceStartBox;             /* '<S3>/parameter&#x421;alculation' */
  real_T sideBox;                      /* '<S3>/parameter&#x421;alculation' */
  real_T isCalcBox;                    /* '<S3>/parameter&#x421;alculation' */
  real_T minimumDistance;              /* '<S3>/parameter&#x421;alculation' */
  real_T tdMinBox;                     /* '<S3>/parameter&#x421;alculation' */
  real_T tdMinFinish;                  /* '<S3>/parameter&#x421;alculation' */
  real_T ticksPauseBox;                /* '<S3>/parameter&#x421;alculation' */
  int32_T sfEvent;                     /* '<S3>/parameter&#x421;alculation' */
  uint32_T temporalCounter_i1;         /* '<S4>/angleRegulator' */
  uint32_T temporalCounter_i4;         /* '<S3>/parameter&#x421;alculation' */
  flightMode1ModeType flightMode1Mode; /* '<S3>/parameter&#x421;alculation' */
  pointsModeType pointsMode;           /* '<S3>/parameter&#x421;alculation' */
  waitingEnginesModeType waitingEnginesMode;/* '<S4>/angleRegulator' */
  TurnModeType TurnMode;               /* '<S4>/angleRegulator' */
  angleCorrectionModeType angleCorrectionMode;/* '<S4>/distanceRegulator' */
  angleDoingModeType angleDoingMode;   /* '<S4>/distanceRegulator' */
  struct {
    uint_T is_flightMode:3;            /* '<S3>/parameter&#x421;alculation' */
    uint_T is_c6_flightRegulatorCFB:2; /* '<S4>/holdingAngle' */
    uint_T is_controlAngle:2;          /* '<S4>/distanceRegulator' */
    uint_T is_resetOrder:2;            /* '<S4>/angleRegulator' */
    uint_T is_updatePlan:2;            /* '<S3>/parameter&#x421;alculation' */
    uint_T is_updateAngle:2;           /* '<S3>/parameter&#x421;alculation' */
    uint_T is_azimutInCorridor:2;      /* '<S3>/parameter&#x421;alculation' */
    uint_T is_criteriaDefinition:2;    /* '<S3>/parameter&#x421;alculation' */
    uint_T is_mathBox:2;               /* '<S3>/parameter&#x421;alculation' */
    uint_T is_active_c6_flightRegulatorCFB:1;/* '<S4>/holdingAngle' */
    uint_T is_active_c4_flightRegulatorCFB:1;/* '<S4>/distanceRegulator' */
    uint_T is_active_c5_flightRegulatorCFB:1;/* '<S4>/angleRegulator' */
    uint_T is_active_c1_flightRegulatorCFB:1;/* '<S3>/parameter&#x421;alculation' */
    uint_T is_calculateDistance1:1;    /* '<S3>/parameter&#x421;alculation' */
    uint_T is_active_calculateDistance1:1;/* '<S3>/parameter&#x421;alculation' */
    uint_T is_changePoint1:1;          /* '<S3>/parameter&#x421;alculation' */
    uint_T is_active_changePoint1:1;   /* '<S3>/parameter&#x421;alculation' */
    uint_T is_active_updatePlan:1;     /* '<S3>/parameter&#x421;alculation' */
    uint_T is_active_updateAngle:1;    /* '<S3>/parameter&#x421;alculation' */
    uint_T is_active_azimutInCorridor:1;/* '<S3>/parameter&#x421;alculation' */
    uint_T is_active_flightMode1:1;    /* '<S3>/parameter&#x421;alculation' */
    uint_T is_active_flightMode:1;     /* '<S3>/parameter&#x421;alculation' */
    uint_T is_active_criteriaDefinition:1;/* '<S3>/parameter&#x421;alculation' */
    uint_T is_active_mathBox:1;        /* '<S3>/parameter&#x421;alculation' */
    uint_T is_criteriaDefinition1:1;   /* '<S3>/parameter&#x421;alculation' */
    uint_T is_active_criteriaDefinition1:1;/* '<S3>/parameter&#x421;alculation' */
    uint_T is_active_points:1;         /* '<S3>/parameter&#x421;alculation' */
    uint_T is_active_outControl:1;     /* '<S3>/parameter&#x421;alculation' */
  } bitsForTID0;

  uint8_T temporalCounter_i1_f;        /* '<S4>/distanceRegulator' */
  uint8_T temporalCounter_i2;          /* '<S4>/distanceRegulator' */
  uint8_T temporalCounter_i1_fv;       /* '<S3>/parameter&#x421;alculation' */
  uint8_T temporalCounter_i2_g;        /* '<S3>/parameter&#x421;alculation' */
  uint8_T temporalCounter_i3;          /* '<S3>/parameter&#x421;alculation' */
} DW;

/* External inputs (root inport signals with default storage) */
typedef struct {
  real_T XYZi[3];                      /* '<Root>/XYZi' */
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
} ExtU;

/* External outputs (root outports fed by signals with default storage) */
typedef struct {
  real_T tightenSling;                 /* '<Root>/tightenSling' */
  real_T directionOfRotation;          /* '<Root>/directionOfRotation' */
  real_T tz;                           /* '<Root>/tz' */
  real_T tx;                           /* '<Root>/tx' */
  real_T modeFlight;                   /* '<Root>/modeFlight' */
  uint8_T doingManeuverMode;           /* '<Root>/doingManeuverMode' */
  real_T angleAngle;                   /* '<Root>/angleAngle ' */
  uint8_T cmdTightenSlings;            /* '<Root>/cmdTightenSlings' */
  uint8_T cmdTouchDown;                /* '<Root>/cmdTouchDown' */
  real_T Period_ms;                    /* '<Root>/Period_ms' */
  real_T txNextPoint;                  /* '<Root>/txNextPoint' */
  int16_T distanceAB;                  /* '<Root>/distanceAB' */
  uint16_T distanceB;                  /* '<Root>/distanceB' */
  int16_T diffAngle;                   /* '<Root>/diffAngle' */
  uint16_T setAngle;                   /* '<Root>/setAngle' */
  uint8_T stateAngleCorrection;        /* '<Root>/stateAngleCorrection' */
  uint8_T stateAngleDoing;             /* '<Root>/stateAngleDoing' */
  uint8_T stateTurn;                   /* '<Root>/stateTurn' */
  uint8_T changeControl;               /* '<Root>/changeControl' */
  real_T lat1;                         /* '<Root>/lat1' */
  real_T lat2;                         /* '<Root>/lat2' */
  real_T lon1;                         /* '<Root>/lon1' */
  real_T lon2;                         /* '<Root>/lon2' */
  real_T orderAngle;                   /* '<Root>/orderAngle' */
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
 * These blocks were eliminated from the model due to optimizations:
 *
 * Block '<S1>/Scope' : Unused code path elimination
 * Block '<S1>/Scope3' : Unused code path elimination
 * Block '<S3>/Display4' : Unused code path elimination
 * Block '<S3>/Gain5' : Unused code path elimination
 * Block '<S3>/Gain6' : Unused code path elimination
 * Block '<S3>/Scope' : Unused code path elimination
 * Block '<S3>/Scope1' : Unused code path elimination
 * Block '<S3>/Scope2' : Unused code path elimination
 * Block '<S3>/Scope3' : Unused code path elimination
 * Block '<S3>/Scope4' : Unused code path elimination
 * Block '<S3>/Scope5' : Unused code path elimination
 * Block '<S3>/distancePoint2' : Unused code path elimination
 * Block '<S4>/Constant1' : Unused code path elimination
 * Block '<S4>/Display' : Unused code path elimination
 * Block '<S4>/Scope' : Unused code path elimination
 * Block '<S4>/Scope1' : Unused code path elimination
 * Block '<S4>/Scope2' : Unused code path elimination
 * Block '<S4>/Scope3' : Unused code path elimination
 * Block '<S4>/Scope4' : Unused code path elimination
 * Block '<S4>/Scope5' : Unused code path elimination
 * Block '<S4>/piToDeg1' : Unused code path elimination
 * Block '<S4>/piToDeg10' : Unused code path elimination
 * Block '<S4>/piToDeg12' : Unused code path elimination
 * Block '<S4>/piToDeg2' : Unused code path elimination
 * Block '<S4>/piToDeg3' : Unused code path elimination
 * Block '<S4>/piToDeg4' : Unused code path elimination
 * Block '<S4>/piToDeg5' : Unused code path elimination
 * Block '<S4>/piToDeg6' : Unused code path elimination
 * Block '<S4>/piToDeg7' : Unused code path elimination
 * Block '<S4>/piToDeg8' : Unused code path elimination
 * Block '<S4>/piToDeg9' : Unused code path elimination
 * Block '<S5>/Constant1' : Unused code path elimination
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
 * hilite_system('kinematic25/flightRegulatorCFB')    - opens subsystem kinematic25/flightRegulatorCFB
 * hilite_system('kinematic25/flightRegulatorCFB/Kp') - opens and selects block Kp
 *
 * Here is the system hierarchy for this model
 *
 * '<Root>' : 'kinematic25'
 * '<S1>'   : 'kinematic25/flightRegulatorCFB'
 * '<S2>'   : 'kinematic25/flightRegulatorCFB/Model Info'
 * '<S3>'   : 'kinematic25/flightRegulatorCFB/Subsystem'
 * '<S4>'   : 'kinematic25/flightRegulatorCFB/regulator'
 * '<S5>'   : 'kinematic25/flightRegulatorCFB/touchDown'
 * '<S6>'   : 'kinematic25/flightRegulatorCFB/Subsystem/parameter&#x421;alculation'
 * '<S7>'   : 'kinematic25/flightRegulatorCFB/regulator/angleRegulator'
 * '<S8>'   : 'kinematic25/flightRegulatorCFB/regulator/distanceRegulator'
 * '<S9>'   : 'kinematic25/flightRegulatorCFB/regulator/fuzzyAngle'
 * '<S10>'  : 'kinematic25/flightRegulatorCFB/regulator/holdingAngle'
 * '<S11>'  : 'kinematic25/flightRegulatorCFB/regulator/fuzzyAngle/Defuzzify Outputs'
 * '<S12>'  : 'kinematic25/flightRegulatorCFB/regulator/fuzzyAngle/Evaluate Rule Antecedents'
 * '<S13>'  : 'kinematic25/flightRegulatorCFB/regulator/fuzzyAngle/Evaluate Rule Consequents'
 * '<S14>'  : 'kinematic25/flightRegulatorCFB/touchDown/Compare To Constant1'
 */
#endif                                 /* RTW_HEADER_flightRegulatorCFB_h_ */

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
