/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 *
 * File: Easy_reg.c
 *
 * Code generated for Simulink model 'Easy_reg'.
 *
 * Model version                  : 1.116
 * Simulink Coder version         : 8.14 (R2018a) 06-Feb-2018
 * C/C++ source code generated on : Mon Sep 30 15:38:34 2019
 *
 * Target selection: ert.tlc
 * Embedded hardware selection: ARM Compatible->ARM Cortex
 * Code generation objectives: Unspecified
 * Validation result: Not run
 */

#include "Easy_reg.h"

/* Named constants for Chart: '<S1>/Chart' */
#define IN_Angle                       ((uint8_T)1U)
#define IN_LD                          ((uint8_T)2U)
#define IN_LU                          ((uint8_T)3U)
#define IN_NO_ACTIVE_CHILD             ((uint8_T)0U)
#define IN_OFF                         ((uint8_T)1U)
#define IN_ON                          ((uint8_T)2U)
#define IN_RD                          ((uint8_T)4U)
#define IN_RU                          ((uint8_T)5U)
#define IN_Task_FinalReplace           ((uint8_T)1U)
#define IN_Task_Flight                 ((uint8_T)1U)
#define IN_Task_Init                   ((uint8_T)2U)
#define IN_Task_TargetReplace          ((uint8_T)2U)
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
static void Heading_true(const real_T rtu_u1[3], const real_T rtu_u0[3],
  DW_Heading_true *localDW);
static void SimulinkTargeting(const real_T rtu_Position[3], const real_T
  rtu_Target[3], DW_SimulinkTargeting *localDW);

/* Forward declaration for local functions */
static real_T DistanceCalculator(const real_T Position[3], const real_T Target[3]);
static real_T CheckFinalMane(const real_T Position[3], const real_T Target[3],
  real_T HorSpeed, real_T VertSpeed);
static real_T CheckArrival(const real_T Now[3], const real_T To[3], const real_T
  *ArrivalRadius);
static void PositionRangeConverting(real_T InputPoint[3]);
static void CheckpointCreator(const real_T Initial_point[3], const real_T TDP[3],
  real_T Angle, real_T BoxStep, real_T AMP[3], real_T BLU[3], real_T BLD[3],
  real_T BRU[3], real_T BRD[3]);
static uint8_T VerticeSelect(const real_T AMP[3], const real_T BLU[3], const
  real_T BLD[3], const real_T BRU[3], const real_T BRD[3]);

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
 *    '<S4>/Heading_true'
 *    '<S4>/Heading_true1'
 */
static void Heading_true(const real_T rtu_u1[3], const real_T rtu_u0[3],
  DW_Heading_true *localDW)
{
  real_T un_pow1;
  real_T un_pow2;
  real_T un_idx_1;
  real_T un_idx_0;

  /*  Высчитаем, как изменились координаты */
  /* :  un = u1 - u0; */
  un_idx_0 = rtu_u1[0] - rtu_u0[0];
  un_idx_1 = rtu_u1[1] - rtu_u0[1];

  /* :  d_lat = un(1); */
  /* :  d_lon = un(2); */
  /*  Условие для определения кратчайшего пути с учетом переходов между */
  /*  меридианами 0 и 180 по долготе */
  /* :  if d_lon > 180 */
  if (un_idx_1 > 180.0) {
    /* :  d_lon = d_lon - 360; */
    un_idx_1 -= 360.0;
  } else {
    if (un_idx_1 < -180.0) {
      /* :  elseif d_lon < (-180) */
      /* :  d_lon = d_lon +360; */
      un_idx_1 += 360.0;
    }
  }

  /*  Расчет курса */
  /* :  un_pow1 = d_lat*d_lat; */
  un_pow1 = un_idx_0 * un_idx_0;

  /* :  un_pow2 = d_lon*d_lon; */
  un_pow2 = un_idx_1 * un_idx_1;

  /* :  if (un_pow1 < 1e-16) && (un_pow2 < 1e-16) */
  if ((un_pow1 < 1.0E-16) && (un_pow2 < 1.0E-16)) {
    /* :  y = 0; */
    localDW->y = 0.0;
  } else {
    /* :  else */
    /* :  x = acos( d_lon / (sqrt(un_pow2 + un_pow1))); */
    /* :  y = acos( d_lat / (sqrt(un_pow2 + un_pow1))); */
    un_pow1 = sqrt(un_pow2 + un_pow1);
    localDW->y = un_idx_0 / un_pow1;
    localDW->y = acos(localDW->y);

    /* :  if x > 1.5708 */
    if (acos(un_idx_1 / un_pow1) > 1.5708) {
      /* :  y = 6.28319 - y; */
      localDW->y = 6.28319 - localDW->y;
    }
  }
}

/* Output and update for function-call system: '<S2>/Simulink.Targeting' */
static void SimulinkTargeting(const real_T rtu_Position[3], const real_T
  rtu_Target[3], DW_SimulinkTargeting *localDW)
{
  real_T rtb_Memory[3];
  real_T rtb_Sum1;

  /* Memory: '<S4>/Memory' */
  rtb_Memory[0] = localDW->Memory_PreviousInput[0];
  rtb_Memory[1] = localDW->Memory_PreviousInput[1];
  rtb_Memory[2] = localDW->Memory_PreviousInput[2];

  /* MATLAB Function: '<S4>/Heading_true' */
  /*  u0 - точка отсчета */
  /*  u1 - конечная точка */
  /*  la - долгота */
  /*  fi - широта */
  /* :  la1 = u0(2)*pi/180; */
  /* :  fi1 = u0(1)*pi/180; */
  /* :  la2 = u1(2)*pi/180; */
  /* :  fi2 = u1(1)*pi/180; */
  /* :  delta_la = la2-la1; */
  /* :  y = atan2(sin(delta_la)*cos(fi2), cos(fi1)*sin(fi2)-sin(fi1)*cos(fi2)*cos(delta_la)); */
  /* :  y = rem((y + 2*pi), 2*pi); */
  Heading_true(rtu_Target, rtu_Position, &localDW->sf_Heading_true);

  /* MATLAB Function: '<S4>/Heading_true1' */
  Heading_true(rtu_Position, rtb_Memory, &localDW->sf_Heading_true1);

  /* Sum: '<S4>/Sum1' */
  rtb_Sum1 = localDW->sf_Heading_true.y - localDW->sf_Heading_true1.y;

  /* MATLAB Function: '<S4>/ControlDemode' */
  /*  Здесь переводим угол из диапазона [0 360] */
  /*  в диапазон [-180 180], что позволит нам определять */
  /*  оптимальное направление разворота (лево и право) */
  /* :  y = u; */
  /*  Если больше 180 */
  /* :  if y > 3.14 */
  if (rtb_Sum1 > 3.14) {
    /*  то это уже отрицательная полуокружность */
    /* :  y = y - 6.2819; */
    rtb_Sum1 -= 6.2819;

    /*  Если меньше -180 */
  } else {
    if (rtb_Sum1 < -3.14) {
      /* :  elseif (y < (-3.14)) */
      /*  то это уже положительная полуокружность */
      /* :  y = y + 6.2819; */
      rtb_Sum1 += 6.2819;
    }
  }

  /* End of MATLAB Function: '<S4>/ControlDemode' */

  /* Gain: '<S13>/Gain' */
  rtb_Sum1 *= 57.295779513082323;

  /* Saturate: '<S4>/Base_Sat' */
  /* :  if abs(u) < 20 */
  if (rtb_Sum1 > 50.0) {
    rtb_Sum1 = 50.0;
  } else {
    if (rtb_Sum1 < -50.0) {
      rtb_Sum1 = -50.0;
    }
  }

  /* End of Saturate: '<S4>/Base_Sat' */

  /* ManualSwitch: '<S14>/Manual Switch' incorporates:
   *  Gain: '<S14>/Gain'
   *  Gain: '<S14>/Gain1'
   *  Rounding: '<S14>/Rounding Function'
   */
  localDW->ManualSwitch = floor(0.1 * rtb_Sum1) * 10.0;

  /* Update for Memory: '<S4>/Memory' */
  localDW->Memory_PreviousInput[0] = rtu_Position[0];
  localDW->Memory_PreviousInput[1] = rtu_Position[1];
  localDW->Memory_PreviousInput[2] = rtu_Position[2];
}

/*
 * Function for Chart: '<S1>/Chart'
 * function y = DistanceCalculator (Position, Target)
 *  Вычитаем расстояние (через вектора)
 *  dif = (TDP - ActualPoint);
 *  % Вычисляем проекции на оси с переводом в Км.
 *  distance_lon2x = dif(1)*40000/360*cos(dif(2)*pi/180);
 *  distance_lat2y = dif(2)*40000/360;
 *  distance = sqrt(distance_lon2x^2 + distance_lat2y^2);
 */
static real_T DistanceCalculator(const real_T Position[3], const real_T Target[3])
{
  real_T fi1;
  real_T fi2;

  /*  Вычитаем расстояние (через тригонометрию) */
  /* :  la1 = Position(2)*pi/180; */
  /* :  fi1 = Position(1)*pi/180; */
  fi1 = Position[0] * 3.1415926535897931 / 180.0;

  /* :  la2 = Target(2)*pi/180; */
  /* :  fi2 = Target(1)*pi/180; */
  fi2 = Target[0] * 3.1415926535897931 / 180.0;

  /* :  d = sin(fi1)*sin(fi2) + cos(fi1)*cos(-fi2)*cos(la2-la1); */
  /*  6371 км средний радиус земли */
  /* :  y = acos(d)*6371e3; */
  return acos(cos(Target[1] * 3.1415926535897931 / 180.0 - Position[1] *
                  3.1415926535897931 / 180.0) * (cos(fi1) * cos(-fi2)) + sin(fi1)
              * sin(fi2)) * 6.371E+6;
}

/*
 * Function for Chart: '<S1>/Chart'
 * function y = CheckFinalMane (Position, Target, HorSpeed, VertSpeed)
 *  Вычислим расстояние с помощью функции DistanceCalculator()
 */
static real_T CheckFinalMane(const real_T Position[3], const real_T Target[3],
  real_T HorSpeed, real_T VertSpeed)
{
  real_T y;

  /* :  distance = DistanceCalculator(Position, Target); */
  /*  Узнаем угол между текущим курсом и курсом до цели */
  /* diff_angle = HeadingTrue(Target, Posintion); */
  /*  Вычисляем время необходимое для преодоления расстояния до TDP и до посадки */
  /* :  T_hor = distance/HorSpeed; */
  /* :  T_vert = (Position(3)-Target(3))/VertSpeed; */
  /*  Возвращаем сигнал о необходимости перейти на финальный маневр */
  /* :  if T_hor > T_vert */
  if (DistanceCalculator(Position, Target) / HorSpeed > (Position[2] - Target[2])
      / VertSpeed) {
    /* :  y = 1; */
    y = 1.0;
  } else {
    /* :  else */
    /* :  y = 0; */
    y = 0.0;
  }

  return y;
}

/*
 * Function for Chart: '<S1>/Chart'
 * function y = CheckArrival(Now,To)
 *  Вычитаем коррдинаты и берем модуль
 * dif = (To - Now);
 * distance = sqrt(dif(1)^2 + dif(2)^2);
 *  Вычисляем расстояние с помощью DistanceCalculator
 */
static real_T CheckArrival(const real_T Now[3], const real_T To[3], const real_T
  *ArrivalRadius)
{
  real_T y;

  /* :  distance = DistanceCalculator(Now,To); */
  /*  Попаданием будем считать радиус в 200м */
  /* :  if distance < ArrivalRadius */
  if (DistanceCalculator(Now, To) < *ArrivalRadius) {
    /* :  y = 1; */
    y = 1.0;
  } else {
    /* :  else */
    /* :  y = 0; */
    y = 0.0;
  }

  return y;
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
 * Function for Chart: '<S1>/Chart'
 * function OutputPoint = PositionRangeConverting(InputPoint)
 *  Контроль пределов Координат
 *  InputPoint(1) - Широта
 *  InputPoint(2) - Долгота
 *  InputPoint(3) - Высота
 */
static void PositionRangeConverting(real_T InputPoint[3])
{
  /* :  OutputPoint = InputPoint; */
  /*  Преобразование Долготы */
  /* :  if OutputPoint(2) > 180 */
  if (InputPoint[1] > 180.0) {
    /* :  OutputPoint(2) = OutputPoint(2) - 360; */
    InputPoint[1] -= 360.0;
  } else {
    if (InputPoint[1] < -180.0) {
      /* :  elseif OutputPoint(2) < (-180) */
      /* :  OutputPoint(2) = OutputPoint(2) + 360; */
      InputPoint[1] += 360.0;
    }
  }
}

/*
 * Function for Chart: '<S1>/Chart'
 * function [AMP, BLU, BLD, BRU, BRD] = CheckpointCreator(Initial_point, TDP, Angle, BoxStep)
 *  Расшифровка переменных
 *  AMP - Координата углового манёвра
 *  BLU - Координата верхней левой точки квадрата
 *  BLD - Координата нижней левой точки квадрата
 *  BRU - Координата верхней правой точки квадрата
 *  BRD - Координата нижней правой точки квадрата
 *  AMP, BLU, BLD, BRU, BRD,
 *  Индексы
 *  Это точка углового манёвра расчитанная по равностороннему треугольнику
 */
static void CheckpointCreator(const real_T Initial_point[3], const real_T TDP[3],
  real_T Angle, real_T BoxStep, real_T AMP[3], real_T BLU[3], real_T BLD[3],
  real_T BRU[3], real_T BRD[3])
{
  real_T dx;
  real_T dy;
  real_T BLU_tmp;
  real_T BLD_tmp;
  real_T LatStep_tmp;

  /* :  x1 = Initial_point(2); */
  /* :  y1 = Initial_point(1); */
  /* :  x2 = TDP(2); */
  /* :  y2 = TDP(1); */
  /*  Возьмем половину расстояния до точки посадки и повернём этот вектор на */
  /* :  dx = (x2-x1)/2; */
  dx = (TDP[1] - Initial_point[1]) / 2.0;

  /* :  dy = (y2-y1)/2; */
  dy = (TDP[0] - Initial_point[0]) / 2.0;

  /* :  x3 = dx*cos(Angle*pi/180) - dy*sin(Angle*pi/180)+x1; */
  /* :  y3 = dx*sin(Angle*pi/180) + dy*cos(Angle*pi/180)+y1; */
  /* :  AMP = [y3;x3;0]; */
  /*  Расчитываем точки квадрата */
  /* :  BLU=[0; 0; 0]; */
  /* :  BLD = [0; 0; 0]; */
  /* :  BRU = [0; 0; 0]; */
  /* :  BRD=[0; 0; 0]; */
  BLU[2] = 0.0;
  BLD[2] = 0.0;
  BRU[2] = 0.0;
  BRD[2] = 0.0;

  /*  Вычисляем шаг в градусах по Широте (однозначно сопоставляются) */
  /* :  LatStep = BoxStep/1000*360/40000; */
  LatStep_tmp = BoxStep / 1000.0 * 360.0 / 40000.0;

  /*  Координаты по Широте */
  /* :  BLU(1) = TDP(1) + LatStep; */
  BLU_tmp = TDP[0] + LatStep_tmp;
  BLU[0] = BLU_tmp;

  /* :  BLD(1) = TDP(1) - LatStep; */
  BLD_tmp = TDP[0] - LatStep_tmp;
  BLD[0] = BLD_tmp;

  /* :  BRU(1) = TDP(1) + LatStep; */
  BRU[0] = BLU_tmp;

  /* :  BRD(1) = TDP(1) - LatStep; */
  BRD[0] = BLD_tmp;

  /*  Вычисляем шаг по Долготе без учета поправки на Широту (долгота зависит от */
  /*  широты), а поправку на широту будем делать прямо в выражении */
  /* :  LonStep = BoxStep/1000*360/40000; */
  /*  Координаты по Долготе */
  /* :  BLU(2) = TDP(2) - LonStep/cos(pi*BLU(1)/180); */
  BLU[1] = TDP[1] - LatStep_tmp / cos(3.1415926535897931 * BLU_tmp / 180.0);

  /* :  BLD(2) = TDP(2) - LonStep/cos(pi*BLD(1)/180); */
  BLD[1] = TDP[1] - LatStep_tmp / cos(3.1415926535897931 * BLD_tmp / 180.0);

  /* :  BRU(2) = TDP(2) + LonStep/cos(pi*BRU(1)/180); */
  BRU[1] = LatStep_tmp / cos(3.1415926535897931 * BLU_tmp / 180.0) + TDP[1];

  /* :  BRD(2) = TDP(2) + LonStep/cos(pi*BRD(1)/180); */
  BRD[1] = LatStep_tmp / cos(3.1415926535897931 * BLD_tmp / 180.0) + TDP[1];

  /*  Проверим выход точек за пределы координат */
  /* :  AMP = PositionRangeConverting(AMP); */
  LatStep_tmp = Angle * 3.1415926535897931 / 180.0;
  BLU_tmp = cos(LatStep_tmp);
  LatStep_tmp = sin(LatStep_tmp);
  AMP[0] = (LatStep_tmp * dx + BLU_tmp * dy) + Initial_point[0];
  AMP[1] = (BLU_tmp * dx - LatStep_tmp * dy) + Initial_point[1];
  AMP[2] = 0.0;
  PositionRangeConverting(AMP);

  /* :  BLU = PositionRangeConverting(BLU); */
  PositionRangeConverting(BLU);

  /* :  BLD = PositionRangeConverting(BLD); */
  PositionRangeConverting(BLD);

  /* :  BRU = PositionRangeConverting(BRU); */
  PositionRangeConverting(BRU);

  /* :  BRD = PositionRangeConverting(BRD); */
  PositionRangeConverting(BRD);
}

/*
 * Function for Chart: '<S1>/Chart'
 * function Vertice = VerticeSelect(AMP, BLU, BLD, BRU, BRD)
 *  Функция определения точки входа в "посадочной коробке"
 */
static uint8_T VerticeSelect(const real_T AMP[3], const real_T BLU[3], const
  real_T BLD[3], const real_T BRU[3], const real_T BRD[3])
{
  uint8_T Vertice;
  real_T x[4];
  int32_T b_Vertice;

  /* :  D_BLU = DistanceCalculator(AMP,BLU); */
  /* :  D_BLD = DistanceCalculator(AMP,BLD); */
  /* :  D_BRU = DistanceCalculator(AMP,BRU); */
  /* :  D_BRD = DistanceCalculator(AMP,BRD); */
  /*  Соответсвие значения Vertice и вершин коробки */
  /*  1 - Левая верхняя */
  /*  2 - Левая нижняя */
  /*  3 - Правая верхняя */
  /*  4 - Правая нижняя */
  /* :  x = [D_BLU, D_BLD, D_BRU, D_BRD]; */
  x[0] = DistanceCalculator(AMP, BLU);
  x[1] = DistanceCalculator(AMP, BLD);
  x[2] = DistanceCalculator(AMP, BRU);
  x[3] = DistanceCalculator(AMP, BRD);

  /* :  Vertice = 1 */
  b_Vertice = 0;

  /* :  for i = 1:1:3 */
  /* :  if x(i+1) < x(Vertice) */
  if (DistanceCalculator(AMP, BLD) < DistanceCalculator(AMP, BLU)) {
    /* :  Vertice = i+1 */
    b_Vertice = 1;
  }

  /* :  if x(i+1) < x(Vertice) */
  if (DistanceCalculator(AMP, BRU) < x[b_Vertice]) {
    /* :  Vertice = i+1 */
    b_Vertice = 2;
  }

  /* :  if x(i+1) < x(Vertice) */
  if (DistanceCalculator(AMP, BRD) < x[b_Vertice]) {
    /* :  Vertice = i+1 */
    b_Vertice = 3;
  }

  /* :  Vertice = uint8(Vertice); */
  Vertice = (uint8_T)(b_Vertice + 1);
  return Vertice;
}

/* Model step function */
void Easy_reg_step(void)
{
  real_T ArrivalRadius;
  real_T TmpSignalConversionAtSFunctionI[3];
  real_T TmpSignalConversionAtSFunctio_g[3];
  real_T Target[3];
  int16_T tmp;
  boolean_T guard1 = false;

  /* Outputs for Atomic SubSystem: '<Root>/Easy_reg' */
  /* DataStoreWrite: '<S1>/Data Store Write2' incorporates:
   *  Constant: '<S1>/ArrivalRadius'
   */
  ArrivalRadius = 500.0;

  /* SignalConversion: '<S2>/TmpSignal ConversionAt SFunction Inport1' incorporates:
   *  Chart: '<S1>/Chart'
   *  Inport: '<Root>/Pos_alt'
   *  Inport: '<Root>/Pos_lat'
   *  Inport: '<Root>/Pos_lon'
   */
  TmpSignalConversionAtSFunctionI[0] = Easy_reg_U.Pos_lat;
  TmpSignalConversionAtSFunctionI[1] = Easy_reg_U.Pos_lon;
  TmpSignalConversionAtSFunctionI[2] = Easy_reg_U.Pos_alt;

  /* SignalConversion: '<S2>/TmpSignal ConversionAt SFunction Inport2' incorporates:
   *  Chart: '<S1>/Chart'
   *  Inport: '<Root>/TDP_alt'
   *  Inport: '<Root>/TDP_lat'
   *  Inport: '<Root>/TDP_lon'
   */
  TmpSignalConversionAtSFunctio_g[0] = Easy_reg_U.TDP_lat;
  TmpSignalConversionAtSFunctio_g[1] = Easy_reg_U.TDP_lon;
  TmpSignalConversionAtSFunctio_g[2] = Easy_reg_U.TDP_alt;

  /* Chart: '<S1>/Chart' incorporates:
   *  Constant: '<S1>/AngleManevr'
   *  Constant: '<S1>/BoxSize'
   *  DataStoreWrite: '<S1>/Data Store Write'
   *  DataStoreWrite: '<S1>/Data Store Write1'
   *  Inport: '<Root>/Pos_alt'
   *  Inport: '<Root>/Relief'
   *  Inport: '<Root>/ReliefAvailable'
   *  Inport: '<Root>/TDP_alt'
   *  Inport: '<Root>/TDP_lat'
   *  Inport: '<Root>/TDP_lon'
   */
  if (Easy_reg_DW.is_active_c1_Easy_reg == 0U) {
    Easy_reg_DW.is_active_c1_Easy_reg = 1U;

    /*  Основной цикл задач */
    Easy_reg_DW.is_Task_Control = IN_Task_Init;

    /*  Производим планирование путевых точек для сброса высоты */
    /* :  [AngleManPoint, BoxLUPoint, BoxLDPoint, BoxRUPoint, BoxRDPoint] = math.CheckpointCreator(ActualPoint, TDPoint, AngleManevr, BoxSize); */
    CheckpointCreator(TmpSignalConversionAtSFunctionI,
                      TmpSignalConversionAtSFunctio_g, 30.0, 1500.0,
                      Easy_reg_DW.AngleManPoint, Easy_reg_DW.BoxLUPoint,
                      Easy_reg_DW.BoxLDPoint, Easy_reg_DW.BoxRUPoint,
                      Easy_reg_DW.BoxRDPoint);

    /* :  BoxEntry = math.VerticeSelect(AngleManPoint, BoxLUPoint, BoxLDPoint, BoxRUPoint, BoxRDPoint); */
    Easy_reg_DW.BoxEntry = VerticeSelect(Easy_reg_DW.AngleManPoint,
      Easy_reg_DW.BoxLUPoint, Easy_reg_DW.BoxLDPoint, Easy_reg_DW.BoxRUPoint,
      Easy_reg_DW.BoxRDPoint);

    /* :  TargetPoint = AngleManPoint; */
    Easy_reg_DW.TargetPoint[0] = Easy_reg_DW.AngleManPoint[0];
    Easy_reg_DW.TargetPoint[1] = Easy_reg_DW.AngleManPoint[1];
    Easy_reg_DW.TargetPoint[2] = Easy_reg_DW.AngleManPoint[2];

    /* Outport: '<Root>/BIM_CMD' incorporates:
     *  Constant: '<S1>/AngleManevr'
     *  Constant: '<S1>/BoxSize'
     *  DataStoreWrite: '<S1>/Data Store Write'
     *  DataStoreWrite: '<S1>/Data Store Write1'
     */
    /*  Инициализируем выходы */
    /* :  BIM_CMD = 0; */
    Easy_reg_Y.BIM_CMD = 0.0;

    /* :  TD_CMD = 0; */
    /* Здесь мы отдельно будем контролировать высоту, */
    /*  для своевременного открытия посадотчных парашютов */
    Easy_reg_DW.is_Task_Touchdown = IN_OFF;

    /* :  TD_CMD = 0; */
    Easy_reg_DW.TD_CMD = 0U;
  } else {
    /*  Основной цикл задач */
    switch (Easy_reg_DW.is_Task_Control) {
     case IN_Task_Flight:
      /*  Режим полета по точкам */
      /* :  BIM_CMD =Simulink.Targeting (ActualPoint, TargetPoint, ActualCourse); */
      Target[0] = Easy_reg_DW.TargetPoint[0];
      Target[1] = Easy_reg_DW.TargetPoint[1];
      Target[2] = Easy_reg_DW.TargetPoint[2];

      /* Outputs for Function Call SubSystem: '<S2>/Simulink.Targeting' */
      SimulinkTargeting(TmpSignalConversionAtSFunctionI, Target,
                        &Easy_reg_DW.SimulinkTargeting_e);

      /* End of Outputs for SubSystem: '<S2>/Simulink.Targeting' */

      /* Outport: '<Root>/BIM_CMD' */
      Easy_reg_Y.BIM_CMD = Easy_reg_DW.SimulinkTargeting_e.ManualSwitch;
      if (Easy_reg_DW.is_Task_Flight == IN_Task_FinalReplace) {
        /*  Состояние последнего манёвра */
        /*  Цель которого выйти на заданную */
        /*  точку приземления */
        /* :  TargetPoint = TDPoint; */
        Easy_reg_DW.TargetPoint[0] = Easy_reg_U.TDP_lat;
        Easy_reg_DW.TargetPoint[1] = Easy_reg_U.TDP_lon;
        Easy_reg_DW.TargetPoint[2] = Easy_reg_U.TDP_alt;
      } else {
        /* :  sf_internal_predicateOutput = ... */
        /* :  math.CheckFinalMane (ActualPoint, TDPoint, 20, 11) == 1; */
        if (CheckFinalMane(TmpSignalConversionAtSFunctionI,
                           TmpSignalConversionAtSFunctio_g, 20.0, 11.0) == 1.0)
        {
          Easy_reg_DW.is_Task_TargetReplace = IN_NO_ACTIVE_CHILD;
          Easy_reg_DW.is_Task_Flight = IN_Task_FinalReplace;

          /*  Состояние последнего манёвра */
          /*  Цель которого выйти на заданную */
          /*  точку приземления */
          /* :  TargetPoint = TDPoint; */
          Easy_reg_DW.TargetPoint[0] = Easy_reg_U.TDP_lat;
          Easy_reg_DW.TargetPoint[1] = Easy_reg_U.TDP_lon;
          Easy_reg_DW.TargetPoint[2] = Easy_reg_U.TDP_alt;
        } else {
          /*  Задача контроля и подмены цели */
          /*  Anlge - нацеливание на координату уголового манёвра */
          /*  LU - Нацеливание на верхний левый угол "коробки" */
          /*  LD - Нацеливание на нижний левый угол "коробки" */
          /*  RD - Нацеливание на нижний правый угол "коробки" */
          /*  RU - Нацеливание на верхний правый угол "коробки" */
          switch (Easy_reg_DW.is_Task_TargetReplace) {
           case IN_Angle:
            /* :  sf_internal_predicateOutput = ... */
            /* :  math.CheckArrival(ActualPoint, TargetPoint) == 1; */
            if (CheckArrival(TmpSignalConversionAtSFunctionI,
                             Easy_reg_DW.TargetPoint, &ArrivalRadius) == 1.0) {
              /* :  sf_internal_predicateOutput = ... */
              /* :  BoxEntry == 1; */
              switch (Easy_reg_DW.BoxEntry) {
               case 1:
                /* :  TargetPoint = BoxLUPoint; */
                Easy_reg_DW.TargetPoint[0] = Easy_reg_DW.BoxLUPoint[0];
                Easy_reg_DW.TargetPoint[1] = Easy_reg_DW.BoxLUPoint[1];
                Easy_reg_DW.TargetPoint[2] = Easy_reg_DW.BoxLUPoint[2];
                Easy_reg_DW.is_Task_TargetReplace = IN_LU;
                break;

               case 4:
                /* :  sf_internal_predicateOutput = ... */
                /* :  BoxEntry == 4; */
                /* :  TargetPoint = BoxRDPoint; */
                Easy_reg_DW.TargetPoint[0] = Easy_reg_DW.BoxRDPoint[0];
                Easy_reg_DW.TargetPoint[1] = Easy_reg_DW.BoxRDPoint[1];
                Easy_reg_DW.TargetPoint[2] = Easy_reg_DW.BoxRDPoint[2];
                Easy_reg_DW.is_Task_TargetReplace = IN_RD;
                break;

               case 2:
                /* :  sf_internal_predicateOutput = ... */
                /* :  BoxEntry == 2; */
                /* :  TargetPoint = BoxLDPoint; */
                Easy_reg_DW.TargetPoint[0] = Easy_reg_DW.BoxLDPoint[0];
                Easy_reg_DW.TargetPoint[1] = Easy_reg_DW.BoxLDPoint[1];
                Easy_reg_DW.TargetPoint[2] = Easy_reg_DW.BoxLDPoint[2];
                Easy_reg_DW.is_Task_TargetReplace = IN_LD;
                break;

               case 3:
                /* :  sf_internal_predicateOutput = ... */
                /* :  BoxEntry == 3; */
                /* :  TargetPoint = BoxRUPoint; */
                Easy_reg_DW.TargetPoint[0] = Easy_reg_DW.BoxRUPoint[0];
                Easy_reg_DW.TargetPoint[1] = Easy_reg_DW.BoxRUPoint[1];
                Easy_reg_DW.TargetPoint[2] = Easy_reg_DW.BoxRUPoint[2];
                Easy_reg_DW.is_Task_TargetReplace = IN_RU;
                break;
              }
            }
            break;

           case IN_LD:
            /* :  sf_internal_predicateOutput = ... */
            /* :  math.CheckArrival(ActualPoint, TargetPoint) == 1; */
            if (CheckArrival(TmpSignalConversionAtSFunctionI,
                             Easy_reg_DW.TargetPoint, &ArrivalRadius) == 1.0) {
              /* :  TargetPoint = BoxRDPoint; */
              Easy_reg_DW.TargetPoint[0] = Easy_reg_DW.BoxRDPoint[0];
              Easy_reg_DW.TargetPoint[1] = Easy_reg_DW.BoxRDPoint[1];
              Easy_reg_DW.TargetPoint[2] = Easy_reg_DW.BoxRDPoint[2];
              Easy_reg_DW.is_Task_TargetReplace = IN_RD;
            }
            break;

           case IN_LU:
            /* :  sf_internal_predicateOutput = ... */
            /* :  math.CheckArrival(ActualPoint, TargetPoint) == 1; */
            if (CheckArrival(TmpSignalConversionAtSFunctionI,
                             Easy_reg_DW.TargetPoint, &ArrivalRadius) == 1.0) {
              /* :  TargetPoint = BoxLDPoint; */
              Easy_reg_DW.TargetPoint[0] = Easy_reg_DW.BoxLDPoint[0];
              Easy_reg_DW.TargetPoint[1] = Easy_reg_DW.BoxLDPoint[1];
              Easy_reg_DW.TargetPoint[2] = Easy_reg_DW.BoxLDPoint[2];
              Easy_reg_DW.is_Task_TargetReplace = IN_LD;
            }
            break;

           case IN_RD:
            /* :  sf_internal_predicateOutput = ... */
            /* :  math.CheckArrival(ActualPoint, TargetPoint) == 1; */
            if (CheckArrival(TmpSignalConversionAtSFunctionI,
                             Easy_reg_DW.TargetPoint, &ArrivalRadius) == 1.0) {
              /* :  TargetPoint = BoxRUPoint; */
              Easy_reg_DW.TargetPoint[0] = Easy_reg_DW.BoxRUPoint[0];
              Easy_reg_DW.TargetPoint[1] = Easy_reg_DW.BoxRUPoint[1];
              Easy_reg_DW.TargetPoint[2] = Easy_reg_DW.BoxRUPoint[2];
              Easy_reg_DW.is_Task_TargetReplace = IN_RU;
            }
            break;

           default:
            /* :  sf_internal_predicateOutput = ... */
            /* :  math.CheckArrival(ActualPoint, TargetPoint) == 1; */
            if (CheckArrival(TmpSignalConversionAtSFunctionI,
                             Easy_reg_DW.TargetPoint, &ArrivalRadius) == 1.0) {
              /* :  TargetPoint = BoxLUPoint; */
              Easy_reg_DW.TargetPoint[0] = Easy_reg_DW.BoxLUPoint[0];
              Easy_reg_DW.TargetPoint[1] = Easy_reg_DW.BoxLUPoint[1];
              Easy_reg_DW.TargetPoint[2] = Easy_reg_DW.BoxLUPoint[2];
              Easy_reg_DW.is_Task_TargetReplace = IN_LU;
            }
            break;
          }
        }
      }
      break;

     case IN_Task_Init:
      /* :  sf_internal_predicateOutput = ... */
      /* :  Mode == 1; */
      Easy_reg_DW.is_Task_Control = IN_Task_Flight;

      /*  Режим полета по точкам */
      /* :  BIM_CMD =Simulink.Targeting (ActualPoint, TargetPoint, ActualCourse); */
      Target[0] = Easy_reg_DW.TargetPoint[0];
      Target[1] = Easy_reg_DW.TargetPoint[1];
      Target[2] = Easy_reg_DW.TargetPoint[2];

      /* Outputs for Function Call SubSystem: '<S2>/Simulink.Targeting' */
      SimulinkTargeting(TmpSignalConversionAtSFunctionI, Target,
                        &Easy_reg_DW.SimulinkTargeting_e);

      /* End of Outputs for SubSystem: '<S2>/Simulink.Targeting' */

      /* Outport: '<Root>/BIM_CMD' */
      Easy_reg_Y.BIM_CMD = Easy_reg_DW.SimulinkTargeting_e.ManualSwitch;
      Easy_reg_DW.is_Task_Flight = IN_Task_TargetReplace;

      /*  Задача контроля и подмены цели */
      /*  Anlge - нацеливание на координату уголового манёвра */
      /*  LU - Нацеливание на верхний левый угол "коробки" */
      /*  LD - Нацеливание на нижний левый угол "коробки" */
      /*  RD - Нацеливание на нижний правый угол "коробки" */
      /*  RU - Нацеливание на верхний правый угол "коробки" */
      /* :  TargetPoint = AngleManPoint; */
      Easy_reg_DW.TargetPoint[0] = Easy_reg_DW.AngleManPoint[0];
      Easy_reg_DW.TargetPoint[1] = Easy_reg_DW.AngleManPoint[1];
      Easy_reg_DW.TargetPoint[2] = Easy_reg_DW.AngleManPoint[2];
      Easy_reg_DW.is_Task_TargetReplace = IN_Angle;
      break;

     default:
      /* Outputs for Function Call SubSystem: '<S2>/Simulink.Targeting' */

      /*  Режим прицеливания на TDP */
      /* :  BIM_CMD =Simulink.Targeting (ActualPoint, TDPoint, ActualCourse); */
      SimulinkTargeting(TmpSignalConversionAtSFunctionI,
                        TmpSignalConversionAtSFunctio_g,
                        &Easy_reg_DW.SimulinkTargeting_e);

      /* End of Outputs for SubSystem: '<S2>/Simulink.Targeting' */

      /* Outport: '<Root>/BIM_CMD' */
      Easy_reg_Y.BIM_CMD = Easy_reg_DW.SimulinkTargeting_e.ManualSwitch;
      break;
    }

    /* Здесь мы отдельно будем контролировать высоту, */
    /*  для своевременного открытия посадотчных парашютов */
    if (Easy_reg_DW.is_Task_Touchdown == IN_OFF) {
      /* :  sf_internal_predicateOutput = ... */
      /* :  ReliefAvailable == 0; */
      guard1 = false;
      if ((Easy_reg_U.ReliefAvailable == 0) && (Easy_reg_U.Pos_alt <=
           Easy_reg_U.TDP_alt)) {
        /* :  sf_internal_predicateOutput = ... */
        /* :  ActualPoint(3) <= TDPoint(3); */
        guard1 = true;
      } else {
        /* :  sf_internal_predicateOutput = ... */
        /* :  ReliefAvailable ==1; */
        if (Easy_reg_U.ReliefAvailable == 1) {
          /* :  sf_internal_predicateOutput = ... */
          /* :  (ActualPoint(3)-Relief) <= TDPoint(3); */
          ArrivalRadius = rt_roundd_snf(Easy_reg_U.Pos_alt - (real_T)
            Easy_reg_U.Relief);
          if (ArrivalRadius < 32768.0) {
            if (ArrivalRadius >= -32768.0) {
              tmp = (int16_T)ArrivalRadius;
            } else {
              tmp = MIN_int16_T;
            }
          } else {
            tmp = MAX_int16_T;
          }

          if (tmp <= Easy_reg_U.TDP_alt) {
            guard1 = true;
          } else {
            /* :  TD_CMD = 0; */
            Easy_reg_DW.TD_CMD = 0U;
          }
        } else {
          /* :  TD_CMD = 0; */
          Easy_reg_DW.TD_CMD = 0U;
        }
      }

      if (guard1) {
        Easy_reg_DW.is_Task_Touchdown = IN_ON;

        /* :  TD_CMD = 1; */
        Easy_reg_DW.TD_CMD = 1U;
      }
    } else {
      /* :  TD_CMD = 1; */
      Easy_reg_DW.TD_CMD = 1U;
    }
  }

  /* Outport: '<Root>/TD_CMD' incorporates:
   *  ManualSwitch: '<S1>/TD_SysSwitch'
   */
  /* :  un = u1-u0; */
  /* :  v_lat = un(1)*40000/360*1000/1; */
  /* :  v_lon = un(2)*(40000/360*1000/1 * cos(pi/180*u0(1))); */
  /* :  v_alt = un(3); */
  Easy_reg_Y.TD_CMD = Easy_reg_DW.TD_CMD;

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
