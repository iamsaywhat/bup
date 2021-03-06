#include "1636pp52y.h"
#include "otherlib.h"
#include "bupboard.h"
#include "MDR32F9Qx_rst_clk.h"
#include "MDR32F9Qx_ssp.h"




// Объединение для работы с четырехбайтовым адресом 
typedef volatile union  
 {
   uint32_t DWord ;    // Доступ к 4-байтному слову	
   float    Float;     // Доступ к вещественному значению (4 байта)  
   uint8_t 	Byte[4];   // Доступ к отдельным байтам
   uint16_t Word[2];   // Доступ к отдельным словам
 } Dword_to_Byte;


 
 
 /**************************************************************************************************************
    Объявления локальных функций модуля
***************************************************************************************************************/
void static SPI_1636PP52Y_writeBlock (uint8_t* Source, uint8_t* Destination, uint32_t Size);


 
 
 
/**************************************************************************************************************
    SPI_1636PP52Y_RetargetPins - Назначение пинов для работы с внещней памятью 1636рр52у по SPI
**************************************************************************************************************/
static void SPI_1636PP52Y_RetargetPins (void)
{
	// Конфигурация SCLK
	Pin_initialize (SPI_1636PP52Y_CLK);
	// Конфигурация MISO
	Pin_initialize (SPI_1636PP52Y_RXD);
	// Конфигурация MOSI
	Pin_initialize (SPI_1636PP52Y_TXD);
	// Конфигурация CS1
	Pin_initialize (SPI_1636PP52Y_CS1);
	// Конфигурация CS2
	Pin_initialize (SPI_1636PP52Y_CS2);
	// Конфигурация CS3
	Pin_initialize (SPI_1636PP52Y_CS3);
	// Конфигурация CS4
	Pin_initialize (SPI_1636PP52Y_CS4);
	// Сбросим все CSn
	CSnDisable(SPI_1636PP52Y_CS1);
	CSnDisable(SPI_1636PP52Y_CS2);
	CSnDisable(SPI_1636PP52Y_CS3);
	CSnDisable(SPI_1636PP52Y_CS4);
}


/**************************************************************************************************************
    SPI_1636PP52Y_initialize - Инициализация SSP в режиме SPI для работы с внешней памятью 1636рр52у 
**************************************************************************************************************/
void SPI_1636PP52Y_initialize (void)
{
	SSP_InitTypeDef SSPInitStructure; 
	
	// Конфигурируем пины 
	SPI_1636PP52Y_RetargetPins();
	// Деинициализация SSP2
	SSP_DeInit (SPI_1636PP52Y_Module);
	// Заполнение структуры по-умолчанию
	SSP_StructInit (&SSPInitStructure);
	// Задать коэффициент деления частоты PCLK для получения частоты F_SSPCLK
	SSP_BRGInit (SPI_1636PP52Y_Module, SSP_HCLKdiv1);
	// Конфигурация SSP (Master - ведущий)
	SSPInitStructure.SSP_Mode = SSP_ModeMaster; 
	// Предделители частоты сигнала SCLK
	// F_SCLK = F_SSPCLK / ( CPSDVR * (1 + SCR) = 80МГц / (5 * (1 + 1)) = 8 МГц 
	SSPInitStructure.SSP_CPSDVSR = 5;  	 // от 2 до 254
	SSPInitStructure.SSP_SCR = 1;          // от 0 до 255
	// Размер кадра 8 бит
	SSPInitStructure.SSP_WordLength = SSP_WordLength8b; 
	// Режим 3 работы SPI
	// При этом FSS устанавливается в низкое состояние на время передачи всего блока данных  
	SSPInitStructure.SSP_SPO = SSP_SPO_High;        // Полярность SCLK
	SSPInitStructure.SSP_SPH = SSP_SPH_2Edge;       // Фаза SCLK (по заднему фронту)
	// Аппаратное управление передачей данных (приемо-передатчик пока отключен) 
	SSPInitStructure.SSP_HardwareFlowControl = SSP_HardwareFlowControl_SSE; 
	// Формат фрейма (Motorola)
	SSPInitStructure.SSP_FRF = SSP_FRF_SPI_Motorola; 	   
	SSP_Init (SPI_1636PP52Y_Module, &SSPInitStructure);
	// Разрешить работу SSP
	SSP_Cmd (SPI_1636PP52Y_Module, ENABLE);	
}



/**************************************************************************************************************
    SPI_1636PP52Y_enableWrite - Отправить запрос на разрешение записи во внешнюю SPI-память    
**************************************************************************************************************/
void SPI_1636PP52Y_enableWrite (PinConfigType CSn)
{
	TimeoutType timeout; 
	
	// Выставим ChipSelect нужной микросхемы
	CSnReady(CSn);
	// Устанавливаем таймаут функции
	setTimeout (&timeout, _1636PP52Y_TIMEOUT);
	// Дождаться полного окончания предыдущих операций
	while ((SPI_1636PP52Y_Module->SR & SSP_FLAG_BSY) && (timeoutStatus(&timeout) != TIME_IS_UP));
	// Передать байт
	SPI_1636PP52Y_Module->DR = _1636PP52Y_CMD_WriteEnable;
	// Дождаться полного окончания операции
	while ((SPI_1636PP52Y_Module->SR & SSP_FLAG_BSY) && (timeoutStatus(&timeout) != TIME_IS_UP));
	// Хоть результат чтения и не нужен, но прочитать всё равно надо
	SPI_1636PP52Y_Module->DR; 
	// Деактивируем ChipSelect микросхемы
	CSnDisable(CSn);
}

/**************************************************************************************************************
    SPI_1636PP52Y_disableWrite - Отправить запрос на запрет записи во внешнюю SPI-память    
**************************************************************************************************************/ 
void SPI_1636PP52Y_disableWrite (PinConfigType CSn)
{
	TimeoutType timeout;
	
	// Выставим ChipSelect нужной микросхемы
	CSnReady(CSn);
	// Устанавливаем таймаут функции
	setTimeout (&timeout, _1636PP52Y_TIMEOUT);
	// Дождаться полного окончания предыдущих операций
	while ((SPI_1636PP52Y_Module->SR & SSP_FLAG_BSY) && (timeoutStatus(&timeout) != TIME_IS_UP));
	// Передать байт
	SPI_1636PP52Y_Module->DR = _1636PP52Y_CMD_WriteDisable;
	// Дождаться полного окончания операции
	while ((SPI_1636PP52Y_Module->SR & SSP_FLAG_BSY) && (timeoutStatus(&timeout) != TIME_IS_UP));
	// Хоть результат чтения и не нужен, но прочитать всё равно надо	
	SPI_1636PP52Y_Module->DR; 
	// Деактивируем ChipSelect микросхемы
	CSnDisable(CSn);
	
}

/**************************************************************************************************************
    SPI_1636PP52Y_eraseChip - Очистка всей микросхемы внешней SPI-памяти    
**************************************************************************************************************/
void SPI_1636PP52Y_eraseChip (PinConfigType CSn)
{
	uint8_t src, dst;
	
	// Снимаем защиту с обоих секторов
	SPI_1636PP52Y_unprotectSector (CSn,Sector0);
	SPI_1636PP52Y_unprotectSector (CSn,Sector1);
	// Разрешаем запись
	SPI_1636PP52Y_enableWrite (CSn);
	
	src = _1636PP52Y_CMD_ChipErase;
	
	// Выставим ChipSelect нужной микросхемы
	CSnReady(CSn);
	// Передать блок данных
	SPI_1636PP52Y_writeBlock (&src, &dst, 1);
	// Деактивируем ChipSelect микросхемы
	CSnDisable(CSn);
	
	// Время операции стирания микросхемы, мс:  110 
	// Ожидание стирания всей памяти (немного с запасом) 
	delay_us (110000);
	
	// Включаем защиту с обоих секторов
	SPI_1636PP52Y_protectSector (CSn,Sector0);
	SPI_1636PP52Y_protectSector (CSn,Sector1);
}



/**************************************************************************************************************
    SPI_1636PP52Y_reset - Перезапуск микросхемы внешней SPI-памяти    
**************************************************************************************************************/
void SPI_1636PP52Y_reset (PinConfigType CSn)
{
	uint8_t src[2], dst[2];
	
	src[0] = _1636PP52Y_CMD_Reset;
	src[1] = 0xD0; 	// Подтверждающий байт
	
	// Выставим ChipSelect нужной микросхемы
	CSnReady(CSn);
	// Передать блок данных
	SPI_1636PP52Y_writeBlock (src, dst, 2);
	// Деактивируем ChipSelect микросхемы
	CSnDisable(CSn);
}	


/**************************************************************************************************************
    SPI_1636PP52Y_readID - Запрос ID микросхемы и производителя внешней SPI-памяти    
**************************************************************************************************************/
uint16_t SPI_1636PP52Y_readID (PinConfigType CSn)
{
	Dword_to_Byte result;
	uint8_t src[3], dst[3];
	
	src[0] = _1636PP52Y_CMD_ReadID;
	src[1] = 0x00;
	src[2] = 0x00;
	
	// Выставим ChipSelect нужной микросхемы
	CSnReady(CSn);
	// Передаём блок данных
	SPI_1636PP52Y_writeBlock (src, dst, 3);
	// Деактивируем ChipSelect микросхемы
	CSnDisable(CSn);
	
	// Формируем результат
	result.DWord = 0;
	result.Byte[0] = dst[1];	// Код производителя 0x01
	result.Byte[1] = dst[2];	// Код микросхемы 0xC8

	return (uint16_t)result.DWord;
}


/**************************************************************************************************************
    SPI_1636PP52Y_SectorErase - Стирание сектора внешней SPI-памяти (всего их 2)    
**************************************************************************************************************/
void SPI_1636PP52Y_eraseSector (PinConfigType CSn, uint32_t Address)
{
	Dword_to_Byte Addr;
	uint8_t src[4], dst[4];   // Посылаем всего 4 байта
	
	Addr.DWord = Address;
	src[0] = _1636PP52Y_CMD_SectorErase;
	src[1] = Addr.Byte[2];    // Старшим байтом вперед.
	src[2] = Addr.Byte[1];
	src[3] = Addr.Byte[0];
	
	// Снимаем защиту сектора
	SPI_1636PP52Y_unprotectSector (CSn,Address);
	
	// Разрешить запись
	SPI_1636PP52Y_enableWrite (CSn);	
	
	// Выставим ChipSelect нужной микросхемы
	CSnReady(CSn);
	// Передать блок данных
	SPI_1636PP52Y_writeBlock (src, dst, 4);
	// Деактивируем ChipSelect микросхемы
	CSnDisable(CSn);
	
	// Время стирания сектора, мс:  55
	// Ожидание стирания сектора (немного с запасом)
	delay_us(55000);
	
	// Включаем защиту сектора
	SPI_1636PP52Y_unprotectSector (CSn,Address);
}



/**************************************************************************************************************
    SPI_1636PP52Y_protectSector - Включение защиты сектора внешней SPI-памяти (всего их 2)    
**************************************************************************************************************/
void SPI_1636PP52Y_protectSector (PinConfigType CSn, uint32_t Address)
{
	uint8_t src[4], dst[4];
	Dword_to_Byte Addr;
	
	Addr.DWord = Address;
	src[0] = _1636PP52Y_CMD_ProtectSector;
	src[1] = Addr.Byte[2];
	src[2] = Addr.Byte[1];
	src[3] = Addr.Byte[0];
	// Разрешить запись
	SPI_1636PP52Y_enableWrite (CSn);

	// Выставим ChipSelect нужной микросхемы
	CSnReady(CSn);
	// Передать блок данных
	SPI_1636PP52Y_writeBlock (src, dst, 4);
	// Деактивируем ChipSelect микросхемы
	CSnDisable(CSn);
}



/**************************************************************************************************************
    SPI_1636PP52Y_unprotectSector - Отключение защиты сектора внешней SPI-памяти (всего их 2)    
**************************************************************************************************************/
void SPI_1636PP52Y_unprotectSector (PinConfigType CSn, uint32_t Address)
{
	uint8_t src[4], dst[4];
	Dword_to_Byte Addr;
	
	Addr.DWord = Address;
	src[0] = _1636PP52Y_CMD_UnprotectSector;
	src[1] = Addr.Byte[2];
	src[2] = Addr.Byte[1];
	src[3] = Addr.Byte[0];
	
	// Разрешить запись
	SPI_1636PP52Y_enableWrite (CSn);	

	// Выставим ChipSelect нужной микросхемы
	CSnReady(CSn);	
	// Передать блок данных
	SPI_1636PP52Y_writeBlock (src, dst, 4);
	// Деактивируем ChipSelect микросхемы
	CSnDisable(CSn);
}


/**************************************************************************************************************
    SPI_1636PP52Y_readSectorProtectionRegister - Функция для определения включена ли защита сектора    
**************************************************************************************************************/
uint32_t SPI_1636PP52Y_readSectorProtectionRegister (PinConfigType CSn, uint32_t Address)
{
	Dword_to_Byte result;
	uint8_t src[5], dst[5];
	Dword_to_Byte Addr;
	
	Addr.DWord = Address;
	src[0] = _1636PP52Y_CMD_ReadSectorProtectionRegister;
	src[1] = Addr.Byte[2];
	src[2] = Addr.Byte[1];
	src[3] = Addr.Byte[0];
	src[4] = 0;
	
	// Выставим ChipSelect нужной микросхемы
	CSnReady(CSn);
	// Передать блок данных
	SPI_1636PP52Y_writeBlock (src, dst, 5);
	// Деактивируем ChipSelect микросхемы
	CSnDisable(CSn);
	// Формирование результатов
	result.DWord = 0;
	result.Byte[0] = dst[4];	
	
	return result.DWord;
}

/**************************************************************************************************************
    SPI_1636PP52Y_readStatusRegister - Функция чтения регистра статуса
**************************************************************************************************************/
uint32_t SPI_1636PP52Y_readStatusRegister (PinConfigType CSn)
{
	Dword_to_Byte result;
	uint8_t src[2], dst[2];
	
	src[0] =_1636PP52Y_CMD_ReadStatusRegister;
	
	// Выставим ChipSelect нужной микросхемы
	CSnReady(CSn);
	// Передать блок данных
	SPI_1636PP52Y_writeBlock (src, dst, 2);
	// Деактивируем ChipSelect микросхемы
	CSnDisable(CSn);
	// Формирование результата
	result.DWord = 0;
	result.Byte[0] = dst[1];	
	
	return result.DWord;
}

/**************************************************************************************************************
    SPI_1636PP52Y_writeStatusRegister - Функция записи в регистр статуса
**************************************************************************************************************/
void SPI_1636PP52Y_writeStatusRegister (PinConfigType CSn, uint8_t status)
{
	uint8_t src[2], dst[2];
	
	src[0] = _1636PP52Y_CMD_WriteStatusRegister;
	src[1] = status & 0xC0; // Можно устанавливать только два старших бита
	// Разрешить запись
	SPI_1636PP52Y_enableWrite (CSn);		
	
	// Выставим ChipSelect нужной микросхемы
	CSnReady(CSn);
	// Передать блок данных
	SPI_1636PP52Y_writeBlock (src, dst, 2);
	// Деактивируем ChipSelect микросхемы
	CSnDisable(CSn);
}



/**************************************************************************************************************
    SPI_1636PP52Y_writeByte - Запись байта во внешнюю память
**************************************************************************************************************/
void SPI_1636PP52Y_writeByte (PinConfigType CSn, uint32_t Address, uint8_t Value)
{
	uint8_t src[5], dst[5];
	Dword_to_Byte Addr;
	Addr.DWord = Address;
	
	src[0] = _1636PP52Y_CMD_ByteProgram;
	src[1] = Addr.Byte[2];
	src[2] = Addr.Byte[1];
	src[3] = Addr.Byte[0];
	src[4] = Value;
	
	// Снимаем защиту сектора
	SPI_1636PP52Y_unprotectSector (CSn,Address);
	
	// Разрешить запись
	SPI_1636PP52Y_enableWrite (CSn);	
	
	// Выставим ChipSelect нужной микросхемы
	CSnReady(CSn);
	// Передать блок данных
	SPI_1636PP52Y_writeBlock (src, dst, 5);
	// Деактивируем ChipSelect микросхемы
	CSnDisable(CSn);
	
	// Время операции программирования байта, мкс:  45 
	// Ожидание окончания записи байта 
	delay_us (45);
	
	// Включаем защиту сектора
	SPI_1636PP52Y_protectSector (CSn,Address);
}


/**************************************************************************************************************
    SPI_1636PP52Y_readWord - Чтение 32 разрядного слова из внешней SPI-памяти
**************************************************************************************************************/
uint32_t SPI_1636PP52Y_readWord (PinConfigType CSn, uint32_t Address)
{
	Dword_to_Byte result;
	Dword_to_Byte Addr;

	uint8_t src[4];
	uint32_t i;
	volatile uint32_t data;
	uint32_t timeout = 0;
	
	Addr.DWord = Address;
	src[0] = 0x03;
	src[1] = Addr.Byte[2];
	src[2] = Addr.Byte[1];
	src[3] = Addr.Byte[0];
	
	// Выставим ChipSelect нужной микросхемы
	CSnReady(CSn);
	
	// Дождаться полного освобождения SPI
	while (((SPI_1636PP52Y_Module->SR) & SSP_FLAG_BSY)  && (timeout != _1636PP52Y_TIMEOUT)) timeout ++;
	// Вычитать все данные из входного буфера, так как там может лежать мусор	
	while ((SPI_1636PP52Y_Module->SR & SSP_FLAG_RNE))
		data = SPI_1636PP52Y_Module->DR;	
	// Передаем 4 байта с кодом команды и адресом
	for (i = 0; i < 4; i++)
		SPI_1636PP52Y_Module->DR = src[i];
	// Передать 4 пустых байта 
	for (i = 0; i < 4; i++)
		SPI_1636PP52Y_Module->DR = 0;
	// Читаем ответные 4 байта, хоть они нам и не нужны
	for (i = 0; i < 4; i++)
	{
		// Дождаться приема байта
		while (!(SPI_1636PP52Y_Module->SR & SSP_FLAG_RNE) && (timeout != _1636PP52Y_TIMEOUT)) timeout ++;
		SPI_1636PP52Y_Module->DR;
	}
	// Читаем и сохраняем 4 полезных ответных байта
	for (i = 0; i < 4; i++)
	{
		// Дождаться приема байта
		while (!(SPI_1636PP52Y_Module->SR & SSP_FLAG_RNE) && (timeout != _1636PP52Y_TIMEOUT)) timeout ++;
		result.Byte[i] = SPI_1636PP52Y_Module->DR;
	}

	// Деактивируем ChipSelect микросхемы
	CSnDisable(CSn);
	
	return result.DWord;	
}

/**************************************************************************************************************
    SPI_1636PP52Y_readArray15 - Чтение массива данных из внешней SPI-памяти	(частота до 15 MHz)
**************************************************************************************************************/
uint32_t SPI_1636PP52Y_readArray15 (PinConfigType CSn, uint32_t Address, uint8_t* Destination, uint32_t Size)
{
	uint8_t           src[4];
	Dword_to_Byte     Addr;
	uint32_t          i,count;
	volatile uint32_t data;
	uint8_t*          Dst;
	TimeoutType       timeout;
	
	Dst = Destination;
	Addr.DWord = Address;
	
	src[0] = 0x03;
	src[1] = Addr.Byte[2];
	src[2] = Addr.Byte[1];
	src[3] = Addr.Byte[0];
	
	// Выставим ChipSelect нужной микросхемы
	CSnReady(CSn);
	// Устанавливаем таймаут функции
	setTimeout (&timeout, _1636PP52Y_TIMEOUT);
	// Дождаться полного освобождения SPI
	while (((SPI_1636PP52Y_Module->SR) & SSP_FLAG_BSY) && (timeoutStatus(&timeout) != TIME_IS_UP));
	// Вычитать все данные из входного буфера, так как там может лежать мусор	
	while ((SPI_1636PP52Y_Module->SR & SSP_FLAG_RNE))
		data = SPI_1636PP52Y_Module->DR;	
	// Передать 4 байта с кодом команды и адресом
	for (i = 0; i < 4; i++)
		SPI_1636PP52Y_Module->DR = src[i];
	// Читаем ответные 4 байта, хоть они нам и не нужны
	for (i = 0; i < 4; i++)
	{
		// Дождаться приема байта
		while (!(SPI_1636PP52Y_Module->SR & SSP_FLAG_RNE) && (timeoutStatus(&timeout) != TIME_IS_UP));
		data = SPI_1636PP52Y_Module->DR;
	}
	// Передаем пустой байт и принимаем ответный
	for (i = 0, count = 0; i < Size; i++)
	{
		// Дождаться появления свободного места в выходном буфере
		while (!(SPI_1636PP52Y_Module->SR & SSP_FLAG_TNF) && (timeoutStatus(&timeout) != TIME_IS_UP))
		{
	    // Читаем байт, полученный от 1636РР52У
		  if (SPI_1636PP52Y_Module->SR & SSP_FLAG_RNE)
			{
	     *(Dst++) = SPI_1636PP52Y_Module->DR;					
			 count++;	
			}
		}
		// Передаем пустой байт
		SPI_1636PP52Y_Module->DR = 0;		
		// Читаем байт, полученный от 1636РР52У
		if (SPI_1636PP52Y_Module->SR & SSP_FLAG_RNE)
		{
		 *(Dst++) = SPI_1636PP52Y_Module->DR;					
		 count++;	
		}		
	}
	// Читаем остаток еще не принятых байт	
	while (SPI_1636PP52Y_Module->SR & (SSP_FLAG_RNE | SSP_FLAG_BSY))
	{
		if ((SPI_1636PP52Y_Module->SR & SSP_FLAG_RNE) && count < Size)
		{
		 *(Dst++) = SPI_1636PP52Y_Module->DR;					
		 count++;
		}
	}
	// Деактивируем ChipSelect микросхемы
	CSnDisable(CSn)
	
	// Сколько байт прочитали
	return count; 
}


/**************************************************************************************************************
    SPI_1636PP52Y_readArray - Чтение массива данных из внешней SPI-памяти	(частота до 100 MHz)
**************************************************************************************************************/
uint32_t SPI_1636PP52Y_readArray (PinConfigType CSn, uint32_t Address, uint8_t* Destination, uint32_t Size)
{
	uint8_t            src[5];
	Dword_to_Byte      Addr;
	uint32_t           i,count;
	volatile uint32_t  data;
	uint8_t*           Dst;
	TimeoutType        timeout;
	
	Dst = Destination;
	Addr.DWord = Address;
	src[0] = _1636PP52Y_CMD_ReadArray;
	src[1] = Addr.Byte[2];
	src[2] = Addr.Byte[1];
	src[3] = Addr.Byte[0];
	src[4] = 0;  // Фиктивный байт
	
	// Выставим ChipSelect нужной микросхемы
	CSnReady(CSn);
	// Устанавливаем таймаут функции
	setTimeout (&timeout, _1636PP52Y_TIMEOUT);
	// Дождаться полного освобождения SPI
	while (((SPI_1636PP52Y_Module->SR) & SSP_FLAG_BSY) && (timeoutStatus(&timeout) != TIME_IS_UP));
	// Вычитать все данные из входного буфера, так как там может лежать мусор	
	while ((SPI_1636PP52Y_Module->SR & SSP_FLAG_RNE))
		data = SPI_1636PP52Y_Module->DR;	
	// Передать 4 байта с кодом команды и адресом
	for (i = 0; i < 5; i++)
		SPI_1636PP52Y_Module->DR = src[i];
	// Читаем ответные 4 байта, хоть они нам и не нужны
	for (i = 0; i < 5; i++)
	{
		// Дождаться приема байта
		while (!(SPI_1636PP52Y_Module->SR & SSP_FLAG_RNE) && (timeoutStatus(&timeout) != TIME_IS_UP));
		data = SPI_1636PP52Y_Module->DR;
	}
	
	// Передаем пустой байт и принимаем ответный
	for (i = 0, count = 0; i < Size; i++)
	{
		// Дождаться появления свободного места в выходном буфере
		while (!(SPI_1636PP52Y_Module->SR & SSP_FLAG_TNF) && (timeoutStatus(&timeout) != TIME_IS_UP))
		{
	    // Читаем байт, полученный от 1636РР52У
		  if (SPI_1636PP52Y_Module->SR & SSP_FLAG_RNE)
			{
	     *(Dst++) = SPI_1636PP52Y_Module->DR;					
			 count++;	
			}
		}
		// Передаем пустой байт
		SPI_1636PP52Y_Module->DR = 0;		
		// Читаем байт, полученный от 1636РР52У
		if (SPI_1636PP52Y_Module->SR & SSP_FLAG_RNE)
		{
		 *(Dst++) = SPI_1636PP52Y_Module->DR;					
		 count++;	
		}		
	}
	// Читаем остаток еще не принятых байт	
	while (SPI_1636PP52Y_Module->SR & (SSP_FLAG_RNE | SSP_FLAG_BSY))
	{
		if ((SPI_1636PP52Y_Module->SR & SSP_FLAG_RNE) && count < Size)
		{
		 *(Dst++) = SPI_1636PP52Y_Module->DR;					
		 count++;
		}
	}
	
	// Деактивируем ChipSelect микросхемы
	CSnDisable(CSn);
	
	// Сколько байт прочитали
	return count; 
}


/**************************************************************************************************************
    SPI_1636PP52Y_writeArray - Запись массива данных во внешнюю SPI-память
**************************************************************************************************************/
void SPI_1636PP52Y_writeArray (PinConfigType CSn, uint32_t Address, uint8_t* Source, uint32_t Size)
{
	uint8_t src[5], dst[5];
	Dword_to_Byte Addr;
	uint32_t i;

	Addr.DWord = Address;
	src[0] = _1636PP52Y_CMD_ByteProgram;
	
	// Снимаем защиту с обоих секторов
	SPI_1636PP52Y_unprotectSector (CSn,Sector0);
	SPI_1636PP52Y_unprotectSector (CSn,Sector1);
	
	// Записать все байты блока
	for (i = 0; i < Size; i++, Addr.DWord++)
	{
		src[1] = Addr.Byte[2];
		src[2] = Addr.Byte[1];
		src[3] = Addr.Byte[0];
		src[4] = Source[i];
		
		// Разрешить запись
		SPI_1636PP52Y_enableWrite (CSn);	
		
		// Выставим ChipSelect нужной микросхемы
		CSnReady(CSn);
		// Передать блок данных
		SPI_1636PP52Y_writeBlock (src, dst, 5);
		// Деактивируем ChipSelect микросхемы
		CSnDisable(CSn);
		
		// Время операции программирования байта, мкс:  50 
		delay_us(50);
	}
	// Включаем защиту с обоих секторов
	SPI_1636PP52Y_protectSector (CSn,Sector0);
	SPI_1636PP52Y_protectSector (CSn,Sector1);
}

/**************************************************************************************************************
    SPI_1636PP52Y_memoryTest - Функция тестирования микросхемы памяти. Проиводит запись и чтение
                               по всем доступным адресам
**************************************************************************************************************/
uint8_t SPI_1636PP52Y_memoryTest (PinConfigType CSn)
{
	uint8_t  temp[2];
	uint32_t i;
	
	SPI_1636PP52Y_reset (CSn);
	// Проверка ID
	if(SPI_1636PP52Y_readID(CSn) != _1636PP52Y_ID) return 0;
	// Очищаем память перед тестом
	SPI_1636PP52Y_eraseChip (CSn);
	// Проверка на стирание и запись
	for(i = 0; i < 0x20000; i++)
	{
		// Читаеи i-й адрес на предмет чистой ячейки
		SPI_1636PP52Y_readArray (CSn, i, temp, 1);
		if(temp[0] != 0xFF) return 0;
		// Записываем номер ячейки в ячейку
		temp[0] = (uint8_t)i;
		SPI_1636PP52Y_writeArray (CSn, i, temp, 1);
		// Проверяем на соответствие записанного
		SPI_1636PP52Y_readArray (CSn, i, temp, 1);
		if(temp[0] != (uint8_t)i) 
			return 0;
	}
	return 1;
}


/**************************************************************************************************************
    SPI_1636PP52Y_writeBlock - Внутреняя функция отправки сообщений по SPI
    Параметры:
                Source      - Указатель на буфер источника;
                Destination - Указатель на буфер места назначения;
                Size		- Размер передаваемомой посылки в байтах
    Возвращает: NONE	
**************************************************************************************************************/
static void SPI_1636PP52Y_writeBlock (uint8_t* Source, uint8_t* Destination, uint32_t Size)
{
	uint32_t     i;
	uint32_t     temp;
	TimeoutType  timeout;
	
	// Устанавливаем таймаут функции
	setTimeout (&timeout, _1636PP52Y_TIMEOUT);
	
	// Дождаемся полного освобождения буфера
	while ((SPI_1636PP52Y_Module->SR & SSP_FLAG_BSY) && (timeoutStatus(&timeout) != TIME_IS_UP));

	// Читаем входной буфер, чтобы его вычистить, так как там может лежать мусор	
	while ((SPI_1636PP52Y_Module->SR & SSP_FLAG_RNE))
		temp = SPI_1636PP52Y_Module->DR;
	
	for (i = 0; i < Size; i++)
	{
		// Дождаться появления свободного места в буфера передатчика
		while (!(SPI_1636PP52Y_Module->SR & SSP_FLAG_TNF) && (timeoutStatus(&timeout) != TIME_IS_UP));
		// Передать байт
		SPI_1636PP52Y_Module->DR = *Source++;
	}
	for (i = 0; i < Size; i++)
	{
		// Дождаться приема байта
		while (!(SPI_1636PP52Y_Module->SR & SSP_FLAG_RNE) && (timeoutStatus(&timeout) != TIME_IS_UP));
		// Читаем байт, полученный от 1636РР52У
		*Destination++ = SPI_1636PP52Y_Module->DR;		
	}
}
