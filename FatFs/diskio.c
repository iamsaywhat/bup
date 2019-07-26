/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2016        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "ff.h"			/* Obtains integer types */
#include "diskio.h"		/* Declarations of disk functions */

#include "../1636PP52Y.h"
#include "MDR32F9Qx_port.h"

/* Definitions of physical drive number for each drive */
#define DEV_FLASH		0	/* Example: Map Ramdisk to physical drive 0 */
#define DEV_MMC		1	/* Example: Map MMC/SD card to physical drive 1 */
#define DEV_USB		2	/* Example: Map USB MSD to physical drive 2 */


//uint8_t bufferflash[65536];

/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/


DSTATUS disk_status (BYTE pdrv)		// pdrv - Physical drive nmuber to identify the drive 
{
	DSTATUS stat;
	if(pdrv != DEV_FLASH)
		return STA_NOINIT;
	return 0;
}



void FLASH_disk_write(const BYTE *buff, uint32_t sector, uint8_t count);
void FLASH_disk_read(uint8_t *buff, uint32_t sector, uint8_t count);
void erase_512b_flash (uint8_t sectorFS);
int FLASH_disk_initialize (void);


/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (BYTE pdrv)				// pdrv - Physical drive nmuber to identify the drive */
{
	DSTATUS stat;
	int result;

	switch (pdrv) {
	case DEV_FLASH :
		result = FLASH_disk_initialize();

		// translate the reslut code here

		return stat;
	}
	return STA_NOINIT;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber to identify the drive */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Start sector in LBA */
	UINT count		/* Number of sectors to read */
)
{
	
	FLASH_disk_read(buff, sector, count);

	return RES_OK;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if FF_FS_READONLY == 0

DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber to identify the drive */
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Start sector in LBA */
	UINT count			/* Number of sectors to write */
)
{
		FLASH_disk_write(buff, sector, count);
		return RES_OK;
}

#endif


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	DRESULT res = RES_ERROR;

	switch (cmd)
	{
	case CTRL_SYNC:
		res = RES_OK;
		break;

	case GET_SECTOR_COUNT:
		*(DWORD*)buff = 128;//2048
		res = RES_OK;
		break;

	case GET_SECTOR_SIZE:
		*(DWORD*)buff = 512;
		res = RES_OK;
		break;

	case GET_BLOCK_SIZE:
		*(DWORD*)buff = 512;
		res = RES_OK;
		break;
	}

return res;
}








// Функции заглушки

int FLASH_disk_initialize ()
{
	
	return 0;
}



DWORD get_fattime(){return 0;}



void erase_512b_flash (uint8_t sectorFS)
{
	uint32_t absolute_adr_sectorFS, adr_flash_sector, i;
	uint8_t  temp;
	
	// определим абсолютный адрес блока ФС
	 absolute_adr_sectorFS= sectorFS*512;
	// Определим адрес начала сектора, которому принадлежит этот адресс блока
	adr_flash_sector = absolute_adr_sectorFS & 0x10000;	
	
	
	
	// Стираем сектор, в который поместим копию стираемого сектора
	SPI_1636PP52Y_SectorErase (SPI_1636PP52Y_CS3, Sector1);
	
	// Выполним перенос информации физического сектора на буферный
	for(i = 0; i < 0x10000; i++)
	{
		if( ((adr_flash_sector + i) < absolute_adr_sectorFS) || ((adr_flash_sector + i) >= absolute_adr_sectorFS+512))
		{
			// Читаем байт из памяти где ФС
			SPI_1636PP52Y_ReadArray (SPI_1636PP52Y_CS4, adr_flash_sector + i, &temp, 1);
			// записываем в память - буфер
			SPI_1636PP52Y_ByteProgram (SPI_1636PP52Y_CS3, Sector1+i, temp);
		}
	}
	
  // Стираем сектор и восстанавливаем все кроме блока 512 байт (который к этому моменту уже забит 0xff)
	SPI_1636PP52Y_SectorErase (SPI_1636PP52Y_CS4, adr_flash_sector);
	// Выполним перенос информации физического сектора на буферный
	for(i = 0; i < 0x10000; i++)
	{
			// Читаем байт из памяти где ФС
			SPI_1636PP52Y_ReadArray (SPI_1636PP52Y_CS3, Sector1+i, &temp, 1);
			// записываем в память - буфер
			SPI_1636PP52Y_ByteProgram (SPI_1636PP52Y_CS4, adr_flash_sector + i, temp);
		
		  SPI_1636PP52Y_ReadArray (SPI_1636PP52Y_CS4, adr_flash_sector + i, &temp, 1);
	}
}



void FLASH_disk_read(uint8_t *buff, uint32_t sector, uint8_t count)
{
	uint32_t absolute_adr_sectorFS, adr_flash_sector, i;
	uint8_t  temp;
	
	for(i=0; i<512*count; i++){
	absolute_adr_sectorFS = sector * 512;
	//SPI_1636PP52Y_ReadArray (SPI_1636PP52Y_CS4, absolute_adr_sectorFS, buff, 512*count);
		SPI_1636PP52Y_ReadArray (SPI_1636PP52Y_CS4, absolute_adr_sectorFS, buff+i, 1);
	}
}


void FLASH_disk_write(const BYTE *buff, uint32_t sector, uint8_t count)
{
	uint32_t absolute_adr_sectorFS, adr_flash_sector, i;
	uint8_t  temp;
	
	for(i = 0; i < count; i++)
	{
		erase_512b_flash (sector + 512*i);
	}
	for(i = 0; i < count; i++)
	{
		absolute_adr_sectorFS = (sector + i) * 512;
		SPI_1636PP52Y_BlockProgram (SPI_1636PP52Y_CS4, absolute_adr_sectorFS, (uint8_t*)(buff+(i*512)), 512);
	}
}
