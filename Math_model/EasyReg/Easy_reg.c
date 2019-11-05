/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 *
 * File: Easy_reg.c
 *
 * Code generated for Simulink model 'Easy_reg'.
 *
 * Model version                  : 1.265
 * Simulink Coder version         : 8.14 (R2018a) 06-Feb-2018
 * C/C++ source code generated on : Tue Nov  5 17:10:06 2019
 *
 * Target selection: ert.tlc
 * Embedded hardware selection: ARM Compatible->ARM Cortex
 * Code generation objectives:
 *    1. Execution efficiency
 *    2. RAM efficiency
 * Validation result: Not run
 */

#include "Easy_reg.h"

/* Named constants for Chart: '<S1>/TaskController' */
#define IN_Angle                       ((uint8_T)1U)
#define IN_Box                         ((uint8_T)2U)
#define IN_Final                       ((uint8_T)3U)
#define IN_Flight                      ((uint8_T)1U)
#define IN_Init                        ((uint8_T)2U)
#define IN_Init_n                      ((uint8_T)1U)
#define IN_LD                          ((uint8_T)2U)
#define IN_LU                          ((uint8_T)3U)
#define IN_NO_ACTIVE_CHILD             ((uint8_T)0U)
#define IN_OFF                         ((uint8_T)1U)
#define IN_ON                          ((uint8_T)2U)
#define IN_RD                          ((uint8_T)4U)
#define IN_RU                          ((uint8_T)5U)
#define IN_StrapParking                ((uint8_T)4U)
#define IN_TDP_Only                    ((uint8_T)3U)
#define event_CancelFinal              (0)
#define event_Final                    (1)
#define event_NextStage                (2)
#define NumBitsPerChar                 8U

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

/* Forward declaration for local functions */
static real_T DistanceCalculator(const real_T Position[3], const real_T Target[3]);
static uint8_T VerticeSelect(const real_T AMP[3], const real_T BLU[3], const
  real_T BLD[3], const real_T BRU[3], const real_T BRD[3]);
static void enter_internal_Box(void);
static real_T CourseCalculator(const real_T To[3], const real_T From[3]);
static void EntryAngleMan(const real_T AM1[3], real_T AM2[3], const real_T
  Position[3], real_T TrackingCourse);
static void Control(const uint8_T *Mode, const real_T *y, const real_T
                    TmpSignalConversionAtSFunctionI[3], int32_T *sfEvent);
static real_T CheckFinalMane_o(const real_T *y, const real_T
  TmpSignalConversionAtSFunctionI[3], const real_T *y_m);
static real_T CheckArrival(const real_T Now[3], const real_T To[3], real_T
  Radius);
static void PositionRangeConverting(real_T InputPoint[3]);
static void AngleManCreator(const real_T Initial_point[3], const real_T TDP[3],
  real_T Angle, real_T AMP1[3], real_T AMP2[3]);
static void BOXCreator(const real_T TDP[3], real_T BoxStep, real_T BLU[3],
  real_T BLD[3], real_T BRU[3], real_T BRD[3]);
static void CheckFinalMane(const real_T *y, const real_T
  TmpSignalConversionAtSFunctionI[3], const real_T *y_m);

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
extern real_T rtGetNaN(void);
extern real32_T rtGetNaNF(void);

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

/* Function for Chart: '<S1>/TaskController' */
static real_T DistanceCalculator(const real_T Position[3], const real_T Target[3])
{
  real_T fi1;
  real_T fi2;
  fi1 = Position[0] * 3.1415926535897931 / 180.0;
  fi2 = Target[0] * 3.1415926535897931 / 180.0;
  return acos(cos(Target[1] * 3.1415926535897931 / 180.0 - Position[1] *
                  3.1415926535897931 / 180.0) * (cos(fi1) * cos(-fi2)) + sin(fi1)
              * sin(fi2)) * 6.371E+6;
}

/* Function for Chart: '<S1>/TaskController' */
static uint8_T VerticeSelect(const real_T AMP[3], const real_T BLU[3], const
  real_T BLD[3], const real_T BRU[3], const real_T BRD[3])
{
  uint8_T Vertice;
  real_T x[4];
  int32_T b_Vertice;
  x[0] = DistanceCalculator(AMP, BLU);
  x[1] = DistanceCalculator(AMP, BLD);
  x[2] = DistanceCalculator(AMP, BRU);
  x[3] = DistanceCalculator(AMP, BRD);
  b_Vertice = 0;
  if (DistanceCalculator(AMP, BLD) < DistanceCalculator(AMP, BLU)) {
    b_Vertice = 1;
  }

  if (DistanceCalculator(AMP, BRU) < x[b_Vertice]) {
    b_Vertice = 2;
  }

  if (DistanceCalculator(AMP, BRD) < x[b_Vertice]) {
    b_Vertice = 3;
  }

  Vertice = (uint8_T)(b_Vertice + 1);
  return Vertice;
}

/* Function for Chart: '<S1>/TaskController' */
static void enter_internal_Box(void)
{
  switch (Easy_reg_DW.was_Box) {
   case IN_Init_n:
    if (Easy_reg_DW.is_Box != IN_Init_n) {
      Easy_reg_DW.is_Box = IN_Init_n;
      Easy_reg_DW.was_Box = IN_Init_n;
      Easy_reg_DW.BoxEntry = VerticeSelect
        (Easy_reg_DW.TmpSignalConversionAtSFunctio_g, Easy_reg_DW.LUPoint,
         Easy_reg_DW.LDPoint, Easy_reg_DW.RUPoint, Easy_reg_DW.RDPoint);
    }
    break;

   case IN_LD:
    if (Easy_reg_DW.is_Box != IN_LD) {
      Easy_reg_DW.is_Box = IN_LD;
      Easy_reg_DW.was_Box = IN_LD;
      Easy_reg_DW.TargetPoint[0] = Easy_reg_DW.LDPoint[0];
      Easy_reg_DW.TargetPoint[1] = Easy_reg_DW.LDPoint[1];
      Easy_reg_DW.TargetPoint[2] = Easy_reg_DW.LDPoint[2];
    }
    break;

   case IN_LU:
    if (Easy_reg_DW.is_Box != IN_LU) {
      Easy_reg_DW.is_Box = IN_LU;
      Easy_reg_DW.was_Box = IN_LU;
      Easy_reg_DW.TargetPoint[0] = Easy_reg_DW.LUPoint[0];
      Easy_reg_DW.TargetPoint[1] = Easy_reg_DW.LUPoint[1];
      Easy_reg_DW.TargetPoint[2] = Easy_reg_DW.LUPoint[2];
    }
    break;

   case IN_RD:
    if (Easy_reg_DW.is_Box != IN_RD) {
      Easy_reg_DW.is_Box = IN_RD;
      Easy_reg_DW.was_Box = IN_RD;
      Easy_reg_DW.TargetPoint[0] = Easy_reg_DW.RDPoint[0];
      Easy_reg_DW.TargetPoint[1] = Easy_reg_DW.RDPoint[1];
      Easy_reg_DW.TargetPoint[2] = Easy_reg_DW.RDPoint[2];
    }
    break;

   case IN_RU:
    if (Easy_reg_DW.is_Box != IN_RU) {
      Easy_reg_DW.is_Box = IN_RU;
      Easy_reg_DW.was_Box = IN_RU;
      Easy_reg_DW.TargetPoint[0] = Easy_reg_DW.RUPoint[0];
      Easy_reg_DW.TargetPoint[1] = Easy_reg_DW.RUPoint[1];
      Easy_reg_DW.TargetPoint[2] = Easy_reg_DW.RUPoint[2];
    }
    break;

   default:
    if (Easy_reg_DW.is_Box != IN_Init_n) {
      Easy_reg_DW.is_Box = IN_Init_n;
      Easy_reg_DW.was_Box = IN_Init_n;
      Easy_reg_DW.BoxEntry = VerticeSelect
        (Easy_reg_DW.TmpSignalConversionAtSFunctio_g, Easy_reg_DW.LUPoint,
         Easy_reg_DW.LDPoint, Easy_reg_DW.RUPoint, Easy_reg_DW.RDPoint);
    }
    break;
  }
}

/* Function for Chart: '<S1>/TaskController' */
static real_T CourseCalculator(const real_T To[3], const real_T From[3])
{
  real_T y;
  real_T un_pow1;
  real_T un_pow2;
  real_T un_idx_1;
  real_T un_idx_0;
  un_idx_0 = To[0] - From[0];
  un_idx_1 = To[1] - From[1];
  if (un_idx_1 > 180.0) {
    un_idx_1 -= 360.0;
  } else {
    if (un_idx_1 < -180.0) {
      un_idx_1 += 360.0;
    }
  }

  un_pow1 = un_idx_0 * un_idx_0;
  un_pow2 = un_idx_1 * un_idx_1;
  if ((un_pow1 < 1.0E-16) && (un_pow2 < 1.0E-16)) {
    y = 0.0;
  } else {
    un_pow1 = sqrt(un_pow2 + un_pow1);
    y = acos(un_idx_0 / un_pow1);
    if (acos(un_idx_1 / un_pow1) > 1.5708) {
      y = 6.28319 - y;
    }
  }

  return y;
}

/* Function for Chart: '<S1>/TaskController' */
static void EntryAngleMan(const real_T AM1[3], real_T AM2[3], const real_T
  Position[3], real_T TrackingCourse)
{
  real_T Course1;
  real_T Course2;
  Course1 = CourseCalculator(AM1, Position) - TrackingCourse;
  Course2 = CourseCalculator(AM2, Position) - TrackingCourse;
  if (Course1 > 3.1415926535897931) {
    Course1 -= 6.2831853071795862;
  }

  if (Course1 < -3.1415926535897931) {
    Course1 += 6.2831853071795862;
  }

  if (Course2 > 3.1415926535897931) {
    Course2 -= 6.2831853071795862;
  }

  if (Course2 < -3.1415926535897931) {
    Course2 += 6.2831853071795862;
  }

  Course1 = fabs(Course1);
  Course2 = fabs(Course2);
  if (Course1 < Course2) {
    AM2[0] = AM1[0];
    AM2[1] = AM1[1];
    AM2[2] = AM1[2];
  }
}

/* Function for Chart: '<S1>/TaskController' */
static void Control(const uint8_T *Mode, const real_T *y, const real_T
                    TmpSignalConversionAtSFunctionI[3], int32_T *sfEvent)
{
  switch (Easy_reg_DW.is_Control) {
   case IN_Flight:
    switch (Easy_reg_DW.is_Flight) {
     case IN_Angle:
      switch (*sfEvent) {
       case event_NextStage:
        Easy_reg_DW.is_Flight = IN_Box;
        Easy_reg_DW.was_Flight = IN_Box;
        enter_internal_Box();
        break;

       case event_Final:
        Easy_reg_DW.is_Flight = IN_Final;
        Easy_reg_DW.was_Flight = IN_Final;
        Easy_reg_DW.TargetPoint[0] = TmpSignalConversionAtSFunctionI[0];
        Easy_reg_DW.TargetPoint[1] = TmpSignalConversionAtSFunctionI[1];
        Easy_reg_DW.TargetPoint[2] = TmpSignalConversionAtSFunctionI[2];
        break;
      }
      break;

     case IN_Box:
      if ((*sfEvent == event_Final) && (Easy_reg_DW.timeout == 0U)) {
        Easy_reg_DW.timeout = 60U;
        Easy_reg_DW.is_Box = IN_NO_ACTIVE_CHILD;
        Easy_reg_DW.is_Flight = IN_Final;
        Easy_reg_DW.was_Flight = IN_Final;
        Easy_reg_DW.TargetPoint[0] = TmpSignalConversionAtSFunctionI[0];
        Easy_reg_DW.TargetPoint[1] = TmpSignalConversionAtSFunctionI[1];
        Easy_reg_DW.TargetPoint[2] = TmpSignalConversionAtSFunctionI[2];
      } else {
        switch (Easy_reg_DW.is_Box) {
         case IN_Init_n:
          switch (Easy_reg_DW.BoxEntry) {
           case 1:
            Easy_reg_DW.is_Box = IN_LU;
            Easy_reg_DW.was_Box = IN_LU;
            Easy_reg_DW.TargetPoint[0] = Easy_reg_DW.LUPoint[0];
            Easy_reg_DW.TargetPoint[1] = Easy_reg_DW.LUPoint[1];
            Easy_reg_DW.TargetPoint[2] = Easy_reg_DW.LUPoint[2];
            break;

           case 4:
            Easy_reg_DW.is_Box = IN_RD;
            Easy_reg_DW.was_Box = IN_RD;
            Easy_reg_DW.TargetPoint[0] = Easy_reg_DW.RDPoint[0];
            Easy_reg_DW.TargetPoint[1] = Easy_reg_DW.RDPoint[1];
            Easy_reg_DW.TargetPoint[2] = Easy_reg_DW.RDPoint[2];
            break;

           case 2:
            Easy_reg_DW.is_Box = IN_LD;
            Easy_reg_DW.was_Box = IN_LD;
            Easy_reg_DW.TargetPoint[0] = Easy_reg_DW.LDPoint[0];
            Easy_reg_DW.TargetPoint[1] = Easy_reg_DW.LDPoint[1];
            Easy_reg_DW.TargetPoint[2] = Easy_reg_DW.LDPoint[2];
            break;

           case 3:
            Easy_reg_DW.is_Box = IN_RU;
            Easy_reg_DW.was_Box = IN_RU;
            Easy_reg_DW.TargetPoint[0] = Easy_reg_DW.RUPoint[0];
            Easy_reg_DW.TargetPoint[1] = Easy_reg_DW.RUPoint[1];
            Easy_reg_DW.TargetPoint[2] = Easy_reg_DW.RUPoint[2];
            break;

           default:
            Easy_reg_DW.BoxEntry = VerticeSelect
              (Easy_reg_DW.TmpSignalConversionAtSFunctio_g, Easy_reg_DW.LUPoint,
               Easy_reg_DW.LDPoint, Easy_reg_DW.RUPoint, Easy_reg_DW.RDPoint);
            break;
          }
          break;

         case IN_LD:
          if (*sfEvent == event_NextStage) {
            Easy_reg_DW.is_Box = IN_RD;
            Easy_reg_DW.was_Box = IN_RD;
            Easy_reg_DW.TargetPoint[0] = Easy_reg_DW.RDPoint[0];
            Easy_reg_DW.TargetPoint[1] = Easy_reg_DW.RDPoint[1];
            Easy_reg_DW.TargetPoint[2] = Easy_reg_DW.RDPoint[2];
          }
          break;

         case IN_LU:
          if (*sfEvent == event_NextStage) {
            Easy_reg_DW.is_Box = IN_LD;
            Easy_reg_DW.was_Box = IN_LD;
            Easy_reg_DW.TargetPoint[0] = Easy_reg_DW.LDPoint[0];
            Easy_reg_DW.TargetPoint[1] = Easy_reg_DW.LDPoint[1];
            Easy_reg_DW.TargetPoint[2] = Easy_reg_DW.LDPoint[2];
          }
          break;

         case IN_RD:
          if (*sfEvent == event_NextStage) {
            Easy_reg_DW.is_Box = IN_RU;
            Easy_reg_DW.was_Box = IN_RU;
            Easy_reg_DW.TargetPoint[0] = Easy_reg_DW.RUPoint[0];
            Easy_reg_DW.TargetPoint[1] = Easy_reg_DW.RUPoint[1];
            Easy_reg_DW.TargetPoint[2] = Easy_reg_DW.RUPoint[2];
          }
          break;

         case IN_RU:
          if (*sfEvent == event_NextStage) {
            Easy_reg_DW.is_Box = IN_LU;
            Easy_reg_DW.was_Box = IN_LU;
            Easy_reg_DW.TargetPoint[0] = Easy_reg_DW.LUPoint[0];
            Easy_reg_DW.TargetPoint[1] = Easy_reg_DW.LUPoint[1];
            Easy_reg_DW.TargetPoint[2] = Easy_reg_DW.LUPoint[2];
          }
          break;
        }
      }
      break;

     case IN_Final:
      if (*sfEvent == event_NextStage) {
        Easy_reg_DW.is_Flight = IN_StrapParking;
        Easy_reg_DW.was_Flight = IN_StrapParking;
        Easy_reg_DW.TargetEnable = 0U;
      }
      break;

     case IN_StrapParking:
      break;
    }
    break;

   case IN_Init:
    if (*Mode == 0) {
      Easy_reg_DW.is_Control = IN_TDP_Only;
      Easy_reg_DW.TargetPoint[0] = TmpSignalConversionAtSFunctionI[0];
      Easy_reg_DW.TargetPoint[1] = TmpSignalConversionAtSFunctionI[1];
      Easy_reg_DW.TargetPoint[2] = TmpSignalConversionAtSFunctionI[2];
      Easy_reg_DW.TargetEnable = 1U;
    } else if (((*Mode == 1) || (*Mode == 2)) && (Easy_reg_DW.timeout == 0U)) {
      Easy_reg_DW.is_Control = IN_Flight;
      Easy_reg_DW.TargetEnable = 1U;
      switch (Easy_reg_DW.was_Flight) {
       case IN_Angle:
        if (Easy_reg_DW.is_Flight != IN_Angle) {
          Easy_reg_DW.is_Flight = IN_Angle;
          Easy_reg_DW.was_Flight = IN_Angle;
          Easy_reg_DW.TargetPoint[0] = Easy_reg_DW.Point2[0];
          Easy_reg_DW.TargetPoint[1] = Easy_reg_DW.Point2[1];
          Easy_reg_DW.TargetPoint[2] = Easy_reg_DW.Point2[2];
          EntryAngleMan(Easy_reg_DW.Point1, Easy_reg_DW.TargetPoint,
                        Easy_reg_DW.TmpSignalConversionAtSFunctio_g, *y);
        }
        break;

       case IN_Box:
        if (Easy_reg_DW.is_Flight != IN_Box) {
          Easy_reg_DW.is_Flight = IN_Box;
          Easy_reg_DW.was_Flight = IN_Box;
        }

        enter_internal_Box();
        break;

       case IN_Final:
        if (Easy_reg_DW.is_Flight != IN_Final) {
          Easy_reg_DW.is_Flight = IN_Final;
          Easy_reg_DW.was_Flight = IN_Final;
          Easy_reg_DW.TargetPoint[0] = TmpSignalConversionAtSFunctionI[0];
          Easy_reg_DW.TargetPoint[1] = TmpSignalConversionAtSFunctionI[1];
          Easy_reg_DW.TargetPoint[2] = TmpSignalConversionAtSFunctionI[2];
        }
        break;

       case IN_StrapParking:
        if (Easy_reg_DW.is_Flight != IN_StrapParking) {
          Easy_reg_DW.is_Flight = IN_StrapParking;
          Easy_reg_DW.was_Flight = IN_StrapParking;
          Easy_reg_DW.TargetEnable = 0U;
        }
        break;

       default:
        if (*Mode == 1) {
          if (Easy_reg_DW.is_Flight != IN_Box) {
            Easy_reg_DW.is_Flight = IN_Box;
            Easy_reg_DW.was_Flight = IN_Box;
          }

          enter_internal_Box();
        } else {
          if ((*Mode == 2) && (!(Easy_reg_DW.is_Flight == IN_Angle))) {
            Easy_reg_DW.is_Flight = IN_Angle;
            Easy_reg_DW.was_Flight = IN_Angle;
            Easy_reg_DW.TargetPoint[0] = Easy_reg_DW.Point2[0];
            Easy_reg_DW.TargetPoint[1] = Easy_reg_DW.Point2[1];
            Easy_reg_DW.TargetPoint[2] = Easy_reg_DW.Point2[2];
            EntryAngleMan(Easy_reg_DW.Point1, Easy_reg_DW.TargetPoint,
                          Easy_reg_DW.TmpSignalConversionAtSFunctio_g, *y);
          }
        }
        break;
      }
    } else {
      Easy_reg_DW.TargetEnable = 1U;
    }
    break;

   case IN_TDP_Only:
    break;
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

/* Function for Chart: '<S1>/TaskController' */
static real_T CheckFinalMane_o(const real_T *y, const real_T
  TmpSignalConversionAtSFunctionI[3], const real_T *y_m)
{
  real_T status;
  real_T angle;

  /* Outport: '<Root>/DistanceToTDP' */
  Easy_reg_Y.DistanceToTDP = DistanceCalculator
    (Easy_reg_DW.TmpSignalConversionAtSFunctio_g,
     TmpSignalConversionAtSFunctionI);

  /* Outport: '<Root>/TimeToHorTarget' incorporates:
   *  Constant: '<S3>/Constant1'
   *  Outport: '<Root>/DistanceToTDP'
   */
  Easy_reg_Y.TimeToHorTarget = Easy_reg_Y.DistanceToTDP / 20.0;

  /* Outport: '<Root>/TimeToTD' incorporates:
   *  Constant: '<S3>/Constant2'
   *  Inport: '<Root>/ReliefOnTDP'
   */
  Easy_reg_Y.TimeToTD = (Easy_reg_DW.TmpSignalConversionAtSFunctio_g[2] -
    (TmpSignalConversionAtSFunctionI[2] + (real_T)Easy_reg_U.ReliefOnTDP)) / 8.0;
  angle = fabs(*y_m - *y) * 180.0 / 3.1415926535897931;
  if (angle > 180.0) {
    angle -= 180.0;
  }

  /* Outport: '<Root>/TimeToHorTarget' incorporates:
   *  Outport: '<Root>/TimeToTD'
   */
  Easy_reg_Y.TimeToHorTarget += angle * angle * 0.003 + 0.008 * angle;
  status = (Easy_reg_Y.TimeToHorTarget > Easy_reg_Y.TimeToTD);
  return status;
}

/* Function for Chart: '<S1>/TaskController' */
static real_T CheckArrival(const real_T Now[3], const real_T To[3], real_T
  Radius)
{
  real_T status;
  status = (DistanceCalculator(Now, To) < Radius);
  return status;
}

/* Function for Chart: '<S1>/TaskController' */
static void PositionRangeConverting(real_T InputPoint[3])
{
  if (InputPoint[1] > 180.0) {
    InputPoint[1] -= 360.0;
  } else {
    if (InputPoint[1] < -180.0) {
      InputPoint[1] += 360.0;
    }
  }
}

/* Function for Chart: '<S1>/TaskController' */
static void AngleManCreator(const real_T Initial_point[3], const real_T TDP[3],
  real_T Angle, real_T AMP1[3], real_T AMP2[3])
{
  real_T dx;
  real_T dy;
  real_T AMP1_tmp;
  real_T AMP1_tmp_0;
  dx = (TDP[1] - Initial_point[1]) / 2.0;
  dy = (TDP[0] - Initial_point[0]) / 2.0;
  AMP1_tmp = Angle * 3.1415926535897931 / 180.0;
  AMP1_tmp_0 = cos(AMP1_tmp);
  AMP1_tmp = sin(AMP1_tmp);
  AMP1[0] = (AMP1_tmp * dx + AMP1_tmp_0 * dy) + Initial_point[0];
  AMP1[1] = (AMP1_tmp_0 * dx - AMP1_tmp * dy) + Initial_point[1];
  AMP1[2] = 0.0;
  PositionRangeConverting(AMP1);
  AMP1_tmp = -Angle * 3.1415926535897931 / 180.0;
  AMP1_tmp_0 = cos(AMP1_tmp);
  AMP1_tmp = sin(AMP1_tmp);
  AMP2[0] = (AMP1_tmp * dx + AMP1_tmp_0 * dy) + Initial_point[0];
  AMP2[1] = (AMP1_tmp_0 * dx - AMP1_tmp * dy) + Initial_point[1];
  AMP2[2] = 0.0;
  PositionRangeConverting(AMP2);
}

/* Function for Chart: '<S1>/TaskController' */
static void BOXCreator(const real_T TDP[3], real_T BoxStep, real_T BLU[3],
  real_T BLD[3], real_T BRU[3], real_T BRD[3])
{
  real_T BLU_tmp;
  real_T BLD_tmp;
  real_T LatStep_tmp;
  BLU[2] = 0.0;
  BLD[2] = 0.0;
  BRU[2] = 0.0;
  BRD[2] = 0.0;
  LatStep_tmp = BoxStep / 1000.0 * 360.0 / 40000.0;
  BLU_tmp = TDP[0] + LatStep_tmp;
  BLU[0] = BLU_tmp;
  BLD_tmp = TDP[0] - LatStep_tmp;
  BLD[0] = BLD_tmp;
  BRU[0] = BLU_tmp;
  BRD[0] = BLD_tmp;
  BLU[1] = TDP[1] - LatStep_tmp / cos(3.1415926535897931 * BLU_tmp / 180.0);
  BLD[1] = TDP[1] - LatStep_tmp / cos(3.1415926535897931 * BLD_tmp / 180.0);
  BRU[1] = LatStep_tmp / cos(3.1415926535897931 * BLU_tmp / 180.0) + TDP[1];
  BRD[1] = LatStep_tmp / cos(3.1415926535897931 * BLD_tmp / 180.0) + TDP[1];
  PositionRangeConverting(BLU);
  PositionRangeConverting(BLD);
  PositionRangeConverting(BRU);
  PositionRangeConverting(BRD);
}

/* Function for Chart: '<S1>/TaskController' */
static void CheckFinalMane(const real_T *y, const real_T
  TmpSignalConversionAtSFunctionI[3], const real_T *y_m)
{
  real_T angle;

  /* Outport: '<Root>/DistanceToTDP' */
  Easy_reg_Y.DistanceToTDP = DistanceCalculator
    (Easy_reg_DW.TmpSignalConversionAtSFunctio_g,
     TmpSignalConversionAtSFunctionI);

  /* Outport: '<Root>/TimeToHorTarget' incorporates:
   *  Constant: '<S3>/Constant1'
   *  Outport: '<Root>/DistanceToTDP'
   */
  Easy_reg_Y.TimeToHorTarget = Easy_reg_Y.DistanceToTDP / 20.0;

  /* Outport: '<Root>/TimeToTD' incorporates:
   *  Constant: '<S3>/Constant2'
   *  Inport: '<Root>/ReliefOnTDP'
   */
  Easy_reg_Y.TimeToTD = (Easy_reg_DW.TmpSignalConversionAtSFunctio_g[2] -
    (TmpSignalConversionAtSFunctionI[2] + (real_T)Easy_reg_U.ReliefOnTDP)) / 8.0;
  angle = fabs(*y_m - *y) * 180.0 / 3.1415926535897931;
  if (angle > 180.0) {
    angle -= 180.0;
  }

  /* Outport: '<Root>/TimeToHorTarget' */
  Easy_reg_Y.TimeToHorTarget += angle * angle * 0.003 + 0.008 * angle;
}

/* Model step function */
void Easy_reg_step(void)
{
  real_T un[3];
  real_T d_lon;
  real_T un_pow1;
  real_T un_pow2;
  uint8_T Mode;
  real_T y;
  int32_T sfEvent;
  uint32_T qY;
  int16_T tmp;
  boolean_T guard1 = false;

  /* Outputs for Atomic SubSystem: '<Root>/Easy_reg' */
  /* DataStoreWrite: '<S1>/Data Store Write3' incorporates:
   *  Constant: '<S1>/Mode'
   */
  Mode = 2U;

  /* MATLAB Function: '<S8>/Heading_true' incorporates:
   *  Inport: '<Root>/Pos_lat'
   *  Inport: '<Root>/Pos_lon'
   *  Memory: '<S8>/LastPos'
   *  Memory: '<S8>/Memory1'
   */
  un[0] = Easy_reg_U.Pos_lat - Easy_reg_DW.LastPos_1_PreviousInput;
  un[1] = Easy_reg_U.Pos_lon - Easy_reg_DW.LastPos_2_PreviousInput;
  d_lon = un[1];
  if (un[1] > 180.0) {
    d_lon = un[1] - 360.0;
  } else {
    if (un[1] < -180.0) {
      d_lon = un[1] + 360.0;
    }
  }

  un_pow1 = un[0] * un[0];
  un_pow2 = d_lon * d_lon;
  if ((un_pow1 < 1.0E-16) && (un_pow2 < 1.0E-16)) {
    y = Easy_reg_DW.Memory1_PreviousInput;
  } else {
    un_pow1 = sqrt(un_pow2 + un_pow1);
    y = acos(un[0] / un_pow1);
    if (acos(d_lon / un_pow1) > 1.5708) {
      y = 6.28319 - y;
    }
  }

  /* End of MATLAB Function: '<S8>/Heading_true' */

  /* MATLAB Function: '<S7>/TDPCourse' incorporates:
   *  Inport: '<Root>/Pos_lat'
   *  Inport: '<Root>/Pos_lon'
   *  Inport: '<Root>/TDP_lat'
   *  Inport: '<Root>/TDP_lon'
   */
  un[0] = Easy_reg_U.TDP_lat - Easy_reg_U.Pos_lat;
  un[1] = Easy_reg_U.TDP_lon - Easy_reg_U.Pos_lon;
  d_lon = un[1];
  if (un[1] > 180.0) {
    d_lon = un[1] - 360.0;
  } else {
    if (un[1] < -180.0) {
      d_lon = un[1] + 360.0;
    }
  }

  un_pow1 = un[0] * un[0];
  un_pow2 = d_lon * d_lon;
  if ((un_pow1 < 1.0E-16) && (un_pow2 < 1.0E-16)) {
    un_pow1 = 0.0;
  } else {
    un_pow2 = sqrt(un_pow2 + un_pow1);
    un_pow1 = acos(un[0] / un_pow2);
    if (acos(d_lon / un_pow2) > 1.5708) {
      un_pow1 = 6.28319 - un_pow1;
    }
  }

  /* End of MATLAB Function: '<S7>/TDPCourse' */

  /* SignalConversion: '<S5>/TmpSignal ConversionAt SFunction Inport1' incorporates:
   *  Chart: '<S1>/TaskController'
   *  Inport: '<Root>/TDP_alt'
   *  Inport: '<Root>/TDP_lat'
   *  Inport: '<Root>/TDP_lon'
   */
  un[0] = Easy_reg_U.TDP_lat;
  un[1] = Easy_reg_U.TDP_lon;
  un[2] = Easy_reg_U.TDP_alt;

  /* SignalConversion: '<S5>/TmpSignal ConversionAt SFunction Inport2' incorporates:
   *  Chart: '<S1>/TaskController'
   *  Inport: '<Root>/Pos_alt'
   *  Inport: '<Root>/Pos_lat'
   *  Inport: '<Root>/Pos_lon'
   */
  Easy_reg_DW.TmpSignalConversionAtSFunctio_g[0] = Easy_reg_U.Pos_lat;
  Easy_reg_DW.TmpSignalConversionAtSFunctio_g[1] = Easy_reg_U.Pos_lon;
  Easy_reg_DW.TmpSignalConversionAtSFunctio_g[2] = Easy_reg_U.Pos_alt;

  /* Chart: '<S1>/TaskController' incorporates:
   *  Constant: '<S1>/AngleManevr'
   *  Constant: '<S1>/ArrivalRadius'
   *  Constant: '<S1>/BoxSize'
   *  DataStoreWrite: '<S1>/Data Store Write'
   *  DataStoreWrite: '<S1>/Data Store Write1'
   *  DataStoreWrite: '<S1>/Data Store Write2'
   *  Inport: '<Root>/Relief'
   *  Inport: '<Root>/TDP_alt'
   *  Inport: '<Root>/TDP_lat'
   *  Inport: '<Root>/TDP_lon'
   */
  sfEvent = -1;
  if (Easy_reg_DW.is_active_c1_Easy_reg == 0U) {
    Easy_reg_DW.is_active_c1_Easy_reg = 1U;
    if (Easy_reg_DW.is_active_Timer != 1U) {
      Easy_reg_DW.is_active_Timer = 1U;
      Easy_reg_DW.time = 0U;
      Easy_reg_DW.timeout = 0U;
    }

    if (Easy_reg_DW.is_active_Control != 1U) {
      Easy_reg_DW.is_active_Control = 1U;
    }

    if (Easy_reg_DW.is_Control != IN_Init) {
      Easy_reg_DW.is_Control = IN_Init;
      Easy_reg_DW.TargetEnable = 0U;

      /* Outport: '<Root>/TD_CMD' */
      Easy_reg_Y.TD_CMD = 0U;
      Easy_reg_DW.timeout = 60U;
      Easy_reg_DW.TargetPoint[0] = Easy_reg_U.TDP_lat;
      Easy_reg_DW.TargetPoint[1] = Easy_reg_U.TDP_lon;
      Easy_reg_DW.TargetPoint[2] = Easy_reg_U.TDP_alt;
      AngleManCreator(Easy_reg_DW.TmpSignalConversionAtSFunctio_g, un, 10.0,
                      Easy_reg_DW.Point1, Easy_reg_DW.Point2);
      BOXCreator(un, 1500.0, Easy_reg_DW.LUPoint, Easy_reg_DW.LDPoint,
                 Easy_reg_DW.RUPoint, Easy_reg_DW.RDPoint);
    }

    if (Easy_reg_DW.is_active_Touchdown != 1U) {
      Easy_reg_DW.is_active_Touchdown = 1U;
    }

    if (Easy_reg_DW.is_Touchdown != IN_OFF) {
      Easy_reg_DW.is_Touchdown = IN_OFF;

      /* Outport: '<Root>/TD_CMD' */
      Easy_reg_Y.TD_CMD = 0U;
    }

    if (Easy_reg_DW.is_active_EventGen != 1U) {
      Easy_reg_DW.is_active_EventGen = 1U;
      CheckFinalMane(&y, un, &un_pow1);
    }
  } else {
    if (Easy_reg_DW.is_active_Timer != 0U) {
      qY = Easy_reg_DW.time + /*MW:OvSatOk*/ 1U;
      if (qY < Easy_reg_DW.time) {
        qY = MAX_uint32_T;
      }

      Easy_reg_DW.time = qY;
      if (Easy_reg_DW.timeout > 0U) {
        qY = Easy_reg_DW.timeout - /*MW:OvSatOk*/ 1U;
        if (qY > Easy_reg_DW.timeout) {
          qY = 0U;
        }

        Easy_reg_DW.timeout = qY;
      }
    }

    if (Easy_reg_DW.is_active_Control != 0U) {
      Control(&Mode, &y, un, &sfEvent);
    }

    if (Easy_reg_DW.is_active_Touchdown != 0U) {
      guard1 = false;
      switch (Easy_reg_DW.is_Touchdown) {
       case IN_OFF:
        if ((Easy_reg_U.ReliefAvailable == 0) &&
            (Easy_reg_DW.TmpSignalConversionAtSFunctio_g[2] <=
             Easy_reg_U.TDP_alt)) {
          guard1 = true;
        } else if (Easy_reg_U.ReliefAvailable == 1) {
          d_lon = rt_roundd_snf(Easy_reg_DW.TmpSignalConversionAtSFunctio_g[2] -
                                (real_T)Easy_reg_U.Relief);
          if (d_lon < 32768.0) {
            if (d_lon >= -32768.0) {
              tmp = (int16_T)d_lon;
            } else {
              tmp = MIN_int16_T;
            }
          } else {
            tmp = MAX_int16_T;
          }

          if (tmp <= Easy_reg_U.TDP_alt) {
            guard1 = true;
          } else {
            /* Outport: '<Root>/TD_CMD' */
            Easy_reg_Y.TD_CMD = 0U;
          }
        } else {
          /* Outport: '<Root>/TD_CMD' */
          Easy_reg_Y.TD_CMD = 0U;
        }
        break;

       case IN_ON:
        /* Outport: '<Root>/TD_CMD' */
        Easy_reg_Y.TD_CMD = 1U;
        break;
      }

      if (guard1) {
        Easy_reg_DW.is_Touchdown = IN_ON;

        /* Outport: '<Root>/TD_CMD' */
        Easy_reg_Y.TD_CMD = 1U;
      }
    }

    if (Easy_reg_DW.is_active_EventGen != 0U) {
      if (CheckFinalMane_o(&y, un, &un_pow1) == 1.0) {
        sfEvent = event_Final;
        if (Easy_reg_DW.is_active_Control != 0U) {
          Control(&Mode, &y, un, &sfEvent);
        }
      }

      if (CheckArrival(Easy_reg_DW.TmpSignalConversionAtSFunctio_g,
                       Easy_reg_DW.TargetPoint, 300.0) == 1.0) {
        sfEvent = event_NextStage;
        if (Easy_reg_DW.is_active_Control != 0U) {
          Control(&Mode, &y, un, &sfEvent);
        }
      } else {
        sfEvent = event_CancelFinal;
        if (Easy_reg_DW.is_active_Control != 0U) {
          Control(&Mode, &y, un, &sfEvent);
        }
      }
    }
  }

  /* MATLAB Function: '<S2>/TargetCourse' incorporates:
   *  Inport: '<Root>/Pos_lat'
   *  Inport: '<Root>/Pos_lon'
   */
  un[0] = Easy_reg_DW.TargetPoint[0] - Easy_reg_U.Pos_lat;
  un[1] = Easy_reg_DW.TargetPoint[1] - Easy_reg_U.Pos_lon;
  d_lon = un[1];
  if (un[1] > 180.0) {
    d_lon = un[1] - 360.0;
  } else {
    if (un[1] < -180.0) {
      d_lon = un[1] + 360.0;
    }
  }

  un_pow1 = un[0] * un[0];
  un_pow2 = d_lon * d_lon;
  if ((un_pow1 < 1.0E-16) && (un_pow2 < 1.0E-16)) {
    un_pow1 = 0.0;
  } else {
    un_pow2 = sqrt(un_pow2 + un_pow1);
    un_pow1 = acos(un[0] / un_pow2);
    if (acos(d_lon / un_pow2) > 1.5708) {
      un_pow1 = 6.28319 - un_pow1;
    }
  }

  /* End of MATLAB Function: '<S2>/TargetCourse' */

  /* Sum: '<S4>/Sum1' */
  d_lon = un_pow1 - y;

  /* MATLAB Function: '<S4>/ControlDemode' */
  if (d_lon > 3.14) {
    d_lon -= 6.2819;
  } else {
    if (d_lon < -3.14) {
      d_lon += 6.2819;
    }
  }

  /* End of MATLAB Function: '<S4>/ControlDemode' */

  /* Gain: '<S19>/Gain' */
  d_lon *= 57.295779513082323;

  /* Saturate: '<S4>/Base_Sat' */
  if (d_lon > 50.0) {
    d_lon = 50.0;
  } else {
    if (d_lon < -50.0) {
      d_lon = -50.0;
    }
  }

  /* End of Saturate: '<S4>/Base_Sat' */

  /* Gain: '<S20>/Gain1' incorporates:
   *  Gain: '<S20>/Gain'
   *  Rounding: '<S20>/Rounding Function'
   */
  d_lon = floor(0.1 * d_lon) * 10.0;

  /* Update for Memory: '<S8>/LastPos' incorporates:
   *  Inport: '<Root>/Pos_lat'
   *  Inport: '<Root>/Pos_lon'
   */
  Easy_reg_DW.LastPos_1_PreviousInput = Easy_reg_U.Pos_lat;
  Easy_reg_DW.LastPos_2_PreviousInput = Easy_reg_U.Pos_lon;

  /* Update for Memory: '<S8>/Memory1' */
  Easy_reg_DW.Memory1_PreviousInput = y;

  /* MATLAB Function: '<S20>/dead-zone' */
  if (fabs(d_lon) < 20.0) {
    d_lon = 0.0;
  }

  /* End of MATLAB Function: '<S20>/dead-zone' */

  /* Outport: '<Root>/BIM_CMD' incorporates:
   *  Product: '<S4>/Product'
   */
  Easy_reg_Y.BIM_CMD = (real_T)Easy_reg_DW.TargetEnable * d_lon;

  /* End of Outputs for SubSystem: '<Root>/Easy_reg' */
}

/* Model initialize function */
void Easy_reg_initialize(void)
{
  /* Registration code */

  /* initialize non-finites */
  rt_InitInfAndNaN(sizeof(real_T));
}

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
