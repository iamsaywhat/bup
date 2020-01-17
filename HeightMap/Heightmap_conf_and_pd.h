/***************************************************************************************************

    Heightmap_conf_and_pd - Подмодуль Heightmap, благодаря которому, платформозависимые 
    функции и определения вынесены отдельно. Позволяет проводить отладку в любой среде
    и осуществлять быстрое портирование на любые платформы
		
    Текущее расположение карты высот во внешней памяти по адресам, начиная с нулевого
		
               0| Сведения о карте |55
              56| Высоты по широте |855
             856| Высоты по широте |1655
            1656| Высоты по широте |2455
        ...................................
          319256| Высоты по широте |320055
		
	
    Графически это соответствует:
		
                Долгота
                 /\
                  |
                56| Высоты по широте |855
               856| Высоты по широте |1655
              1656| Высоты по широте |2455
                    .............
            319256| Высоты по широте |320055
    Нуль точка--->0----------------------------> Широта
					 
    При загрузке карты сейчас по адресу 320056 внешней памяти лежит признак загрузки карты
    "OOOO" или в hex 0x4F4F4F4F
    Если память была очищена, либо разметка карты была испорчена, данный признак, будет там отсутствовать.
					 
					 
    Использует: 
        1636PP52Y.h/с     - Драйвер внешней памяти, где располается карта
        MDR32F9Qx_port.h  - Драйвер библиотеки SPL от Миландр
		
***************************************************************************************************/
#ifndef _HEIGHT_MAP_CONF_AND_PD_
#define _HEIGHT_MAP_CONF_AND_PD_



/**********************************************************************************
  CheckMap - Проверить загружена ли карта и точка приземления
**********************************************************************************/
char CheckMap(void);



/**********************************************************************************
  GetTouchDownPointLat - Получить широту точки приземления 	
**********************************************************************************/
double GetTouchDownPointLat(void);



/**********************************************************************************
  GetTouchDownPointLon - Получить долготу точки приземления 
**********************************************************************************/
double GetTouchDownPointLon(void);



/**********************************************************************************
  GetTouchDownPointAlt - Получить высоту раскрытия парашюта 
**********************************************************************************/
double GetTouchDownPointAlt(void);



/**********************************************************************************
  GetMapProperties_LatCount - Получить количество узловых точек по широте 
**********************************************************************************/
short GetMapProperties_LatCount(void);



/**********************************************************************************
  GetMapProperties_LonCount - Получить количество узловых точек по долготе 
**********************************************************************************/
short GetMapProperties_LonCount(void);



/**********************************************************************************
  GetMapProperties_NullPointLat - Получить широту нулевой точки 
**********************************************************************************/
double GetMapProperties_NullPointLat(void);



/**********************************************************************************
  GetMapProperties_NullPointLat - Получить долготу нулевой точки 
**********************************************************************************/
double GetMapProperties_NullPointLon(void);



/**********************************************************************************
  GetMapProperties_MapStepLat - Получить размер шага по широте 
**********************************************************************************/
double GetMapProperties_MapStepLat(void);



/**********************************************************************************
  GetMapProperties_MapStepLon - Получить размер шага по долготе 
**********************************************************************************/
double GetMapProperties_MapStepLon(void);



/**********************************************************************************
  GetMapProperties_Alt - Осуществляет поиск высоты в узловой точке с 
  индексами (LON_INDEX, LAT_INDEX)
			
  Параметры: 
            LON_INDEX - Индекс по долготе сетки карты от нулевой точки.
            LAT_INDEX - Индекс по широте сетки карты от нулевой точки
**********************************************************************************/
short GetMapProperties_Alt(unsigned int LON_INDEX, unsigned int LAT_INDEX);



#endif
