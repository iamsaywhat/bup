/*****************************************************************************************************************

        25q64fv - Драйвер для работы с внешней SPI памятью типа 25Q64FV от WINBOND.
    
*****************************************************************************************************************/
#ifndef _25Q64FV_H_
#define _25Q64FV_H_
	
#include "stdint.h"
#include "bupboard.h"

// Параметры микросхемы
#define _25Q64FV_SECTORS_NUM               2048          // Количество секторов 
#define _25Q64FV_SECTORS_SIZE              0x1000        // Размер сектора
#define _25Q64FV_ID                        0xEF16        // ID микросхемы и производителя
#define _25Q64FV_TIMEOUT                   1             // Таймаут на чтение по SPI, мс
#define _25Q64FV_ChipErase_delay_us        25000000      // Время стирания всего чипа (от 20с до 100 с)
#define _25Q64FV_SectorErase_delay_us      60000         // Время стирания сектора (от 45 мс до 400 мс)
#define _25Q64FV_ProgramByte_delay_us      50            // Время записи одного байта сектора (от 20мкс до 50мкс)


// Список поддерживаемых команд для 25Q64FV
enum _25Q64FV_CMD
{
  _25Q64FV_CMD_WriteEnable          = 0x06,      // Разрешение записи в файл
  _25Q64FV_CMD_WriteDisable         = 0x04,      // Запрет записи в память
  _25Q64FV_CMD_PageProgram          = 0x02,      // Программирование страницы
  _25Q64FV_CMD_SectorErase          = 0x20,      // Очистка сектора (размером 4 Кб)
  _25Q64FV_CMD_BlockErase32k        = 0x52,      // Очистить блок разрмером 32Кб
  _25Q64FV_CMD_BlockErase64k        = 0xD8,      // Очистить блок разрмером 64Кб
  _25Q64FV_CMD_ChipErase            = 0xC7,      // Полная очистка микросхемы
  _25Q64FV_CMD_ReadData             = 0x03,      // Чтение данных
  _25Q64FV_CMD_FastRead             = 0x0B,      // Чтение данных на повышенной частоте (см. даташит) 
  _25Q64FV_CMD_ManufacturerDeviceID = 0x90,      // Чтение ID производителя и микросхемы	
  _25Q64FV_CMD_EnableReset          = 0x66,      // Разрешение команды сброса
  _25Q64FV_CMD_Reset                = 0x99       // Перезапуск микросхемы
};

 
/**************************************************************************************************************
  SPI_25Q64FV_initialize - Инициализация SSP в режиме SPI для работы с внешней памятью 25Q64FV
  Параметры:
            NONE
**************************************************************************************************************/
void SPI_25Q64FV_initialize (void);


/**************************************************************************************************************
  SPI_25Q64FV_enableWrite - Отправить запрос на разрешение записи во внешнюю SPI-память
  Параметры:
            CSn - Выбор микросхемы
**************************************************************************************************************/
void SPI_25Q64FV_enableWrite (PinConfigType CSn);
 
 
/**************************************************************************************************************
  SPI_25Q64FV_disableWrite - Отправить запрос на запрет записи во внешнюю SPI-память
  Параметры:
            CSn - Выбор микросхемы
**************************************************************************************************************/
void SPI_25Q64FV_disableWrite (PinConfigType CSn);


/**************************************************************************************************************
  SPI_25Q64FV_eraseChip - Очистка всей микросхемы внешней SPI-памяти
  Параметры:
            CSn - Выбор микросхемы
**************************************************************************************************************/
void SPI_25Q64FV_eraseChip (PinConfigType CSn);


/**************************************************************************************************************
  SPI_25Q64FV_reset - Перезапуск микросхемы внешней SPI-памяти
  Параметры:
            CSn - Выбор микросхемы
**************************************************************************************************************/
void SPI_25Q64FV_reset (PinConfigType CSn);


/**************************************************************************************************************
  SPI_25Q64FV_readID - Запрос ID микросхемы и производителя внешней SPI-памяти
  Параметры:
            CSn - Выбор микросхемы
  Возвращает: 
            Два байта, где младший код производителя 0x01,
            а старший код микросхемы 0xC8
**************************************************************************************************************/
uint16_t SPI_25Q64FV_readID (PinConfigType CSn);


/**************************************************************************************************************
  SPI_25Q64FV_eraseSector - Стирание сектора внешней SPI-памяти (всего их 2)
  Параметры:
            CSn - Выбор микросхемы
            Address -  Адрес, который однозначно принадлежит стираемому сектору;
  Возвращает: 
            NONE
**************************************************************************************************************/
void SPI_25Q64FV_eraseSector (PinConfigType CSn, uint32_t Address);


/**************************************************************************************************************
  SPI_25Q64FV_writeByte - Запись байта во внешнюю память
  Параметры:
            CSn     - Выбор микросхемы;
            Address - Адрес места записи;
            Value   - Значение записываемого байта информации;
  Возвращает:
            NONE;
**************************************************************************************************************/
void SPI_25Q64FV_writeByte (PinConfigType CSn, uint32_t Address, uint8_t Value);


/**************************************************************************************************************
  SPI_25Q64FV_readArray - Чтение массива данных из внешней SPI-памяти
  Параметры:
            CSn         - Выбор микросхемы;
            Address     - Адрес места чтения;
            Destination - Указатель на буфер места назначения;
            Size        - Размер читаемой области памяти в байтах;
  Возвращает:
            Количество считанных байт;
**************************************************************************************************************/
uint32_t SPI_25Q64FV_readArray (PinConfigType CSn, uint32_t Address, uint8_t* Destination, uint32_t Size);


/**************************************************************************************************************
  SPI_25Q64FV_memoryTest - Функция тестирования микросхемы памяти. Проиводит запись и чтение
                           по всем доступным адресам
  Параметры:
            CSn - Выбор микросхемы
  Возвращает:
            Значение регистра статуса микросхемы;
**************************************************************************************************************/
uint8_t SPI_25Q64FV_memoryTest (PinConfigType CSn); 
 
 
#endif 
