#include "Heightmap_conf_and_pd.h"
#include "../1636PP52Y.h"
#include "MDR32F9Qx_port.h"


// Адреса, по которым лежит информация по карте
#define Adr_TDP_lat              0          // Адрес, по которому лежит ШИРОТА точки приземления
#define Adr_TDP_lon              8          // Адрес, по которому лежит ДОЛГОТА точки приземления
#define Adr_TDP_alt              16         // Адрес, по которому лежит ВЫСОТА РАСКРЫТИЯ ПАРАШЮТА
#define Adr_PacketNum            18         // Адрес, по которому лежит КОЛИЧЕСТВО ПАКЕТОВ с картой
#define Adr_LatCount             20         // Адрес, по которому лежит ЧИСЛО ШИРОТ ПАРАШЮТА
#define Adr_LonCount             22         // Адрес, по которому лежит ЧИСЛО ДОЛГОТ ПАРАШЮТА
#define Adr_NullPointLat         24         // Адрес, по которому лежит ШИРОТА НУЛЬ ТОЧКИ - Левая нижняя на карте
#define Adr_NullPointLon         32         // Адрес, по которому лежит ДОЛГОТА НУЛЬ ТОЧКИ - Левая нижняя на карте
#define Adr_StepLat              40         // Адрес, по которому лежит ШАГ СЕТКИ ПО ШИРОТЕ
#define Adr_StepLon              48         // Адрес, по которому лежит ШАГ СЕТКИ ПО ДОЛГОТЕ
#define Adr_CheckMap             320056     // Адрес, по которому лежит ПРИЗНАК ЗАГРУЖЕНОСТИ КАРТЫ
#define Adr_FirstAlt             56         // Адрес, по которому лежит НАЧАЛО ПЕРВОГО ПАКЕТА с высотами

// Структурные определения информационной части карты
#define ByteSizeMapPacket        800        // Размер в байтах ПАКЕТА С ВЫСОТАМИ
#define AltNumINPacket           400        // Количество высот в одном пакете 



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
	// Признак загружености карты записан по адресу 320056 (4 байта)
	ReadMap(Adr_CheckMap, buff, 4);
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
	ReadMap(Adr_TDP_lat, buff, 8);
	return *(double*)buff;
}



/**********************************************************************************
      GetTouchDownPointLon - Получить долготу точки приземления 			
**********************************************************************************/
double GetTouchDownPointLon(void)
{
	unsigned char buff[8];
	ReadMap(Adr_TDP_lon , buff, 8);
	return *(double*)buff;
}


/**********************************************************************************
       GetTouchDownPointAlt - Получить высоту раскрытия парашюта 
**********************************************************************************/
double GetTouchDownPointAlt(void)
{
	unsigned char buff[2];
	ReadMap(Adr_TDP_alt, buff, 2);
	return *(short*)buff;
}



/**********************************************************************************
      GetMapProperties_LatCount - Получить количество узловых точек по широте 			
**********************************************************************************/
short GetMapProperties_LatCount(void)
{
	unsigned char buff[2];
	ReadMap(Adr_LatCount, buff, 2);
	return *(short*)buff;
}



/**********************************************************************************
      GetMapProperties_LonCount - Получить количество узловых точек по долготе 			
**********************************************************************************/
short GetMapProperties_LonCount(void)
{
	unsigned char buff[2];
	
	ReadMap(Adr_LonCount, buff, 2);
	return *(short*)buff;
}



/**********************************************************************************
      GetMapProperties_NullPointLat - Получить широту нулевой точки 		
**********************************************************************************/
double GetMapProperties_NullPointLat(void)
{
	unsigned char buff[8];
	
	ReadMap(Adr_NullPointLat, buff, 8);
	return *(double*)buff;
}



/**********************************************************************************
      GetMapProperties_NullPointLat - Получить долготу нулевой точки 
**********************************************************************************/
double GetMapProperties_NullPointLon(void)
{
	unsigned char buff[8];
	
	ReadMap(Adr_NullPointLon, buff, 8);
	return *(double*)buff;
}



/**********************************************************************************
      GetMapProperties_MapStepLat - Получить размер шага по широте 			
**********************************************************************************/
double GetMapProperties_MapStepLat(void)
{
	unsigned char buff[8];
	
	ReadMap(Adr_StepLat, buff, 8);
	// Значение действительно в секундах, то нужно привести к градусам делением на 3600
	return (*(double*)buff) / 3600.0;
}



/**********************************************************************************
      GetMapProperties_MapStepLon - Получить размер шага по долготе 	
**********************************************************************************/
double GetMapProperties_MapStepLon(void)
{
	unsigned char buff[8];
	
	ReadMap(Adr_StepLon, buff, 8);
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
	
	Address = Adr_FirstAlt + ByteSizeMapPacket * (AltNumINPacket - 1 - LAT_INDEX) + 2 * LON_INDEX;
	ReadMap(Address, buff, 2);
	
	return *(short*)buff;
}
