/*****************************************************************************************************************

        1636pp52y.c - Драйвер для работы с внешней SPI памятью типа 1636рр52у от Миландр
    
*****************************************************************************************************************/
#ifndef _1636PP52Y_H_
#define _1636PP52Y_H_

#include "stdint.h"
#include "bupboard.h"	
	

// Адреса, начала каждого из секторов памяти (используеются как идентификатор сектора)
#define Sector0                  0x0
#define Sector1                  0x10000
#define _1636PP52Y_MAX_ADR       0x1FFFF   // Последний адрес микросхемы
#define _1636PP52Y_ID            0xC801    // ID микросхемы и производителя
#define _1636PP52Y_TIMEOUT       1         // Таймаут на чтение по SPI, мс

// Список поддерживаемых команд для 1636РР52У
enum _1636PP52Y_CMD
{
  _1636PP52Y_CMD_ReadArray                         =  0x0B,    // Чтение массива данных
  _1636PP52Y_CMD_SectorErase                       =  0xD8,    // Стирание сектора	
  _1636PP52Y_CMD_ChipErase                         =  0x60,    // Стирание всей микросхемы
  _1636PP52Y_CMD_ByteProgram                       =  0x02,    // Программирование байта
  _1636PP52Y_CMD_WriteEnable                       =  0x06,    // Разрешение записи
  _1636PP52Y_CMD_WriteDisable                      =  0x04,    // Запрет записи
  _1636PP52Y_CMD_ProtectSector                     =  0x36,    // Включение защиты сектора
  _1636PP52Y_CMD_UnprotectSector                   =  0x39,    // Отключение защиты сектора
  _1636PP52Y_CMD_ReadSectorProtectionRegister      =  0x3C,    // Чтение регистра защиты сектора 
  _1636PP52Y_CMD_ReadStatusRegister                =  0x05,    // Чтение регистра состояния
  _1636PP52Y_CMD_WriteStatusRegister               =  0x01,    // Запись в регистр состония
  _1636PP52Y_CMD_Reset                             =  0xF0,    // Ресет микросхемы
  _1636PP52Y_CMD_ReadID                            =  0x9F,    // Чтение ID производителя и микросхемы
};

 
// Макросы для выбора микросхемы памяти (0 = выбран, 1 == не выбран)
// Использовать так: CSnReady(SPI_1636PP52Y_CS1);
#define CSnReady(x)          Pin_reset(x);    // Выбор микросхемы
#define CSnDisable(x)        Pin_set(x);      // Отмена
 
 
/************************************************************************************************************** 
  SPI_1636PP52Y_Init - Инициализация SSP в режиме SPI для работы с внешней памятью 1636рр52у
  Параметры:
            NONE    
**************************************************************************************************************/
void SPI_1636PP52Y_initialize (void);


/**************************************************************************************************************
  SPI_1636PP52Y_enableWrite - Отправить запрос на разрешение записи во внешнюю SPI-память
  Параметры:
            CSn - Выбор микросхемы
**************************************************************************************************************/
void SPI_1636PP52Y_enableWrite (PinConfigType CSn);
 
 
/**************************************************************************************************************
  SPI_1636PP52Y_disableWrite - Отправить запрос на запрет записи во внешнюю SPI-память
  Параметры:
            CSn - Выбор микросхемы
**************************************************************************************************************/
void SPI_1636PP52Y_disableWrite (PinConfigType CSn);


/**************************************************************************************************************
  SPI_1636PP52Y_eraseChip - Очистка всей микросхемы внешней SPI-памяти
  Параметры:
            CSn - Выбор микросхемы
**************************************************************************************************************/
void SPI_1636PP52Y_eraseChip (PinConfigType CSn);


/**************************************************************************************************************
  SPI_1636PP52Y_reset - Перезапуск микросхемы внешней SPI-памяти
  Параметры:
            CSn - Выбор микросхемы	
**************************************************************************************************************/
void SPI_1636PP52Y_reset (PinConfigType CSn);


/**************************************************************************************************************
  SPI_1636PP52Y_readID - Запрос ID микросхемы и производителя внешней SPI-памяти
  Параметры:
            CSn - Выбор микросхемы.
  Возвращает: Два байта, где младший код производителя 0x01,
              а старший код микросхемы 0xC8	
**************************************************************************************************************/
uint16_t SPI_1636PP52Y_readID (PinConfigType CSn);


/**************************************************************************************************************
  SPI_1636PP52Y_eraseSector - Стирание сектора внешней SPI-памяти (всего их 2)
  Параметры:
            CSn     - Выбор микросхемы;
            Address - Адрес, который однозначно принадлежит стираемому сектору;
  Возвращает: 
            NONE
**************************************************************************************************************/
void SPI_1636PP52Y_eraseSector (PinConfigType CSn, uint32_t Address);


/**************************************************************************************************************
  SPI_1636PP52Y_protectSector - Включение защиты сектора внешней SPI-памяти (всего их 2)
  Параметры:
            CSn     - Выбор микросхемы;
            Address - Адрес, который однозначно принадлежит защищаемому сектору;
  Возвращает: 
            NONE
**************************************************************************************************************/
void SPI_1636PP52Y_protectSector (PinConfigType CSn, uint32_t Address);


/**************************************************************************************************************
  SPI_1636PP52Y_unprotectSector - Отключение защиты сектора внешней SPI-памяти (всего их 2)    
  Параметры:    
            CSn     - Выбор микросхемы;
            Address - Адрес, который однозначно принадлежит нужному сектору;    
  Возвращает:
            NONE
**************************************************************************************************************/
void SPI_1636PP52Y_unprotectSector (PinConfigType CSn, uint32_t Address);


/**************************************************************************************************************
  SPI_1636PP52Y_readSectorProtectionRegister - Функция для определения включена ли защита сектора
                                              (всего их 2) 
  Параметры:
            CSn     - Выбор микросхемы;
            Address - Адрес, который однозначно принадлежит нужному сектору;
  Возвращает:
            00h - Cектор не защищен;
            FFh - Cектор не защищен;
**************************************************************************************************************/
uint32_t SPI_1636PP52Y_readSectorProtectionRegister (PinConfigType CSn, uint32_t Address);


/**************************************************************************************************************
  SPI_1636PP52Y_readStatusRegister - Функция чтения регистра статуса
  Параметры:
            CSn - Выбор микросхемы.
  Возвращает:
            Значение регистра статуса микросхемы;
**************************************************************************************************************/
uint32_t SPI_1636PP52Y_readStatusRegister (PinConfigType CSn);


/**************************************************************************************************************
  SPI_1636PP52Y_writeStatusRegister - Функция записи в регистр статуса
  Параметры:
            CSn    - Выбор микросхемы;
            status - Комбинация битов RSTE и SPRL   (см.даташит)
  Возвращает:
            NONE
**************************************************************************************************************/
void SPI_1636PP52Y_writeStatusRegister (PinConfigType CSn, uint8_t status);										


/**************************************************************************************************************
  SPI_1636PP52Y_writeByte - Запись байта во внешнюю память
  Параметры:
            CSn     - Выбор микросхемы;
            Address - Адрес места записи;
            Value   - Значение записываемого байта информации.
  Возвращает:
            NONE;	
**************************************************************************************************************/
void SPI_1636PP52Y_writeByte (PinConfigType CSn, uint32_t Address, uint8_t Value);



/**************************************************************************************************************
  SPI_1636PP52Y_readWord - Чтение 32 разрядного слова из внешней SPI-памяти
  Параметры:
            CSn     - Выбор микросхемы;
            Address - Адрес места чтения
  Возвращает:
            Считанный байт информации;	
**************************************************************************************************************/
uint32_t SPI_1636PP52Y_readWord (PinConfigType CSn, uint32_t Address);


/**************************************************************************************************************
  SPI_1636PP52Y_readArray15 - Чтение массива данных из внешней SPI-памяти	(частота до 15 MHz)
  Параметры:
            CSn         - Выбор микросхемы;
            Address     - Адрес места чтения;
            Destination - Указатель на буфер места назначения;
            Size        - Размер читаемой области памяти в байтах;
  Возвращает:
            Количество считанных байт;	
**************************************************************************************************************/
uint32_t SPI_1636PP52Y_readArray15 (PinConfigType CSn, uint32_t Address, uint8_t* Destination, uint32_t Size);


/**************************************************************************************************************
  SPI_1636PP52Y_readArray - Чтение массива данных из внешней SPI-памяти	(частота до 100 MHz)
  Параметры:
            CSn         - Выбор микросхемы;
            Address     - Адрес места чтения;
            Destination - Указатель на буфер места назначения;
            Size        - Размер читаемой области памяти в байтах;
  Возвращает:
            Количество считанных байт;	
**************************************************************************************************************/
uint32_t SPI_1636PP52Y_readArray (PinConfigType CSn, uint32_t Address, uint8_t* Destination, uint32_t Size);


/**************************************************************************************************************
  SPI_1636PP52Y_writeArray - Запись массива данных во внешнюю SPI-память
  Параметры:
            CSn         - Выбор микросхемы;
            Address     - Адрес места записи;
            Destination - Указатель на буфер источника;
            Size        - Размер записываемой области памяти в байтах;
  Возвращает:
            Количество считанных байт;
**************************************************************************************************************/
void SPI_1636PP52Y_writeArray (PinConfigType CSn, uint32_t Address, uint8_t* Source, uint32_t Size);


/**************************************************************************************************************
  SPI_1636PP52Y_memoryTest - Функция тестирования микросхемы памяти. Проиводит запись и чтение
                             по всем доступным адресам
  Параметры: 
            CSn - Выбор микросхемы	
  Возвращает:
            Значение регистра статуса микросхемы;
**************************************************************************************************************/
uint8_t SPI_1636PP52Y_memoryTest (PinConfigType CSn); 
 
#endif 
	
