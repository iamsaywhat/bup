/*
 * File: flightRegulatorCFB.c
 *
 * Code generated for Simulink model 'flightRegulatorCFB'.
 *
 * Model version                  : 1.2149
 * Simulink Coder version         : 8.14 (R2018a) 06-Feb-2018
 * C/C++ source code generated on : Mon Nov 11 14:39:05 2019
 *
 * Target selection: ert.tlc
 * Embedded hardware selection: ARM Compatible->ARM Cortex
 * Code generation objectives:
 *    1. Execution efficiency
 *    2. RAM efficiency
 * Validation result: Not run
 */

#include "flightRegulatorCFB.h"

/* Named constants for Chart: '<S9>/parameter&#x421;alculation' */
#define CALL_EVENT_g                   (-1)
#define IN_angleUpdate                 ((uint8_T)1U)
#define IN_inCorridor                  ((uint8_T)1U)
#define IN_math                        ((uint8_T)1U)
#define IN_math1                       ((uint8_T)2U)
#define IN_math2                       ((uint8_T)3U)
#define IN_noCorridor                  ((uint8_T)2U)
#define IN_notNullOrderAngle           ((uint8_T)1U)
#define IN_nullOrderAngle              ((uint8_T)2U)
#define IN_update                      ((uint8_T)2U)
#define event_eventChangePoint         (0)
#define event_finishLine               (1)
#define event_flightBox                (2)
#define event_flightLine               (4)
#define event_flightSnake              (5)
#define event_flightTurning            (6)

/* Named constants for Chart: '<S11>/angleRegulator' */
#define IN_resetOrderAngle             ((uint8_T)1U)
#define IN_resetOrderAngle1            ((uint8_T)2U)

/* Named constants for Chart: '<S11>/holdingAngle' */
#define IN_angleCorrection             ((uint8_T)1U)
#define IN_angleCorrection1            ((uint8_T)2U)
#define IN_angleDecision               ((uint8_T)3U)
#define NumBitsPerChar                 8U
#ifndef UCHAR_MAX
#include <limits.h>
#endif

#if ( UCHAR_MAX != (0xFFU) ) || ( SCHAR_MAX != (0x7F) )
#error Code was generated for compiler with different sized uchar/char. \
Consider adjusting Test hardware word size settings on the \
Hardware Implementation pane to match your compiler word sizes as \
defined in limits.h of the compiler. Alternatively, you can \
select the Test hardware is the same as production hardware option and \
select the Enable portable word sizes option on the Code Generation > \
Verification pane for ERT based targets, which will disable the \
preprocessor word size checks.
#endif

#if ( USHRT_MAX != (0xFFFFU) ) || ( SHRT_MAX != (0x7FFF) )
#error Code was generated for compiler with different sized ushort/short. \
Consider adjusting Test hardware word size settings on the \
Hardware Implementation pane to match your compiler word sizes as \
defined in limits.h of the compiler. Alternatively, you can \
select the Test hardware is the same as production hardware option and \
select the Enable portable word sizes option on the Code Generation > \
Verification pane for ERT based targets, which will disable the \
preprocessor word size checks.
#endif

#if ( UINT_MAX != (0xFFFFFFFFU) ) || ( INT_MAX != (0x7FFFFFFF) )
#error Code was generated for compiler with different sized uint/int. \
Consider adjusting Test hardware word size settings on the \
Hardware Implementation pane to match your compiler word sizes as \
defined in limits.h of the compiler. Alternatively, you can \
select the Test hardware is the same as production hardware option and \
select the Enable portable word sizes option on the Code Generation > \
Verification pane for ERT based targets, which will disable the \
preprocessor word size checks.
#endif

#if ( ULONG_MAX != (0xFFFFFFFFU) ) || ( LONG_MAX != (0x7FFFFFFF) )
#error Code was generated for compiler with different sized ulong/long. \
Consider adjusting Test hardware word size settings on the \
Hardware Implementation pane to match your compiler word sizes as \
defined in limits.h of the compiler. Alternatively, you can \
select the Test hardware is the same as production hardware option and \
select the Enable portable word sizes option on the Code Generation > \
Verification pane for ERT based targets, which will disable the \
preprocessor word size checks.
#endif

/* Block signals and states (default storage) */
DW rtDW;

/* External inputs (root inport signals with default storage) */
ExtU rtU;

/* External outputs (root outports fed by signals with default storage) */
ExtY rtY;

/* Real-time model */
RT_MODEL rtM_;
RT_MODEL *const rtM = &rtM_;
extern real_T rt_atan2d_snf(real_T u0, real_T u1);
extern real_T rt_remd_snf(real_T u0, real_T u1);
static void distanceBetween2Points1(const real_T rtu_latLon1[2], real_T
  rtu_latLon2, real_T rtu_latLon2_m, real_T *rty_dist);

/* Forward declaration for local functions */
static void distanceAndAngleBetween2Points(real_T lat1, real_T lon1, real_T lat2,
  real_T lon2, real_T *dist, real_T *azi);
static real_T cutAngle(real_T u);
static void broadcast_flightLine(void);
static void flightBox(void);
static void flightExecuteMode(void);
static void enter_atomic_math1(void);
static real_T distanceFromLine(real_T x0, real_T b_y0, real_T x1, real_T b_y1,
  real_T x2, real_T y2);
static void c1_flightRegulatorCFB(void);
static void decideSide(real_T currA, real_T setA, real_T *side, real_T *cA,
  real_T *sA);
extern real_T rtGetNaN(void);
extern real32_T rtGetNaNF(void);

/*===========*
 * Constants *
 *===========*/
#define RT_PI                          3.14159265358979323846
#define RT_PIF                         3.1415927F
#define RT_LN_10                       2.30258509299404568402
#define RT_LN_10F                      2.3025851F
#define RT_LOG10E                      0.43429448190325182765
#define RT_LOG10EF                     0.43429449F
#define RT_E                           2.7182818284590452354
#define RT_EF                          2.7182817F

/*
 * UNUSED_PARAMETER(x)
 *   Used to specify that a function parameter (argument) is required but not
 *   accessed by the function body.
 */
#ifndef UNUSED_PARAMETER
# if defined(__LCC__)
#   define UNUSED_PARAMETER(x)                                   /* do nothing */
# else

/*
 * This is the semi-ANSI standard way of indicating that an
 * unused function parameter is required.
 */
#   define UNUSED_PARAMETER(x)         (void) (x)
# endif
#endif

extern real_T rtInf;
extern real_T rtMinusInf;
extern real_T rtNaN;
extern real32_T rtInfF;
extern real32_T rtMinusInfF;
extern real32_T rtNaNF;
extern void rt_InitInfAndNaN(size_t realSize);
extern boolean_T rtIsInf(real_T value);
extern boolean_T rtIsInfF(real32_T value);
extern boolean_T rtIsNaN(real_T value);
extern boolean_T rtIsNaNF(real32_T value);
typedef struct {
  struct {
    uint32_T wordH;
    uint32_T wordL;
  } words;
} BigEndianIEEEDouble;

typedef struct {
  struct {
    uint32_T wordL;
    uint32_T wordH;
  } words;
} LittleEndianIEEEDouble;

typedef struct {
  union {
    real32_T wordLreal;
    uint32_T wordLuint;
  } wordL;
} IEEESingle;

real_T rtInf;
real_T rtMinusInf;
real_T rtNaN;
real32_T rtInfF;
real32_T rtMinusInfF;
real32_T rtNaNF;
extern real_T rtGetInf(void);
extern real32_T rtGetInfF(void);
extern real_T rtGetMinusInf(void);
extern real32_T rtGetMinusInfF(void);

/*
 * Initialize rtNaN needed by the generated code.
 * NaN is initialized as non-signaling. Assumes IEEE.
 */
real_T rtGetNaN(void)
{
  size_t bitsPerReal = sizeof(real_T) * (NumBitsPerChar);
  real_T nan = 0.0;
  if (bitsPerReal == 32U) {
    nan = rtGetNaNF();
  } else {
    union {
      LittleEndianIEEEDouble bitVal;
      real_T fltVal;
    } tmpVal;

    tmpVal.bitVal.words.wordH = 0xFFF80000U;
    tmpVal.bitVal.words.wordL = 0x00000000U;
    nan = tmpVal.fltVal;
  }

  return nan;
}

/*
 * Initialize rtNaNF needed by the generated code.
 * NaN is initialized as non-signaling. Assumes IEEE.
 */
real32_T rtGetNaNF(void)
{
  IEEESingle nanF = { { 0 } };

  nanF.wordL.wordLuint = 0xFFC00000U;
  return nanF.wordL.wordLreal;
}

/*
 * Initialize the rtInf, rtMinusInf, and rtNaN needed by the
 * generated code. NaN is initialized as non-signaling. Assumes IEEE.
 */
void rt_InitInfAndNaN(size_t realSize)
{
  (void) (realSize);
  rtNaN = rtGetNaN();
  rtNaNF = rtGetNaNF();
  rtInf = rtGetInf();
  rtInfF = rtGetInfF();
  rtMinusInf = rtGetMinusInf();
  rtMinusInfF = rtGetMinusInfF();
}

/* Test if value is infinite */
boolean_T rtIsInf(real_T value)
{
  return (boolean_T)((value==rtInf || value==rtMinusInf) ? 1U : 0U);
}

/* Test if single-precision value is infinite */
boolean_T rtIsInfF(real32_T value)
{
  return (boolean_T)(((value)==rtInfF || (value)==rtMinusInfF) ? 1U : 0U);
}

/* Test if value is not a number */
boolean_T rtIsNaN(real_T value)
{
  return (boolean_T)((value!=value) ? 1U : 0U);
}

/* Test if single-precision value is not a number */
boolean_T rtIsNaNF(real32_T value)
{
  return (boolean_T)(((value!=value) ? 1U : 0U));
}

/*
 * Initialize rtInf needed by the generated code.
 * Inf is initialized as non-signaling. Assumes IEEE.
 */
real_T rtGetInf(void)
{
  size_t bitsPerReal = sizeof(real_T) * (NumBitsPerChar);
  real_T inf = 0.0;
  if (bitsPerReal == 32U) {
    inf = rtGetInfF();
  } else {
    union {
      LittleEndianIEEEDouble bitVal;
      real_T fltVal;
    } tmpVal;

    tmpVal.bitVal.words.wordH = 0x7FF00000U;
    tmpVal.bitVal.words.wordL = 0x00000000U;
    inf = tmpVal.fltVal;
  }

  return inf;
}

/*
 * Initialize rtInfF needed by the generated code.
 * Inf is initialized as non-signaling. Assumes IEEE.
 */
real32_T rtGetInfF(void)
{
  IEEESingle infF;
  infF.wordL.wordLuint = 0x7F800000U;
  return infF.wordL.wordLreal;
}

/*
 * Initialize rtMinusInf needed by the generated code.
 * Inf is initialized as non-signaling. Assumes IEEE.
 */
real_T rtGetMinusInf(void)
{
  size_t bitsPerReal = sizeof(real_T) * (NumBitsPerChar);
  real_T minf = 0.0;
  if (bitsPerReal == 32U) {
    minf = rtGetMinusInfF();
  } else {
    union {
      LittleEndianIEEEDouble bitVal;
      real_T fltVal;
    } tmpVal;

    tmpVal.bitVal.words.wordH = 0xFFF00000U;
    tmpVal.bitVal.words.wordL = 0x00000000U;
    minf = tmpVal.fltVal;
  }

  return minf;
}

/*
 * Initialize rtMinusInfF needed by the generated code.
 * Inf is initialized as non-signaling. Assumes IEEE.
 */
real32_T rtGetMinusInfF(void)
{
  IEEESingle minfF;
  minfF.wordL.wordLuint = 0xFF800000U;
  return minfF.wordL.wordLreal;
}

/*
 * Output and update for atomic system:
 *    '<S10>/distanceBetween2Points1'
 *    '<S10>/distanceBetween2Points2'
 *    '<S10>/distanceBetween2Points3'
 *    '<S10>/distanceBetween2Points4'
 */
static void distanceBetween2Points1(const real_T rtu_latLon1[2], real_T
  rtu_latLon2, real_T rtu_latLon2_m, real_T *rty_dist)
{
  /* SignalConversion: '<S32>/TmpSignal ConversionAt SFunction Inport2' */
  *rty_dist = acos(cos(rtu_latLon1[0]) * cos(rtu_latLon2) * cos(rtu_latLon2_m -
    rtu_latLon1[1]) + sin(rtu_latLon1[0]) * sin(rtu_latLon2)) * 180.0 /
    3.1415926535897931 * 40000.0 / 360.0 * 1000.0;
}

real_T rt_atan2d_snf(real_T u0, real_T u1)
{
  real_T y;
  int32_T u0_0;
  int32_T u1_0;
  if (rtIsNaN(u0) || rtIsNaN(u1)) {
    y = (rtNaN);
  } else if (rtIsInf(u0) && rtIsInf(u1)) {
    if (u0 > 0.0) {
      u0_0 = 1;
    } else {
      u0_0 = -1;
    }

    if (u1 > 0.0) {
      u1_0 = 1;
    } else {
      u1_0 = -1;
    }

    y = atan2(u0_0, u1_0);
  } else if (u1 == 0.0) {
    if (u0 > 0.0) {
      y = RT_PI / 2.0;
    } else if (u0 < 0.0) {
      y = -(RT_PI / 2.0);
    } else {
      y = 0.0;
    }
  } else {
    y = atan2(u0, u1);
  }

  return y;
}

/* Function for Chart: '<S9>/parameter&#x421;alculation' */
static void distanceAndAngleBetween2Points(real_T lat1, real_T lon1, real_T lat2,
  real_T lon2, real_T *dist, real_T *azi)
{
  *dist = acos(cos(lat1) * cos(lat2) * cos(lon2 - lon1) + sin(lat1) * sin(lat2))
    * 180.0 / 3.1415926535897931 * 40000.0 / 360.0 * 1000.0;
  *azi = rt_atan2d_snf(sin(lon2 - lon1) * cos(lat1), cos(lat1) * sin(lat2) - sin
                       (lat1) * cos(lat2) * cos(lon2 - lon1));
}

real_T rt_remd_snf(real_T u0, real_T u1)
{
  real_T y;
  real_T q;
  if (!((!rtIsNaN(u0)) && (!rtIsInf(u0)) && ((!rtIsNaN(u1)) && (!rtIsInf(u1)))))
  {
    y = (rtNaN);
  } else if ((u1 != 0.0) && (u1 != trunc(u1))) {
    q = fabs(u0 / u1);
    if (fabs(q - floor(q + 0.5)) <= DBL_EPSILON * q) {
      y = 0.0 * u0;
    } else {
      y = fmod(u0, u1);
    }
  } else {
    y = fmod(u0, u1);
  }

  return y;
}

/* Function for Chart: '<S9>/parameter&#x421;alculation' */
static real_T cutAngle(real_T u)
{
  real_T angle;
  angle = rt_remd_snf(u, 6.2831853071795862);
  if (angle < 0.0) {
    angle = 6.2831853071795862 - fabs(angle);
  }

  return angle;
}

/* Function for Chart: '<S9>/parameter&#x421;alculation' */
static void broadcast_flightLine(void)
{
  int32_T b_previousEvent;
  b_previousEvent = rtDW.sfEvent;
  rtDW.sfEvent = event_flightLine;

  /* Chart: '<S9>/parameter&#x421;alculation' */
  c1_flightRegulatorCFB();
  rtDW.sfEvent = b_previousEvent;
}

/* Function for Chart: '<S9>/parameter&#x421;alculation' */
static void flightBox(void)
{
  if (rtDW.sfEvent == event_finishLine) {
    rtDW.is_active_outControl = 0U;
    rtDW.pointsMode = pointsModeType_None;
    rtDW.is_active_points = 0U;

    /* Outport: '<Root>/executeMode' */
    rtY.executeMode = flightExecuteModeModeType_finishLine;
  } else {
    if (rtDW.is_active_points != 0U) {
      switch (rtDW.pointsMode) {
       case pointsModeType_startBox:
        if ((rtDW.nearPoint == 1.0) || (rtDW.nearPoint > 4.0) || (rtDW.nearPoint
             < 1.0)) {
          rtDW.pointsMode = pointsModeType_goToPoint1;
          rtDW.temporalCounter_i3 = 0U;
          rtDW.phi2 = rtDW.boxPoints[0];
          rtDW.la2 = rtDW.boxPoints[4];
        } else if (rtDW.nearPoint == 2.0) {
          rtDW.pointsMode = pointsModeType_goToPoint2;
          rtDW.temporalCounter_i3 = 0U;
          rtDW.phi2 = rtDW.boxPoints[1];
          rtDW.la2 = rtDW.boxPoints[5];
        } else if (rtDW.nearPoint == 3.0) {
          rtDW.pointsMode = pointsModeType_goToPoint3;
          rtDW.temporalCounter_i3 = 0U;
          rtDW.phi2 = rtDW.boxPoints[2];
          rtDW.la2 = rtDW.boxPoints[6];
        } else {
          if (rtDW.nearPoint == 4.0) {
            rtDW.pointsMode = pointsModeType_goToPoint4;
            rtDW.temporalCounter_i3 = 0U;
            rtDW.phi2 = rtDW.boxPoints[3];
            rtDW.la2 = rtDW.boxPoints[7];
          }
        }
        break;

       case pointsModeType_goToPoint1:
        if ((rtDW.sfEvent == CALL_EVENT_g) && (rtDW.temporalCounter_i3 >= 1U)) {
          /* Outport: '<Root>/updateAngle' */
          rtY.updateAngle = 1.0;
          rtDW.pointsMode = pointsModeType_pausePoint1;
        }
        break;

       case pointsModeType_pausePoint1:
        if (fabs(rtDW.outDistance2) < rtDW.minimumDistance) {
          rtDW.pointsMode = pointsModeType_goToPoint2;
          rtDW.temporalCounter_i3 = 0U;
          rtDW.phi2 = rtDW.boxPoints[1];
          rtDW.la2 = rtDW.boxPoints[5];
        }
        break;

       case pointsModeType_goToPoint2:
        if ((rtDW.sfEvent == CALL_EVENT_g) && (rtDW.temporalCounter_i3 >= 1U)) {
          /* Outport: '<Root>/updateAngle' */
          rtY.updateAngle = 1.0;
          rtDW.pointsMode = pointsModeType_pausePoint2;
        }
        break;

       case pointsModeType_pausePoint2:
        if (fabs(rtDW.outDistance2) < rtDW.minimumDistance) {
          rtDW.pointsMode = pointsModeType_goToPoint3;
          rtDW.temporalCounter_i3 = 0U;
          rtDW.phi2 = rtDW.boxPoints[2];
          rtDW.la2 = rtDW.boxPoints[6];
        }
        break;

       case pointsModeType_goToPoint3:
        if ((rtDW.sfEvent == CALL_EVENT_g) && (rtDW.temporalCounter_i3 >= 1U)) {
          /* Outport: '<Root>/updateAngle' */
          rtY.updateAngle = 1.0;
          rtDW.pointsMode = pointsModeType_pausePoint3;
        }
        break;

       case pointsModeType_pausePoint3:
        if (fabs(rtDW.outDistance2) < rtDW.minimumDistance) {
          rtDW.pointsMode = pointsModeType_goToPoint4;
          rtDW.temporalCounter_i3 = 0U;
          rtDW.phi2 = rtDW.boxPoints[3];
          rtDW.la2 = rtDW.boxPoints[7];
        }
        break;

       case pointsModeType_goToPoint4:
        if ((rtDW.sfEvent == CALL_EVENT_g) && (rtDW.temporalCounter_i3 >= 1U)) {
          /* Outport: '<Root>/updateAngle' */
          rtY.updateAngle = 1.0;
          rtDW.pointsMode = pointsModeType_pausePoint4;
        }
        break;

       case pointsModeType_pausePoint4:
        if (fabs(rtDW.outDistance2) < rtDW.minimumDistance) {
          rtDW.pointsMode = pointsModeType_goToPoint1;
          rtDW.temporalCounter_i3 = 0U;
          rtDW.phi2 = rtDW.boxPoints[0];
          rtDW.la2 = rtDW.boxPoints[4];
        }
        break;
      }
    }

    if (rtDW.is_active_outControl != 0U) {
      rtDW.outAngle_h = rtDW.azimut;
    }
  }
}

/* Function for Chart: '<S9>/parameter&#x421;alculation' */
static void flightExecuteMode(void)
{
  boolean_T guard1 = false;
  boolean_T guard2 = false;

  /* Outport: '<Root>/executeMode' incorporates:
   *  Outport: '<Root>/updateAngle'
   */
  guard1 = false;
  guard2 = false;
  switch (rtY.executeMode) {
   case flightExecuteModeModeType_flightStart:
    if ((rtDW.sfEvent == event_flightTurning) || (rtDW.sfEvent ==
         event_flightLine) || (rtDW.sfEvent == event_flightSnake) ||
        (rtDW.sfEvent == event_flightBox)) {
      switch (rtDW.sfEvent) {
       case event_flightLine:
        rtY.executeMode = flightExecuteModeModeType_flightLine;
        break;

       case event_flightTurning:
        rtY.executeMode = flightExecuteModeModeType_flightTurning;
        break;

       case event_flightSnake:
        rtY.executeMode = flightExecuteModeModeType_flightSnake;
        if (!(rtDW.flightSnakeMode == flightSnakeModeType_CalculateParameters))
        {
          rtDW.flightSnakeMode = flightSnakeModeType_CalculateParameters;
        }
        break;

       case event_flightBox:
        rtY.executeMode = flightExecuteModeModeType_flightBox;
        rtDW.is_active_points = 1U;
        if (!(rtDW.pointsMode == pointsModeType_startBox)) {
          rtDW.pointsMode = pointsModeType_startBox;
        }

        if (rtDW.is_active_outControl != 1U) {
          rtDW.is_active_outControl = 1U;
        }
        break;
      }
    }
    break;

   case flightExecuteModeModeType_flightLine:
    if ((rtDW.sfEvent == event_flightBox) || (rtDW.sfEvent == event_flightSnake)
        || (rtDW.sfEvent == event_flightTurning)) {
      if (rtDW.sfEvent == event_flightLine) {
        rtY.executeMode = flightExecuteModeModeType_flightLine;
      } else if (rtDW.sfEvent == event_flightTurning) {
        rtY.executeMode = flightExecuteModeModeType_flightTurning;
      } else if (rtDW.sfEvent == event_flightSnake) {
        rtY.executeMode = flightExecuteModeModeType_flightSnake;
        if (!(rtDW.flightSnakeMode == flightSnakeModeType_CalculateParameters))
        {
          rtDW.flightSnakeMode = flightSnakeModeType_CalculateParameters;
        }
      } else if (rtDW.sfEvent == event_flightBox) {
        rtY.executeMode = flightExecuteModeModeType_flightBox;
        rtDW.is_active_points = 1U;
        if (!(rtDW.pointsMode == pointsModeType_startBox)) {
          rtDW.pointsMode = pointsModeType_startBox;
        }

        if (rtDW.is_active_outControl != 1U) {
          rtDW.is_active_outControl = 1U;
        }
      } else {
        guard1 = true;
      }
    } else {
      guard1 = true;
    }
    break;

   case flightExecuteModeModeType_finishLine:
    rtDW.outAngle_h = rtDW.azimutB;
    rtDW.phi2 = rtDW.phiB;
    rtDW.la2 = rtDW.laB;
    rtY.updateAngle = 1.0;
    break;

   case flightExecuteModeModeType_flightTurning:
    if ((rtDW.sfEvent == event_flightBox) || (rtDW.sfEvent == event_flightLine) ||
        (rtDW.sfEvent == event_flightSnake)) {
      switch (rtDW.sfEvent) {
       case event_flightLine:
        rtY.executeMode = flightExecuteModeModeType_flightLine;
        break;

       case event_flightTurning:
        rtY.executeMode = flightExecuteModeModeType_flightTurning;
        break;

       case event_flightSnake:
        rtY.executeMode = flightExecuteModeModeType_flightSnake;
        if (!(rtDW.flightSnakeMode == flightSnakeModeType_CalculateParameters))
        {
          rtDW.flightSnakeMode = flightSnakeModeType_CalculateParameters;
        }
        break;

       case event_flightBox:
        rtY.executeMode = flightExecuteModeModeType_flightBox;
        rtDW.is_active_points = 1U;
        if (!(rtDW.pointsMode == pointsModeType_startBox)) {
          rtDW.pointsMode = pointsModeType_startBox;
        }

        if (rtDW.is_active_outControl != 1U) {
          rtDW.is_active_outControl = 1U;
        }
        break;
      }
    }
    break;

   case flightExecuteModeModeType_flightSnake:
    if ((rtDW.sfEvent == event_flightBox) || (rtDW.sfEvent == event_flightLine) ||
        (rtDW.sfEvent == event_flightTurning)) {
      rtDW.outAngle_h = rtDW.azimutB;
      rtY.updateAngle = 1.0;
      if (rtDW.sfEvent == event_flightLine) {
        rtDW.flightSnakeMode = flightSnakeModeType_None;
        rtY.executeMode = flightExecuteModeModeType_flightLine;
      } else if (rtDW.sfEvent == event_flightTurning) {
        rtDW.flightSnakeMode = flightSnakeModeType_None;
        rtY.executeMode = flightExecuteModeModeType_flightTurning;
      } else if (rtDW.sfEvent == event_flightSnake) {
        rtDW.flightSnakeMode = flightSnakeModeType_None;
        rtY.executeMode = flightExecuteModeModeType_flightSnake;
        if (!(rtDW.flightSnakeMode == flightSnakeModeType_CalculateParameters))
        {
          rtDW.flightSnakeMode = flightSnakeModeType_CalculateParameters;
        }
      } else if (rtDW.sfEvent == event_flightBox) {
        rtDW.flightSnakeMode = flightSnakeModeType_None;
        rtY.executeMode = flightExecuteModeModeType_flightBox;
        rtDW.is_active_points = 1U;
        if (!(rtDW.pointsMode == pointsModeType_startBox)) {
          rtDW.pointsMode = pointsModeType_startBox;
        }

        if (rtDW.is_active_outControl != 1U) {
          rtDW.is_active_outControl = 1U;
        }
      } else {
        guard2 = true;
      }
    } else {
      guard2 = true;
    }
    break;

   case flightExecuteModeModeType_flightBox:
    flightBox();
    break;
  }

  if (guard2) {
    switch (rtDW.flightSnakeMode) {
     case flightSnakeModeType_CalculateParameters:
      rtDW.flightSnakeMode = flightSnakeModeType_leftShoulder;
      rtDW.temporalCounter_i3 = 0U;
      rtDW.outAngle_h = cutAngle(rtDW.azimutB - rtDW.pAngleSnake);
      break;

     case flightSnakeModeType_leftShoulder:
      if ((rtDW.sfEvent == CALL_EVENT_g) && (rtDW.temporalCounter_i3 >=
           (uint32_T)rtDW.tShoulderSnake)) {
        rtDW.flightSnakeMode = flightSnakeModeType_pauseOrder1;
        rtDW.temporalCounter_i3 = 0U;
        rtY.updateAngle = 1.0;
      }
      break;

     case flightSnakeModeType_pauseOrder1:
      if ((rtDW.sfEvent == CALL_EVENT_g) && (rtDW.temporalCounter_i3 >= 2U)) {
        rtDW.flightSnakeMode = flightSnakeModeType_rightShoulder;
        rtDW.temporalCounter_i3 = 0U;
        rtDW.outAngle_h = cutAngle(rtDW.azimutB + rtDW.pAngleSnake);
      }
      break;

     case flightSnakeModeType_rightShoulder:
      if ((rtDW.sfEvent == CALL_EVENT_g) && (rtDW.temporalCounter_i3 >=
           (uint32_T)rtDW.tShoulderSnake)) {
        rtDW.flightSnakeMode = flightSnakeModeType_pauseOrder;
        rtDW.temporalCounter_i3 = 0U;
        rtY.updateAngle = 1.0;
      }
      break;

     case flightSnakeModeType_pauseOrder:
      if ((rtDW.sfEvent == CALL_EVENT_g) && (rtDW.temporalCounter_i3 >= 2U)) {
        rtDW.flightSnakeMode = flightSnakeModeType_leftShoulder;
        rtDW.temporalCounter_i3 = 0U;
        rtDW.outAngle_h = cutAngle(rtDW.azimutB - rtDW.pAngleSnake);
      }
      break;
    }
  }

  if (guard1) {
    if (rtDW.sfEvent == event_finishLine) {
      rtY.executeMode = flightExecuteModeModeType_finishLine;
    } else {
      rtDW.outAngle_h = rtDW.azimutB;
      rtDW.phi2 = rtDW.phiB;
      rtDW.la2 = rtDW.laB;
    }
  }

  /* End of Outport: '<Root>/executeMode' */
}

/* Function for Chart: '<S9>/parameter&#x421;alculation' */
static void enter_atomic_math1(void)
{
  int32_T b_previousEvent;
  b_previousEvent = rtDW.sfEvent;
  rtDW.sfEvent = event_eventChangePoint;

  /* Chart: '<S9>/parameter&#x421;alculation' */
  c1_flightRegulatorCFB();
  rtDW.sfEvent = b_previousEvent;
}

/* Function for Chart: '<S9>/parameter&#x421;alculation' */
static real_T distanceFromLine(real_T x0, real_T b_y0, real_T x1, real_T b_y1,
  real_T x2, real_T y2)
{
  real_T a_tmp;
  real_T b_a_tmp;
  a_tmp = y2 - b_y1;
  b_a_tmp = x2 - x1;
  return (((a_tmp * x0 - b_a_tmp * b_y0) + x2 * b_y1) - y2 * x1) / sqrt(a_tmp *
    a_tmp + b_a_tmp * b_a_tmp);
}

/* Function for Chart: '<S9>/parameter&#x421;alculation' */
static void c1_flightRegulatorCFB(void)
{
  int32_T b_previousEvent;
  real_T diffAngle;

  /* Chart: '<S9>/parameter&#x421;alculation' incorporates:
   *  Constant: '<S1>/Constant3'
   *  Constant: '<S9>/Constant4'
   *  Inport: '<Root>/angle'
   *  Inport: '<Root>/highStopUPS'
   *  Outport: '<Root>/tD'
   *  Outport: '<Root>/tx'
   *  Outport: '<Root>/tz'
   *  Outport: '<Root>/updateAngle'
   */
  if (rtDW.is_active_c1_flightRegulatorCFB == 0U) {
    rtDW.is_active_c1_flightRegulatorCFB = 1U;
    if (rtDW.is_active_changePoint1 != 1U) {
      rtDW.is_active_changePoint1 = 1U;
    }

    rtDW.flatXC = 0.0;
    rtDW.flatYC = 0.0;
    rtDW.phi1 = rtDW.TmpSignalConversionAtSFunctionI[0];
    rtDW.la1 = rtDW.TmpSignalConversionAtSFunctionI[1];
    rtDW.phiC = rtDW.phi1;
    rtDW.laC = rtDW.la2;
    if (rtDW.is_changePoint1 != IN_math) {
      rtDW.is_changePoint1 = IN_math;
      rtDW.la1 = rtDW.laC;
    }

    if (rtDW.is_active_calculateDistance1 != 1U) {
      rtDW.is_active_calculateDistance1 = 1U;
    }

    rtDW.phiC = rtDW.TmpSignalConversionAtSFunctionI[0];
    rtDW.laC = rtDW.TmpSignalConversionAtSFunctionI[1];
    rtDW.phi1 = rtDW.phiC;
    rtDW.la1 = rtDW.laC;
    rtDW.phiB = rtDW.TmpSignalConversionAtSFunctio_m[0];
    rtDW.laB = rtDW.TmpSignalConversionAtSFunctio_m[1];
    rtDW.flatX1 = 0.0;
    rtDW.flatY1 = 0.0;
    if (rtDW.is_calculateDistance1 != IN_math) {
      rtDW.is_calculateDistance1 = IN_math;
    }

    rtDW.is_active_azimutInCorridor = 1U;
    if (rtDW.is_azimutInCorridor != IN_noCorridor) {
      rtDW.is_azimutInCorridor = IN_noCorridor;
    }

    rtDW.is_active_updateAngle = 1U;
    if (rtDW.is_updateAngle != IN_update) {
      rtDW.is_updateAngle = IN_update;
      rtDW.temporalCounter_i1_f = 0U;
    }

    rtDW.is_active_flightControlMode = 1U;
    if (!(rtDW.flightControlModeMode == flightControlModeModeType_flightTurning))
    {
      rtDW.flightControlModeMode = flightControlModeModeType_flightTurning;
      rtDW.temporalCounter_i4 = 0U;
      broadcast_flightLine();
      if (!(rtDW.flightControlModeMode !=
            flightControlModeModeType_flightTurning)) {
        /* Outport: '<Root>/updateAngle' */
        rtY.updateAngle = 1.0;
      }
    }

    rtDW.is_active_flightExecuteMode = 1U;

    /* Outport: '<Root>/executeMode' */
    rtY.executeMode = flightExecuteModeModeType_flightStart;
    rtDW.is_active_updatePlan = 1U;
    rtDW.phi2 = rtDW.TmpSignalConversionAtSFunctio_m[0];
    rtDW.la2 = rtDW.TmpSignalConversionAtSFunctio_m[1];
    if (rtDW.is_updatePlan != IN_math) {
      rtDW.is_updatePlan = IN_math;
    }

    if (rtDW.is_active_mathBox != 1U) {
      rtDW.is_active_mathBox = 1U;
    }

    if (rtDW.is_mathBox != IN_math) {
      rtDW.is_mathBox = IN_math;
    }

    rtDW.is_active_counterOrderAngle = 1U;
    rtDW.is_counterOrderAngle = IN_nullOrderAngle;
    rtDW.is_active_criteriaDefinition1 = 1U;
    if (!(rtDW.criteriaDefinition1Mode == criteriaDefinition1ModeType_desicion))
    {
      rtDW.criteriaDefinition1Mode = criteriaDefinition1ModeType_desicion;
    }
  } else {
    if (!((rtDW.is_active_changePoint1 == 0U) || (rtDW.is_changePoint1 !=
          IN_math) || (!(rtDW.sfEvent == event_eventChangePoint)))) {
      rtDW.is_changePoint1 = IN_math;
      rtDW.phi1 = rtDW.phiC;
      rtDW.la1 = rtDW.laC;
    }

    if (!((rtDW.is_active_calculateDistance1 == 0U) ||
          (rtDW.is_calculateDistance1 != IN_math))) {
      rtDW.phiC = rtDW.TmpSignalConversionAtSFunctionI[0];
      rtDW.laC = rtDW.TmpSignalConversionAtSFunctionI[1];
      rtDW.phiB = rtDW.TmpSignalConversionAtSFunctio_m[0];
      rtDW.laB = rtDW.TmpSignalConversionAtSFunctio_m[1];
      distanceAndAngleBetween2Points(rtDW.phiC, rtDW.laC, rtDW.phiB, rtDW.laB,
        &rtDW.outDistanceB, &rtDW.azimutB);
      diffAngle = cutAngle(rtU.angle - rtDW.outAngle_h);
      if (fabs(diffAngle) > 3.1415926535897931) {
        diffAngle = 6.2831853071795862 - diffAngle;
      }

      diffAngle = trunc(fabs(diffAngle) * 180.0 / 3.1415926535897931 /
                        rtDW.pAngleVelocity_d);
      rtY.tx = fabs(rtDW.outDistanceB / 20.0) + diffAngle;
      rtY.tz = fabs(((fabs(rtDW.TmpSignalConversionAtSFunctionI[2]) -
                      rtU.highStopUPS) - (8.0 + rtDW.highTurnVelocity) *
                     diffAngle) / 8.0) + diffAngle;
      rtY.tD = rtY.tz / rtY.tx;
      if (rtY.tD > 100.0) {
        rtY.tD = 1.0;
      }
    }

    if (rtDW.is_active_azimutInCorridor != 0U) {
      switch (rtDW.is_azimutInCorridor) {
       case IN_inCorridor:
        if (fabs(rtU.angle - rtDW.azimutB) > rtDW.angleCorridor_d) {
          rtDW.is_azimutInCorridor = IN_noCorridor;
        }
        break;

       case IN_noCorridor:
        if (fabs(rtU.angle - rtDW.azimutB) < rtDW.angleCorridor_d) {
          rtDW.is_azimutInCorridor = IN_inCorridor;
        }
        break;
      }
    }

    if (rtDW.is_active_updateAngle != 0U) {
      switch (rtDW.is_updateAngle) {
       case IN_angleUpdate:
        rtDW.is_updateAngle = IN_update;
        rtDW.temporalCounter_i1_f = 0U;
        break;

       case IN_update:
        if ((rtDW.sfEvent == CALL_EVENT_g) && (rtDW.temporalCounter_i1_f >=
             (uint32_T)rtDW.periodicOrder)) {
          rtDW.is_updateAngle = IN_angleUpdate;
          rtY.updateAngle = 1.0;
        } else if (rtY.tx > 100.0) {
          rtDW.periodicOrder = 60.0;
        } else {
          rtDW.periodicOrder = 15.0;
        }
        break;
      }
    }

    if (rtDW.is_active_flightControlMode != 0U) {
      switch (rtDW.flightControlModeMode) {
       case flightControlModeModeType_flightTurning:
        if ((rtDW.sfEvent == CALL_EVENT_g) && (rtDW.temporalCounter_i4 >= 2)) {
          rtDW.isOffFlightTurning = 1.0;
          rtDW.flightControlModeMode = flightControlModeModeType_flightLine;
          broadcast_flightLine();
        }
        break;

       case flightControlModeModeType_flightLine:
       case flightControlModeModeType_finishLine:
        break;

       case flightControlModeModeType_flightBox:
        if (rtY.tD <= 1.0) {
          rtY.updateAngle = 1.0;
          rtDW.flightControlModeMode = flightControlModeModeType_finishLine;
          b_previousEvent = rtDW.sfEvent;
          rtDW.sfEvent = event_finishLine;
          c1_flightRegulatorCFB();
          rtDW.sfEvent = b_previousEvent;
        }
        break;

       case flightControlModeModeType_flightSnake:
        if (rtDW.outDistanceB < rtDW.distanceStartBox) {
          rtY.updateAngle = 1.0;
          rtDW.flightControlModeMode = flightControlModeModeType_flightBox;
          b_previousEvent = rtDW.sfEvent;
          rtDW.sfEvent = event_flightBox;
          c1_flightRegulatorCFB();
          rtDW.sfEvent = b_previousEvent;
        }
        break;
      }
    }

    if (rtDW.is_active_flightExecuteMode != 0U) {
      flightExecuteMode();
    }

    if (rtDW.is_active_updatePlan != 0U) {
      switch (rtDW.is_updatePlan) {
       case IN_math:
        if (rtDW.changePoint != 0.0) {
          rtDW.is_updatePlan = IN_math1;
          enter_atomic_math1();
        } else if ((rtDW.last_phi2 != rtDW.phi2) || (rtDW.last_la2 != rtDW.la2))
        {
          rtDW.is_updatePlan = IN_math2;
        } else {
          rtDW.phi2 = rtDW.TmpSignalConversionAtSFunctio_m[0];
          rtDW.la2 = rtDW.TmpSignalConversionAtSFunctio_m[1];
        }
        break;

       case IN_math1:
        rtDW.is_updatePlan = IN_math;
        break;

       case IN_math2:
        rtDW.is_updatePlan = IN_math1;
        enter_atomic_math1();
        break;
      }
    }

    if (!((rtDW.is_active_mathBox == 0U) || (rtDW.is_mathBox != IN_math))) {
      rtDW.boxPoints[0] = rtDW.Gain1[0];
      rtDW.boxPoints[4] = rtDW.Gain1[4];
      rtDW.boxPoints[1] = rtDW.Gain1[1];
      rtDW.boxPoints[5] = rtDW.Gain1[5];
      rtDW.boxPoints[2] = rtDW.Gain1[2];
      rtDW.boxPoints[6] = rtDW.Gain1[6];
      rtDW.boxPoints[3] = rtDW.Gain1[3];
      rtDW.boxPoints[7] = rtDW.Gain1[7];
      rtDW.nearPoint = rtDW.DataTypeConversion;
    }

    if (rtDW.is_active_counterOrderAngle != 0U) {
      switch (rtDW.is_counterOrderAngle) {
       case IN_notNullOrderAngle:
        if ((rtDW.sfEvent == CALL_EVENT_g) && (rtDW.temporalCounter_i2 >=
             (uint32_T)rtDW.pTimeOrderAngle)) {
          rtY.updateAngle = 0.0;
          rtDW.is_counterOrderAngle = IN_nullOrderAngle;
        }
        break;

       case IN_nullOrderAngle:
        if (rtY.updateAngle != 0.0) {
          rtDW.is_counterOrderAngle = IN_notNullOrderAngle;
          rtDW.temporalCounter_i2 = 0U;
        }
        break;
      }
    }

    if (!((rtDW.is_active_criteriaDefinition1 == 0U) ||
          (rtDW.criteriaDefinition1Mode != criteriaDefinition1ModeType_desicion)))
    {
      rtDW.outDistanceAB = distanceFromLine(rtDW.flatXC, rtDW.flatYC,
        rtDW.flatX1, rtDW.flatY1, rtDW.flatX2, rtDW.flatY2);
      distanceAndAngleBetween2Points(rtDW.phiC, rtDW.laC, rtDW.phi2, rtDW.la2,
        &rtDW.outDistance2, &rtDW.azimut);
    }
  }

  /* End of Chart: '<S9>/parameter&#x421;alculation' */
}

/* Function for Chart: '<S11>/angleRegulator' */
static void decideSide(real_T currA, real_T setA, real_T *side, real_T *cA,
  real_T *sA)
{
  int32_T compareAnglesMode;
  *cA = 0.0;
  *sA = 0.0;
  compareAnglesMode = 0;
  *side = 0.0;
  if ((currA >= 0.0) && (currA < 1.5707963267948966)) {
    *cA = 1.0;
  } else if ((currA >= 1.5707963267948966) && (currA < 3.1415926535897931)) {
    *cA = 2.0;
  } else if ((currA >= 3.1415926535897931) && (currA < 4.71238898038469)) {
    *cA = 3.0;
  } else {
    if ((currA > 4.71238898038469) && (currA < 6.2831853071795862)) {
      *cA = 4.0;
    }
  }

  if ((setA >= 0.0) && (setA < 1.5707963267948966)) {
    *sA = 1.0;
  } else if ((setA >= 1.5707963267948966) && (setA < 3.1415926535897931)) {
    *sA = 2.0;
  } else if ((setA >= 3.1415926535897931) && (setA < 4.71238898038469)) {
    *sA = 3.0;
  } else {
    if ((setA > 4.71238898038469) && (setA < 6.2831853071795862)) {
      *sA = 4.0;
    }
  }

  if ((*cA == 1.0) && (*sA == 1.0)) {
    compareAnglesMode = 1;
  } else if ((*cA == 1.0) && (*sA == 2.0)) {
    *side = 1.0;
  } else if ((*cA == 1.0) && (*sA == 3.0)) {
    compareAnglesMode = 2;
  } else if ((*cA == 1.0) && (*sA == 4.0)) {
    *side = -1.0;
  } else if ((*cA == 2.0) && (*sA == 1.0)) {
    *side = -1.0;
  } else if ((*cA == 2.0) && (*sA == 2.0)) {
    compareAnglesMode = 1;
  } else if ((*cA == 2.0) && (*sA == 3.0)) {
    *side = 1.0;
  } else if ((*cA == 2.0) && (*sA == 4.0)) {
    compareAnglesMode = 2;
  } else if ((*cA == 3.0) && (*sA == 1.0)) {
    compareAnglesMode = 3;
  } else if ((*cA == 3.0) && (*sA == 2.0)) {
    *side = -1.0;
  } else if ((*cA == 3.0) && (*sA == 3.0)) {
    compareAnglesMode = 1;
  } else if ((*cA == 3.0) && (*sA == 4.0)) {
    *side = 1.0;
  } else if ((*cA == 4.0) && (*sA == 1.0)) {
    *side = 1.0;
  } else if ((*cA == 4.0) && (*sA == 2.0)) {
    compareAnglesMode = 3;
  } else if ((*cA == 4.0) && (*sA == 3.0)) {
    *side = -1.0;
  } else {
    if ((*cA == 4.0) && (*sA == 4.0)) {
      compareAnglesMode = 1;
    }
  }

  switch (compareAnglesMode) {
   case 1:
    if (currA > setA) {
      *side = -1.0;
    } else {
      *side = (currA < setA);
    }
    break;

   case 2:
    if (currA + 3.1415926535897931 < setA) {
      *side = -1.0;
    } else {
      if (currA + 3.1415926535897931 >= setA) {
        *side = 1.0;
      }
    }
    break;

   case 3:
    if (currA - 3.1415926535897931 <= setA) {
      *side = -1.0;
    } else {
      if (currA - 3.1415926535897931 > setA) {
        *side = 1.0;
      }
    }
    break;
  }
}

/* Model step function */
void flightRegulatorCFB_step(void)
{
  real_T rtb_Add;
  real_T rtb_Sum1_jp;
  real_T rtb_Sum;
  real_T rtb_VectorConcatenate[8];
  real_T rtb_Sum_m;
  real_T rtb_Sum_b;
  real_T rtb_Sum_h;
  int32_T yElIdx;
  int32_T uElOffset1;
  int32_T ntIdx1;
  int32_T uElOffset0;
  real_T rtb_Gain1_nh;
  real_T rtb_Gain1_m;
  real_T rtb_Gain1_o[2];
  real_T rtb_PermuteDimensions[8];
  real_T rtb_Sum1_b_tmp_tmp;

  /* Outputs for Atomic SubSystem: '<Root>/flightRegulatorCFB' */
  /* Gain: '<S37>/deg2rad' incorporates:
   *  Gain: '<S28>/Gain1'
   *  Gain: '<S30>/Gain1'
   *  Gain: '<S37>/Gain1'
   *  Gain: '<S37>/Gain6'
   *  Inport: '<Root>/xyzPoints'
   *  Sum: '<S37>/Sum'
   */
  rtb_Sum_h = (0.009 + rtU.xyzPoints[0]) * 0.017453292519943295;

  /* Trigonometry: '<S37>/Trigonometric Function2' incorporates:
   *  Gain: '<S37>/deg2rad'
   *  Trigonometry: '<S39>/Trigonometric Function2'
   */
  rtb_Sum1_jp = cos(rtb_Sum_h);

  /* Sum: '<S37>/Sum1' incorporates:
   *  Gain: '<S22>/minus'
   *  Gain: '<S37>/Gain'
   *  Gain: '<S37>/Gain5'
   *  Inport: '<Root>/xyzPoints'
   *  Product: '<S37>/Divide'
   *  Trigonometry: '<S37>/Trigonometric Function2'
   */
  rtb_Sum = -0.009 / rtb_Sum1_jp + rtU.xyzPoints[1];

  /* SignalConversion: '<S10>/ConcatBufferAtVector ConcatenateIn1' incorporates:
   *  Gain: '<S37>/Gain1'
   *  Gain: '<S37>/Gain6'
   *  Inport: '<Root>/xyzPoints'
   *  Sum: '<S37>/Sum'
   */
  rtb_VectorConcatenate[0] = 0.009 + rtU.xyzPoints[0];
  rtb_VectorConcatenate[1] = rtb_Sum;

  /* Gain: '<S36>/deg2rad' incorporates:
   *  Gain: '<S22>/plus1'
   *  Gain: '<S29>/Gain1'
   *  Gain: '<S31>/Gain1'
   *  Gain: '<S36>/Gain1'
   *  Gain: '<S36>/Gain6'
   *  Inport: '<Root>/xyzPoints'
   *  Sum: '<S36>/Sum'
   */
  rtb_Sum1_b_tmp_tmp = (-0.009 + rtU.xyzPoints[0]) * 0.017453292519943295;

  /* Trigonometry: '<S36>/Trigonometric Function2' incorporates:
   *  Gain: '<S36>/deg2rad'
   *  Trigonometry: '<S38>/Trigonometric Function2'
   */
  rtb_Add = cos(rtb_Sum1_b_tmp_tmp);

  /* Sum: '<S36>/Sum1' incorporates:
   *  Gain: '<S22>/minus1'
   *  Gain: '<S36>/Gain'
   *  Gain: '<S36>/Gain5'
   *  Inport: '<Root>/xyzPoints'
   *  Product: '<S36>/Divide'
   *  Trigonometry: '<S36>/Trigonometric Function2'
   */
  rtb_Sum_m = -0.009 / rtb_Add + rtU.xyzPoints[1];

  /* SignalConversion: '<S10>/ConcatBufferAtVector ConcatenateIn2' incorporates:
   *  Gain: '<S22>/plus1'
   *  Gain: '<S36>/Gain1'
   *  Gain: '<S36>/Gain6'
   *  Inport: '<Root>/xyzPoints'
   *  Sum: '<S36>/Sum'
   */
  rtb_VectorConcatenate[2] = -0.009 + rtU.xyzPoints[0];
  rtb_VectorConcatenate[3] = rtb_Sum_m;

  /* Sum: '<S38>/Sum1' incorporates:
   *  Gain: '<S38>/Gain'
   *  Gain: '<S38>/Gain5'
   *  Inport: '<Root>/xyzPoints'
   *  Product: '<S38>/Divide'
   */
  rtb_Sum_b = 0.009 / rtb_Add + rtU.xyzPoints[1];

  /* SignalConversion: '<S10>/ConcatBufferAtVector ConcatenateIn3' incorporates:
   *  Gain: '<S22>/plus3'
   *  Gain: '<S38>/Gain1'
   *  Gain: '<S38>/Gain6'
   *  Inport: '<Root>/xyzPoints'
   *  Sum: '<S38>/Sum'
   */
  rtb_VectorConcatenate[4] = -0.009 + rtU.xyzPoints[0];
  rtb_VectorConcatenate[5] = rtb_Sum_b;

  /* Sum: '<S39>/Sum1' incorporates:
   *  Gain: '<S39>/Gain'
   *  Gain: '<S39>/Gain5'
   *  Inport: '<Root>/xyzPoints'
   *  Product: '<S39>/Divide'
   */
  rtb_Sum1_jp = 0.009 / rtb_Sum1_jp + rtU.xyzPoints[1];

  /* SignalConversion: '<S10>/ConcatBufferAtVector ConcatenateIn4' incorporates:
   *  Gain: '<S39>/Gain1'
   *  Gain: '<S39>/Gain6'
   *  Inport: '<Root>/xyzPoints'
   *  Sum: '<S39>/Sum'
   */
  rtb_VectorConcatenate[6] = 0.009 + rtU.xyzPoints[0];
  rtb_VectorConcatenate[7] = rtb_Sum1_jp;

  /* PermuteDimensions: '<S10>/Permute Dimensions' */
  yElIdx = 0;
  uElOffset1 = 0;
  for (ntIdx1 = 0; ntIdx1 < 2; ntIdx1++) {
    rtb_PermuteDimensions[yElIdx] = rtb_VectorConcatenate[uElOffset1];
    yElIdx++;
    uElOffset0 = uElOffset1 + 2;
    rtb_PermuteDimensions[yElIdx] = rtb_VectorConcatenate[uElOffset0];
    yElIdx++;
    uElOffset0 += 2;
    rtb_PermuteDimensions[yElIdx] = rtb_VectorConcatenate[uElOffset0];
    yElIdx++;
    uElOffset0 += 2;
    rtb_PermuteDimensions[yElIdx] = rtb_VectorConcatenate[uElOffset0];
    yElIdx++;
    uElOffset1++;
  }

  /* End of PermuteDimensions: '<S10>/Permute Dimensions' */

  /* Gain: '<S2>/Gain1' */
  for (yElIdx = 0; yElIdx < 8; yElIdx++) {
    rtDW.Gain1[yElIdx] = 0.017453292519943295 * rtb_PermuteDimensions[yElIdx];
  }

  /* End of Gain: '<S2>/Gain1' */

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

  /* Gain: '<S3>/Gain1' incorporates:
   *  Inport: '<Root>/XYZi'
   */
  rtb_Gain1_nh = 0.017453292519943295 * rtU.XYZi[0];

  /* Gain: '<S4>/Gain1' incorporates:
   *  Inport: '<Root>/XYZi'
   */
  rtb_Gain1_m = 0.017453292519943295 * rtU.XYZi[1];

  /* Gain: '<S28>/Gain1' */
  rtb_Gain1_o[0] = rtb_Sum_h;
  rtb_Gain1_o[1] = 0.017453292519943295 * rtb_Sum;

  /* MATLAB Function: '<S10>/distanceBetween2Points1' */
  distanceBetween2Points1(rtb_Gain1_o, rtb_Gain1_nh, rtb_Gain1_m, &rtb_Sum);

  /* Gain: '<S29>/Gain1' */
  rtb_Gain1_o[0] = rtb_Sum1_b_tmp_tmp;
  rtb_Gain1_o[1] = 0.017453292519943295 * rtb_Sum_m;

  /* MATLAB Function: '<S10>/distanceBetween2Points2' */
  distanceBetween2Points1(rtb_Gain1_o, rtb_Gain1_nh, rtb_Gain1_m, &rtb_Sum_m);

  /* Gain: '<S31>/Gain1' */
  rtb_Gain1_o[0] = rtb_Sum1_b_tmp_tmp;
  rtb_Gain1_o[1] = 0.017453292519943295 * rtb_Sum_b;

  /* MATLAB Function: '<S10>/distanceBetween2Points3' */
  distanceBetween2Points1(rtb_Gain1_o, rtb_Gain1_nh, rtb_Gain1_m, &rtb_Sum_b);

  /* Gain: '<S30>/Gain1' */
  rtb_Gain1_o[0] = rtb_Sum_h;
  rtb_Gain1_o[1] = 0.017453292519943295 * rtb_Sum1_jp;

  /* MATLAB Function: '<S10>/distanceBetween2Points4' */
  distanceBetween2Points1(rtb_Gain1_o, rtb_Gain1_nh, rtb_Gain1_m, &rtb_Sum1_jp);

  /* MinMax: '<S10>/MinMax' */
  rtb_Sum_h = fmin(fmin(fmin(rtb_Sum, rtb_Sum_m), rtb_Sum_b), rtb_Sum1_jp);

  /* DataTypeConversion: '<S10>/Data Type Conversion' incorporates:
   *  Gain: '<S10>/Gain3'
   *  Gain: '<S10>/Gain8'
   *  Gain: '<S10>/Gain9'
   *  RelationalOperator: '<S10>/Relational Operator'
   *  RelationalOperator: '<S10>/Relational Operator1'
   *  RelationalOperator: '<S10>/Relational Operator2'
   *  RelationalOperator: '<S10>/Relational Operator3'
   *  Sum: '<S10>/Add1'
   */
  rtDW.DataTypeConversion = (real_T)(uint8_T)((((((uint32_T)((rtb_Sum_h ==
    rtb_Sum_m) << 7) << 1) + ((uint32_T)(rtb_Sum_h == rtb_Sum) << 7)) +
    ((uint32_T)(rtb_Sum_h == rtb_Sum_b ? 192 : 0) << 1)) + ((uint32_T)
    ((rtb_Sum_h == rtb_Sum1_jp) << 7) << 2)) >> 2) * 0.03125;

  /* SignalConversion: '<S21>/TmpSignal ConversionAt SFunction Inport2' incorporates:
   *  Chart: '<S9>/parameter&#x421;alculation'
   */
  rtDW.TmpSignalConversionAtSFunctionI[0] = rtb_Gain1_nh;
  rtDW.TmpSignalConversionAtSFunctionI[1] = rtb_Gain1_m;
  rtDW.TmpSignalConversionAtSFunctionI[2] = rtb_Add;

  /* SignalConversion: '<S21>/TmpSignal ConversionAt SFunction Inport3' incorporates:
   *  Chart: '<S9>/parameter&#x421;alculation'
   *  Gain: '<S5>/Gain1'
   *  Gain: '<S6>/Gain1'
   *  Inport: '<Root>/xyzPoints'
   */
  rtDW.TmpSignalConversionAtSFunctio_m[0] = 0.017453292519943295 *
    rtU.xyzPoints[0];
  rtDW.TmpSignalConversionAtSFunctio_m[1] = 0.017453292519943295 *
    rtU.xyzPoints[1];
  rtDW.TmpSignalConversionAtSFunctio_m[2] = rtU.xyzPoints[2];

  /* Chart: '<S9>/parameter&#x421;alculation' */
  rtDW.sfEvent = CALL_EVENT_g;
  if (rtDW.temporalCounter_i1_f < MAX_uint32_T) {
    rtDW.temporalCounter_i1_f++;
  }

  if (rtDW.temporalCounter_i2 < MAX_uint32_T) {
    rtDW.temporalCounter_i2++;
  }

  if (rtDW.temporalCounter_i3 < MAX_uint32_T) {
    rtDW.temporalCounter_i3++;
  }

  if (rtDW.temporalCounter_i4 < 3U) {
    rtDW.temporalCounter_i4++;
  }

  c1_flightRegulatorCFB();

  /* Chart: '<S11>/holdingAngle' incorporates:
   *  Outport: '<Root>/updateAngle'
   */
  if (rtDW.is_active_c6_flightRegulatorCFB == 0U) {
    rtDW.is_active_c6_flightRegulatorCFB = 1U;
    rtDW.outAngle = rtDW.outAngle_h;
    rtDW.is_c6_flightRegulatorCFB = IN_angleCorrection;
    rtDW.outOrder = 0.0;
  } else {
    switch (rtDW.is_c6_flightRegulatorCFB) {
     case IN_angleCorrection:
      rtDW.outOrder = 0.0;
      if (rtY.updateAngle == 1.0) {
        rtDW.is_c6_flightRegulatorCFB = IN_angleDecision;
      }
      break;

     case IN_angleCorrection1:
      rtDW.outOrder = 1.0;
      if (rtY.updateAngle == 0.0) {
        rtDW.is_c6_flightRegulatorCFB = IN_angleCorrection;
        rtDW.outOrder = 0.0;
      }
      break;

     default:
      if ((rtDW.memoryInAngle > rtDW.outAngle_h + rtDW.angleCorridor) ||
          (rtDW.memoryInAngle < rtDW.outAngle_h - rtDW.angleCorridor)) {
        rtDW.memoryInAngle = rtDW.outAngle_h;
        rtDW.outAngle = rtDW.outAngle_h;
        rtDW.is_c6_flightRegulatorCFB = IN_angleCorrection1;
        rtDW.outOrder = 1.0;
      }
      break;
    }
  }

  /* End of Chart: '<S11>/holdingAngle' */

  /* Chart: '<S11>/angleRegulator' incorporates:
   *  Inport: '<Root>/angle'
   */
  if (rtDW.temporalCounter_i1 < MAX_uint32_T) {
    rtDW.temporalCounter_i1++;
  }

  if (rtDW.is_active_c5_flightRegulatorCFB == 0U) {
    rtDW.is_active_c5_flightRegulatorCFB = 1U;

    /* Outport: '<Root>/tightenSling' */
    rtY.tightenSling = rtDW.pTightenSling;
    rtDW.TurnMode = TurnModeType_wait;
    rtDW.orderAngle = 0.0;
    rtDW.is_resetOrder = IN_resetOrderAngle;
  } else {
    if (rtDW.TurnMode == TurnModeType_wait) {
      if (rtDW.orderAngle != 0.0) {
        if (rtDW.orderAngle == 0.0) {
          rtDW.TurnMode = TurnModeType_wait;
        } else {
          rtDW.TurnMode = TurnModeType_turning;
          rtDW.directionOfRotation = rtDW.decisionSide;
        }
      }
    } else {
      if (rtDW.orderAngle == 0.0) {
        rtDW.directionOfRotation = 0.0;
        rtDW.TurnMode = TurnModeType_wait;
      }
    }

    rtDW.outSetAngle = cutAngle(rtDW.outAngle);
    rtb_Gain1_nh = cutAngle(rtU.angle - rtDW.outSetAngle);
    if (fabs(rtb_Gain1_nh) > 3.1415926535897931) {
      rtb_Gain1_nh = 6.2831853071795862 - rtb_Gain1_nh;
    }

    decideSide(rtU.angle, rtDW.outSetAngle, &rtDW.decisionSide, &rtb_Gain1_m,
               &rtb_Sum1_jp);
    if (rtDW.is_resetOrder == IN_resetOrderAngle) {
      if (rtDW.outOrder > 0.0) {
        rtDW.is_resetOrder = IN_resetOrderAngle1;
        rtDW.temporalCounter_i1 = 0U;
        rtDW.orderAngle = 1.0;
        rtDW.timeTurn = fabs(rtb_Gain1_nh) * 180.0 / 3.1415926535897931 /
          rtDW.pAngleVelocity;
        rtDW.timeTurn = floor(rtDW.timeTurn);
      }
    } else {
      if (rtDW.temporalCounter_i1 >= (uint32_T)fabs(rtDW.timeTurn)) {
        rtDW.orderAngle = 0.0;
        rtDW.is_resetOrder = IN_resetOrderAngle;
      }
    }
  }

  /* End of Chart: '<S11>/angleRegulator' */

  /* Switch: '<S1>/Switch1' incorporates:
   *  Constant: '<S50>/Constant'
   *  RelationalOperator: '<S50>/Compare'
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

  /* Outport: '<Root>/modeFlight' */
  rtY.modeFlight = 0.0;

  /* Outport: '<Root>/doingManeuverMode' */
  rtY.doingManeuverMode = 0U;

  /* Outputs for Atomic SubSystem: '<Root>/flightRegulatorCFB' */
  /* Outport: '<Root>/angleAngle ' incorporates:
   *  DataTypeConversion: '<S1>/Data Type Conversion13'
   *  Gain: '<S8>/Gain'
   */
  rtY.angleAngle = (uint16_T)(57.295779513082323 * rtDW.outAngle_h);

  /* Outport: '<Root>/cmdTightenSlings' incorporates:
   *  Constant: '<S1>/Constant1'
   *  DataTypeConversion: '<S1>/Data Type Conversion1'
   */
  rtY.cmdTightenSlings = 0U;

  /* Outport: '<Root>/cmdTouchDown' incorporates:
   *  DataTypeConversion: '<S1>/Data Type Conversion12'
   *  Inport: '<Root>/highStopUPS'
   *  Inport: '<Root>/isVeracityGns'
   *  Logic: '<S12>/Logical Operator'
   *  RelationalOperator: '<S12>/Relational Operator'
   */
  rtY.cmdTouchDown = (uint8_T)((rtb_Add <= rtU.highStopUPS) &&
    (rtU.isVeracityGns != 0.0));

  /* Outport: '<Root>/Period_ms' incorporates:
   *  Gain: '<S1>/Gain2'
   */
  rtY.Period_ms = 1000.0;

  /* End of Outputs for SubSystem: '<Root>/flightRegulatorCFB' */

  /* Outport: '<Root>/txNextPoint' */
  rtY.txNextPoint = 0.0;

  /* Outputs for Atomic SubSystem: '<Root>/flightRegulatorCFB' */
  /* Outport: '<Root>/distanceAB' incorporates:
   *  DataTypeConversion: '<S1>/Data Type Conversion11'
   */
  rtY.distanceAB = (int16_T)floor(rtDW.outDistanceAB);

  /* Outport: '<Root>/distanceB' incorporates:
   *  DataTypeConversion: '<S1>/Data Type Conversion2'
   */
  rtY.distanceB = (uint16_T)rtDW.outDistanceB;

  /* End of Outputs for SubSystem: '<Root>/flightRegulatorCFB' */

  /* Outport: '<Root>/diffAngle' */
  rtY.diffAngle = 0;

  /* Outputs for Atomic SubSystem: '<Root>/flightRegulatorCFB' */
  /* Outport: '<Root>/setAngle' incorporates:
   *  DataTypeConversion: '<S1>/Data Type Conversion5'
   *  Gain: '<S40>/Gain'
   */
  rtY.setAngle = (uint16_T)(57.295779513082323 * rtDW.outSetAngle);

  /* End of Outputs for SubSystem: '<Root>/flightRegulatorCFB' */

  /* Outport: '<Root>/stateAngleCorrection' */
  rtY.stateAngleCorrection = 0U;

  /* Outport: '<Root>/stateAngleDoing' */
  rtY.stateAngleDoing = 0U;

  /* Outputs for Atomic SubSystem: '<Root>/flightRegulatorCFB' */
  /* Outport: '<Root>/stateTurn' incorporates:
   *  DataTypeConversion: '<S1>/Data Type Conversion8'
   */
  rtY.stateTurn = (uint8_T)rtDW.TurnMode;

  /* Outport: '<Root>/changeControl' incorporates:
   *  DataTypeConversion: '<S1>/Data Type Conversion9'
   */
  rtY.changeControl = (uint8_T)rtDW.isOffFlightTurning;

  /* Outport: '<Root>/lat1' incorporates:
   *  Gain: '<S13>/Gain'
   */
  rtY.lat1 = 57.295779513082323 * rtDW.phi1;

  /* Outport: '<Root>/lat2' incorporates:
   *  Gain: '<S14>/Gain'
   */
  rtY.lat2 = 57.295779513082323 * rtDW.phi2;

  /* Outport: '<Root>/lon1' incorporates:
   *  Gain: '<S15>/Gain'
   */
  rtY.lon1 = 57.295779513082323 * rtDW.la1;

  /* Outport: '<Root>/lon2' incorporates:
   *  Gain: '<S16>/Gain'
   */
  rtY.lon2 = 57.295779513082323 * rtDW.la2;

  /* Outport: '<Root>/orderAngle' incorporates:
   *  DataTypeConversion: '<S1>/Data Type Conversion4'
   */
  rtY.orderAngle = (uint8_T)rtDW.orderAngle;

  /* Outport: '<Root>/distance2' incorporates:
   *  DataTypeConversion: '<S1>/Data Type Conversion14'
   */
  rtY.distance2 = (uint16_T)rtDW.outDistance2;

  /* End of Outputs for SubSystem: '<Root>/flightRegulatorCFB' */

  /* Outport: '<Root>/high' */
  rtY.high = rtb_Add;
}

/* Model initialize function */
void flightRegulatorCFB_initialize(void)
{
  /* Registration code */

  /* initialize non-finites */
  rt_InitInfAndNaN(sizeof(real_T));

  /* SystemInitialize for Atomic SubSystem: '<Root>/flightRegulatorCFB' */
  /* SystemInitialize for Chart: '<S9>/parameter&#x421;alculation' */
  rtDW.sfEvent = CALL_EVENT_g;
  rtDW.angleCorridor_d = 0.3490658503988659;
  rtDW.distanceStartBox = 3000.0;
  rtDW.minimumDistance = 300.0;
  rtDW.pTimeOrderAngle = 2.0;
  rtDW.tShoulderSnake = 60.0;
  rtDW.pAngleSnake = 1.0471975511965976;
  rtDW.pAngleVelocity_d = 7.0;
  rtDW.periodicOrder = 30.0;
  rtDW.highTurnVelocity = 4.5;
  rtDW.pointsMode = pointsModeType_None;
  rtDW.criteriaDefinition1Mode = criteriaDefinition1ModeType_None;
  rtDW.flightSnakeMode = flightSnakeModeType_None;

  /* InitializeConditions for Outport: '<Root>/executeMode' incorporates:
   *  Chart: '<S9>/parameter&#x421;alculation'
   */
  rtY.executeMode = flightExecuteModeModeType_None;

  /* SystemInitialize for Chart: '<S9>/parameter&#x421;alculation' */
  rtDW.flightControlModeMode = flightControlModeModeType_None;

  /* SystemInitialize for Chart: '<S11>/holdingAngle' */
  rtDW.angleCorridor = 0.36651914291880922;

  /* SystemInitialize for Chart: '<S11>/angleRegulator' */
  rtDW.pTightenSling = 50.0;
  rtDW.pAngleVelocity = 7.0;
  rtDW.TurnMode = TurnModeType_None;

  /* End of SystemInitialize for SubSystem: '<Root>/flightRegulatorCFB' */
}

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
