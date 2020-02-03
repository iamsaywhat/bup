#include "Log_FS_llio.h"
#include "25Q64FV.h"
    
/******************************************************************************************
  Приватные функции нижнего уровня
******************************************************************************************/
void MemoryWrite(uint32_t Address, uint8_t* buffer, uint32_t size)
{
  uint32_t i;
  for(i = 0; i < size; i++)
  {
    SPI_25Q64FV_writeByte(SPI_25Q64FV_CSn, Address+i, buffer[i]);
  }
}

void MemoryRead(uint32_t Address, uint8_t* buffer, uint32_t size)
{
  SPI_25Q64FV_readArray (SPI_25Q64FV_CSn, Address, buffer, size);
}

void EraseChip(void)
{
  SPI_25Q64FV_eraseChip(SPI_25Q64FV_CSn);
}

void EraseSector(uint32_t Address)
{
  SPI_25Q64FV_eraseSector (SPI_25Q64FV_CSn, Address);
}
