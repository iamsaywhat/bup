/*
 * File: Easy_reg.c
 *
 * Code generated for Simulink model 'Easy_reg'.
 *
 * Model version                  : 1.749
 * Simulink Coder version         : 8.14 (R2018a) 06-Feb-2018
 * C/C++ source code generated on : Thu Dec  5 15:36:40 2019
 *
 * Target selection: ert.tlc
 * Embedded hardware selection: ARM Compatible->ARM Cortex
 * Code generation objectives:
 *    1. Execution efficiency
 *    2. RAM efficiency
 * Validation result: Not run
 */

#include "Easy_reg.h"

/* Named constants for Chart: '<S2>/DriftEstimator' */
#define IN_DriftSolver                 ((uint8_T)1U)
#define IN_FixBeforeCorrection         ((uint8_T)2U)
#define IN_Initialization              ((uint8_T)3U)
#define IN_Pause                       ((uint8_T)4U)

/* Named constants for Chart: '<S1>/TaskController' */
#define IN_Angle                       ((uint8_T)1U)
#define IN_Box                         ((uint8_T)2U)
#define IN_Disable                     ((uint8_T)1U)
#define IN_Enable                      ((uint8_T)2U)
#define IN_Final                       ((uint8_T)3U)
#define IN_Flight                      ((uint8_T)1U)
#define IN_Get                         ((uint8_T)1U)
#define IN_Init                        ((uint8_T)2U)
#define IN_Init_n                      ((uint8_T)1U)
#define IN_Init_n0                     ((uint8_T)3U)
#define IN_LD                          ((uint8_T)2U)
#define IN_LU                          ((uint8_T)3U)
#define IN_Left                        ((uint8_T)1U)
#define IN_NonBlockingControl          ((uint8_T)4U)
#define IN_OFF                         ((uint8_T)1U)
#define IN_ON                          ((uint8_T)2U)
#define IN_RD                          ((uint8_T)4U)
#define IN_RU                          ((uint8_T)5U)
#define IN_Right                       ((uint8_T)2U)
#define IN_Shutdown                    ((uint8_T)5U)
#define IN_Skip                        ((uint8_T)2U)
#define IN_StepAngle                   ((uint8_T)4U)
#define IN_TDP_Only                    ((uint8_T)3U)
#define IN_Wait                        ((uint8_T)3U)
#define event_CancelFinal              (0)
#define event_Final                    (1)
#define event_NextStage                (2)
#define event_NonBlockingMode          (3)
#define event_Parking                  (4)
#define event_wantDisable              (5)
#define event_wantEnable               (6)
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
extern real_T rt_roundd_snf(real_T u);

/* Forward declaration for local functions */
static uint32_T TimeoutDecrement(uint32_T TimeoutTag);
static uint8_T VerticeSelect(const real_T AMP[3], const real_T BLU[3], const
  real_T BLD[3], const real_T BRU[3], const real_T BRD[3]);
static void enter_internal_Box(void);
static void BimSupply(void);
static real_T nonlinearCoefficientFunction(real_T distance);
static real_T Angle2Course(real_T angle);
static void Flight(const real_T *AngleManevr, const uint32_T *AngleTimeout);
static real_T Heading(const real_T u1[3], const real_T u0[3], real_T last_y);
static void Control(const real_T *AngleManevr, const uint32_T *AngleTimeout,
                    const uint8_T *Mode);
static real_T CheckFinalMane(const real_T *V_hor, const real_T *V_vert);
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
typedef struct
{
  struct
  {
    uint32_T wordH;
    uint32_T wordL;
  }
  words;
}
BigEndianIEEEDouble;

typedef struct
{
  struct
  {
    uint32_T wordL;
    uint32_T wordH;
  }
  words;
}
LittleEndianIEEEDouble;

typedef struct
{
  union
  {
    real32_T wordLreal;
    uint32_T wordLuint;
  }
  wordL;
}
IEEESingle;

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
  if (bitsPerReal == 32U)
  {
    nan = rtGetNaNF();
  }
  else
  {
    union
    {
      LittleEndianIEEEDouble bitVal;
      real_T fltVal;
    }
    tmpVal;

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
  IEEESingle nanF =
  {
    {
      0
    }
  };

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
  if (bitsPerReal == 32U)
  {
    inf = rtGetInfF();
  }
  else
  {
    union
    {
      LittleEndianIEEEDouble bitVal;
      real_T fltVal;
    }
    tmpVal;

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
  if (bitsPerReal == 32U)
  {
    minf = rtGetMinusInfF();
  }
  else
  {
    union
    {
      LittleEndianIEEEDouble bitVal;
      real_T fltVal;
    }
    tmpVal;

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

real_T rt_atan2d_snf(real_T u0, real_T u1)
{
  real_T y;
  int32_T u0_0;
  int32_T u1_0;
  if (rtIsNaN(u0) || rtIsNaN(u1))
  {
    y = (rtNaN);
  }
  else if (rtIsInf(u0) && rtIsInf(u1))
  {
    if (u0 > 0.0)
    {
      u0_0 = 1;
    }
    else
    {
      u0_0 = -1;
    }

    if (u1 > 0.0)
    {
      u1_0 = 1;
    }
    else
    {
      u1_0 = -1;
    }

    y = atan2(u0_0, u1_0);
  }
  else if (u1 == 0.0)
  {
    if (u0 > 0.0)
    {
      y = RT_PI / 2.0;
    }
    else if (u0 < 0.0)
    {
      y = -(RT_PI / 2.0);
    }
    else
    {
      y = 0.0;
    }
  }
  else
  {
    y = atan2(u0, u1);
  }

  return y;
}

/*
 * Function for Chart: '<S1>/TaskController'
 * function Timeoutput = TimeoutDecrement (TimeoutTag)
 */
static uint32_T TimeoutDecrement(uint32_T TimeoutTag)
{
  uint32_T Timeoutput;

  /* MATLAB Function 'TimeoutDecrement': '<S7>:573' */
  /* Graphical Function 'TimeoutDecrement': '<S7>:573' */
  /* '<S7>:577:1' sf_internal_predicateOutput = ... */
  /* '<S7>:577:1' TimeoutTag > 0; */
  if (TimeoutTag > 0U)
  {
    /* '<S7>:577:1' Timeoutput = TimeoutTag - 1; */
    Timeoutput = TimeoutTag - 1U;
  }
  else
  {
    /* '<S7>:578:1' Timeoutput = uint32(0); */
    Timeoutput = 0U;
  }

  return Timeoutput;
}

real_T rt_remd_snf(real_T u0, real_T u1)
{
  real_T y;
  real_T q;
  if (!((!rtIsNaN(u0)) && (!rtIsInf(u0)) && ((!rtIsNaN(u1)) && (!rtIsInf(u1)))))
  {
    y = (rtNaN);
  }
  else if ((u1 != 0.0) && (u1 != trunc(u1)))
  {
    q = fabs(u0 / u1);
    if (fabs(q - floor(q + 0.5)) <= DBL_EPSILON * q)
    {
      y = 0.0 * u0;
    }
    else
    {
      y = fmod(u0, u1);
    }
  }
  else
  {
    y = fmod(u0, u1);
  }

  return y;
}

/*
 * Function for Chart: '<S1>/TaskController'
 * function Vertice = VerticeSelect(AMP, BLU, BLD, BRU, BRD)
 *     %% Функция определения точки входа в "посадочной коробке"
 */
static uint8_T VerticeSelect(const real_T AMP[3], const real_T BLU[3], const
  real_T BLD[3], const real_T BRU[3], const real_T BRD[3])
{
  real_T x[4];
  int32_T b_Vertice;
  real_T fi2;
  real_T b_fi2;
  real_T c_fi2;
  real_T d_fi2;
  real_T fi1_tmp;
  real_T x_tmp;

  /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
  /*   VerticeSelect - Выбор оптимального входа в манёвр "коробка" */
  /*  */
  /*   AMP - Координаты точки, от которой расчитывается вход */
  /*   BLU - Координаты точки левого верхнего угла коробки */
  /*   BLD - Координаты точки левого нижнего угла коробки  */
  /*   BRU - Координаты точки правого верхнего угла коробки  */
  /*   BRD - Координаты точки нижнего верхнего угла коробки */
  /*  */
  /*   Возвращает номер вершины оптимального входа */
  /*       1 - BLU, 2 - BLD, 3 - BRU, 4 - BRD */
  /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
  /* 'VerticeSelect:15' D_BLU = DistanceCalculator(AMP,BLU); */
  /*     %% Вычитаем расстояние (через вектора) */
  /*  dif = (TDP - ActualPoint); */
  /*  % Вычисляем проекции на оси с переводом в Км. */
  /*  distance_lon2x = dif(1)*40000/360*cos(dif(2)*pi/180); */
  /*  distance_lat2y = dif(2)*40000/360; */
  /*  distance = sqrt(distance_lon2x^2 + distance_lat2y^2); */
  /*     %% Вычитаем расстояние (через тригонометрию) */
  /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
  /*  DistanceCalculator - Вычисление расстрояния от Position до Target  */
  /*  в метрах по сферическим координатам */
  /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
  /* 'DistanceCalculator:13' la1 = Position(2)*pi/180; */
  /* 'DistanceCalculator:14' fi1 = Position(1)*pi/180; */
  fi1_tmp = AMP[0] * 3.1415926535897931 / 180.0;

  /* 'DistanceCalculator:15' la2 = Target(2)*pi/180; */
  /* 'DistanceCalculator:16' fi2 = Target(1)*pi/180; */
  fi2 = BLU[0] * 3.1415926535897931 / 180.0;

  /* 'DistanceCalculator:17' d = sin(fi1)*sin(fi2) + cos(fi1)*cos(-fi2)*cos(la2-la1); */
  /*  6371 км средний радиус земли */
  /* 'DistanceCalculator:19' y = acos(d)*6378245; */
  /* 'VerticeSelect:16' D_BLD = DistanceCalculator(AMP,BLD); */
  /*     %% Вычитаем расстояние (через вектора) */
  /*  dif = (TDP - ActualPoint); */
  /*  % Вычисляем проекции на оси с переводом в Км. */
  /*  distance_lon2x = dif(1)*40000/360*cos(dif(2)*pi/180); */
  /*  distance_lat2y = dif(2)*40000/360; */
  /*  distance = sqrt(distance_lon2x^2 + distance_lat2y^2); */
  /*     %% Вычитаем расстояние (через тригонометрию) */
  /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
  /*  DistanceCalculator - Вычисление расстрояния от Position до Target  */
  /*  в метрах по сферическим координатам */
  /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
  /* 'DistanceCalculator:13' la1 = Position(2)*pi/180; */
  /* 'DistanceCalculator:14' fi1 = Position(1)*pi/180; */
  /* 'DistanceCalculator:15' la2 = Target(2)*pi/180; */
  /* 'DistanceCalculator:16' fi2 = Target(1)*pi/180; */
  b_fi2 = BLD[0] * 3.1415926535897931 / 180.0;

  /* 'DistanceCalculator:17' d = sin(fi1)*sin(fi2) + cos(fi1)*cos(-fi2)*cos(la2-la1); */
  /*  6371 км средний радиус земли */
  /* 'DistanceCalculator:19' y = acos(d)*6378245; */
  /* 'VerticeSelect:17' D_BRU = DistanceCalculator(AMP,BRU); */
  /*     %% Вычитаем расстояние (через вектора) */
  /*  dif = (TDP - ActualPoint); */
  /*  % Вычисляем проекции на оси с переводом в Км. */
  /*  distance_lon2x = dif(1)*40000/360*cos(dif(2)*pi/180); */
  /*  distance_lat2y = dif(2)*40000/360; */
  /*  distance = sqrt(distance_lon2x^2 + distance_lat2y^2); */
  /*     %% Вычитаем расстояние (через тригонометрию) */
  /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
  /*  DistanceCalculator - Вычисление расстрояния от Position до Target  */
  /*  в метрах по сферическим координатам */
  /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
  /* 'DistanceCalculator:13' la1 = Position(2)*pi/180; */
  /* 'DistanceCalculator:14' fi1 = Position(1)*pi/180; */
  /* 'DistanceCalculator:15' la2 = Target(2)*pi/180; */
  /* 'DistanceCalculator:16' fi2 = Target(1)*pi/180; */
  c_fi2 = BRU[0] * 3.1415926535897931 / 180.0;

  /* 'DistanceCalculator:17' d = sin(fi1)*sin(fi2) + cos(fi1)*cos(-fi2)*cos(la2-la1); */
  /*  6371 км средний радиус земли */
  /* 'DistanceCalculator:19' y = acos(d)*6378245; */
  /* 'VerticeSelect:18' D_BRD = DistanceCalculator(AMP,BRD); */
  /*     %% Вычитаем расстояние (через вектора) */
  /*  dif = (TDP - ActualPoint); */
  /*  % Вычисляем проекции на оси с переводом в Км. */
  /*  distance_lon2x = dif(1)*40000/360*cos(dif(2)*pi/180); */
  /*  distance_lat2y = dif(2)*40000/360; */
  /*  distance = sqrt(distance_lon2x^2 + distance_lat2y^2); */
  /*     %% Вычитаем расстояние (через тригонометрию) */
  /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
  /*  DistanceCalculator - Вычисление расстрояния от Position до Target  */
  /*  в метрах по сферическим координатам */
  /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
  /* 'DistanceCalculator:13' la1 = Position(2)*pi/180; */
  /* 'DistanceCalculator:14' fi1 = Position(1)*pi/180; */
  /* 'DistanceCalculator:15' la2 = Target(2)*pi/180; */
  /* 'DistanceCalculator:16' fi2 = Target(1)*pi/180; */
  d_fi2 = BRD[0] * 3.1415926535897931 / 180.0;

  /* 'DistanceCalculator:17' d = sin(fi1)*sin(fi2) + cos(fi1)*cos(-fi2)*cos(la2-la1); */
  /*  6371 км средний радиус земли */
  /* 'DistanceCalculator:19' y = acos(d)*6378245; */
  /*     %% Соответсвие значения Vertice и вершин коробки */
  /*  1 - Левая верхняя */
  /*  2 - Левая нижняя */
  /*  3 - Правая верхняя */
  /*  4 - Правая нижняя */
  /* 'VerticeSelect:24' x = [D_BLU, D_BLD, D_BRU, D_BRD]; */
  x_tmp = AMP[1] * 3.1415926535897931 / 180.0;
  x[0] = acos(cos(BLU[1] * 3.1415926535897931 / 180.0 - x_tmp) * (cos(fi1_tmp) *
    cos(-fi2)) + sin(fi1_tmp) * sin(fi2)) * 6.378245E+6;
  x[1] = acos(cos(BLD[1] * 3.1415926535897931 / 180.0 - x_tmp) * (cos(fi1_tmp) *
    cos(-b_fi2)) + sin(fi1_tmp) * sin(b_fi2)) * 6.378245E+6;
  x[2] = acos(cos(BRU[1] * 3.1415926535897931 / 180.0 - x_tmp) * (cos(fi1_tmp) *
    cos(-c_fi2)) + sin(fi1_tmp) * sin(c_fi2)) * 6.378245E+6;
  x[3] = acos(cos(BRD[1] * 3.1415926535897931 / 180.0 - x_tmp) * (cos(fi1_tmp) *
    cos(-d_fi2)) + sin(fi1_tmp) * sin(d_fi2)) * 6.378245E+6;

  /* 'VerticeSelect:25' Vertice = 1; */
  b_Vertice = 0;

  /* 'VerticeSelect:26' for i = 1:1:3 */
  /* 'VerticeSelect:27' if x(i+1) < x(Vertice) */
  if (x[1] < x[0])
  {
    /* 'VerticeSelect:28' Vertice = i+1; */
    b_Vertice = 1;
  }

  /* 'VerticeSelect:27' if x(i+1) < x(Vertice) */
  if (x[2] < x[b_Vertice])
  {
    /* 'VerticeSelect:28' Vertice = i+1; */
    b_Vertice = 2;
  }

  /* 'VerticeSelect:27' if x(i+1) < x(Vertice) */
  if (x[3] < x[b_Vertice])
  {
    /* 'VerticeSelect:28' Vertice = i+1; */
    b_Vertice = 3;
  }

  /* 'VerticeSelect:31' Vertice = uint8(Vertice); */
  return (uint8_T)(b_Vertice + 1);
}

/* Function for Chart: '<S1>/TaskController' */
static void enter_internal_Box(void)
{
  /* Entry Internal 'Box': '<S7>:242' */
  switch (rtDW.was_Box)
  {
   case IN_Init_n:
    if (rtDW.is_Box != IN_Init_n)
    {
      rtDW.is_Box = IN_Init_n;
      rtDW.was_Box = IN_Init_n;

      /* Entry 'Init': '<S7>:393' */
      /* '<S7>:393:1' BoxEntry = VerticeSelect(ActualPoint, LUPoint, LDPoint, RUPoint, RDPoint); */
      rtDW.BoxEntry = VerticeSelect(rtDW.TmpSignalConversionAtSFunctio_g,
        rtDW.LUPoint, rtDW.LDPoint, rtDW.RUPoint, rtDW.RDPoint);
    }
    break;

   case IN_LD:
    if (rtDW.is_Box != IN_LD)
    {
      rtDW.is_Box = IN_LD;
      rtDW.was_Box = IN_LD;

      /* Entry 'LD': '<S7>:244' */
      /* '<S7>:244:1' TargetPoint = LDPoint; */
      rtDW.TargetPoint[0] = rtDW.LDPoint[0];
      rtDW.TargetPoint[1] = rtDW.LDPoint[1];
      rtDW.TargetPoint[2] = rtDW.LDPoint[2];
    }
    break;

   case IN_LU:
    if (rtDW.is_Box != IN_LU)
    {
      rtDW.is_Box = IN_LU;
      rtDW.was_Box = IN_LU;

      /* Entry 'LU': '<S7>:246' */
      /* '<S7>:246:1' TargetPoint = LUPoint; */
      rtDW.TargetPoint[0] = rtDW.LUPoint[0];
      rtDW.TargetPoint[1] = rtDW.LUPoint[1];
      rtDW.TargetPoint[2] = rtDW.LUPoint[2];
    }
    break;

   case IN_RD:
    if (rtDW.is_Box != IN_RD)
    {
      rtDW.is_Box = IN_RD;
      rtDW.was_Box = IN_RD;

      /* Entry 'RD': '<S7>:248' */
      /* '<S7>:248:1' TargetPoint = RDPoint; */
      rtDW.TargetPoint[0] = rtDW.RDPoint[0];
      rtDW.TargetPoint[1] = rtDW.RDPoint[1];
      rtDW.TargetPoint[2] = rtDW.RDPoint[2];
    }
    break;

   case IN_RU:
    if (rtDW.is_Box != IN_RU)
    {
      rtDW.is_Box = IN_RU;
      rtDW.was_Box = IN_RU;

      /* Entry 'RU': '<S7>:249' */
      /* '<S7>:249:1' TargetPoint = RUPoint; */
      rtDW.TargetPoint[0] = rtDW.RUPoint[0];
      rtDW.TargetPoint[1] = rtDW.RUPoint[1];
      rtDW.TargetPoint[2] = rtDW.RUPoint[2];
    }
    break;

   default:
    /* Transition: '<S7>:394' */
    if (rtDW.is_Box != IN_Init_n)
    {
      rtDW.is_Box = IN_Init_n;
      rtDW.was_Box = IN_Init_n;

      /* Entry 'Init': '<S7>:393' */
      /* '<S7>:393:1' BoxEntry = VerticeSelect(ActualPoint, LUPoint, LDPoint, RUPoint, RDPoint); */
      rtDW.BoxEntry = VerticeSelect(rtDW.TmpSignalConversionAtSFunctio_g,
        rtDW.LUPoint, rtDW.LDPoint, rtDW.RUPoint, rtDW.RDPoint);
    }
    break;
  }
}

/* Function for Chart: '<S1>/TaskController' */
static void BimSupply(void)
{
  /* During 'BimSupply': '<S7>:452' */
  switch (rtDW.is_BimSupply)
  {
   case IN_Disable:
    /* During 'Disable': '<S7>:458' */
    /* '<S7>:706:1' sf_internal_predicateOutput = ... */
    /* '<S7>:706:1' Parking; */
    switch (rtDW.sfEvent)
    {
     case event_Parking:
      /* Transition: '<S7>:706' */
      rtDW.is_BimSupply = IN_Shutdown;

      /* Entry 'Shutdown': '<S7>:461' */
      /* '<S7>:461:1' TargetEnable = 0; */
      rtDW.TargetEnable = 0U;
      break;

     case event_NonBlockingMode:
      /* '<S7>:464:1' sf_internal_predicateOutput = ... */
      /* '<S7>:464:1' NonBlockingMode; */
      /* Transition: '<S7>:464' */
      rtDW.is_BimSupply = IN_NonBlockingControl;

      /* Entry 'NonBlockingControl': '<S7>:484' */
      /* '<S7>:484:1' TargetEnable = 1; */
      rtDW.TargetEnable = 1U;
      break;

     default:
      /* '<S7>:622:1' sf_internal_predicateOutput = ... */
      /* '<S7>:622:1' wantEnable; */
      if ((rtDW.sfEvent == event_wantEnable) || (rtDW.timeout_g == 0U))
      {
        /* Transition: '<S7>:622' */
        /* Transition: '<S7>:460' */
        /* Transition: '<S7>:621' */
        rtDW.is_BimSupply = IN_Enable;

        /* Entry 'Enable': '<S7>:456' */
        /* '<S7>:456:1' TargetEnable = 1; */
        rtDW.TargetEnable = 1U;
      }
      else
      {
        /* '<S7>:460:1' sf_internal_predicateOutput = ... */
        /* '<S7>:460:1' timeout == 0; */
      }
      break;
    }
    break;

   case IN_Enable:
    /* During 'Enable': '<S7>:456' */
    /* '<S7>:459:1' sf_internal_predicateOutput = ... */
    /* '<S7>:459:1' wantDisable; */
    switch (rtDW.sfEvent)
    {
     case event_wantDisable:
      /* Transition: '<S7>:459' */
      rtDW.is_BimSupply = IN_Disable;

      /* Entry 'Disable': '<S7>:458' */
      /* '<S7>:458:1' TargetEnable = 0; */
      rtDW.TargetEnable = 0U;

      /* '<S7>:458:1' timeout = BimTimeout; */
      rtDW.timeout_g = rtDW.BimTimeout;
      break;

     case event_NonBlockingMode:
      /* '<S7>:465:1' sf_internal_predicateOutput = ... */
      /* '<S7>:465:1' NonBlockingMode; */
      /* Transition: '<S7>:465' */
      rtDW.is_BimSupply = IN_NonBlockingControl;

      /* Entry 'NonBlockingControl': '<S7>:484' */
      /* '<S7>:484:1' TargetEnable = 1; */
      rtDW.TargetEnable = 1U;
      break;

     case event_Parking:
      /* '<S7>:705:1' sf_internal_predicateOutput = ... */
      /* '<S7>:705:1' Parking; */
      /* Transition: '<S7>:705' */
      rtDW.is_BimSupply = IN_Shutdown;

      /* Entry 'Shutdown': '<S7>:461' */
      /* '<S7>:461:1' TargetEnable = 0; */
      rtDW.TargetEnable = 0U;
      break;
    }
    break;

   case IN_Init_n0:
    /* During 'Init': '<S7>:603' */
    /* Transition: '<S7>:605' */
    rtDW.is_BimSupply = IN_Enable;

    /* Entry 'Enable': '<S7>:456' */
    /* '<S7>:456:1' TargetEnable = 1; */
    rtDW.TargetEnable = 1U;
    break;

   case IN_NonBlockingControl:
    /* During 'NonBlockingControl': '<S7>:484' */
    /* '<S7>:485:1' sf_internal_predicateOutput = ... */
    /* '<S7>:485:1' Parking; */
    if (rtDW.sfEvent == event_Parking)
    {
      /* Transition: '<S7>:485' */
      rtDW.is_BimSupply = IN_Shutdown;

      /* Entry 'Shutdown': '<S7>:461' */
      /* '<S7>:461:1' TargetEnable = 0; */
      rtDW.TargetEnable = 0U;
    }
    else
    {
      /* '<S7>:484:1' TargetEnable = 1; */
      rtDW.TargetEnable = 1U;
    }
    break;

   case IN_Shutdown:
    /* During 'Shutdown': '<S7>:461' */
    /* '<S7>:461:1' TargetEnable = 0; */
    rtDW.TargetEnable = 0U;
    break;
  }
}

/*
 * Function for Chart: '<S1>/TaskController'
 * function coefficient = nonlinearCoefficientFunction(distance)
 */
static real_T nonlinearCoefficientFunction(real_T distance)
{
  real_T coefficient;

  /* MATLAB Function 'nonlinearCoefficientFunction': '<S7>:701' */
  /* '<S7>:701:2' coefficient = 3*exp(-distance/2.5e3); */
  coefficient = exp(-distance / 2500.0) * 3.0;

  /*      if coefficient > 1.5 */
  /*          coefficient = 1.5; */
  /*      end */
  return coefficient;
}

/*
 * Function for Chart: '<S1>/TaskController'
 * function course = Angle2Course(angle)
 */
static real_T Angle2Course(real_T angle)
{
  real_T course;

  /* MATLAB Function 'Angle2Course': '<S7>:690' */
  /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
  /*   */
  /*  Angle2Course - Преобразование курса к используемому в модели виду */
  /*  */
  /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
  /* '<S7>:690:7' course = angle; */
  course = angle;

  /* '<S7>:690:8' if angle < 0 */
  if (angle < 0.0)
  {
    /* '<S7>:690:9' course = course + 2*pi; */
    course = angle + 6.2831853071795862;
  }

  return course;
}

real_T rt_roundd_snf(real_T u)
{
  real_T y;
  if (fabs(u) < 4.503599627370496E+15)
  {
    if (u >= 0.5)
    {
      y = floor(u + 0.5);
    }
    else if (u > -0.5)
    {
      y = u * 0.0;
    }
    else
    {
      y = ceil(u - 0.5);
    }
  }
  else
  {
    y = u;
  }

  return y;
}

/* Function for Chart: '<S1>/TaskController' */
static void Flight(const real_T *AngleManevr, const uint32_T *AngleTimeout)
{
  real_T fi1;
  real_T fi2;
  real_T delta_la;
  int32_T b_previousEvent;

  /* During 'Flight': '<S7>:23' */
  /* '<S7>:23:1' Course2Target = Azimut(TargetPoint + TDPOfset, ActualPoint); */
  /*  u0 - точка отсчета */
  /*  u1 - конечная точка */
  /*  la - долгота */
  /*  fi - широта */
  /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
  /*  */
  /*    Azimut - Вычисление курса от точки u0 до точки u1 */
  /*  */
  /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
  /* 'Azimut:11' la1 = u0(2)*pi/180; */
  /* 'Azimut:12' fi1 = u0(1)*pi/180; */
  fi1 = rtDW.TmpSignalConversionAtSFunctio_g[0] * 3.1415926535897931 / 180.0;

  /* 'Azimut:13' la2 = u1(2)*pi/180; */
  /* 'Azimut:14' fi2 = u1(1)*pi/180; */
  fi2 = (rtDW.TargetPoint[0] + rtDW.TDPOfset[0]) * 3.1415926535897931 / 180.0;

  /* 'Azimut:15' delta_la = la2-la1; */
  delta_la = (rtDW.TargetPoint[1] + rtDW.TDPOfset[1]) * 3.1415926535897931 /
    180.0 - rtDW.TmpSignalConversionAtSFunctio_g[1] * 3.1415926535897931 / 180.0;

  /* 'Azimut:16' y = atan2(sin(delta_la)*cos(fi2), cos(fi1)*sin(fi2)-sin(fi1)*cos(fi2)*cos(delta_la)); */
  rtDW.Course2Target = rt_atan2d_snf(sin(delta_la) * cos(fi2), cos(fi1) * sin
    (fi2) - sin(fi1) * cos(fi2) * cos(delta_la));

  /* 'Azimut:17' y = rem((y + 2*pi), 2*pi); */
  rtDW.Course2Target = rt_remd_snf(rtDW.Course2Target + 6.2831853071795862,
    6.2831853071795862);

  /* Course2Target = Heading(TargetPoint + TDPOfset, ActualPoint, 0); */
  switch (rtDW.is_Flight)
  {
   case IN_Angle:
    /* During 'Angle': '<S7>:82' */
    /* '<S7>:89:1' sf_internal_predicateOutput = ... */
    /* '<S7>:89:1' NextStage; */
    switch (rtDW.sfEvent)
    {
     case event_NextStage:
      /* Transition: '<S7>:89' */
      rtDW.is_Flight = IN_Box;
      rtDW.was_Flight = IN_Box;

      /* Entry 'Box': '<S7>:242' */
      /*  LU - Нацеливание на верхний левый угол "коробки" */
      /*  LD - Нацеливание на нижний левый угол "коробки" */
      /*  RD - Нацеливание на нижний правый угол "коробки" */
      /*  RU - Нацеливание на верхний правый угол "коробки" */
      enter_internal_Box();
      break;

     case event_Final:
      /* '<S7>:258:1' sf_internal_predicateOutput = ... */
      /* '<S7>:258:1' Final; */
      /* Transition: '<S7>:258' */
      rtDW.is_Flight = IN_Final;
      rtDW.was_Flight = IN_Final;

      /* Entry 'Final': '<S7>:94' */
      /*  Состояние последнего манёвра */
      /*  Цель которого выйти на заданную */
      /*  точку приземления */
      /* '<S7>:94:3' BimSupply.wantEnable; */
      /* Event: '<S7>:606' */
      b_previousEvent = rtDW.sfEvent;
      rtDW.sfEvent = event_wantEnable;
      if (rtDW.is_active_BimSupply != 0U)
      {
        BimSupply();
      }

      rtDW.sfEvent = b_previousEvent;

      /* '<S7>:94:4' TargetPoint = TDPoint */
      rtDW.TargetPoint[0] = rtDW.TmpSignalConversionAtSFunctionI[0];
      rtDW.TargetPoint[1] = rtDW.TmpSignalConversionAtSFunctionI[1];
      rtDW.TargetPoint[2] = rtDW.TmpSignalConversionAtSFunctionI[2];
      break;
    }
    break;

   case IN_Box:
    /* During 'Box': '<S7>:242' */
    /* '<S7>:108:1' sf_internal_predicateOutput = ... */
    /* '<S7>:108:1' Final; */
    if (rtDW.sfEvent == event_Final)
    {
      /* Transition: '<S7>:108' */
      /* Exit Internal 'Box': '<S7>:242' */
      rtDW.is_Box = 0;
      rtDW.is_Flight = IN_Final;
      rtDW.was_Flight = IN_Final;

      /* Entry 'Final': '<S7>:94' */
      /*  Состояние последнего манёвра */
      /*  Цель которого выйти на заданную */
      /*  точку приземления */
      /* '<S7>:94:3' BimSupply.wantEnable; */
      /* Event: '<S7>:606' */
      b_previousEvent = rtDW.sfEvent;
      rtDW.sfEvent = event_wantEnable;
      if (rtDW.is_active_BimSupply != 0U)
      {
        BimSupply();
      }

      rtDW.sfEvent = b_previousEvent;

      /* '<S7>:94:4' TargetPoint = TDPoint */
      rtDW.TargetPoint[0] = rtDW.TmpSignalConversionAtSFunctionI[0];
      rtDW.TargetPoint[1] = rtDW.TmpSignalConversionAtSFunctionI[1];
      rtDW.TargetPoint[2] = rtDW.TmpSignalConversionAtSFunctionI[2];
    }
    else
    {
      /*  LU - Нацеливание на верхний левый угол "коробки" */
      /*  LD - Нацеливание на нижний левый угол "коробки" */
      /*  RD - Нацеливание на нижний правый угол "коробки" */
      /*  RU - Нацеливание на верхний правый угол "коробки" */
      switch (rtDW.is_Box)
      {
       case IN_Init_n:
        /* During 'Init': '<S7>:393' */
        /* Transition: '<S7>:256' */
        /* '<S7>:251:1' sf_internal_predicateOutput = ... */
        /* '<S7>:251:1' BoxEntry == 2; */
        switch (rtDW.BoxEntry)
        {
         case 2:
          /* Transition: '<S7>:251' */
          rtDW.is_Box = IN_LD;
          rtDW.was_Box = IN_LD;

          /* Entry 'LD': '<S7>:244' */
          /* '<S7>:244:1' TargetPoint = LDPoint; */
          rtDW.TargetPoint[0] = rtDW.LDPoint[0];
          rtDW.TargetPoint[1] = rtDW.LDPoint[1];
          rtDW.TargetPoint[2] = rtDW.LDPoint[2];
          break;

         case 1:
          /* '<S7>:243:1' sf_internal_predicateOutput = ... */
          /* '<S7>:243:1' BoxEntry == 1; */
          /* Transition: '<S7>:243' */
          rtDW.is_Box = IN_LU;
          rtDW.was_Box = IN_LU;

          /* Entry 'LU': '<S7>:246' */
          /* '<S7>:246:1' TargetPoint = LUPoint; */
          rtDW.TargetPoint[0] = rtDW.LUPoint[0];
          rtDW.TargetPoint[1] = rtDW.LUPoint[1];
          rtDW.TargetPoint[2] = rtDW.LUPoint[2];
          break;

         case 4:
          /* '<S7>:255:1' sf_internal_predicateOutput = ... */
          /* '<S7>:255:1' BoxEntry == 4; */
          /* Transition: '<S7>:255' */
          rtDW.is_Box = IN_RD;
          rtDW.was_Box = IN_RD;

          /* Entry 'RD': '<S7>:248' */
          /* '<S7>:248:1' TargetPoint = RDPoint; */
          rtDW.TargetPoint[0] = rtDW.RDPoint[0];
          rtDW.TargetPoint[1] = rtDW.RDPoint[1];
          rtDW.TargetPoint[2] = rtDW.RDPoint[2];
          break;

         case 3:
          /* '<S7>:253:1' sf_internal_predicateOutput = ... */
          /* '<S7>:253:1' BoxEntry == 3; */
          /* Transition: '<S7>:253' */
          rtDW.is_Box = IN_RU;
          rtDW.was_Box = IN_RU;

          /* Entry 'RU': '<S7>:249' */
          /* '<S7>:249:1' TargetPoint = RUPoint; */
          rtDW.TargetPoint[0] = rtDW.RUPoint[0];
          rtDW.TargetPoint[1] = rtDW.RUPoint[1];
          rtDW.TargetPoint[2] = rtDW.RUPoint[2];
          break;

         default:
          /* '<S7>:393:1' BoxEntry = VerticeSelect(ActualPoint, LUPoint, LDPoint, RUPoint, RDPoint); */
          rtDW.BoxEntry = VerticeSelect(rtDW.TmpSignalConversionAtSFunctio_g,
            rtDW.LUPoint, rtDW.LDPoint, rtDW.RUPoint, rtDW.RDPoint);
          break;
        }
        break;

       case IN_LD:
        /* During 'LD': '<S7>:244' */
        /* '<S7>:250:1' sf_internal_predicateOutput = ... */
        /* '<S7>:250:1' NextStage; */
        if (rtDW.sfEvent == event_NextStage)
        {
          /* Transition: '<S7>:250' */
          rtDW.is_Box = IN_RD;
          rtDW.was_Box = IN_RD;

          /* Entry 'RD': '<S7>:248' */
          /* '<S7>:248:1' TargetPoint = RDPoint; */
          rtDW.TargetPoint[0] = rtDW.RDPoint[0];
          rtDW.TargetPoint[1] = rtDW.RDPoint[1];
          rtDW.TargetPoint[2] = rtDW.RDPoint[2];
        }
        break;

       case IN_LU:
        /* During 'LU': '<S7>:246' */
        /* '<S7>:254:1' sf_internal_predicateOutput = ... */
        /* '<S7>:254:1' NextStage; */
        if (rtDW.sfEvent == event_NextStage)
        {
          /* Transition: '<S7>:254' */
          rtDW.is_Box = IN_LD;
          rtDW.was_Box = IN_LD;

          /* Entry 'LD': '<S7>:244' */
          /* '<S7>:244:1' TargetPoint = LDPoint; */
          rtDW.TargetPoint[0] = rtDW.LDPoint[0];
          rtDW.TargetPoint[1] = rtDW.LDPoint[1];
          rtDW.TargetPoint[2] = rtDW.LDPoint[2];
        }
        break;

       case IN_RD:
        /* During 'RD': '<S7>:248' */
        /* '<S7>:245:1' sf_internal_predicateOutput = ... */
        /* '<S7>:245:1' NextStage; */
        if (rtDW.sfEvent == event_NextStage)
        {
          /* Transition: '<S7>:245' */
          rtDW.is_Box = IN_RU;
          rtDW.was_Box = IN_RU;

          /* Entry 'RU': '<S7>:249' */
          /* '<S7>:249:1' TargetPoint = RUPoint; */
          rtDW.TargetPoint[0] = rtDW.RUPoint[0];
          rtDW.TargetPoint[1] = rtDW.RUPoint[1];
          rtDW.TargetPoint[2] = rtDW.RUPoint[2];
        }
        break;

       case IN_RU:
        /* During 'RU': '<S7>:249' */
        /* '<S7>:252:1' sf_internal_predicateOutput = ... */
        /* '<S7>:252:1' NextStage; */
        if (rtDW.sfEvent == event_NextStage)
        {
          /* Transition: '<S7>:252' */
          rtDW.is_Box = IN_LU;
          rtDW.was_Box = IN_LU;

          /* Entry 'LU': '<S7>:246' */
          /* '<S7>:246:1' TargetPoint = LUPoint; */
          rtDW.TargetPoint[0] = rtDW.LUPoint[0];
          rtDW.TargetPoint[1] = rtDW.LUPoint[1];
          rtDW.TargetPoint[2] = rtDW.LUPoint[2];
        }
        break;
      }
    }
    break;

   case IN_Final:
    /* During 'Final': '<S7>:94' */
    /*  Состояние последнего манёвра */
    /*  Цель которого выйти на заданную */
    /*  точку приземления */
    /* '<S7>:94:3' BimSupply.wantEnable; */
    /* Event: '<S7>:606' */
    b_previousEvent = rtDW.sfEvent;
    rtDW.sfEvent = event_wantEnable;
    if (rtDW.is_active_BimSupply != 0U)
    {
      BimSupply();
    }

    rtDW.sfEvent = b_previousEvent;

    /* '<S7>:94:3' TargetPoint = TDPoint */
    rtDW.TargetPoint[0] = rtDW.TmpSignalConversionAtSFunctionI[0];
    rtDW.TargetPoint[1] = rtDW.TmpSignalConversionAtSFunctionI[1];
    rtDW.TargetPoint[2] = rtDW.TmpSignalConversionAtSFunctionI[2];
    break;

   case IN_StepAngle:
    /* Outport: '<Root>/DistanceToTDP' */
    /* During 'StepAngle': '<S7>:582' */
    /* '<S7>:582:1' coeff = nonlinearCoefficientFunction(Distance); */
    fi1 = nonlinearCoefficientFunction(rtY.DistanceToTDP);
    switch (rtDW.is_StepAngle)
    {
     case IN_Left:
      /* During 'Left': '<S7>:592' */
      /* '<S7>:589:1' sf_internal_predicateOutput = ... */
      /* '<S7>:589:1' timeout == 0; */
      if (rtDW.timeout == 0U)
      {
        /* Transition: '<S7>:589' */
        /* '<S7>:589:1' timeout = AngleTimeout; */
        rtDW.timeout = *AngleTimeout;

        /* Transition: '<S7>:626' */
        if (rtDW.is_StepAngle == IN_Left)
        {
          /* Exit 'Left': '<S7>:592' */
          /* '<S7>:592:3' LeftOrRight = true; */
          rtDW.LeftOrRight = true;
          rtDW.is_StepAngle = 0;
        }

        if (rtDW.is_StepAngle != IN_Wait)
        {
          rtDW.is_StepAngle = IN_Wait;

          /* Entry 'Wait': '<S7>:591' */
          /* '<S7>:591:1' BimSupply.wantEnable */
          /* Event: '<S7>:606' */
          b_previousEvent = rtDW.sfEvent;
          rtDW.sfEvent = event_wantEnable;
          if (rtDW.is_active_BimSupply != 0U)
          {
            BimSupply();
          }

          rtDW.sfEvent = b_previousEvent;

          /* '<S7>:591:1' waiting = true; */
          rtDW.waiting = true;

          /* '<S7>:591:3' justReturnedHere = true; */
          rtDW.justReturnedHere = true;
        }
      }
      else
      {
        /* '<S7>:592:4' Course2Target = Angle2Course(Course2Target-AngleManevr); */
        rtDW.Course2Target = Angle2Course(rtDW.Course2Target - *AngleManevr);
      }
      break;

     case IN_Right:
      /* During 'Right': '<S7>:590' */
      /* '<S7>:585:1' sf_internal_predicateOutput = ... */
      /* '<S7>:585:1' timeout == 0; */
      if (rtDW.timeout == 0U)
      {
        /* Transition: '<S7>:585' */
        /* '<S7>:585:1' timeout = AngleTimeout; */
        rtDW.timeout = *AngleTimeout;

        /* Transition: '<S7>:629' */
        if (rtDW.is_StepAngle == IN_Right)
        {
          /* Exit 'Right': '<S7>:590' */
          /* '<S7>:590:3' LeftOrRight = false; */
          rtDW.LeftOrRight = false;
          rtDW.is_StepAngle = 0;
        }

        if (rtDW.is_StepAngle != IN_Wait)
        {
          rtDW.is_StepAngle = IN_Wait;

          /* Entry 'Wait': '<S7>:591' */
          /* '<S7>:591:1' BimSupply.wantEnable */
          /* Event: '<S7>:606' */
          b_previousEvent = rtDW.sfEvent;
          rtDW.sfEvent = event_wantEnable;
          if (rtDW.is_active_BimSupply != 0U)
          {
            BimSupply();
          }

          rtDW.sfEvent = b_previousEvent;

          /* '<S7>:591:1' waiting = true; */
          rtDW.waiting = true;

          /* '<S7>:591:3' justReturnedHere = true; */
          rtDW.justReturnedHere = true;
        }
      }
      else
      {
        /* '<S7>:590:4' Course2Target = Angle2Course(Course2Target+AngleManevr); */
        rtDW.Course2Target = Angle2Course(rtDW.Course2Target + *AngleManevr);
      }
      break;

     case IN_Wait:
      /* Outport: '<Root>/TimeToTD' incorporates:
       *  Outport: '<Root>/TimeToHorTarget'
       */
      /* During 'Wait': '<S7>:591' */
      /* '<S7>:587:1' sf_internal_predicateOutput = ... */
      /* '<S7>:587:1' (T_ver-T_hor) > 1  && timeout == 0; */
      if ((rtY.TimeToTD - rtY.TimeToHorTarget > 1.0) && (rtDW.timeout == 0U))
      {
        /* Transition: '<S7>:587' */
        /* '<S7>:586:1' sf_internal_predicateOutput = ... */
        /* '<S7>:586:1' LeftOrRight; */
        if (rtDW.LeftOrRight)
        {
          /* Transition: '<S7>:586' */
          rtDW.is_StepAngle = IN_Right;

          /* Entry 'Right': '<S7>:590' */
          /* '<S7>:590:1' timeout = uint32((T_ver - T_hor)*coeff) */
          fi1 = rt_roundd_snf((rtY.TimeToTD - rtY.TimeToHorTarget) * fi1);
          if (fi1 < 4.294967296E+9)
          {
            if (fi1 >= 0.0)
            {
              rtDW.timeout = (uint32_T)fi1;
            }
            else
            {
              rtDW.timeout = 0U;
            }
          }
          else
          {
            rtDW.timeout = MAX_uint32_T;
          }

          /* '<S7>:590:3' waiting = false; */
          rtDW.waiting = false;

          /* '<S7>:590:3' justReturnedHere = false; */
          rtDW.justReturnedHere = false;

          /* '<S7>:590:4' BimSupply.wantEnable */
          /* Event: '<S7>:606' */
          b_previousEvent = rtDW.sfEvent;
          rtDW.sfEvent = event_wantEnable;
          if (rtDW.is_active_BimSupply != 0U)
          {
            BimSupply();
          }

          rtDW.sfEvent = b_previousEvent;
        }
        else
        {
          /* '<S7>:588:1' sf_internal_predicateOutput = ... */
          /* '<S7>:588:1' ~LeftOrRight; */
          /* Transition: '<S7>:588' */
          rtDW.is_StepAngle = IN_Left;

          /* Entry 'Left': '<S7>:592' */
          /* '<S7>:592:1' timeout = uint32((T_ver - T_hor)*coeff) */
          fi1 = rt_roundd_snf((rtY.TimeToTD - rtY.TimeToHorTarget) * fi1);
          if (fi1 < 4.294967296E+9)
          {
            if (fi1 >= 0.0)
            {
              rtDW.timeout = (uint32_T)fi1;
            }
            else
            {
              rtDW.timeout = 0U;
            }
          }
          else
          {
            rtDW.timeout = MAX_uint32_T;
          }

          /* '<S7>:592:3' waiting = false; */
          rtDW.waiting = false;

          /* '<S7>:592:3' justReturnedHere = false; */
          rtDW.justReturnedHere = false;

          /* '<S7>:592:4' BimSupply.wantEnable */
          /* Event: '<S7>:606' */
          b_previousEvent = rtDW.sfEvent;
          rtDW.sfEvent = event_wantEnable;
          if (rtDW.is_active_BimSupply != 0U)
          {
            BimSupply();
          }

          rtDW.sfEvent = b_previousEvent;
        }
      }

      /* End of Outport: '<Root>/TimeToTD' */
      break;
    }
    break;
  }
}

/*
 * Function for Chart: '<S1>/TaskController'
 * function y = Heading(u1, u0, last_y)
 *     %% Высчитаем, как изменились координаты
 */
static real_T Heading(const real_T u1[3], const real_T u0[3], real_T last_y)
{
  real_T y;
  real_T un_pow1;
  real_T un_pow2;
  real_T un_idx_1;
  real_T un_idx_0;

  /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
  /*  */
  /*    Heading - Вычисление курса */
  /*    u0 - Координата точки относительно, которой вычисляем */
  /*    u1 - Координата точки курс до которой, вычисляем */
  /*    last_y - Возращаемое значение в случае, если расчет не удался */
  /*  */
  /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
  /* 'Heading:11' un = u1 - u0; */
  un_idx_0 = u1[0] - u0[0];
  un_idx_1 = u1[1] - u0[1];

  /* 'Heading:12' d_lat = un(1); */
  /* 'Heading:13' d_lon = un(2); */
  /*     %% Условие для определения кратчайшего пути с учетом переходов между */
  /*  меридианами 0 и 180 по долготе */
  /* 'Heading:16' if d_lon > 180 */
  if (un_idx_1 > 180.0)
  {
    /* 'Heading:17' d_lon = d_lon - 360; */
    un_idx_1 -= 360.0;
  }
  else
  {
    if (un_idx_1 < -180.0)
    {
      /* 'Heading:18' elseif d_lon < (-180) */
      /* 'Heading:19' d_lon = d_lon +360; */
      un_idx_1 += 360.0;
    }
  }

  /*     %% Расчет курса */
  /* 'Heading:22' un_pow1 = d_lat*d_lat; */
  un_pow1 = un_idx_0 * un_idx_0;

  /* 'Heading:23' un_pow2 = d_lon*d_lon; */
  un_pow2 = un_idx_1 * un_idx_1;

  /* 'Heading:24' if (un_pow1 < 1e-16) && (un_pow2 < 1e-16) */
  if ((un_pow1 < 1.0E-16) && (un_pow2 < 1.0E-16))
  {
    /* 'Heading:25' y = last_y; */
    y = last_y;
  }
  else
  {
    /* 'Heading:26' else */
    /* 'Heading:27' x = acos( d_lon / (sqrt(un_pow2 + un_pow1))); */
    /* 'Heading:28' y = acos( d_lat / (sqrt(un_pow2 + un_pow1))); */
    un_pow1 = sqrt(un_pow2 + un_pow1);
    y = acos(un_idx_0 / un_pow1);

    /* 'Heading:29' if x > pi/2 */
    if (acos(un_idx_1 / un_pow1) > 1.5707963267948966)
    {
      /* 'Heading:30' y = 2*pi - y; */
      y = 6.2831853071795862 - y;
    }
  }

  return y;
}

/* Function for Chart: '<S1>/TaskController' */
static void Control(const real_T *AngleManevr, const uint32_T *AngleTimeout,
                    const uint8_T *Mode)
{
  real_T fi1;
  real_T fi2;
  real_T delta_la;
  int32_T b_previousEvent;

  /* During 'Control': '<S7>:9' */
  /*  Основной цикл задач */
  switch (rtDW.is_Control)
  {
   case IN_Flight:
    Flight(AngleManevr, AngleTimeout);
    break;

   case IN_Init:
    /* During 'Init': '<S7>:11' */
    /* Transition: '<S7>:25' */
    /* '<S7>:171:1' sf_internal_predicateOutput = ... */
    /* '<S7>:171:1' Mode == 0; */
    if (*Mode == 0)
    {
      /* Transition: '<S7>:171' */
      rtDW.is_Control = IN_TDP_Only;

      /* Entry 'TDP_Only': '<S7>:167' */
      /*  Режим прицеливания на TDP */
      /* '<S7>:167:1' TargetPoint = TDPoint; */
      rtDW.TargetPoint[0] = rtDW.TmpSignalConversionAtSFunctionI[0];
      rtDW.TargetPoint[1] = rtDW.TmpSignalConversionAtSFunctionI[1];
      rtDW.TargetPoint[2] = rtDW.TmpSignalConversionAtSFunctionI[2];

      /* Course2Target = Azimut(TargetPoint, ActualPoint); */
      /* '<S7>:167:3' Course2Target = Heading(TargetPoint, ActualPoint, 0); */
      rtDW.Course2Target = Heading(rtDW.TargetPoint,
        rtDW.TmpSignalConversionAtSFunctio_g, 0.0);
    }
    else
    {
      /* '<S7>:172:1' sf_internal_predicateOutput = ... */
      /* '<S7>:172:1' (Mode == 1 || Mode == 2); */
      if ((*Mode == 1) || (*Mode == 2))
      {
        /* Transition: '<S7>:172' */
        rtDW.is_Control = IN_Flight;

        /* Entry 'Flight': '<S7>:23' */
        /*  Режим полета по точкам */
        /* '<S7>:23:1' Course2Target = Azimut(TargetPoint + TDPOfset, ActualPoint); */
        /*  u0 - точка отсчета */
        /*  u1 - конечная точка */
        /*  la - долгота */
        /*  fi - широта */
        /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
        /*  */
        /*    Azimut - Вычисление курса от точки u0 до точки u1 */
        /*  */
        /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
        /* 'Azimut:11' la1 = u0(2)*pi/180; */
        /* 'Azimut:12' fi1 = u0(1)*pi/180; */
        fi1 = rtDW.TmpSignalConversionAtSFunctio_g[0] * 3.1415926535897931 /
          180.0;

        /* 'Azimut:13' la2 = u1(2)*pi/180; */
        /* 'Azimut:14' fi2 = u1(1)*pi/180; */
        fi2 = (rtDW.TargetPoint[0] + rtDW.TDPOfset[0]) * 3.1415926535897931 /
          180.0;

        /* 'Azimut:15' delta_la = la2-la1; */
        delta_la = (rtDW.TargetPoint[1] + rtDW.TDPOfset[1]) * 3.1415926535897931
          / 180.0 - rtDW.TmpSignalConversionAtSFunctio_g[1] * 3.1415926535897931
          / 180.0;

        /* 'Azimut:16' y = atan2(sin(delta_la)*cos(fi2), cos(fi1)*sin(fi2)-sin(fi1)*cos(fi2)*cos(delta_la)); */
        rtDW.Course2Target = rt_atan2d_snf(sin(delta_la) * cos(fi2), cos(fi1) *
          sin(fi2) - sin(fi1) * cos(fi2) * cos(delta_la));

        /* 'Azimut:17' y = rem((y + 2*pi), 2*pi); */
        rtDW.Course2Target = rt_remd_snf(rtDW.Course2Target + 6.2831853071795862,
          6.2831853071795862);

        /* Course2Target = Heading(TargetPoint + TDPOfset, ActualPoint, 0); */
        /* Entry Internal 'Flight': '<S7>:23' */
        switch (rtDW.was_Flight)
        {
         case IN_Angle:
          if (rtDW.is_Flight != IN_Angle)
          {
            rtDW.is_Flight = IN_Angle;
            rtDW.was_Flight = IN_Angle;

            /* Inport: '<Root>/ActualCourse' */
            /* Entry 'Angle': '<S7>:82' */
            /*  Режим углового манёвра */
            /* '<S7>:82:1' TargetPoint = EntryAngleMan (Point1, Point2, ActualPoint, Course); */
            /*     %% Определим курс до точек манёвра */
            /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
            /*  EntryAngleMan - Определение оптимальной точки входа в угловой манёвр */
            /*  AM1            - Точка углового манёвра 1 */
            /*  AM2            - Точка углового манёвра 2 */
            /*  Position       - Текущие координаты */
            /*  TrackingCourse - Текущий курс */
            /*  */
            /*  Возвращает одну из точек (AM1 или AM2) в зависимости от оптимальности */
            /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
            /* 'EntryAngleMan:12' Course1 = Heading(AM1, Position, 0); */
            /* 'EntryAngleMan:13' Course2 = Heading(AM2, Position, 0); */
            /*     %% Найдём разницу с путевым */
            /* 'EntryAngleMan:15' Course1 = Course1 - TrackingCourse; */
            fi1 = Heading(rtDW.Point1, rtDW.TmpSignalConversionAtSFunctio_g, 0.0)
              - rtU.ActualCourse;

            /* 'EntryAngleMan:16' Course2 = Course2 - TrackingCourse; */
            fi2 = Heading(rtDW.Point2, rtDW.TmpSignalConversionAtSFunctio_g, 0.0)
              - rtU.ActualCourse;

            /*     %% Проведем нормирование разницы */
            /* 'EntryAngleMan:18' if Course1 > pi */
            if (fi1 > 3.1415926535897931)
            {
              /* 'EntryAngleMan:19' Course1 = Course1 - 2*pi; */
              fi1 -= 6.2831853071795862;
            }

            /* 'EntryAngleMan:21' if Course1 < (-pi) */
            if (fi1 < -3.1415926535897931)
            {
              /* 'EntryAngleMan:22' Course1 = Course1 + 2*pi; */
              fi1 += 6.2831853071795862;
            }

            /* 'EntryAngleMan:24' if Course2 > pi */
            if (fi2 > 3.1415926535897931)
            {
              /* 'EntryAngleMan:25' Course2 = Course2 - 2*pi; */
              fi2 -= 6.2831853071795862;
            }

            /* 'EntryAngleMan:27' if Course2 < (-pi) */
            if (fi2 < -3.1415926535897931)
            {
              /* 'EntryAngleMan:28' Course2 = Course2 + 2*pi; */
              fi2 += 6.2831853071795862;
            }

            /*     %% Возьмём модуль для сравнения */
            /* 'EntryAngleMan:31' Course1 = abs(Course1); */
            fi1 = fabs(fi1);

            /* 'EntryAngleMan:32' Course2 = abs(Course2); */
            fi2 = fabs(fi2);

            /*     %% Будет выбрана точка до которой поворот меньше */
            /* 'EntryAngleMan:34' if Course1 < Course2 */
            if (fi1 < fi2)
            {
              /* 'EntryAngleMan:35' point = AM1; */
              rtDW.TargetPoint[0] = rtDW.Point1[0];
              rtDW.TargetPoint[1] = rtDW.Point1[1];
              rtDW.TargetPoint[2] = rtDW.Point1[2];
            }
            else
            {
              /* 'EntryAngleMan:36' else */
              /* 'EntryAngleMan:37' point = AM2; */
              rtDW.TargetPoint[0] = rtDW.Point2[0];
              rtDW.TargetPoint[1] = rtDW.Point2[1];
              rtDW.TargetPoint[2] = rtDW.Point2[2];
            }
          }
          break;

         case IN_Box:
          if (rtDW.is_Flight != IN_Box)
          {
            rtDW.is_Flight = IN_Box;
            rtDW.was_Flight = IN_Box;

            /* Entry 'Box': '<S7>:242' */
            /*  LU - Нацеливание на верхний левый угол "коробки" */
            /*  LD - Нацеливание на нижний левый угол "коробки" */
            /*  RD - Нацеливание на нижний правый угол "коробки" */
            /*  RU - Нацеливание на верхний правый угол "коробки" */
          }

          enter_internal_Box();
          break;

         case IN_Final:
          if (rtDW.is_Flight != IN_Final)
          {
            rtDW.is_Flight = IN_Final;
            rtDW.was_Flight = IN_Final;

            /* Entry 'Final': '<S7>:94' */
            /*  Состояние последнего манёвра */
            /*  Цель которого выйти на заданную */
            /*  точку приземления */
            /* '<S7>:94:3' BimSupply.wantEnable; */
            /* Event: '<S7>:606' */
            b_previousEvent = rtDW.sfEvent;
            rtDW.sfEvent = event_wantEnable;
            if (rtDW.is_active_BimSupply != 0U)
            {
              BimSupply();
            }

            rtDW.sfEvent = b_previousEvent;

            /* '<S7>:94:4' TargetPoint = TDPoint */
            rtDW.TargetPoint[0] = rtDW.TmpSignalConversionAtSFunctionI[0];
            rtDW.TargetPoint[1] = rtDW.TmpSignalConversionAtSFunctionI[1];
            rtDW.TargetPoint[2] = rtDW.TmpSignalConversionAtSFunctionI[2];
          }
          break;

         case IN_StepAngle:
          if (rtDW.is_Flight != IN_StepAngle)
          {
            rtDW.is_Flight = IN_StepAngle;
            rtDW.was_Flight = IN_StepAngle;

            /* Entry 'StepAngle': '<S7>:582' */
            rtDW.LeftOrRight = false;

            /* '<S7>:582:1' coeff = nonlinearCoefficientFunction(Distance); */
          }

          /* Entry Internal 'StepAngle': '<S7>:582' */
          /* Transition: '<S7>:634' */
          /* '<S7>:634:1' timeout = AngleTimeout; */
          rtDW.timeout = *AngleTimeout;
          if (rtDW.is_StepAngle != IN_Wait)
          {
            rtDW.is_StepAngle = IN_Wait;

            /* Entry 'Wait': '<S7>:591' */
            /* '<S7>:591:1' BimSupply.wantEnable */
            /* Event: '<S7>:606' */
            b_previousEvent = rtDW.sfEvent;
            rtDW.sfEvent = event_wantEnable;
            if (rtDW.is_active_BimSupply != 0U)
            {
              BimSupply();
            }

            rtDW.sfEvent = b_previousEvent;

            /* '<S7>:591:1' waiting = true; */
            rtDW.waiting = true;

            /* '<S7>:591:3' justReturnedHere = true; */
            rtDW.justReturnedHere = true;
          }
          break;

         default:
          /* Transition: '<S7>:268' */
          /* '<S7>:267:1' sf_internal_predicateOutput = ... */
          /* '<S7>:267:1' Mode == 1; */
          switch (*Mode)
          {
           case 1:
            /* Transition: '<S7>:267' */
            if (rtDW.is_Flight != IN_Box)
            {
              rtDW.is_Flight = IN_Box;
              rtDW.was_Flight = IN_Box;

              /* Entry 'Box': '<S7>:242' */
              /*  LU - Нацеливание на верхний левый угол "коробки" */
              /*  LD - Нацеливание на нижний левый угол "коробки" */
              /*  RD - Нацеливание на нижний правый угол "коробки" */
              /*  RU - Нацеливание на верхний правый угол "коробки" */
            }

            enter_internal_Box();
            break;

           case 2:
            /* '<S7>:83:1' sf_internal_predicateOutput = ... */
            /* '<S7>:83:1' Mode == 2; */
            /* Transition: '<S7>:83' */
            if (rtDW.is_Flight != IN_StepAngle)
            {
              rtDW.is_Flight = IN_StepAngle;
              rtDW.was_Flight = IN_StepAngle;

              /* Entry 'StepAngle': '<S7>:582' */
              rtDW.LeftOrRight = false;

              /* '<S7>:582:1' coeff = nonlinearCoefficientFunction(Distance); */
            }

            /* Entry Internal 'StepAngle': '<S7>:582' */
            /* Transition: '<S7>:634' */
            /* '<S7>:634:1' timeout = AngleTimeout; */
            rtDW.timeout = *AngleTimeout;
            if (rtDW.is_StepAngle != IN_Wait)
            {
              rtDW.is_StepAngle = IN_Wait;

              /* Entry 'Wait': '<S7>:591' */
              /* '<S7>:591:1' BimSupply.wantEnable */
              /* Event: '<S7>:606' */
              b_previousEvent = rtDW.sfEvent;
              rtDW.sfEvent = event_wantEnable;
              if (rtDW.is_active_BimSupply != 0U)
              {
                BimSupply();
              }

              rtDW.sfEvent = b_previousEvent;

              /* '<S7>:591:1' waiting = true; */
              rtDW.waiting = true;

              /* '<S7>:591:3' justReturnedHere = true; */
              rtDW.justReturnedHere = true;
            }
            break;
          }
          break;
        }
      }
    }
    break;

   case IN_TDP_Only:
    /* During 'TDP_Only': '<S7>:167' */
    /* '<S7>:167:1' TargetPoint = TDPoint; */
    rtDW.TargetPoint[0] = rtDW.TmpSignalConversionAtSFunctionI[0];
    rtDW.TargetPoint[1] = rtDW.TmpSignalConversionAtSFunctionI[1];
    rtDW.TargetPoint[2] = rtDW.TmpSignalConversionAtSFunctionI[2];

    /* Course2Target = Azimut(TargetPoint, ActualPoint); */
    /* '<S7>:167:3' Course2Target = Heading(TargetPoint, ActualPoint, 0); */
    rtDW.Course2Target = Heading(rtDW.TargetPoint,
      rtDW.TmpSignalConversionAtSFunctio_g, 0.0);
    break;
  }
}

/*
 * Function for Chart: '<S1>/TaskController'
 * function status = CheckFinalMane
 *     %% Вычислим расстояние с помощью функции DistanceCalculator()
 */
static real_T CheckFinalMane(const real_T *V_hor, const real_T *V_vert)
{
  real_T status;
  real_T fi1;
  real_T fi2;

  /* MATLAB Function 'CheckFinalMane': '<S7>:141' */
  /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
  /*  CheckFinalMane - Проверка необходимости перехода к финальному манёвру */
  /*  */
  /*  Использует глобальные переменные: */
  /*     ActualPoint - Текущие координаты */
  /*     TDPoint     - Координаты точки приземления */
  /*     V_hor       - Горизонтальная скорость полета (м/с) */
  /*     V_ver       - Скорость снижения (м/c) */
  /*     Course      - Путевой курс */
  /*     Course2TDP  - Курс (азимут) до точки приземления */
  /*     ReliefOnTDP - Высота рельефа над точкой приземления */
  /*  */
  /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
  /* '<S7>:141:16' Distance = DistanceCalculator(ActualPoint, TDPoint+TDPOfset); */
  /*     %% Вычитаем расстояние (через вектора) */
  /*  dif = (TDP - ActualPoint); */
  /*  % Вычисляем проекции на оси с переводом в Км. */
  /*  distance_lon2x = dif(1)*40000/360*cos(dif(2)*pi/180); */
  /*  distance_lat2y = dif(2)*40000/360; */
  /*  distance = sqrt(distance_lon2x^2 + distance_lat2y^2); */
  /*     %% Вычитаем расстояние (через тригонометрию) */
  /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
  /*  DistanceCalculator - Вычисление расстрояния от Position до Target  */
  /*  в метрах по сферическим координатам */
  /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
  /* 'DistanceCalculator:13' la1 = Position(2)*pi/180; */
  /* 'DistanceCalculator:14' fi1 = Position(1)*pi/180; */
  fi1 = rtDW.TmpSignalConversionAtSFunctio_g[0] * 3.1415926535897931 / 180.0;

  /* 'DistanceCalculator:15' la2 = Target(2)*pi/180; */
  /* 'DistanceCalculator:16' fi2 = Target(1)*pi/180; */
  fi2 = (rtDW.TmpSignalConversionAtSFunctionI[0] + rtDW.TDPOfset[0]) *
    3.1415926535897931 / 180.0;

  /* Outport: '<Root>/DistanceToTDP' */
  /* 'DistanceCalculator:17' d = sin(fi1)*sin(fi2) + cos(fi1)*cos(-fi2)*cos(la2-la1); */
  /*  6371 км средний радиус земли */
  /* 'DistanceCalculator:19' y = acos(d)*6378245; */
  rtY.DistanceToTDP = acos(cos((rtDW.TmpSignalConversionAtSFunctionI[1] +
    rtDW.TDPOfset[1]) * 3.1415926535897931 / 180.0 -
    rtDW.TmpSignalConversionAtSFunctio_g[1] * 3.1415926535897931 / 180.0) * (cos
    (fi1) * cos(-fi2)) + sin(fi1) * sin(fi2)) * 6.378245E+6;

  /* Outport: '<Root>/TimeToHorTarget' incorporates:
   *  Outport: '<Root>/DistanceToTDP'
   */
  /*  С учетом поправки */
  /* Distance = DistanceCalculator(ActualPoint, TDPoint); % С учетом поправки */
  /*     %% Вычисляем время необходимое для преодоления расстояния до TDP и до посадки */
  /* '<S7>:141:19' T_hor = Distance/HorizontalVelocity; */
  rtY.TimeToHorTarget = rtY.DistanceToTDP / *V_hor;

  /* Outport: '<Root>/TimeToTD' incorporates:
   *  Inport: '<Root>/ReliefOnTDP'
   */
  /* '<S7>:141:20' T_ver = (ActualPoint(3) - (TDPoint(3) + double(ReliefOnTDP)))/VerticalVelocity; */
  rtY.TimeToTD = (rtDW.TmpSignalConversionAtSFunctio_g[2] -
                  (rtDW.TmpSignalConversionAtSFunctionI[2] + (real_T)
                   rtU.ReliefOnTDP)) / *V_vert;

  /* Outport: '<Root>/TimeToHorTarget' incorporates:
   *  Outport: '<Root>/TimeToTD'
   */
  /*     %% Определяем поправку по времени необходимому на разворот */
  /*  Определим курс до точки приземления */
  /* '<S7>:141:23' Course2TDP = Heading(TDPoint+TDPOfset, ActualPoint, 0); */
  /*  Учитываем поправку */
  /* Course2TDP = Heading(TDPoint, ActualPoint, 0);  % Учитываем поправку */
  /*  Угол разворота */
  /* '<S7>:141:26' angle = ControlDemode(Course2TDP - Course); */
  /* '<S7>:141:27' angle = abs(angle/pi*180); */
  /*  Вычисляем поправку как линейную функцию:  */
  /* T_hor_popr = angle / TurnSpeed; */
  /*  Вычисляем поправку по параболе */
  /* T_hor_popr = 0.0005*angle^2 + 0.06*angle; */
  /*  Нулевая поправка */
  /* '<S7>:141:35' T_hor_popr = 0; */
  /* '<S7>:141:37' T_hor = T_hor + T_hor_popr; */
  /*     %% Возвращаем сигнал о необходимости перейти на финальный маневр */
  /* '<S7>:141:39' if T_hor > T_ver */
  if (rtY.TimeToHorTarget > rtY.TimeToTD)
  {
    /* '<S7>:141:40' status = 1; */
    status = 1.0;
  }
  else
  {
    /* '<S7>:141:41' else */
    /* '<S7>:141:42' status = 0; */
    status = 0.0;
  }

  return status;
}

/* Model step function */
void Easy_reg_step(void)
{
  real_T powY;
  real_T powX;
  real_T WindX;
  real_T WindY;
  int32_T b_previousEvent;
  real_T AngleManevr;
  uint32_T AngleTimeout;
  uint8_T Mode;
  real_T V_hor;
  real_T rtb_TmpSignalConversionAtSFun_0;
  uint32_T qY;
  int16_T tmp;
  real_T WindY_tmp;
  boolean_T guard1 = false;

  /* Outputs for Atomic SubSystem: '<Root>/Easy_reg' */
  /* DataStoreWrite: '<S1>/Data Store Write1' */
  AngleManevr = 1.5707963267948966;

  /* DataStoreWrite: '<S1>/Data Store Write9' incorporates:
   *  Constant: '<S1>/AngleTimeout'
   */
  AngleTimeout = 60U;

  /* DataStoreWrite: '<S1>/Data Store Write5' incorporates:
   *  Constant: '<S1>/BimTimeout'
   */
  rtDW.BimTimeout = 30U;

  /* DataStoreWrite: '<S1>/Data Store Write3' incorporates:
   *  Constant: '<S1>/Mode'
   */
  Mode = 2U;

  /* Outputs for Atomic SubSystem: '<S16>/ProjectionSpeed' */
  /* MATLAB Function: '<S24>/Speed' incorporates:
   *  Inport: '<Root>/VelocityAltitude'
   *  Inport: '<Root>/VelocityLatitude'
   *  Inport: '<Root>/VelocityLongitude'
   */
  /* MATLAB Function 'ProjectionSpeed/Speed': '<S26>:1' */
  /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
  /*  */
  /*    SpeedProjections - Пересчет проекций векторов скорости по lat, lon, alt */
  /*    в горизонтальную и вертикальную скорость полёта */
  /*  */
  /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
  /* '<S26>:1:8' V_hor = sqrt(Vel_lat^2 + Vel_lon^2); */
  /* '<S26>:1:9' V_vert = abs(Vel_alt); */
  rtb_TmpSignalConversionAtSFun_0 = fabs(rtU.AltitudeVelocity);
  V_hor = sqrt(rtU.LatitudeVelocity * rtU.LatitudeVelocity +
               rtU.LongitudeVelocity * rtU.LongitudeVelocity);

  /* End of Outputs for SubSystem: '<S16>/ProjectionSpeed' */

  /* Chart: '<S2>/DriftEstimator' incorporates:
   *  Constant: '<S1>/HSpeed'
   *  DataStoreWrite: '<S1>/Data Store Write7'
   *  Delay: '<S2>/DsblTg'
   *  Delay: '<S2>/EnblTg'
   *  Delay: '<S2>/SkipSolution'
   *  Inport: '<Root>/ActualCourse'
   */
  /* Gateway: DriftEstimator */
  /* During: DriftEstimator */
  if (rtDW.is_active_c10_BupSimulinkLibrar == 0U)
  {
    /* Entry: DriftEstimator */
    rtDW.is_active_c10_BupSimulinkLibrar = 1U;

    /* Entry Internal: DriftEstimator */
    /* Transition: '<S9>:34' */
    rtDW.is_c10_BupSimulinkLibraries = IN_Initialization;

    /* Entry 'Initialization': '<S9>:33' */
    /* '<S9>:33:1' ReferenceCourse = 0; */
    rtDW.ReferenceCourse = 0.0;

    /* '<S9>:33:1' RealCourse = 0; */
    /* '<S9>:33:3' RealSpeed = 0; */
    /* '<S9>:33:4' DriftSpeed = 0; */
    rtDW.DriftSpeed = 0.0;

    /* '<S9>:33:5' DriftAngle = 0; */
    rtDW.DriftAngle = 0.0;

    /* '<S9>:33:6' timeHasPassed = 0; */
    rtDW.timeHasPassed = 0U;
  }
  else
  {
    switch (rtDW.is_c10_BupSimulinkLibraries)
    {
     case IN_DriftSolver:
      /* During 'DriftSolver': '<S9>:23' */
      /* Transition: '<S9>:38' */
      /* '<S9>:38:1' timeHasPassed = 0; */
      rtDW.timeHasPassed = 0U;
      rtDW.is_c10_BupSimulinkLibraries = IN_FixBeforeCorrection;

      /* Entry 'FixBeforeCorrection': '<S9>:24' */
      /* '<S9>:24:1' timeHasPassed = timeHasPassed + 1; */
      qY = rtDW.timeHasPassed + /*MW:OvSatOk*/ 1U;
      if (qY < rtDW.timeHasPassed)
      {
        qY = MAX_uint32_T;
      }

      rtDW.timeHasPassed = qY;
      break;

     case IN_FixBeforeCorrection:
      /* During 'FixBeforeCorrection': '<S9>:24' */
      /* '<S9>:21:1' sf_internal_predicateOutput = ... */
      /* '<S9>:21:1' BimWasEnabled == 1 && timeHasPassed > 20; */
      if (rtDW.EnblTg_DSTATE && (rtDW.timeHasPassed > 20U))
      {
        /* Transition: '<S9>:21' */
        /* Exit 'FixBeforeCorrection': '<S9>:24' */
        /* '<S9>:24:3' RealCourse = Course; */
        /* '<S9>:24:4' RealSpeed  = Speed; */
        rtDW.is_c10_BupSimulinkLibraries = IN_DriftSolver;

        /* Entry 'DriftSolver': '<S9>:23' */
        /* '<S9>:23:1' [DriftSpeed, DriftAngle] = SpeedTriangle(ReferenceSpeed, RealSpeed, ReferenceCourse, RealCourse); */
        /*     %% Определяем проекции вектора воздушной скорости */
        /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
        /*  */
        /*    Расчет параметров ветра по навигационному треугольнику скоростей */
        /*  */
        /*    Обозначения: */
        /*    TrueAirspeed - Истинная воздушная скорость */
        /*    GroundSpeed - Скорость относительно земли */
        /*    Heading - Направление продольной оси */
        /*    Track - Путевой курс */
        /*  */
        /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
        /* 'SpeedTriangle:14' TrueAirspeedX = TrueAirspeed * cos(pi/2 - Heading); */
        /* 'SpeedTriangle:15' TrueAirspeedY = TrueAirspeed * cos(- Heading); */
        /*     %% Определяем проекции вектора путевой скорости */
        /* 'SpeedTriangle:17' GroundSpeedX = GroundSpeed * cos(pi/2 - Track); */
        /* 'SpeedTriangle:18' GroundSpeedY = GroundSpeed * cos(- Track); */
        /*     %% Определяем проекции вектора ветра */
        /* 'SpeedTriangle:20' WindX = -(TrueAirspeedX - GroundSpeedX); */
        WindX = -(cos(1.5707963267948966 - rtDW.ReferenceCourse) * 20.0 - cos
                  (1.5707963267948966 - rtU.ActualCourse) * V_hor);

        /* 'SpeedTriangle:21' WindY = -(TrueAirspeedY - GroundSpeedY); */
        WindY = -(20.0 * cos(-rtDW.ReferenceCourse) - V_hor * cos
                  (-rtU.ActualCourse));

        /*     %% Определяем модуль вектора ветра */
        /* 'SpeedTriangle:23' powY = WindY*WindY; */
        powY = WindY * WindY;

        /* 'SpeedTriangle:24' powX = WindX*WindX; */
        powX = WindX * WindX;

        /* 'SpeedTriangle:25' WindForce = sqrt(powX + powY); */
        /*     %% Расчет направления ветра */
        /*     %%  */
        /* 'SpeedTriangle:28' if (powX < 1e-16) && (powY < 1e-16) */
        if ((powX < 1.0E-16) && (powY < 1.0E-16))
        {
          /* 'SpeedTriangle:29' y = 0; */
          WindY = 0.0;
        }
        else
        {
          /* 'SpeedTriangle:30' else */
          /* 'SpeedTriangle:31' x = acos( WindX / (sqrt(powX + powY))); */
          /* 'SpeedTriangle:32' y = acos( WindY / (sqrt(powX + powY))); */
          WindY_tmp = sqrt(powX + powY);
          WindY = acos(WindY / WindY_tmp);

          /* 'SpeedTriangle:33' if x > 1.5708 */
          if (acos(WindX / WindY_tmp) > 1.5708)
          {
            /* 'SpeedTriangle:34' y = 6.28319 - y; */
            WindY = 6.28319 - WindY;
          }
        }

        /* 'SpeedTriangle:37' WindAngle = y; */
        rtDW.DriftSpeed = sqrt(powX + powY);
        rtDW.DriftAngle = WindY;

        /* SpeedTrianglePlot(ReferenceSpeed, RealSpeed, ReferenceCourse, RealCourse); */
      }
      else
      {
        /* '<S9>:55:1' sf_internal_predicateOutput = ... */
        /* '<S9>:55:1' Skip; */
        if (rtDW.SkipOfsetSolution)
        {
          /* Transition: '<S9>:55' */
          /* Exit 'FixBeforeCorrection': '<S9>:24' */
          /* '<S9>:24:3' RealCourse = Course; */
          /* '<S9>:24:4' RealSpeed  = Speed; */
          rtDW.is_c10_BupSimulinkLibraries = IN_Pause;
        }
        else
        {
          /* '<S9>:31:1' sf_internal_predicateOutput = ... */
          /* '<S9>:31:1' BimWasDisabled == 1; */
          if (rtDW.DsblTg_DSTATE)
          {
            /* Transition: '<S9>:31' */
            /* '<S9>:31:1' ReferenceCourse = Course; */
            rtDW.ReferenceCourse = rtU.ActualCourse;

            /* '<S9>:31:3' timeHasPassed = 0; */
            rtDW.timeHasPassed = 0U;

            /* Transition: '<S9>:32' */
            /* Exit 'FixBeforeCorrection': '<S9>:24' */
            /* '<S9>:24:3' RealCourse = Course; */
            /* '<S9>:24:4' RealSpeed  = Speed; */
            rtDW.is_c10_BupSimulinkLibraries = IN_FixBeforeCorrection;

            /* Entry 'FixBeforeCorrection': '<S9>:24' */
            /* '<S9>:24:1' timeHasPassed = timeHasPassed + 1; */
            qY = rtDW.timeHasPassed + /*MW:OvSatOk*/ 1U;
            if (qY < rtDW.timeHasPassed)
            {
              qY = MAX_uint32_T;
            }

            rtDW.timeHasPassed = qY;
          }
          else
          {
            /* '<S9>:24:1' timeHasPassed = timeHasPassed + 1; */
            qY = rtDW.timeHasPassed + /*MW:OvSatOk*/ 1U;
            if (qY < rtDW.timeHasPassed)
            {
              qY = MAX_uint32_T;
            }

            rtDW.timeHasPassed = qY;
          }
        }
      }
      break;

     case IN_Initialization:
      /* During 'Initialization': '<S9>:33' */
      /* Transition: '<S9>:22' */
      rtDW.is_c10_BupSimulinkLibraries = IN_Pause;
      break;

     default:
      /* During 'Pause': '<S9>:36' */
      /* '<S9>:37:1' sf_internal_predicateOutput = ... */
      /* '<S9>:37:1' BimWasEnabled == 1; */
      if (rtDW.EnblTg_DSTATE)
      {
        /* Transition: '<S9>:37' */
        rtDW.is_c10_BupSimulinkLibraries = IN_FixBeforeCorrection;

        /* Entry 'FixBeforeCorrection': '<S9>:24' */
        /* '<S9>:24:1' timeHasPassed = timeHasPassed + 1; */
        qY = rtDW.timeHasPassed + /*MW:OvSatOk*/ 1U;
        if (qY < rtDW.timeHasPassed)
        {
          qY = MAX_uint32_T;
        }

        rtDW.timeHasPassed = qY;
      }
      break;
    }
  }

  /* End of Chart: '<S2>/DriftEstimator' */

  /* Outputs for Atomic SubSystem: '<S2>/PreemptionTDP' */
  /* MATLAB Function: '<S11>/MATLAB Function' incorporates:
   *  Constant: '<S1>/FallingTime'
   *  DataStoreWrite: '<S1>/Data Store Write8'
   *  Gain: '<S2>/Gain'
   *  Inport: '<Root>/TDP_lat'
   *  Product: '<S2>/Product'
   */
  /*     %% Рассчитаем угловое расстояние */
  /* MATLAB Function 'PreemptionTDP/MATLAB Function': '<S14>:1' */
  /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
  /*  */
  /*    PointMovement - Перемещение из точки Position на расстояние  */
  /*    DistanceM (метры) в направлении относительно севера Bearing */
  /*  */
  /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
  /* '<S14>:1:9' AngularDistance = DistanceM/6378160; */
  WindX = -30.0 * rtDW.DriftSpeed / 6.37816E+6;

  /*     %% Координаты в радианы */
  /* '<S14>:1:11' Lat1 = Position(1)/180*pi; */
  powY = rtU.TDP_lat / 180.0 * 3.1415926535897931;

  /* '<S14>:1:12' Lon1 = Position(2)/180*pi; */
  /*     %% Получаем координаты точки перемещения */
  /* '<S14>:1:14' Lat2 = asin(sin(Lat1)*cos(AngularDistance) + cos(Lat1)*sin(AngularDistance)*cos(Bearing)); */
  powX = asin(cos(powY) * sin(WindX) * cos(rtDW.DriftAngle) + sin(powY) * cos
              (WindX));

  /* Sum: '<S2>/Sum' incorporates:
   *  Inport: '<Root>/TDP_alt'
   *  Inport: '<Root>/TDP_lat'
   *  Inport: '<Root>/TDP_lon'
   *  MATLAB Function: '<S11>/MATLAB Function'
   */
  /* '<S14>:1:15' Lon2 = Lon1 + atan2(sin(Bearing)*sin(AngularDistance)*cos(Lat1), cos(AngularDistance)-sin(Lat1)*sin(Lat2)); */
  /*     %% Переводим в градусы */
  /* '<S14>:1:17' Lat2 = Lat2/pi*180; */
  /* '<S14>:1:18' Lon2 = Lon2/pi*180; */
  /* '<S14>:1:19' NPoint = [Lat2, Lon2, Position(3)]; */
  WindY = powX / 3.1415926535897931 * 180.0 - rtU.TDP_lat;
  WindX = (rtU.TDP_lon / 180.0 * 3.1415926535897931 + rt_atan2d_snf(sin
            (rtDW.DriftAngle) * sin(WindX) * cos(powY), cos(WindX) - sin(powY) *
            sin(powX))) / 3.1415926535897931 * 180.0 - rtU.TDP_lon;
  powY = rtU.TDP_alt - rtU.TDP_alt;

  /* End of Outputs for SubSystem: '<S2>/PreemptionTDP' */

  /* SignalConversion: '<S7>/TmpSignal ConversionAt SFunction Inport1' incorporates:
   *  Chart: '<S1>/TaskController'
   *  Inport: '<Root>/TDP_alt'
   *  Inport: '<Root>/TDP_lat'
   *  Inport: '<Root>/TDP_lon'
   */
  rtDW.TmpSignalConversionAtSFunctionI[0] = rtU.TDP_lat;
  rtDW.TmpSignalConversionAtSFunctionI[1] = rtU.TDP_lon;
  rtDW.TmpSignalConversionAtSFunctionI[2] = rtU.TDP_alt;

  /* SignalConversion: '<S7>/TmpSignal ConversionAt SFunction Inport2' incorporates:
   *  Chart: '<S1>/TaskController'
   *  Inport: '<Root>/Pos_alt'
   *  Inport: '<Root>/Pos_lat'
   *  Inport: '<Root>/Pos_lon'
   */
  rtDW.TmpSignalConversionAtSFunctio_g[0] = rtU.Pos_lat;
  rtDW.TmpSignalConversionAtSFunctio_g[1] = rtU.Pos_lon;
  rtDW.TmpSignalConversionAtSFunctio_g[2] = rtU.Pos_alt;

  /* Chart: '<S1>/TaskController' incorporates:
   *  Constant: '<S1>/ArrivalRadius'
   *  DataStoreWrite: '<S1>/Data Store Write1'
   *  DataStoreWrite: '<S1>/Data Store Write2'
   *  Delay: '<S2>/DsblTg'
   *  Inport: '<Root>/Relief'
   */
  /* Gateway: Easy_reg/TaskController */
  rtDW.sfEvent = -1;

  /* During: Easy_reg/TaskController */
  if (rtDW.is_active_c1_Easy_reg == 0U)
  {
    /* Entry: Easy_reg/TaskController */
    rtDW.is_active_c1_Easy_reg = 1U;

    /* Entry Internal: Easy_reg/TaskController */
    if (rtDW.is_active_Timer != 1U)
    {
      rtDW.is_active_Timer = 1U;

      /* Entry 'Timer': '<S7>:207' */
      /* '<S7>:207:1' time = 0; */
      rtDW.time = 0U;

      /* '<S7>:207:1' BimSupply.timeout = 0; */
      rtDW.timeout_g = 0U;

      /* '<S7>:207:3' Control.Flight.StepAngle.timeout = 0; */
      rtDW.timeout = 0U;
    }

    if (rtDW.is_active_Control != 1U)
    {
      rtDW.is_active_Control = 1U;

      /* Entry 'Control': '<S7>:9' */
      /*  Основной цикл задач */
    }

    /* Entry Internal 'Control': '<S7>:9' */
    /* Transition: '<S7>:12' */
    if (rtDW.is_Control != IN_Init)
    {
      rtDW.is_Control = IN_Init;

      /* Entry 'Init': '<S7>:11' */
      /*  Производим планирование путевых точек для сброса высоты */
      /* '<S7>:11:4' TargetPoint = TDPoint; */
      rtDW.TargetPoint[0] = rtDW.TmpSignalConversionAtSFunctionI[0];
      rtDW.TargetPoint[1] = rtDW.TmpSignalConversionAtSFunctionI[1];
      rtDW.TargetPoint[2] = rtDW.TmpSignalConversionAtSFunctionI[2];

      /* '<S7>:11:4' [Flight.Angle.Point1, Flight.Angle.Point2] = AngleManCreator(ActualPoint, TDPoint, AngleManevr); */
      /*     %% Расшифровка переменных */
      /*  AMP - Координата углового манёвра */
      /*     %% Это точка углового манёвра расчитанная по равностороннему треугольнику */
      /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
      /*  AngleManCreator - Создание точек углового манёвра по обе стороны от ЛЗП */
      /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
      /* 'AngleManCreator:8' x1 = Initial_point(2); */
      /* 'AngleManCreator:9' y1 = Initial_point(1); */
      /* 'AngleManCreator:10' x2 = TDP(2); */
      /* 'AngleManCreator:11' y2 = TDP(1); */
      /*  Возьмем половину расстояния до точки посадки и повернём этот вектор на */
      /* 'AngleManCreator:13' dx = (x2-x1)/2; */
      AngleManevr = (rtDW.TmpSignalConversionAtSFunctionI[1] -
                     rtDW.TmpSignalConversionAtSFunctio_g[1]) / 2.0;

      /* 'AngleManCreator:14' dy = (y2-y1)/2; */
      rtb_TmpSignalConversionAtSFun_0 = (rtDW.TmpSignalConversionAtSFunctionI[0]
        - rtDW.TmpSignalConversionAtSFunctio_g[0]) / 2.0;

      /*  Определяем первую точку */
      /* 'AngleManCreator:16' x3 = dx*cos(Angle*pi/180) - dy*sin(Angle*pi/180)+x1; */
      /* 'AngleManCreator:17' y3 = dx*sin(Angle*pi/180) + dy*cos(Angle*pi/180)+y1; */
      /* 'AngleManCreator:18' AMP1 = [y3;x3;0]; */
      rtDW.Point1[0] = (AngleManevr * 0.02741213359204429 +
                        rtb_TmpSignalConversionAtSFun_0 * 0.99962421685948166) +
        rtDW.TmpSignalConversionAtSFunctio_g[0];
      rtDW.Point1[1] = (AngleManevr * 0.99962421685948166 -
                        rtb_TmpSignalConversionAtSFun_0 * 0.02741213359204429) +
        rtDW.TmpSignalConversionAtSFunctio_g[1];
      rtDW.Point1[2] = 0.0;

      /*  Определяем вторую точку */
      /* 'AngleManCreator:20' x4 = dx*cos(-Angle*pi/180) - dy*sin(-Angle*pi/180)+x1; */
      /* 'AngleManCreator:21' y4 = dx*sin(-Angle*pi/180) + dy*cos(-Angle*pi/180)+y1; */
      /* 'AngleManCreator:22' AMP2 = [y4;x4;0]; */
      rtDW.Point2[0] = (AngleManevr * -0.02741213359204429 +
                        rtb_TmpSignalConversionAtSFun_0 * 0.99962421685948166) +
        rtDW.TmpSignalConversionAtSFunctio_g[0];
      rtDW.Point2[1] = (AngleManevr * 0.99962421685948166 -
                        rtb_TmpSignalConversionAtSFun_0 * -0.02741213359204429)
        + rtDW.TmpSignalConversionAtSFunctio_g[1];
      rtDW.Point2[2] = 0.0;

      /*     %% Проверим выход точек за пределы координат */
      /* 'AngleManCreator:24' AMP1 = PositionRangeConverting(AMP1); */
      /*     %% Контроль пределов Координат */
      /*  InputPoint(1) - Широта */
      /*  InputPoint(2) - Долгота */
      /*  InputPoint(3) - Высота */
      /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
      /*  PositionRangeConverting - Проверка корректности сферических координат  */
      /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
      /* 'PositionRangeConverting:9' OutputPoint = InputPoint; */
      /*     %% Преобразование Долготы */
      /* 'PositionRangeConverting:11' if OutputPoint(2) > 180 */
      if (rtDW.Point1[1] > 180.0)
      {
        /* 'PositionRangeConverting:12' OutputPoint(2) = OutputPoint(2) - 360; */
        rtDW.Point1[1] -= 360.0;
      }
      else
      {
        if (rtDW.Point1[1] < -180.0)
        {
          /* 'PositionRangeConverting:13' elseif OutputPoint(2) < (-180) */
          /* 'PositionRangeConverting:14' OutputPoint(2) = OutputPoint(2) + 360; */
          rtDW.Point1[1] += 360.0;
        }
      }

      /* 'AngleManCreator:25' AMP2 = PositionRangeConverting(AMP2); */
      /*     %% Контроль пределов Координат */
      /*  InputPoint(1) - Широта */
      /*  InputPoint(2) - Долгота */
      /*  InputPoint(3) - Высота */
      /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
      /*  PositionRangeConverting - Проверка корректности сферических координат  */
      /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
      /* 'PositionRangeConverting:9' OutputPoint = InputPoint; */
      /*     %% Преобразование Долготы */
      /* 'PositionRangeConverting:11' if OutputPoint(2) > 180 */
      if (rtDW.Point2[1] > 180.0)
      {
        /* 'PositionRangeConverting:12' OutputPoint(2) = OutputPoint(2) - 360; */
        rtDW.Point2[1] -= 360.0;
      }
      else
      {
        if (rtDW.Point2[1] < -180.0)
        {
          /* 'PositionRangeConverting:13' elseif OutputPoint(2) < (-180) */
          /* 'PositionRangeConverting:14' OutputPoint(2) = OutputPoint(2) + 360; */
          rtDW.Point2[1] += 360.0;
        }
      }

      /* '<S7>:11:5' [Flight.Box.LUPoint, Flight.Box.LDPoint, Flight.Box.RUPoint, Flight.Box.RDPoint] = BOXCreator(TDPoint, BoxSize); */
      /*     %% Расшифровка переменных */
      /*  BLU - Координата верхней левой точки квадрата */
      /*  BLD - Координата нижней левой точки квадрата */
      /*  BRU - Координата верхней правой точки квадрата */
      /*  BRD - Координата нижней правой точки квадрата */
      /*     %% Расчитываем точки квадрата */
      /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
      /*  BOXCreator - Создание опорных точек аварийного манёвра "Коробка" */
      /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
      /* 'BOXCreator:11' BLU=[0; 0; 0]; */
      /* 'BOXCreator:11' BLD = [0; 0; 0]; */
      /* 'BOXCreator:11' BRU = [0; 0; 0]; */
      /* 'BOXCreator:11' BRD=[0; 0; 0]; */
      rtDW.LUPoint[0] = 0.0;
      rtDW.LDPoint[0] = 0.0;
      rtDW.RUPoint[0] = 0.0;
      rtDW.RDPoint[0] = 0.0;
      rtDW.LUPoint[1] = 0.0;
      rtDW.LDPoint[1] = 0.0;
      rtDW.RUPoint[1] = 0.0;
      rtDW.RDPoint[1] = 0.0;
      rtDW.LUPoint[2] = 0.0;
      rtDW.LDPoint[2] = 0.0;
      rtDW.RUPoint[2] = 0.0;
      rtDW.RDPoint[2] = 0.0;

      /*  Вычисляем шаг в градусах по Широте (однозначно сопоставляются) */
      /* 'BOXCreator:13' LatStep = BoxStep/1000*360/40000; */
      /*  Координаты по Широте */
      /* 'BOXCreator:15' BLU(1) = TDP(1) + LatStep; */
      rtDW.LUPoint[0] = rtDW.TmpSignalConversionAtSFunctionI[0] + 0.009;

      /* 'BOXCreator:16' BLD(1) = TDP(1) - LatStep; */
      rtDW.LDPoint[0] = rtDW.TmpSignalConversionAtSFunctionI[0] - 0.009;

      /* 'BOXCreator:17' BRU(1) = TDP(1) + LatStep; */
      rtDW.RUPoint[0] = rtDW.TmpSignalConversionAtSFunctionI[0] + 0.009;

      /* 'BOXCreator:18' BRD(1) = TDP(1) - LatStep; */
      rtDW.RDPoint[0] = rtDW.TmpSignalConversionAtSFunctionI[0] - 0.009;

      /*  Вычисляем шаг по Долготе без учета поправки на Широту (долгота зависит от */
      /*  широты), а поправку на широту будем делать прямо в выражении */
      /* 'BOXCreator:21' LonStep = BoxStep/1000*360/40000; */
      /*  Координаты по Долготе */
      /* 'BOXCreator:23' BLU(2) = TDP(2) - LonStep/cos(pi*BLU(1)/180); */
      rtDW.LUPoint[1] = rtDW.TmpSignalConversionAtSFunctionI[1] - 0.009 / cos
        (3.1415926535897931 * rtDW.LUPoint[0] / 180.0);

      /* 'BOXCreator:24' BLD(2) = TDP(2) - LonStep/cos(pi*BLD(1)/180); */
      rtDW.LDPoint[1] = rtDW.TmpSignalConversionAtSFunctionI[1] - 0.009 / cos
        (3.1415926535897931 * rtDW.LDPoint[0] / 180.0);

      /* 'BOXCreator:25' BRU(2) = TDP(2) + LonStep/cos(pi*BRU(1)/180); */
      rtDW.RUPoint[1] = 0.009 / cos(3.1415926535897931 * rtDW.RUPoint[0] / 180.0)
        + rtDW.TmpSignalConversionAtSFunctionI[1];

      /* 'BOXCreator:26' BRD(2) = TDP(2) + LonStep/cos(pi*BRD(1)/180); */
      rtDW.RDPoint[1] = 0.009 / cos(3.1415926535897931 * rtDW.RDPoint[0] / 180.0)
        + rtDW.TmpSignalConversionAtSFunctionI[1];

      /*     %% Проверим выход точек за пределы координат */
      /* 'BOXCreator:28' BLU = PositionRangeConverting(BLU); */
      /*     %% Контроль пределов Координат */
      /*  InputPoint(1) - Широта */
      /*  InputPoint(2) - Долгота */
      /*  InputPoint(3) - Высота */
      /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
      /*  PositionRangeConverting - Проверка корректности сферических координат  */
      /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
      /* 'PositionRangeConverting:9' OutputPoint = InputPoint; */
      /*     %% Преобразование Долготы */
      /* 'PositionRangeConverting:11' if OutputPoint(2) > 180 */
      if (rtDW.LUPoint[1] > 180.0)
      {
        /* 'PositionRangeConverting:12' OutputPoint(2) = OutputPoint(2) - 360; */
        rtDW.LUPoint[1] -= 360.0;
      }
      else
      {
        if (rtDW.LUPoint[1] < -180.0)
        {
          /* 'PositionRangeConverting:13' elseif OutputPoint(2) < (-180) */
          /* 'PositionRangeConverting:14' OutputPoint(2) = OutputPoint(2) + 360; */
          rtDW.LUPoint[1] += 360.0;
        }
      }

      /* 'BOXCreator:29' BLD = PositionRangeConverting(BLD); */
      /*     %% Контроль пределов Координат */
      /*  InputPoint(1) - Широта */
      /*  InputPoint(2) - Долгота */
      /*  InputPoint(3) - Высота */
      /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
      /*  PositionRangeConverting - Проверка корректности сферических координат  */
      /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
      /* 'PositionRangeConverting:9' OutputPoint = InputPoint; */
      /*     %% Преобразование Долготы */
      /* 'PositionRangeConverting:11' if OutputPoint(2) > 180 */
      if (rtDW.LDPoint[1] > 180.0)
      {
        /* 'PositionRangeConverting:12' OutputPoint(2) = OutputPoint(2) - 360; */
        rtDW.LDPoint[1] -= 360.0;
      }
      else
      {
        if (rtDW.LDPoint[1] < -180.0)
        {
          /* 'PositionRangeConverting:13' elseif OutputPoint(2) < (-180) */
          /* 'PositionRangeConverting:14' OutputPoint(2) = OutputPoint(2) + 360; */
          rtDW.LDPoint[1] += 360.0;
        }
      }

      /* 'BOXCreator:30' BRU = PositionRangeConverting(BRU); */
      /*     %% Контроль пределов Координат */
      /*  InputPoint(1) - Широта */
      /*  InputPoint(2) - Долгота */
      /*  InputPoint(3) - Высота */
      /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
      /*  PositionRangeConverting - Проверка корректности сферических координат  */
      /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
      /* 'PositionRangeConverting:9' OutputPoint = InputPoint; */
      /*     %% Преобразование Долготы */
      /* 'PositionRangeConverting:11' if OutputPoint(2) > 180 */
      if (rtDW.RUPoint[1] > 180.0)
      {
        /* 'PositionRangeConverting:12' OutputPoint(2) = OutputPoint(2) - 360; */
        rtDW.RUPoint[1] -= 360.0;
      }
      else
      {
        if (rtDW.RUPoint[1] < -180.0)
        {
          /* 'PositionRangeConverting:13' elseif OutputPoint(2) < (-180) */
          /* 'PositionRangeConverting:14' OutputPoint(2) = OutputPoint(2) + 360; */
          rtDW.RUPoint[1] += 360.0;
        }
      }

      /* 'BOXCreator:31' BRD = PositionRangeConverting(BRD); */
      /*     %% Контроль пределов Координат */
      /*  InputPoint(1) - Широта */
      /*  InputPoint(2) - Долгота */
      /*  InputPoint(3) - Высота */
      /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
      /*  PositionRangeConverting - Проверка корректности сферических координат  */
      /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
      /* 'PositionRangeConverting:9' OutputPoint = InputPoint; */
      /*     %% Преобразование Долготы */
      /* 'PositionRangeConverting:11' if OutputPoint(2) > 180 */
      if (rtDW.RDPoint[1] > 180.0)
      {
        /* 'PositionRangeConverting:12' OutputPoint(2) = OutputPoint(2) - 360; */
        rtDW.RDPoint[1] -= 360.0;
      }
      else
      {
        if (rtDW.RDPoint[1] < -180.0)
        {
          /* 'PositionRangeConverting:13' elseif OutputPoint(2) < (-180) */
          /* 'PositionRangeConverting:14' OutputPoint(2) = OutputPoint(2) + 360; */
          rtDW.RDPoint[1] += 360.0;
        }
      }
    }

    if (rtDW.is_active_Touchdown != 1U)
    {
      rtDW.is_active_Touchdown = 1U;

      /* Entry 'Touchdown': '<S7>:34' */
      /* Здесь мы отдельно будем контролировать высоту, */
      /*  для своевременного открытия посадотчных парашютов */
    }

    /* Entry Internal 'Touchdown': '<S7>:34' */
    /* Transition: '<S7>:37' */
    if (rtDW.is_Touchdown != IN_OFF)
    {
      rtDW.is_Touchdown = IN_OFF;

      /* Outport: '<Root>/TD_CMD' */
      /* Entry 'OFF': '<S7>:38' */
      /* '<S7>:38:1' TD_CMD = 0; */
      rtY.TD_CMD = 0U;
    }

    if (rtDW.is_active_EventGenerator != 1U)
    {
      rtDW.is_active_EventGenerator = 1U;

      /* Entry 'EventGenerator': '<S7>:260' */
    }

    if (rtDW.is_active_BimSupply != 1U)
    {
      rtDW.is_active_BimSupply = 1U;

      /* Entry 'BimSupply': '<S7>:452' */
    }

    /* Entry Internal 'BimSupply': '<S7>:452' */
    /* Transition: '<S7>:457' */
    if (rtDW.is_BimSupply != IN_Init_n0)
    {
      rtDW.is_BimSupply = IN_Init_n0;

      /* Entry 'Init': '<S7>:603' */
      /* '<S7>:603:1' TargetEnable = 0; */
      rtDW.TargetEnable = 0U;
    }

    if (rtDW.is_active_OfsetTDP != 1U)
    {
      rtDW.is_active_OfsetTDP = 1U;

      /* Entry 'OfsetTDP': '<S7>:637' */
      /* '<S7>:637:1' TDPOfset = TDPPopr; */
      rtDW.TDPOfset[0] = WindY;
      rtDW.TDPOfset[1] = WindX;
      rtDW.TDPOfset[2] = powY;
    }

    /* Entry Internal 'OfsetTDP': '<S7>:637' */
    /* Transition: '<S7>:710' */
    if (rtDW.is_OfsetTDP != IN_Get)
    {
      rtDW.is_OfsetTDP = IN_Get;

      /* Entry 'Get': '<S7>:709' */
      /* '<S7>:709:1' SkipOfsetSolution = false; */
      rtDW.SkipOfsetSolution = false;
    }
  }
  else
  {
    if (rtDW.is_active_Timer != 0U)
    {
      /* During 'Timer': '<S7>:207' */
      /* '<S7>:207:4' time = time + 1; */
      qY = rtDW.time + /*MW:OvSatOk*/ 1U;
      if (qY < rtDW.time)
      {
        qY = MAX_uint32_T;
      }

      rtDW.time = qY;

      /* '<S7>:207:4' BimSupply.timeout = TimeoutDecrement(BimSupply.timeout); */
      rtDW.timeout_g = TimeoutDecrement(rtDW.timeout_g);

      /* '<S7>:207:5' Control.Flight.StepAngle.timeout = TimeoutDecrement(Control.Flight.StepAngle.timeout); */
      rtDW.timeout = TimeoutDecrement(rtDW.timeout);
    }

    if (rtDW.is_active_Control != 0U)
    {
      Control(&AngleManevr, &AngleTimeout, &Mode);
    }

    if (rtDW.is_active_Touchdown != 0U)
    {
      /* During 'Touchdown': '<S7>:34' */
      /* Здесь мы отдельно будем контролировать высоту, */
      /*  для своевременного открытия посадотчных парашютов */
      guard1 = false;
      switch (rtDW.is_Touchdown)
      {
       case IN_OFF:
        /* During 'OFF': '<S7>:38' */
        /* Transition: '<S7>:119' */
        /* '<S7>:43:1' sf_internal_predicateOutput = ... */
        /* '<S7>:43:1' ReliefAvailable == 0; */
        if ((rtU.ReliefAvailable == 0) && (rtDW.TmpSignalConversionAtSFunctio_g
             [2] <= rtDW.TmpSignalConversionAtSFunctionI[2]))
        {
          /* Transition: '<S7>:43' */
          /* '<S7>:128:1' sf_internal_predicateOutput = ... */
          /* '<S7>:128:1' ActualPoint(3) <= TDPoint(3); */
          /* Transition: '<S7>:128' */
          guard1 = true;
        }
        else
        {
          /* '<S7>:125:1' sf_internal_predicateOutput = ... */
          /* '<S7>:125:1' ReliefAvailable ==1; */
          if (rtU.ReliefAvailable == 1)
          {
            /* Transition: '<S7>:125' */
            /* '<S7>:129:1' sf_internal_predicateOutput = ... */
            /* '<S7>:129:1' (ActualPoint(3)-Relief) <= TDPoint(3); */
            powX = rt_roundd_snf(rtDW.TmpSignalConversionAtSFunctio_g[2] -
                                 (real_T)rtU.Relief);
            if (powX < 32768.0)
            {
              if (powX >= -32768.0)
              {
                tmp = (int16_T)powX;
              }
              else
              {
                tmp = MIN_int16_T;
              }
            }
            else
            {
              tmp = MAX_int16_T;
            }

            if (tmp <= rtDW.TmpSignalConversionAtSFunctionI[2])
            {
              /* Transition: '<S7>:129' */
              guard1 = true;
            }
            else
            {
              /* Outport: '<Root>/TD_CMD' */
              /* '<S7>:38:1' TD_CMD = 0; */
              rtY.TD_CMD = 0U;
            }
          }
          else
          {
            /* Outport: '<Root>/TD_CMD' */
            /* '<S7>:38:1' TD_CMD = 0; */
            rtY.TD_CMD = 0U;
          }
        }
        break;

       case IN_ON:
        /* Outport: '<Root>/TD_CMD' */
        /* During 'ON': '<S7>:36' */
        /* '<S7>:36:1' TD_CMD = 1; */
        rtY.TD_CMD = 1U;
        break;
      }

      if (guard1)
      {
        rtDW.is_Touchdown = IN_ON;

        /* Outport: '<Root>/TD_CMD' */
        /* Entry 'ON': '<S7>:36' */
        /* '<S7>:36:1' TD_CMD = 1; */
        rtY.TD_CMD = 1U;
      }
    }

    if (rtDW.is_active_EventGenerator != 0U)
    {
      /* During 'EventGenerator': '<S7>:260' */
      /*  Контроль выхода на точку */
      /* '<S7>:260:1' if func.CheckFinalMane  == 1 */
      if (CheckFinalMane(&V_hor, &rtb_TmpSignalConversionAtSFun_0) == 1.0)
      {
        /* '<S7>:260:3' Control.Final; */
        /* Event: '<S7>:361' */
        b_previousEvent = rtDW.sfEvent;
        rtDW.sfEvent = event_Final;
        if (rtDW.is_active_Control != 0U)
        {
          Control(&AngleManevr, &AngleTimeout, &Mode);
        }

        /* '<S7>:260:4' BimSupply.wantEnable; */
        /* Event: '<S7>:606' */
        rtDW.sfEvent = event_wantEnable;
        if (rtDW.is_active_BimSupply != 0U)
        {
          BimSupply();
        }

        rtDW.sfEvent = b_previousEvent;
      }
      else
      {
        /* '<S7>:260:5' else */
        /* '<S7>:260:5' Control.CancelFinal; */
        /* Event: '<S7>:391' */
        b_previousEvent = rtDW.sfEvent;
        rtDW.sfEvent = event_CancelFinal;
        if (rtDW.is_active_Control != 0U)
        {
          Control(&AngleManevr, &AngleTimeout, &Mode);
        }

        rtDW.sfEvent = b_previousEvent;
      }

      /*  Контроль попадания в зону текущей цели */
      /* '<S7>:260:10' if CheckArrival(ActualPoint, TargetPoint, ArrivalRadius) == 1 */
      /*     %% Вычитаем коррдинаты и берем модуль */
      /* dif = (To - Now); */
      /* distance = sqrt(dif(1)^2 + dif(2)^2); */
      /*     %% Вычисляем расстояние с помощью DistanceCalculator */
      /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
      /*  CheckArrival - Проверка прибытия в точку с координатами "To" */
      /*  Now    - Текущие координаты */
      /*  To     - Координаты точки приземления */
      /*  Radius - Радиус зоны попадания, м */
      /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
      /* 'CheckArrival:12' distance = DistanceCalculator (Now, To); */
      /*     %% Вычитаем расстояние (через вектора) */
      /*  dif = (TDP - ActualPoint); */
      /*  % Вычисляем проекции на оси с переводом в Км. */
      /*  distance_lon2x = dif(1)*40000/360*cos(dif(2)*pi/180); */
      /*  distance_lat2y = dif(2)*40000/360; */
      /*  distance = sqrt(distance_lon2x^2 + distance_lat2y^2); */
      /*     %% Вычитаем расстояние (через тригонометрию) */
      /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
      /*  DistanceCalculator - Вычисление расстрояния от Position до Target  */
      /*  в метрах по сферическим координатам */
      /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
      /* 'DistanceCalculator:13' la1 = Position(2)*pi/180; */
      /* 'DistanceCalculator:14' fi1 = Position(1)*pi/180; */
      rtb_TmpSignalConversionAtSFun_0 = rtDW.TmpSignalConversionAtSFunctio_g[0] *
        3.1415926535897931 / 180.0;

      /* 'DistanceCalculator:15' la2 = Target(2)*pi/180; */
      /* 'DistanceCalculator:16' fi2 = Target(1)*pi/180; */
      V_hor = rtDW.TargetPoint[0] * 3.1415926535897931 / 180.0;

      /* 'DistanceCalculator:17' d = sin(fi1)*sin(fi2) + cos(fi1)*cos(-fi2)*cos(la2-la1); */
      /*  6371 км средний радиус земли */
      /* 'DistanceCalculator:19' y = acos(d)*6378245; */
      /*     %% Попаданием будем считать радиус  */
      /* 'CheckArrival:14' if distance < Radius */
      powX = rtDW.TmpSignalConversionAtSFunctio_g[1] * 3.1415926535897931 /
        180.0;
      if (acos(cos(rtDW.TargetPoint[1] * 3.1415926535897931 / 180.0 - powX) *
               (cos(rtb_TmpSignalConversionAtSFun_0) * cos(-V_hor)) + sin
               (rtb_TmpSignalConversionAtSFun_0) * sin(V_hor)) * 6.378245E+6 <
          300.0)
      {
        /* 'CheckArrival:15' status = 1; */
        /* '<S7>:260:11' Control.NextStage; */
        /* Event: '<S7>:362' */
        b_previousEvent = rtDW.sfEvent;
        rtDW.sfEvent = event_NextStage;
        if (rtDW.is_active_Control != 0U)
        {
          Control(&AngleManevr, &AngleTimeout, &Mode);
        }

        rtDW.sfEvent = b_previousEvent;
      }
      else
      {
        /* 'CheckArrival:16' else */
        /* 'CheckArrival:17' status = 0; */
      }

      /*  Контроль попадания в зону точки приземления */
      /*  где на работу БИМов нет ограничений */
      /* '<S7>:260:15' if CheckArrival(ActualPoint, TDPoint, 700) == 1 */
      /*     %% Вычитаем коррдинаты и берем модуль */
      /* dif = (To - Now); */
      /* distance = sqrt(dif(1)^2 + dif(2)^2); */
      /*     %% Вычисляем расстояние с помощью DistanceCalculator */
      /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
      /*  CheckArrival - Проверка прибытия в точку с координатами "To" */
      /*  Now    - Текущие координаты */
      /*  To     - Координаты точки приземления */
      /*  Radius - Радиус зоны попадания, м */
      /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
      /* 'CheckArrival:12' distance = DistanceCalculator (Now, To); */
      /*     %% Вычитаем расстояние (через вектора) */
      /*  dif = (TDP - ActualPoint); */
      /*  % Вычисляем проекции на оси с переводом в Км. */
      /*  distance_lon2x = dif(1)*40000/360*cos(dif(2)*pi/180); */
      /*  distance_lat2y = dif(2)*40000/360; */
      /*  distance = sqrt(distance_lon2x^2 + distance_lat2y^2); */
      /*     %% Вычитаем расстояние (через тригонометрию) */
      /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
      /*  DistanceCalculator - Вычисление расстрояния от Position до Target  */
      /*  в метрах по сферическим координатам */
      /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
      /* 'DistanceCalculator:13' la1 = Position(2)*pi/180; */
      /* 'DistanceCalculator:14' fi1 = Position(1)*pi/180; */
      /* 'DistanceCalculator:15' la2 = Target(2)*pi/180; */
      /* 'DistanceCalculator:16' fi2 = Target(1)*pi/180; */
      AngleManevr = rtDW.TmpSignalConversionAtSFunctionI[0] * 3.1415926535897931
        / 180.0;

      /* 'DistanceCalculator:17' d = sin(fi1)*sin(fi2) + cos(fi1)*cos(-fi2)*cos(la2-la1); */
      /*  6371 км средний радиус земли */
      /* 'DistanceCalculator:19' y = acos(d)*6378245; */
      /*     %% Попаданием будем считать радиус  */
      /* 'CheckArrival:14' if distance < Radius */
      if (acos(cos(rtDW.TmpSignalConversionAtSFunctionI[1] * 3.1415926535897931 /
                   180.0 - powX) * (cos(rtb_TmpSignalConversionAtSFun_0) * cos
            (-AngleManevr)) + sin(rtb_TmpSignalConversionAtSFun_0) * sin
               (AngleManevr)) * 6.378245E+6 < 700.0)
      {
        /* 'CheckArrival:15' status = 1; */
        /* '<S7>:260:16' BimSupply.NonBlockingMode; */
        /* Event: '<S7>:486' */
        b_previousEvent = rtDW.sfEvent;
        rtDW.sfEvent = event_NonBlockingMode;
        if (rtDW.is_active_BimSupply != 0U)
        {
          BimSupply();
        }

        rtDW.sfEvent = b_previousEvent;
      }
      else
      {
        /* 'CheckArrival:16' else */
        /* 'CheckArrival:17' status = 0; */
      }

      /*  Контроль попадания в зону точки приземления */
      /*  Где управление уже запрещено */
      /* '<S7>:260:19' if CheckArrival(ActualPoint, TDPoint, 300) == 1 */
      /*     %% Вычитаем коррдинаты и берем модуль */
      /* dif = (To - Now); */
      /* distance = sqrt(dif(1)^2 + dif(2)^2); */
      /*     %% Вычисляем расстояние с помощью DistanceCalculator */
      /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
      /*  CheckArrival - Проверка прибытия в точку с координатами "To" */
      /*  Now    - Текущие координаты */
      /*  To     - Координаты точки приземления */
      /*  Radius - Радиус зоны попадания, м */
      /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
      /* 'CheckArrival:12' distance = DistanceCalculator (Now, To); */
      /*     %% Вычитаем расстояние (через вектора) */
      /*  dif = (TDP - ActualPoint); */
      /*  % Вычисляем проекции на оси с переводом в Км. */
      /*  distance_lon2x = dif(1)*40000/360*cos(dif(2)*pi/180); */
      /*  distance_lat2y = dif(2)*40000/360; */
      /*  distance = sqrt(distance_lon2x^2 + distance_lat2y^2); */
      /*     %% Вычитаем расстояние (через тригонометрию) */
      /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
      /*  DistanceCalculator - Вычисление расстрояния от Position до Target  */
      /*  в метрах по сферическим координатам */
      /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
      /* 'DistanceCalculator:13' la1 = Position(2)*pi/180; */
      /* 'DistanceCalculator:14' fi1 = Position(1)*pi/180; */
      /* 'DistanceCalculator:15' la2 = Target(2)*pi/180; */
      /* 'DistanceCalculator:16' fi2 = Target(1)*pi/180; */
      /* 'DistanceCalculator:17' d = sin(fi1)*sin(fi2) + cos(fi1)*cos(-fi2)*cos(la2-la1); */
      /*  6371 км средний радиус земли */
      /* 'DistanceCalculator:19' y = acos(d)*6378245; */
      /*     %% Попаданием будем считать радиус  */
      /* 'CheckArrival:14' if distance < Radius */
      if (acos(cos(rtDW.TmpSignalConversionAtSFunctionI[1] * 3.1415926535897931 /
                   180.0 - rtDW.TmpSignalConversionAtSFunctio_g[1] *
                   3.1415926535897931 / 180.0) * (cos
            (rtb_TmpSignalConversionAtSFun_0) * cos(-AngleManevr)) + sin
               (rtb_TmpSignalConversionAtSFun_0) * sin(AngleManevr)) *
          6.378245E+6 < 300.0)
      {
        /* 'CheckArrival:15' status = 1; */
        /* '<S7>:260:21' BimSupply.Parking; */
        /* Event: '<S7>:467' */
        b_previousEvent = rtDW.sfEvent;
        rtDW.sfEvent = event_Parking;
        if (rtDW.is_active_BimSupply != 0U)
        {
          BimSupply();
        }

        rtDW.sfEvent = b_previousEvent;
      }
      else
      {
        /* 'CheckArrival:16' else */
        /* 'CheckArrival:17' status = 0; */
      }

      /* '<S7>:260:21' if EndOfCorrection  == 1 */
      if (rtDW.DsblTg_DSTATE)
      {
        /* '<S7>:260:22' if Control.Flight.StepAngle.justReturnedHere */
        if (rtDW.justReturnedHere)
        {
          /* '<S7>:260:25' Control.Flight.StepAngle.justReturnedHere = false; */
          rtDW.justReturnedHere = false;
        }
        else
        {
          /* '<S7>:260:26' else */
          /* '<S7>:260:26' BimSupply.wantDisable */
          /* Event: '<S7>:607' */
          b_previousEvent = rtDW.sfEvent;
          rtDW.sfEvent = event_wantDisable;
          if (rtDW.is_active_BimSupply != 0U)
          {
            BimSupply();
          }

          rtDW.sfEvent = b_previousEvent;
        }
      }
    }

    if (rtDW.is_active_BimSupply != 0U)
    {
      BimSupply();
    }

    if (rtDW.is_active_OfsetTDP != 0U)
    {
      /* During 'OfsetTDP': '<S7>:637' */
      /* '<S7>:637:1' TDPOfset = TDPPopr; */
      rtDW.TDPOfset[0] = WindY;
      rtDW.TDPOfset[1] = WindX;
      rtDW.TDPOfset[2] = powY;
      switch (rtDW.is_OfsetTDP)
      {
       case IN_Get:
        /* During 'Get': '<S7>:709' */
        /* '<S7>:711:1' sf_internal_predicateOutput = ... */
        /* '<S7>:711:1' ~Control.Flight.StepAngle.waiting; */
        if (!rtDW.waiting)
        {
          /* Transition: '<S7>:711' */
          rtDW.is_OfsetTDP = IN_Skip;

          /* Entry 'Skip': '<S7>:707' */
          /* '<S7>:707:1' SkipOfsetSolution = true; */
          rtDW.SkipOfsetSolution = true;
        }
        else
        {
          /* '<S7>:709:1' SkipOfsetSolution = false; */
          rtDW.SkipOfsetSolution = false;
        }
        break;

       case IN_Skip:
        /* During 'Skip': '<S7>:707' */
        /* '<S7>:712:1' sf_internal_predicateOutput = ... */
        /* '<S7>:712:1' Control.Flight.StepAngle.waiting; */
        if (rtDW.waiting)
        {
          /* Transition: '<S7>:712' */
          rtDW.is_OfsetTDP = IN_Get;

          /* Entry 'Get': '<S7>:709' */
          /* '<S7>:709:1' SkipOfsetSolution = false; */
          rtDW.SkipOfsetSolution = false;
        }
        else
        {
          /* '<S7>:707:1' SkipOfsetSolution = true; */
          rtDW.SkipOfsetSolution = true;
        }
        break;
      }
    }
  }

  /* Sum: '<S6>/Sum1' incorporates:
   *  Inport: '<Root>/ActualCourse'
   */
  AngleManevr = rtDW.Course2Target - rtU.ActualCourse;

  /* Outputs for Atomic SubSystem: '<S6>/ControlDemode' */
  /* MATLAB Function: '<S27>/ControlDemode' */
  /*  Здесь переводим угол из диапазона [0 360] */
  /*  в диапазон [-180 180], что позволит нам определять */
  /*  оптимальное направление разворота (лево и право) */
  /* MATLAB Function 'ControlDemode/ControlDemode': '<S35>:1' */
  /* '<S35>:1:5' y = u; */
  /*  Если больше 180 */
  /* '<S35>:1:7' if y > pi */
  if (AngleManevr > 3.1415926535897931)
  {
    /*  то это уже отрицательная полуокружность */
    /* '<S35>:1:9' y = y - 2*pi; */
    AngleManevr -= 6.2831853071795862;

    /*  Если меньше -180 */
  }
  else
  {
    if (AngleManevr < -3.1415926535897931)
    {
      /* '<S35>:1:11' elseif (y < (-pi)) */
      /*  то это уже положительная полуокружность */
      /* '<S35>:1:13' y = y + 2*pi; */
      AngleManevr += 6.2831853071795862;
    }
  }

  /* End of MATLAB Function: '<S27>/ControlDemode' */
  /* End of Outputs for SubSystem: '<S6>/ControlDemode' */

  /* Gain: '<S6>/Base_Gain' incorporates:
   *  Gain: '<S32>/Gain'
   */
  AngleManevr = 57.295779513082323 * AngleManevr * 2.0;

  /* Saturate: '<S6>/Base_Sat' */
  if (AngleManevr > 50.0)
  {
    AngleManevr = 50.0;
  }
  else
  {
    if (AngleManevr < -50.0)
    {
      AngleManevr = -50.0;
    }
  }

  /* End of Saturate: '<S6>/Base_Sat' */

  /* Gain: '<S33>/Gain1' incorporates:
   *  Gain: '<S33>/Gain'
   *  Rounding: '<S33>/Rounding Function'
   */
  AngleManevr = floor(0.1 * AngleManevr) * 10.0;

  /* Outputs for Atomic SubSystem: '<S33>/DeadZone' */
  /* MATLAB Function: '<S36>/DeadZone' */
  /* MATLAB Function 'DeadZone/DeadZone': '<S37>:1' */
  /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
  /*  */
  /*    Звено нечувствительности */
  /*  */
  /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
  /* '<S37>:1:7' if abs(u) < 20 */
  if (fabs(AngleManevr) < 20.0)
  {
    /* '<S37>:1:8' y = 0; */
    AngleManevr = 0.0;
  }
  else
  {
    /* '<S37>:1:9' else */
    /* '<S37>:1:10' y = u; */
  }

  /* End of MATLAB Function: '<S36>/DeadZone' */
  /* End of Outputs for SubSystem: '<S33>/DeadZone' */

  /* Product: '<S6>/Product' */
  /*  if u > 10 */
  /*      y = 50; */
  /*  elseif u < -10 */
  /*      y = -50; */
  /*  else */
  /*      y = 0; */
  /*  end */
  AngleManevr *= (real_T)rtDW.TargetEnable;

  /* Outputs for Atomic SubSystem: '<S2>/BimTriggers' */
  /* Chart: '<S8>/BimTrigger' */
  /* Gateway: BimTriggers/BimTrigger */
  /* During: BimTriggers/BimTrigger */
  if (rtDW.is_active_c9_BupSimulinkLibrari == 0U)
  {
    /* Entry: BimTriggers/BimTrigger */
    rtDW.is_active_c9_BupSimulinkLibrari = 1U;

    /* Entry Internal: BimTriggers/BimTrigger */
    /* Entry 'Triggers': '<S12>:18' */
    /* Entry Internal 'Triggers': '<S12>:18' */
    /* Entry 'DisabledTrigger': '<S12>:19' */
    /* '<S12>:19:1' if LastLineState > 0 && Line <=0 */
    if ((rtDW.LastLineState > 0.0) && (AngleManevr <= 0.0))
    {
      /* Update for Delay: '<S2>/DsblTg' */
      /* '<S12>:19:1' Disabled = true; */
      rtDW.DsblTg_DSTATE = true;
    }
    else if ((rtDW.LastLineState < 0.0) && (AngleManevr >= 0.0))
    {
      /* Update for Delay: '<S2>/DsblTg' */
      /* '<S12>:19:2' elseif LastLineState < 0 && Line >=0 */
      /* '<S12>:19:4' Disabled = true; */
      rtDW.DsblTg_DSTATE = true;
    }
    else
    {
      /* Update for Delay: '<S2>/DsblTg' */
      /* '<S12>:19:4' else */
      /* '<S12>:19:4' Disabled = false; */
      rtDW.DsblTg_DSTATE = false;
    }

    /* Entry 'EnabledTrigger1': '<S12>:21' */
    /* '<S12>:21:1' if LastLineState == 0 && Line > 0 */
    if ((rtDW.LastLineState == 0.0) && (AngleManevr > 0.0))
    {
      /* Update for Delay: '<S2>/EnblTg' */
      /* '<S12>:21:1' Enabled = true; */
      rtDW.EnblTg_DSTATE = true;
    }
    else if ((rtDW.LastLineState == 0.0) && (AngleManevr < 0.0))
    {
      /* Update for Delay: '<S2>/EnblTg' */
      /* '<S12>:21:2' elseif LastLineState == 0 && Line < 0 */
      /* '<S12>:21:4' Enabled = true; */
      rtDW.EnblTg_DSTATE = true;
    }
    else
    {
      /* Update for Delay: '<S2>/EnblTg' */
      /* '<S12>:21:4' else */
      /* '<S12>:21:4' Enabled = false; */
      rtDW.EnblTg_DSTATE = false;
    }

    /* Entry 'RemeberLastState': '<S12>:29' */
    /* '<S12>:29:1' LastLineState = Line; */
    rtDW.LastLineState = AngleManevr;
  }
  else
  {
    /* During 'Triggers': '<S12>:18' */
    /* During 'DisabledTrigger': '<S12>:19' */
    /* '<S12>:19:1' if LastLineState > 0 && Line <=0 */
    if ((rtDW.LastLineState > 0.0) && (AngleManevr <= 0.0))
    {
      /* Update for Delay: '<S2>/DsblTg' */
      /* '<S12>:19:1' Disabled = true; */
      rtDW.DsblTg_DSTATE = true;
    }
    else if ((rtDW.LastLineState < 0.0) && (AngleManevr >= 0.0))
    {
      /* Update for Delay: '<S2>/DsblTg' */
      /* '<S12>:19:2' elseif LastLineState < 0 && Line >=0 */
      /* '<S12>:19:3' Disabled = true; */
      rtDW.DsblTg_DSTATE = true;
    }
    else
    {
      /* Update for Delay: '<S2>/DsblTg' */
      /* '<S12>:19:4' else */
      /* '<S12>:19:4' Disabled = false; */
      rtDW.DsblTg_DSTATE = false;
    }

    /* During 'EnabledTrigger1': '<S12>:21' */
    /* '<S12>:21:1' if LastLineState == 0 && Line > 0 */
    if ((rtDW.LastLineState == 0.0) && (AngleManevr > 0.0))
    {
      /* Update for Delay: '<S2>/EnblTg' */
      /* '<S12>:21:1' Enabled = true; */
      rtDW.EnblTg_DSTATE = true;
    }
    else if ((rtDW.LastLineState == 0.0) && (AngleManevr < 0.0))
    {
      /* Update for Delay: '<S2>/EnblTg' */
      /* '<S12>:21:2' elseif LastLineState == 0 && Line < 0 */
      /* '<S12>:21:3' Enabled = true; */
      rtDW.EnblTg_DSTATE = true;
    }
    else
    {
      /* Update for Delay: '<S2>/EnblTg' */
      /* '<S12>:21:4' else */
      /* '<S12>:21:4' Enabled = false; */
      rtDW.EnblTg_DSTATE = false;
    }

    /* During 'RemeberLastState': '<S12>:29' */
    /* '<S12>:29:1' LastLineState = Line; */
    rtDW.LastLineState = AngleManevr;
  }

  /* End of Chart: '<S8>/BimTrigger' */
  /* End of Outputs for SubSystem: '<S2>/BimTriggers' */

  /* Outputs for Atomic SubSystem: '<S15>/Bearing' */
  /* MATLAB Function: '<S18>/Heading_true' incorporates:
   *  Delay: '<S15>/LastPos'
   *  Delay: '<S18>/PreviousBearing'
   *  Inport: '<Root>/Pos_lat'
   *  Inport: '<Root>/Pos_lon'
   */
  /*  u0 - точка отсчета */
  /*  u1 - конечная точка */
  /*  la - долгота */
  /*  fi - широта */
  /* MATLAB Function 'Azimuth/Azimut': '<S21>:1' */
  /* '<S21>:1:6' la1 = u0(2)*pi/180; */
  /* '<S21>:1:7' fi1 = u0(1)*pi/180; */
  /* '<S21>:1:8' la2 = u1(2)*pi/180; */
  /* '<S21>:1:9' fi2 = u1(1)*pi/180; */
  /* '<S21>:1:10' delta_la = la2-la1; */
  /* '<S21>:1:11' y = atan2(sin(delta_la)*cos(fi2), cos(fi1)*sin(fi2)-sin(fi1)*cos(fi2)*cos(delta_la)); */
  /* '<S21>:1:12' y = rem((y + 2*pi), 2*pi); */
  /*  Высчитаем, как изменились координаты */
  /* MATLAB Function 'Bearing/Heading_true': '<S22>:1' */
  /* '<S22>:1:3' un = u1 - u0; */
  WindY = rtU.Pos_lat - rtDW.LastPos_1_DSTATE;
  rtb_TmpSignalConversionAtSFun_0 = rtU.Pos_lon - rtDW.LastPos_2_DSTATE;

  /* '<S22>:1:4' d_lat = un(1); */
  /* '<S22>:1:5' d_lon = un(2); */
  /*  Условие для определения кратчайшего пути с учетом переходов между */
  /*  меридианами 0 и 180 по долготе */
  /* '<S22>:1:8' if d_lon > 180 */
  if (rtb_TmpSignalConversionAtSFun_0 > 180.0)
  {
    /* '<S22>:1:9' d_lon = d_lon - 360; */
    rtb_TmpSignalConversionAtSFun_0 -= 360.0;
  }
  else
  {
    if (rtb_TmpSignalConversionAtSFun_0 < -180.0)
    {
      /* '<S22>:1:10' elseif d_lon < (-180) */
      /* '<S22>:1:11' d_lon = d_lon +360; */
      rtb_TmpSignalConversionAtSFun_0 += 360.0;
    }
  }

  /*  Расчет курса */
  /* '<S22>:1:14' un_pow1 = d_lat*d_lat; */
  V_hor = WindY * WindY;

  /* '<S22>:1:15' un_pow2 = d_lon*d_lon; */
  WindX = rtb_TmpSignalConversionAtSFun_0 * rtb_TmpSignalConversionAtSFun_0;

  /* '<S22>:1:16' if (un_pow1 < 1e-16) && (un_pow2 < 1e-16) */
  if ((V_hor < 1.0E-16) && (WindX < 1.0E-16))
  {
    /* '<S22>:1:17' y = last_y; */
    powY = rtDW.PreviousBearing_DSTATE;
  }
  else
  {
    /* '<S22>:1:18' else */
    /* '<S22>:1:19' x = acos( d_lon / (sqrt(un_pow2 + un_pow1))); */
    /* '<S22>:1:20' y = acos( d_lat / (sqrt(un_pow2 + un_pow1))); */
    V_hor = sqrt(WindX + V_hor);
    powY = acos(WindY / V_hor);

    /* '<S22>:1:21' if x > pi/2 */
    if (acos(rtb_TmpSignalConversionAtSFun_0 / V_hor) > 1.5707963267948966)
    {
      /* '<S22>:1:22' y = 2*pi - y; */
      powY = 6.2831853071795862 - powY;
    }
  }

  /* End of MATLAB Function: '<S18>/Heading_true' */

  /* Update for Delay: '<S18>/PreviousBearing' */
  rtDW.PreviousBearing_DSTATE = powY;

  /* End of Outputs for SubSystem: '<S15>/Bearing' */

  /* Update for Delay: '<S15>/LastPos' incorporates:
   *  Inport: '<Root>/Pos_lat'
   *  Inport: '<Root>/Pos_lon'
   */
  /* MATLAB Function 'GPSVelocity/Velocity': '<S25>:1' */
  /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
  /*  */
  /*    GPSPos2Speed - Определение скорости по сигналам GPS */
  /*    u1 - Точка относительно, которой считаем перемещение (lat, lon, alt) */
  /*    u0 - Точка до которой отладываем перемещение (lat, lon, alt) */
  /*    time - Время между замерами координат */
  /*  */
  /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
  /* '<S25>:1:10' un = u1-u0; */
  /* '<S25>:1:11' v_lat = un(1)*40000/360*1000/time; */
  /* '<S25>:1:12' v_lon = un(2)*(40000/360*1000/time * cos(pi/180*u0(1))); */
  /* '<S25>:1:13' v_alt = un(3); */
  rtDW.LastPos_1_DSTATE = rtU.Pos_lat;
  rtDW.LastPos_2_DSTATE = rtU.Pos_lon;

  /* End of Outputs for SubSystem: '<Root>/Easy_reg' */

  /* Outport: '<Root>/BIM_CMD' */
  rtY.BIM_CMD = AngleManevr;
}

/* Model initialize function */
void Easy_reg_initialize(void)
{
  /* Registration code */

  /* initialize non-finites */
  rt_InitInfAndNaN(sizeof(real_T));

  /* SystemInitialize for Atomic SubSystem: '<Root>/Easy_reg' */
  /* SystemInitialize for Chart: '<S1>/TaskController' */
  rtDW.sfEvent = -1;

  /* End of SystemInitialize for SubSystem: '<Root>/Easy_reg' */
}

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
