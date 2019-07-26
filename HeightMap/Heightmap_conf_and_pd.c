#include "Heightmap_conf_and_pd.h"
#include "../1636PP52Y.h"
#include "MDR32F9Qx_port.h"




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
			  SPI_1636PP52Y_ReadArray (SPI_1636PP52Y_CS1, Address + Bytecount, Destination++, 1);
			
			else if (((Address + Bytecount) >= 0x20000)  &&  ((Address + Bytecount) < 0x40000))
			  SPI_1636PP52Y_ReadArray (SPI_1636PP52Y_CS2, Address + Bytecount, Destination++, 1);
				
			else if (((Address + Bytecount) >= 0x40000)  &&  ((Address + Bytecount) < 0x60000))
			  SPI_1636PP52Y_ReadArray (SPI_1636PP52Y_CS3, Address + Bytecount, Destination++, 1);
			
			else if (((Address + Bytecount) >= 0x60000)  &&  ((Address + Bytecount) < 0x80000))
			  SPI_1636PP52Y_ReadArray (SPI_1636PP52Y_CS4, Address + Bytecount, Destination++, 1);

			Bytecount ++;
	}
}



/**********************************************************************************
      CheckMap - Проверить загружена ли карта и точка приземления
**********************************************************************************/
char CheckMap(void)
{	
	unsigned char buff[4];
	// Признак загружености карты записан по адресу 320054 (4 байта)
	ReadMap(320054, buff, 4);
	// Проверяем признак
	if(*((uint32_t*)buff) == 0x4F4F4F4F)
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
	double temp = 0;
	ReadMap(0, buff, 8);
	temp = *(double*)buff;
	return temp;
}



/**********************************************************************************
      GetTouchDownPointLon - Получить долготу точки приземления 			
**********************************************************************************/
double GetTouchDownPointLon(void)
{
	unsigned char buff[8];
	
	ReadMap(8, buff, 8);
	return *(double*)buff;
}



/**********************************************************************************
      GetMapProperties_LatCount - Получить количество узловых точек по широте 			
**********************************************************************************/
short GetMapProperties_LatCount(void)
{
	unsigned char buff[2];
	
	ReadMap(18, buff, 2);
	return *(short*)buff;
}



/**********************************************************************************
      GetMapProperties_LonCount - Получить количество узловых точек по долготе 			
**********************************************************************************/
short GetMapProperties_LonCount(void)
{
	unsigned char buff[2];
	
	ReadMap(20, buff, 2);
	return *(short*)buff;
}



/**********************************************************************************
      GetMapProperties_NullPointLat - Получить широту нулевой точки 		
**********************************************************************************/
double GetMapProperties_NullPointLat(void)
{
	unsigned char buff[8];
	
	ReadMap(22, buff, 8);
	return *(double*)buff;
}



/**********************************************************************************
      GetMapProperties_NullPointLat - Получить долготу нулевой точки 
**********************************************************************************/
double GetMapProperties_NullPointLon(void)
{
	unsigned char buff[8];
	
	ReadMap(30, buff, 8);
	return *(double*)buff;
}



/**********************************************************************************
      GetMapProperties_MapStepLat - Получить размер шага по широте 			
**********************************************************************************/
double GetMapProperties_MapStepLat(void)
{
	unsigned char buff[8];
	
	ReadMap(38, buff, 8);
	// Значение действительно в секундах, то нужно привести к градусам делением на 3600
	return (*(double*)buff) / 3600.0;
}



/**********************************************************************************
      GetMapProperties_MapStepLon - Получить размер шага по долготе 	
**********************************************************************************/
double GetMapProperties_MapStepLon(void)
{
	unsigned char buff[8];
	
	ReadMap(46, buff, 8);
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
	// Формула для расчета адреса чтения из памяти 54 + 800*(400 - 1 - j) + 2*i, 
	// где 800 - размер пакета высот, количество точек по широте, 
	// 54 размер информационного пакета лежащего перед картой в памяти
	unsigned char buff[2];
	unsigned int Address = 0;
	
	Address = 54 + 800 * (400 - 1 - LAT_INDEX) + 2 * LON_INDEX;
	ReadMap(Address, buff, 2);
	
	return *(short*)buff;
}
