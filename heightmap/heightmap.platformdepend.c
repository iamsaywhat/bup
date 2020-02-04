#include "heightmap.platformdepend.h"
#include "mapflashlayout.h"
#include "1636pp52y.h"


/**********************************************************************************
  readMap - Чтение карты с носителя
  Параметры:
            Address     - Адрес места чтения      
            Destination - Буфер для чтения
            Size        - Размер читаемой области в байтах
**********************************************************************************/
static void readMap(uint32_t Address, uint8_t* Destination, uint32_t Size)
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
  checkMap - Проверить загружена ли карта и точка приземления
**********************************************************************************/
char checkMap(void)
{	
  unsigned char buff[4];
  // Признак загружености карты записан по адресу 320056 (4 байта)
  readMap(ADDRESS_MAP_TAG, buff, 4);
  // Проверяем признак
  if(*((uint32_t*)buff) == MAP_TAG)
    return 1;
  else
    return 0;
}

/**********************************************************************************
  getTouchdownLatitude - Получить широту точки приземления 	
**********************************************************************************/
double getTouchdownLatitude(void)
{
  unsigned char buff[8];
  readMap(ADDRESS_TDP_LAT, buff, 8);
  return *(double*)buff;
}

/**********************************************************************************
  getTouchdownLongitude - Получить долготу точки приземления 
**********************************************************************************/
double getTouchdownLongitude(void)
{
  unsigned char buff[8];
  readMap(ADDRESS_TDP_LON , buff, 8);
  return *(double*)buff;
}

/**********************************************************************************
  getTouchdownAltitude - Получить высоту раскрытия парашюта 
**********************************************************************************/
double getTouchdownAltitude(void)
{
  unsigned char buff[2];
  readMap(ADDRESS_TDP_ALT, buff, 2);
  return *(short*)buff;
}

/**********************************************************************************
  getNumberOfLatitude - Получить количество узловых точек по широте 
**********************************************************************************/
short getNumberOfLatitude(void)
{
  unsigned char buff[2];
  readMap(ADDRESS_LAT_COUNT, buff, 2);
  return *(short*)buff;
}

/**********************************************************************************
  getNumberOfLongitude - Получить количество узловых точек по долготе 
**********************************************************************************/
short getNumberOfLongitude(void)
{
  unsigned char buff[2];
  readMap(ADDRESS_LON_COUNT, buff, 2);
  return *(short*)buff;
}

/**********************************************************************************
  getBottomLeftLatitude - Получить широту нулевой точки 
**********************************************************************************/
double getBottomLeftLatitude(void)
{
  unsigned char buff[8];
  readMap(ADDRESS_NULL_POINT_LAT, buff, 8);
  return *(double*)buff;
}

/**********************************************************************************
  getBottomLeftLongitude - Получить долготу нулевой точки 
**********************************************************************************/
double getBottomLeftLongitude(void)
{
  unsigned char buff[8];
  readMap(ADDRESS_NULL_POINT_LON, buff, 8);
  return *(double*)buff;
}

/**********************************************************************************
  getLatitudeGridStep - Получить размер шага по широте 
**********************************************************************************/
double getLatitudeGridStep(void)
{
  unsigned char buff[8];
  readMap(ADDRESS_STEP_LAT, buff, 8);
  // Значение действительно в секундах, то нужно привести к градусам делением на 3600
  return (*(double*)buff) / 3600.0;
}

/**********************************************************************************
  getLongitudeGridStep - Получить размер шага по долготе 
**********************************************************************************/
double getLongitudeGridStep(void)
{
  unsigned char buff[8];
  readMap(ADDRESS_STEP_LON, buff, 8);
  // Значение действительно в секундах, то нужно привести к градусам делением на 3600
  return (*(double*)buff) / 3600.0;
}

/**********************************************************************************
  getAltitudeOnJunctionPoint - Осуществляет поиск высоты в узловой точке с 
  индексами (longitudeIndex, latitudeIndex)
			
  Параметры: 
            longitudeIndex - Индекс по долготе сетки карты от нулевой точки.
            latitudeIndex - Индекс по широте сетки карты от нулевой точки
**********************************************************************************/
short getAltitudeOnJunctionPoint(unsigned int longitudeIndex, unsigned int latitudeIndex)
{
  // Формула для расчета адреса чтения из памяти 56 + 800*(400 - 1 - j) + 2*i, 
  // где 800 - размер пакета высот, количество точек по широте, 
  // 54 размер информационного пакета лежащего перед картой в памяти
  unsigned char buff[2];
  unsigned int Address = 0;
	
  Address = ADDRESS_FIRST_ALT + 
            SIZE_OF_n_PACKET_DATA * (NUMBER_OF_DATA_IN_PACKET - 1 - latitudeIndex) + 
            2 * longitudeIndex;
  readMap(Address, buff, 2);
	
  return *(short*)buff;
}
