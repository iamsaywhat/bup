/*****************************************************************************************************************

        1636PP52Y.c - Драйвер для работы с внешней SPI памятью типа 1636рр52у от Миландр
    
        Особенности работы с драйвером:
    
        В данном драйвере реализована поддержка 4х микросхем, выбор которых производится 
    
*****************************************************************************************************************/
#ifndef _1636PP52Y_H_
#define _1636PP52Y_H_
	
#include "MDR32F9Qx_port.h"	
#include "stdint.h"

	
#define SPI_1636PP52Y_Module                 MDR_SSP1
#define RST_CLK_PCLK_SPI_1636PP52Y_Port      RST_CLK_PCLK_PORTB
#define RST_CLK_PCLK_SPI_1636PP52Y_Module    RST_CLK_PCLK_SSP1

// ChipSelect n - выбор микросхемы № n
#define SPI_1636PP52Y_CS1                    PORT_Pin_8          // ChipSelect 1 - выбор микросхемы 1
#define SPI_1636PP52Y_CS2                    PORT_Pin_9          // ChipSelect 2 - выбор микросхемы 2
#define SPI_1636PP52Y_CS3                    PORT_Pin_10         // ChipSelect 3 - выбор микросхемы 3
#define SPI_1636PP52Y_CS4                    PORT_Pin_11         // ChipSelect 4 - выбор микросхемы 4
#define SPI_1636PP52Y_CSn_PORT               MDR_PORTB           // Используем порт B под CS
#define SPI_1636PP52Y_CSn_FUNC               PORT_FUNC_PORT      // Функция пинов - просто порт (выход)

// SCLK - последовательный тактовый сигнал шины SPI
#define SPI_1636PP52Y_CLK                    PORT_Pin_13					
#define SPI_1636PP52Y_CLK_PORT               MDR_PORTB           // Используем порт B
#define SPI_1636PP52Y_CLK_FUNC               PORT_FUNC_ALTER     // Функция пинов - альтернативнаая: SSP1_CLK

// MISO - вход для мастера, выход для ведомого
#define SPI_1636PP52Y_RXD                    PORT_Pin_14	
#define SPI_1636PP52Y_RXD_PORT               MDR_PORTB           // Используем порт B
#define SPI_1636PP52Y_RXD_FUNC               PORT_FUNC_ALTER     // Функция пинов - альтернативнаая: SSP1_RXD

// MOSI - выход для мастера, вход для ведомого
#define SPI_1636PP52Y_TXD                    PORT_Pin_15					
#define SPI_1636PP52Y_TXD_PORT               MDR_PORTB           // Используем порт B
#define SPI_1636PP52Y_TXD_FUNC               PORT_FUNC_ALTER     // Функция пинов - альтернативнаая: SSP1_ЕXD

// Адреса, начала каждого из секторов памяти (используеются как идентификатор сектора)
#define Sector0                    0x0
#define Sector1                    0x10000

// Последний адрес микросхемы
#define _1636PP52Y_MAX_ADR       0x1FFFF

// ID микросхемы и производителя
#define _1636PP52Y_ID            0xC801

// Таймаут на чтение по SPI, мс
#define TIMEOUT_1636PP52Y        1

// Список поддерживаемых команд для 1636РР52У
#define _1636PP52Y_CMD_ReadArray                           0x0B    // Чтение массива данных
#define _1636PP52Y_CMD_SectorErase                         0xD8    // Стирание сектора	
#define _1636PP52Y_CMD_ChipErase                           0x60    // Стирание всей микросхемы
#define _1636PP52Y_CMD_ByteProgram                         0x02    // Программирование байта
#define _1636PP52Y_CMD_WriteEnable                         0x06    // Разрешение записи
#define _1636PP52Y_CMD_WriteDisable                        0x04    // Запрет записи
#define _1636PP52Y_CMD_ProtectSector                       0x36    // Включение защиты сектора
#define _1636PP52Y_CMD_UnprotectSector                     0x39    // Отключение защиты сектора
#define _1636PP52Y_CMD_ReadSectorProtectionRegister        0x3C    // Чтение регистра защиты сектора 
#define _1636PP52Y_CMD_ReadStatusRegister                  0x05    // Чтение регистра состояния
#define _1636PP52Y_CMD_WriteStatusRegister                 0x01    // Запись в регистр состония
#define _1636PP52Y_CMD_Reset                               0xF0    // Ресет микросхемы
#define _1636PP52Y_CMD_ReadID                              0x9F    // Чтение ID производителя и микросхемы		


 
// Макросы для выбора микросхемы памяти (0 = выбран, 1 == не выбран)
// Использовать так: CSnReady(SPI_1636PP52Y_CS1);
#define CSnReady(x)          PORT_ResetBits (SPI_1636PP52Y_CSn_PORT, x);    // Выбор микросхемы
#define CSnDisable(x)        PORT_SetBits (SPI_1636PP52Y_CSn_PORT, x);      // Отмена
 
/**************************************************************************************************************
    SPI_1636PP52Y_RetargetPins - Назначение пинов для работы с внещней памятью 1636рр52у по SPI
    Параметры:  NONE
**************************************************************************************************************/
void SPI_1636PP52Y_RetargetPins (void);

 
/************************************************************************************************************** 
    SPI_1636PP52Y_Init - Инициализация SSP в режиме SPI для работы с внешней памятью 1636рр52у
    Параметры:  NONE    
**************************************************************************************************************/
void SPI_1636PP52Y_Init (void);


/**************************************************************************************************************
    SPI_1636PP52Y_WriteEnable - Отправить запрос на разрешение записи во внешнюю SPI-память
    Параметры:
                CSn - Выбор микросхемы
**************************************************************************************************************/
void SPI_1636PP52Y_WriteEnable (uint32_t CSn);
 
 
/**************************************************************************************************************
    SPI_1636PP52Y_WriteDisable - Отправить запрос на запрет записи во внешнюю SPI-память
    Параметры:
                CSn - Выбор микросхемы
**************************************************************************************************************/
void SPI_1636PP52Y_WriteDisable (uint32_t CSn);


/**************************************************************************************************************
    SPI_1636PP52Y_ChipErase - Очистка всей микросхемы внешней SPI-памяти
    Параметры:
                CSn - Выбор микросхемы
**************************************************************************************************************/
void SPI_1636PP52Y_ChipErase (uint32_t CSn);


/**************************************************************************************************************
    SPI_1636PP52Y_Reset - Перезапуск микросхемы внешней SPI-памяти
    Параметры:
                CSn - Выбор микросхемы	
**************************************************************************************************************/
void SPI_1636PP52Y_Reset (uint32_t CSn);


/**************************************************************************************************************
    SPI_1636PP52Y_ReadID - Запрос ID микросхемы и производителя внешней SPI-памяти
    Параметры:
                CSn - Выбор микросхемы.
    Возвращает: Два байта, где младший код производителя 0x01,
                а старший код микросхемы 0xC8	
**************************************************************************************************************/
uint16_t SPI_1636PP52Y_ReadID (uint32_t CSn);


/**************************************************************************************************************
    SPI_1636PP52Y_SectorErase - Стирание сектора внешней SPI-памяти (всего их 2)
    Параметры:
                CSn     - Выбор микросхемы;
                Address - Адрес, который однозначно принадлежит стираемому сектору;
    Возвращает: NONE
**************************************************************************************************************/
void SPI_1636PP52Y_SectorErase (uint32_t CSn, uint32_t Address);


/**************************************************************************************************************
    SPI_1636PP52Y_ProtectSector - Включение защиты сектора внешней SPI-памяти (всего их 2)
    Параметры:
                CSn     - Выбор микросхемы;
                Address - Адрес, который однозначно принадлежит защищаемому сектору;
    Возвращает: NONE
**************************************************************************************************************/
void SPI_1636PP52Y_ProtectSector (uint32_t CSn, uint32_t Address);


/**************************************************************************************************************
    SPI_1636PP52Y_UnprotectSector - Отключение защиты сектора внешней SPI-памяти (всего их 2)    
    Параметры:    
                CSn     - Выбор микросхемы;
                Address - Адрес, который однозначно принадлежит нужному сектору;    
    Возвращает: NONE
**************************************************************************************************************/
void SPI_1636PP52Y_UnprotectSector (uint32_t CSn, uint32_t Address);


/**************************************************************************************************************
    SPI_1636PP52Y_ReadSectorProtectionRegister - Функция для определения включена ли защита сектора
                                                (всего их 2) 
    Параметры:
                CSn     - Выбор микросхемы;
                Address - Адрес, который однозначно принадлежит нужному сектору;
    Возвращает:
                00h - Cектор не защищен;
                FFh - Cектор не защищен;
**************************************************************************************************************/
uint32_t SPI_1636PP52Y_ReadSectorProtectionRegister (uint32_t CSn, uint32_t Address);


/**************************************************************************************************************
    SPI_1636PP52Y_ReadStatusRegister - Функция чтения регистра статуса
    Параметры:
                CSn - Выбор микросхемы.
    Возвращает:
                Значение регистра статуса микросхемы;
**************************************************************************************************************/
uint32_t SPI_1636PP52Y_ReadStatusRegister (uint32_t CSn);


/**************************************************************************************************************
    SPI_1636PP52Y_WriteStatusRegister - Функция записи в регистр статуса
    Параметры:
                CSn    - Выбор микросхемы;
                status - Комбинация битов RSTE и SPRL   (см.даташит)
    Возвращает:
                NONE
**************************************************************************************************************/
void SPI_1636PP52Y_WriteStatusRegister (uint32_t CSn, uint8_t status);										


/**************************************************************************************************************
    SPI_1636PP52Y_Byte_Program - Запись байта во внешнюю память
    Параметры:
                CSn     - Выбор микросхемы;
                Address - Адрес места записи;
                Value   - Значение записываемого байта информации.
    Возвращает:
                NONE;	
**************************************************************************************************************/
void SPI_1636PP52Y_ByteProgram (uint32_t CSn, uint32_t Address, uint8_t Value);



/**************************************************************************************************************
    SPI_1636PP52Y_ReadWord - Чтение 32 разрядного слова из внешней SPI-памяти
    Параметры:
                CSn     - Выбор микросхемы;
                Address - Адрес места чтения
    Возвращает:
                Считанный байт информации;	
**************************************************************************************************************/
uint32_t SPI_1636PP52Y_ReadWord (uint32_t CSn, uint32_t Address);


/**************************************************************************************************************
    SPI_1636PP52Y_ReadArray15 - Чтение массива данных из внешней SPI-памяти	(частота до 15 MHz)
    Параметры:
                CSn         - Выбор микросхемы;
                Address     - Адрес места чтения;
                Destination - Указатель на буфер места назначения;
                Size        - Размер читаемой области памяти в байтах;
    Возвращает:
                Количество считанных байт;	
**************************************************************************************************************/
uint32_t SPI_1636PP52Y_ReadArray15 (uint32_t CSn, uint32_t Address, uint8_t* Destination, uint32_t Size);


/**************************************************************************************************************
    SPI_1636PP52Y_ReadArray - Чтение массива данных из внешней SPI-памяти	(частота до 100 MHz)
    Параметры:
                CSn         - Выбор микросхемы;
                Address     - Адрес места чтения;
                Destination - Указатель на буфер места назначения;
                Size        - Размер читаемой области памяти в байтах;
    Возвращает:
                Количество считанных байт;	
**************************************************************************************************************/
uint32_t SPI_1636PP52Y_ReadArray (uint32_t CSn, uint32_t Address, uint8_t* Destination, uint32_t Size);


/**************************************************************************************************************
    SPI_1636PP52Y_BlockProgram - Запись массива данных во внешнюю SPI-память
    Параметры:
                CSn         - Выбор микросхемы;
                Address     - Адрес места записи;
                Destination - Указатель на буфер источника;
                Size        - Размер записываемой области памяти в байтах;
    Возвращает:
                Количество считанных байт;
**************************************************************************************************************/
void SPI_1636PP52Y_BlockProgram (uint32_t CSn, uint32_t Address, uint8_t* Source, uint32_t Size);


/**************************************************************************************************************
    SPI_1636PP52Y_TestMemory - Функция тестирования микросхемы памяти. Проиводит запись и чтение
                                по всем доступным адресам
    Параметры: 
                CSn - Выбор микросхемы	
    Возвращает:
                Значение регистра статуса микросхемы;
**************************************************************************************************************/
uint8_t SPI_1636PP52Y_TestMemory (uint32_t CSn); 
 
#endif 
	
