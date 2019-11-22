/*
 * File: Easy_reg.c
 *
 * Code generated for Simulink model 'Easy_reg'.
 *
 * Model version                  : 1.418
 * Simulink Coder version         : 8.14 (R2018a) 06-Feb-2018
 * C/C++ source code generated on : Thu Nov 21 16:08:32 2019
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
#define IN_Disable                     ((uint8_T)1U)
#define IN_Enable                      ((uint8_T)2U)
#define IN_Final                       ((uint8_T)3U)
#define IN_Flight                      ((uint8_T)1U)
#define IN_Init                        ((uint8_T)2U)
#define IN_Init_n                      ((uint8_T)1U)
#define IN_LD                          ((uint8_T)2U)
#define IN_LU                          ((uint8_T)3U)
#define IN_NonBlockingControl          ((uint8_T)3U)
#define IN_OFF                         ((uint8_T)1U)
#define IN_ON                          ((uint8_T)2U)
#define IN_RD                          ((uint8_T)4U)
#define IN_RU                          ((uint8_T)5U)
#define IN_Shutdown                    ((uint8_T)4U)
#define IN_TDP_Only                    ((uint8_T)3U)
#define event_CancelFinal              (0)
#define event_Final                    (1)
#define event_NextStage                (2)
#define event_NonBlockingMode          (3)
#define event_Parking                  (4)
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
static void Bearing(real_T rtu_to, real_T rtu_to_e, real_T rtu_from, real_T
                    rtu_from_l, real_T *rty_Bearing, DW_Bearing *localDW);

/* Forward declaration for local functions */
static uint8_T VerticeSelect(const real_T AMP[3], const real_T BLU[3], const
  real_T BLD[3], const real_T BRU[3], const real_T BRD[3]);
static void enter_internal_Box(void);
static real_T Heading(const real_T u1[3], const real_T u0[3], real_T last_y);
static void enter_atomic_Angle(const real_T *y);
static void Control(const uint8_T *Mode, const real_T *y, const real_T
                    TmpSignalConversionAtSFunctionI[3], int32_T *sfEvent);
static real_T CheckFinalMane(const real_T *TurnSpeed, const real_T *y, const
  real_T TmpSignalConversionAtSFunctionI[3], real_T *y_p);
static void BimSupply(const boolean_T *Delay2, int32_T *sfEvent);

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

/*
 * Output and update for atomic system:
 *    '<S13>/Bearing'
 *    '<S14>/Bearing'
 */
static void Bearing(real_T rtu_to, real_T rtu_to_e, real_T rtu_from, real_T
                    rtu_from_l, real_T *rty_Bearing, DW_Bearing *localDW)
{
  real_T un_pow1;
  real_T un_pow2;
  real_T un_idx_0;
  real_T un_idx_1;

  /* MATLAB Function: '<S16>/Heading_true' incorporates:
   *  Delay: '<S16>/PreviousBearing'
   *  SignalConversion: '<S17>/TmpSignal ConversionAt SFunction Inport1'
   */
  /*  Высчитаем, как изменились координаты */
  /* MATLAB Function 'Bearing/Heading_true': '<S17>:1' */
  /* '<S17>:1:3' un = u1 - u0; */
  un_idx_0 = rtu_to - rtu_from;
  un_idx_1 = rtu_to_e - rtu_from_l;

  /* '<S17>:1:4' d_lat = un(1); */
  /* '<S17>:1:5' d_lon = un(2); */
  /*  Условие для определения кратчайшего пути с учетом переходов между */
  /*  меридианами 0 и 180 по долготе */
  /* '<S17>:1:8' if d_lon > 180 */
  if (un_idx_1 > 180.0) {
    /* '<S17>:1:9' d_lon = d_lon - 360; */
    un_idx_1 -= 360.0;
  } else {
    if (un_idx_1 < -180.0) {
      /* '<S17>:1:10' elseif d_lon < (-180) */
      /* '<S17>:1:11' d_lon = d_lon +360; */
      un_idx_1 += 360.0;
    }
  }

  /*  Расчет курса */
  /* '<S17>:1:14' un_pow1 = d_lat*d_lat; */
  un_pow1 = un_idx_0 * un_idx_0;

  /* '<S17>:1:15' un_pow2 = d_lon*d_lon; */
  un_pow2 = un_idx_1 * un_idx_1;

  /* '<S17>:1:16' if (un_pow1 < 1e-16) && (un_pow2 < 1e-16) */
  if ((un_pow1 < 1.0E-16) && (un_pow2 < 1.0E-16)) {
    /* '<S17>:1:17' y = last_y; */
    *rty_Bearing = localDW->PreviousBearing_DSTATE;
  } else {
    /* '<S17>:1:18' else */
    /* '<S17>:1:19' x = acos( d_lon / (sqrt(un_pow2 + un_pow1))); */
    /* '<S17>:1:20' y = acos( d_lat / (sqrt(un_pow2 + un_pow1))); */
    un_pow1 = sqrt(un_pow2 + un_pow1);
    *rty_Bearing = un_idx_0 / un_pow1;
    *rty_Bearing = acos(*rty_Bearing);

    /* '<S17>:1:21' if x > pi/2 */
    if (acos(un_idx_1 / un_pow1) > 1.5707963267948966) {
      /* '<S17>:1:22' y = 2*pi - y; */
      *rty_Bearing = 6.2831853071795862 - *rty_Bearing;
    }
  }

  /* End of MATLAB Function: '<S16>/Heading_true' */

  /* Update for Delay: '<S16>/PreviousBearing' */
  localDW->PreviousBearing_DSTATE = *rty_Bearing;
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
  if (x[1] < x[0]) {
    /* 'VerticeSelect:28' Vertice = i+1; */
    b_Vertice = 1;
  }

  /* 'VerticeSelect:27' if x(i+1) < x(Vertice) */
  if (x[2] < x[b_Vertice]) {
    /* 'VerticeSelect:28' Vertice = i+1; */
    b_Vertice = 2;
  }

  /* 'VerticeSelect:27' if x(i+1) < x(Vertice) */
  if (x[3] < x[b_Vertice]) {
    /* 'VerticeSelect:28' Vertice = i+1; */
    b_Vertice = 3;
  }

  /* 'VerticeSelect:31' Vertice = uint8(Vertice); */
  return (uint8_T)(b_Vertice + 1);
}

/* Function for Chart: '<S1>/TaskController' */
static void enter_internal_Box(void)
{
  /* Entry Internal 'Box': '<S10>:242' */
  switch (Easy_reg_DW.was_Box) {
   case IN_Init_n:
    if (Easy_reg_DW.is_Box != IN_Init_n) {
      Easy_reg_DW.is_Box = IN_Init_n;
      Easy_reg_DW.was_Box = IN_Init_n;

      /* Entry 'Init': '<S10>:393' */
      /* '<S10>:393:1' BoxEntry = VerticeSelect(ActualPoint, LUPoint, LDPoint, RUPoint, RDPoint); */
      Easy_reg_DW.BoxEntry = VerticeSelect
        (Easy_reg_DW.TmpSignalConversionAtSFunctio_g, Easy_reg_DW.LUPoint,
         Easy_reg_DW.LDPoint, Easy_reg_DW.RUPoint, Easy_reg_DW.RDPoint);
    }
    break;

   case IN_LD:
    if (Easy_reg_DW.is_Box != IN_LD) {
      Easy_reg_DW.is_Box = IN_LD;
      Easy_reg_DW.was_Box = IN_LD;

      /* Entry 'LD': '<S10>:244' */
      /* '<S10>:244:1' TargetPoint = LDPoint; */
      Easy_reg_DW.TargetPoint[0] = Easy_reg_DW.LDPoint[0];
      Easy_reg_DW.TargetPoint[1] = Easy_reg_DW.LDPoint[1];
      Easy_reg_DW.TargetPoint[2] = Easy_reg_DW.LDPoint[2];
    }
    break;

   case IN_LU:
    if (Easy_reg_DW.is_Box != IN_LU) {
      Easy_reg_DW.is_Box = IN_LU;
      Easy_reg_DW.was_Box = IN_LU;

      /* Entry 'LU': '<S10>:246' */
      /* '<S10>:246:1' TargetPoint = LUPoint; */
      Easy_reg_DW.TargetPoint[0] = Easy_reg_DW.LUPoint[0];
      Easy_reg_DW.TargetPoint[1] = Easy_reg_DW.LUPoint[1];
      Easy_reg_DW.TargetPoint[2] = Easy_reg_DW.LUPoint[2];
    }
    break;

   case IN_RD:
    if (Easy_reg_DW.is_Box != IN_RD) {
      Easy_reg_DW.is_Box = IN_RD;
      Easy_reg_DW.was_Box = IN_RD;

      /* Entry 'RD': '<S10>:248' */
      /* '<S10>:248:1' TargetPoint = RDPoint; */
      Easy_reg_DW.TargetPoint[0] = Easy_reg_DW.RDPoint[0];
      Easy_reg_DW.TargetPoint[1] = Easy_reg_DW.RDPoint[1];
      Easy_reg_DW.TargetPoint[2] = Easy_reg_DW.RDPoint[2];
    }
    break;

   case IN_RU:
    if (Easy_reg_DW.is_Box != IN_RU) {
      Easy_reg_DW.is_Box = IN_RU;
      Easy_reg_DW.was_Box = IN_RU;

      /* Entry 'RU': '<S10>:249' */
      /* '<S10>:249:1' TargetPoint = RUPoint; */
      Easy_reg_DW.TargetPoint[0] = Easy_reg_DW.RUPoint[0];
      Easy_reg_DW.TargetPoint[1] = Easy_reg_DW.RUPoint[1];
      Easy_reg_DW.TargetPoint[2] = Easy_reg_DW.RUPoint[2];
    }
    break;

   default:
    /* Transition: '<S10>:394' */
    if (Easy_reg_DW.is_Box != IN_Init_n) {
      Easy_reg_DW.is_Box = IN_Init_n;
      Easy_reg_DW.was_Box = IN_Init_n;

      /* Entry 'Init': '<S10>:393' */
      /* '<S10>:393:1' BoxEntry = VerticeSelect(ActualPoint, LUPoint, LDPoint, RUPoint, RDPoint); */
      Easy_reg_DW.BoxEntry = VerticeSelect
        (Easy_reg_DW.TmpSignalConversionAtSFunctio_g, Easy_reg_DW.LUPoint,
         Easy_reg_DW.LDPoint, Easy_reg_DW.RUPoint, Easy_reg_DW.RDPoint);
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
  if (un_idx_1 > 180.0) {
    /* 'Heading:17' d_lon = d_lon - 360; */
    un_idx_1 -= 360.0;
  } else {
    if (un_idx_1 < -180.0) {
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
  if ((un_pow1 < 1.0E-16) && (un_pow2 < 1.0E-16)) {
    /* 'Heading:25' y = last_y; */
    y = last_y;
  } else {
    /* 'Heading:26' else */
    /* 'Heading:27' x = acos( d_lon / (sqrt(un_pow2 + un_pow1))); */
    /* 'Heading:28' y = acos( d_lat / (sqrt(un_pow2 + un_pow1))); */
    un_pow1 = sqrt(un_pow2 + un_pow1);
    y = acos(un_idx_0 / un_pow1);

    /* 'Heading:29' if x > pi/2 */
    if (acos(un_idx_1 / un_pow1) > 1.5707963267948966) {
      /* 'Heading:30' y = 2*pi - y; */
      y = 6.2831853071795862 - y;
    }
  }

  return y;
}

/* Function for Chart: '<S1>/TaskController' */
static void enter_atomic_Angle(const real_T *y)
{
  real_T Course1;
  real_T Course2;

  /* Entry 'Angle': '<S10>:82' */
  /*  Режим углового манёвра */
  /* '<S10>:82:1' TargetPoint = EntryAngleMan (Point1, Point2, ActualPoint, Course); */
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
  Course1 = Heading(Easy_reg_DW.Point1,
                    Easy_reg_DW.TmpSignalConversionAtSFunctio_g, 0.0) - *y;

  /* 'EntryAngleMan:16' Course2 = Course2 - TrackingCourse; */
  Course2 = Heading(Easy_reg_DW.Point2,
                    Easy_reg_DW.TmpSignalConversionAtSFunctio_g, 0.0) - *y;

  /*     %% Проведем нормирование разницы */
  /* 'EntryAngleMan:18' if Course1 > pi */
  if (Course1 > 3.1415926535897931) {
    /* 'EntryAngleMan:19' Course1 = Course1 - 2*pi; */
    Course1 -= 6.2831853071795862;
  }

  /* 'EntryAngleMan:21' if Course1 < (-pi) */
  if (Course1 < -3.1415926535897931) {
    /* 'EntryAngleMan:22' Course1 = Course1 + 2*pi; */
    Course1 += 6.2831853071795862;
  }

  /* 'EntryAngleMan:24' if Course2 > pi */
  if (Course2 > 3.1415926535897931) {
    /* 'EntryAngleMan:25' Course2 = Course2 - 2*pi; */
    Course2 -= 6.2831853071795862;
  }

  /* 'EntryAngleMan:27' if Course2 < (-pi) */
  if (Course2 < -3.1415926535897931) {
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
  if (Course1 < Course2) {
    /* 'EntryAngleMan:35' point = AM1; */
    Easy_reg_DW.TargetPoint[0] = Easy_reg_DW.Point1[0];
    Easy_reg_DW.TargetPoint[1] = Easy_reg_DW.Point1[1];
    Easy_reg_DW.TargetPoint[2] = Easy_reg_DW.Point1[2];
  } else {
    /* 'EntryAngleMan:36' else */
    /* 'EntryAngleMan:37' point = AM2; */
    Easy_reg_DW.TargetPoint[0] = Easy_reg_DW.Point2[0];
    Easy_reg_DW.TargetPoint[1] = Easy_reg_DW.Point2[1];
    Easy_reg_DW.TargetPoint[2] = Easy_reg_DW.Point2[2];
  }
}

/* Function for Chart: '<S1>/TaskController' */
static void Control(const uint8_T *Mode, const real_T *y, const real_T
                    TmpSignalConversionAtSFunctionI[3], int32_T *sfEvent)
{
  /* During 'Control': '<S10>:9' */
  /*  Основной цикл задач */
  switch (Easy_reg_DW.is_Control) {
   case IN_Flight:
    /* During 'Flight': '<S10>:23' */
    switch (Easy_reg_DW.is_Flight) {
     case IN_Angle:
      /* During 'Angle': '<S10>:82' */
      /* '<S10>:89:1' sf_internal_predicateOutput = ... */
      /* '<S10>:89:1' NextStage; */
      switch (*sfEvent) {
       case event_NextStage:
        /* Transition: '<S10>:89' */
        Easy_reg_DW.is_Flight = IN_Box;
        Easy_reg_DW.was_Flight = IN_Box;

        /* Entry 'Box': '<S10>:242' */
        /*  LU - Нацеливание на верхний левый угол "коробки" */
        /*  LD - Нацеливание на нижний левый угол "коробки" */
        /*  RD - Нацеливание на нижний правый угол "коробки" */
        /*  RU - Нацеливание на верхний правый угол "коробки" */
        enter_internal_Box();
        break;

       case event_Final:
        /* '<S10>:258:1' sf_internal_predicateOutput = ... */
        /* '<S10>:258:1' Final; */
        /* Transition: '<S10>:258' */
        Easy_reg_DW.is_Flight = IN_Final;
        Easy_reg_DW.was_Flight = IN_Final;

        /* Entry 'Final': '<S10>:94' */
        /*  Состояние последнего манёвра */
        /*  Цель которого выйти на заданную */
        /*  точку приземления */
        /* '<S10>:94:3' TargetPoint = TDPoint; */
        Easy_reg_DW.TargetPoint[0] = TmpSignalConversionAtSFunctionI[0];
        Easy_reg_DW.TargetPoint[1] = TmpSignalConversionAtSFunctionI[1];
        Easy_reg_DW.TargetPoint[2] = TmpSignalConversionAtSFunctionI[2];
        break;
      }
      break;

     case IN_Box:
      /* During 'Box': '<S10>:242' */
      /* '<S10>:108:1' sf_internal_predicateOutput = ... */
      /* '<S10>:108:1' Final; */
      if (*sfEvent == event_Final) {
        /* Transition: '<S10>:108' */
        /* Exit Internal 'Box': '<S10>:242' */
        Easy_reg_DW.is_Box = 0;
        Easy_reg_DW.is_Flight = IN_Final;
        Easy_reg_DW.was_Flight = IN_Final;

        /* Entry 'Final': '<S10>:94' */
        /*  Состояние последнего манёвра */
        /*  Цель которого выйти на заданную */
        /*  точку приземления */
        /* '<S10>:94:3' TargetPoint = TDPoint; */
        Easy_reg_DW.TargetPoint[0] = TmpSignalConversionAtSFunctionI[0];
        Easy_reg_DW.TargetPoint[1] = TmpSignalConversionAtSFunctionI[1];
        Easy_reg_DW.TargetPoint[2] = TmpSignalConversionAtSFunctionI[2];
      } else {
        /*  LU - Нацеливание на верхний левый угол "коробки" */
        /*  LD - Нацеливание на нижний левый угол "коробки" */
        /*  RD - Нацеливание на нижний правый угол "коробки" */
        /*  RU - Нацеливание на верхний правый угол "коробки" */
        switch (Easy_reg_DW.is_Box) {
         case IN_Init_n:
          /* During 'Init': '<S10>:393' */
          /* Transition: '<S10>:256' */
          /* '<S10>:243:1' sf_internal_predicateOutput = ... */
          /* '<S10>:243:1' BoxEntry == 1; */
          switch (Easy_reg_DW.BoxEntry) {
           case 1:
            /* Transition: '<S10>:243' */
            Easy_reg_DW.is_Box = IN_LU;
            Easy_reg_DW.was_Box = IN_LU;

            /* Entry 'LU': '<S10>:246' */
            /* '<S10>:246:1' TargetPoint = LUPoint; */
            Easy_reg_DW.TargetPoint[0] = Easy_reg_DW.LUPoint[0];
            Easy_reg_DW.TargetPoint[1] = Easy_reg_DW.LUPoint[1];
            Easy_reg_DW.TargetPoint[2] = Easy_reg_DW.LUPoint[2];
            break;

           case 4:
            /* '<S10>:255:1' sf_internal_predicateOutput = ... */
            /* '<S10>:255:1' BoxEntry == 4; */
            /* Transition: '<S10>:255' */
            Easy_reg_DW.is_Box = IN_RD;
            Easy_reg_DW.was_Box = IN_RD;

            /* Entry 'RD': '<S10>:248' */
            /* '<S10>:248:1' TargetPoint = RDPoint; */
            Easy_reg_DW.TargetPoint[0] = Easy_reg_DW.RDPoint[0];
            Easy_reg_DW.TargetPoint[1] = Easy_reg_DW.RDPoint[1];
            Easy_reg_DW.TargetPoint[2] = Easy_reg_DW.RDPoint[2];
            break;

           case 2:
            /* '<S10>:251:1' sf_internal_predicateOutput = ... */
            /* '<S10>:251:1' BoxEntry == 2; */
            /* Transition: '<S10>:251' */
            Easy_reg_DW.is_Box = IN_LD;
            Easy_reg_DW.was_Box = IN_LD;

            /* Entry 'LD': '<S10>:244' */
            /* '<S10>:244:1' TargetPoint = LDPoint; */
            Easy_reg_DW.TargetPoint[0] = Easy_reg_DW.LDPoint[0];
            Easy_reg_DW.TargetPoint[1] = Easy_reg_DW.LDPoint[1];
            Easy_reg_DW.TargetPoint[2] = Easy_reg_DW.LDPoint[2];
            break;

           case 3:
            /* '<S10>:253:1' sf_internal_predicateOutput = ... */
            /* '<S10>:253:1' BoxEntry == 3; */
            /* Transition: '<S10>:253' */
            Easy_reg_DW.is_Box = IN_RU;
            Easy_reg_DW.was_Box = IN_RU;

            /* Entry 'RU': '<S10>:249' */
            /* '<S10>:249:1' TargetPoint = RUPoint; */
            Easy_reg_DW.TargetPoint[0] = Easy_reg_DW.RUPoint[0];
            Easy_reg_DW.TargetPoint[1] = Easy_reg_DW.RUPoint[1];
            Easy_reg_DW.TargetPoint[2] = Easy_reg_DW.RUPoint[2];
            break;

           default:
            /* '<S10>:393:1' BoxEntry = VerticeSelect(ActualPoint, LUPoint, LDPoint, RUPoint, RDPoint); */
            Easy_reg_DW.BoxEntry = VerticeSelect
              (Easy_reg_DW.TmpSignalConversionAtSFunctio_g, Easy_reg_DW.LUPoint,
               Easy_reg_DW.LDPoint, Easy_reg_DW.RUPoint, Easy_reg_DW.RDPoint);
            break;
          }
          break;

         case IN_LD:
          /* During 'LD': '<S10>:244' */
          /* '<S10>:250:1' sf_internal_predicateOutput = ... */
          /* '<S10>:250:1' NextStage; */
          if (*sfEvent == event_NextStage) {
            /* Transition: '<S10>:250' */
            Easy_reg_DW.is_Box = IN_RD;
            Easy_reg_DW.was_Box = IN_RD;

            /* Entry 'RD': '<S10>:248' */
            /* '<S10>:248:1' TargetPoint = RDPoint; */
            Easy_reg_DW.TargetPoint[0] = Easy_reg_DW.RDPoint[0];
            Easy_reg_DW.TargetPoint[1] = Easy_reg_DW.RDPoint[1];
            Easy_reg_DW.TargetPoint[2] = Easy_reg_DW.RDPoint[2];
          }
          break;

         case IN_LU:
          /* During 'LU': '<S10>:246' */
          /* '<S10>:254:1' sf_internal_predicateOutput = ... */
          /* '<S10>:254:1' NextStage; */
          if (*sfEvent == event_NextStage) {
            /* Transition: '<S10>:254' */
            Easy_reg_DW.is_Box = IN_LD;
            Easy_reg_DW.was_Box = IN_LD;

            /* Entry 'LD': '<S10>:244' */
            /* '<S10>:244:1' TargetPoint = LDPoint; */
            Easy_reg_DW.TargetPoint[0] = Easy_reg_DW.LDPoint[0];
            Easy_reg_DW.TargetPoint[1] = Easy_reg_DW.LDPoint[1];
            Easy_reg_DW.TargetPoint[2] = Easy_reg_DW.LDPoint[2];
          }
          break;

         case IN_RD:
          /* During 'RD': '<S10>:248' */
          /* '<S10>:245:1' sf_internal_predicateOutput = ... */
          /* '<S10>:245:1' NextStage; */
          if (*sfEvent == event_NextStage) {
            /* Transition: '<S10>:245' */
            Easy_reg_DW.is_Box = IN_RU;
            Easy_reg_DW.was_Box = IN_RU;

            /* Entry 'RU': '<S10>:249' */
            /* '<S10>:249:1' TargetPoint = RUPoint; */
            Easy_reg_DW.TargetPoint[0] = Easy_reg_DW.RUPoint[0];
            Easy_reg_DW.TargetPoint[1] = Easy_reg_DW.RUPoint[1];
            Easy_reg_DW.TargetPoint[2] = Easy_reg_DW.RUPoint[2];
          }
          break;

         case IN_RU:
          /* During 'RU': '<S10>:249' */
          /* '<S10>:252:1' sf_internal_predicateOutput = ... */
          /* '<S10>:252:1' NextStage; */
          if (*sfEvent == event_NextStage) {
            /* Transition: '<S10>:252' */
            Easy_reg_DW.is_Box = IN_LU;
            Easy_reg_DW.was_Box = IN_LU;

            /* Entry 'LU': '<S10>:246' */
            /* '<S10>:246:1' TargetPoint = LUPoint; */
            Easy_reg_DW.TargetPoint[0] = Easy_reg_DW.LUPoint[0];
            Easy_reg_DW.TargetPoint[1] = Easy_reg_DW.LUPoint[1];
            Easy_reg_DW.TargetPoint[2] = Easy_reg_DW.LUPoint[2];
          }
          break;
        }
      }
      break;

     case IN_Final:
      /* During 'Final': '<S10>:94' */
      /*  Состояние последнего манёвра */
      /*  Цель которого выйти на заданную */
      /*  точку приземления */
      break;
    }
    break;

   case IN_Init:
    /* During 'Init': '<S10>:11' */
    /* Transition: '<S10>:25' */
    /* '<S10>:171:1' sf_internal_predicateOutput = ... */
    /* '<S10>:171:1' Mode == 0; */
    if (*Mode == 0) {
      /* Transition: '<S10>:171' */
      Easy_reg_DW.is_Control = IN_TDP_Only;

      /* Entry 'TDP_Only': '<S10>:167' */
      /*  Режим прицеливания на TDP */
      /* entry: */
      /* '<S10>:167:1' TargetPoint = TDPoint; */
      Easy_reg_DW.TargetPoint[0] = TmpSignalConversionAtSFunctionI[0];
      Easy_reg_DW.TargetPoint[1] = TmpSignalConversionAtSFunctionI[1];
      Easy_reg_DW.TargetPoint[2] = TmpSignalConversionAtSFunctionI[2];
    } else {
      /* '<S10>:172:1' sf_internal_predicateOutput = ... */
      /* '<S10>:172:1' (Mode == 1 || Mode == 2); */
      if ((*Mode == 1) || (*Mode == 2)) {
        /* Transition: '<S10>:172' */
        Easy_reg_DW.is_Control = IN_Flight;

        /* Entry 'Flight': '<S10>:23' */
        /*  Режим полета по точкам */
        /* Entry Internal 'Flight': '<S10>:23' */
        switch (Easy_reg_DW.was_Flight) {
         case IN_Angle:
          if (Easy_reg_DW.is_Flight != IN_Angle) {
            Easy_reg_DW.is_Flight = IN_Angle;
            Easy_reg_DW.was_Flight = IN_Angle;
            enter_atomic_Angle(y);
          }
          break;

         case IN_Box:
          if (Easy_reg_DW.is_Flight != IN_Box) {
            Easy_reg_DW.is_Flight = IN_Box;
            Easy_reg_DW.was_Flight = IN_Box;

            /* Entry 'Box': '<S10>:242' */
            /*  LU - Нацеливание на верхний левый угол "коробки" */
            /*  LD - Нацеливание на нижний левый угол "коробки" */
            /*  RD - Нацеливание на нижний правый угол "коробки" */
            /*  RU - Нацеливание на верхний правый угол "коробки" */
          }

          enter_internal_Box();
          break;

         case IN_Final:
          if (Easy_reg_DW.is_Flight != IN_Final) {
            Easy_reg_DW.is_Flight = IN_Final;
            Easy_reg_DW.was_Flight = IN_Final;

            /* Entry 'Final': '<S10>:94' */
            /*  Состояние последнего манёвра */
            /*  Цель которого выйти на заданную */
            /*  точку приземления */
            /* '<S10>:94:3' TargetPoint = TDPoint; */
            Easy_reg_DW.TargetPoint[0] = TmpSignalConversionAtSFunctionI[0];
            Easy_reg_DW.TargetPoint[1] = TmpSignalConversionAtSFunctionI[1];
            Easy_reg_DW.TargetPoint[2] = TmpSignalConversionAtSFunctionI[2];
          }
          break;

         default:
          /* Transition: '<S10>:268' */
          /* '<S10>:267:1' sf_internal_predicateOutput = ... */
          /* '<S10>:267:1' Mode == 1; */
          if (*Mode == 1) {
            /* Transition: '<S10>:267' */
            if (Easy_reg_DW.is_Flight != IN_Box) {
              Easy_reg_DW.is_Flight = IN_Box;
              Easy_reg_DW.was_Flight = IN_Box;

              /* Entry 'Box': '<S10>:242' */
              /*  LU - Нацеливание на верхний левый угол "коробки" */
              /*  LD - Нацеливание на нижний левый угол "коробки" */
              /*  RD - Нацеливание на нижний правый угол "коробки" */
              /*  RU - Нацеливание на верхний правый угол "коробки" */
            }

            enter_internal_Box();
          } else {
            /* '<S10>:83:1' sf_internal_predicateOutput = ... */
            /* '<S10>:83:1' Mode == 2; */
            if ((*Mode == 2) && (!(Easy_reg_DW.is_Flight == IN_Angle))) {
              /* Transition: '<S10>:83' */
              Easy_reg_DW.is_Flight = IN_Angle;
              Easy_reg_DW.was_Flight = IN_Angle;
              enter_atomic_Angle(y);
            }
          }
          break;
        }
      }
    }
    break;

   case IN_TDP_Only:
    /* During 'TDP_Only': '<S10>:167' */
    /* entry: */
    /* '<S10>:167:1' TargetPoint = TDPoint; */
    Easy_reg_DW.TargetPoint[0] = TmpSignalConversionAtSFunctionI[0];
    Easy_reg_DW.TargetPoint[1] = TmpSignalConversionAtSFunctionI[1];
    Easy_reg_DW.TargetPoint[2] = TmpSignalConversionAtSFunctionI[2];
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

/*
 * Function for Chart: '<S1>/TaskController'
 * function status = CheckFinalMane
 *     %% Вычислим расстояние с помощью функции DistanceCalculator()
 */
static real_T CheckFinalMane(const real_T *TurnSpeed, const real_T *y, const
  real_T TmpSignalConversionAtSFunctionI[3], real_T *y_p)
{
  real_T status;
  real_T fi1;
  real_T fi2;

  /* MATLAB Function 'CheckFinalMane': '<S10>:141' */
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
  /* '<S10>:141:16' Distance = DistanceCalculator(ActualPoint, TDPoint); */
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
  fi1 = Easy_reg_DW.TmpSignalConversionAtSFunctio_g[0] * 3.1415926535897931 /
    180.0;

  /* 'DistanceCalculator:15' la2 = Target(2)*pi/180; */
  /* 'DistanceCalculator:16' fi2 = Target(1)*pi/180; */
  fi2 = TmpSignalConversionAtSFunctionI[0] * 3.1415926535897931 / 180.0;

  /* Outport: '<Root>/DistanceToTDP' */
  /* 'DistanceCalculator:17' d = sin(fi1)*sin(fi2) + cos(fi1)*cos(-fi2)*cos(la2-la1); */
  /*  6371 км средний радиус земли */
  /* 'DistanceCalculator:19' y = acos(d)*6378245; */
  Easy_reg_Y.DistanceToTDP = acos(cos(TmpSignalConversionAtSFunctionI[1] *
    3.1415926535897931 / 180.0 - Easy_reg_DW.TmpSignalConversionAtSFunctio_g[1] *
    3.1415926535897931 / 180.0) * (cos(fi1) * cos(-fi2)) + sin(fi1) * sin(fi2)) *
    6.378245E+6;

  /* Outport: '<Root>/TimeToHorTarget' incorporates:
   *  Constant: '<S4>/Constant1'
   *  Outport: '<Root>/DistanceToTDP'
   */
  /*     %% Вычисляем время необходимое для преодоления расстояния до TDP и до посадки */
  /* '<S10>:141:18' T_hor = Distance/V_hor; */
  Easy_reg_Y.TimeToHorTarget = Easy_reg_Y.DistanceToTDP / 20.0;

  /* Outport: '<Root>/TimeToTD' incorporates:
   *  Constant: '<S4>/Constant2'
   *  Inport: '<Root>/ReliefOnTDP'
   */
  /* '<S10>:141:19' T_ver = (ActualPoint(3) - (TDPoint(3) + double(ReliefOnTDP)))/V_ver; */
  Easy_reg_Y.TimeToTD = (Easy_reg_DW.TmpSignalConversionAtSFunctio_g[2] -
    (TmpSignalConversionAtSFunctionI[2] + (real_T)Easy_reg_U.ReliefOnTDP)) / 8.0;

  /*     %% Определяем поправку по времени необходимому на разворот */
  /*  Угол разворота */
  /* '<S10>:141:22' angle = abs(Course2TDP-Course) * 180 / pi; */
  fi1 = fabs(*y_p - *y) * 180.0 / 3.1415926535897931;

  /* '<S10>:141:23' if angle > 180 */
  if (fi1 > 180.0) {
    /* '<S10>:141:24' angle = angle - 180; */
    fi1 -= 180.0;
  }

  /* Outport: '<Root>/TimeToHorTarget' incorporates:
   *  Outport: '<Root>/TimeToTD'
   */
  /*  Вычисляем поправку как линейную функцию:  */
  /* '<S10>:141:28' T_hor_popr = angle / TurnSpeed; */
  /*  Вычисляем поправку по параболе */
  /* T_hor_popr = 0.003*angle^2 + 0.008*angle; */
  /* '<S10>:141:33' T_hor = T_hor + T_hor_popr; */
  Easy_reg_Y.TimeToHorTarget += fi1 / *TurnSpeed;

  /*     %% Возвращаем сигнал о необходимости перейти на финальный маневр */
  /* '<S10>:141:35' if T_hor > T_ver */
  if (Easy_reg_Y.TimeToHorTarget > Easy_reg_Y.TimeToTD) {
    /* '<S10>:141:36' status = 1; */
    status = 1.0;
  } else {
    /* '<S10>:141:37' else */
    /* '<S10>:141:38' status = 0; */
    status = 0.0;
  }

  return status;
}

/* Function for Chart: '<S1>/TaskController' */
static void BimSupply(const boolean_T *Delay2, int32_T *sfEvent)
{
  /* During 'BimSupply': '<S10>:452' */
  switch (Easy_reg_DW.is_BimSupply) {
   case IN_Disable:
    /* During 'Disable': '<S10>:458' */
    /* '<S10>:464:1' sf_internal_predicateOutput = ... */
    /* '<S10>:464:1' NonBlockingMode; */
    if (*sfEvent == event_NonBlockingMode) {
      /* Transition: '<S10>:464' */
      Easy_reg_DW.is_BimSupply = IN_NonBlockingControl;

      /* Entry 'NonBlockingControl': '<S10>:484' */
      /* '<S10>:484:1' TargetEnable = 1; */
      Easy_reg_DW.TargetEnable = 1U;
    } else {
      /* '<S10>:460:1' sf_internal_predicateOutput = ... */
      /* '<S10>:460:1' timeout == 0; */
      if (Easy_reg_DW.timeout == 0U) {
        /* Transition: '<S10>:460' */
        Easy_reg_DW.is_BimSupply = IN_Enable;

        /* Entry 'Enable': '<S10>:456' */
        /* '<S10>:456:1' TargetEnable = 1; */
        Easy_reg_DW.TargetEnable = 1U;
      }
    }
    break;

   case IN_Enable:
    /* During 'Enable': '<S10>:456' */
    /* '<S10>:465:1' sf_internal_predicateOutput = ... */
    /* '<S10>:465:1' NonBlockingMode; */
    if (*sfEvent == event_NonBlockingMode) {
      /* Transition: '<S10>:465' */
      Easy_reg_DW.is_BimSupply = IN_NonBlockingControl;

      /* Entry 'NonBlockingControl': '<S10>:484' */
      /* '<S10>:484:1' TargetEnable = 1; */
      Easy_reg_DW.TargetEnable = 1U;
    } else {
      /* '<S10>:459:1' sf_internal_predicateOutput = ... */
      /* '<S10>:459:1' CompletedCorrection  == 1; */
      if (*Delay2) {
        /* Transition: '<S10>:459' */
        Easy_reg_DW.is_BimSupply = IN_Disable;

        /* Entry 'Disable': '<S10>:458' */
        /* '<S10>:458:1' TargetEnable = 0; */
        Easy_reg_DW.TargetEnable = 0U;

        /* '<S10>:458:1' timeout = 30; */
        Easy_reg_DW.timeout = 30U;
      }
    }
    break;

   case IN_NonBlockingControl:
    /* During 'NonBlockingControl': '<S10>:484' */
    /* '<S10>:485:1' sf_internal_predicateOutput = ... */
    /* '<S10>:485:1' Parking; */
    if (*sfEvent == event_Parking) {
      /* Transition: '<S10>:485' */
      Easy_reg_DW.is_BimSupply = IN_Shutdown;

      /* Entry 'Shutdown': '<S10>:461' */
      /* '<S10>:461:1' TargetEnable = 0; */
      Easy_reg_DW.TargetEnable = 0U;
    } else {
      /* '<S10>:484:1' TargetEnable = 1; */
      Easy_reg_DW.TargetEnable = 1U;
    }
    break;

   case IN_Shutdown:
    /* During 'Shutdown': '<S10>:461' */
    /* '<S10>:461:1' TargetEnable = 0; */
    Easy_reg_DW.TargetEnable = 0U;
    break;
  }
}

/* Model step function */
void Easy_reg_step(void)
{
  int32_T b_previousEvent;
  uint8_T Mode;
  real_T TurnSpeed;
  real_T y;
  boolean_T Delay2;
  real_T TmpSignalConversionAtSFunctionI[3];
  int32_T sfEvent;
  real_T y_p;
  real_T tmp;
  uint32_T qY;
  int16_T tmp_0;
  boolean_T guard1 = false;

  /* Outputs for Atomic SubSystem: '<Root>/Easy_reg' */
  /* DataStoreWrite: '<S1>/Data Store Write3' incorporates:
   *  Constant: '<S1>/Mode'
   */
  Mode = 0U;

  /* DataStoreWrite: '<S1>/Data Store Write4' incorporates:
   *  Constant: '<S1>/TurnSpeed'
   */
  TurnSpeed = 7.0;

  /* Outputs for Atomic SubSystem: '<S14>/Bearing' */

  /* Update for Inport: '<Root>/Pos_lat' incorporates:
   *  Delay: '<S14>/LastPos'
   *  Inport: '<Root>/Pos_alt'
   *  Inport: '<Root>/Pos_lon'
   */
  Bearing(Easy_reg_U.Pos_lat, Easy_reg_U.Pos_lon, Easy_reg_DW.LastPos_1_DSTATE,
          Easy_reg_DW.LastPos_2_DSTATE, &y, &Easy_reg_DW.Bearing_e);

  /* End of Outputs for SubSystem: '<S14>/Bearing' */

  /* Outputs for Atomic SubSystem: '<S13>/Bearing' */

  /* Inport: '<Root>/TDP_lat' incorporates:
   *  Inport: '<Root>/Pos_alt'
   *  Inport: '<Root>/Pos_lat'
   *  Inport: '<Root>/Pos_lon'
   *  Inport: '<Root>/TDP_alt'
   *  Inport: '<Root>/TDP_lon'
   */
  Bearing(Easy_reg_U.TDP_lat, Easy_reg_U.TDP_lon, Easy_reg_U.Pos_lat,
          Easy_reg_U.Pos_lon, &y_p, &Easy_reg_DW.Bearing_o);

  /* End of Outputs for SubSystem: '<S13>/Bearing' */

  /* Delay: '<S1>/Delay2' */
  Delay2 = Easy_reg_DW.Delay2_DSTATE;

  /* SignalConversion: '<S10>/TmpSignal ConversionAt SFunction Inport1' incorporates:
   *  Chart: '<S1>/TaskController'
   *  Inport: '<Root>/TDP_alt'
   *  Inport: '<Root>/TDP_lat'
   *  Inport: '<Root>/TDP_lon'
   */
  TmpSignalConversionAtSFunctionI[0] = Easy_reg_U.TDP_lat;
  TmpSignalConversionAtSFunctionI[1] = Easy_reg_U.TDP_lon;
  TmpSignalConversionAtSFunctionI[2] = Easy_reg_U.TDP_alt;

  /* SignalConversion: '<S10>/TmpSignal ConversionAt SFunction Inport2' incorporates:
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
  if (Easy_reg_DW.is_active_c1_Easy_reg == 0U) {
    /* Entry: Easy_reg/TaskController */
    Easy_reg_DW.is_active_c1_Easy_reg = 1U;

    /* Entry Internal: Easy_reg/TaskController */
    if (Easy_reg_DW.is_active_Timer != 1U) {
      Easy_reg_DW.is_active_Timer = 1U;

      /* Entry 'Timer': '<S10>:207' */
      /* '<S10>:207:1' time = 0; */
      Easy_reg_DW.time = 0U;
    }

    if (Easy_reg_DW.is_active_Control != 1U) {
      Easy_reg_DW.is_active_Control = 1U;

      /* Entry 'Control': '<S10>:9' */
      /*  Основной цикл задач */
    }

    /* Entry Internal 'Control': '<S10>:9' */
    /* Transition: '<S10>:12' */
    if (Easy_reg_DW.is_Control != IN_Init) {
      Easy_reg_DW.is_Control = IN_Init;

      /* Entry 'Init': '<S10>:11' */
      /*  Производим планирование путевых точек для сброса высоты */
      /* '<S10>:11:4' TargetPoint = TDPoint; */
      Easy_reg_DW.TargetPoint[0] = Easy_reg_U.TDP_lat;
      Easy_reg_DW.TargetPoint[1] = Easy_reg_U.TDP_lon;
      Easy_reg_DW.TargetPoint[2] = Easy_reg_U.TDP_alt;

      /* '<S10>:11:4' [Flight.Angle.Point1, Flight.Angle.Point2] = AngleManCreator(ActualPoint, TDPoint, AngleManevr); */
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
      TurnSpeed = (Easy_reg_U.TDP_lon -
                   Easy_reg_DW.TmpSignalConversionAtSFunctio_g[1]) / 2.0;

      /* 'AngleManCreator:14' dy = (y2-y1)/2; */
      y_p = (Easy_reg_U.TDP_lat - Easy_reg_DW.TmpSignalConversionAtSFunctio_g[0])
        / 2.0;

      /*  Определяем первую точку */
      /* 'AngleManCreator:16' x3 = dx*cos(Angle*pi/180) - dy*sin(Angle*pi/180)+x1; */
      /* 'AngleManCreator:17' y3 = dx*sin(Angle*pi/180) + dy*cos(Angle*pi/180)+y1; */
      /* 'AngleManCreator:18' AMP1 = [y3;x3;0]; */
      Easy_reg_DW.Point1[0] = (TurnSpeed * 0.17364817766693033 + y_p *
        0.984807753012208) + Easy_reg_DW.TmpSignalConversionAtSFunctio_g[0];
      Easy_reg_DW.Point1[1] = (TurnSpeed * 0.984807753012208 - y_p *
        0.17364817766693033) + Easy_reg_DW.TmpSignalConversionAtSFunctio_g[1];
      Easy_reg_DW.Point1[2] = 0.0;

      /*  Определяем вторую точку */
      /* 'AngleManCreator:20' x4 = dx*cos(-Angle*pi/180) - dy*sin(-Angle*pi/180)+x1; */
      /* 'AngleManCreator:21' y4 = dx*sin(-Angle*pi/180) + dy*cos(-Angle*pi/180)+y1; */
      /* 'AngleManCreator:22' AMP2 = [y4;x4;0]; */
      Easy_reg_DW.Point2[0] = (TurnSpeed * -0.17364817766693033 + y_p *
        0.984807753012208) + Easy_reg_DW.TmpSignalConversionAtSFunctio_g[0];
      Easy_reg_DW.Point2[1] = (TurnSpeed * 0.984807753012208 - y_p *
        -0.17364817766693033) + Easy_reg_DW.TmpSignalConversionAtSFunctio_g[1];
      Easy_reg_DW.Point2[2] = 0.0;

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
      if (Easy_reg_DW.Point1[1] > 180.0) {
        /* 'PositionRangeConverting:12' OutputPoint(2) = OutputPoint(2) - 360; */
        Easy_reg_DW.Point1[1] -= 360.0;
      } else {
        if (Easy_reg_DW.Point1[1] < -180.0) {
          /* 'PositionRangeConverting:13' elseif OutputPoint(2) < (-180) */
          /* 'PositionRangeConverting:14' OutputPoint(2) = OutputPoint(2) + 360; */
          Easy_reg_DW.Point1[1] += 360.0;
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
      if (Easy_reg_DW.Point2[1] > 180.0) {
        /* 'PositionRangeConverting:12' OutputPoint(2) = OutputPoint(2) - 360; */
        Easy_reg_DW.Point2[1] -= 360.0;
      } else {
        if (Easy_reg_DW.Point2[1] < -180.0) {
          /* 'PositionRangeConverting:13' elseif OutputPoint(2) < (-180) */
          /* 'PositionRangeConverting:14' OutputPoint(2) = OutputPoint(2) + 360; */
          Easy_reg_DW.Point2[1] += 360.0;
        }
      }

      /* '<S10>:11:5' [Flight.Box.LUPoint, Flight.Box.LDPoint, Flight.Box.RUPoint, Flight.Box.RDPoint] = BOXCreator(TDPoint, BoxSize); */
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
      Easy_reg_DW.LUPoint[0] = 0.0;
      Easy_reg_DW.LDPoint[0] = 0.0;
      Easy_reg_DW.RUPoint[0] = 0.0;
      Easy_reg_DW.RDPoint[0] = 0.0;
      Easy_reg_DW.LUPoint[1] = 0.0;
      Easy_reg_DW.LDPoint[1] = 0.0;
      Easy_reg_DW.RUPoint[1] = 0.0;
      Easy_reg_DW.RDPoint[1] = 0.0;
      Easy_reg_DW.LUPoint[2] = 0.0;
      Easy_reg_DW.LDPoint[2] = 0.0;
      Easy_reg_DW.RUPoint[2] = 0.0;
      Easy_reg_DW.RDPoint[2] = 0.0;

      /*  Вычисляем шаг в градусах по Широте (однозначно сопоставляются) */
      /* 'BOXCreator:13' LatStep = BoxStep/1000*360/40000; */
      /*  Координаты по Широте */
      /* 'BOXCreator:15' BLU(1) = TDP(1) + LatStep; */
      Easy_reg_DW.LUPoint[0] = Easy_reg_U.TDP_lat + 0.0135;

      /* 'BOXCreator:16' BLD(1) = TDP(1) - LatStep; */
      Easy_reg_DW.LDPoint[0] = Easy_reg_U.TDP_lat - 0.0135;

      /* 'BOXCreator:17' BRU(1) = TDP(1) + LatStep; */
      Easy_reg_DW.RUPoint[0] = Easy_reg_U.TDP_lat + 0.0135;

      /* 'BOXCreator:18' BRD(1) = TDP(1) - LatStep; */
      Easy_reg_DW.RDPoint[0] = Easy_reg_U.TDP_lat - 0.0135;

      /*  Вычисляем шаг по Долготе без учета поправки на Широту (долгота зависит от */
      /*  широты), а поправку на широту будем делать прямо в выражении */
      /* 'BOXCreator:21' LonStep = BoxStep/1000*360/40000; */
      /*  Координаты по Долготе */
      /* 'BOXCreator:23' BLU(2) = TDP(2) - LonStep/cos(pi*BLU(1)/180); */
      Easy_reg_DW.LUPoint[1] = Easy_reg_U.TDP_lon - 0.0135 / cos
        (3.1415926535897931 * Easy_reg_DW.LUPoint[0] / 180.0);

      /* 'BOXCreator:24' BLD(2) = TDP(2) - LonStep/cos(pi*BLD(1)/180); */
      Easy_reg_DW.LDPoint[1] = Easy_reg_U.TDP_lon - 0.0135 / cos
        (3.1415926535897931 * Easy_reg_DW.LDPoint[0] / 180.0);

      /* 'BOXCreator:25' BRU(2) = TDP(2) + LonStep/cos(pi*BRU(1)/180); */
      Easy_reg_DW.RUPoint[1] = 0.0135 / cos(3.1415926535897931 *
        Easy_reg_DW.RUPoint[0] / 180.0) + Easy_reg_U.TDP_lon;

      /* 'BOXCreator:26' BRD(2) = TDP(2) + LonStep/cos(pi*BRD(1)/180); */
      Easy_reg_DW.RDPoint[1] = 0.0135 / cos(3.1415926535897931 *
        Easy_reg_DW.RDPoint[0] / 180.0) + Easy_reg_U.TDP_lon;

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
      if (Easy_reg_DW.LUPoint[1] > 180.0) {
        /* 'PositionRangeConverting:12' OutputPoint(2) = OutputPoint(2) - 360; */
        Easy_reg_DW.LUPoint[1] -= 360.0;
      } else {
        if (Easy_reg_DW.LUPoint[1] < -180.0) {
          /* 'PositionRangeConverting:13' elseif OutputPoint(2) < (-180) */
          /* 'PositionRangeConverting:14' OutputPoint(2) = OutputPoint(2) + 360; */
          Easy_reg_DW.LUPoint[1] += 360.0;
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
      if (Easy_reg_DW.LDPoint[1] > 180.0) {
        /* 'PositionRangeConverting:12' OutputPoint(2) = OutputPoint(2) - 360; */
        Easy_reg_DW.LDPoint[1] -= 360.0;
      } else {
        if (Easy_reg_DW.LDPoint[1] < -180.0) {
          /* 'PositionRangeConverting:13' elseif OutputPoint(2) < (-180) */
          /* 'PositionRangeConverting:14' OutputPoint(2) = OutputPoint(2) + 360; */
          Easy_reg_DW.LDPoint[1] += 360.0;
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
      if (Easy_reg_DW.RUPoint[1] > 180.0) {
        /* 'PositionRangeConverting:12' OutputPoint(2) = OutputPoint(2) - 360; */
        Easy_reg_DW.RUPoint[1] -= 360.0;
      } else {
        if (Easy_reg_DW.RUPoint[1] < -180.0) {
          /* 'PositionRangeConverting:13' elseif OutputPoint(2) < (-180) */
          /* 'PositionRangeConverting:14' OutputPoint(2) = OutputPoint(2) + 360; */
          Easy_reg_DW.RUPoint[1] += 360.0;
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
      if (Easy_reg_DW.RDPoint[1] > 180.0) {
        /* 'PositionRangeConverting:12' OutputPoint(2) = OutputPoint(2) - 360; */
        Easy_reg_DW.RDPoint[1] -= 360.0;
      } else {
        if (Easy_reg_DW.RDPoint[1] < -180.0) {
          /* 'PositionRangeConverting:13' elseif OutputPoint(2) < (-180) */
          /* 'PositionRangeConverting:14' OutputPoint(2) = OutputPoint(2) + 360; */
          Easy_reg_DW.RDPoint[1] += 360.0;
        }
      }
    }

    if (Easy_reg_DW.is_active_TargetCourse != 1U) {
      Easy_reg_DW.is_active_TargetCourse = 1U;

      /* Entry 'TargetCourse': '<S10>:468' */
      /* Course2Target = Azimut(TargetPoint, ActualPoint); */
      /* '<S10>:468:2' Course2Target = Heading(TargetPoint, ActualPoint, 0); */
      Easy_reg_DW.Course2Target = Heading(Easy_reg_DW.TargetPoint,
        Easy_reg_DW.TmpSignalConversionAtSFunctio_g, 0.0);
    }

    if (Easy_reg_DW.is_active_Touchdown != 1U) {
      Easy_reg_DW.is_active_Touchdown = 1U;

      /* Entry 'Touchdown': '<S10>:34' */
      /* Здесь мы отдельно будем контролировать высоту, */
      /*  для своевременного открытия посадотчных парашютов */
    }

    /* Entry Internal 'Touchdown': '<S10>:34' */
    /* Transition: '<S10>:37' */
    if (Easy_reg_DW.is_Touchdown != IN_OFF) {
      Easy_reg_DW.is_Touchdown = IN_OFF;

      /* Outport: '<Root>/TD_CMD' */
      /* Entry 'OFF': '<S10>:38' */
      /* '<S10>:38:1' TD_CMD = 0; */
      Easy_reg_Y.TD_CMD = 0U;
    }

    if (Easy_reg_DW.is_active_EventGenerator != 1U) {
      Easy_reg_DW.is_active_EventGenerator = 1U;

      /* Entry 'EventGenerator': '<S10>:260' */
    }

    if (Easy_reg_DW.is_active_BimSupply != 1U) {
      Easy_reg_DW.is_active_BimSupply = 1U;

      /* Entry 'BimSupply': '<S10>:452' */
      /* '<S10>:452:1' timeout = 0; */
      Easy_reg_DW.timeout = 0U;

      /* '<S10>:452:1' TargetEnable = 1; */
      Easy_reg_DW.TargetEnable = 1U;
    }

    /* Entry Internal 'BimSupply': '<S10>:452' */
    /* Transition: '<S10>:457' */
    if (Easy_reg_DW.is_BimSupply != IN_Enable) {
      Easy_reg_DW.is_BimSupply = IN_Enable;

      /* Entry 'Enable': '<S10>:456' */
      /* '<S10>:456:1' TargetEnable = 1; */
      Easy_reg_DW.TargetEnable = 1U;
    }
  } else {
    if (Easy_reg_DW.is_active_Timer != 0U) {
      /* During 'Timer': '<S10>:207' */
      /* '<S10>:207:1' time = time + 1; */
      qY = Easy_reg_DW.time + /*MW:OvSatOk*/ 1U;
      if (qY < Easy_reg_DW.time) {
        qY = MAX_uint32_T;
      }

      Easy_reg_DW.time = qY;

      /* '<S10>:207:3' if BimSupply.timeout > 0 */
      if (Easy_reg_DW.timeout > 0U) {
        /* '<S10>:207:4' BimSupply.timeout = BimSupply.timeout - 1; */
        qY = Easy_reg_DW.timeout - /*MW:OvSatOk*/ 1U;
        if (qY > Easy_reg_DW.timeout) {
          qY = 0U;
        }

        Easy_reg_DW.timeout = qY;
      }
    }

    if (Easy_reg_DW.is_active_Control != 0U) {
      Control(&Mode, &y, TmpSignalConversionAtSFunctionI, &sfEvent);
    }

    if (Easy_reg_DW.is_active_TargetCourse != 0U) {
      /* During 'TargetCourse': '<S10>:468' */
      /* Course2Target = Azimut(TargetPoint, ActualPoint); */
      /* '<S10>:468:2' Course2Target = Heading(TargetPoint, ActualPoint, 0); */
      Easy_reg_DW.Course2Target = Heading(Easy_reg_DW.TargetPoint,
        Easy_reg_DW.TmpSignalConversionAtSFunctio_g, 0.0);
    }

    if (Easy_reg_DW.is_active_Touchdown != 0U) {
      /* During 'Touchdown': '<S10>:34' */
      /* Здесь мы отдельно будем контролировать высоту, */
      /*  для своевременного открытия посадотчных парашютов */
      guard1 = false;
      switch (Easy_reg_DW.is_Touchdown) {
       case IN_OFF:
        /* During 'OFF': '<S10>:38' */
        /* Transition: '<S10>:119' */
        /* '<S10>:43:1' sf_internal_predicateOutput = ... */
        /* '<S10>:43:1' ReliefAvailable == 0; */
        if ((Easy_reg_U.ReliefAvailable == 0) &&
            (Easy_reg_DW.TmpSignalConversionAtSFunctio_g[2] <=
             Easy_reg_U.TDP_alt)) {
          /* Transition: '<S10>:43' */
          /* '<S10>:128:1' sf_internal_predicateOutput = ... */
          /* '<S10>:128:1' ActualPoint(3) <= TDPoint(3); */
          /* Transition: '<S10>:128' */
          guard1 = true;
        } else {
          /* '<S10>:125:1' sf_internal_predicateOutput = ... */
          /* '<S10>:125:1' ReliefAvailable ==1; */
          if (Easy_reg_U.ReliefAvailable == 1) {
            /* Transition: '<S10>:125' */
            /* '<S10>:129:1' sf_internal_predicateOutput = ... */
            /* '<S10>:129:1' (ActualPoint(3)-Relief) <= TDPoint(3); */
            tmp = rt_roundd_snf(Easy_reg_DW.TmpSignalConversionAtSFunctio_g[2] -
                                (real_T)Easy_reg_U.Relief);
            if (tmp < 32768.0) {
              if (tmp >= -32768.0) {
                tmp_0 = (int16_T)tmp;
              } else {
                tmp_0 = MIN_int16_T;
              }
            } else {
              tmp_0 = MAX_int16_T;
            }

            if (tmp_0 <= Easy_reg_U.TDP_alt) {
              /* Transition: '<S10>:129' */
              guard1 = true;
            } else {
              /* Outport: '<Root>/TD_CMD' */
              /* '<S10>:38:1' TD_CMD = 0; */
              Easy_reg_Y.TD_CMD = 0U;
            }
          } else {
            /* Outport: '<Root>/TD_CMD' */
            /* '<S10>:38:1' TD_CMD = 0; */
            Easy_reg_Y.TD_CMD = 0U;
          }
        }
        break;

       case IN_ON:
        /* Outport: '<Root>/TD_CMD' */
        /* During 'ON': '<S10>:36' */
        /* '<S10>:36:1' TD_CMD = 1; */
        Easy_reg_Y.TD_CMD = 1U;
        break;
      }

      if (guard1) {
        Easy_reg_DW.is_Touchdown = IN_ON;

        /* Outport: '<Root>/TD_CMD' */
        /* Entry 'ON': '<S10>:36' */
        /* '<S10>:36:1' TD_CMD = 1; */
        Easy_reg_Y.TD_CMD = 1U;
      }
    }

    if (Easy_reg_DW.is_active_EventGenerator != 0U) {
      /* During 'EventGenerator': '<S10>:260' */
      /*  Контроль выхода на точку */
      /* '<S10>:260:1' if func.CheckFinalMane  == 1 */
      if (CheckFinalMane(&TurnSpeed, &y, TmpSignalConversionAtSFunctionI, &y_p) ==
          1.0) {
        /* '<S10>:260:3' Control.Final; */
        /* Event: '<S10>:361' */
        b_previousEvent = sfEvent;
        sfEvent = event_Final;
        if (Easy_reg_DW.is_active_Control != 0U) {
          Control(&Mode, &y, TmpSignalConversionAtSFunctionI, &sfEvent);
        }

        sfEvent = b_previousEvent;
      }

      /*  Контроль попадания в зону текущей цели */
      /* '<S10>:260:7' if CheckArrival(ActualPoint, TargetPoint, ArrivalRadius) == 1 */
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
      TurnSpeed = Easy_reg_DW.TmpSignalConversionAtSFunctio_g[0] *
        3.1415926535897931 / 180.0;

      /* 'DistanceCalculator:15' la2 = Target(2)*pi/180; */
      /* 'DistanceCalculator:16' fi2 = Target(1)*pi/180; */
      y_p = Easy_reg_DW.TargetPoint[0] * 3.1415926535897931 / 180.0;

      /* 'DistanceCalculator:17' d = sin(fi1)*sin(fi2) + cos(fi1)*cos(-fi2)*cos(la2-la1); */
      /*  6371 км средний радиус земли */
      /* 'DistanceCalculator:19' y = acos(d)*6378245; */
      /*     %% Попаданием будем считать радиус  */
      /* 'CheckArrival:14' if distance < Radius */
      tmp = Easy_reg_DW.TmpSignalConversionAtSFunctio_g[1] * 3.1415926535897931 /
        180.0;
      if (acos(cos(Easy_reg_DW.TargetPoint[1] * 3.1415926535897931 / 180.0 - tmp)
               * (cos(TurnSpeed) * cos(-y_p)) + sin(TurnSpeed) * sin(y_p)) *
          6.378245E+6 < 300.0) {
        /* 'CheckArrival:15' status = 1; */
        /* '<S10>:260:8' Control.NextStage; */
        /* Event: '<S10>:362' */
        b_previousEvent = sfEvent;
        sfEvent = event_NextStage;
        if (Easy_reg_DW.is_active_Control != 0U) {
          Control(&Mode, &y, TmpSignalConversionAtSFunctionI, &sfEvent);
        }

        sfEvent = b_previousEvent;
      } else {
        /* 'CheckArrival:16' else */
        /* 'CheckArrival:17' status = 0; */
        /* '<S10>:260:8' else */
        /* '<S10>:260:8' Control.CancelFinal; */
        /* Event: '<S10>:391' */
        b_previousEvent = sfEvent;
        sfEvent = event_CancelFinal;
        if (Easy_reg_DW.is_active_Control != 0U) {
          Control(&Mode, &y, TmpSignalConversionAtSFunctionI, &sfEvent);
        }

        sfEvent = b_previousEvent;
      }

      /*  Контроль попадания в зону точки приземления */
      /*  где на работу БИМов нет ограничений */
      /* '<S10>:260:14' if CheckArrival(ActualPoint, TDPoint, 1000) == 1 */
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
      y_p = Easy_reg_U.TDP_lat * 3.1415926535897931 / 180.0;

      /* 'DistanceCalculator:17' d = sin(fi1)*sin(fi2) + cos(fi1)*cos(-fi2)*cos(la2-la1); */
      /*  6371 км средний радиус земли */
      /* 'DistanceCalculator:19' y = acos(d)*6378245; */
      /*     %% Попаданием будем считать радиус  */
      /* 'CheckArrival:14' if distance < Radius */
      if (acos(cos(Easy_reg_U.TDP_lon * 3.1415926535897931 / 180.0 - tmp) * (cos
            (TurnSpeed) * cos(-y_p)) + sin(TurnSpeed) * sin(y_p)) * 6.378245E+6 <
          1000.0) {
        /* 'CheckArrival:15' status = 1; */
        /* '<S10>:260:15' BimSupply.NonBlockingMode; */
        /* Event: '<S10>:486' */
        sfEvent = event_NonBlockingMode;
        if (Easy_reg_DW.is_active_BimSupply != 0U) {
          BimSupply(&Delay2, &sfEvent);
        }

        sfEvent = b_previousEvent;
      } else {
        /* 'CheckArrival:16' else */
        /* 'CheckArrival:17' status = 0; */
      }

      /*  Контроль попадания в зону точки приземления */
      /*  Где управление уже запрещено */
      /* '<S10>:260:18' if CheckArrival(ActualPoint, TDPoint, 300) == 1 */
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
      if (acos(cos(Easy_reg_U.TDP_lon * 3.1415926535897931 / 180.0 -
                   Easy_reg_DW.TmpSignalConversionAtSFunctio_g[1] *
                   3.1415926535897931 / 180.0) * (cos(TurnSpeed) * cos(-y_p)) +
               sin(TurnSpeed) * sin(y_p)) * 6.378245E+6 < 300.0) {
        /* 'CheckArrival:15' status = 1; */
        /* '<S10>:260:20' BimSupply.Parking; */
        /* Event: '<S10>:467' */
        sfEvent = event_Parking;
        if (Easy_reg_DW.is_active_BimSupply != 0U) {
          BimSupply(&Delay2, &sfEvent);
        }

        sfEvent = b_previousEvent;
      } else {
        /* 'CheckArrival:16' else */
        /* 'CheckArrival:17' status = 0; */
      }
    }

    if (Easy_reg_DW.is_active_BimSupply != 0U) {
      BimSupply(&Delay2, &sfEvent);
    }
  }

  /* Sum: '<S9>/Sum1' */
  y = Easy_reg_DW.Course2Target - y;

  /* MATLAB Function: '<S34>/ControlDemode' */
  /*  Здесь переводим угол из диапазона [0 360] */
  /*  в диапазон [-180 180], что позволит нам определять */
  /*  оптимальное направление разворота (лево и право) */
  /* MATLAB Function 'ControlDemode/ControlDemode': '<S40>:1' */
  /* '<S40>:1:5' y = u; */
  /*  Если больше 180 */
  /* '<S40>:1:7' if y > pi */
  if (y > 3.1415926535897931) {
    /*  то это уже отрицательная полуокружность */
    /* '<S40>:1:9' y = y - 2*pi; */
    y -= 6.2831853071795862;

    /*  Если меньше -180 */
  } else {
    if (y < -3.1415926535897931) {
      /* '<S40>:1:11' elseif (y < (-pi)) */
      /*  то это уже положительная полуокружность */
      /* '<S40>:1:13' y = y + 2*pi; */
      y += 6.2831853071795862;
    }
  }

  /* End of MATLAB Function: '<S34>/ControlDemode' */

  /* Gain: '<S9>/Base_Gain' incorporates:
   *  Gain: '<S38>/Gain'
   */
  y = 57.295779513082323 * y * 2.0;

  /* Saturate: '<S9>/Base_Sat' */
  if (y > 50.0) {
    y = 50.0;
  } else {
    if (y < -50.0) {
      y = -50.0;
    }
  }

  /* End of Saturate: '<S9>/Base_Sat' */

  /* Product: '<S9>/Product' incorporates:
   *  Gain: '<S39>/Gain'
   *  Gain: '<S39>/Gain1'
   *  Rounding: '<S39>/Rounding Function'
   */
  y = floor(0.1 * y) * 10.0 * (real_T)Easy_reg_DW.TargetEnable;

  /* Chart: '<S2>/BimTrigger' */
  /* Gateway: BimTriggers/BimTrigger */
  /* During: BimTriggers/BimTrigger */
  if (Easy_reg_DW.is_active_c9_BupSimulinkLibrari == 0U) {
    /* Entry: BimTriggers/BimTrigger */
    Easy_reg_DW.is_active_c9_BupSimulinkLibrari = 1U;

    /* Entry Internal: BimTriggers/BimTrigger */
    /* Entry 'Triggers': '<S11>:18' */
    /* Entry Internal 'Triggers': '<S11>:18' */
    /* Entry 'DisabledTrigger': '<S11>:19' */
    /* '<S11>:19:1' if LastLineState > 0 && Line <=0 */
    if ((Easy_reg_DW.LastLineState > 0.0) && (y <= 0.0)) {
      /* Update for Delay: '<S1>/Delay2' */
      /* '<S11>:19:1' Disabled = true; */
      Easy_reg_DW.Delay2_DSTATE = true;
    } else if ((Easy_reg_DW.LastLineState < 0.0) && (y >= 0.0)) {
      /* Update for Delay: '<S1>/Delay2' */
      /* '<S11>:19:2' elseif LastLineState < 0 && Line >=0 */
      /* '<S11>:19:4' Disabled = true; */
      Easy_reg_DW.Delay2_DSTATE = true;
    } else {
      /* Update for Delay: '<S1>/Delay2' */
      /* '<S11>:19:4' else */
      /* '<S11>:19:4' Disabled = false; */
      Easy_reg_DW.Delay2_DSTATE = false;
    }

    /* Entry 'EnabledTrigger1': '<S11>:21' */
    /* '<S11>:21:1' if LastLineState == 0 && Line > 0 */
    /* Entry 'RemeberLastState': '<S11>:29' */
    /* '<S11>:29:1' LastLineState = Line; */
    Easy_reg_DW.LastLineState = y;
  } else {
    /* During 'Triggers': '<S11>:18' */
    /* During 'DisabledTrigger': '<S11>:19' */
    /* '<S11>:19:1' if LastLineState > 0 && Line <=0 */
    if ((Easy_reg_DW.LastLineState > 0.0) && (y <= 0.0)) {
      /* Update for Delay: '<S1>/Delay2' */
      /* '<S11>:19:1' Disabled = true; */
      Easy_reg_DW.Delay2_DSTATE = true;
    } else if ((Easy_reg_DW.LastLineState < 0.0) && (y >= 0.0)) {
      /* Update for Delay: '<S1>/Delay2' */
      /* '<S11>:19:2' elseif LastLineState < 0 && Line >=0 */
      /* '<S11>:19:3' Disabled = true; */
      Easy_reg_DW.Delay2_DSTATE = true;
    } else {
      /* Update for Delay: '<S1>/Delay2' */
      /* '<S11>:19:4' else */
      /* '<S11>:19:4' Disabled = false; */
      Easy_reg_DW.Delay2_DSTATE = false;
    }

    /* During 'EnabledTrigger1': '<S11>:21' */
    /* '<S11>:21:1' if LastLineState == 0 && Line > 0 */
    /* During 'RemeberLastState': '<S11>:29' */
    /* '<S11>:29:1' LastLineState = Line; */
    Easy_reg_DW.LastLineState = y;
  }

  /* End of Chart: '<S2>/BimTrigger' */

  /* Update for Delay: '<S14>/LastPos' incorporates:
   *  Inport: '<Root>/Pos_alt'
   *  Inport: '<Root>/Pos_lat'
   *  Inport: '<Root>/Pos_lon'
   */
  /*  u0 - точка отсчета */
  /*  u1 - конечная точка */
  /*  la - долгота */
  /*  fi - широта */
  /* MATLAB Function 'Azimuth/Azimut': '<S22>:1' */
  /* '<S22>:1:6' la1 = u0(2)*pi/180; */
  /* '<S22>:1:7' fi1 = u0(1)*pi/180; */
  /* '<S22>:1:8' la2 = u1(2)*pi/180; */
  /* '<S22>:1:9' fi2 = u1(1)*pi/180; */
  /* '<S22>:1:10' delta_la = la2-la1; */
  /* '<S22>:1:11' y = atan2(sin(delta_la)*cos(fi2), cos(fi1)*sin(fi2)-sin(fi1)*cos(fi2)*cos(delta_la)); */
  /* '<S22>:1:12' y = rem((y + 2*pi), 2*pi); */
  /* MATLAB Function 'GPSVelocity/Velocity': '<S26>:1' */
  /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
  /*  */
  /*    GPSPos2Speed - Определение скорости по сигналам GPS */
  /*    u1 - Точка относительно, которой считаем перемещение (lat, lon, alt) */
  /*    u0 - Точка до которой отладываем перемещение (lat, lon, alt) */
  /*    time - Время между замерами координат */
  /*  */
  /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
  /* '<S26>:1:10' un = u1-u0; */
  /* '<S26>:1:11' v_lat = un(1)*40000/360*1000/time; */
  /* '<S26>:1:12' v_lon = un(2)*(40000/360*1000/time * cos(pi/180*u0(1))); */
  /* '<S26>:1:13' v_alt = un(3); */
  /* MATLAB Function 'ProjectionSpeed/Speed': '<S27>:1' */
  /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
  /*  */
  /*    SpeedProjections - Пересчет проекций векторов скорости по lat, lon, alt */
  /*    в горизонтальную и вертикальную скорость полёта */
  /*  */
  /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
  /* '<S27>:1:8' V_hor = sqrt(Vel_lat^2 + Vel_lon^2); */
  /* '<S27>:1:9' V_vert = abs(Vel_alt); */
  /* MATLAB Function 'DeadZone/DeadZone': '<S42>:1' */
  /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
  /*  */
  /*    Звено нечувствительности */
  /*  */
  /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
  /* '<S42>:1:7' if abs(u) < 20 */
  /*  if u > 10 */
  /*      y = 50; */
  /*  elseif u < - 10 */
  /*      y = -50; */
  /*  else */
  /*      y = 0; */
  /*  end */
  Easy_reg_DW.LastPos_1_DSTATE = Easy_reg_U.Pos_lat;
  Easy_reg_DW.LastPos_2_DSTATE = Easy_reg_U.Pos_lon;
  Easy_reg_DW.LastPos_3_DSTATE = Easy_reg_U.Pos_alt;

  /* End of Outputs for SubSystem: '<Root>/Easy_reg' */

  /* Outport: '<Root>/BIM_CMD' */
  Easy_reg_Y.BIM_CMD = y;
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
