#include "25q64fv.h"

#include "MDR32F9Qx_rst_clk.h"
#include "MDR32F9Qx_ssp.h"
#include "otherlib.h"



// Макросы для выбора микросхемы памяти (0 = выбран, 1 == не выбран)
// Использовать так: CSnReady(SPI_1636PP52Y_CS1);
#define CSnReady(x)			Pin_reset(x)       // Выбор микросхемы
#define CSnDisable(x)		Pin_set(x)         // Отмена

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
static void  SPI_25Q64FV_writeBlock (uint8_t* Source, uint8_t* Destination, uint32_t Size);
 
 
/**************************************************************************************************************
  SPI_25Q64FV_RetargetPins - Назначение пинов для работы с внешней памятью 25Q64FV по SPI
**************************************************************************************************************/
static void SPI_25Q64FV_RetargetPins (void)
{
	// Конфигурация SCLK
	Pin_initialize (SPI_25Q64FV_CLK);
	// Конфигурация MISO
	Pin_initialize (SPI_25Q64FV_RXD);
	// Конфигурация MOSI
	Pin_initialize (SPI_25Q64FV_TXD);
	// Конфигурация CS
	Pin_initialize (SPI_25Q64FV_CSn);
	// Сбросим чипселект CSn
	CSnDisable(SPI_25Q64FV_CSn);
}


/**************************************************************************************************************
  SPI_25Q64FV_initialize - Инициализация SSP в режиме SPI для работы с внешней памятью 25Q64FV
**************************************************************************************************************/
void SPI_25Q64FV_initialize (void)
{
  SSP_InitTypeDef SSPInitStructure; 
	
  // Конфигурируем пины 
  SPI_25Q64FV_RetargetPins();
  // Деинициализация SSP2
  SSP_DeInit (SPI_25Q64FV_Module);
  // Заполнение структуры по-умолчанию
  SSP_StructInit (&SSPInitStructure);
  // Задать коэффициент деления частоты PCLK для получения частоты F_SSPCLK
  SSP_BRGInit (SPI_25Q64FV_Module, SSP_HCLKdiv1);
  // Конфигурация SSP (Master - ведущий)
  SSPInitStructure.SSP_Mode = SSP_ModeMaster; 
  // Предделители частоты сигнала SCLK
  // F_SCLK = F_SSPCLK / ( CPSDVR * (1 + SCR) = 80МГц / (5 * (1 + 1)) = 8 МГц 
  SSPInitStructure.SSP_CPSDVSR = 5;  	 // от 2 до 254
  SSPInitStructure.SSP_SCR = 1;        // от 0 до 255
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
  SSP_Init (SPI_25Q64FV_Module, &SSPInitStructure);
  // Разрешить работу SSP
  SSP_Cmd (SPI_25Q64FV_Module, ENABLE);	
}



/**************************************************************************************************************
  SPI_25Q64FV_enableWrite - Отправить запрос на разрешение записи во внешнюю SPI-память 
**************************************************************************************************************/
void SPI_25Q64FV_enableWrite (PinConfigType CSn)
{
  TimeoutType timeout;
	
  // Выставим ChipSelect нужной микросхемы
  CSnReady(CSn);
  // Устанавливаем таймаут
  setTimeout (&timeout, _25Q64FV_TIMEOUT);
  // Дождаться полного окончания предыдущих операций
  while ((SPI_25Q64FV_Module->SR & SSP_FLAG_BSY) && (timeoutStatus(&timeout) != TIME_IS_UP));
  // Передать байт
  SPI_25Q64FV_Module->DR = _25Q64FV_CMD_WriteEnable;
  // Дождаться полного окончания операции
  while ((SPI_25Q64FV_Module->SR & SSP_FLAG_BSY) && (timeoutStatus(&timeout) != TIME_IS_UP));
  // Хоть результат чтения и не нужен, но прочитать всё равно надо
  SPI_25Q64FV_Module->DR; 
  // Деактивируем ChipSelect микросхемы
  CSnDisable(CSn);
}

/**************************************************************************************************************
  SPI_25Q64FV_disableWrite - Отправить запрос на запрет записи во внешнюю SPI-память 
**************************************************************************************************************/ 
void SPI_25Q64FV_disableWrite (PinConfigType CSn)
{
  TimeoutType timeout;
	
  // Выставим ChipSelect нужной микросхемы
  CSnReady(CSn);
  // Устанавливаем таймаут
  setTimeout (&timeout, _25Q64FV_TIMEOUT);
  // Дождаться полного окончания предыдущих операций
  while ((SPI_25Q64FV_Module->SR & SSP_FLAG_BSY) && (timeoutStatus(&timeout) != TIME_IS_UP));
  // Передать байт
  SPI_25Q64FV_Module->DR = _25Q64FV_CMD_WriteDisable;
  // Дождаться полного окончания операции
  while ((SPI_25Q64FV_Module->SR & SSP_FLAG_BSY) && (timeoutStatus(&timeout) != TIME_IS_UP));
  // Хоть результат чтения и не нужен, но прочитать всё равно надо	
  SPI_25Q64FV_Module->DR; 
  // Деактивируем ChipSelect микросхемы
  CSnDisable(CSn);	
}

/**************************************************************************************************************
  SPI_25Q64FV_eraseChip - Очистка всей микросхемы внешней SPI-памяти   
**************************************************************************************************************/
void SPI_25Q64FV_eraseChip (PinConfigType CSn)
{
  uint8_t src, dst;
	
  // Разрешаем запись
  SPI_25Q64FV_enableWrite (CSn);
	
  // Берем команду на стирание
  src = _25Q64FV_CMD_ChipErase;
	
  // Выставим ChipSelect нужной микросхемы
  CSnReady(CSn);
  // Передать блок данных
  SPI_25Q64FV_writeBlock (&src, &dst, 1);
  // Деактивируем ChipSelect микросхемы
  CSnDisable(CSn);
	
  // Время операции стирания микросхемы, от 20 с до 100 с
  // Ожидание стирания всей памяти (пока с максимальным значением) 
  delay_us (_25Q64FV_ChipErase_delay_us);
}



/**************************************************************************************************************
  SPI_25Q64FV_reset - Перезапуск микросхемы внешней SPI-памяти
**************************************************************************************************************/
void SPI_25Q64FV_reset (PinConfigType CSn)
{
  uint8_t src[2], dst[2];
	
  // Сброс выполнянется последовательно командами:
  src[0] = _25Q64FV_CMD_EnableReset;
  src[1] = _25Q64FV_CMD_Reset;
	
  // Выставим ChipSelect нужной микросхемы
  CSnReady(CSn);
  // Передать блок данных
  SPI_25Q64FV_writeBlock (src, dst, 2);
  // Деактивируем ChipSelect микросхемы
  CSnDisable(CSn);
}	


/**************************************************************************************************************
  SPI_25Q64FV_readID - Запрос ID микросхемы и производителя внешней SPI-памяти
**************************************************************************************************************/
uint16_t SPI_25Q64FV_readID (PinConfigType CSn)
{
  Dword_to_Byte result;
  uint8_t src[6], dst[6];
  
  // Первой идет команда
  src[0] = _25Q64FV_CMD_ManufacturerDeviceID;
  // Затем 24 битный адрес (он неважен, поэтому просто нули
  src[1] = 0x00;
  src[2] = 0x00;
  src[3] = 0x00;
  // Еще байта любым значением (синхронно им будут выданы коды)
  src[3] = 0x11;
  src[3] = 0x11;
  
  // Выставим ChipSelect нужной микросхемы
  CSnReady(CSn);
  // Передаём блок данных
  SPI_25Q64FV_writeBlock (src, dst, 6);
  // Деактивируем ChipSelect микросхемы
  CSnDisable(CSn);
  // Формируем результат
  result.DWord = 0;
  result.Byte[0] = dst[4];	// Код производителя 0xEF
  result.Byte[1] = dst[5];	// Код микросхемы 0x40
	
  return (uint16_t)result.DWord;
}

/**************************************************************************************************************
  SPI_25Q64FV_eraseSector - Стирание сектора внешней SPI-памяти (4кб)
**************************************************************************************************************/
void SPI_25Q64FV_eraseSector (PinConfigType CSn, uint32_t Address)
{
  Dword_to_Byte Addr;
  uint8_t src[4], dst[4];  // Посылаем всего 4 байта
  
  // Копируем адрес в передаваемый буфер
  Addr.DWord = Address;
  src[0] = _25Q64FV_CMD_SectorErase;
  // Старшим байтом вперед, поэтому переворачиваем
  src[1] = Addr.Byte[2];								
  src[2] = Addr.Byte[1];
  src[3] = Addr.Byte[0];
  
  // Разрешить запись
  SPI_25Q64FV_enableWrite (CSn);	
  
  // Выставим ChipSelect нужной микросхемы 
  CSnReady(CSn);
  // Передать блок данных
  SPI_25Q64FV_writeBlock (src, dst, 4);
  // Деактивируем ChipSelect микросхемы
  CSnDisable(CSn);
  
  // Время стирания сектора, мс:  от 45 до 400
  // Ожидание стирания сектора по верхней границе
  delay_us(_25Q64FV_SectorErase_delay_us);
}


/**************************************************************************************************************
  SPI_25Q64FV_writeByte - Запись байта во внешнюю память
**************************************************************************************************************/
void SPI_25Q64FV_writeByte (PinConfigType CSn, uint32_t Address, uint8_t Value)
{
  uint8_t src[5], dst[5];
  Dword_to_Byte Addr;
  Addr.DWord = Address;
	
  src[0] = _25Q64FV_CMD_PageProgram;
  src[1] = Addr.Byte[2];
  src[2] = Addr.Byte[1];
  src[3] = Addr.Byte[0];
  src[4] = Value;
		
  // Разрешить запись
  SPI_25Q64FV_enableWrite (CSn);	
	
  // Выставим ChipSelect нужной микросхемы
  CSnReady(CSn);
  // Передать блок данных
  SPI_25Q64FV_writeBlock (src, dst, 5);
  // Деактивируем ChipSelect микросхемы
  CSnDisable(CSn);
	
  // Время операции программирования байта, мкс: от 20 до 50 
  // Ожидание окончания записи байта 
  delay_us (_25Q64FV_ProgramByte_delay_us);
}





/**************************************************************************************************************
  SPI_25Q64FV_readArray - Чтение массива данных из внешней SPI-памяти
**************************************************************************************************************/
uint32_t SPI_25Q64FV_readArray (PinConfigType CSn, uint32_t Address, uint8_t* Destination, uint32_t Size)
{
  TimeoutType timeout;
  uint8_t src[4];
  Dword_to_Byte Addr;
  uint32_t i,count;
  volatile uint32_t data;
  uint8_t* Dst;
	
  Dst = Destination;
  Addr.DWord = Address;
	
  // Первым делом отправляем команду
  src[0] = _25Q64FV_CMD_ReadData;
  // После 3 байта адреса
  src[1] = Addr.Byte[2];
  src[2] = Addr.Byte[1];
  src[3] = Addr.Byte[0];

	
  // Выставим ChipSelect нужной микросхемы
  CSnReady(CSn);
	
  // Установим таймаут функции
  setTimeout (&timeout, _25Q64FV_TIMEOUT);
	
  // Дождаться полного освобождения SPI
  while (((SPI_25Q64FV_Module->SR) & SSP_FLAG_BSY) && (timeoutStatus(&timeout) != TIME_IS_UP));
	
  // Вычитать все данные из входного буфера, так как там может лежать мусор	
  while ((SPI_25Q64FV_Module->SR & SSP_FLAG_RNE))
    data = SPI_25Q64FV_Module->DR;	
	
  // Передать 4 байта с кодом команды и адресом
  for (i = 0; i < 4; i++)
  SPI_25Q64FV_Module->DR = src[i];
	
  // Читаем ответные 4 байта, хоть они нам и не нужны
  for (i = 0; i < 4; i++)
  {
    // Дождаться приема байта
    while (!(SPI_25Q64FV_Module->SR & SSP_FLAG_RNE) && (timeoutStatus(&timeout) != TIME_IS_UP));
    data = SPI_25Q64FV_Module->DR;
  }
	
  // Передаем пустой байт и принимаем ответный Size раз
  for (i = 0, count = 0; i < Size; i++)
  {
    // Дождаться появления свободного места в выходном буфере
    while (!(SPI_25Q64FV_Module->SR & SSP_FLAG_TNF) && (timeoutStatus(&timeout) != TIME_IS_UP))
    {
      // Читаем байт, полученный от 1636РР52У
      if (SPI_25Q64FV_Module->SR & SSP_FLAG_RNE)
      {
        *(Dst++) = SPI_25Q64FV_Module->DR;					
        count++;	
      }
    }
    // Передаем пустой байт
    SPI_25Q64FV_Module->DR = 0;		
    // Читаем байт, полученный от SPI_25Q64FV
    if (SPI_25Q64FV_Module->SR & SSP_FLAG_RNE)
    {
      *(Dst++) = SPI_25Q64FV_Module->DR;					
      count++;	
      }		
    }
    // Читаем остаток еще не принятых байт	
    while (SPI_25Q64FV_Module->SR & (SSP_FLAG_RNE | SSP_FLAG_BSY))
    {
      if ((SPI_25Q64FV_Module->SR & SSP_FLAG_RNE) && count < Size)
      {
        *(Dst++) = SPI_25Q64FV_Module->DR;					
        count++;
      }
    }
	
  // Деактивируем ChipSelect микросхемы
  CSnDisable(CSn);
	
  // Сколько байт прочитали
  return count; 
}



/**************************************************************************************************************
  SPI_25Q64FV_memoryTest - Функция тестирования микросхемы памяти. Проиводит запись и чтение 
  по всем доступным адресам                                                
**************************************************************************************************************/
uint8_t SPI_25Q64FV_memoryTest (PinConfigType CSn)
{
  uint8_t  buffer[4];
  uint8_t  dest[4];
  uint32_t i,j;
  uint8_t  temp = 0;
  uint32_t address = 0;
	
  *(uint32_t*)buffer = 0x557799AA;
	
  SPI_25Q64FV_reset (CSn);
  // Проверка ID
  if(SPI_25Q64FV_readID(CSn) != _25Q64FV_ID) 
    return 1;
  // Очищаем память перед тестом
  SPI_25Q64FV_eraseChip (CSn);
  // Проверка на стирание и запись
  for(i = 0; i < _25Q64FV_SECTORS_NUM+5; i++)
  {
    address = i*_25Q64FV_SECTORS_SIZE;
    SPI_25Q64FV_readArray (CSn, address , &temp, 1);
    if(temp != 0xFF) 
      return 1;
  
    temp = 0;
  }
  for(i = 1; i < _25Q64FV_SECTORS_NUM; i++)
  {
    address = i*_25Q64FV_SECTORS_SIZE;
    SPI_25Q64FV_writeByte (CSn, address, buffer[0]);
    address = i*_25Q64FV_SECTORS_SIZE + 1;
    SPI_25Q64FV_writeByte (CSn, address, buffer[1]);
    address = (i + 1)*_25Q64FV_SECTORS_SIZE  - 2;
    SPI_25Q64FV_writeByte (CSn, address, buffer[2]);
    address = (i + 1)*_25Q64FV_SECTORS_SIZE - 1;
    SPI_25Q64FV_writeByte (CSn, address, buffer[3]);
  
    address = i*_25Q64FV_SECTORS_SIZE;
    SPI_25Q64FV_readArray (CSn, address, dest, 2);
    address = (i + 1)*_25Q64FV_SECTORS_SIZE - 2;
    SPI_25Q64FV_readArray (CSn, address, &dest[2], 2);
  
    if(*(uint32_t*)buffer != *(uint32_t*)dest) 
      return 1;
		
    *(uint32_t*)dest = 0;
  }
  for(i = 0; i < _25Q64FV_SECTORS_NUM; i++)
  {
    address = i*_25Q64FV_SECTORS_SIZE+50;
    *(uint32_t*)buffer = address;
    for(j=0; j< 4; j++)
    {
      SPI_25Q64FV_writeByte (CSn, address+j, buffer[j]);
    }	
    SPI_25Q64FV_readArray (CSn, address, dest, 4);
    if(*(uint32_t*)buffer != *(uint32_t*)dest) 
      return 1;
    *(uint32_t*)dest = 0;
  }
  return 0;
}

/**************************************************************************************************************
  SPI_25Q64FV_writeBlock - Внутреняя функция отправки сообщений по SPI
  Параметры:
            Source      - Указатель на буфер источника;
            Destination - Указатель на буфер места назначения;
            Size        - Размер передаваемомой посылки в байтах
  Возвращает: NONE
**************************************************************************************************************/
static void SPI_25Q64FV_writeBlock (uint8_t* Source, uint8_t* Destination, uint32_t Size)
{
  uint32_t    temp;
  TimeoutType timeout;
  
  // Установим таймаут функции
  setTimeout (&timeout, _25Q64FV_TIMEOUT);
	
  // Дождаемся полного освобождения буфера
  while ((SPI_25Q64FV_Module->SR & SSP_FLAG_BSY) && (timeoutStatus(&timeout) != TIME_IS_UP));

  // Читаем входной буфер, чтобы его вычистить, так как там может лежать мусор	
  while ((SPI_25Q64FV_Module->SR & SSP_FLAG_RNE))
  temp = SPI_25Q64FV_Module->DR;

  for(uint32_t i = 0; i < Size; i++)
  {
    // Дождаться появления свободного места в буфера передатчика
    while (!(SPI_25Q64FV_Module->SR & SSP_FLAG_TNF) && (timeoutStatus(&timeout) != TIME_IS_UP));
    // Передать байт
    SPI_25Q64FV_Module->DR = *Source++;
  }
  for(uint32_t i = 0; i < Size; i++)
  {
    // Дождаться приема байта
    while (!(SPI_25Q64FV_Module->SR & SSP_FLAG_RNE) && (timeoutStatus(&timeout) != TIME_IS_UP));
    // Читаем байт, полученный от 1636РР52У
    *Destination++ = SPI_25Q64FV_Module->DR;		
  }
}
