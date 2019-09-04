/*
 * File: flightRegulatorCFB.c
 *
 * Code generated for Simulink model 'flightRegulatorCFB'.
 *
 * Model version                  : 1.1519
 * Simulink Coder version         : 8.14 (R2018a) 06-Feb-2018
 * C/C++ source code generated on : Mon Sep  2 14:18:42 2019
 *
 * Target selection: ert.tlc
 * Embedded hardware selection: ARM Compatible->ARM Cortex
 * Code generation objectives:
 *    1. Execution efficiency
 *    2. RAM efficiency
 * Validation result: Not run
 */

#include "flightRegulatorCFB.h"

/* Named constants for Chart: '<S3>/parameter&#x421;alculation' */
#define CALL_EVENT                     (-1)
#define IN_NO_ACTIVE_CHILD             ((uint8_T)0U)
#define IN_angleMath                   ((uint8_T)1U)
#define IN_angleUpdate                 ((uint8_T)1U)
#define IN_checkOrderAngle             ((uint8_T)1U)
#define IN_desicion                    ((uint8_T)2U)
#define IN_desicion1                   ((uint8_T)1U)
#define IN_finishLine                  ((uint8_T)1U)
#define IN_flightBox                   ((uint8_T)2U)
#define IN_flightLine                  ((uint8_T)3U)
#define IN_flightTurning               ((uint8_T)4U)
#define IN_flyingBox                   ((uint8_T)1U)
#define IN_inCorridor                  ((uint8_T)1U)
#define IN_math                        ((uint8_T)1U)
#define IN_math1                       ((uint8_T)2U)
#define IN_math2                       ((uint8_T)3U)
#define IN_math_d                      ((uint8_T)2U)
#define IN_noCorridor                  ((uint8_T)2U)
#define IN_update                      ((uint8_T)2U)
#define IN_wait                        ((uint8_T)2U)
#define event_eventChangePoint         (0)
#define event_finishLine               (1)
#define event_flightAngle              (2)
#define event_flightBox                (3)
#define event_flightLine               (4)
#define event_flightTurning            (5)

/* Named constants for Chart: '<S4>/angleRegulator' */
#define IN_enableEngines1              ((uint8_T)1U)
#define IN_previewTightenSling         ((uint8_T)2U)
#define IN_resetOrderAngle             ((uint8_T)1U)
#define IN_resetOrderAngle1            ((uint8_T)2U)

/* Named constants for Chart: '<S4>/distanceRegulator' */
#define IN_setOrderAngle               ((uint8_T)1U)
#define IN_waitChangeAngle             ((uint8_T)2U)
#define turnAngle                      (0.22689280275926282)

/* Named constants for Chart: '<S4>/holdingAngle' */
#define IN_angleCorrection             ((uint8_T)1U)
#define IN_angleCorrection1            ((uint8_T)2U)
#define NumBitsPerChar                 8U

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

/* Forward declaration for local functions */
static void changePoint1(void);
static void calculateDistance1(void);
static void azimutInCorridor(void);
static void updateAngle(void);
static void enter_atomic_finishLine(void);
static void broadcast_flightLine(void);
static void flightMode1(void);
static void enter_atomic_math1(void);
static real_T distanceFromLine(real_T x0, real_T b_y0, real_T x1, real_T b_y1,
  real_T x2, real_T y2);
static real_T cutAngle(real_T u);
static void c1_flightRegulatorCFB(void);
static void decideSide(real_T currA, real_T setA, real_T *side, real_T *cA,
  real_T *sA);
static void calculateTightenSling(void);
static void calculateTimeTurn(void);
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

/* Function for Chart: '<S3>/parameter&#x421;alculation' */
static void changePoint1(void)
{
  if (!((rtDW.is_changePoint1 != IN_math) || (!(rtDW.sfEvent ==
         event_eventChangePoint)))) {
    rtDW.is_changePoint1 = IN_math;
    rtDW.phi1 = rtDW.phiC;
    rtDW.la1 = rtDW.laC;
  }
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

/* Function for Chart: '<S3>/parameter&#x421;alculation' */
static void calculateDistance1(void)
{
  if (rtDW.is_calculateDistance1 == IN_math) {
    rtDW.phiC = rtDW.TmpSignalConversionAtSFunctionI[0];
    rtDW.laC = rtDW.TmpSignalConversionAtSFunctionI[1];
    rtDW.phiB = rtDW.TmpSignalConversionAtSFunctio_j[0];
    rtDW.laB = rtDW.TmpSignalConversionAtSFunctio_j[1];
    rtDW.outDistanceB = acos(cos(rtDW.phiC) * cos(rtDW.phiB) * cos(rtDW.laB -
      rtDW.laC) + sin(rtDW.phiC) * sin(rtDW.phiB)) * rtDW.radEarth;

    /* Outport: '<Root>/tx' incorporates:
     *  Constant: '<S3>/Constant3'
     */
    rtY.tx = fabs(rtDW.outDistanceB / 20.0);

    /* Outport: '<Root>/tz' incorporates:
     *  Constant: '<S3>/Constant4'
     */
    rtY.tz = fabs((fabs(rtDW.TmpSignalConversionAtSFunctionI[2]) - 300.0) / -8.0);
    rtDW.azimutB = rt_atan2d_snf(sin(rtDW.laB - rtDW.laC) * cos(rtDW.phiC), cos
      (rtDW.phiC) * sin(rtDW.phiB) - sin(rtDW.phiC) * cos(rtDW.phiB) * cos
      (rtDW.laB - rtDW.laC));
  }
}

/* Function for Chart: '<S3>/parameter&#x421;alculation' */
static void azimutInCorridor(void)
{
  switch (rtDW.is_azimutInCorridor) {
   case IN_inCorridor:
    /* Inport: '<Root>/angle' */
    if (fabs(rtU.angle - rtDW.azimutB) > rtDW.angleCorridor) {
      rtDW.is_azimutInCorridor = IN_noCorridor;
    }
    break;

   case IN_noCorridor:
    /* Inport: '<Root>/angle' */
    if (fabs(rtU.angle - rtDW.azimutB) < rtDW.angleCorridor) {
      rtDW.is_azimutInCorridor = IN_inCorridor;
    }
    break;
  }
}

/* Function for Chart: '<S3>/parameter&#x421;alculation' */
static void updateAngle(void)
{
  switch (rtDW.is_updateAngle) {
   case IN_angleUpdate:
    if ((rtDW.sfEvent == CALL_EVENT) && (rtDW.temporalCounter_i1_e >= 2)) {
      if (rtDW.is_updateAngle == IN_angleUpdate) {
        rtDW.orderAngle_g = 0.0;
        rtDW.is_updateAngle = IN_NO_ACTIVE_CHILD;
      }

      if (rtDW.is_updateAngle != IN_update) {
        rtDW.is_updateAngle = IN_update;
        rtDW.temporalCounter_i1_e = 0U;
      }
    }
    break;

   case IN_update:
    if ((rtDW.sfEvent == CALL_EVENT) && (rtDW.temporalCounter_i1_e >= 10)) {
      rtDW.is_updateAngle = IN_angleUpdate;
      rtDW.temporalCounter_i1_e = 0U;
      rtDW.orderAngle_g = 1.0;
    }
    break;
  }
}

/* Function for Chart: '<S3>/parameter&#x421;alculation' */
static void enter_atomic_finishLine(void)
{
  int32_T b_previousEvent;
  b_previousEvent = rtDW.sfEvent;
  rtDW.sfEvent = event_finishLine;

  /* Chart: '<S3>/parameter&#x421;alculation' */
  c1_flightRegulatorCFB();
  rtDW.sfEvent = b_previousEvent;
}

/* Function for Chart: '<S3>/parameter&#x421;alculation' */
static void broadcast_flightLine(void)
{
  int32_T b_previousEvent;
  b_previousEvent = rtDW.sfEvent;
  rtDW.sfEvent = event_flightLine;

  /* Chart: '<S3>/parameter&#x421;alculation' */
  c1_flightRegulatorCFB();
  rtDW.sfEvent = b_previousEvent;
}

/* Function for Chart: '<S3>/parameter&#x421;alculation' */
static void flightMode1(void)
{
  boolean_T guard1 = false;
  guard1 = false;
  switch (rtDW.flightMode1Mode) {
   case flightMode1ModeType_flightStart:
    if ((rtDW.sfEvent == event_flightTurning) || (rtDW.sfEvent ==
         event_flightLine) || (rtDW.sfEvent == event_flightAngle) ||
        (rtDW.sfEvent == event_flightTurning)) {
      switch (rtDW.sfEvent) {
       case event_flightTurning:
        rtDW.flightMode1Mode = flightMode1ModeType_flightTurning;
        break;

       case event_flightBox:
        rtDW.flightMode1Mode = flightMode1ModeType_flightBox;
        rtDW.is_active_points = 1U;
        rtDW.isFlightBox = 1.0;
        if (!(rtDW.pointsMode == pointsModeType_startBox)) {
          rtDW.pointsMode = pointsModeType_startBox;
        }

        if (rtDW.is_active_outControl != 1U) {
          rtDW.is_active_outControl = 1U;
        }
        break;

       case event_flightLine:
        rtDW.flightMode1Mode = flightMode1ModeType_flightLine;
        break;
      }
    }
    break;

   case flightMode1ModeType_flightTurning:
    if ((rtDW.sfEvent == event_flightBox) || (rtDW.sfEvent == event_flightLine) ||
        (rtDW.sfEvent == event_flightAngle)) {
      switch (rtDW.sfEvent) {
       case event_flightTurning:
        rtDW.flightMode1Mode = flightMode1ModeType_flightTurning;
        break;

       case event_flightBox:
        rtDW.flightMode1Mode = flightMode1ModeType_flightBox;
        rtDW.is_active_points = 1U;
        rtDW.isFlightBox = 1.0;
        if (!(rtDW.pointsMode == pointsModeType_startBox)) {
          rtDW.pointsMode = pointsModeType_startBox;
        }

        if (rtDW.is_active_outControl != 1U) {
          rtDW.is_active_outControl = 1U;
        }
        break;

       case event_flightLine:
        rtDW.flightMode1Mode = flightMode1ModeType_flightLine;
        break;
      }
    }
    break;

   case flightMode1ModeType_flightBox:
    if (rtDW.sfEvent == event_finishLine) {
      rtDW.is_active_outControl = 0U;
      rtDW.pointsMode = pointsModeType_None;
      rtDW.is_active_points = 0U;
      rtDW.flightMode1Mode = flightMode1ModeType_finishLine;
      rtDW.orderAngle_g = 1.0;
    } else {
      if (rtDW.is_active_points != 0U) {
        switch (rtDW.pointsMode) {
         case pointsModeType_startBox:
          rtDW.pointsMode = pointsModeType_goToPoint1;
          rtDW.temporalCounter_i2 = 0U;
          rtDW.phi1 = rtDW.phiC;
          rtDW.la1 = rtDW.laC;
          break;

         case pointsModeType_goToPoint1:
          if ((rtDW.sfEvent == CALL_EVENT) && (rtDW.temporalCounter_i2 >=
               (uint32_T)rtDW.ticksPauseBox)) {
            rtDW.pointsMode = pointsModeType_pausePoint1;
            rtDW.orderAngle_g = 1.0;
            rtDW.outDistance2 = 1000.0;
          } else {
            rtDW.phi2 = rtDW.boxPoints[0];
            rtDW.la2 = rtDW.boxPoints[4];
          }
          break;

         case pointsModeType_pausePoint1:
          if (fabs(rtDW.outDistance2) < rtDW.minimumDistance) {
            rtDW.pointsMode = pointsModeType_goToPoint2;
            rtDW.temporalCounter_i2 = 0U;
            rtDW.phi1 = rtDW.phiC;
            rtDW.la1 = rtDW.laC;
          }
          break;

         case pointsModeType_goToPoint2:
          if ((rtDW.sfEvent == CALL_EVENT) && (rtDW.temporalCounter_i2 >=
               (uint32_T)rtDW.ticksPauseBox)) {
            rtDW.pointsMode = pointsModeType_pausePoint2;
            rtDW.orderAngle_g = 1.0;
            rtDW.outDistance2 = 1000.0;
          } else {
            rtDW.phi2 = rtDW.boxPoints[1];
            rtDW.la2 = rtDW.boxPoints[5];
          }
          break;

         case pointsModeType_pausePoint2:
          if (fabs(rtDW.outDistance2) < rtDW.minimumDistance) {
            rtDW.pointsMode = pointsModeType_goToPoint3;
            rtDW.temporalCounter_i2 = 0U;
            rtDW.phi1 = rtDW.phiC;
            rtDW.la1 = rtDW.laC;
          }
          break;

         case pointsModeType_goToPoint3:
          if ((rtDW.sfEvent == CALL_EVENT) && (rtDW.temporalCounter_i2 >=
               (uint32_T)rtDW.ticksPauseBox)) {
            rtDW.pointsMode = pointsModeType_pausePoint3;
            rtDW.orderAngle_g = 1.0;
            rtDW.outDistance2 = 1000.0;
          } else {
            rtDW.phi2 = rtDW.boxPoints[2];
            rtDW.la2 = rtDW.boxPoints[6];
          }
          break;

         case pointsModeType_pausePoint3:
          if (fabs(rtDW.outDistance2) < rtDW.minimumDistance) {
            rtDW.pointsMode = pointsModeType_goToPoint4;
            rtDW.temporalCounter_i2 = 0U;
            rtDW.phi1 = rtDW.phiC;
            rtDW.la1 = rtDW.laC;
          }
          break;

         case pointsModeType_goToPoint4:
          if ((rtDW.sfEvent == CALL_EVENT) && (rtDW.temporalCounter_i2 >=
               (uint32_T)rtDW.ticksPauseBox)) {
            rtDW.pointsMode = pointsModeType_pausePoint4;
            rtDW.orderAngle_g = 1.0;
            rtDW.outDistance2 = 1000.0;
          } else {
            rtDW.phi2 = rtDW.boxPoints[3];
            rtDW.la2 = rtDW.boxPoints[7];
          }
          break;

         case pointsModeType_pausePoint4:
          if (fabs(rtDW.outDistance2) < rtDW.minimumDistance) {
            rtDW.pointsMode = pointsModeType_goToPoint1;
            rtDW.temporalCounter_i2 = 0U;
            rtDW.phi1 = rtDW.phiC;
            rtDW.la1 = rtDW.laC;
          }
          break;
        }
      }

      if (rtDW.is_active_outControl != 0U) {
        rtDW.outAngle_h = rtDW.azimut;
        rtDW.flatX1 = 0.0;
        rtDW.flatY1 = 0.0;
      }
    }
    break;

   case flightMode1ModeType_finishLine:
    rtDW.phi1 = rtDW.phiC;
    rtDW.la1 = rtDW.laC;
    rtDW.outAngle_h = rtDW.azimutB;
    rtDW.phi2 = rtDW.phiB;
    rtDW.la2 = rtDW.laB;
    rtDW.flatX1 = 0.0;
    rtDW.flatY1 = 0.0;
    rtDW.outPrecision = 1.0;
    break;

   case flightMode1ModeType_flightLine:
    if ((rtDW.sfEvent == event_flightBox) || (rtDW.sfEvent == event_flightAngle)
        || (rtDW.sfEvent == event_flightTurning)) {
      if (rtDW.sfEvent == event_flightTurning) {
        rtDW.flightMode1Mode = flightMode1ModeType_flightTurning;
      } else if (rtDW.sfEvent == event_flightBox) {
        rtDW.flightMode1Mode = flightMode1ModeType_flightBox;
        rtDW.is_active_points = 1U;
        rtDW.isFlightBox = 1.0;
        if (!(rtDW.pointsMode == pointsModeType_startBox)) {
          rtDW.pointsMode = pointsModeType_startBox;
        }

        if (rtDW.is_active_outControl != 1U) {
          rtDW.is_active_outControl = 1U;
        }
      } else if (rtDW.sfEvent == event_flightLine) {
        rtDW.flightMode1Mode = flightMode1ModeType_flightLine;
      } else {
        guard1 = true;
      }
    } else {
      guard1 = true;
    }
    break;
  }

  if (guard1) {
    if (rtDW.sfEvent == event_finishLine) {
      rtDW.flightMode1Mode = flightMode1ModeType_finishLine;
      rtDW.orderAngle_g = 1.0;
    } else {
      rtDW.outAngle_h = rtDW.azimutB;
      rtDW.phi2 = rtDW.phiB;
      rtDW.la2 = rtDW.laB;
      rtDW.flatX1 = 0.0;
      rtDW.flatY1 = 0.0;
    }
  }
}

/* Function for Chart: '<S3>/parameter&#x421;alculation' */
static void enter_atomic_math1(void)
{
  int32_T b_previousEvent;
  b_previousEvent = rtDW.sfEvent;
  rtDW.sfEvent = event_eventChangePoint;

  /* Chart: '<S3>/parameter&#x421;alculation' */
  c1_flightRegulatorCFB();
  rtDW.sfEvent = b_previousEvent;
}

/* Function for Chart: '<S3>/parameter&#x421;alculation' */
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

/* Function for Chart: '<S3>/parameter&#x421;alculation' */
static real_T cutAngle(real_T u)
{
  real_T angle;
  angle = rt_remd_snf(u, 6.2831853071795862);
  if (angle < 0.0) {
    angle = 6.2831853071795862 - fabs(angle);
  }

  return angle;
}

/* Function for Chart: '<S3>/parameter&#x421;alculation' */
static void c1_flightRegulatorCFB(void)
{
  int32_T b_previousEvent;
  real_T boxPoints_tmp;
  real_T boxPoints_tmp_0;
  real_T boxPoints_tmp_1;
  boolean_T guard1 = false;

  /* Chart: '<S3>/parameter&#x421;alculation' incorporates:
   *  Inport: '<Root>/angle'
   *  Outport: '<Root>/angleVelocity'
   *  Outport: '<Root>/tx'
   *  Outport: '<Root>/tz'
   */
  if (rtDW.is_active_c1_flightRegulatorCFB == 0U) {
    rtDW.is_active_c1_flightRegulatorCFB = 1U;
    if (rtDW.is_active_changePoint1 != 1U) {
      rtDW.is_active_changePoint1 = 1U;
    }

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
    rtDW.phiB = rtDW.TmpSignalConversionAtSFunctio_j[0];
    rtDW.laB = rtDW.TmpSignalConversionAtSFunctio_j[1];
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
      rtDW.temporalCounter_i1_e = 0U;
    }

    rtDW.is_active_flightMode = 1U;
    if (rtDW.is_flightMode != IN_flightTurning) {
      rtDW.is_flightMode = IN_flightTurning;
      rtDW.temporalCounter_i3 = 0U;
      broadcast_flightLine();
      if (rtDW.is_flightMode == IN_flightTurning) {
        rtDW.orderAngle_g = 1.0;
      }
    }

    rtDW.is_active_criteriaDefinition = 1U;
    if (rtDW.is_criteriaDefinition != IN_desicion) {
      rtDW.is_criteriaDefinition = IN_desicion;
    }

    rtDW.is_active_flightMode1 = 1U;
    rtDW.flightMode1Mode = flightMode1ModeType_flightStart;
    rtDW.is_active_updatePlan = 1U;
    rtDW.phi2 = rtDW.TmpSignalConversionAtSFunctio_j[0];
    rtDW.la2 = rtDW.TmpSignalConversionAtSFunctio_j[1];
    if (rtDW.is_updatePlan != IN_math) {
      rtDW.is_updatePlan = IN_math;
    }

    rtDW.is_active_criteriaDefinition1 = 1U;
    if (!(rtDW.criteriaDefinition1Mode == criteriaDefinition1ModeType_desicion))
    {
      rtDW.criteriaDefinition1Mode = criteriaDefinition1ModeType_desicion;
    }

    if (rtDW.is_active_mathBox != 1U) {
      rtDW.is_active_mathBox = 1U;
    }

    if (rtDW.is_mathBox != IN_math_d) {
      rtDW.is_mathBox = IN_math_d;
    }

    if (rtDW.is_active_DinamicsDefinition != 1U) {
      rtDW.is_active_DinamicsDefinition = 1U;
    }

    if (rtDW.is_DinamicsDefinition != IN_angleMath) {
      rtDW.is_DinamicsDefinition = IN_angleMath;
    }

    rtDW.is_active_mathChangePoint = 1U;
    if (rtDW.is_mathChangePoint != IN_wait) {
      rtDW.is_mathChangePoint = IN_wait;
    }
  } else {
    if (rtDW.is_active_changePoint1 != 0U) {
      changePoint1();
    }

    if (rtDW.is_active_calculateDistance1 != 0U) {
      calculateDistance1();
    }

    if (rtDW.is_active_azimutInCorridor != 0U) {
      azimutInCorridor();
    }

    if (rtDW.is_active_updateAngle != 0U) {
      updateAngle();
    }

    if (rtDW.is_active_flightMode != 0U) {
      switch (rtDW.is_flightMode) {
       case IN_finishLine:
        rtDW.isOffFlightTurning = 0.0;
        rtDW.orderAngle_g = 1.0;
        break;

       case IN_flightBox:
        if (rtDW.tD < rtDW.tdMinFinish) {
          rtDW.orderAngle_g = 1.0;
          rtDW.is_flightMode = IN_finishLine;
          enter_atomic_finishLine();
        } else {
          rtDW.isOffFlightTurning = 1.0;
          rtDW.orderAngle_g = 0.0;
        }
        break;

       case IN_flightLine:
        if ((rtDW.tD > rtDW.tdMinBox) && (rtDW.outDistanceB <
             rtDW.distanceStartBox) && (rtDW.boxAllow != 0.0)) {
          rtDW.is_flightMode = IN_flightBox;
          b_previousEvent = rtDW.sfEvent;
          rtDW.sfEvent = event_flightBox;
          c1_flightRegulatorCFB();
          rtDW.sfEvent = b_previousEvent;
        } else {
          if ((rtDW.outDistanceB < rtDW.distanceStartBox) && (!(rtDW.boxAllow !=
                0.0))) {
            rtDW.is_flightMode = IN_finishLine;
            enter_atomic_finishLine();
          }
        }
        break;

       case IN_flightTurning:
        if ((rtDW.sfEvent == CALL_EVENT) && (rtDW.temporalCounter_i3 >= 2)) {
          rtDW.isOffFlightTurning = 1.0;
          rtDW.is_flightMode = IN_flightLine;
          broadcast_flightLine();
        }
        break;
      }
    }

    if (rtDW.is_active_criteriaDefinition != 0U) {
      guard1 = false;
      switch (rtDW.is_criteriaDefinition) {
       case IN_checkOrderAngle:
        if ((rtDW.sfEvent == CALL_EVENT) && (rtDW.temporalCounter_i4 >= 1)) {
          rtDW.orderAngle_g = 0.0;
          rtDW.is_criteriaDefinition = IN_desicion;
        }
        break;

       case IN_desicion:
        if (rtDW.orderAngle_g > 0.0) {
          guard1 = true;
        } else {
          if (rtDW.updatePoint != 0.0) {
            rtDW.orderAngle_g = 1.0;
            guard1 = true;
          }
        }
        break;
      }

      if (guard1) {
        rtDW.is_criteriaDefinition = IN_checkOrderAngle;
        rtDW.temporalCounter_i4 = 0U;
      }
    }

    if (rtDW.is_active_flightMode1 != 0U) {
      flightMode1();
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
          rtDW.phi2 = rtDW.TmpSignalConversionAtSFunctio_j[0];
          rtDW.la2 = rtDW.TmpSignalConversionAtSFunctio_j[1];
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

    if (!((rtDW.is_active_criteriaDefinition1 == 0U) ||
          (rtDW.criteriaDefinition1Mode != criteriaDefinition1ModeType_desicion)))
    {
      rtDW.outDistanceAB = distanceFromLine((rtDW.phiC - rtDW.phi1) *
        rtDW.radEarth, (rtDW.laC - rtDW.la1) * rtDW.radEarth, rtDW.flatX1,
        rtDW.flatY1, (rtDW.phi2 - rtDW.phi1) * rtDW.radEarth, (rtDW.la2 -
        rtDW.la1) * rtDW.radEarth);
      rtDW.outDistance2 = acos(cos(rtDW.phiC) * cos(rtDW.phi2) * cos(rtDW.la2 -
        rtDW.laC) + sin(rtDW.phiC) * sin(rtDW.phi2)) * rtDW.radEarth;
      rtDW.azimut = rt_atan2d_snf(sin(rtDW.la2 - rtDW.laC) * cos(rtDW.phiC), cos
        (rtDW.phiC) * sin(rtDW.phi2) - sin(rtDW.phiC) * cos(rtDW.phi2) * cos
        (rtDW.la2 - rtDW.laC));
      rtDW.tD = rtY.tz / rtY.tx;
    }

    if ((rtDW.is_active_mathBox != 0U) && (rtDW.is_mathBox == IN_math_d)) {
      if ((rtDW.isFlightBox != 0.0) && (rtDW.isCalcBox != 0.0)) {
        rtDW.is_mathBox = IN_flyingBox;
      } else {
        boxPoints_tmp = rtDW.sideBox / rtDW.radEarth;
        boxPoints_tmp_0 = rtDW.phiB - boxPoints_tmp;
        rtDW.boxPoints[0] = boxPoints_tmp_0;
        boxPoints_tmp_1 = rtDW.laB - boxPoints_tmp;
        rtDW.boxPoints[4] = boxPoints_tmp_1;
        rtDW.boxPoints[1] = boxPoints_tmp_0;
        boxPoints_tmp_0 = boxPoints_tmp + rtDW.laB;
        rtDW.boxPoints[5] = boxPoints_tmp_0;
        boxPoints_tmp += rtDW.phiB;
        rtDW.boxPoints[2] = boxPoints_tmp;
        rtDW.boxPoints[6] = boxPoints_tmp_0;
        rtDW.boxPoints[3] = boxPoints_tmp;
        rtDW.boxPoints[7] = boxPoints_tmp_1;
        rtDW.isCalcBox = 1.0;
      }
    }

    if (!((rtDW.is_active_DinamicsDefinition == 0U) ||
          (rtDW.is_DinamicsDefinition != IN_angleMath))) {
      rtDW.angleVelocity = fabs(cutAngle(rtU.angle - rtDW.angleLast)) /
        rtDW.TdReg1;
      rtY.angleVelocity = rtDW.angleVelocity * 180.0 / 3.1415926535897931;
      rtDW.angleLast = rtU.angle;
    }

    if (rtDW.is_active_mathChangePoint != 0U) {
      switch (rtDW.is_mathChangePoint) {
       case IN_desicion1:
        if ((rtDW.sfEvent == CALL_EVENT) && (rtDW.temporalCounter_i5 >= 1)) {
          rtDW.changePoint = 0.0;
          rtDW.orderAngle_g = 0.0;
          rtDW.is_mathChangePoint = IN_wait;
        }
        break;

       case IN_wait:
        if ((fabs(rtDW.outDistanceAB) > rtDW.overDistanceAB) &&
            (rtDW.isFlightBox == 0.0)) {
          rtDW.orderAngle_g = 1.0;
          rtDW.is_mathChangePoint = IN_desicion1;
          rtDW.temporalCounter_i5 = 0U;
          rtDW.changePoint = 1.0;
        }
        break;
      }
    }
  }

  /* End of Chart: '<S3>/parameter&#x421;alculation' */
}

/* Function for Chart: '<S4>/angleRegulator' */
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

/* Function for Chart: '<S4>/angleRegulator' */
static void calculateTightenSling(void)
{
  if (rtDW.diffAngleDegAbs <= 180.0) {
    if (rtDW.diffAngleDegAbs <= 50.0) {
      if (rtDW.diffAngleDegAbs > 40.0) {
        /* Outport: '<Root>/tightenSling' */
        rtY.tightenSling = 40.0;
      } else if (rtDW.diffAngleDegAbs <= 40.0) {
        if (rtDW.diffAngleDegAbs <= 20.0) {
          /* Outport: '<Root>/tightenSling' */
          rtY.tightenSling = 10.0;
        } else if (rtDW.diffAngleDegAbs > 20.0) {
          /* Outport: '<Root>/tightenSling' */
          rtY.tightenSling = 20.0;
        } else {
          /* Outport: '<Root>/tightenSling' */
          rtY.tightenSling = 50.0;
        }
      } else {
        /* Outport: '<Root>/tightenSling' */
        rtY.tightenSling = 50.0;
      }
    } else {
      /* Outport: '<Root>/tightenSling' */
      rtY.tightenSling = 50.0;
    }
  } else {
    /* Outport: '<Root>/tightenSling' */
    rtY.tightenSling = 50.0;
  }
}

/* Function for Chart: '<S4>/angleRegulator' */
static void calculateTimeTurn(void)
{
  boolean_T guard1 = false;
  boolean_T guard2 = false;
  boolean_T guard3 = false;
  boolean_T guard4 = false;
  boolean_T guard5 = false;
  boolean_T guard6 = false;
  boolean_T guard7 = false;
  boolean_T guard8 = false;

  /* Outport: '<Root>/tightenSling' */
  if (rtY.tightenSling <= 100.0) {
    guard1 = false;
    guard2 = false;
    guard3 = false;
    guard4 = false;
    guard5 = false;
    guard6 = false;
    guard7 = false;
    guard8 = false;
    if (rtY.tightenSling <= 90.0) {
      if (rtY.tightenSling <= 80.0) {
        if (rtY.tightenSling <= 70.0) {
          if (rtY.tightenSling <= 60.0) {
            if (rtY.tightenSling <= 50.0) {
              if (rtY.tightenSling <= 40.0) {
                if (rtY.tightenSling <= 30.0) {
                  if (rtY.tightenSling <= 20.0) {
                    if (rtY.tightenSling <= 10.0) {
                      rtDW.timeWaitEngines = rtDW.timeWaitOfEngines[0];
                    } else if (rtY.tightenSling > 10.0) {
                      rtDW.timeWaitEngines = rtDW.timeWaitOfEngines[1];
                    } else {
                      guard8 = true;
                    }
                  } else {
                    guard8 = true;
                  }
                } else {
                  guard7 = true;
                }
              } else {
                guard6 = true;
              }
            } else {
              guard5 = true;
            }
          } else {
            guard4 = true;
          }
        } else {
          guard3 = true;
        }
      } else {
        guard2 = true;
      }
    } else {
      guard1 = true;
    }

    if (guard8) {
      if (rtY.tightenSling > 20.0) {
        rtDW.timeWaitEngines = rtDW.timeWaitOfEngines[2];
      } else {
        guard7 = true;
      }
    }

    if (guard7) {
      if (rtY.tightenSling > 30.0) {
        rtDW.timeWaitEngines = rtDW.timeWaitOfEngines[3];
      } else {
        guard6 = true;
      }
    }

    if (guard6) {
      if (rtY.tightenSling > 40.0) {
        rtDW.timeWaitEngines = rtDW.timeWaitOfEngines[4];
      } else {
        guard5 = true;
      }
    }

    if (guard5) {
      if (rtY.tightenSling > 50.0) {
        rtDW.timeWaitEngines = rtDW.timeWaitOfEngines[5];
      } else {
        guard4 = true;
      }
    }

    if (guard4) {
      if (rtY.tightenSling > 60.0) {
        rtDW.timeWaitEngines = rtDW.timeWaitOfEngines[6];
      } else {
        guard3 = true;
      }
    }

    if (guard3) {
      if (rtY.tightenSling > 70.0) {
        rtDW.timeWaitEngines = rtDW.timeWaitOfEngines[7];
      } else {
        guard2 = true;
      }
    }

    if (guard2) {
      if (rtY.tightenSling > 80.0) {
        rtDW.timeWaitEngines = rtDW.timeWaitOfEngines[8];
      } else {
        guard1 = true;
      }
    }

    if (guard1) {
      if (rtY.tightenSling > 90.0) {
        rtDW.timeWaitEngines = rtDW.timeWaitOfEngines[9];
      }
    }
  }

  /* End of Outport: '<Root>/tightenSling' */
}

/* Model step function */
void flightRegulatorCFB_step(void)
{
  real_T ncA;
  real_T nsA;
  int16_T rtb_DataTypeConversion11;
  real_T rtb_Add;
  real_T rtb_Switch2;
  int16_T rtb_DataTypeConversion11_0;
  int32_T tmp;
  boolean_T guard1 = false;

  /* Outputs for Atomic SubSystem: '<Root>/flightRegulatorCFB' */
  /* Switch: '<S1>/Switch' incorporates:
   *  Constant: '<S1>/Constant4'
   *  Inport: '<Root>/HighDigitalTerrain'
   *  Inport: '<Root>/isAvailableDigitalTerrain'
   */
  if (rtU.isAvailableDigitalTerrain > 0.0) {
    ncA = rtU.HighDigitalTerrain;
  } else {
    ncA = 0.0;
  }

  /* End of Switch: '<S1>/Switch' */

  /* Sum: '<S1>/Add' incorporates:
   *  Inport: '<Root>/XYZi'
   */
  rtb_Add = rtU.XYZi[2] - ncA;

  /* SignalConversion: '<S6>/TmpSignal ConversionAt SFunction Inport1' incorporates:
   *  Chart: '<S3>/parameter&#x421;alculation'
   *  Gain: '<S1>/Gain5'
   *  Gain: '<S1>/Gain6'
   *  Inport: '<Root>/XYZi'
   */
  rtDW.TmpSignalConversionAtSFunctionI[0] = 0.017453292519943295 * rtU.XYZi[0];
  rtDW.TmpSignalConversionAtSFunctionI[1] = 0.017453292519943295 * rtU.XYZi[1];
  rtDW.TmpSignalConversionAtSFunctionI[2] = rtb_Add;

  /* SignalConversion: '<S6>/TmpSignal ConversionAt SFunction Inport2' incorporates:
   *  Chart: '<S3>/parameter&#x421;alculation'
   *  Gain: '<S1>/Gain1'
   *  Gain: '<S1>/Gain7'
   *  Inport: '<Root>/xyzPoints'
   */
  rtDW.TmpSignalConversionAtSFunctio_j[0] = 0.017453292519943295 *
    rtU.xyzPoints[0];
  rtDW.TmpSignalConversionAtSFunctio_j[1] = 0.017453292519943295 *
    rtU.xyzPoints[1];
  rtDW.TmpSignalConversionAtSFunctio_j[2] = rtU.xyzPoints[2];

  /* Chart: '<S3>/parameter&#x421;alculation' */
  rtDW.sfEvent = CALL_EVENT;
  if (rtDW.temporalCounter_i1_e < 15U) {
    rtDW.temporalCounter_i1_e++;
  }

  if (rtDW.temporalCounter_i2 < MAX_uint32_T) {
    rtDW.temporalCounter_i2++;
  }

  if (rtDW.temporalCounter_i3 < 3U) {
    rtDW.temporalCounter_i3++;
  }

  if (rtDW.temporalCounter_i4 < 3U) {
    rtDW.temporalCounter_i4++;
  }

  if (rtDW.temporalCounter_i5 < 3U) {
    rtDW.temporalCounter_i5++;
  }

  c1_flightRegulatorCFB();

  /* DataTypeConversion: '<S1>/Data Type Conversion11' */
  rtb_DataTypeConversion11 = (int16_T)floor(rtDW.outDistanceAB);

  /* Chart: '<S4>/holdingAngle' */
  if (rtDW.is_active_c6_flightRegulatorCFB == 0U) {
    rtDW.is_active_c6_flightRegulatorCFB = 1U;
    rtDW.outAngle = rtDW.outAngle_h;
    rtDW.is_c6_flightRegulatorCFB = IN_angleCorrection;
  } else if (rtDW.is_c6_flightRegulatorCFB == IN_angleCorrection) {
    if (rtDW.orderAngle_g == 1.0) {
      rtDW.outAngle = rtDW.outAngle_h;
      rtDW.is_c6_flightRegulatorCFB = IN_angleCorrection1;
    }
  } else {
    if (rtDW.orderAngle_g == 0.0) {
      rtDW.is_c6_flightRegulatorCFB = IN_angleCorrection;
    }
  }

  /* End of Chart: '<S4>/holdingAngle' */

  /* Chart: '<S4>/distanceRegulator' */
  if (rtDW.temporalCounter_i1_f < 7U) {
    rtDW.temporalCounter_i1_f++;
  }

  if (rtDW.temporalCounter_i2_b < 7U) {
    rtDW.temporalCounter_i2_b++;
  }

  if (rtDW.is_active_c4_flightRegulatorCFB == 0U) {
    rtDW.is_active_c4_flightRegulatorCFB = 1U;
    rtDW.outAngle_f = rtDW.outAngle;
    rtDW.angleDoingMode = angleDoingModeType_angleWaitCorrection;
    rtDW.orderAngle = 0.0;
    rtDW.localOutAngle = rtDW.outAngle_f;
    rtDW.is_controlAngle = IN_waitChangeAngle;
  } else {
    switch (rtDW.angleDoingMode) {
     case angleDoingModeType_angleWaitCorrection:
      if (rtb_DataTypeConversion11 < 0) {
        tmp = -rtb_DataTypeConversion11;
        if (tmp > 32767) {
          tmp = 32767;
        }

        rtb_DataTypeConversion11_0 = (int16_T)tmp;
      } else {
        rtb_DataTypeConversion11_0 = rtb_DataTypeConversion11;
      }

      /* Switch: '<S3>/Switch1' incorporates:
       *  Constant: '<S3>/anglePos2'
       *  Constant: '<S3>/anglePos5'
       */
      if (rtDW.outPrecision != 0.0) {
        tmp = 50;
      } else {
        tmp = 200;
      }

      /* End of Switch: '<S3>/Switch1' */
      if (rtb_DataTypeConversion11_0 > tmp) {
        rtDW.orderAngle = 1.0;
        rtDW.angleDoingMode = angleDoingModeType_angleCorrection;
        rtDW.angleCorrectionMode = angleCorrectionModeType_startAngleCorrection;
      }
      break;

     case angleDoingModeType_angleCorrection:
      if (rtb_DataTypeConversion11 < 0) {
        tmp = -rtb_DataTypeConversion11;
        if (tmp > 32767) {
          tmp = 32767;
        }

        rtb_DataTypeConversion11_0 = (int16_T)tmp;
      } else {
        rtb_DataTypeConversion11_0 = rtb_DataTypeConversion11;
      }

      /* Switch: '<S3>/Switch2' incorporates:
       *  Constant: '<S3>/anglePos3'
       *  Constant: '<S3>/anglePos6'
       */
      if (rtDW.outPrecision != 0.0) {
        ncA = 50.0;
      } else {
        ncA = 100.0;
      }

      /* End of Switch: '<S3>/Switch2' */
      if (rtb_DataTypeConversion11_0 < (int32_T)ncA) {
        rtDW.angleCorrectionMode = angleCorrectionModeType_None;
        rtDW.angleDoingMode = angleDoingModeType_waitOrderAngle;
        rtDW.temporalCounter_i1_f = 0U;
        rtDW.orderAngle = 1.0;
        rtDW.angleAdded = 0.0;
      } else {
        switch (rtDW.angleCorrectionMode) {
         case angleCorrectionModeType_startAngleCorrection:
          if (rtb_DataTypeConversion11 < 0) {
            rtDW.angleCorrectionMode = angleCorrectionModeType_rightAngle;
            rtDW.angleAdded = -0.22689280275926282;
          } else {
            if (rtb_DataTypeConversion11 > 0) {
              rtDW.angleCorrectionMode = angleCorrectionModeType_leftAngle;
              rtDW.angleAdded = turnAngle;
            }
          }
          break;

         case angleCorrectionModeType_rightAngle:
          rtDW.angleCorrectionMode = angleCorrectionModeType_waitZeroDistance;
          break;

         case angleCorrectionModeType_waitZeroDistance:
          if (rtb_DataTypeConversion11 < 0) {
            rtDW.angleCorrectionMode = angleCorrectionModeType_rightAngle;
            rtDW.angleAdded = -0.22689280275926282;
          } else if (rtb_DataTypeConversion11 > 0) {
            rtDW.angleCorrectionMode = angleCorrectionModeType_leftAngle;
            rtDW.angleAdded = turnAngle;
          } else {
            rtDW.angleAdded = 0.0;
            rtDW.orderAngle = 1.0;
            rtDW.angleCorrectionMode =
              angleCorrectionModeType_startAngleCorrection;
          }
          break;

         default:
          rtDW.angleCorrectionMode = angleCorrectionModeType_waitZeroDistance;
          break;
        }
      }
      break;

     default:
      if (rtDW.temporalCounter_i1_f >= 3) {
        rtDW.angleDoingMode = angleDoingModeType_angleWaitCorrection;
        rtDW.orderAngle = 0.0;
      }
      break;
    }

    if (rtDW.is_controlAngle == IN_setOrderAngle) {
      if (rtDW.temporalCounter_i2_b >= 3) {
        rtDW.orderAngle = 0.0;
        rtDW.is_controlAngle = IN_waitChangeAngle;
      }
    } else {
      if (rtDW.localOutAngle != rtDW.outAngle_f) {
        rtDW.is_controlAngle = IN_setOrderAngle;
        rtDW.temporalCounter_i2_b = 0U;
        rtDW.orderAngle = 1.0;
        rtDW.localOutAngle = rtDW.outAngle_f;
      }
    }

    rtDW.outAngle_f = rtDW.outAngle + rtDW.angleAdded;
  }

  /* End of Chart: '<S4>/distanceRegulator' */

  /* Switch: '<S3>/Switch' incorporates:
   *  Constant: '<S3>/anglePos1'
   *  Constant: '<S3>/anglePos4'
   */
  if (rtDW.outPrecision != 0.0) {
    tmp = 20;
  } else {
    tmp = 30;
  }

  /* End of Switch: '<S3>/Switch' */

  /* Gain: '<S3>/Gain3' */
  rtb_Switch2 = 0.017453292519943295 * (real_T)tmp;

  /* Chart: '<S4>/angleRegulator' incorporates:
   *  Inport: '<Root>/angle'
   *  Outport: '<Root>/directionOfRotation'
   *  Outport: '<Root>/tightenSling'
   */
  if (rtDW.temporalCounter_i1 < MAX_uint32_T) {
    rtDW.temporalCounter_i1++;
  }

  if (rtDW.is_active_c5_flightRegulatorCFB == 0U) {
    rtDW.is_active_c5_flightRegulatorCFB = 1U;

    /* Outport: '<Root>/tightenSling' */
    rtY.tightenSling = rtDW.pTightenSling;
    rtDW.enableEngines = 1.0;
    rtDW.timeWaitEngines = 1.0;
    rtDW.TurnMode = TurnModeType_wait;
    rtDW.waitingEnginesMode = waitingEnginesModeType_enableEngines;
    rtDW.orderAngle_a = 0.0;
    rtDW.is_resetOrder = IN_resetOrderAngle;

    /* Outport: '<Root>/directionOfRotation' */
    rtY.directionOfRotation = 0.0;
    rtDW.is_calculationTimeTurn = IN_previewTightenSling;
  } else {
    if (rtDW.TurnMode == TurnModeType_wait) {
      if (rtDW.enableEngines == 1.0) {
        guard1 = false;
        if (rtDW.orderAngle_a > 0.0) {
          ncA = fabs(rtDW.diffAngle);
          if (ncA > rtb_Switch2) {
            rtDW.TurnMode = TurnModeType_turning;
          } else if (ncA <= rtb_Switch2) {
            rtDW.TurnMode = TurnModeType_wait;
          } else {
            guard1 = true;
          }
        } else {
          guard1 = true;
        }

        if (guard1) {
          if (rtDW.orderAngle_a == 0.0) {
            rtDW.TurnMode = TurnModeType_wait;
          }
        }
      }
    } else if (rtDW.orderAngle_a == 0.0) {
      /* Outport: '<Root>/directionOfRotation' */
      rtY.directionOfRotation = 0.0;
      rtDW.TurnMode = TurnModeType_wait;
    } else {
      /* Outport: '<Root>/directionOfRotation' */
      rtY.directionOfRotation = rtDW.decisionSide;
    }

    /* Switch: '<S4>/Switch' */
    if (rtDW.isOffFlightTurning > 0.0) {
      ncA = rtDW.outAngle_f;
    } else {
      ncA = rtDW.outAngle;
    }

    /* End of Switch: '<S4>/Switch' */
    rtDW.outSetAngle = cutAngle(ncA);
    rtDW.diffAngle = rtU.angle - rtDW.outSetAngle;
    rtDW.diffAngleDegAbs = fabs(rtDW.diffAngle * 180.0 / 3.1415926535897931);
    decideSide(rtU.angle, rtDW.outSetAngle, &rtDW.decisionSide, &ncA, &nsA);
    calculateTightenSling();
    switch (rtDW.waitingEnginesMode) {
     case waitingEnginesModeType_enableEngines:
      if (rtY.directionOfRotation != rtDW.lastDirectionOfRotation) {
        rtDW.waitingEnginesMode = waitingEnginesModeType_disableEngines;
        rtDW.temporalCounter_i1 = 0U;
        rtDW.enableEngines = 0.0;
        rtDW.lastDirectionOfRotation = rtY.directionOfRotation;
      } else {
        if (rtDW.lastTightenSling != rtY.tightenSling) {
          rtDW.waitingEnginesMode = waitingEnginesModeType_disableEngines1;
          rtDW.temporalCounter_i1 = 0U;
          rtDW.enableEngines = 0.0;
          rtDW.lastTightenSling = rtY.tightenSling;
        }
      }
      break;

     case waitingEnginesModeType_disableEngines:
      if (rtDW.temporalCounter_i1 >= (uint32_T)rtDW.timeWaitEngines) {
        rtDW.enableEngines = 1.0;
        rtDW.waitingEnginesMode = waitingEnginesModeType_enableEngines;
      }
      break;

     default:
      if (rtDW.temporalCounter_i1 >= (uint32_T)rtDW.timeWaitEngines) {
        rtDW.enableEngines = 1.0;
        rtDW.waitingEnginesMode = waitingEnginesModeType_enableEngines;
      }
      break;
    }

    if (rtDW.is_resetOrder == IN_resetOrderAngle) {
      if (rtDW.orderAngle > 0.0) {
        rtDW.is_resetOrder = IN_resetOrderAngle1;
        rtDW.orderAngle_a = rtDW.orderAngle;
      }
    } else {
      if (fabs(rtDW.diffAngle) <= rtb_Switch2) {
        rtY.directionOfRotation = 0.0;
        if (rtDW.orderAngle == 0.0) {
          rtDW.orderAngle_a = 0.0;
          rtDW.is_resetOrder = IN_resetOrderAngle;
          rtY.directionOfRotation = 0.0;
        }
      }
    }

    if (rtDW.is_calculationTimeTurn == IN_enableEngines1) {
      rtDW.is_calculationTimeTurn = IN_previewTightenSling;
    } else {
      if (rtY.tightenSling != rtDW.lastTightenSling) {
        rtDW.is_calculationTimeTurn = IN_enableEngines1;
        calculateTimeTurn();
      }
    }
  }

  /* End of Chart: '<S4>/angleRegulator' */
  /* End of Outputs for SubSystem: '<Root>/flightRegulatorCFB' */

  /* Outport: '<Root>/modeFlight' */
  rtY.modeFlight = 0.0;

  /* Outputs for Atomic SubSystem: '<Root>/flightRegulatorCFB' */
  /* Outport: '<Root>/doingManeuverMode' incorporates:
   *  DataTypeConversion: '<S1>/Data Type Conversion10'
   */
  rtY.doingManeuverMode = (uint8_T)rtDW.flightMode1Mode;

  /* Outport: '<Root>/angleAngle ' incorporates:
   *  DataTypeConversion: '<S1>/Data Type Conversion13'
   *  Gain: '<S1>/Gain4'
   */
  rtY.angleAngle = (uint16_T)(57.295779513082323 * rtDW.outAngle_h);

  /* Outport: '<Root>/cmdTightenSlings' incorporates:
   *  Constant: '<S1>/Constant1'
   *  DataTypeConversion: '<S1>/Data Type Conversion1'
   */
  rtY.cmdTightenSlings = 0U;

  /* Outport: '<Root>/cmdTouchDown' incorporates:
   *  Constant: '<S10>/Constant'
   *  DataTypeConversion: '<S1>/Data Type Conversion12'
   *  Inport: '<Root>/isVeracityGns'
   *  Logic: '<S5>/Logical Operator'
   *  RelationalOperator: '<S10>/Compare'
   */
  rtY.cmdTouchDown = (uint8_T)((rtb_Add <= 300.0) && (rtU.isVeracityGns != 0.0));

  /* Outport: '<Root>/Period_ms' incorporates:
   *  Gain: '<S1>/Gain2'
   */
  rtY.Period_ms = 1000.0;

  /* End of Outputs for SubSystem: '<Root>/flightRegulatorCFB' */

  /* Outport: '<Root>/txNextPoint' */
  rtY.txNextPoint = 0.0;

  /* Outport: '<Root>/distanceAB' */
  rtY.distanceAB = rtb_DataTypeConversion11;

  /* Outputs for Atomic SubSystem: '<Root>/flightRegulatorCFB' */
  /* Outport: '<Root>/distanceB' incorporates:
   *  DataTypeConversion: '<S1>/Data Type Conversion2'
   */
  rtY.distanceB = (uint16_T)rtDW.outDistanceB;

  /* Outport: '<Root>/diffAngle' incorporates:
   *  DataTypeConversion: '<S1>/Data Type Conversion3'
   */
  rtY.diffAngle = (int16_T)floor(rtDW.diffAngleDegAbs);

  /* Outport: '<Root>/setAngle' incorporates:
   *  DataTypeConversion: '<S1>/Data Type Conversion5'
   *  Gain: '<S4>/piToDeg11'
   */
  rtY.setAngle = (uint16_T)(57.295779513082323 * rtDW.outSetAngle);

  /* Outport: '<Root>/stateAngleCorrection' incorporates:
   *  DataTypeConversion: '<S1>/Data Type Conversion6'
   */
  rtY.stateAngleCorrection = (uint8_T)rtDW.angleCorrectionMode;

  /* Outport: '<Root>/stateAngleDoing' incorporates:
   *  DataTypeConversion: '<S1>/Data Type Conversion7'
   */
  rtY.stateAngleDoing = (uint8_T)rtDW.angleDoingMode;

  /* Outport: '<Root>/stateTurn' incorporates:
   *  DataTypeConversion: '<S1>/Data Type Conversion8'
   */
  rtY.stateTurn = (uint8_T)rtDW.TurnMode;

  /* Outport: '<Root>/changeControl' incorporates:
   *  DataTypeConversion: '<S1>/Data Type Conversion9'
   */
  rtY.changeControl = (uint8_T)rtDW.isOffFlightTurning;

  /* Outport: '<Root>/lat1' incorporates:
   *  Gain: '<S3>/Gain'
   */
  rtY.lat1 = 57.295779513082323 * rtDW.phi1;

  /* Outport: '<Root>/lat2' incorporates:
   *  Gain: '<S3>/Gain1'
   */
  rtY.lat2 = 57.295779513082323 * rtDW.phi2;

  /* Outport: '<Root>/lon1' incorporates:
   *  Gain: '<S3>/Gain2'
   */
  rtY.lon1 = 57.295779513082323 * rtDW.la1;

  /* Outport: '<Root>/lon2' incorporates:
   *  Gain: '<S3>/Gain4'
   */
  rtY.lon2 = 57.295779513082323 * rtDW.la2;

  /* Outport: '<Root>/orderAngle' incorporates:
   *  DataTypeConversion: '<S1>/Data Type Conversion4'
   */
  rtY.orderAngle = (uint8_T)rtDW.orderAngle_a;

  /* Outport: '<Root>/distance2' incorporates:
   *  DataTypeConversion: '<S1>/Data Type Conversion14'
   */
  rtY.distance2 = (uint16_T)rtDW.outDistance2;

  /* End of Outputs for SubSystem: '<Root>/flightRegulatorCFB' */
}

/* Model initialize function */
void flightRegulatorCFB_initialize(void)
{
  /* Registration code */

  /* initialize non-finites */
  rt_InitInfAndNaN(sizeof(real_T));

  {
    int32_T i;

    /* SystemInitialize for Atomic SubSystem: '<Root>/flightRegulatorCFB' */
    /* SystemInitialize for Chart: '<S3>/parameter&#x421;alculation' */
    rtDW.sfEvent = CALL_EVENT;
    rtDW.radEarth = 6.378137E+6;
    rtDW.angleCorridor = 0.26179938779914941;
    rtDW.distanceStartBox = 1300.0;
    rtDW.sideBox = 500.0;
    rtDW.minimumDistance = 50.0;
    rtDW.tdMinBox = 3.0;
    rtDW.tdMinFinish = 2.1;
    rtDW.ticksPauseBox = 2.0;
    rtDW.boxAllow = 1.0;
    rtDW.TdReg1 = 1.0;
    rtDW.flightMode1Mode = flightMode1ModeType_None;
    rtDW.pointsMode = pointsModeType_None;
    rtDW.criteriaDefinition1Mode = criteriaDefinition1ModeType_None;

    /* SystemInitialize for Chart: '<S4>/distanceRegulator' */
    rtDW.angleCorrectionMode = angleCorrectionModeType_None;
    rtDW.angleDoingMode = angleDoingModeType_None;

    /* SystemInitialize for Chart: '<S4>/angleRegulator' */
    rtDW.timeWaitEngines = 1.0;
    rtDW.pTightenSling = 30.0;
    for (i = 0; i < 10; i++) {
      rtDW.timeWaitOfEngines[i] = 1.0;
    }

    rtDW.waitingEnginesMode = waitingEnginesModeType_None;
    rtDW.TurnMode = TurnModeType_None;
    rtDW.enableEngines = 1.0;

    /* End of SystemInitialize for Chart: '<S4>/angleRegulator' */
    /* End of SystemInitialize for SubSystem: '<Root>/flightRegulatorCFB' */
  }
}

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
