/*
 * File: flightController.c
 *
 * Code generated for Simulink model 'flightController'.
 *
 * Model version                  : 1.832
 * Simulink Coder version         : 8.14 (R2018a) 06-Feb-2018
 * C/C++ source code generated on : Fri Feb  7 17:19:33 2020
 *
 * Target selection: ert.tlc
 * Embedded hardware selection: ARM Compatible->ARM Cortex
 * Code generation objectives:
 *    1. Execution efficiency
 *    2. RAM efficiency
 * Validation result: Not run
 */

#include "flightController.h"

/* Named constants for Chart: '<S1>/LogicController' */
#define IN_Disable                     ((uint8_T)1U)
#define IN_Enable                      ((uint8_T)2U)
#define IN_Exit                        ((uint8_T)1U)
#define IN_Flight                      ((uint8_T)1U)
#define IN_Init                        ((uint8_T)2U)
#define IN_Init_n                      ((uint8_T)3U)
#define IN_Left                        ((uint8_T)2U)
#define IN_NO_ACTIVE_CHILD             ((uint8_T)0U)
#define IN_Off                         ((uint8_T)1U)
#define IN_On                          ((uint8_T)2U)
#define IN_Right                       ((uint8_T)3U)
#define IN_Shutdown                    ((uint8_T)4U)
#define IN_StepAngle                   ((uint8_T)1U)
#define IN_TDP_Only                    ((uint8_T)3U)
#define IN_Wait                        ((uint8_T)4U)
#define event_cancelFinal              (0)
#define event_final                    (1)
#define event_nextStage                (2)
#define event_nonBlockingMode          (3)
#define event_parking                  (4)
#define event_wantDisable              (5)
#define event_wantEnable               (6)

/* Named constants for Chart: '<S9>/Chart' */
#define IN_FixCurrentVector            ((uint8_T)1U)
#define IN_FixInitialVector            ((uint8_T)2U)
#define IN_GetResult                   ((uint8_T)3U)
#define IN_Initialization              ((uint8_T)4U)
#define IN_Pause                       ((uint8_T)5U)
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
static real_T nonlinearCoefficientFunction(void);
static real_T errorCourseLimit(void);
static real_T Angle2Course(real_T angle);
static real_T wherePoint(const real_T initialVectorPoint[3], const real_T
  endVectorPoint[3], const real_T point[3]);
static void BimSupply(const uint32_T *BimTimeout, int32_T *sfEvent);
static void Control(const real_T *AngleManevr, const uint32_T *AngleTimeout,
                    const uint32_T *BimTimeout, uint8_T *Mode, int32_T *sfEvent);
static real_T checkFinalMane(const real_T *V_hor, const real_T *V_vert);
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

/*
 * Function for Chart: '<S1>/LogicController'
 * function Timeoutput = TimeoutDecrement (TimeoutTag)
 */
static uint32_T TimeoutDecrement(uint32_T TimeoutTag)
{
  uint32_T Timeoutput;

  /* MATLAB Function 'TimeoutDecrement': '<S5>:573' */
  /* Graphical Function 'TimeoutDecrement': '<S5>:573' */
  /* '<S5>:577:1' sf_internal_predicateOutput = ... */
  /* '<S5>:577:1' TimeoutTag > 0; */
  if (TimeoutTag > 0U)
  {
    /* '<S5>:577:1' Timeoutput = TimeoutTag - 1; */
    Timeoutput = TimeoutTag - 1U;
  }
  else
  {
    /* '<S5>:578:1' Timeoutput = uint32(0); */
    Timeoutput = 0U;
  }

  return Timeoutput;
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
 * Function for Chart: '<S1>/LogicController'
 * function coefficient = nonlinearCoefficientFunction
 */
static real_T nonlinearCoefficientFunction(void)
{
  real_T coefficient;

  /* Outport: '<Root>/horizontalDistance' */
  /* MATLAB Function 'nonlinearCoefficientFunction': '<S5>:701' */
  /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
  /*    nonlinearCoefficientFunction - Возвращает коэффициент эффективности */
  /*    маневров потери высоты. */
  /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
  /* '<S5>:701:6' coefficient = 3*exp(-distanceToTarget/2.5e3); */
  coefficient = exp(-rtY.horizontalDistance / 2500.0) * 3.0;

  /* '<S5>:701:7' if coefficient > 1 */
  if (coefficient > 1.0)
  {
    /* '<S5>:701:8' coefficient = 1; */
    coefficient = 1.0;
  }

  return coefficient;
}

/*
 * Function for Chart: '<S1>/LogicController'
 * function status = errorCourseLimit
 *  centralCourse - курс до цели до начала манёвра
 */
static real_T errorCourseLimit(void)
{
  real_T status;
  real_T u;

  /* MATLAB Function 'errorCourseLimit': '<S5>:719' */
  /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
  /*    errorCourseLimit - Контролирует, чтобы при маневрах потери высоты, */
  /*    разница курс до цели до начала маневра и после его завершения не были  */
  /*    больше некоторого погового значения. Таким образом ограничиваем влияние */
  /*    бокового ветра (не даём в боковом маневре уйти далеко от ЛЗП); */
  /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
  /* '<S5>:719:9' result = abs(ControlDemode(courseToTargetPoint-centralCourse)); */
  u = rtDW.courseToTargetPoint - rtDW.centralCourse;

  /*  Здесь переводим угол из диапазона [0 360] */
  /*  в диапазон [-180 180], что позволит нам определять */
  /*  оптимальное направление разворота (лево и право) */
  /* 'ControlDemode:5' y = u; */
  /*  Если больше 180 */
  /* 'ControlDemode:7' if y > pi */
  if (u > 3.1415926535897931)
  {
    /*  то это уже отрицательная полуокружность */
    /* 'ControlDemode:9' y = y - 2*pi; */
    u -= 6.2831853071795862;

    /*  Если меньше -180 */
  }
  else
  {
    if (u < -3.1415926535897931)
    {
      /* 'ControlDemode:11' elseif (y < (-pi)) */
      /*  то это уже положительная полуокружность */
      /* 'ControlDemode:13' y = y + 2*pi; */
      u += 6.2831853071795862;
    }
  }

  /* '<S5>:719:10' if result > (25/180*pi) */
  if (fabs(u) > 0.43633231299858238)
  {
    /* '<S5>:719:11' status = 1; */
    status = 1.0;
  }
  else
  {
    /* '<S5>:719:12' else */
    /* '<S5>:719:13' status = 0; */
    status = 0.0;
  }

  return status;
}

/*
 * Function for Chart: '<S1>/LogicController'
 * function resultCourse = Angle2Course(angle)
 */
static real_T Angle2Course(real_T angle)
{
  real_T resultCourse;

  /* MATLAB Function 'Angle2Course': '<S5>:690' */
  /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
  /*   */
  /*  Angle2Course - Преобразование курса к используемому в модели виду */
  /*  */
  /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
  /* '<S5>:690:7' resultCourse = angle; */
  resultCourse = angle;

  /* '<S5>:690:8' if angle < 0 */
  if (angle < 0.0)
  {
    /* '<S5>:690:9' resultCourse = resultCourse + 2*pi; */
    resultCourse = angle + 6.2831853071795862;
  }

  return resultCourse;
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

/*
 * Function for Chart: '<S1>/LogicController'
 * function status = wherePoint(initialVectorPoint, endVectorPoint, point)
 *     %% Обозначим переменные
 */
static real_T wherePoint(const real_T initialVectorPoint[3], const real_T
  endVectorPoint[3], const real_T point[3])
{
  real_T status;
  real_T vectorMul;

  /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
  /*  wherePoint - Определение стороны, по которую от вектора находится точка */
  /*  Параметры: */
  /*            initialVectorPoint - Координаты начала вектора */
  /*            endVectorPoint     - Координаты конца вектора */
  /*            point              - Координаты точки */
  /*  Возвращает: */
  /*            1 - Если точка point находится слева от вектора; */
  /*           -1 - Если точка point находится справа от вектора; */
  /*            0 - Если точка point лежит на векторе. */
  /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
  /* 'wherePoint:14' ax = initialVectorPoint(2); */
  /* 'wherePoint:15' ay = initialVectorPoint(1); */
  /* 'wherePoint:16' bx = endVectorPoint(2); */
  /* 'wherePoint:17' by = endVectorPoint(1); */
  /* 'wherePoint:18' px = point(2); */
  /* 'wherePoint:19' py = point(1); */
  /*     %% Требуется построение двух векторов: */
  /*  ab вектор, относительно которой определяем положение точки */
  /*  ap вспомогательный вектор проведенный из того же начала с концом в */
  /*  точке p; */
  /* 'wherePoint:24' vectorMul = (bx-ax)*(py-ay)-(by-ay)*(px-ax); */
  vectorMul = (endVectorPoint[1] - initialVectorPoint[1]) * (point[0] -
    initialVectorPoint[0]) - (endVectorPoint[0] - initialVectorPoint[0]) *
    (point[1] - initialVectorPoint[1]);

  /* 'wherePoint:25' if vectorMul > 0 */
  if (vectorMul > 0.0)
  {
    /* 'wherePoint:26' status = 1; */
    status = 1.0;
  }
  else if (vectorMul < 0.0)
  {
    /* 'wherePoint:27' elseif vectorMul < 0 */
    /* 'wherePoint:28' status = -1; */
    status = -1.0;
  }
  else
  {
    /* 'wherePoint:29' else */
    /* 'wherePoint:30' status = 0; */
    status = 0.0;
  }

  return status;
}

/* Function for Chart: '<S1>/LogicController' */
static void BimSupply(const uint32_T *BimTimeout, int32_T *sfEvent)
{
  /* During 'BimSupply': '<S5>:452' */
  switch (rtDW.is_BimSupply)
  {
   case IN_Disable:
    /* During 'Disable': '<S5>:458' */
    /* '<S5>:706:1' sf_internal_predicateOutput = ... */
    /* '<S5>:706:1' parking; */
    if (*sfEvent == event_parking)
    {
      /* Transition: '<S5>:706' */
      rtDW.is_BimSupply = IN_Shutdown;

      /* Entry 'Shutdown': '<S5>:461' */
      /* '<S5>:461:1' enableTargetting = 0; */
      rtDW.enableTargetting = 0U;
    }
    else
    {
      /* '<S5>:622:1' sf_internal_predicateOutput = ... */
      /* '<S5>:622:1' wantEnable; */
      if ((*sfEvent == event_wantEnable) || (rtDW.timeout_b == 0U))
      {
        /* Transition: '<S5>:622' */
        /* Transition: '<S5>:460' */
        /* Transition: '<S5>:621' */
        rtDW.is_BimSupply = IN_Enable;

        /* Entry 'Enable': '<S5>:456' */
        /* '<S5>:456:1' timeout = 0; */
        rtDW.timeout_b = 0U;

        /* '<S5>:456:1' enableTargetting = 1; */
        rtDW.enableTargetting = 1U;
      }
      else
      {
        /* '<S5>:460:1' sf_internal_predicateOutput = ... */
        /* '<S5>:460:1' timeout == 0; */
      }
    }
    break;

   case IN_Enable:
    /* During 'Enable': '<S5>:456' */
    /* '<S5>:459:1' sf_internal_predicateOutput = ... */
    /* '<S5>:459:1' wantDisable; */
    switch (*sfEvent)
    {
     case event_wantDisable:
      /* Transition: '<S5>:459' */
      rtDW.is_BimSupply = IN_Disable;

      /* Entry 'Disable': '<S5>:458' */
      /* '<S5>:458:1' enableTargetting = 0; */
      rtDW.enableTargetting = 0U;

      /* '<S5>:458:1' timeout = BimTimeout; */
      rtDW.timeout_b = *BimTimeout;
      break;

     case event_parking:
      /* '<S5>:705:1' sf_internal_predicateOutput = ... */
      /* '<S5>:705:1' parking; */
      /* Transition: '<S5>:705' */
      rtDW.is_BimSupply = IN_Shutdown;

      /* Entry 'Shutdown': '<S5>:461' */
      /* '<S5>:461:1' enableTargetting = 0; */
      rtDW.enableTargetting = 0U;
      break;
    }
    break;

   case IN_Init_n:
    /* During 'Init': '<S5>:603' */
    /* Transition: '<S5>:605' */
    rtDW.is_BimSupply = IN_Enable;

    /* Entry 'Enable': '<S5>:456' */
    /* '<S5>:456:1' timeout = 0; */
    rtDW.timeout_b = 0U;

    /* '<S5>:456:1' enableTargetting = 1; */
    rtDW.enableTargetting = 1U;
    break;

   case IN_Shutdown:
    /* During 'Shutdown': '<S5>:461' */
    /* '<S5>:461:1' enableTargetting = 0; */
    rtDW.enableTargetting = 0U;
    break;
  }
}

/* Function for Chart: '<S1>/LogicController' */
static void Control(const real_T *AngleManevr, const uint32_T *AngleTimeout,
                    const uint32_T *BimTimeout, uint8_T *Mode, int32_T *sfEvent)
{
  real_T fi1;
  real_T fi2;
  int32_T b_previousEvent;
  real_T delta_la_tmp;
  boolean_T guard1 = false;
  boolean_T guard2 = false;
  boolean_T guard3 = false;

  /* During 'Control': '<S5>:9' */
  /*  Основной цикл задач */
  guard1 = false;
  guard2 = false;
  guard3 = false;
  switch (rtDW.is_Control)
  {
   case IN_Flight:
    /* During 'Flight': '<S5>:23' */
    /* '<S5>:23:1' courseToTargetPoint = Azimut(targetPoint + touchdownPoint_Offset, currentPoint); */
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
    fi2 = (rtDW.targetPoint[0] + rtDW.touchdownPoint_Offset[0]) *
      3.1415926535897931 / 180.0;

    /* 'Azimut:15' delta_la = la2-la1; */
    delta_la_tmp = (rtDW.targetPoint[1] + rtDW.touchdownPoint_Offset[1]) *
      3.1415926535897931 / 180.0 - rtDW.TmpSignalConversionAtSFunctio_g[1] *
      3.1415926535897931 / 180.0;

    /* 'Azimut:16' y = atan2(sin(delta_la)*cos(fi2), cos(fi1)*sin(fi2)-sin(fi1)*cos(fi2)*cos(delta_la)); */
    rtDW.courseToTargetPoint = rt_atan2d_snf(sin(delta_la_tmp) * cos(fi2), cos
      (fi1) * sin(fi2) - sin(fi1) * cos(fi2) * cos(delta_la_tmp));

    /* 'Azimut:17' y = rem((y + 2*pi), 2*pi); */
    rtDW.courseToTargetPoint = rt_remd_snf(rtDW.courseToTargetPoint
      + 6.2831853071795862, 6.2831853071795862);

    /* courseToTargetPoint = Heading(targetPoint + touchdownPoint_Offset, currentPoint, 0); */
    /* '<S5>:23:3' desiredCourse = courseToTargetPoint; */
    rtDW.desiredCourse = rtDW.courseToTargetPoint;
    if (rtDW.is_Flight != IN_StepAngle)
    {
    }
    else
    {
      /* During 'StepAngle': '<S5>:582' */
      /* '<S5>:582:1' coeff = nonlinearCoefficientFunction; */
      fi1 = nonlinearCoefficientFunction();

      /* Outport: '<Root>/touchdownTime' incorporates:
       *  Outport: '<Root>/onTargetTime'
       */
      /* '<S5>:582:1' timeDifference = timeVertical - timeHorizontal; */
      fi2 = rtY.touchdownTime - rtY.onTargetTime;
      switch (rtDW.is_StepAngle)
      {
       case IN_Exit:
        /* During 'Exit': '<S5>:684' */
        break;

       case IN_Left:
        /* During 'Left': '<S5>:592' */
        /* '<S5>:589:1' sf_internal_predicateOutput = ... */
        /* '<S5>:589:1' timeout == 0; */
        if (rtDW.timeout == 0U)
        {
          /* Transition: '<S5>:589' */
          /* '<S5>:589:1' timeout = AngleTimeout; */
          rtDW.timeout = *AngleTimeout;
          guard1 = true;
        }
        else
        {
          /* '<S5>:717:1' sf_internal_predicateOutput = ... */
          /* '<S5>:717:1' errorCourseLimit; */
          fi1 = errorCourseLimit();
          if (fi1 != 0.0)
          {
            /* Transition: '<S5>:717' */
            /* '<S5>:717:1' timeout = AngleTimeout/2; */
            rtDW.timeout = (uint32_T)rt_roundd_snf((real_T)*AngleTimeout / 2.0);
            guard1 = true;
          }
          else
          {
            /* '<S5>:592:4' desiredCourse = Angle2Course(desiredCourse-AngleManevr); */
            rtDW.desiredCourse = Angle2Course(rtDW.desiredCourse - *AngleManevr);
          }
        }
        break;

       case IN_Right:
        /* During 'Right': '<S5>:590' */
        /* '<S5>:585:1' sf_internal_predicateOutput = ... */
        /* '<S5>:585:1' timeout == 0; */
        if (rtDW.timeout == 0U)
        {
          /* Transition: '<S5>:585' */
          /* '<S5>:585:1' timeout = AngleTimeout; */
          rtDW.timeout = *AngleTimeout;
          guard2 = true;
        }
        else
        {
          /* '<S5>:721:1' sf_internal_predicateOutput = ... */
          /* '<S5>:721:1' errorCourseLimit; */
          fi1 = errorCourseLimit();
          if (fi1 != 0.0)
          {
            /* Transition: '<S5>:721' */
            /* '<S5>:721:1' timeout = AngleTimeout/2; */
            rtDW.timeout = (uint32_T)rt_roundd_snf((real_T)*AngleTimeout / 2.0);
            guard2 = true;
          }
          else
          {
            /* '<S5>:590:4' desiredCourse = Angle2Course(desiredCourse+AngleManevr); */
            rtDW.desiredCourse = Angle2Course(rtDW.desiredCourse + *AngleManevr);
          }
        }
        break;

       case IN_Wait:
        /* During 'Wait': '<S5>:591' */
        /* '<S5>:587:1' sf_internal_predicateOutput = ... */
        /* '<S5>:587:1' (timeDifference) > 1  && timeout == 0; */
        if ((fi2 > 1.0) && (rtDW.timeout == 0U))
        {
          /* Transition: '<S5>:587' */
          /* '<S5>:586:1' sf_internal_predicateOutput = ... */
          /* '<S5>:586:1' side; */
          if (rtDW.side)
          {
            /* Transition: '<S5>:586' */
            if (rtDW.is_StepAngle == IN_Wait)
            {
              /* Inport: '<Root>/trackingCourse' */
              /* Exit 'Wait': '<S5>:591' */
              /* '<S5>:591:4' centralCourse = course; */
              rtDW.centralCourse = rtU.ActualCourse;
              rtDW.is_StepAngle = IN_NO_ACTIVE_CHILD;
            }

            if (rtDW.is_StepAngle != IN_Right)
            {
              rtDW.is_StepAngle = IN_Right;

              /* Entry 'Right': '<S5>:590' */
              /* '<S5>:590:1' timeout = uint32((timeDifference)*coeff) */
              fi1 = rt_roundd_snf(fi2 * fi1);
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

              /* '<S5>:590:3' waiting = false; */
              /* '<S5>:590:3' justReturnedHere = false; */
              rtDW.justReturnedHere = false;

              /* '<S5>:590:4' BimSupply.wantEnable; */
              /* Event: '<S5>:606' */
              b_previousEvent = *sfEvent;
              *sfEvent = event_wantEnable;
              if (rtDW.is_active_BimSupply != 0U)
              {
                BimSupply(BimTimeout, sfEvent);
              }

              *sfEvent = b_previousEvent;
            }
          }
          else
          {
            /* '<S5>:588:1' sf_internal_predicateOutput = ... */
            /* '<S5>:588:1' ~side; */
            if (!rtDW.side)
            {
              /* Transition: '<S5>:588' */
              if (rtDW.is_StepAngle == IN_Wait)
              {
                /* Inport: '<Root>/trackingCourse' */
                /* Exit 'Wait': '<S5>:591' */
                /* '<S5>:591:4' centralCourse = course; */
                rtDW.centralCourse = rtU.ActualCourse;
                rtDW.is_StepAngle = IN_NO_ACTIVE_CHILD;
              }

              if (rtDW.is_StepAngle != IN_Left)
              {
                rtDW.is_StepAngle = IN_Left;

                /* Entry 'Left': '<S5>:592' */
                /* '<S5>:592:1' timeout = uint32((timeDifference)*coeff) */
                fi1 = rt_roundd_snf(fi2 * fi1);
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

                /* '<S5>:592:3' waiting = false; */
                /* '<S5>:592:3' justReturnedHere = false; */
                rtDW.justReturnedHere = false;

                /* '<S5>:592:4' BimSupply.wantEnable; */
                /* Event: '<S5>:606' */
                b_previousEvent = *sfEvent;
                *sfEvent = event_wantEnable;
                if (rtDW.is_active_BimSupply != 0U)
                {
                  BimSupply(BimTimeout, sfEvent);
                }

                *sfEvent = b_previousEvent;
              }
            }
            else
            {
              guard3 = true;
            }
          }
        }
        else
        {
          guard3 = true;
        }
        break;
      }
    }
    break;

   case IN_Init:
    /* During 'Init': '<S5>:11' */
    /* Transition: '<S5>:25' */
    /* '<S5>:171:1' sf_internal_predicateOutput = ... */
    /* '<S5>:171:1' Mode == 0; */
    if (*Mode == 0)
    {
      /* Transition: '<S5>:171' */
      rtDW.is_Control = IN_TDP_Only;

      /* Entry 'TDP_Only': '<S5>:167' */
      /*  Режим прицеливания на TDP */
      /* '<S5>:167:1' targetPoint = touchdownPoint; */
      rtDW.targetPoint[0] = rtDW.TmpSignalConversionAtSFunctionI[0];
      rtDW.targetPoint[1] = rtDW.TmpSignalConversionAtSFunctionI[1];
      rtDW.targetPoint[2] = rtDW.TmpSignalConversionAtSFunctionI[2];

      /* '<S5>:167:2' courseToTargetPoint = Azimut(targetPoint, currentPoint); */
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
      fi2 = rtDW.targetPoint[0] * 3.1415926535897931 / 180.0;

      /* 'Azimut:15' delta_la = la2-la1; */
      delta_la_tmp = rtDW.targetPoint[1] * 3.1415926535897931 / 180.0 -
        rtDW.TmpSignalConversionAtSFunctio_g[1] * 3.1415926535897931 / 180.0;

      /* 'Azimut:16' y = atan2(sin(delta_la)*cos(fi2), cos(fi1)*sin(fi2)-sin(fi1)*cos(fi2)*cos(delta_la)); */
      rtDW.courseToTargetPoint = rt_atan2d_snf(sin(delta_la_tmp) * cos(fi2), cos
        (fi1) * sin(fi2) - sin(fi1) * cos(fi2) * cos(delta_la_tmp));

      /* 'Azimut:17' y = rem((y + 2*pi), 2*pi); */
      rtDW.courseToTargetPoint = rt_remd_snf(rtDW.courseToTargetPoint
        + 6.2831853071795862, 6.2831853071795862);

      /* courseToTargetPoint = Heading(targetPoint, currentPoint, 0); */
      /* '<S5>:167:4' desiredCourse = courseToTargetPoint; */
      rtDW.desiredCourse = rtDW.courseToTargetPoint;
    }
    else
    {
      /* '<S5>:172:1' sf_internal_predicateOutput = ... */
      /* '<S5>:172:1' (Mode == 1 || Mode == 2); */
      if ((*Mode == 1) || (*Mode == 2))
      {
        /* Transition: '<S5>:172' */
        rtDW.is_Control = IN_Flight;

        /* Entry 'Flight': '<S5>:23' */
        /*  Режим полета по точкам */
        /* '<S5>:23:1' courseToTargetPoint = Azimut(targetPoint + touchdownPoint_Offset, currentPoint); */
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
        fi2 = (rtDW.targetPoint[0] + rtDW.touchdownPoint_Offset[0]) *
          3.1415926535897931 / 180.0;

        /* 'Azimut:15' delta_la = la2-la1; */
        delta_la_tmp = (rtDW.targetPoint[1] + rtDW.touchdownPoint_Offset[1]) *
          3.1415926535897931 / 180.0 - rtDW.TmpSignalConversionAtSFunctio_g[1] *
          3.1415926535897931 / 180.0;

        /* 'Azimut:16' y = atan2(sin(delta_la)*cos(fi2), cos(fi1)*sin(fi2)-sin(fi1)*cos(fi2)*cos(delta_la)); */
        rtDW.courseToTargetPoint = rt_atan2d_snf(sin(delta_la_tmp) * cos(fi2),
          cos(fi1) * sin(fi2) - sin(fi1) * cos(fi2) * cos(delta_la_tmp));

        /* 'Azimut:17' y = rem((y + 2*pi), 2*pi); */
        rtDW.courseToTargetPoint = rt_remd_snf(rtDW.courseToTargetPoint
          + 6.2831853071795862, 6.2831853071795862);

        /* courseToTargetPoint = Heading(targetPoint + touchdownPoint_Offset, currentPoint, 0); */
        /* '<S5>:23:3' desiredCourse = courseToTargetPoint; */
        rtDW.desiredCourse = rtDW.courseToTargetPoint;

        /* Entry Internal 'Flight': '<S5>:23' */
        if (rtDW.was_Flight == IN_StepAngle)
        {
          if (rtDW.is_Flight != IN_StepAngle)
          {
            rtDW.is_Flight = IN_StepAngle;
            rtDW.was_Flight = IN_StepAngle;

            /* Entry 'StepAngle': '<S5>:582' */
            rtDW.side = false;

            /* '<S5>:582:1' coeff = nonlinearCoefficientFunction; */
            /* '<S5>:582:1' timeDifference = timeVertical - timeHorizontal; */
          }

          /* Entry Internal 'StepAngle': '<S5>:582' */
          /* Transition: '<S5>:634' */
          /* '<S5>:634:1' timeout = 15; */
          rtDW.timeout = 15U;
          if (rtDW.is_StepAngle != IN_Wait)
          {
            rtDW.is_StepAngle = IN_Wait;

            /* Entry 'Wait': '<S5>:591' */
            /* '<S5>:591:1' BimSupply.wantEnable */
            /* Event: '<S5>:606' */
            b_previousEvent = *sfEvent;
            *sfEvent = event_wantEnable;
            if (rtDW.is_active_BimSupply != 0U)
            {
              BimSupply(BimTimeout, sfEvent);
            }

            *sfEvent = b_previousEvent;

            /* '<S5>:591:1' waiting = true; */
            /* '<S5>:591:3' justReturnedHere = true; */
            rtDW.justReturnedHere = true;
          }
        }
        else
        {
          /* Transition: '<S5>:268' */
          /* '<S5>:83:1' sf_internal_predicateOutput = ... */
          /* '<S5>:83:1' Mode == 2; */
          if (*Mode == 2)
          {
            /* Transition: '<S5>:83' */
            if (rtDW.is_Flight != IN_StepAngle)
            {
              rtDW.is_Flight = IN_StepAngle;
              rtDW.was_Flight = IN_StepAngle;

              /* Entry 'StepAngle': '<S5>:582' */
              rtDW.side = false;

              /* '<S5>:582:1' coeff = nonlinearCoefficientFunction; */
              /* '<S5>:582:1' timeDifference = timeVertical - timeHorizontal; */
            }

            /* Entry Internal 'StepAngle': '<S5>:582' */
            /* Transition: '<S5>:634' */
            /* '<S5>:634:1' timeout = 15; */
            rtDW.timeout = 15U;
            if (rtDW.is_StepAngle != IN_Wait)
            {
              rtDW.is_StepAngle = IN_Wait;

              /* Entry 'Wait': '<S5>:591' */
              /* '<S5>:591:1' BimSupply.wantEnable */
              /* Event: '<S5>:606' */
              b_previousEvent = *sfEvent;
              *sfEvent = event_wantEnable;
              if (rtDW.is_active_BimSupply != 0U)
              {
                BimSupply(BimTimeout, sfEvent);
              }

              *sfEvent = b_previousEvent;

              /* '<S5>:591:1' waiting = true; */
              /* '<S5>:591:3' justReturnedHere = true; */
              rtDW.justReturnedHere = true;
            }
          }
        }
      }
    }
    break;

   case IN_TDP_Only:
    /* During 'TDP_Only': '<S5>:167' */
    /* '<S5>:167:1' targetPoint = touchdownPoint; */
    rtDW.targetPoint[0] = rtDW.TmpSignalConversionAtSFunctionI[0];
    rtDW.targetPoint[1] = rtDW.TmpSignalConversionAtSFunctionI[1];
    rtDW.targetPoint[2] = rtDW.TmpSignalConversionAtSFunctionI[2];

    /* '<S5>:167:2' courseToTargetPoint = Azimut(targetPoint, currentPoint); */
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
    fi2 = rtDW.targetPoint[0] * 3.1415926535897931 / 180.0;

    /* 'Azimut:15' delta_la = la2-la1; */
    delta_la_tmp = rtDW.targetPoint[1] * 3.1415926535897931 / 180.0 -
      rtDW.TmpSignalConversionAtSFunctio_g[1] * 3.1415926535897931 / 180.0;

    /* 'Azimut:16' y = atan2(sin(delta_la)*cos(fi2), cos(fi1)*sin(fi2)-sin(fi1)*cos(fi2)*cos(delta_la)); */
    rtDW.courseToTargetPoint = rt_atan2d_snf(sin(delta_la_tmp) * cos(fi2), cos
      (fi1) * sin(fi2) - sin(fi1) * cos(fi2) * cos(delta_la_tmp));

    /* 'Azimut:17' y = rem((y + 2*pi), 2*pi); */
    rtDW.courseToTargetPoint = rt_remd_snf(rtDW.courseToTargetPoint
      + 6.2831853071795862, 6.2831853071795862);

    /* courseToTargetPoint = Heading(targetPoint, currentPoint, 0); */
    /* '<S5>:167:4' desiredCourse = courseToTargetPoint; */
    rtDW.desiredCourse = rtDW.courseToTargetPoint;
    break;
  }

  if (guard3)
  {
    /* Outport: '<Root>/horizontalDistance' incorporates:
     *  Inport: '<Root>/trackingCourse'
     */
    /* '<S5>:685:1' sf_internal_predicateOutput = ... */
    /* '<S5>:685:1' distanceToTarget < 500; */
    if (rtY.horizontalDistance < 500.0)
    {
      /* Transition: '<S5>:685' */
      if (rtDW.is_StepAngle == IN_Wait)
      {
        /* Exit 'Wait': '<S5>:591' */
        /* '<S5>:591:4' centralCourse = course; */
        rtDW.centralCourse = rtU.ActualCourse;
      }

      rtDW.is_StepAngle = IN_Exit;
    }

    /* End of Outport: '<Root>/horizontalDistance' */
  }

  if (guard2)
  {
    /* Transition: '<S5>:629' */
    if (rtDW.is_StepAngle == IN_Right)
    {
      /* Exit 'Right': '<S5>:590' */
      /* '<S5>:590:7' if wherePoint(initialPoint, touchdownPoint, currentPoint) == -1 */
      if (wherePoint(rtDW.initialPoint, rtDW.TmpSignalConversionAtSFunctionI,
                     rtDW.TmpSignalConversionAtSFunctio_g) == -1.0)
      {
        /* '<S5>:590:7' side = false; */
        rtDW.side = false;
      }
      else
      {
        /* '<S5>:590:7' else */
        /* '<S5>:590:7' side = true; */
        rtDW.side = true;
      }

      rtDW.is_StepAngle = IN_NO_ACTIVE_CHILD;
    }

    if (rtDW.is_StepAngle != IN_Wait)
    {
      rtDW.is_StepAngle = IN_Wait;

      /* Entry 'Wait': '<S5>:591' */
      /* '<S5>:591:1' BimSupply.wantEnable */
      /* Event: '<S5>:606' */
      b_previousEvent = *sfEvent;
      *sfEvent = event_wantEnable;
      if (rtDW.is_active_BimSupply != 0U)
      {
        BimSupply(BimTimeout, sfEvent);
      }

      *sfEvent = b_previousEvent;

      /* '<S5>:591:1' waiting = true; */
      /* '<S5>:591:3' justReturnedHere = true; */
      rtDW.justReturnedHere = true;
    }
  }

  if (guard1)
  {
    /* Transition: '<S5>:626' */
    if (rtDW.is_StepAngle == IN_Left)
    {
      /* Exit 'Left': '<S5>:592' */
      /* '<S5>:592:8' side = true; */
      rtDW.side = true;

      /* '<S5>:592:8' if wherePoint(initialPoint, touchdownPoint, currentPoint) == 1 */
      if (wherePoint(rtDW.initialPoint, rtDW.TmpSignalConversionAtSFunctionI,
                     rtDW.TmpSignalConversionAtSFunctio_g) == 1.0)
      {
        /* '<S5>:592:8' side = true; */
        rtDW.side = true;
      }
      else
      {
        /* '<S5>:592:8' else */
        /* '<S5>:592:8' side = false; */
        rtDW.side = false;
      }

      rtDW.is_StepAngle = IN_NO_ACTIVE_CHILD;
    }

    if (rtDW.is_StepAngle != IN_Wait)
    {
      rtDW.is_StepAngle = IN_Wait;

      /* Entry 'Wait': '<S5>:591' */
      /* '<S5>:591:1' BimSupply.wantEnable */
      /* Event: '<S5>:606' */
      b_previousEvent = *sfEvent;
      *sfEvent = event_wantEnable;
      if (rtDW.is_active_BimSupply != 0U)
      {
        BimSupply(BimTimeout, sfEvent);
      }

      *sfEvent = b_previousEvent;

      /* '<S5>:591:1' waiting = true; */
      /* '<S5>:591:3' justReturnedHere = true; */
      rtDW.justReturnedHere = true;
    }
  }
}

/*
 * Function for Chart: '<S1>/LogicController'
 * function status = checkFinalMane
 *     %% Вычислим расстояние с помощью функции DistanceCalculator()
 */
static real_T checkFinalMane(const real_T *V_hor, const real_T *V_vert)
{
  real_T status;
  real_T fi1;
  real_T fi2;

  /* MATLAB Function 'checkFinalMane': '<S5>:141' */
  /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
  /*  CheckFinalMane - Проверка необходимости перехода к финальному манёвру */
  /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
  /* '<S5>:141:6' distanceToTarget = DistanceCalculator(currentPoint, touchdownPoint+touchdownPoint_Offset); */
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
  fi2 = (rtDW.TmpSignalConversionAtSFunctionI[0] + rtDW.touchdownPoint_Offset[0])
    * 3.1415926535897931 / 180.0;

  /* Outport: '<Root>/horizontalDistance' */
  /* 'DistanceCalculator:17' d = sin(fi1)*sin(fi2) + cos(fi1)*cos(-fi2)*cos(la2-la1); */
  /*  6371 км средний радиус земли */
  /* 'DistanceCalculator:19' y = acos(d)*6378245; */
  rtY.horizontalDistance = acos(cos((rtDW.TmpSignalConversionAtSFunctionI[1] +
    rtDW.touchdownPoint_Offset[1]) * 3.1415926535897931 / 180.0 -
    rtDW.TmpSignalConversionAtSFunctio_g[1] * 3.1415926535897931 / 180.0) * (cos
    (fi1) * cos(-fi2)) + sin(fi1) * sin(fi2)) * 6.378245E+6;

  /* Outport: '<Root>/onTargetTime' incorporates:
   *  Outport: '<Root>/horizontalDistance'
   */
  /*     %% Вычисляем время необходимое для преодоления расстояния до TDP и до посадки */
  /* '<S5>:141:8' timeHorizontal = distanceToTarget/velocityHorizontal; */
  rtY.onTargetTime = rtY.horizontalDistance / *V_hor;

  /* Outport: '<Root>/touchdownTime' incorporates:
   *  Inport: '<Root>/reliefOnTdp'
   */
  /* '<S5>:141:9' timeVertical = (currentPoint(3) - (touchdownPoint(3) + double(reliefOnTDP)))/velocityVertical; */
  rtY.touchdownTime = (rtDW.TmpSignalConversionAtSFunctio_g[2] -
                       (rtDW.TmpSignalConversionAtSFunctionI[2] + (real_T)
                        rtU.ReliefOnTDP)) / *V_vert;

  /* Outport: '<Root>/onTargetTime' incorporates:
   *  Outport: '<Root>/touchdownTime'
   */
  /*     %% Определяем поправку по времени необходимому на разворот */
  /*  Угол разворота */
  /* '<S5>:141:12' angle = ControlDemode(courseToTargetPoint - course); */
  /* '<S5>:141:13' angle = abs(angle/pi*180); */
  /*  Вычисляем поправку как линейную функцию:  */
  /* T_hor_popr = angle / TurnSpeed; */
  /*  Вычисляем поправку по параболе */
  /* '<S5>:141:19' timeHorizontalCorrection = 0.0005*angle^2 + 0.06*angle; */
  /*  Нулевая поправка */
  /* '<S5>:141:22' timeHorizontalCorrection = 0; */
  /* '<S5>:141:24' timeHorizontal = timeHorizontal + timeHorizontalCorrection; */
  /*     %% Возвращаем сигнал о необходимости перейти на финальный маневр */
  /* '<S5>:141:26' if timeHorizontal > timeVertical */
  if (rtY.onTargetTime > rtY.touchdownTime)
  {
    /* '<S5>:141:27' status = 1; */
    status = 1.0;
  }
  else
  {
    /* '<S5>:141:28' else */
    /* '<S5>:141:29' status = 0; */
    status = 0.0;
  }

  return status;
}

/* Model step function */
void flightController_step(void)
{
  int32_T b_previousEvent;
  real_T fi2;
  real_T h;
  real_T Yh;
  real_T un_pow2;
  boolean_T rtb_Disabled;
  boolean_T rtb_Enabled;
  real_T AngleManevr;
  uint32_T AngleTimeout;
  uint32_T BimTimeout;
  real_T V_hor;
  uint8_T Mode;
  int32_T sfEvent;
  real_T un_idx_0;
  real_T un_idx_1;
  uint32_T qY;
  int16_T tmp;
  real_T h_tmp;
  boolean_T guard1 = false;

  /* Outputs for Atomic SubSystem: '<Root>/flightController' */
  /* DataStoreWrite: '<S1>/Data Store Write1' */
  AngleManevr = 1.5707963267948966;

  /* DataStoreWrite: '<S1>/Data Store Write9' incorporates:
   *  Constant: '<S1>/AngleTimeout'
   */
  AngleTimeout = 60U;

  /* DataStoreWrite: '<S1>/Data Store Write5' incorporates:
   *  Constant: '<S1>/BimTimeout'
   */
  BimTimeout = 30U;

  /* DataStoreWrite: '<S1>/Data Store Write3' incorporates:
   *  Constant: '<S1>/Mode'
   */
  Mode = 2U;

  /* Outputs for Atomic SubSystem: '<S18>/ProjectionSpeed' */
  /* MATLAB Function: '<S26>/Speed' incorporates:
   *  Inport: '<Root>/velocityAltitude'
   *  Inport: '<Root>/velocityLatitude'
   *  Inport: '<Root>/velocityLongitude'
   */
  /* MATLAB Function 'ProjectionSpeed/Speed': '<S28>:1' */
  /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
  /*  */
  /*    SpeedProjections - Пересчет проекций векторов скорости по lat, lon, alt */
  /*    в горизонтальную и вертикальную скорость полёта */
  /*  */
  /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
  /* '<S28>:1:8' V_hor = sqrt(Vel_lat^2 + Vel_lon^2); */
  /* '<S28>:1:9' V_vert = abs(Vel_alt); */
  un_idx_1 = fabs(rtU.AltitudeVelocity);
  V_hor = sqrt(rtU.LatitudeVelocity * rtU.LatitudeVelocity +
               rtU.LongitudeVelocity * rtU.LongitudeVelocity);

  /* End of Outputs for SubSystem: '<S18>/ProjectionSpeed' */

  /* SignalConversion: '<S5>/TmpSignal ConversionAt SFunction Inport1' incorporates:
   *  Chart: '<S1>/LogicController'
   *  Inport: '<Root>/TDP_alt'
   *  Inport: '<Root>/TDP_lat'
   *  Inport: '<Root>/TDP_lon'
   */
  rtDW.TmpSignalConversionAtSFunctionI[0] = rtU.TDP_lat;
  rtDW.TmpSignalConversionAtSFunctionI[1] = rtU.TDP_lon;
  rtDW.TmpSignalConversionAtSFunctionI[2] = rtU.TDP_alt;

  /* SignalConversion: '<S5>/TmpSignal ConversionAt SFunction Inport2' incorporates:
   *  Chart: '<S1>/LogicController'
   *  Inport: '<Root>/Pos_lat'
   *  Inport: '<Root>/Pos_lon'
   */
  rtDW.TmpSignalConversionAtSFunctio_g[0] = rtU.Pos_lat;
  rtDW.TmpSignalConversionAtSFunctio_g[1] = rtU.Pos_lon;

  /* Switch: '<S8>/Switch' incorporates:
   *  Inport: '<Root>/barometricAvailable'
   */
  if (rtU.barometricAvailable > 0)
  {
    /* SignalConversion: '<S5>/TmpSignal ConversionAt SFunction Inport2' incorporates:
     *  Chart: '<S1>/LogicController'
     *  Constant: '<S1>/weightCoefficient'
     *  Constant: '<S8>/constant'
     *  DataStoreWrite: '<S1>/Data Store Write10'
     *  Inport: '<Root>/Pos_alt'
     *  Inport: '<Root>/barometricAltitude'
     *  Product: '<S8>/Product'
     *  Sum: '<S8>/Add'
     *  Sum: '<S8>/Sum'
     */
    rtDW.TmpSignalConversionAtSFunctio_g[2] = 0.0 * rtU.barometricAltitude +
      rtU.Pos_alt;
  }
  else
  {
    /* SignalConversion: '<S5>/TmpSignal ConversionAt SFunction Inport2' incorporates:
     *  Chart: '<S1>/LogicController'
     *  Inport: '<Root>/Pos_alt'
     */
    rtDW.TmpSignalConversionAtSFunctio_g[2] = rtU.Pos_alt;
  }

  /* End of Switch: '<S8>/Switch' */

  /* Chart: '<S1>/LogicController' incorporates:
   *  Constant: '<S1>/ArrivalRadius'
   *  DataStoreWrite: '<S1>/Data Store Write2'
   *  Delay: '<S9>/DsblTg'
   *  Inport: '<Root>/relief'
   */
  /* Gateway: flightController/LogicController */
  sfEvent = -1;

  /* During: flightController/LogicController */
  if (rtDW.is_active_c1_flightController == 0U)
  {
    /* Entry: flightController/LogicController */
    rtDW.is_active_c1_flightController = 1U;

    /* Entry Internal: flightController/LogicController */
    if (rtDW.is_active_Timer != 1U)
    {
      rtDW.is_active_Timer = 1U;

      /* Entry 'Timer': '<S5>:207' */
      /* '<S5>:207:1' time = 0; */
      rtDW.time = 0U;

      /* '<S5>:207:1' BimSupply.timeout = 0; */
      rtDW.timeout_b = 0U;

      /* '<S5>:207:3' Control.Flight.StepAngle.timeout = 0; */
      rtDW.timeout = 0U;
    }

    if (rtDW.is_active_Control != 1U)
    {
      rtDW.is_active_Control = 1U;

      /* Entry 'Control': '<S5>:9' */
      /*  Основной цикл задач */
    }

    /* Entry Internal 'Control': '<S5>:9' */
    /* Transition: '<S5>:12' */
    if (rtDW.is_Control != IN_Init)
    {
      rtDW.is_Control = IN_Init;

      /* Entry 'Init': '<S5>:11' */
      /*  Производим планирование путевых точек для сброса высоты */
      /* '<S5>:11:3' targetPoint = touchdownPoint; */
      /* [Flight.Angle.Point1, Flight.Angle.Point2] = AngleManCreator(currentPoint, touchdownPoint, AngleManevr); */
      /* [Flight.Box.LUPoint, Flight.Box.LDPoint, Flight.Box.RUPoint, Flight.Box.RDPoint] = BOXCreator(touchdownPoint, BoxSize); */
      /* '<S5>:11:6' Control.Flight.StepAngle.justReturnedHere = false; */
      rtDW.justReturnedHere = false;

      /* '<S5>:11:7' Control.Flight.StepAngle.waiting = true; */
      /* '<S5>:11:8' initialPoint = currentPoint; */
      /* '<S5>:11:9' touchdownPoint_Offset = [0;0;0]; */
      rtDW.targetPoint[0] = rtDW.TmpSignalConversionAtSFunctionI[0];
      rtDW.initialPoint[0] = rtDW.TmpSignalConversionAtSFunctio_g[0];
      rtDW.touchdownPoint_Offset[0] = 0.0;
      rtDW.targetPoint[1] = rtDW.TmpSignalConversionAtSFunctionI[1];
      rtDW.initialPoint[1] = rtDW.TmpSignalConversionAtSFunctio_g[1];
      rtDW.touchdownPoint_Offset[1] = 0.0;
      rtDW.targetPoint[2] = rtDW.TmpSignalConversionAtSFunctionI[2];
      rtDW.initialPoint[2] = rtDW.TmpSignalConversionAtSFunctio_g[2];
      rtDW.touchdownPoint_Offset[2] = 0.0;
    }

    if (rtDW.is_active_Touchdown != 1U)
    {
      rtDW.is_active_Touchdown = 1U;

      /* Entry 'Touchdown': '<S5>:34' */
      /* Здесь мы отдельно будем контролировать высоту, */
      /*  для своевременного открытия посадотчных парашютов */
    }

    /* Entry Internal 'Touchdown': '<S5>:34' */
    /* Transition: '<S5>:37' */
    if (rtDW.is_Touchdown != IN_Off)
    {
      rtDW.is_Touchdown = IN_Off;

      /* Outport: '<Root>/touchdown' */
      /* Entry 'Off': '<S5>:38' */
      /* '<S5>:38:1' touchdownEnable = 0; */
      rtY.touchdown = 0U;
    }

    if (rtDW.is_active_EventGenerator != 1U)
    {
      rtDW.is_active_EventGenerator = 1U;

      /* Outport: '<Root>/touchdownTime' */
      /* Entry 'EventGenerator': '<S5>:260' */
      /* '<S5>:260:1' timeVertical = 0; */
      rtY.touchdownTime = 0.0;

      /* Outport: '<Root>/onTargetTime' */
      /* '<S5>:260:1' timeHorizontal = 0; */
      rtY.onTargetTime = 0.0;
    }

    if (rtDW.is_active_BimSupply != 1U)
    {
      rtDW.is_active_BimSupply = 1U;

      /* Entry 'BimSupply': '<S5>:452' */
    }

    /* Entry Internal 'BimSupply': '<S5>:452' */
    /* Transition: '<S5>:457' */
    if (rtDW.is_BimSupply != IN_Init_n)
    {
      rtDW.is_BimSupply = IN_Init_n;

      /* Entry 'Init': '<S5>:603' */
      /* '<S5>:603:1' enableTargetting = 0; */
      rtDW.enableTargetting = 0U;
    }
  }
  else
  {
    if (rtDW.is_active_Timer != 0U)
    {
      /* During 'Timer': '<S5>:207' */
      /* '<S5>:207:4' time = time + 1; */
      qY = rtDW.time + /*MW:OvSatOk*/ 1U;
      if (qY < rtDW.time)
      {
        qY = MAX_uint32_T;
      }

      rtDW.time = qY;

      /* '<S5>:207:4' BimSupply.timeout = TimeoutDecrement(BimSupply.timeout); */
      rtDW.timeout_b = TimeoutDecrement(rtDW.timeout_b);

      /* '<S5>:207:5' Control.Flight.StepAngle.timeout = TimeoutDecrement(Control.Flight.StepAngle.timeout); */
      rtDW.timeout = TimeoutDecrement(rtDW.timeout);
    }

    if (rtDW.is_active_Control != 0U)
    {
      Control(&AngleManevr, &AngleTimeout, &BimTimeout, &Mode, &sfEvent);
    }

    if (rtDW.is_active_Touchdown != 0U)
    {
      /* During 'Touchdown': '<S5>:34' */
      /* Здесь мы отдельно будем контролировать высоту, */
      /*  для своевременного открытия посадотчных парашютов */
      guard1 = false;
      switch (rtDW.is_Touchdown)
      {
       case IN_Off:
        /* During 'Off': '<S5>:38' */
        /* Transition: '<S5>:119' */
        /* '<S5>:43:1' sf_internal_predicateOutput = ... */
        /* '<S5>:43:1' reliefAvailable == 0; */
        if ((rtU.ReliefAvailable == 0) && (rtDW.TmpSignalConversionAtSFunctio_g
             [2] <= rtDW.TmpSignalConversionAtSFunctionI[2]))
        {
          /* Transition: '<S5>:43' */
          /* '<S5>:128:1' sf_internal_predicateOutput = ... */
          /* '<S5>:128:1' currentPoint(3) <= touchdownPoint(3); */
          /* Transition: '<S5>:128' */
          guard1 = true;
        }
        else
        {
          /* '<S5>:125:1' sf_internal_predicateOutput = ... */
          /* '<S5>:125:1' reliefAvailable ==1; */
          if (rtU.ReliefAvailable == 1)
          {
            /* Transition: '<S5>:125' */
            /* '<S5>:129:1' sf_internal_predicateOutput = ... */
            /* '<S5>:129:1' (currentPoint(3)-relief) <= touchdownPoint(3); */
            un_pow2 = rt_roundd_snf(rtDW.TmpSignalConversionAtSFunctio_g[2] -
              (real_T)rtU.Relief);
            if (un_pow2 < 32768.0)
            {
              if (un_pow2 >= -32768.0)
              {
                tmp = (int16_T)un_pow2;
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
              /* Transition: '<S5>:129' */
              guard1 = true;
            }
            else
            {
              /* Outport: '<Root>/touchdown' */
              /* '<S5>:38:1' touchdownEnable = 0; */
              rtY.touchdown = 0U;
            }
          }
          else
          {
            /* Outport: '<Root>/touchdown' */
            /* '<S5>:38:1' touchdownEnable = 0; */
            rtY.touchdown = 0U;
          }
        }
        break;

       case IN_On:
        /* Outport: '<Root>/touchdown' */
        /* During 'On': '<S5>:36' */
        /* '<S5>:36:1' touchdownEnable = 1; */
        rtY.touchdown = 1U;
        break;
      }

      if (guard1)
      {
        rtDW.is_Touchdown = IN_On;

        /* Outport: '<Root>/touchdown' */
        /* Entry 'On': '<S5>:36' */
        /* '<S5>:36:1' touchdownEnable = 1; */
        rtY.touchdown = 1U;
      }
    }

    if (rtDW.is_active_EventGenerator != 0U)
    {
      /* During 'EventGenerator': '<S5>:260' */
      /*  Контроль выхода на точку */
      /* '<S5>:260:4' if checkFinalMane  == 1 */
      if (checkFinalMane(&V_hor, &un_idx_1) == 1.0)
      {
        /* '<S5>:260:5' Control.final; */
        /* Event: '<S5>:361' */
        b_previousEvent = sfEvent;
        sfEvent = event_final;
        if (rtDW.is_active_Control != 0U)
        {
          Control(&AngleManevr, &AngleTimeout, &BimTimeout, &Mode, &sfEvent);
        }

        sfEvent = b_previousEvent;
      }
      else
      {
        /* '<S5>:260:7' else */
        /* '<S5>:260:7' Control.cancelFinal; */
        /* Event: '<S5>:391' */
        b_previousEvent = sfEvent;
        sfEvent = event_cancelFinal;
        if (rtDW.is_active_Control != 0U)
        {
          Control(&AngleManevr, &AngleTimeout, &BimTimeout, &Mode, &sfEvent);
        }

        sfEvent = b_previousEvent;
      }

      /*  Контроль попадания в зону текущей цели */
      /* '<S5>:260:10' if CheckArrival(currentPoint, targetPoint, ArrivalRadius) == 1 */
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
      un_idx_1 = rtDW.TmpSignalConversionAtSFunctio_g[0] * 3.1415926535897931 /
        180.0;

      /* 'DistanceCalculator:15' la2 = Target(2)*pi/180; */
      /* 'DistanceCalculator:16' fi2 = Target(1)*pi/180; */
      fi2 = rtDW.targetPoint[0] * 3.1415926535897931 / 180.0;

      /* 'DistanceCalculator:17' d = sin(fi1)*sin(fi2) + cos(fi1)*cos(-fi2)*cos(la2-la1); */
      /*  6371 км средний радиус земли */
      /* 'DistanceCalculator:19' y = acos(d)*6378245; */
      /*     %% Попаданием будем считать радиус  */
      /* 'CheckArrival:14' if distance < Radius */
      un_pow2 = rtDW.TmpSignalConversionAtSFunctio_g[1] * 3.1415926535897931 /
        180.0;
      if (acos(cos(rtDW.targetPoint[1] * 3.1415926535897931 / 180.0 - un_pow2) *
               (cos(un_idx_1) * cos(-fi2)) + sin(un_idx_1) * sin(fi2)) *
          6.378245E+6 < 300.0)
      {
        /* 'CheckArrival:15' status = 1; */
        /* '<S5>:260:11' Control.nextStage; */
        /* Event: '<S5>:362' */
        sfEvent = event_nextStage;
        if (rtDW.is_active_Control != 0U)
        {
          Control(&AngleManevr, &AngleTimeout, &BimTimeout, &Mode, &sfEvent);
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
      /* '<S5>:260:15' if CheckArrival(currentPoint, touchdownPoint, 2000) == 1 */
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
                   180.0 - un_pow2) * (cos(un_idx_1) * cos(-AngleManevr)) + sin
               (un_idx_1) * sin(AngleManevr)) * 6.378245E+6 < 2000.0)
      {
        /* 'CheckArrival:15' status = 1; */
        /* '<S5>:260:16' BimSupply.nonBlockingMode; */
        /* Event: '<S5>:486' */
        sfEvent = event_nonBlockingMode;
        if (rtDW.is_active_BimSupply != 0U)
        {
          BimSupply(&BimTimeout, &sfEvent);
        }

        sfEvent = b_previousEvent;

        /* '<S5>:260:17' BimTimeout = 10; */
        BimTimeout = 10U;
      }
      else
      {
        /* 'CheckArrival:16' else */
        /* 'CheckArrival:17' status = 0; */
      }

      /*  Контроль попадания в зону точки приземления */
      /*  Где управление уже запрещено */
      /* '<S5>:260:21' if CheckArrival(currentPoint, touchdownPoint, 300) == 1 */
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
                   3.1415926535897931 / 180.0) * (cos(un_idx_1) * cos
            (-AngleManevr)) + sin(un_idx_1) * sin(AngleManevr)) * 6.378245E+6 <
          300.0)
      {
        /* 'CheckArrival:15' status = 1; */
        /* '<S5>:260:22' BimSupply.parking; */
        /* Event: '<S5>:467' */
        sfEvent = event_parking;
        if (rtDW.is_active_BimSupply != 0U)
        {
          BimSupply(&BimTimeout, &sfEvent);
        }

        sfEvent = b_previousEvent;
      }
      else
      {
        /* 'CheckArrival:16' else */
        /* 'CheckArrival:17' status = 0; */
      }

      /* '<S5>:260:24' if fallingBim  == 1 */
      if (rtDW.DsblTg_DSTATE)
      {
        /* '<S5>:260:25' if Control.Flight.StepAngle.justReturnedHere */
        if (rtDW.justReturnedHere)
        {
          /* '<S5>:260:26' Control.Flight.StepAngle.justReturnedHere = false; */
          rtDW.justReturnedHere = false;
        }
        else
        {
          /* '<S5>:260:28' else */
          /* '<S5>:260:28' BimSupply.wantDisable */
          /* Event: '<S5>:607' */
          sfEvent = event_wantDisable;
          if (rtDW.is_active_BimSupply != 0U)
          {
            BimSupply(&BimTimeout, &sfEvent);
          }

          sfEvent = b_previousEvent;
        }
      }
    }

    if (rtDW.is_active_BimSupply != 0U)
    {
      BimSupply(&BimTimeout, &sfEvent);
    }
  }

  /* Sum: '<S7>/Sum1' incorporates:
   *  Inport: '<Root>/trackingCourse'
   */
  AngleManevr = rtDW.desiredCourse - rtU.ActualCourse;

  /* Outputs for Atomic SubSystem: '<S7>/ControlDemode' */
  /* MATLAB Function: '<S29>/ControlDemode' */
  /*  Здесь переводим угол из диапазона [0 360] */
  /*  в диапазон [-180 180], что позволит нам определять */
  /*  оптимальное направление разворота (лево и право) */
  /* MATLAB Function 'ControlDemode/ControlDemode': '<S37>:1' */
  /* '<S37>:1:5' y = u; */
  /*  Если больше 180 */
  /* '<S37>:1:7' if y > pi */
  if (AngleManevr > 3.1415926535897931)
  {
    /*  то это уже отрицательная полуокружность */
    /* '<S37>:1:9' y = y - 2*pi; */
    AngleManevr -= 6.2831853071795862;

    /*  Если меньше -180 */
  }
  else
  {
    if (AngleManevr < -3.1415926535897931)
    {
      /* '<S37>:1:11' elseif (y < (-pi)) */
      /*  то это уже положительная полуокружность */
      /* '<S37>:1:13' y = y + 2*pi; */
      AngleManevr += 6.2831853071795862;
    }
  }

  /* End of MATLAB Function: '<S29>/ControlDemode' */
  /* End of Outputs for SubSystem: '<S7>/ControlDemode' */

  /* Gain: '<S7>/Base_Gain' incorporates:
   *  Gain: '<S34>/Gain'
   */
  AngleManevr = 57.295779513082323 * AngleManevr * 2.0;

  /* Saturate: '<S7>/Base_Sat' */
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

  /* End of Saturate: '<S7>/Base_Sat' */

  /* Gain: '<S35>/Gain1' incorporates:
   *  Gain: '<S35>/Gain'
   *  Rounding: '<S35>/Rounding Function'
   */
  AngleManevr = floor(0.1 * AngleManevr) * 10.0;

  /* Outputs for Atomic SubSystem: '<S35>/DeadZone' */
  /* MATLAB Function: '<S38>/DeadZone' */
  /* MATLAB Function 'DeadZone/DeadZone': '<S39>:1' */
  /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
  /*  */
  /*    Звено нечувствительности */
  /*  */
  /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
  /* '<S39>:1:7' if abs(u) < 20 */
  if (fabs(AngleManevr) < 20.0)
  {
    /* '<S39>:1:8' y = 0; */
    AngleManevr = 0.0;
  }
  else
  {
    /* '<S39>:1:9' else */
    /* '<S39>:1:10' y = u; */
  }

  /* End of MATLAB Function: '<S38>/DeadZone' */
  /* End of Outputs for SubSystem: '<S35>/DeadZone' */

  /* Product: '<S7>/Product' */
  /*  if u > 10 */
  /*      y = 50; */
  /*  elseif u < -10 */
  /*      y = -50; */
  /*  else */
  /*      y = 0; */
  /*  end */
  AngleManevr *= (real_T)rtDW.enableTargetting;

  /* Outputs for Atomic SubSystem: '<S17>/Bearing' */
  /* MATLAB Function: '<S20>/Heading_true' incorporates:
   *  Delay: '<S17>/LastPos'
   *  Delay: '<S20>/PreviousBearing'
   *  Inport: '<Root>/Pos_lat'
   *  Inport: '<Root>/Pos_lon'
   */
  /*  u0 - точка отсчета */
  /*  u1 - конечная точка */
  /*  la - долгота */
  /*  fi - широта */
  /* MATLAB Function 'Azimuth/Azimut': '<S23>:1' */
  /* '<S23>:1:6' la1 = u0(2)*pi/180; */
  /* '<S23>:1:7' fi1 = u0(1)*pi/180; */
  /* '<S23>:1:8' la2 = u1(2)*pi/180; */
  /* '<S23>:1:9' fi2 = u1(1)*pi/180; */
  /* '<S23>:1:10' delta_la = la2-la1; */
  /* '<S23>:1:11' y = atan2(sin(delta_la)*cos(fi2), cos(fi1)*sin(fi2)-sin(fi1)*cos(fi2)*cos(delta_la)); */
  /* '<S23>:1:12' y = rem((y + 2*pi), 2*pi); */
  /*  Высчитаем, как изменились координаты */
  /* MATLAB Function 'Bearing/Heading_true': '<S24>:1' */
  /* '<S24>:1:3' un = u1 - u0; */
  un_idx_0 = rtU.Pos_lat - rtDW.LastPos_1_DSTATE;
  un_idx_1 = rtU.Pos_lon - rtDW.LastPos_2_DSTATE;

  /* '<S24>:1:4' d_lat = un(1); */
  /* '<S24>:1:5' d_lon = un(2); */
  /*  Условие для определения кратчайшего пути с учетом переходов между */
  /*  меридианами 0 и 180 по долготе */
  /* '<S24>:1:8' if d_lon > 180 */
  if (un_idx_1 > 180.0)
  {
    /* '<S24>:1:9' d_lon = d_lon - 360; */
    un_idx_1 -= 360.0;
  }
  else
  {
    if (un_idx_1 < -180.0)
    {
      /* '<S24>:1:10' elseif d_lon < (-180) */
      /* '<S24>:1:11' d_lon = d_lon +360; */
      un_idx_1 += 360.0;
    }
  }

  /*  Расчет курса */
  /* '<S24>:1:14' un_pow1 = d_lat*d_lat; */
  fi2 = un_idx_0 * un_idx_0;

  /* '<S24>:1:15' un_pow2 = d_lon*d_lon; */
  un_pow2 = un_idx_1 * un_idx_1;

  /* '<S24>:1:16' if (un_pow1 < 1e-16) && (un_pow2 < 1e-16) */
  if ((fi2 < 1.0E-16) && (un_pow2 < 1.0E-16))
  {
    /* '<S24>:1:17' y = last_y; */
    un_idx_0 = rtDW.PreviousBearing_DSTATE;
  }
  else
  {
    /* '<S24>:1:18' else */
    /* '<S24>:1:19' x = acos( d_lon / (sqrt(un_pow2 + un_pow1))); */
    /* '<S24>:1:20' y = acos( d_lat / (sqrt(un_pow2 + un_pow1))); */
    un_pow2 = sqrt(un_pow2 + fi2);
    un_idx_0 = acos(un_idx_0 / un_pow2);

    /* '<S24>:1:21' if x > pi/2 */
    if (acos(un_idx_1 / un_pow2) > 1.5707963267948966)
    {
      /* '<S24>:1:22' y = 2*pi - y; */
      un_idx_0 = 6.2831853071795862 - un_idx_0;
    }
  }

  /* End of MATLAB Function: '<S20>/Heading_true' */

  /* Update for Delay: '<S20>/PreviousBearing' */
  rtDW.PreviousBearing_DSTATE = un_idx_0;

  /* End of Outputs for SubSystem: '<S17>/Bearing' */

  /* Outputs for Atomic SubSystem: '<S9>/BimTriggers' */
  /* Chart: '<S40>/BimTrigger' */
  /* MATLAB Function 'GPSVelocity/Velocity': '<S27>:1' */
  /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
  /*  */
  /*    GPSPos2Speed - Определение скорости по сигналам GPS */
  /*    u1 - Точка относительно, которой считаем перемещение (lat, lon, alt) */
  /*    u0 - Точка до которой отладываем перемещение (lat, lon, alt) */
  /*    time - Время между замерами координат */
  /*  */
  /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
  /* '<S27>:1:10' un = u1-u0; */
  /* '<S27>:1:11' v_lat = un(1)*40000/360*1000/time; */
  /* '<S27>:1:12' v_lon = un(2)*(40000/360*1000/time * cos(pi/180*u0(1))); */
  /* '<S27>:1:13' v_alt = un(3); */
  /* Gateway: BimTriggers/BimTrigger */
  /* During: BimTriggers/BimTrigger */
  if (rtDW.is_active_c9_BupSimulinkLibrari == 0U)
  {
    /* Entry: BimTriggers/BimTrigger */
    rtDW.is_active_c9_BupSimulinkLibrari = 1U;

    /* Entry Internal: BimTriggers/BimTrigger */
    /* Entry 'Triggers': '<S44>:18' */
    /* Entry Internal 'Triggers': '<S44>:18' */
    /* Entry 'DisabledTrigger': '<S44>:19' */
    /* '<S44>:19:1' if LastLineState > 0 && Line <=0 */
    if ((rtDW.LastLineState > 0.0) && (AngleManevr <= 0.0))
    {
      /* '<S44>:19:1' Disabled = true; */
      rtb_Disabled = true;
    }
    else if ((rtDW.LastLineState < 0.0) && (AngleManevr >= 0.0))
    {
      /* '<S44>:19:2' elseif LastLineState < 0 && Line >=0 */
      /* '<S44>:19:4' Disabled = true; */
      rtb_Disabled = true;
    }
    else
    {
      /* '<S44>:19:4' else */
      /* '<S44>:19:4' Disabled = false; */
      rtb_Disabled = false;
    }

    /* Entry 'EnabledTrigger1': '<S44>:21' */
    /* '<S44>:21:1' if LastLineState == 0 && Line > 0 */
    if ((rtDW.LastLineState == 0.0) && (AngleManevr > 0.0))
    {
      /* '<S44>:21:1' Enabled = true; */
      rtb_Enabled = true;
    }
    else if ((rtDW.LastLineState == 0.0) && (AngleManevr < 0.0))
    {
      /* '<S44>:21:2' elseif LastLineState == 0 && Line < 0 */
      /* '<S44>:21:4' Enabled = true; */
      rtb_Enabled = true;
    }
    else
    {
      /* '<S44>:21:4' else */
      /* '<S44>:21:4' Enabled = false; */
      rtb_Enabled = false;
    }

    /* Entry 'RemeberLastState': '<S44>:29' */
    /* '<S44>:29:1' LastLineState = Line; */
    rtDW.LastLineState = AngleManevr;
  }
  else
  {
    /* During 'Triggers': '<S44>:18' */
    /* During 'DisabledTrigger': '<S44>:19' */
    /* '<S44>:19:1' if LastLineState > 0 && Line <=0 */
    if ((rtDW.LastLineState > 0.0) && (AngleManevr <= 0.0))
    {
      /* '<S44>:19:1' Disabled = true; */
      rtb_Disabled = true;
    }
    else if ((rtDW.LastLineState < 0.0) && (AngleManevr >= 0.0))
    {
      /* '<S44>:19:2' elseif LastLineState < 0 && Line >=0 */
      /* '<S44>:19:3' Disabled = true; */
      rtb_Disabled = true;
    }
    else
    {
      /* '<S44>:19:4' else */
      /* '<S44>:19:4' Disabled = false; */
      rtb_Disabled = false;
    }

    /* During 'EnabledTrigger1': '<S44>:21' */
    /* '<S44>:21:1' if LastLineState == 0 && Line > 0 */
    if ((rtDW.LastLineState == 0.0) && (AngleManevr > 0.0))
    {
      /* '<S44>:21:1' Enabled = true; */
      rtb_Enabled = true;
    }
    else if ((rtDW.LastLineState == 0.0) && (AngleManevr < 0.0))
    {
      /* '<S44>:21:2' elseif LastLineState == 0 && Line < 0 */
      /* '<S44>:21:3' Enabled = true; */
      rtb_Enabled = true;
    }
    else
    {
      /* '<S44>:21:4' else */
      /* '<S44>:21:4' Enabled = false; */
      rtb_Enabled = false;
    }

    /* During 'RemeberLastState': '<S44>:29' */
    /* '<S44>:29:1' LastLineState = Line; */
    rtDW.LastLineState = AngleManevr;
  }

  /* End of Chart: '<S40>/BimTrigger' */
  /* End of Outputs for SubSystem: '<S9>/BimTriggers' */

  /* Chart: '<S9>/Chart' incorporates:
   *  Inport: '<Root>/trackingCourse'
   */
  /* Gateway: flightController/feedback/Chart */
  /* During: flightController/feedback/Chart */
  if (rtDW.is_active_c5_flightController == 0U)
  {
    /* Entry: flightController/feedback/Chart */
    rtDW.is_active_c5_flightController = 1U;

    /* Entry Internal: flightController/feedback/Chart */
    /* Transition: '<S41>:26' */
    rtDW.is_c5_flightController = IN_Initialization;

    /* Entry 'Initialization': '<S41>:25' */
    /* '<S41>:25:1' initialGroundSpeed = 0; */
    rtDW.initialGroundSpeed = 0.0;

    /* '<S41>:25:1' initialCourse = 0; */
    rtDW.initialCourse = 0.0;

    /* '<S41>:25:2' currentGroundSpeed = 0; */
    rtDW.currentGroundSpeed = 0.0;

    /* '<S41>:25:3' currentCourse = 0; */
    rtDW.currentCourse = 0.0;

    /* '<S41>:25:4' windForce = 0; */
    rtDW.windForce = 0.0;

    /* '<S41>:25:4' windCourse = 0; */
    rtDW.windCourse = 0.0;
  }
  else
  {
    switch (rtDW.is_c5_flightController)
    {
     case IN_FixCurrentVector:
      /* During 'FixCurrentVector': '<S41>:28' */
      /* Transition: '<S41>:36' */
      rtDW.is_c5_flightController = IN_GetResult;

      /* Entry 'GetResult': '<S41>:31' */
      /* '<S41>:31:1' [windForce, windCourse] = WindEstimator(initialGroundSpeed, initialCourse, currentGroundSpeed, currentCourse, 20) */
      /*  Вычисляем проекции векторов */
      /* 'WindEstimator:3' groundSpeedX0 = groundSpeedModule0 * cos(pi/2 - groundSpeedCourse0); */
      V_hor = cos(1.5707963267948966 - rtDW.initialCourse) *
        rtDW.initialGroundSpeed;

      /* 'WindEstimator:4' groundSpeedY0 = groundSpeedModule0 * cos(- groundSpeedCourse0); */
      fi2 = rtDW.initialGroundSpeed * cos(-rtDW.initialCourse);

      /*  Вычисляем проекции векторов */
      /* 'WindEstimator:6' groundSpeedX1 = groundSpeedModule1 * cos(pi/2 - groundSpeedCourse1); */
      /* 'WindEstimator:7' groundSpeedY1 = groundSpeedModule1 * cos(- groundSpeedCourse1); */
      /*  Определяем расстояние между точками */
      /* 'WindEstimator:9' D = sqrt((groundSpeedX1 - groundSpeedX0)^2 + (groundSpeedY1 - groundSpeedY0)^2); */
      un_pow2 = cos(1.5707963267948966 - rtDW.currentCourse) *
        rtDW.currentGroundSpeed - V_hor;
      h_tmp = rtDW.currentGroundSpeed * cos(-rtDW.currentCourse) - fi2;
      un_idx_0 = sqrt(un_pow2 * un_pow2 + h_tmp * h_tmp);

      /* 'WindEstimator:10' if(D > 2*airSpeed || D < 0.5 || isnan(D)) */
      if ((un_idx_0 > 40.0) || (un_idx_0 < 0.5) || rtIsNaN(un_idx_0))
      {
        /*     %% Решение отсутсвует, окружности не пересекаются */
        /* 'WindEstimator:12' winwForce = 0; */
        rtDW.windForce = 0.0;

        /* 'WindEstimator:13' windAngle = 0; */
        V_hor = 0.0;
      }
      else
      {
        /* 'WindEstimator:14' else */
        /*     %% Сторона a одного из двух прямоугольных треугольников */
        /* a = (app.airSpeed^2 - app.airSpeed^2 + D^2)/(2*D); */
        /* 'WindEstimator:17' a = (D^2)/(2*D); */
        un_idx_1 = un_idx_0 * un_idx_0 / (2.0 * un_idx_0);

        /*     %% Высота прямоугольных труегольников */
        /* 'WindEstimator:19' h = sqrt(airSpeed^2 - a^2); */
        h = sqrt(400.0 - un_idx_1 * un_idx_1);

        /*     %% Координаты точки, которая делит общую сторону D на a и b  */
        /* 'WindEstimator:21' Xh = groundSpeedX0 + a*(groundSpeedX1-groundSpeedX0)/D; */
        V_hor += un_pow2 * un_idx_1 / un_idx_0;

        /* 'WindEstimator:22' Yh = groundSpeedY0 + a*(groundSpeedY1-groundSpeedY0)/D; */
        Yh = h_tmp * un_idx_1 / un_idx_0 + fi2;

        /*     %% Координты точек пересечения окружностей */
        /* 'WindEstimator:24' P1x = Xh + h*(groundSpeedY1-groundSpeedY0)/D; */
        fi2 = h_tmp * h / un_idx_0;
        un_idx_1 = fi2 + V_hor;

        /* 'WindEstimator:25' P1y = Yh - h*(groundSpeedX1-groundSpeedX0)/D; */
        un_idx_0 = un_pow2 * h / un_idx_0;
        un_pow2 = Yh - un_idx_0;

        /* 'WindEstimator:26' P2x = Xh - h*(groundSpeedY1-groundSpeedY0)/D; */
        fi2 = V_hor - fi2;

        /* 'WindEstimator:27' P2y = Yh + h*(groundSpeedX1-groundSpeedX0)/D; */
        V_hor = un_idx_0 + Yh;

        /*     %% Здесь нужно выбрать одну из двух точек */
        /*     %% 1) Будем выбирать, как ближайшую к началу координат в проекции на */
        /*  groundSpeed0, поэтому поворачиваем систему коопдинат так, чтобы ось X */
        /*  была соноправлена с вектором groundSpeed0, и выберем ту точку, которая */
        /*  вектор до которой короче в проекции на новую ось X */
        /*  probX1 = app.P1x*cos(absoluteAngle) + app.P1y*sin(absoluteAngle);  */
        /*  probX2 = app.P2x*cos(absoluteAngle) + app.P2y*sin(absoluteAngle); */
        /*  if(abs(probX1) < abs(probX2)) */
        /*      Px = P1x; */
        /*      Py = P1y; */
        /*  else */
        /*      Px = P2x; */
        /*      Py = P2y; */
        /*  end */
        /*     %% 2) Будем выбирать, как ближайшую к началу координат (минимальная длина) */
        /* 'WindEstimator:43' if (sqrt(P1x^2 + P1y^2) < sqrt(P2x^2 + P2y^2)) */
        if (sqrt(un_idx_1 * un_idx_1 + un_pow2 * un_pow2) < sqrt(fi2 * fi2 +
             V_hor * V_hor))
        {
          /* 'WindEstimator:44' Px = P1x; */
          fi2 = un_idx_1;

          /* 'WindEstimator:45' Py = P1y; */
          V_hor = un_pow2;
        }
        else
        {
          /* 'WindEstimator:46' else */
          /* 'WindEstimator:47' Px = P2x; */
          /* 'WindEstimator:48' Py = P2y; */
        }

        /*     %% Расчитываем характеристики ветра */
        /* 'WindEstimator:51' winwForce = sqrt(Px^2 + Py^2); */
        un_idx_0 = V_hor * V_hor;
        rtDW.windForce = sqrt(fi2 * fi2 + un_idx_0);

        /* 'WindEstimator:52' windAngle = Heading([Py, Px], [0 0], 0); */
        /*     %% Высчитаем, как изменились координаты */
        /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
        /*  */
        /*    Heading - Вычисление курса */
        /*    u0 - Координата точки относительно, которой вычисляем */
        /*    u1 - Координата точки курс до которой, вычисляем */
        /*    last_y - Возращаемое значение в случае, если расчет не удался */
        /*  */
        /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
        /* 'Heading:11' un = u1 - u0; */
        /* 'Heading:12' d_lat = un(1); */
        /* 'Heading:13' d_lon = un(2); */
        un_idx_1 = fi2;

        /*     %% Условие для определения кратчайшего пути с учетом переходов между */
        /*  меридианами 0 и 180 по долготе */
        /* 'Heading:16' if d_lon > 180 */
        if (fi2 > 180.0)
        {
          /* 'Heading:17' d_lon = d_lon - 360; */
          un_idx_1 = fi2 - 360.0;
        }
        else
        {
          if (fi2 < -180.0)
          {
            /* 'Heading:18' elseif d_lon < (-180) */
            /* 'Heading:19' d_lon = d_lon +360; */
            un_idx_1 = fi2 + 360.0;
          }
        }

        /*     %% Расчет курса */
        /* 'Heading:22' un_pow1 = d_lat*d_lat; */
        /* 'Heading:23' un_pow2 = d_lon*d_lon; */
        un_pow2 = un_idx_1 * un_idx_1;

        /* 'Heading:24' if (un_pow1 < 1e-16) && (un_pow2 < 1e-16) */
        if ((un_idx_0 < 1.0E-16) && (un_pow2 < 1.0E-16))
        {
          /* 'Heading:25' y = last_y; */
          V_hor = 0.0;
        }
        else
        {
          /* 'Heading:26' else */
          /* 'Heading:27' x = acos( d_lon / (sqrt(un_pow2 + un_pow1))); */
          /* 'Heading:28' y = acos( d_lat / (sqrt(un_pow2 + un_pow1))); */
          fi2 = sqrt(un_pow2 + un_idx_0);
          V_hor = acos(V_hor / fi2);

          /* 'Heading:29' if x > pi/2 */
          if (acos(un_idx_1 / fi2) > 1.5707963267948966)
          {
            /* 'Heading:30' y = 2*pi - y; */
            V_hor = 6.2831853071795862 - V_hor;
          }
        }
      }

      rtDW.windCourse = V_hor;

      /* '<S41>:31:3' initialGroundSpeed = currentGroundSpeed; */
      rtDW.initialGroundSpeed = rtDW.currentGroundSpeed;

      /* '<S41>:31:4' initialCourse = currentCourse; */
      rtDW.initialCourse = rtDW.currentCourse;
      break;

     case IN_FixInitialVector:
      /* During 'FixInitialVector': '<S41>:27' */
      /* Transition: '<S41>:39' */
      /* '<S41>:33:1' sf_internal_predicateOutput = ... */
      /* '<S41>:33:1' bimWasDisabled == 1; */
      if (rtb_Disabled)
      {
        /* Transition: '<S41>:33' */
        rtDW.is_c5_flightController = IN_Pause;
      }
      break;

     case IN_GetResult:
      /* During 'GetResult': '<S41>:31' */
      /* Transition: '<S41>:38' */
      /* '<S41>:33:1' sf_internal_predicateOutput = ... */
      /* '<S41>:33:1' bimWasDisabled == 1; */
      if (rtb_Disabled)
      {
        /* Transition: '<S41>:33' */
        rtDW.is_c5_flightController = IN_Pause;
      }
      break;

     case IN_Initialization:
      /* During 'Initialization': '<S41>:25' */
      /* '<S41>:29:1' sf_internal_predicateOutput = ... */
      /* '<S41>:29:1' bimWasEnabled == 1; */
      if (rtb_Enabled)
      {
        /* Transition: '<S41>:29' */
        rtDW.is_c5_flightController = IN_FixInitialVector;

        /* Entry 'FixInitialVector': '<S41>:27' */
        /* '<S41>:27:1' initialGroundSpeed = groundSpeed; */
        rtDW.initialGroundSpeed = V_hor;

        /* '<S41>:27:3' initialCourse = course; */
        rtDW.initialCourse = rtU.ActualCourse;
      }
      else
      {
        /* '<S41>:25:1' initialGroundSpeed = 0; */
        rtDW.initialGroundSpeed = 0.0;

        /* '<S41>:25:1' initialCourse = 0; */
        rtDW.initialCourse = 0.0;

        /* '<S41>:25:2' currentGroundSpeed = 0; */
        rtDW.currentGroundSpeed = 0.0;

        /* '<S41>:25:3' currentCourse = 0; */
        rtDW.currentCourse = 0.0;

        /* '<S41>:25:4' windForce = 0; */
        rtDW.windForce = 0.0;

        /* '<S41>:25:4' windCourse = 0; */
        rtDW.windCourse = 0.0;
      }
      break;

     default:
      /* During 'Pause': '<S41>:32' */
      /* '<S41>:30:1' sf_internal_predicateOutput = ... */
      /* '<S41>:30:1' bimWasEnabled == 1; */
      if (rtb_Enabled)
      {
        /* Transition: '<S41>:30' */
        rtDW.is_c5_flightController = IN_FixCurrentVector;

        /* Entry 'FixCurrentVector': '<S41>:28' */
        /* '<S41>:28:1' currentGroundSpeed = groundSpeed; */
        rtDW.currentGroundSpeed = V_hor;

        /* '<S41>:28:3' currentCourse = course; */
        rtDW.currentCourse = rtU.ActualCourse;
      }
      break;
    }
  }

  /* End of Chart: '<S9>/Chart' */

  /* Update for Delay: '<S9>/DsblTg' */
  /*     %% Рассчитаем угловое расстояние */
  /* MATLAB Function 'PreemptionTDP/MATLAB Function': '<S46>:1' */
  /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
  /*  */
  /*    PointMovement - Перемещение из точки Position на расстояние  */
  /*    DistanceM (метры) в направлении относительно севера Bearing */
  /*  */
  /* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
  /* '<S46>:1:9' AngularDistance = DistanceM/6378160; */
  /*     %% Координаты в радианы */
  /* '<S46>:1:11' Lat1 = Position(1)/180*pi; */
  /* '<S46>:1:12' Lon1 = Position(2)/180*pi; */
  /*     %% Получаем координаты точки перемещения */
  /* '<S46>:1:14' Lat2 = asin(sin(Lat1)*cos(AngularDistance) + cos(Lat1)*sin(AngularDistance)*cos(Bearing)); */
  /* '<S46>:1:15' Lon2 = Lon1 + atan2(sin(Bearing)*sin(AngularDistance)*cos(Lat1), cos(AngularDistance)-sin(Lat1)*sin(Lat2)); */
  /*     %% Переводим в градусы */
  /* '<S46>:1:17' Lat2 = Lat2/pi*180; */
  /* '<S46>:1:18' Lon2 = Lon2/pi*180; */
  /* '<S46>:1:19' NPoint = [Lat2, Lon2, Position(3)]; */
  rtDW.DsblTg_DSTATE = rtb_Disabled;

  /* Update for Delay: '<S17>/LastPos' incorporates:
   *  Inport: '<Root>/Pos_lat'
   *  Inport: '<Root>/Pos_lon'
   */
  rtDW.LastPos_1_DSTATE = rtU.Pos_lat;
  rtDW.LastPos_2_DSTATE = rtU.Pos_lon;

  /* End of Outputs for SubSystem: '<Root>/flightController' */

  /* Outport: '<Root>/bimCommand' */
  rtY.bimCommand = AngleManevr;

  /* Outputs for Atomic SubSystem: '<Root>/flightController' */
  /* Signum: '<S1>/Sign' */
  if (AngleManevr < 0.0)
  {
    /* Outport: '<Root>/bimSelect' incorporates:
     *  DataTypeConversion: '<S1>/Data Type Conversion'
     */
    rtY.bimSelect = -1;
  }
  else if (AngleManevr > 0.0)
  {
    /* Outport: '<Root>/bimSelect' incorporates:
     *  DataTypeConversion: '<S1>/Data Type Conversion'
     */
    rtY.bimSelect = 1;
  }
  else if (AngleManevr == 0.0)
  {
    /* Outport: '<Root>/bimSelect' incorporates:
     *  DataTypeConversion: '<S1>/Data Type Conversion'
     */
    rtY.bimSelect = 0;
  }
  else
  {
    /* Outport: '<Root>/bimSelect' incorporates:
     *  DataTypeConversion: '<S1>/Data Type Conversion'
     */
    rtY.bimSelect = 0;
  }

  /* End of Signum: '<S1>/Sign' */

  /* Outport: '<Root>/bimForce' incorporates:
   *  Abs: '<S1>/Abs'
   *  DataTypeConversion: '<S1>/Data Type Conversion1'
   */
  rtY.bimForce = (uint8_T)fabs(AngleManevr);

  /* End of Outputs for SubSystem: '<Root>/flightController' */
}

/* Model initialize function */
void flightController_initialize(void)
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
