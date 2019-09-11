/*******************************************************************************************************************************
    Version 1.1

    Heightmap - модуль работы с картой высот.

    Карта высот представляет собой сетку, некоторой области карты. Имеется нуль-точка занимающая 
    нижний левый угол карты (узел сетки), известны шаги в секундах при перемещении между узлами 
    по широте и долготе, известно количество узловых точек по широте и долготе, и известна 
    высота в этих узловых точках. Но координаты каждой из точек исходно неизвестны, но их можно
    найти зная индексы узловых точек по широте и долготе и производя отсчет от нуль точки.

    Сферичностью поверхности намерено принебрегаем, так как загруженная область карты довольно мала,
    и точность навигации в данном случае будет в приемлемых диапазонах. Поэтому координаты 
    lon и lat (широта и долгота) будем использовать как декартовы x,y в декартовой системе координат.


		   
          /\ Широта
           |                                                     MapStepLon  - Шаг сетки по долготе
           |                                                   |<---------->|
      j=m= |____________|____________|____________|____________|____________|____________|_______
 =LatCount |            |            |            |            |            |            |
Число      |            |            |            |            |            |            |     
широт      .            .            .            .            .            .            .
           .            .            .            .            .            .            .
           .            .            .            .            .            .            .
           .            .            .            .            .            .            .
           |            |            |            |            |            |            |    
       j=5 |____________|____________|____________|____________|____________|____... ____|_______
           |            |            |            |            |            |            |
           |            |            |            |            |            |            |
           |            |            |            |            |            |            |
           |            |            |            |            |            |            |
       j=4 |____________|____________|____________|____________|____________|____..._____|_______
           |            |            |            |            |            |            |
           |            |            |            |            |            |            |
           |            |            |            |            |            |            |
           |            |            |            |            |            |            |
       j=3 |____________|____________|____________|____________|____________|_____...____|_______
           |            |            |            |            |            |            |      /|\
           |            |            |            |            |            |            |       |
           |            |            |            |            |            |            |       |   MapStepLat - Шаг сетки по широте
           |            |            |            |            |            |            |       |
       j=2 |____________|____________|____________|____________|____________|_____...____|______\|/
           |            |            |            |            |            |            |
           |            |            |            |            |            |            |
           |            |            |            |            |            |            |
           |            |            |            |            |            |            |
       j=1 |____________|____________|____________|____________|____________|_____...____|_______
           |            |            |            |            |            |            |
           |            |            |            |            |            |            |
           |            |            |            |            |            |            |
           |            |            |            |            |            |            |
           o____________|____________|____________|____________|____________|_____...____|_______________> Долгота
     Нуль точка        i=1          i=2          i=3          i=4          i=5          i=n=LonCount
     (i=0; j=0)                                                                        Число долгот (индексов сетки по долготе)


    Зная индекс по долготе и индекс по широте, широту и долготу нуль-точки, а так же шаг сетки 
    мы всегда можем узнать координаты узловой точки и высоту рельефа в ней:
    Примерно так: 
        Узловая точка с индексами (i,j):  Долгота: Долгота нуль-точки + i * Шаг сетки по долготе;  
                                          Широта: Широта нуль-точки + j * Шаг сетки по широте;  
	  
    Вычисление высоты рельефа в текущей позиции происходит на основе метода триангуляции, то есть
    построение плоскости рельефа. 
	  
    по трем ближайшим точкам (используя их широту, долготу и высоту)
    Далее вы

    Данный модуль позволяет узнать высоту рельефа в текущей точке геолокации.
    Алгоритм:
    1) Передаём долготу и широту нашего месположения функции GetHeigh_OnThisPoint(lon,lat);
    2) Функция ищет между какими 4мя ближайщими узловыми точками находятся переданные ей координаты.
    на этом этапе получаем квадрат с вершинами в узловых точках, в котором мы находимся.
        (i,j+1)      (i+1,j+1)
            A___________B
            |          |
            |    o-----|----- Наши координаты, ABCD - узловые точки
            |          |
            |          |
            |__________|
            C           D
            (i,j)       (i+1,j)

    3) Далее данный квадрат разбивается на два треугольника ABC и BCD следующим образом
            A___________B
            |        / |
            |   o  /   |
            |    /     |
            |  /       |
            |/_________|
            C           D
	  
    4) Теперь необходимо узнать в какой из треугольников попадает наша точка.
    Это определяем так:
    Точка o - наша геолокация. Строим вспомогательные треугольники 
    Для первого треугольника: AoB, AoC, BoC
    Для второго треугольника: BoD, BoC, DoC

    Возможны два варианта:
    1) 
        A_______B
        |.    ./
        | .o. /
        | .  /
        |.  /
        |  /             
        C    

                ./| B
             .  / |
        o .    /  |
        ..    /   |
        .  . /    |
        .   /.    |
        .  /  .   |
        . /     . |
        ./________|
        C           D

    Если точка находится внутри треугольника, сумма площадей воспомогательных треугольников будет равна площади исходного, как например для ABC
    Sabc = Saob + Saoc + Sboc
    В противном случае, как для BCD - не равна
    Sbcd = Sbod + Sboc + Sdoc
	   
    5) Треугольник содержащий точку определили, теперь используя три его вершины (и широту, долготу, высоту рельефа в них) строим аппроксимирующую поверхность (триангуляция)
    данной области рельефа. 

    Необходимо найти уравненение плоскости. Уравнение находится из решения определителя матрицы:
	  
    |x-x1   x2-x1   x3-x1|
    |y-y1   y2-y1   y3-y1|  --->  A*x + B*y + C*z + D = 0
    |z-z1   z2-z1   z3-z1|

    Зная поверность в этой зоне (точнее уравнение поверхности), можем используя две координаты нашей текущей позиции (широта, долгота)
    узнать значение третьей - высоту рельефа в нашей точке.
    Подставляем текущую позицию (lon, lat) - > (x,y).
    Находим z - это и есть наша высота.

*****************************************************************************************************************************************************************************************/
#ifndef _HEIGHT_MAP_
#define _HEIGHT_MAP_

// Тип координат с плавающей точкой
typedef struct {
	double lon;    // Долгота x
	double lat;    // Широта y
	double alt;    // Высота z
}point;

// Тип координат для целочисленных операций
typedef struct {
	long long lon;    // Долгота x
	long long lat;    // Широта y
	short alt;        // Высота z
}integer_point;

// Режимы вычисления функции GetHeigh_OnThisPoint()
typedef enum {
	TRIANGULARTION,    // Метод триангуляции - высокая точность
	UPPER_BOUND,       // Оценка высоты по верхней границе - наименее точный, но быстрый и упреждающий
	AVERAGE            // Метод усреднения высот узловых точек - компромисное решение
} MAP_MODE;

// Код ошибки при отсутсвии решения
typedef enum {
	MAP_NO_SOLUTION = 0x7FFF,
	MAP_NOT_AVAILABLE = 0,
	MAP_AVAILABLE = 1,
} Heightmap_STATUS;



/********************************************************************************************************************
    GetHeigh_OnThisPoint - Узнать высоту рельефа в данной точке
    Параметры:
                lon - Долгота данной точки
                lat - Широта данной точки
                mode - режим оценки высоты (см enum MAP_MODE)
    Возвращает:
                Значение высоты в данной точке
                либо
                если мы вышли за пределы карты - MAP_NO_SOLUTION
*********************************************************************************************************************/
short GetHeight_OnThisPoint(double lon, double lat, MAP_MODE mode);



/**********************************************************************************************
    TrianglePos - Определение принадлежности точки loc, треугольнику с вершинами
    a,b,c.
    Параметры:
                a,b,C - Вершины треугольника
                loc - координаты точки, принадлежность которой определяем
    Возвращает: 0 - Не принадлежит
                1 - Принадлежит
***********************************************************************************************/
char TrianglePos(point a, point b, point c, point loc);


/************************************************************************************************
    HeightFromPlane - Построение поверхности по трём точкам и вычисление высоты
    в точке с координатами loc (метод триангуляции).
    Параметры:
                a,b,c - координаты точек, по которым строим поверхность
                loc - указатель на координаты точки, высоту в которой нужно узнать
    Возвращает:
                loc - через указатель заполняется поле loc.alt, и вся
                структура возвращается
************************************************************************************************/
void HeightFromPlane(point a, point b, point c, point* loc);


/*********************************************************************************************************************************************
    MapHeight - Определение высоты рельефа в точке location, по 4 ближайшим вершинам сетки
    Параметры:
                left_lower   - Левая нижняя вершина квадрата (наименьшие координаты по долготе и широте среди вершин)
                left_upper   - Левая верхняя вершина квадрата
                right_lower  - Правая нижняя вершина квадрата
                right_upper  - Правая верхняя вершина квадрата (наибольшие координаты по долготе и широте среди вершин)
                location     - Точка высоту рельефа в которой необходимо определить
    Возвращает: 
                Округленную по ближайшего целого высоту в точке location
                либо
                MAP_NO_SOLUTION - если ошибка в аргументах, либо точка location вне квадрата

    Примечание: Порядок вершин крайне важен, иначе вернёт ошибку
*********************************************************************************************************************************************/
short MapHeight(point left_lower, point left_upper, point right_lower, point right_upper, point location);



/*********************************************************************************************************************************************
    GetAvailabilityStatus - Получить статус доступности карты в точке (lat,lon)
    Параметры:
                lat - Координата точки по широте
                lon - Координата точки по долготе
    Возвращает: 
                0 - Данная точка находится за пределами карты, карта в этой точке недоступна
                1 - Карта высот в этой точке доступна
*********************************************************************************************************************************************/
Heightmap_STATUS GetAvailabilityStatus(double lon, double lat);


#endif // !_HEIGHT_MAP_

