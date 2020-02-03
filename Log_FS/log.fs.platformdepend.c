#include "log.fs.platformdepend.h"
#include "25q64fv.h"

/******************************************************************************************
*                         Приватные функции нижнего уровня                                *
******************************************************************************************/
void writeMemory(uint32_t Address, uint8_t* buffer, uint32_t size)
{
  for(uint32_t i = 0; i < size; i++)
  {
    SPI_25Q64FV_writeByte(SPI_25Q64FV_CSn, Address+i, buffer[i]);
  }
}


void readMemory(uint32_t Address, uint8_t* buffer, uint32_t size)
{
  SPI_25Q64FV_readArray (SPI_25Q64FV_CSn, Address, buffer, size);
}

void eraseChip(void)
{
  SPI_25Q64FV_eraseChip(SPI_25Q64FV_CSn);
}

void eraseSector(uint32_t Address)
{
  SPI_25Q64FV_eraseSector (SPI_25Q64FV_CSn, Address);
}
