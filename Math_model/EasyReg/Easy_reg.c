/*
 * File: Easy_reg.c
 *
 * Code generated for Simulink model 'Easy_reg'.
 *
 * Model version                  : 1.537
 * Simulink Coder version         : 8.14 (R2018a) 06-Feb-2018
 * C/C++ source code generated on : Wed Nov 27 18:58:35 2019
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
#define IN_FixAfterCorrection          ((uint8_T)2U)
#define IN_FixBeforeCorrection         ((uint8_T)3U)

/* Named constants for Chart: '<S1>/TaskController' */
#define IN_Angle                       ((uint8_T)1U)
#define IN_Box                         ((uint8_T)2U)
#define IN_Disable                     ((uint8_T)1U)
#define IN_Enable                      ((uint8_T)2U)
#define IN_Final                       ((uint8_T)3U)
#define IN_Flight                      ((uint8_T)1U)
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
#define IN_StepAngle                   ((uint8_T)4U)
#define IN_TDP_Only                    ((uint8_T)3U)
#define IN_Wait                        ((uint8_T)3U)
#define event_CancelFinal              (0)
#define event_Final                    (1)
#define event_NextStage                (2)
#define event_NonBlockingMode          (3)
#define event_Parking                  (4)
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
extern real_T rt_roundd_snf(real_T u);

/* Forward declaration for local functions */
static uint32_T TimeoutDecrement(uint32_T TimeoutTag);
static real_T Heading(const real_T u1[3], const real_T u0[3], real_T last_y);
static uint8_T VerticeSelect(const real_T AMP[3], const real_T BLU[3], const
  real_T BLD[3], const real_T BRU[3], const real_T BRD[3]);
static void enter_internal_Box(void);
static void Control(const uint8_T *Mode, const real_T *y, const real_T
                    TmpSignalConversionAtSFunctionI[3], int32_T *sfEvent);
static real_T CheckFinalMane(const real_T *VSpeed, const real_T *HSpeed, const
  real_T TmpSignalConversionAtSFunctionI[3]);
static void BimSupply(const uint32_T *BimTimeout, const boolean_T *DsblTg,
                      int32_T *sfEvent);

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
 * Function for Chart: '<S1>/TaskController'
 * function Timeoutput = TimeoutDecrement (TimeoutTag)
 */
static uint32_T TimeoutDecrement(uint32_T TimeoutTag)
{
  uint32_T Timeoutput;

  /* MATLAB Function 'TimeoutDecrement': '<S6>:573' */
  /* Graphical Function 'TimeoutDecrement': '<S6>:573' */
  /* '<S6>:577:1' sf_internal_predicateOutput = ... */
  /* '<S6>:577:1' TimeoutTag > 0; */
  if (TimeoutTag > 0U)
  {
    /* '<S6>:577:1' Timeoutput = TimeoutTag - 1; */
    Timeoutput = TimeoutTag - 1U;
  }
  else
  {
    /* '<S6>:578:1' Timeoutput = uint32(0); */
    Timeoutput = 0U;
  }

  return Timeoutput;
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
  /* Entry Internal 'Box': '<S6>:242' */
  switch (rtDW.was_Box)
  {
   case IN_Init_n:
    if (rtDW.is_Box != IN_Init_n)
    {
      rtDW.is_Box = IN_Init_n;
      rtDW.was_Box = IN_Init_n;

      /* Entry 'Init': '<S6>:393' */
      /* '<S6>:393:1' BoxEntry = VerticeSelect(ActualPoint, LUPoint, LDPoint, RUPoint, RDPoint); */
      rtDW.BoxEntry = VerticeSelect(rtDW.TmpSignalConversionAtSFunctio_g,
        rtDW.LUPoint, rtDW.LDPoint, rtDW.RUPoint, rtDW.RDPoint);
    }
    break;

   case IN_LD:
    if (rtDW.is_Box != IN_LD)
    {
      rtDW.is_Box = IN_LD;
      rtDW.was_Box = IN_LD;

      /* Entry 'LD': '<S6>:244' */
      /* '<S6>:244:1' TargetPoint = LDPoint; */
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

      /* Entry 'LU': '<S6>:246' */
      /* '<S6>:246:1' TargetPoint = LUPoint; */
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

      /* Entry 'RD': '<S6>:248' */
      /* '<S6>:248:1' TargetPoint = RDPoint; */
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

      /* Entry 'RU': '<S6>:249' */
      /* '<S6>:249:1' TargetPoint = RUPoint; */
      rtDW.TargetPoint[0] = rtDW.RUPoint[0];
      rtDW.TargetPoint[1] = rtDW.RUPoint[1];
      rtDW.TargetPoint[2] = rtDW.RUPoint[2];
    }
    break;

   default:
    /* Transition: '<S6>:394' */
    if (rtDW.is_Box != IN_Init_n)
    {
      rtDW.is_Box = IN_Init_n;
      rtDW.was_Box = IN_Init_n;

      /* Entry 'Init': '<S6>:393' */
      /* '<S6>:393:1' BoxEntry = VerticeSelect(ActualPoint, LUPoint, LDPoint, RUPoint, RDPoint); */
      rtDW.BoxEntry = VerticeSelect(rtDW.TmpSignalConversionAtSFunctio_g,
        rtDW.LUPoint, rtDW.LDPoint, rtDW.RUPoint, rtDW.RDPoint);
    }
    break;
  }
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
static void Control(const uint8_T *Mode, const real_T *y, const real_T
                    TmpSignalConversionAtSFunctionI[3], int32_T *sfEvent)
{
  real_T Course1;
  real_T Course2;

  /* During 'Control': '<S6>:9' */
  /*  Основной цикл задач */
  switch (rtDW.is_Control)
  {
   case IN_Flight:
    /* During 'Flight': '<S6>:23' */
    /* Course2Target = Azimut(TargetPoint, ActualPoint); */
    /* '<S6>:23:2' Course2Target = Heading(TargetPoint, ActualPoint, 0); */
    rtDW.Course2Target = Heading(rtDW.TargetPoint,
      rtDW.TmpSignalConversionAtSFunctio_g, 0.0);
    switch (rtDW.is_Flight)
    {
     case IN_Angle:
      /* During 'Angle': '<S6>:82' */
      /* '<S6>:89:1' sf_internal_predicateOutput = ... */
      /* '<S6>:89:1' NextStage; */
      switch (*sfEvent)
      {
       case event_NextStage:
        /* Transition: '<S6>:89' */
        rtDW.is_Flight = IN_Box;
        rtDW.was_Flight = IN_Box;

        /* Entry 'Box': '<S6>:242' */
        /*  LU - Нацеливание на верхний левый угол "коробки" */
        /*  LD - Нацеливание на нижний левый угол "коробки" */
        /*  RD - Нацеливание на нижний правый угол "коробки" */
        /*  RU - Нацеливание на верхний правый угол "коробки" */
        enter_internal_Box();
        break;

       case event_Final:
        /* '<S6>:258:1' sf_internal_predicateOutput = ... */
        /* '<S6>:258:1' Final; */
        /* Transition: '<S6>:258' */
        rtDW.is_Flight = IN_Final;
        rtDW.was_Flight = IN_Final;

        /* Entry 'Final': '<S6>:94' */
        /*  Состояние последнего манёвра */
        /*  Цель которого выйти на заданную */
        /*  точку приземления */
        /* entry: */
        /* '<S6>:94:3' TargetPoint = TDPoint; */
        rtDW.TargetPoint[0] = TmpSignalConversionAtSFunctionI[0];
        rtDW.TargetPoint[1] = TmpSignalConversionAtSFunctionI[1];
        rtDW.TargetPoint[2] = TmpSignalConversionAtSFunctionI[2];
        break;
      }
      break;

     case IN_Box:
      /* During 'Box': '<S6>:242' */
      /* '<S6>:108:1' sf_internal_predicateOutput = ... */
      /* '<S6>:108:1' Final; */
      if (*sfEvent == event_Final)
      {
        /* Transition: '<S6>:108' */
        /* Exit Internal 'Box': '<S6>:242' */
        rtDW.is_Box = 0;
        rtDW.is_Flight = IN_Final;
        rtDW.was_Flight = IN_Final;

        /* Entry 'Final': '<S6>:94' */
        /*  Состояние последнего манёвра */
        /*  Цель которого выйти на заданную */
        /*  точку приземления */
        /* entry: */
        /* '<S6>:94:3' TargetPoint = TDPoint; */
        rtDW.TargetPoint[0] = TmpSignalConversionAtSFunctionI[0];
        rtDW.TargetPoint[1] = TmpSignalConversionAtSFunctionI[1];
        rtDW.TargetPoint[2] = TmpSignalConversionAtSFunctionI[2];
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
          /* During 'Init': '<S6>:393' */
          /* Transition: '<S6>:256' */
          /* '<S6>:251:1' sf_internal_predicateOutput = ... */
          /* '<S6>:251:1' BoxEntry == 2; */
          switch (rtDW.BoxEntry)
          {
           case 2:
            /* Transition: '<S6>:251' */
            rtDW.is_Box = IN_LD;
            rtDW.was_Box = IN_LD;

            /* Entry 'LD': '<S6>:244' */
            /* '<S6>:244:1' TargetPoint = LDPoint; */
            rtDW.TargetPoint[0] = rtDW.LDPoint[0];
            rtDW.TargetPoint[1] = rtDW.LDPoint[1];
            rtDW.TargetPoint[2] = rtDW.LDPoint[2];
            break;

           case 1:
            /* '<S6>:243:1' sf_internal_predicateOutput = ... */
            /* '<S6>:243:1' BoxEntry == 1; */
            /* Transition: '<S6>:243' */
            rtDW.is_Box = IN_LU;
            rtDW.was_Box = IN_LU;

            /* Entry 'LU': '<S6>:246' */
            /* '<S6>:246:1' TargetPoint = LUPoint; */
            rtDW.TargetPoint[0] = rtDW.LUPoint[0];
            rtDW.TargetPoint[1] = rtDW.LUPoint[1];
            rtDW.TargetPoint[2] = rtDW.LUPoint[2];
            break;

           case 4:
            /* '<S6>:255:1' sf_internal_predicateOutput = ... */
            /* '<S6>:255:1' BoxEntry == 4; */
            /* Transition: '<S6>:255' */
            rtDW.is_Box = IN_RD;
            rtDW.was_Box = IN_RD;

            /* Entry 'RD': '<S6>:248' */
            /* '<S6>:248:1' TargetPoint = RDPoint; */
            rtDW.TargetPoint[0] = rtDW.RDPoint[0];
            rtDW.TargetPoint[1] = rtDW.RDPoint[1];
            rtDW.TargetPoint[2] = rtDW.RDPoint[2];
            break;

           case 3:
            /* '<S6>:253:1' sf_internal_predicateOutput = ... */
            /* '<S6>:253:1' BoxEntry == 3; */
            /* Transition: '<S6>:253' */
            rtDW.is_Box = IN_RU;
            rtDW.was_Box = IN_RU;

            /* Entry 'RU': '<S6>:249' */
            /* '<S6>:249:1' TargetPoint = RUPoint; */
            rtDW.TargetPoint[0] = rtDW.RUPoint[0];
            rtDW.TargetPoint[1] = rtDW.RUPoint[1];
            rtDW.TargetPoint[2] = rtDW.RUPoint[2];
            break;

           default:
            /* '<S6>:393:1' BoxEntry = VerticeSelect(ActualPoint, LUPoint, LDPoint, RUPoint, RDPoint); */
            rtDW.BoxEntry = VerticeSelect(rtDW.TmpSignalConversionAtSFunctio_g,
              rtDW.LUPoint, rtDW.LDPoint, rtDW.RUPoint, rtDW.RDPoint);
            break;
          }
          break;

         case IN_LD:
          /* During 'LD': '<S6>:244' */
          /* '<S6>:250:1' sf_internal_predicateOutput = ... */
          /* '<S6>:250:1' NextStage; */
          if (*sfEvent == event_NextStage)
          {
            /* Transition: '<S6>:250' */
            rtDW.is_Box = IN_RD;
            rtDW.was_Box = IN_RD;

            /* Entry 'RD': '<S6>:248' */
            /* '<S6>:248:1' TargetPoint = RDPoint; */
            rtDW.TargetPoint[0] = rtDW.RDPoint[0];
            rtDW.TargetPoint[1] = rtDW.RDPoint[1];
            rtDW.TargetPoint[2] = rtDW.RDPoint[2];
          }
          break;

         case IN_LU:
          /* During 'LU': '<S6>:246' */
          /* '<S6>:254:1' sf_internal_predicateOutput = ... */
          /* '<S6>:254:1' NextStage; */
          if (*sfEvent == event_NextStage)
          {
            /* Transition: '<S6>:254' */
            rtDW.is_Box = IN_LD;
            rtDW.was_Box = IN_LD;

            /* Entry 'LD': '<S6>:244' */
            /* '<S6>:244:1' TargetPoint = LDPoint; */
            rtDW.TargetPoint[0] = rtDW.LDPoint[0];
            rtDW.TargetPoint[1] = rtDW.LDPoint[1];
            rtDW.TargetPoint[2] = rtDW.LDPoint[2];
          }
          break;

         case IN_RD:
          /* During 'RD': '<S6>:248' */
          /* '<S6>:245:1' sf_internal_predicateOutput = ... */
          /* '<S6>:245:1' NextStage; */
          if (*sfEvent == event_NextStage)
          {
            /* Transition: '<S6>:245' */
            rtDW.is_Box = IN_RU;
            rtDW.was_Box = IN_RU;

            /* Entry 'RU': '<S6>:249' */
            /* '<S6>:249:1' TargetPoint = RUPoint; */
            rtDW.TargetPoint[0] = rtDW.RUPoint[0];
            rtDW.TargetPoint[1] = rtDW.RUPoint[1];
            rtDW.TargetPoint[2] = rtDW.RUPoint[2];
          }
          break;

         case IN_RU:
          /* During 'RU': '<S6>:249' */
          /* '<S6>:252:1' sf_internal_predicateOutput = ... */
          /* '<S6>:252:1' NextStage; */
          if (*sfEvent == event_NextStage)
          {
            /* Transition: '<S6>:252' */
            rtDW.is_Box = IN_LU;
            rtDW.was_Box = IN_LU;

            /* Entry 'LU': '<S6>:246' */
            /* '<S6>:246:1' TargetPoint = LUPoint; */
            rtDW.TargetPoint[0] = rtDW.LUPoint[0];
            rtDW.TargetPoint[1] = rtDW.LUPoint[1];
            rtDW.TargetPoint[2] = rtDW.LUPoint[2];
          }
          break;
        }
      }
      break;

     case IN_Final:
      /* During 'Final': '<S6>:94' */
      /* '<S6>:271:1' sf_internal_predicateOutput = ... */
      /* '<S6>:271:1' CancelFinal; */
      if (*sfEvent == event_CancelFinal)
      {
        /* Transition: '<S6>:271' */
        rtDW.is_Flight = IN_Box;
        rtDW.was_Flight = IN_Box;

        /* Entry 'Box': '<S6>:242' */
        /*  LU - Нацеливание на верхний левый угол "коробки" */
        /*  LD - Нацеливание на нижний левый угол "коробки" */
        /*  RD - Нацеливание на нижний правый угол "коробки" */
        /*  RU - Нацеливание на верхний правый угол "коробки" */
        enter_internal_Box();
      }
      else
      {
        /*  Состояние последнего манёвра */
        /*  Цель которого выйти на заданную */
        /*  точку приземления */
        /* entry: */
        /* '<S6>:94:3' TargetPoint = TDPoint; */
        rtDW.TargetPoint[0] = TmpSignalConversionAtSFunctionI[0];
        rtDW.TargetPoint[1] = TmpSignalConversionAtSFunctionI[1];
        rtDW.TargetPoint[2] = TmpSignalConversionAtSFunctionI[2];
      }
      break;

     case IN_StepAngle:
      /* During 'StepAngle': '<S6>:582' */
      /* '<S6>:593:1' sf_internal_predicateOutput = ... */
      /* '<S6>:593:1' Final; */
      if (*sfEvent == event_Final)
      {
        /* Transition: '<S6>:593' */
        /* Exit Internal 'StepAngle': '<S6>:582' */
        switch (rtDW.is_StepAngle)
        {
         case IN_Left:
          /* Exit 'Left': '<S6>:592' */
          /* '<S6>:592:4' LeftOrRight = true; */
          rtDW.LeftOrRight = true;
          rtDW.is_StepAngle = 0;
          break;

         case IN_Right:
          /* Exit 'Right': '<S6>:590' */
          /* '<S6>:590:3' LeftOrRight = false; */
          rtDW.LeftOrRight = false;
          rtDW.is_StepAngle = 0;
          break;

         default:
          rtDW.is_StepAngle = 0;
          break;
        }

        rtDW.is_Flight = IN_Final;
        rtDW.was_Flight = IN_Final;

        /* Entry 'Final': '<S6>:94' */
        /*  Состояние последнего манёвра */
        /*  Цель которого выйти на заданную */
        /*  точку приземления */
        /* entry: */
        /* '<S6>:94:3' TargetPoint = TDPoint; */
        rtDW.TargetPoint[0] = TmpSignalConversionAtSFunctionI[0];
        rtDW.TargetPoint[1] = TmpSignalConversionAtSFunctionI[1];
        rtDW.TargetPoint[2] = TmpSignalConversionAtSFunctionI[2];
      }
      else
      {
        switch (rtDW.is_StepAngle)
        {
         case IN_Left:
          /* During 'Left': '<S6>:592' */
          /* '<S6>:589:1' sf_internal_predicateOutput = ... */
          /* '<S6>:589:1' timeout == 0; */
          if (rtDW.timeout == 0U)
          {
            /* Transition: '<S6>:589' */
            if (rtDW.is_StepAngle == IN_Left)
            {
              /* Exit 'Left': '<S6>:592' */
              /* '<S6>:592:4' LeftOrRight = true; */
              rtDW.LeftOrRight = true;
              rtDW.is_StepAngle = 0;
            }

            if (rtDW.is_StepAngle != IN_Wait)
            {
              rtDW.is_StepAngle = IN_Wait;

              /* Entry 'Wait': '<S6>:591' */
              /* '<S6>:591:1' timeout = 60; */
              rtDW.timeout = 60U;

              /* '<S6>:591:1' TargetEnable = 1; */
              rtDW.TargetEnable = 1U;
            }
          }
          else
          {
            /* '<S6>:592:3' Course2Target = Angle2Course(Course2Target-pi/2); */
            Course1 = rtDW.Course2Target - 1.5707963267948966;

            /* 'Angle2Course:2' course = angle; */
            rtDW.Course2Target -= 1.5707963267948966;

            /* 'Angle2Course:3' if angle < 0 */
            if (Course1 < 0.0)
            {
              /* 'Angle2Course:4' course = course + 2*pi; */
              rtDW.Course2Target = Course1 + 6.2831853071795862;
            }
          }
          break;

         case IN_Right:
          /* During 'Right': '<S6>:590' */
          /* '<S6>:585:1' sf_internal_predicateOutput = ... */
          /* '<S6>:585:1' timeout == 0; */
          if (rtDW.timeout == 0U)
          {
            /* Transition: '<S6>:585' */
            if (rtDW.is_StepAngle == IN_Right)
            {
              /* Exit 'Right': '<S6>:590' */
              /* '<S6>:590:3' LeftOrRight = false; */
              rtDW.LeftOrRight = false;
              rtDW.is_StepAngle = 0;
            }

            if (rtDW.is_StepAngle != IN_Wait)
            {
              rtDW.is_StepAngle = IN_Wait;

              /* Entry 'Wait': '<S6>:591' */
              /* '<S6>:591:1' timeout = 60; */
              rtDW.timeout = 60U;

              /* '<S6>:591:1' TargetEnable = 1; */
              rtDW.TargetEnable = 1U;
            }
          }
          else
          {
            /* '<S6>:590:3' Course2Target = Angle2Course(Course2Target+pi/2); */
            Course1 = rtDW.Course2Target + 1.5707963267948966;

            /* 'Angle2Course:2' course = angle; */
            rtDW.Course2Target += 1.5707963267948966;

            /* 'Angle2Course:3' if angle < 0 */
            if (Course1 < 0.0)
            {
              /* 'Angle2Course:4' course = course + 2*pi; */
              rtDW.Course2Target = Course1 + 6.2831853071795862;
            }
          }
          break;

         case IN_Wait:
          /* Outport: '<Root>/TimeToTD' incorporates:
           *  Outport: '<Root>/TimeToHorTarget'
           */
          /* During 'Wait': '<S6>:591' */
          /* '<S6>:587:1' sf_internal_predicateOutput = ... */
          /* '<S6>:587:1' (T_ver-T_hor) > 0  && timeout == 0; */
          if ((rtY.TimeToTD - rtY.TimeToHorTarget > 0.0) && (rtDW.timeout == 0U))
          {
            /* Transition: '<S6>:587' */
            /* '<S6>:588:1' sf_internal_predicateOutput = ... */
            /* '<S6>:588:1' ~LeftOrRight; */
            if (!rtDW.LeftOrRight)
            {
              /* Transition: '<S6>:588' */
              rtDW.is_StepAngle = IN_Left;

              /* Entry 'Left': '<S6>:592' */
              /* '<S6>:592:1' timeout = uint32((T_ver - T_hor)*0.7) */
              Course1 = rt_roundd_snf((rtY.TimeToTD - rtY.TimeToHorTarget) * 0.7);
              if (Course1 < 4.294967296E+9)
              {
                if (Course1 >= 0.0)
                {
                  rtDW.timeout = (uint32_T)Course1;
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

              /* '<S6>:592:3' TargetEnable = 1; */
              rtDW.TargetEnable = 1U;
            }
            else
            {
              /* '<S6>:586:1' sf_internal_predicateOutput = ... */
              /* '<S6>:586:1' LeftOrRight; */
              /* Transition: '<S6>:586' */
              rtDW.is_StepAngle = IN_Right;

              /* Entry 'Right': '<S6>:590' */
              /* '<S6>:590:1' timeout = uint32((T_ver - T_hor)*0.7) */
              Course1 = rt_roundd_snf((rtY.TimeToTD - rtY.TimeToHorTarget) * 0.7);
              if (Course1 < 4.294967296E+9)
              {
                if (Course1 >= 0.0)
                {
                  rtDW.timeout = (uint32_T)Course1;
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

              /* '<S6>:590:3' TargetEnable = 1; */
              rtDW.TargetEnable = 1U;
            }
          }

          /* End of Outport: '<Root>/TimeToTD' */
          break;
        }
      }
      break;
    }
    break;

   case IN_Init:
    /* During 'Init': '<S6>:11' */
    /* Transition: '<S6>:25' */
    /* '<S6>:171:1' sf_internal_predicateOutput = ... */
    /* '<S6>:171:1' Mode == 0; */
    if (*Mode == 0)
    {
      /* Transition: '<S6>:171' */
      rtDW.is_Control = IN_TDP_Only;

      /* Entry 'TDP_Only': '<S6>:167' */
      /*  Режим прицеливания на TDP */
      /* '<S6>:167:1' TargetPoint = TDPoint; */
      rtDW.TargetPoint[0] = TmpSignalConversionAtSFunctionI[0];
      rtDW.TargetPoint[1] = TmpSignalConversionAtSFunctionI[1];
      rtDW.TargetPoint[2] = TmpSignalConversionAtSFunctionI[2];

      /* Course2Target = Azimut(TargetPoint, ActualPoint); */
      /* '<S6>:167:3' Course2Target = Heading(TargetPoint, ActualPoint, 0); */
      rtDW.Course2Target = Heading(rtDW.TargetPoint,
        rtDW.TmpSignalConversionAtSFunctio_g, 0.0);
    }
    else
    {
      /* '<S6>:172:1' sf_internal_predicateOutput = ... */
      /* '<S6>:172:1' (Mode == 1 || Mode == 2); */
      if ((*Mode == 1) || (*Mode == 2))
      {
        /* Transition: '<S6>:172' */
        rtDW.is_Control = IN_Flight;

        /* Entry 'Flight': '<S6>:23' */
        /*  Режим полета по точкам */
        /* Course2Target = Azimut(TargetPoint, ActualPoint); */
        /* '<S6>:23:2' Course2Target = Heading(TargetPoint, ActualPoint, 0); */
        rtDW.Course2Target = Heading(rtDW.TargetPoint,
          rtDW.TmpSignalConversionAtSFunctio_g, 0.0);

        /* Entry Internal 'Flight': '<S6>:23' */
        switch (rtDW.was_Flight)
        {
         case IN_Angle:
          if (rtDW.is_Flight != IN_Angle)
          {
            rtDW.is_Flight = IN_Angle;
            rtDW.was_Flight = IN_Angle;

            /* Entry 'Angle': '<S6>:82' */
            /*  Режим углового манёвра */
            /* '<S6>:82:1' TargetPoint = EntryAngleMan (Point1, Point2, ActualPoint, Course); */
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
            Course1 = Heading(rtDW.Point1, rtDW.TmpSignalConversionAtSFunctio_g,
                              0.0) - *y;

            /* 'EntryAngleMan:16' Course2 = Course2 - TrackingCourse; */
            Course2 = Heading(rtDW.Point2, rtDW.TmpSignalConversionAtSFunctio_g,
                              0.0) - *y;

            /*     %% Проведем нормирование разницы */
            /* 'EntryAngleMan:18' if Course1 > pi */
            if (Course1 > 3.1415926535897931)
            {
              /* 'EntryAngleMan:19' Course1 = Course1 - 2*pi; */
              Course1 -= 6.2831853071795862;
            }

            /* 'EntryAngleMan:21' if Course1 < (-pi) */
            if (Course1 < -3.1415926535897931)
            {
              /* 'EntryAngleMan:22' Course1 = Course1 + 2*pi; */
              Course1 += 6.2831853071795862;
            }

            /* 'EntryAngleMan:24' if Course2 > pi */
            if (Course2 > 3.1415926535897931)
            {
              /* 'EntryAngleMan:25' Course2 = Course2 - 2*pi; */
              Course2 -= 6.2831853071795862;
            }

            /* 'EntryAngleMan:27' if Course2 < (-pi) */
            if (Course2 < -3.1415926535897931)
            {
              /* 'EntryAngleMan:28' Course2 = Course2 + 2*pi; */
              Course2 += 6.2831853071795862;
            }

            /*     %% Возьмём модуль для сравнения */
            /* 'EntryAngleMan:31' Course1 = abs(Course1); */
            Course1 = fabs(Course1);

            /* 'EntryAngleMan:32' Course2 = abs(Course2); */
            Course2 = fabs(Course2);

            /*     %% Будет выбрана точка до которой поворот меньше */
            /* 'EntryAngleMan:34' if Course1 < Course2 */
            if (Course1 < Course2)
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

            /* Entry 'Box': '<S6>:242' */
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

            /* Entry 'Final': '<S6>:94' */
            /*  Состояние последнего манёвра */
            /*  Цель которого выйти на заданную */
            /*  точку приземления */
            /* entry: */
            /* '<S6>:94:3' TargetPoint = TDPoint; */
            rtDW.TargetPoint[0] = TmpSignalConversionAtSFunctionI[0];
            rtDW.TargetPoint[1] = TmpSignalConversionAtSFunctionI[1];
            rtDW.TargetPoint[2] = TmpSignalConversionAtSFunctionI[2];
          }
          break;

         case IN_StepAngle:
          if (rtDW.is_Flight != IN_StepAngle)
          {
            rtDW.is_Flight = IN_StepAngle;
            rtDW.was_Flight = IN_StepAngle;

            /* Entry 'StepAngle': '<S6>:582' */
            rtDW.LeftOrRight = false;
          }

          /* Entry Internal 'StepAngle': '<S6>:582' */
          /* Transition: '<S6>:584' */
          if (rtDW.is_StepAngle != IN_Wait)
          {
            rtDW.is_StepAngle = IN_Wait;

            /* Entry 'Wait': '<S6>:591' */
            /* '<S6>:591:1' timeout = 60; */
            rtDW.timeout = 60U;

            /* '<S6>:591:1' TargetEnable = 1; */
            rtDW.TargetEnable = 1U;
          }
          break;

         default:
          /* Transition: '<S6>:268' */
          /* '<S6>:267:1' sf_internal_predicateOutput = ... */
          /* '<S6>:267:1' Mode == 1; */
          switch (*Mode)
          {
           case 1:
            /* Transition: '<S6>:267' */
            if (rtDW.is_Flight != IN_Box)
            {
              rtDW.is_Flight = IN_Box;
              rtDW.was_Flight = IN_Box;

              /* Entry 'Box': '<S6>:242' */
              /*  LU - Нацеливание на верхний левый угол "коробки" */
              /*  LD - Нацеливание на нижний левый угол "коробки" */
              /*  RD - Нацеливание на нижний правый угол "коробки" */
              /*  RU - Нацеливание на верхний правый угол "коробки" */
            }

            enter_internal_Box();
            break;

           case 2:
            /* '<S6>:83:1' sf_internal_predicateOutput = ... */
            /* '<S6>:83:1' Mode == 2; */
            /* Transition: '<S6>:83' */
            if (rtDW.is_Flight != IN_StepAngle)
            {
              rtDW.is_Flight = IN_StepAngle;
              rtDW.was_Flight = IN_StepAngle;

              /* Entry 'StepAngle': '<S6>:582' */
              rtDW.LeftOrRight = false;
            }

            /* Entry Internal 'StepAngle': '<S6>:582' */
            /* Transition: '<S6>:584' */
            if (rtDW.is_StepAngle != IN_Wait)
            {
              rtDW.is_StepAngle = IN_Wait;

              /* Entry 'Wait': '<S6>:591' */
              /* '<S6>:591:1' timeout = 60; */
              rtDW.timeout = 60U;

              /* '<S6>:591:1' TargetEnable = 1; */
              rtDW.TargetEnable = 1U;
            }
            break;
          }
          break;
        }
      }
    }
    break;

   case IN_TDP_Only:
    /* During 'TDP_Only': '<S6>:167' */
    /* '<S6>:167:1' TargetPoint = TDPoint; */
    rtDW.TargetPoint[0] = TmpSignalConversionAtSFunctionI[0];
    rtDW.TargetPoint[1] = TmpSignalConversionAtSFunctionI[1];
    rtDW.TargetPoint[2] = TmpSignalConversionAtSFunctionI[2];

    /* Course2Target = Azimut(TargetPoint, ActualPoint); */
    /* '<S6>:167:3' Course2Target = Heading(TargetPoint, ActualPoint, 0); */
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
static real_T CheckFinalMane(const real_T *VSpeed, const real_T *HSpeed, const
  real_T TmpSignalConversionAtSFunctionI[3])
{
  real_T status;
  real_T fi1;
  real_T fi2;

  /* MATLAB Function 'CheckFinalMane': '<S6>:141' */
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
  /* '<S6>:141:16' Distance = DistanceCalculator(ActualPoint, TDPoint); */
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
  fi2 = TmpSignalConversionAtSFunctionI[0] * 3.1415926535897931 / 180.0;

  /* Outport: '<Root>/DistanceToTDP' */
  /* 'DistanceCalculator:17' d = sin(fi1)*sin(fi2) + cos(fi1)*cos(-fi2)*cos(la2-la1); */
  /*  6371 км средний радиус земли */
  /* 'DistanceCalculator:19' y = acos(d)*6378245; */
  rtY.DistanceToTDP = acos(cos(TmpSignalConversionAtSFunctionI[1] *
    3.1415926535897931 / 180.0 - rtDW.TmpSignalConversionAtSFunctio_g[1] *
    3.1415926535897931 / 180.0) * (cos(fi1) * cos(-fi2)) + sin(fi1) * sin(fi2)) *
    6.378245E+6;

  /* Outport: '<Root>/TimeToHorTarget' incorporates:
   *  DataStoreRead: '<S1>/Data Store Read1'
   *  Outport: '<Root>/DistanceToTDP'
   */
  /* +TDPPopr */
  /*     %% Вычисляем время необходимое для преодоления расстояния до TDP и до посадки */
  /* '<S6>:141:18' T_hor = Distance/HorizontalVelocity; */
  rtY.TimeToHorTarget = rtY.DistanceToTDP / *HSpeed;

  /* Outport: '<Root>/TimeToTD' incorporates:
   *  DataStoreRead: '<S1>/Data Store Read'
   *  Inport: '<Root>/ReliefOnTDP'
   */
  /* '<S6>:141:19' T_ver = (ActualPoint(3) - (TDPoint(3) + double(ReliefOnTDP)))/VerticalVelocity; */
  rtY.TimeToTD = (rtDW.TmpSignalConversionAtSFunctio_g[2] -
                  (TmpSignalConversionAtSFunctionI[2] + (real_T)rtU.ReliefOnTDP))
    / *VSpeed;

  /* Outport: '<Root>/TimeToHorTarget' incorporates:
   *  Outport: '<Root>/TimeToTD'
   */
  /*     %% Определяем поправку по времени необходимому на разворот */
  /*  Определим курс до точки приземления */
  /* '<S6>:141:22' Course2TDP = Heading(TDPoint, ActualPoint, 0); */
  /*  Угол разворота */
  /* '<S6>:141:24' angle = ControlDemode(Course2TDP - Course); */
  /* '<S6>:141:25' angle = abs(angle); */
  /*  Вычисляем поправку как линейную функцию:  */
  /* T_hor_popr = angle / TurnSpeed; */
  /*  Вычисляем поправку по параболе */
  /* T_hor_popr = 0.003*angle^2 + 0.008*angle; */
  /*  Нулевая поправка */
  /* '<S6>:141:33' T_hor_popr = 0; */
  /* '<S6>:141:35' T_hor = T_hor + T_hor_popr; */
  /*     %% Возвращаем сигнал о необходимости перейти на финальный маневр */
  /* '<S6>:141:37' if T_hor > T_ver */
  if (rtY.TimeToHorTarget > rtY.TimeToTD)
  {
    /* '<S6>:141:38' status = 1; */
    status = 1.0;
  }
  else
  {
    /* '<S6>:141:39' else */
    /* '<S6>:141:40' status = 0; */
    status = 0.0;
  }

  return status;
}

/* Function for Chart: '<S1>/TaskController' */
static void BimSupply(const uint32_T *BimTimeout, const boolean_T *DsblTg,
                      int32_T *sfEvent)
{
  /* During 'BimSupply': '<S6>:452' */
  switch (rtDW.is_BimSupply)
  {
   case IN_Disable:
    /* During 'Disable': '<S6>:458' */
    /* '<S6>:464:1' sf_internal_predicateOutput = ... */
    /* '<S6>:464:1' NonBlockingMode; */
    if (*sfEvent == event_NonBlockingMode)
    {
      /* Transition: '<S6>:464' */
      rtDW.is_BimSupply = IN_NonBlockingControl;

      /* Entry 'NonBlockingControl': '<S6>:484' */
      /* '<S6>:484:1' TargetEnable = 1; */
      rtDW.TargetEnable = 1U;
    }
    else
    {
      /* '<S6>:460:1' sf_internal_predicateOutput = ... */
      /* '<S6>:460:1' timeout == 0; */
      if (rtDW.timeout_p == 0U)
      {
        /* Transition: '<S6>:460' */
        rtDW.is_BimSupply = IN_Enable;

        /* Entry 'Enable': '<S6>:456' */
        /* '<S6>:456:1' TargetEnable = 1; */
        rtDW.TargetEnable = 1U;
      }
    }
    break;

   case IN_Enable:
    /* During 'Enable': '<S6>:456' */
    /* '<S6>:459:1' sf_internal_predicateOutput = ... */
    /* '<S6>:459:1' EndOfCorrection  == 1; */
    if (*DsblTg)
    {
      /* Transition: '<S6>:459' */
      rtDW.is_BimSupply = IN_Disable;

      /* Entry 'Disable': '<S6>:458' */
      /* '<S6>:458:1' TargetEnable = 0; */
      rtDW.TargetEnable = 0U;

      /* '<S6>:458:1' timeout = BimTimeout; */
      rtDW.timeout_p = *BimTimeout;
    }
    else
    {
      /* '<S6>:465:1' sf_internal_predicateOutput = ... */
      /* '<S6>:465:1' NonBlockingMode; */
      if (*sfEvent == event_NonBlockingMode)
      {
        /* Transition: '<S6>:465' */
        rtDW.is_BimSupply = IN_NonBlockingControl;

        /* Entry 'NonBlockingControl': '<S6>:484' */
        /* '<S6>:484:1' TargetEnable = 1; */
        rtDW.TargetEnable = 1U;
      }
    }
    break;

   case IN_Init_n0:
    /* During 'Init': '<S6>:603' */
    /* Transition: '<S6>:605' */
    rtDW.is_BimSupply = IN_Enable;

    /* Entry 'Enable': '<S6>:456' */
    /* '<S6>:456:1' TargetEnable = 1; */
    rtDW.TargetEnable = 1U;
    break;

   case IN_NonBlockingControl:
    /* During 'NonBlockingControl': '<S6>:484' */
    /* '<S6>:485:1' sf_internal_predicateOutput = ... */
    /* '<S6>:485:1' Parking; */
    if (*sfEvent == event_Parking)
    {
      /* Transition: '<S6>:485' */
      rtDW.is_BimSupply = IN_Shutdown;

      /* Entry 'Shutdown': '<S6>:461' */
      /* '<S6>:461:1' TargetEnable = 0; */
      rtDW.TargetEnable = 0U;
    }
    else
    {
      /* '<S6>:484:1' TargetEnable = 1; */
      rtDW.TargetEnable = 1U;
    }
    break;

   case IN_Shutdown:
    /* During 'Shutdown': '<S6>:461' */
    /* '<S6>:461:1' TargetEnable = 0; */
    rtDW.TargetEnable = 0U;
    break;
  }
}

/* Model step function */
void Easy_reg_step(void)
{
  int32_T b_previousEvent;
  real_T un[3];
  real_T d_lon;
  real_T un_pow1;
  real_T un_pow2;
  uint32_T BimTimeout;
  uint8_T Mode;
  real_T VSpeed;
  real_T HSpeed;
  boolean_T DsblTg;
  int32_T sfEvent;
  uint32_T qY;
  int16_T tmp;
  boolean_T guard1 = false;

  /* Outputs for Atomic SubSystem: '<Root>/Easy_reg' */
  /* DataStoreWrite: '<S1>/Data Store Write5' incorporates:
   *  Constant: '<S1>/BimTimeout'
   */
  BimTimeout = 30U;

  /* DataStoreWrite: '<S1>/Data Store Write3' incorporates:
   *  Constant: '<S1>/Mode'
   */
  Mode = 2U;

  /* DataStoreWrite: '<S1>/Data Store Write6' incorporates:
   *  Constant: '<S1>/VSpeed'
   */
  VSpeed = 8.0;

  /* DataStoreWrite: '<S1>/Data Store Write7' incorporates:
   *  Constant: '<S1>/HSpeed'
   */
  HSpeed = 20.0;

  /* Outputs for Atomic SubSystem: '<S14>/Bearing' */
  /* MATLAB Function: '<S17>/Heading_true' incorporates:
   *  Delay: '<S14>/LastPos'
   *  Delay: '<S17>/PreviousBearing'
   *  Inport: '<Root>/Pos_lat'
   *  Inport: '<Root>/Pos_lon'
   */
  /*  Высчитаем, как изменились координаты */
  /* MATLAB Function 'Bearing/Heading_true': '<S21>:1' */
  /* '<S21>:1:3' un = u1 - u0; */
  un[0] = rtU.Pos_lat - rtDW.LastPos_1_DSTATE;
  un[1] = rtU.Pos_lon - rtDW.LastPos_2_DSTATE;

  /* '<S21>:1:4' d_lat = un(1); */
  /* '<S21>:1:5' d_lon = un(2); */
  d_lon = un[1];

  /*  Условие для определения кратчайшего пути с учетом переходов между */
  /*  меридианами 0 и 180 по долготе */
  /* '<S21>:1:8' if d_lon > 180 */
  if (un[1] > 180.0)
  {
    /* '<S21>:1:9' d_lon = d_lon - 360; */
    d_lon = un[1] - 360.0;
  }
  else
  {
    if (un[1] < -180.0)
    {
      /* '<S21>:1:10' elseif d_lon < (-180) */
      /* '<S21>:1:11' d_lon = d_lon +360; */
      d_lon = un[1] + 360.0;
    }
  }

  /*  Расчет курса */
  /* '<S21>:1:14' un_pow1 = d_lat*d_lat; */
  un_pow1 = un[0] * un[0];

  /* '<S21>:1:15' un_pow2 = d_lon*d_lon; */
  un_pow2 = d_lon * d_lon;

  /* '<S21>:1:16' if (un_pow1 < 1e-16) && (un_pow2 < 1e-16) */
  if ((un_pow1 < 1.0E-16) && (un_pow2 < 1.0E-16))
  {
    /* '<S21>:1:17' y = last_y; */
    un_pow1 = rtDW.PreviousBearing_DSTATE;
  }
  else
  {
    /* '<S21>:1:18' else */
    /* '<S21>:1:19' x = acos( d_lon / (sqrt(un_pow2 + un_pow1))); */
    /* '<S21>:1:20' y = acos( d_lat / (sqrt(un_pow2 + un_pow1))); */
    un_pow2 = sqrt(un_pow2 + un_pow1);
    un_pow1 = acos(un[0] / un_pow2);

    /* '<S21>:1:21' if x > pi/2 */
    if (acos(d_lon / un_pow2) > 1.5707963267948966)
    {
      /* '<S21>:1:22' y = 2*pi - y; */
      un_pow1 = 6.2831853071795862 - un_pow1;
    }
  }

  /* End of MATLAB Function: '<S17>/Heading_true' */

  /* Update for Delay: '<S17>/PreviousBearing' */
  rtDW.PreviousBearing_DSTATE = un_pow1;

  /* End of Outputs for SubSystem: '<S14>/Bearing' */

  /* Delay: '<S2>/DsblTg' */
  DsblTg = rtDW.DsblTg_DSTATE;

  /* Chart: '<S2>/DriftEstimator' incorporates:
   *  Delay: '<S2>/DsblTg'
   *  Delay: '<S2>/EnblTg'
   */
  /* MATLAB Function 'ProjectionSpeed/Speed': '<S25>:1' */
  /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
  /*  */
  /*    SpeedProjections - Пересчет проекций векторов скорости по lat, lon, alt */
  /*    в горизонтальную и вертикальную скорость полёта */
  /*  */
  /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
  /* '<S25>:1:8' V_hor = sqrt(Vel_lat^2 + Vel_lon^2); */
  /* '<S25>:1:9' V_vert = abs(Vel_alt); */
  /* Gateway: DriftEstimator */
  /* During: DriftEstimator */
  if (rtDW.is_active_c10_BupSimulinkLibrar == 0U)
  {
    /* Entry: DriftEstimator */
    rtDW.is_active_c10_BupSimulinkLibrar = 1U;

    /* Entry Internal: DriftEstimator */
    /* Transition: '<S8>:19' */
    rtDW.is_c10_BupSimulinkLibraries = IN_FixAfterCorrection;

    /* Entry 'FixAfterCorrection': '<S8>:25' */
  }
  else
  {
    switch (rtDW.is_c10_BupSimulinkLibraries)
    {
     case IN_DriftSolver:
      /* During 'DriftSolver': '<S8>:23' */
      /* Transition: '<S8>:20' */
      rtDW.is_c10_BupSimulinkLibraries = IN_FixAfterCorrection;

      /* Entry 'FixAfterCorrection': '<S8>:25' */
      break;

     case IN_FixAfterCorrection:
      /* During 'FixAfterCorrection': '<S8>:25' */
      /* '<S8>:22:1' sf_internal_predicateOutput = ... */
      /* '<S8>:22:1' BimWasDisabled == 1; */
      if (rtDW.DsblTg_DSTATE)
      {
        /* Transition: '<S8>:22' */
        /* Exit 'FixAfterCorrection': '<S8>:25' */
        /* '<S8>:25:1' ReferenceCourse = Course; */
        rtDW.ReferenceCourse = un_pow1;
        rtDW.is_c10_BupSimulinkLibraries = IN_FixBeforeCorrection;

        /* Entry 'FixBeforeCorrection': '<S8>:24' */
      }
      break;

     default:
      /* During 'FixBeforeCorrection': '<S8>:24' */
      /* '<S8>:21:1' sf_internal_predicateOutput = ... */
      /* '<S8>:21:1' BimWasEnabled == 1; */
      if (rtDW.EnblTg_DSTATE)
      {
        /* Transition: '<S8>:21' */
        /* Exit 'FixBeforeCorrection': '<S8>:24' */
        /* '<S8>:24:1' RealCourse = Course; */
        /* '<S8>:24:1' RealSpeed  = Speed; */
        rtDW.is_c10_BupSimulinkLibraries = IN_DriftSolver;

        /* Entry 'DriftSolver': '<S8>:23' */
        /* '<S8>:23:1' [DriftSpeed, DriftAngle] = SpeedTriangle(ReferenceSpeed, RealSpeed, ReferenceCourse, RealCourse); */
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
        /* 'SpeedTriangle:21' WindY = -(TrueAirspeedY - GroundSpeedY); */
        /*     %% Определяем модуль вектора ветра */
        /* 'SpeedTriangle:23' powY = WindY*WindY; */
        /* 'SpeedTriangle:24' powX = WindX*WindX; */
        /* 'SpeedTriangle:25' WindForce = sqrt(powX + powY); */
        /*     %% Расчет направления ветра */
        /*     %%  */
        /* 'SpeedTriangle:28' if (powX < 1e-16) && (powY < 1e-16) */
        /* 'SpeedTriangle:37' WindAngle = y; */
        /* SpeedTrianglePlot(ReferenceSpeed, RealSpeed, ReferenceCourse, RealCourse); */
      }
      break;
    }
  }

  /* End of Chart: '<S2>/DriftEstimator' */

  /* SignalConversion: '<S6>/TmpSignal ConversionAt SFunction Inport1' incorporates:
   *  Chart: '<S1>/TaskController'
   *  Inport: '<Root>/TDP_alt'
   *  Inport: '<Root>/TDP_lat'
   *  Inport: '<Root>/TDP_lon'
   */
  /*     %% Рассчитаем угловое расстояние */
  /* MATLAB Function 'PreemptionTDP/MATLAB Function': '<S13>:1' */
  /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
  /*  */
  /*    PointMovement - Перемещение из точки Position на расстояние  */
  /*    DistanceM (метры) в направлении относительно севера Bearing */
  /*  */
  /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
  /* '<S13>:1:9' AngularDistance = DistanceM/6378160; */
  /*     %% Координаты в радианы */
  /* '<S13>:1:11' Lat1 = Position(1)/180*pi; */
  /* '<S13>:1:12' Lon1 = Position(2)/180*pi; */
  /*     %% Получаем координаты точки перемещения */
  /* '<S13>:1:14' Lat2 = asin(sin(Lat1)*cos(AngularDistance) + cos(Lat1)*sin(AngularDistance)*cos(Bearing)); */
  /* '<S13>:1:15' Lon2 = Lon1 + atan2(sin(Bearing)*sin(AngularDistance)*cos(Lat1), cos(AngularDistance)-sin(Lat1)*sin(Lat2)); */
  /*     %% Переводим в градусы */
  /* '<S13>:1:17' Lat2 = Lat2/pi*180; */
  /* '<S13>:1:18' Lon2 = Lon2/pi*180; */
  /* '<S13>:1:19' NPoint = [Lat2, Lon2, Position(3)]; */
  un[0] = rtU.TDP_lat;
  un[1] = rtU.TDP_lon;
  un[2] = rtU.TDP_alt;

  /* SignalConversion: '<S6>/TmpSignal ConversionAt SFunction Inport2' incorporates:
   *  Chart: '<S1>/TaskController'
   *  Inport: '<Root>/Pos_alt'
   *  Inport: '<Root>/Pos_lat'
   *  Inport: '<Root>/Pos_lon'
   */
  rtDW.TmpSignalConversionAtSFunctio_g[0] = rtU.Pos_lat;
  rtDW.TmpSignalConversionAtSFunctio_g[1] = rtU.Pos_lon;
  rtDW.TmpSignalConversionAtSFunctio_g[2] = rtU.Pos_alt;

  /* Chart: '<S1>/TaskController' incorporates:
   *  Constant: '<S1>/AngleManevr'
   *  Constant: '<S1>/ArrivalRadius'
   *  DataStoreWrite: '<S1>/Data Store Write1'
   *  DataStoreWrite: '<S1>/Data Store Write2'
   *  Inport: '<Root>/Relief'
   *  Inport: '<Root>/TDP_alt'
   *  Inport: '<Root>/TDP_lat'
   *  Inport: '<Root>/TDP_lon'
   */
  /* Gateway: Easy_reg/TaskController */
  sfEvent = -1;

  /* During: Easy_reg/TaskController */
  if (rtDW.is_active_c1_Easy_reg == 0U)
  {
    /* Entry: Easy_reg/TaskController */
    rtDW.is_active_c1_Easy_reg = 1U;

    /* Entry Internal: Easy_reg/TaskController */
    if (rtDW.is_active_Timer != 1U)
    {
      rtDW.is_active_Timer = 1U;

      /* Entry 'Timer': '<S6>:207' */
      /* '<S6>:207:1' time = 0; */
      rtDW.time = 0U;

      /* '<S6>:207:1' BimSupply.timeout = 0; */
      rtDW.timeout_p = 0U;

      /* '<S6>:207:3' Control.Flight.StepAngle.timeout = 0; */
      rtDW.timeout = 0U;
    }

    if (rtDW.is_active_Control != 1U)
    {
      rtDW.is_active_Control = 1U;

      /* Entry 'Control': '<S6>:9' */
      /*  Основной цикл задач */
    }

    /* Entry Internal 'Control': '<S6>:9' */
    /* Transition: '<S6>:12' */
    if (rtDW.is_Control != IN_Init)
    {
      rtDW.is_Control = IN_Init;

      /* Entry 'Init': '<S6>:11' */
      /*  Производим планирование путевых точек для сброса высоты */
      /* '<S6>:11:4' TargetPoint = TDPoint; */
      rtDW.TargetPoint[0] = rtU.TDP_lat;
      rtDW.TargetPoint[1] = rtU.TDP_lon;
      rtDW.TargetPoint[2] = rtU.TDP_alt;

      /* '<S6>:11:4' [Flight.Angle.Point1, Flight.Angle.Point2] = AngleManCreator(ActualPoint, TDPoint, AngleManevr); */
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
      VSpeed = (rtU.TDP_lon - rtDW.TmpSignalConversionAtSFunctio_g[1]) / 2.0;

      /* 'AngleManCreator:14' dy = (y2-y1)/2; */
      HSpeed = (rtU.TDP_lat - rtDW.TmpSignalConversionAtSFunctio_g[0]) / 2.0;

      /*  Определяем первую точку */
      /* 'AngleManCreator:16' x3 = dx*cos(Angle*pi/180) - dy*sin(Angle*pi/180)+x1; */
      /* 'AngleManCreator:17' y3 = dx*sin(Angle*pi/180) + dy*cos(Angle*pi/180)+y1; */
      /* 'AngleManCreator:18' AMP1 = [y3;x3;0]; */
      rtDW.Point1[0] = (VSpeed * 0.17364817766693033 + HSpeed *
                        0.984807753012208) +
        rtDW.TmpSignalConversionAtSFunctio_g[0];
      rtDW.Point1[1] = (VSpeed * 0.984807753012208 - HSpeed *
                        0.17364817766693033) +
        rtDW.TmpSignalConversionAtSFunctio_g[1];
      rtDW.Point1[2] = 0.0;

      /*  Определяем вторую точку */
      /* 'AngleManCreator:20' x4 = dx*cos(-Angle*pi/180) - dy*sin(-Angle*pi/180)+x1; */
      /* 'AngleManCreator:21' y4 = dx*sin(-Angle*pi/180) + dy*cos(-Angle*pi/180)+y1; */
      /* 'AngleManCreator:22' AMP2 = [y4;x4;0]; */
      rtDW.Point2[0] = (VSpeed * -0.17364817766693033 + HSpeed *
                        0.984807753012208) +
        rtDW.TmpSignalConversionAtSFunctio_g[0];
      rtDW.Point2[1] = (VSpeed * 0.984807753012208 - HSpeed *
                        -0.17364817766693033) +
        rtDW.TmpSignalConversionAtSFunctio_g[1];
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

      /* '<S6>:11:5' [Flight.Box.LUPoint, Flight.Box.LDPoint, Flight.Box.RUPoint, Flight.Box.RDPoint] = BOXCreator(TDPoint, BoxSize); */
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
      rtDW.LUPoint[0] = rtU.TDP_lat + 0.009;

      /* 'BOXCreator:16' BLD(1) = TDP(1) - LatStep; */
      rtDW.LDPoint[0] = rtU.TDP_lat - 0.009;

      /* 'BOXCreator:17' BRU(1) = TDP(1) + LatStep; */
      rtDW.RUPoint[0] = rtU.TDP_lat + 0.009;

      /* 'BOXCreator:18' BRD(1) = TDP(1) - LatStep; */
      rtDW.RDPoint[0] = rtU.TDP_lat - 0.009;

      /*  Вычисляем шаг по Долготе без учета поправки на Широту (долгота зависит от */
      /*  широты), а поправку на широту будем делать прямо в выражении */
      /* 'BOXCreator:21' LonStep = BoxStep/1000*360/40000; */
      /*  Координаты по Долготе */
      /* 'BOXCreator:23' BLU(2) = TDP(2) - LonStep/cos(pi*BLU(1)/180); */
      rtDW.LUPoint[1] = rtU.TDP_lon - 0.009 / cos(3.1415926535897931 *
        rtDW.LUPoint[0] / 180.0);

      /* 'BOXCreator:24' BLD(2) = TDP(2) - LonStep/cos(pi*BLD(1)/180); */
      rtDW.LDPoint[1] = rtU.TDP_lon - 0.009 / cos(3.1415926535897931 *
        rtDW.LDPoint[0] / 180.0);

      /* 'BOXCreator:25' BRU(2) = TDP(2) + LonStep/cos(pi*BRU(1)/180); */
      rtDW.RUPoint[1] = 0.009 / cos(3.1415926535897931 * rtDW.RUPoint[0] / 180.0)
        + rtU.TDP_lon;

      /* 'BOXCreator:26' BRD(2) = TDP(2) + LonStep/cos(pi*BRD(1)/180); */
      rtDW.RDPoint[1] = 0.009 / cos(3.1415926535897931 * rtDW.RDPoint[0] / 180.0)
        + rtU.TDP_lon;

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

      /* Entry 'Touchdown': '<S6>:34' */
      /* Здесь мы отдельно будем контролировать высоту, */
      /*  для своевременного открытия посадотчных парашютов */
    }

    /* Entry Internal 'Touchdown': '<S6>:34' */
    /* Transition: '<S6>:37' */
    if (rtDW.is_Touchdown != IN_OFF)
    {
      rtDW.is_Touchdown = IN_OFF;

      /* Outport: '<Root>/TD_CMD' */
      /* Entry 'OFF': '<S6>:38' */
      /* '<S6>:38:1' TD_CMD = 0; */
      rtY.TD_CMD = 0U;
    }

    if (rtDW.is_active_EventGenerator != 1U)
    {
      rtDW.is_active_EventGenerator = 1U;

      /* Entry 'EventGenerator': '<S6>:260' */
    }

    if (rtDW.is_active_BimSupply != 1U)
    {
      rtDW.is_active_BimSupply = 1U;

      /* Entry 'BimSupply': '<S6>:452' */
    }

    /* Entry Internal 'BimSupply': '<S6>:452' */
    /* Transition: '<S6>:457' */
    if (rtDW.is_BimSupply != IN_Init_n0)
    {
      rtDW.is_BimSupply = IN_Init_n0;

      /* Entry 'Init': '<S6>:603' */
      /* '<S6>:603:1' TargetEnable = 0; */
      rtDW.TargetEnable = 0U;
    }
  }
  else
  {
    if (rtDW.is_active_Timer != 0U)
    {
      /* During 'Timer': '<S6>:207' */
      /* '<S6>:207:4' time = time + 1; */
      qY = rtDW.time + /*MW:OvSatOk*/ 1U;
      if (qY < rtDW.time)
      {
        qY = MAX_uint32_T;
      }

      rtDW.time = qY;

      /* '<S6>:207:4' BimSupply.timeout = TimeoutDecrement(BimSupply.timeout); */
      rtDW.timeout_p = TimeoutDecrement(rtDW.timeout_p);

      /* '<S6>:207:5' Control.Flight.StepAngle.timeout = TimeoutDecrement(Control.Flight.StepAngle.timeout); */
      rtDW.timeout = TimeoutDecrement(rtDW.timeout);
    }

    if (rtDW.is_active_Control != 0U)
    {
      Control(&Mode, &un_pow1, un, &sfEvent);
    }

    if (rtDW.is_active_Touchdown != 0U)
    {
      /* During 'Touchdown': '<S6>:34' */
      /* Здесь мы отдельно будем контролировать высоту, */
      /*  для своевременного открытия посадотчных парашютов */
      guard1 = false;
      switch (rtDW.is_Touchdown)
      {
       case IN_OFF:
        /* During 'OFF': '<S6>:38' */
        /* Transition: '<S6>:119' */
        /* '<S6>:43:1' sf_internal_predicateOutput = ... */
        /* '<S6>:43:1' ReliefAvailable == 0; */
        if ((rtU.ReliefAvailable == 0) && (rtDW.TmpSignalConversionAtSFunctio_g
             [2] <= rtU.TDP_alt))
        {
          /* Transition: '<S6>:43' */
          /* '<S6>:128:1' sf_internal_predicateOutput = ... */
          /* '<S6>:128:1' ActualPoint(3) <= TDPoint(3); */
          /* Transition: '<S6>:128' */
          guard1 = true;
        }
        else
        {
          /* '<S6>:125:1' sf_internal_predicateOutput = ... */
          /* '<S6>:125:1' ReliefAvailable ==1; */
          if (rtU.ReliefAvailable == 1)
          {
            /* Transition: '<S6>:125' */
            /* '<S6>:129:1' sf_internal_predicateOutput = ... */
            /* '<S6>:129:1' (ActualPoint(3)-Relief) <= TDPoint(3); */
            d_lon = rt_roundd_snf(rtDW.TmpSignalConversionAtSFunctio_g[2] -
                                  (real_T)rtU.Relief);
            if (d_lon < 32768.0)
            {
              if (d_lon >= -32768.0)
              {
                tmp = (int16_T)d_lon;
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

            if (tmp <= rtU.TDP_alt)
            {
              /* Transition: '<S6>:129' */
              guard1 = true;
            }
            else
            {
              /* Outport: '<Root>/TD_CMD' */
              /* '<S6>:38:1' TD_CMD = 0; */
              rtY.TD_CMD = 0U;
            }
          }
          else
          {
            /* Outport: '<Root>/TD_CMD' */
            /* '<S6>:38:1' TD_CMD = 0; */
            rtY.TD_CMD = 0U;
          }
        }
        break;

       case IN_ON:
        /* Outport: '<Root>/TD_CMD' */
        /* During 'ON': '<S6>:36' */
        /* '<S6>:36:1' TD_CMD = 1; */
        rtY.TD_CMD = 1U;
        break;
      }

      if (guard1)
      {
        rtDW.is_Touchdown = IN_ON;

        /* Outport: '<Root>/TD_CMD' */
        /* Entry 'ON': '<S6>:36' */
        /* '<S6>:36:1' TD_CMD = 1; */
        rtY.TD_CMD = 1U;
      }
    }

    if (rtDW.is_active_EventGenerator != 0U)
    {
      /* During 'EventGenerator': '<S6>:260' */
      /*  Контроль выхода на точку */
      /* '<S6>:260:1' if func.CheckFinalMane  == 1 */
      if (CheckFinalMane(&VSpeed, &HSpeed, un) == 1.0)
      {
        /* '<S6>:260:3' Control.Final; */
        /* Event: '<S6>:361' */
        b_previousEvent = sfEvent;
        sfEvent = event_Final;
        if (rtDW.is_active_Control != 0U)
        {
          Control(&Mode, &un_pow1, un, &sfEvent);
        }

        sfEvent = b_previousEvent;
      }
      else
      {
        /* '<S6>:260:4' else */
        /* '<S6>:260:4' Control.CancelFinal; */
        /* Event: '<S6>:391' */
        b_previousEvent = sfEvent;
        sfEvent = event_CancelFinal;
        if (rtDW.is_active_Control != 0U)
        {
          Control(&Mode, &un_pow1, un, &sfEvent);
        }

        sfEvent = b_previousEvent;
      }

      /*  Контроль попадания в зону текущей цели */
      /* '<S6>:260:9' if CheckArrival(ActualPoint, TargetPoint, ArrivalRadius) == 1 */
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
      VSpeed = rtDW.TmpSignalConversionAtSFunctio_g[0] * 3.1415926535897931 /
        180.0;

      /* 'DistanceCalculator:15' la2 = Target(2)*pi/180; */
      /* 'DistanceCalculator:16' fi2 = Target(1)*pi/180; */
      HSpeed = rtDW.TargetPoint[0] * 3.1415926535897931 / 180.0;

      /* 'DistanceCalculator:17' d = sin(fi1)*sin(fi2) + cos(fi1)*cos(-fi2)*cos(la2-la1); */
      /*  6371 км средний радиус земли */
      /* 'DistanceCalculator:19' y = acos(d)*6378245; */
      /*     %% Попаданием будем считать радиус  */
      /* 'CheckArrival:14' if distance < Radius */
      d_lon = rtDW.TmpSignalConversionAtSFunctio_g[1] * 3.1415926535897931 /
        180.0;
      if (acos(cos(rtDW.TargetPoint[1] * 3.1415926535897931 / 180.0 - d_lon) *
               (cos(VSpeed) * cos(-HSpeed)) + sin(VSpeed) * sin(HSpeed)) *
          6.378245E+6 < 300.0)
      {
        /* 'CheckArrival:15' status = 1; */
        /* '<S6>:260:10' Control.NextStage; */
        /* Event: '<S6>:362' */
        sfEvent = event_NextStage;
        if (rtDW.is_active_Control != 0U)
        {
          Control(&Mode, &un_pow1, un, &sfEvent);
        }

        sfEvent = b_previousEvent;
      }
      else
      {
        /* 'CheckArrival:16' else */
        /* 'CheckArrival:17' status = 0; */
      }

      /*  Контроль попадания в зону точки приземления */
      /*  где на работу БИМов нет ограничений */
      /* '<S6>:260:14' if CheckArrival(ActualPoint, TDPoint, 1000) == 1 */
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
      HSpeed = rtU.TDP_lat * 3.1415926535897931 / 180.0;

      /* 'DistanceCalculator:17' d = sin(fi1)*sin(fi2) + cos(fi1)*cos(-fi2)*cos(la2-la1); */
      /*  6371 км средний радиус земли */
      /* 'DistanceCalculator:19' y = acos(d)*6378245; */
      /*     %% Попаданием будем считать радиус  */
      /* 'CheckArrival:14' if distance < Radius */
      if (acos(cos(rtU.TDP_lon * 3.1415926535897931 / 180.0 - d_lon) * (cos
            (VSpeed) * cos(-HSpeed)) + sin(VSpeed) * sin(HSpeed)) * 6.378245E+6 <
          1000.0)
      {
        /* 'CheckArrival:15' status = 1; */
        /* '<S6>:260:15' BimSupply.NonBlockingMode; */
        /* Event: '<S6>:486' */
        sfEvent = event_NonBlockingMode;
        if (rtDW.is_active_BimSupply != 0U)
        {
          BimSupply(&BimTimeout, &DsblTg, &sfEvent);
        }

        sfEvent = b_previousEvent;
      }
      else
      {
        /* 'CheckArrival:16' else */
        /* 'CheckArrival:17' status = 0; */
      }

      /*  Контроль попадания в зону точки приземления */
      /*  Где управление уже запрещено */
      /* '<S6>:260:18' if CheckArrival(ActualPoint, TDPoint, 300) == 1 */
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
      if (acos(cos(rtU.TDP_lon * 3.1415926535897931 / 180.0 -
                   rtDW.TmpSignalConversionAtSFunctio_g[1] * 3.1415926535897931 /
                   180.0) * (cos(VSpeed) * cos(-HSpeed)) + sin(VSpeed) * sin
               (HSpeed)) * 6.378245E+6 < 300.0)
      {
        /* 'CheckArrival:15' status = 1; */
        /* '<S6>:260:20' BimSupply.Parking; */
        /* Event: '<S6>:467' */
        sfEvent = event_Parking;
        if (rtDW.is_active_BimSupply != 0U)
        {
          BimSupply(&BimTimeout, &DsblTg, &sfEvent);
        }

        sfEvent = b_previousEvent;
      }
      else
      {
        /* 'CheckArrival:16' else */
        /* 'CheckArrival:17' status = 0; */
      }
    }

    if (rtDW.is_active_BimSupply != 0U)
    {
      BimSupply(&BimTimeout, &DsblTg, &sfEvent);
    }
  }

  /* Sum: '<S5>/Sum1' */
  un_pow1 = rtDW.Course2Target - un_pow1;

  /* Outputs for Atomic SubSystem: '<S5>/ControlDemode' */
  /* MATLAB Function: '<S26>/ControlDemode' */
  /*  Здесь переводим угол из диапазона [0 360] */
  /*  в диапазон [-180 180], что позволит нам определять */
  /*  оптимальное направление разворота (лево и право) */
  /* MATLAB Function 'ControlDemode/ControlDemode': '<S32>:1' */
  /* '<S32>:1:5' y = u; */
  /*  Если больше 180 */
  /* '<S32>:1:7' if y > pi */
  if (un_pow1 > 3.1415926535897931)
  {
    /*  то это уже отрицательная полуокружность */
    /* '<S32>:1:9' y = y - 2*pi; */
    un_pow1 -= 6.2831853071795862;

    /*  Если меньше -180 */
  }
  else
  {
    if (un_pow1 < -3.1415926535897931)
    {
      /* '<S32>:1:11' elseif (y < (-pi)) */
      /*  то это уже положительная полуокружность */
      /* '<S32>:1:13' y = y + 2*pi; */
      un_pow1 += 6.2831853071795862;
    }
  }

  /* End of MATLAB Function: '<S26>/ControlDemode' */
  /* End of Outputs for SubSystem: '<S5>/ControlDemode' */

  /* Gain: '<S5>/Base_Gain' incorporates:
   *  Gain: '<S30>/Gain'
   */
  HSpeed = 57.295779513082323 * un_pow1 * 2.0;

  /* Saturate: '<S5>/Base_Sat' */
  if (HSpeed > 50.0)
  {
    HSpeed = 50.0;
  }
  else
  {
    if (HSpeed < -50.0)
    {
      HSpeed = -50.0;
    }
  }

  /* End of Saturate: '<S5>/Base_Sat' */

  /* Product: '<S5>/Product' incorporates:
   *  Gain: '<S31>/Gain'
   *  Gain: '<S31>/Gain1'
   *  Rounding: '<S31>/Rounding Function'
   */
  un_pow1 = floor(0.1 * HSpeed) * 10.0 * (real_T)rtDW.TargetEnable;

  /* Outputs for Atomic SubSystem: '<S2>/BimTriggers' */
  /* Chart: '<S7>/BimTrigger' */
  /* Gateway: BimTriggers/BimTrigger */
  /* During: BimTriggers/BimTrigger */
  if (rtDW.is_active_c9_BupSimulinkLibrari == 0U)
  {
    /* Entry: BimTriggers/BimTrigger */
    rtDW.is_active_c9_BupSimulinkLibrari = 1U;

    /* Entry Internal: BimTriggers/BimTrigger */
    /* Entry 'Triggers': '<S11>:18' */
    /* Entry Internal 'Triggers': '<S11>:18' */
    /* Entry 'DisabledTrigger': '<S11>:19' */
    /* '<S11>:19:1' if LastLineState > 0 && Line <=0 */
    if ((rtDW.LastLineState > 0.0) && (un_pow1 <= 0.0))
    {
      /* Update for Delay: '<S2>/DsblTg' */
      /* '<S11>:19:1' Disabled = true; */
      rtDW.DsblTg_DSTATE = true;
    }
    else if ((rtDW.LastLineState < 0.0) && (un_pow1 >= 0.0))
    {
      /* Update for Delay: '<S2>/DsblTg' */
      /* '<S11>:19:2' elseif LastLineState < 0 && Line >=0 */
      /* '<S11>:19:4' Disabled = true; */
      rtDW.DsblTg_DSTATE = true;
    }
    else
    {
      /* Update for Delay: '<S2>/DsblTg' */
      /* '<S11>:19:4' else */
      /* '<S11>:19:4' Disabled = false; */
      rtDW.DsblTg_DSTATE = false;
    }

    /* Entry 'EnabledTrigger1': '<S11>:21' */
    /* '<S11>:21:1' if LastLineState == 0 && Line > 0 */
    if ((rtDW.LastLineState == 0.0) && (un_pow1 > 0.0))
    {
      /* Update for Delay: '<S2>/EnblTg' */
      /* '<S11>:21:1' Enabled = true; */
      rtDW.EnblTg_DSTATE = true;
    }
    else if ((rtDW.LastLineState == 0.0) && (un_pow1 < 0.0))
    {
      /* Update for Delay: '<S2>/EnblTg' */
      /* '<S11>:21:2' elseif LastLineState == 0 && Line < 0 */
      /* '<S11>:21:4' Enabled = true; */
      rtDW.EnblTg_DSTATE = true;
    }
    else
    {
      /* Update for Delay: '<S2>/EnblTg' */
      /* '<S11>:21:4' else */
      /* '<S11>:21:4' Enabled = false; */
      rtDW.EnblTg_DSTATE = false;
    }

    /* Entry 'RemeberLastState': '<S11>:29' */
    /* '<S11>:29:1' LastLineState = Line; */
    rtDW.LastLineState = un_pow1;
  }
  else
  {
    /* During 'Triggers': '<S11>:18' */
    /* During 'DisabledTrigger': '<S11>:19' */
    /* '<S11>:19:1' if LastLineState > 0 && Line <=0 */
    if ((rtDW.LastLineState > 0.0) && (un_pow1 <= 0.0))
    {
      /* Update for Delay: '<S2>/DsblTg' */
      /* '<S11>:19:1' Disabled = true; */
      rtDW.DsblTg_DSTATE = true;
    }
    else if ((rtDW.LastLineState < 0.0) && (un_pow1 >= 0.0))
    {
      /* Update for Delay: '<S2>/DsblTg' */
      /* '<S11>:19:2' elseif LastLineState < 0 && Line >=0 */
      /* '<S11>:19:3' Disabled = true; */
      rtDW.DsblTg_DSTATE = true;
    }
    else
    {
      /* Update for Delay: '<S2>/DsblTg' */
      /* '<S11>:19:4' else */
      /* '<S11>:19:4' Disabled = false; */
      rtDW.DsblTg_DSTATE = false;
    }

    /* During 'EnabledTrigger1': '<S11>:21' */
    /* '<S11>:21:1' if LastLineState == 0 && Line > 0 */
    if ((rtDW.LastLineState == 0.0) && (un_pow1 > 0.0))
    {
      /* Update for Delay: '<S2>/EnblTg' */
      /* '<S11>:21:1' Enabled = true; */
      rtDW.EnblTg_DSTATE = true;
    }
    else if ((rtDW.LastLineState == 0.0) && (un_pow1 < 0.0))
    {
      /* Update for Delay: '<S2>/EnblTg' */
      /* '<S11>:21:2' elseif LastLineState == 0 && Line < 0 */
      /* '<S11>:21:3' Enabled = true; */
      rtDW.EnblTg_DSTATE = true;
    }
    else
    {
      /* Update for Delay: '<S2>/EnblTg' */
      /* '<S11>:21:4' else */
      /* '<S11>:21:4' Enabled = false; */
      rtDW.EnblTg_DSTATE = false;
    }

    /* During 'RemeberLastState': '<S11>:29' */
    /* '<S11>:29:1' LastLineState = Line; */
    rtDW.LastLineState = un_pow1;
  }

  /* End of Chart: '<S7>/BimTrigger' */
  /* End of Outputs for SubSystem: '<S2>/BimTriggers' */

  /* Update for Delay: '<S14>/LastPos' incorporates:
   *  Inport: '<Root>/Pos_lat'
   *  Inport: '<Root>/Pos_lon'
   */
  /*  u0 - точка отсчета */
  /*  u1 - конечная точка */
  /*  la - долгота */
  /*  fi - широта */
  /* MATLAB Function 'Azimuth/Azimut': '<S20>:1' */
  /* '<S20>:1:6' la1 = u0(2)*pi/180; */
  /* '<S20>:1:7' fi1 = u0(1)*pi/180; */
  /* '<S20>:1:8' la2 = u1(2)*pi/180; */
  /* '<S20>:1:9' fi2 = u1(1)*pi/180; */
  /* '<S20>:1:10' delta_la = la2-la1; */
  /* '<S20>:1:11' y = atan2(sin(delta_la)*cos(fi2), cos(fi1)*sin(fi2)-sin(fi1)*cos(fi2)*cos(delta_la)); */
  /* '<S20>:1:12' y = rem((y + 2*pi), 2*pi); */
  /* MATLAB Function 'GPSVelocity/Velocity': '<S24>:1' */
  /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
  /*  */
  /*    GPSPos2Speed - Определение скорости по сигналам GPS */
  /*    u1 - Точка относительно, которой считаем перемещение (lat, lon, alt) */
  /*    u0 - Точка до которой отладываем перемещение (lat, lon, alt) */
  /*    time - Время между замерами координат */
  /*  */
  /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
  /* '<S24>:1:10' un = u1-u0; */
  /* '<S24>:1:11' v_lat = un(1)*40000/360*1000/time; */
  /* '<S24>:1:12' v_lon = un(2)*(40000/360*1000/time * cos(pi/180*u0(1))); */
  /* '<S24>:1:13' v_alt = un(3); */
  /* MATLAB Function 'DeadZone/DeadZone': '<S34>:1' */
  /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
  /*  */
  /*    Звено нечувствительности */
  /*  */
  /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
  /* '<S34>:1:7' if abs(u) < 20 */
  /*  if u > 10 */
  /*      y = 50; */
  /*  elseif u < - 10 */
  /*      y = -50; */
  /*  else */
  /*      y = 0; */
  /*  end */
  rtDW.LastPos_1_DSTATE = rtU.Pos_lat;
  rtDW.LastPos_2_DSTATE = rtU.Pos_lon;

  /* End of Outputs for SubSystem: '<Root>/Easy_reg' */

  /* Outport: '<Root>/BIM_CMD' */
  rtY.BIM_CMD = un_pow1;
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
