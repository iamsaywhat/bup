/*
 * File: flightRegulatorCFB.h
 *
 * Code generated for Simulink model 'flightRegulatorCFB'.
 *
 * Model version                  : 1.2565
 * Simulink Coder version         : 9.0 (R2018b) 24-May-2018
 * C/C++ source code generated on : Fri Nov 29 17:24:23 2019
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
#include "rtwtypes.h"
#include <stddef.h>
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

#ifndef DEFINED_TYPEDEF_FOR_pointsModeType_
#define DEFINED_TYPEDEF_FOR_pointsModeType_

typedef enum {
  pointsModeType_None = 0,             /* Default value */
  pointsModeType_startBox,
  pointsModeType_goToPoint1,
  pointsModeType_pausePoint1,
  pointsModeType_goToPoint2,
  pointsModeType_pausePoint2,
  pointsModeType_goToPoint3,
  pointsModeType_pausePoint3,
  pointsModeType_goToPoint4,
  pointsModeType_pausePoint4
} pointsModeType;

#endif

#ifndef DEFINED_TYPEDEF_FOR_criteriaDefinition1ModeType_
#define DEFINED_TYPEDEF_FOR_criteriaDefinition1ModeType_

typedef enum {
  criteriaDefinition1ModeType_None = 0,/* Default value */
  criteriaDefinition1ModeType_desicion
} criteriaDefinition1ModeType;

#endif

#ifndef DEFINED_TYPEDEF_FOR_flightSnakeModeType_
#define DEFINED_TYPEDEF_FOR_flightSnakeModeType_

typedef enum {
  flightSnakeModeType_None = 0,        /* Default value */
  flightSnakeModeType_CalculateParameters,
  flightSnakeModeType_leftShoulder,
  flightSnakeModeType_pauseOrder1,
  flightSnakeModeType_rightShoulder,
  flightSnakeModeType_pauseOrder
} flightSnakeModeType;

#endif

#ifndef DEFINED_TYPEDEF_FOR_flightExecuteModeModeType_
#define DEFINED_TYPEDEF_FOR_flightExecuteModeModeType_

typedef enum {
  flightExecuteModeModeType_None = 0,  /* Default value */
  flightExecuteModeModeType_flightStart,
  flightExecuteModeModeType_flightLine,
  flightExecuteModeModeType_finishLine,
  flightExecuteModeModeType_flightTurning,
  flightExecuteModeModeType_flightSnake,
  flightExecuteModeModeType_flightBox
} flightExecuteModeModeType;

#endif

#ifndef DEFINED_TYPEDEF_FOR_flightControlModeModeType_
#define DEFINED_TYPEDEF_FOR_flightControlModeModeType_

typedef enum {
  flightControlModeModeType_None = 0,  /* Default value */
  flightControlModeModeType_flightTurning,
  flightControlModeModeType_flightLine,
  flightControlModeModeType_flightSnake
} flightControlModeModeType;

#endif

#ifndef DEFINED_TYPEDEF_FOR_TurnModeType_
#define DEFINED_TYPEDEF_FOR_TurnModeType_

typedef enum {
  TurnModeType_None = 0,               /* Default value */
  TurnModeType_wait,
  TurnModeType_turning
} TurnModeType;

#endif

/* Block signals and states (default storage) for system '<S25>/calcRelationalParameters.CalcParam_tD' */
typedef struct {
  real_T tPprev0;                      /* '<S25>/calcRelationalParameters.CalcParam_tD' */
  uint32_T temporalCounter_i1;         /* '<S25>/calcRelationalParameters.CalcParam_tD' */
  uint8_T is_c10_libraryFCB;           /* '<S25>/calcRelationalParameters.CalcParam_tD' */
} DW_calcRelationalParametersCalc;

/* Block signals and states (default storage) for system '<Root>' */
typedef struct {
  DW_calcRelationalParametersCalc sf_calcRelationalParametersCa_j;/* '<S25>/calcRelationalParameters.CalcParam_tZ' */
  DW_calcRelationalParametersCalc sf_calcRelationalParametersCalc;/* '<S25>/calcRelationalParameters.CalcParam_tD' */
  real_T Gain1[8];                     /* '<S2>/Gain1' */
  real_T TmpSignalConversionAtSFunctionI[3];/* '<S9>/paramСalculation' */
  real_T TmpSignalConversionAtSFunctio_i[3];/* '<S9>/paramСalculation' */
  real_T boxPoints[8];                 /* '<S9>/paramСalculation' */
  real_T Delay_DSTATE[2];              /* '<S13>/Delay' */
  real_T velocityHorizontal;           /* '<S9>/Discrete Transfer Fcn' */
  real_T velocityVertical;             /* '<S9>/Discrete Transfer Fcn1' */
  real_T DataTypeConversion;           /* '<S10>/Data Type Conversion' */
  real_T outAngle;                     /* '<S11>/holdingAngle' */
  real_T directionOfRotation;          /* '<S11>/angleRegulator' */
  real_T diffAngle;                    /* '<S11>/angleRegulator' */
  real_T orderAngle;                   /* '<S11>/angleRegulator' */
  real_T outSetAngle;                  /* '<S11>/angleRegulator' */
  real_T decisionSide;                 /* '<S11>/angleRegulator' */
  real_T timeTurn;                     /* '<S11>/angleRegulator' */
  real_T outDistanceB;                 /* '<S9>/paramСalculation' */
  real_T azimut;                       /* '<S9>/paramСalculation' */
  real_T outDistanceAB;                /* '<S9>/paramСalculation' */
  real_T outAngle_j;                   /* '<S9>/paramСalculation' */
  real_T isOffFlightTurning;           /* '<S9>/paramСalculation' */
  real_T azimutB;                      /* '<S9>/paramСalculation' */
  real_T phi1;                         /* '<S9>/paramСalculation' */
  real_T phi2;                         /* '<S9>/paramСalculation' */
  real_T la1;                          /* '<S9>/paramСalculation' */
  real_T la2;                          /* '<S9>/paramСalculation' */
  real_T outDistance2;                 /* '<S9>/paramСalculation' */
  real_T kDPrev;                       /* '<S9>/paramСalculation' */
  real_T tP0;                          /* '<S9>/paramСalculation' */
  real_T tP0_l;                        /* '<S9>/paramСalculation' */
  real_T DiscreteTransferFcn_states;   /* '<S9>/Discrete Transfer Fcn' */
  real_T DiscreteTransferFcn1_states;  /* '<S9>/Discrete Transfer Fcn1' */
  real_T UD_DSTATE;                    /* '<S56>/UD' */
  real_T Delay_DSTATE_p;               /* '<S14>/Delay' */
  real_T UD_DSTATE_d;                  /* '<S60>/UD' */
  real_T memoryInAngle;                /* '<S11>/holdingAngle' */
  real_T angleCorridor;                /* '<S11>/holdingAngle' */
  real_T timeWaitEngines;              /* '<S11>/angleRegulator' */
  real_T pTightenSling;                /* '<S11>/angleRegulator' */
  real_T pAngleVelocity;               /* '<S11>/angleRegulator' */
  real_T laC;                          /* '<S9>/paramСalculation' */
  real_T phiC;                         /* '<S9>/paramСalculation' */
  real_T hC;                           /* '<S9>/paramСalculation' */
  real_T angleCorridor_c;              /* '<S9>/paramСalculation' */
  real_T last_phi2;                    /* '<S9>/paramСalculation' */
  real_T last_la2;                     /* '<S9>/paramСalculation' */
  real_T flatXC;                       /* '<S9>/paramСalculation' */
  real_T flatYC;                       /* '<S9>/paramСalculation' */
  real_T flatX2;                       /* '<S9>/paramСalculation' */
  real_T flatY2;                       /* '<S9>/paramСalculation' */
  real_T flatX1;                       /* '<S9>/paramСalculation' */
  real_T flatY1;                       /* '<S9>/paramСalculation' */
  real_T changePoint;                  /* '<S9>/paramСalculation' */
  real_T laB;                          /* '<S9>/paramСalculation' */
  real_T phiB;                         /* '<S9>/paramСalculation' */
  real_T minimumDistance;              /* '<S9>/paramСalculation' */
  real_T pTimeOrderAngle;              /* '<S9>/paramСalculation' */
  real_T tShoulderSnake;               /* '<S9>/paramСalculation' */
  real_T pAngleSnake;                  /* '<S9>/paramСalculation' */
  real_T pAngleVelocity_f;             /* '<S9>/paramСalculation' */
  real_T periodicOrder;                /* '<S9>/paramСalculation' */
  real_T highTurnVelocity;             /* '<S9>/paramСalculation' */
  real_T nearPoint;                    /* '<S9>/paramСalculation' */
  real_T leftRightSholder;             /* '<S9>/paramСalculation' */
  int32_T sfEvent;                     /* '<S9>/paramСalculation' */
  uint32_T temporalCounter_i1;         /* '<S11>/angleRegulator' */
  uint32_T temporalCounter_i1_p;       /* '<S9>/paramСalculation' */
  uint32_T temporalCounter_i2;         /* '<S9>/paramСalculation' */
  uint32_T temporalCounter_i3;         /* '<S9>/paramСalculation' */
  pointsModeType pointsMode;           /* '<S9>/paramСalculation' */
  criteriaDefinition1ModeType criteriaDefinition1Mode;/* '<S9>/paramСalculation' */
  flightSnakeModeType flightSnakeMode; /* '<S9>/paramСalculation' */
  flightControlModeModeType flightControlModeMode;/* '<S9>/paramСalculation' */
  TurnModeType TurnMode;               /* '<S11>/angleRegulator' */
  uint8_T DiscreteTransferFcn_icLoad;  /* '<S9>/Discrete Transfer Fcn' */
  uint8_T DiscreteTransferFcn1_icLoad; /* '<S9>/Discrete Transfer Fcn1' */
  uint8_T is_active_c6_flightRegulatorCFB;/* '<S11>/holdingAngle' */
  uint8_T is_c6_flightRegulatorCFB;    /* '<S11>/holdingAngle' */
  uint8_T is_active_c5_flightRegulatorCFB;/* '<S11>/angleRegulator' */
  uint8_T is_resetOrder;               /* '<S11>/angleRegulator' */
  uint8_T is_active_c1_flightRegulatorCFB;/* '<S9>/paramСalculation' */
  uint8_T is_active_calcRelationalParamet;/* '<S9>/paramСalculation' */
  uint8_T is_active_CalcParam_tD;      /* '<S9>/paramСalculation' */
  uint8_T is_active_CalcParam_tZ;      /* '<S9>/paramСalculation' */
  uint8_T is_updateAngle;              /* '<S9>/paramСalculation' */
  uint8_T is_active_updateAngle;       /* '<S9>/paramСalculation' */
  uint8_T is_calculateDistance1;       /* '<S9>/paramСalculation' */
  uint8_T is_active_calculateDistance1;/* '<S9>/paramСalculation' */
  uint8_T is_changePoint1;             /* '<S9>/paramСalculation' */
  uint8_T is_active_changePoint1;      /* '<S9>/paramСalculation' */
  uint8_T is_updatePlan;               /* '<S9>/paramСalculation' */
  uint8_T is_active_updatePlan;        /* '<S9>/paramСalculation' */
  uint8_T is_active_criteriaDefinition1;/* '<S9>/paramСalculation' */
  uint8_T is_counterOrderAngle;        /* '<S9>/paramСalculation' */
  uint8_T is_active_counterOrderAngle; /* '<S9>/paramСalculation' */
  uint8_T is_azimutInCorridor;         /* '<S9>/paramСalculation' */
  uint8_T is_active_azimutInCorridor;  /* '<S9>/paramСalculation' */
  uint8_T is_active_flightExecuteMode; /* '<S9>/paramСalculation' */
  uint8_T is_active_points;            /* '<S9>/paramСalculation' */
  uint8_T is_active_outControl;        /* '<S9>/paramСalculation' */
  uint8_T is_active_flightControlMode; /* '<S9>/paramСalculation' */
  uint8_T is_mathBox;                  /* '<S9>/paramСalculation' */
  uint8_T is_active_mathBox;           /* '<S9>/paramСalculation' */
  uint8_T temporalCounter_i4;          /* '<S9>/paramСalculation' */
  boolean_T isLowering;                /* '<S9>/paramСalculation' */
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
  real_T highStopUPS;                  /* '<Root>/highStopUPS' */
  real_T airVelocityHor;               /* '<Root>/airVelocityHor' */
} ExtU;

/* External outputs (root outports fed by signals with default storage) */
typedef struct {
  real_T tightenSling;                 /* '<Root>/tightenSling' */
  real_T directionOfRotation;          /* '<Root>/directionOfRotation' */
  real_T tz;                           /* '<Root>/tz' */
  real_T tx;                           /* '<Root>/tx' */
  real_T modeFlight;                   /* '<Root>/modeFlight' */
  uint8_T doingManeuverMode;           /* '<Root>/doingManeuverMode' */
  uint16_T angleAngle;                 /* '<Root>/angleAngle ' */
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
  uint8_T orderAngle;                  /* '<Root>/orderAngle' */
  uint16_T distance2;                  /* '<Root>/distance2' */
  real_T tD;                           /* '<Root>/tD' */
  flightExecuteModeModeType executeMode;/* '<Root>/executeMode' */
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
 * These blocks were eliminated from the model due to optimizations:
 *
 * Block '<S1>/Scope' : Unused code path elimination
 * Block '<S1>/Scope1' : Unused code path elimination
 * Block '<S1>/Scope2' : Unused code path elimination
 * Block '<S1>/Scope3' : Unused code path elimination
 * Block '<S1>/Scope4' : Unused code path elimination
 * Block '<S9>/Display4' : Unused code path elimination
 * Block '<S15>/FixPt Data Type Duplicate' : Unused code path elimination
 * Block '<S16>/FixPt Data Type Duplicate' : Unused code path elimination
 * Block '<S21>/Gain' : Unused code path elimination
 * Block '<S22>/Gain' : Unused code path elimination
 * Block '<S23>/Gain' : Unused code path elimination
 * Block '<S24>/Gain' : Unused code path elimination
 * Block '<S9>/Scope1' : Unused code path elimination
 * Block '<S9>/Scope2' : Unused code path elimination
 * Block '<S9>/Scope3' : Unused code path elimination
 * Block '<S9>/Scope4' : Unused code path elimination
 * Block '<S9>/Scope5' : Unused code path elimination
 * Block '<S9>/Scope6' : Unused code path elimination
 * Block '<S9>/Scope7' : Unused code path elimination
 * Block '<S9>/Scope8' : Unused code path elimination
 * Block '<S9>/distancePoint1' : Unused code path elimination
 * Block '<S9>/distancePoint2' : Unused code path elimination
 * Block '<S9>/distancePoint3' : Unused code path elimination
 * Block '<S9>/distancePoint4' : Unused code path elimination
 * Block '<S10>/Add2' : Unused code path elimination
 * Block '<S29>/Compare' : Unused code path elimination
 * Block '<S29>/Constant' : Unused code path elimination
 * Block '<S30>/Compare' : Unused code path elimination
 * Block '<S30>/Constant' : Unused code path elimination
 * Block '<S31>/Compare' : Unused code path elimination
 * Block '<S31>/Constant' : Unused code path elimination
 * Block '<S32>/Compare' : Unused code path elimination
 * Block '<S32>/Constant' : Unused code path elimination
 * Block '<S33>/Compare' : Unused code path elimination
 * Block '<S33>/Constant' : Unused code path elimination
 * Block '<S10>/ErrorDisplay' : Unused code path elimination
 * Block '<S10>/leftBottomDistance' : Unused code path elimination
 * Block '<S10>/leftBottomPoint' : Unused code path elimination
 * Block '<S10>/leftTopDistance' : Unused code path elimination
 * Block '<S10>/leftTopPoint' : Unused code path elimination
 * Block '<S10>/notEqualZero1' : Unused code path elimination
 * Block '<S10>/notEqualZero2' : Unused code path elimination
 * Block '<S10>/notEqualZero3' : Unused code path elimination
 * Block '<S10>/notEqualZero4' : Unused code path elimination
 * Block '<S10>/numberFour' : Unused code path elimination
 * Block '<S10>/numberOne' : Unused code path elimination
 * Block '<S10>/numberThree' : Unused code path elimination
 * Block '<S10>/numberTwo' : Unused code path elimination
 * Block '<S10>/rightBottomDistance' : Unused code path elimination
 * Block '<S10>/rightBottomPoint' : Unused code path elimination
 * Block '<S10>/rightTopDistance' : Unused code path elimination
 * Block '<S10>/rightTopPoint' : Unused code path elimination
 * Block '<S1>/number' : Unused code path elimination
 * Block '<S1>/number1' : Unused code path elimination
 * Block '<S1>/number2' : Unused code path elimination
 * Block '<S1>/number3' : Unused code path elimination
 * Block '<S1>/number4' : Unused code path elimination
 * Block '<S47>/Gain' : Unused code path elimination
 * Block '<S48>/Gain' : Unused code path elimination
 * Block '<S49>/Gain' : Unused code path elimination
 * Block '<S50>/Gain' : Unused code path elimination
 * Block '<S51>/Gain' : Unused code path elimination
 * Block '<S11>/Scope' : Unused code path elimination
 * Block '<S11>/Scope1' : Unused code path elimination
 * Block '<S11>/Scope2' : Unused code path elimination
 * Block '<S11>/Scope3' : Unused code path elimination
 * Block '<S11>/Scope4' : Unused code path elimination
 * Block '<S56>/Data Type Duplicate' : Unused code path elimination
 * Block '<S13>/Display1' : Unused code path elimination
 * Block '<S13>/Display3' : Unused code path elimination
 * Block '<S57>/FixPt Data Type Duplicate' : Unused code path elimination
 * Block '<S58>/FixPt Data Type Duplicate' : Unused code path elimination
 * Block '<S13>/Scope' : Unused code path elimination
 * Block '<S60>/Data Type Duplicate' : Unused code path elimination
 * Block '<S14>/Display1' : Unused code path elimination
 * Block '<S14>/Display3' : Unused code path elimination
 * Block '<S61>/FixPt Data Type Duplicate' : Unused code path elimination
 * Block '<S62>/FixPt Data Type Duplicate' : Unused code path elimination
 * Block '<S28>/minus2' : Eliminated nontunable gain of 1
 * Block '<S28>/minus3' : Eliminated nontunable gain of 1
 * Block '<S28>/plus' : Eliminated nontunable gain of 1
 * Block '<S28>/plus2' : Eliminated nontunable gain of 1
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
 * hilite_system('maketKinematic27/flightRegulatorCFB')    - opens subsystem maketKinematic27/flightRegulatorCFB
 * hilite_system('maketKinematic27/flightRegulatorCFB/Kp') - opens and selects block Kp
 *
 * Here is the system hierarchy for this model
 *
 * '<Root>' : 'maketKinematic27'
 * '<S1>'   : 'maketKinematic27/flightRegulatorCFB'
 * '<S2>'   : 'maketKinematic27/flightRegulatorCFB/Degrees to Radians'
 * '<S3>'   : 'maketKinematic27/flightRegulatorCFB/Degrees to Radians1'
 * '<S4>'   : 'maketKinematic27/flightRegulatorCFB/Degrees to Radians2'
 * '<S5>'   : 'maketKinematic27/flightRegulatorCFB/Degrees to Radians3'
 * '<S6>'   : 'maketKinematic27/flightRegulatorCFB/Degrees to Radians4'
 * '<S7>'   : 'maketKinematic27/flightRegulatorCFB/Model Info'
 * '<S8>'   : 'maketKinematic27/flightRegulatorCFB/Radians to Degrees4'
 * '<S9>'   : 'maketKinematic27/flightRegulatorCFB/Subsystem'
 * '<S10>'  : 'maketKinematic27/flightRegulatorCFB/Subsystem1'
 * '<S11>'  : 'maketKinematic27/flightRegulatorCFB/regulator'
 * '<S12>'  : 'maketKinematic27/flightRegulatorCFB/touchDown'
 * '<S13>'  : 'maketKinematic27/flightRegulatorCFB/velocityAndAccelerationHorizontalBlock'
 * '<S14>'  : 'maketKinematic27/flightRegulatorCFB/velocityAndAccelerationVerticalBlock'
 * '<S15>'  : 'maketKinematic27/flightRegulatorCFB/Subsystem/Interval Test'
 * '<S16>'  : 'maketKinematic27/flightRegulatorCFB/Subsystem/Interval Test1'
 * '<S17>'  : 'maketKinematic27/flightRegulatorCFB/Subsystem/Radians to Degrees'
 * '<S18>'  : 'maketKinematic27/flightRegulatorCFB/Subsystem/Radians to Degrees1'
 * '<S19>'  : 'maketKinematic27/flightRegulatorCFB/Subsystem/Radians to Degrees2'
 * '<S20>'  : 'maketKinematic27/flightRegulatorCFB/Subsystem/Radians to Degrees3'
 * '<S21>'  : 'maketKinematic27/flightRegulatorCFB/Subsystem/Radians to Degrees4'
 * '<S22>'  : 'maketKinematic27/flightRegulatorCFB/Subsystem/Radians to Degrees5'
 * '<S23>'  : 'maketKinematic27/flightRegulatorCFB/Subsystem/Radians to Degrees6'
 * '<S24>'  : 'maketKinematic27/flightRegulatorCFB/Subsystem/Radians to Degrees7'
 * '<S25>'  : 'maketKinematic27/flightRegulatorCFB/Subsystem/paramСalculation'
 * '<S26>'  : 'maketKinematic27/flightRegulatorCFB/Subsystem/paramСalculation/calcRelationalParameters.CalcParam_tD'
 * '<S27>'  : 'maketKinematic27/flightRegulatorCFB/Subsystem/paramСalculation/calcRelationalParameters.CalcParam_tZ'
 * '<S28>'  : 'maketKinematic27/flightRegulatorCFB/Subsystem1/BoxPoints'
 * '<S29>'  : 'maketKinematic27/flightRegulatorCFB/Subsystem1/Compare To Constant'
 * '<S30>'  : 'maketKinematic27/flightRegulatorCFB/Subsystem1/Compare To Zero'
 * '<S31>'  : 'maketKinematic27/flightRegulatorCFB/Subsystem1/Compare To Zero1'
 * '<S32>'  : 'maketKinematic27/flightRegulatorCFB/Subsystem1/Compare To Zero2'
 * '<S33>'  : 'maketKinematic27/flightRegulatorCFB/Subsystem1/Compare To Zero3'
 * '<S34>'  : 'maketKinematic27/flightRegulatorCFB/Subsystem1/Degrees to Radians'
 * '<S35>'  : 'maketKinematic27/flightRegulatorCFB/Subsystem1/Degrees to Radians1'
 * '<S36>'  : 'maketKinematic27/flightRegulatorCFB/Subsystem1/Degrees to Radians2'
 * '<S37>'  : 'maketKinematic27/flightRegulatorCFB/Subsystem1/Degrees to Radians3'
 * '<S38>'  : 'maketKinematic27/flightRegulatorCFB/Subsystem1/distanceBetween2Points1'
 * '<S39>'  : 'maketKinematic27/flightRegulatorCFB/Subsystem1/distanceBetween2Points2'
 * '<S40>'  : 'maketKinematic27/flightRegulatorCFB/Subsystem1/distanceBetween2Points3'
 * '<S41>'  : 'maketKinematic27/flightRegulatorCFB/Subsystem1/distanceBetween2Points4'
 * '<S42>'  : 'maketKinematic27/flightRegulatorCFB/Subsystem1/BoxPoints/leftBottomCalc'
 * '<S43>'  : 'maketKinematic27/flightRegulatorCFB/Subsystem1/BoxPoints/leftTopCalc'
 * '<S44>'  : 'maketKinematic27/flightRegulatorCFB/Subsystem1/BoxPoints/rightBottomCalc'
 * '<S45>'  : 'maketKinematic27/flightRegulatorCFB/Subsystem1/BoxPoints/rightTopCalc'
 * '<S46>'  : 'maketKinematic27/flightRegulatorCFB/regulator/Radians to Degrees'
 * '<S47>'  : 'maketKinematic27/flightRegulatorCFB/regulator/Radians to Degrees1'
 * '<S48>'  : 'maketKinematic27/flightRegulatorCFB/regulator/Radians to Degrees3'
 * '<S49>'  : 'maketKinematic27/flightRegulatorCFB/regulator/Radians to Degrees4'
 * '<S50>'  : 'maketKinematic27/flightRegulatorCFB/regulator/Radians to Degrees5'
 * '<S51>'  : 'maketKinematic27/flightRegulatorCFB/regulator/Radians to Degrees6'
 * '<S52>'  : 'maketKinematic27/flightRegulatorCFB/regulator/Radians to Degrees7'
 * '<S53>'  : 'maketKinematic27/flightRegulatorCFB/regulator/angleRegulator'
 * '<S54>'  : 'maketKinematic27/flightRegulatorCFB/regulator/holdingAngle'
 * '<S55>'  : 'maketKinematic27/flightRegulatorCFB/touchDown/Compare To Constant2'
 * '<S56>'  : 'maketKinematic27/flightRegulatorCFB/velocityAndAccelerationHorizontalBlock/Discrete Derivative'
 * '<S57>'  : 'maketKinematic27/flightRegulatorCFB/velocityAndAccelerationHorizontalBlock/Interval Test'
 * '<S58>'  : 'maketKinematic27/flightRegulatorCFB/velocityAndAccelerationHorizontalBlock/Interval Test1'
 * '<S59>'  : 'maketKinematic27/flightRegulatorCFB/velocityAndAccelerationHorizontalBlock/matFuncDistanceAndAngle'
 * '<S60>'  : 'maketKinematic27/flightRegulatorCFB/velocityAndAccelerationVerticalBlock/Discrete Derivative'
 * '<S61>'  : 'maketKinematic27/flightRegulatorCFB/velocityAndAccelerationVerticalBlock/Interval Test'
 * '<S62>'  : 'maketKinematic27/flightRegulatorCFB/velocityAndAccelerationVerticalBlock/Interval Test1'
 */
#endif                                 /* RTW_HEADER_flightRegulatorCFB_h_ */

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
