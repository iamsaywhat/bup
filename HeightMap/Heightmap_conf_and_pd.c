#include "Heightmap_conf_and_pd.h"
#include "map_flash_layout.h"
#include "1636PP52Y.h"


/**********************************************************************************
  ReadMap - Чтение карты с носителя
  Параметры:
            Address     - Адрес места чтения      
            Destination - Буфер для чтения
            Size        - Размер читаемой области в байтах
**********************************************************************************/
void ReadMap(uint32_t Address, uint8_t* Destination, uint32_t Size)
{	
  uint32_t  Bytecount = 0;
	
  while(Bytecount < Size)
  {
    if(((Address + Bytecount) < 0x20000))
      SPI_1636PP52Y_readArray (SPI_1636PP52Y_CS1, Address + Bytecount, Destination++, 1);
			
    else if (((Address + Bytecount) >= 0x20000)  &&  ((Address + Bytecount) < 0x40000))
      SPI_1636PP52Y_readArray (SPI_1636PP52Y_CS2, Address + Bytecount, Destination++, 1);
				
    else if (((Address + Bytecount) >= 0x40000)  &&  ((Address + Bytecount) < 0x60000))
      SPI_1636PP52Y_readArray (SPI_1636PP52Y_CS3, Address + Bytecount, Destination++, 1);
			
    else if (((Address + Bytecount) >= 0x60000)  &&  ((Address + Bytecount) < 0x80000))
      SPI_1636PP52Y_readArray (SPI_1636PP52Y_CS4, Address + Bytecount, Destination++, 1);

    Bytecount ++;
	}
}



/**********************************************************************************
  CheckMap - Проверить загружена ли карта и точка приземления
**********************************************************************************/
char CheckMap(void)
{	
  unsigned char buff[4];
  // Признак загружености карты записан по адресу 320056 (4 байта)
  ReadMap(ADDRESS_MAP_TAG, buff, 4);
  // Проверяем признак
  if(*((uint32_t*)buff) == MAP_TAG)
    return 1;
  else
    return 0;
}


/**********************************************************************************
  GetTouchDownPointLat - Получить широту точки приземления 
**********************************************************************************/
double GetTouchDownPointLat(void)
{
  unsigned char buff[8];
  ReadMap(ADDRESS_TDP_LAT, buff, 8);
  return *(double*)buff;
}



/**********************************************************************************
  GetTouchDownPointLon - Получить долготу точки приземления 			
**********************************************************************************/
double GetTouchDownPointLon(void)
{
  unsigned char buff[8];
  ReadMap(ADDRESS_TDP_LON , buff, 8);
  return *(double*)buff;
}


/**********************************************************************************
  GetTouchDownPointAlt - Получить высоту раскрытия парашюта 
**********************************************************************************/
double GetTouchDownPointAlt(void)
{
  unsigned char buff[2];
  ReadMap(ADDRESS_TDP_ALT, buff, 2);
  return *(short*)buff;
}



/**********************************************************************************
  GetMapProperties_LatCount - Получить количество узловых точек по широте 			
**********************************************************************************/
short GetMapProperties_LatCount(void)
{
  unsigned char buff[2];
  ReadMap(ADDRESS_LAT_COUNT, buff, 2);
  return *(short*)buff;
}



/**********************************************************************************
  GetMapProperties_LonCount - Получить количество узловых точек по долготе 			
**********************************************************************************/
short GetMapProperties_LonCount(void)
{
  unsigned char buff[2];
  ReadMap(ADDRESS_LON_COUNT, buff, 2);
  return *(short*)buff;
}



/**********************************************************************************
  GetMapProperties_NullPointLat - Получить широту нулевой точки 		
**********************************************************************************/
double GetMapProperties_NullPointLat(void)
{
  unsigned char buff[8];
  ReadMap(ADDRESS_NULL_POINT_LAT, buff, 8);
  return *(double*)buff;
}



/**********************************************************************************
  GetMapProperties_NullPointLat - Получить долготу нулевой точки 
**********************************************************************************/
double GetMapProperties_NullPointLon(void)
{
  unsigned char buff[8];
  ReadMap(ADDRESS_NULL_POINT_LON, buff, 8);
  return *(double*)buff;
}



/**********************************************************************************
  GetMapProperties_MapStepLat - Получить размер шага по широте 			
**********************************************************************************/
double GetMapProperties_MapStepLat(void)
{
  unsigned char buff[8];
  ReadMap(ADDRESS_STEP_LAT, buff, 8);
  // Значение действительно в секундах, то нужно привести к градусам делением на 3600
  return (*(double*)buff) / 3600.0;
}



/**********************************************************************************
  GetMapProperties_MapStepLon - Получить размер шага по долготе 	
**********************************************************************************/
double GetMapProperties_MapStepLon(void)
{
  unsigned char buff[8];
  ReadMap(ADDRESS_STEP_LON, buff, 8);
  // Значение действительно в секундах, то нужно привести к градусам делением на 3600
  return (*(double*)buff) / 3600.0;
}



/**********************************************************************************
  GetMapProperties_Alt - Осуществляет поиск высоты в узловой точке с 
  индексами (LON_INDEX, LAT_INDEX)	
  Параметры: 
            LON_INDEX - Индекс по долготе сетки карты от нулевой точки.
            LAT_INDEX - Индекс по широте сетки карты от нулевой точки
**********************************************************************************/
short GetMapProperties_Alt(unsigned int LON_INDEX, unsigned int LAT_INDEX)
{
  // Формула для расчета адреса чтения из памяти 56 + 800*(400 - 1 - j) + 2*i, 
  // где 800 - размер пакета высот, количество точек по широте, 
  // 54 размер информационного пакета лежащего перед картой в памяти
  unsigned char buff[2];
  unsigned int Address = 0;
	
  Address = ADDRESS_FIRST_ALT + 
            SIZE_OF_n_PACKET_DATA * (NUMBER_OF_DATA_IN_PACKET - 1 - LAT_INDEX) + 
            2 * LON_INDEX;
  ReadMap(Address, buff, 2);
	
  return *(short*)buff;
}
