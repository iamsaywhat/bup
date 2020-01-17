#include "SWS.h"
#include "crc32.h"
#include "otherlib.h"
#include "bupboard.h"
#include "MDR32F9Qx_uart.h"

/**************************************************************************************************************
  Объявления локальных функций модуля
***************************************************************************************************************/
static int16_t UARTReceiveByte (MDR_UART_TypeDef* UARTx);
static uint8_t UARTSendByte (MDR_UART_TypeDef* UARTx, uint16_t Byte);

/**************************************************************************************************************
  SWS_RetargetPins - Функция переопределения UART1 на другие пины, для работы SWS
***************************************************************************************************************/
static void SWS_RetargetPins (void)
{	
  // Переназчаем UART1 на порт D для работы SWS
  Pin_init (SWS_RX);
  Pin_init (SWS_TX);
  // Пин активации приёмопередатчика
  Pin_init (SWS_DE);
}

/**************************************************************************************************************
  SWS_init - Запуск процедуры обмена с SWS
***************************************************************************************************************/
void SWS_init (void)
{
  UART_InitTypeDef UART_InitStructure;
	
  // Сброс конфигуряции UART
  UART_DeInit(SWS_UART);
  // Заполнение структуры по-умолчанию
  UART_StructInit (&UART_InitStructure);
  // Установка предделителя блока UART1_CLK = HCLK
  UART_BRGInit (SWS_UART,UART_HCLKdiv1); 
  // Заполняем структуру инициализации	
  UART_InitStructure.UART_BaudRate                = BAUDRATE_SWS;
  UART_InitStructure.UART_WordLength              = UART_WordLength8b;
  UART_InitStructure.UART_StopBits                = UART_StopBits1;
  UART_InitStructure.UART_Parity                  = UART_Parity_No;
  UART_InitStructure.UART_FIFOMode                = UART_FIFO_ON;
  UART_InitStructure.UART_HardwareFlowControl     = UART_HardwareFlowControl_RXE | UART_HardwareFlowControl_TXE;
  // Инициализация UART
  UART_Init (SWS_UART,&UART_InitStructure);
  
  // Включение UART - CВС
  UART_Cmd(SWS_UART,ENABLE);
  // Устанавливаем в 0 пин DE - Режим приёмника
  Pin_reset(SWS_DE);
  // Если нужен режим передатчика - устанавливаем в 1;
  //Pin_set(SWS_DE);
}

/**************************************************************************************************************
  SWS_deinit - Деинициализация СВС
***************************************************************************************************************/
void SWS_deinit (void)
{
  // Сброс конфигуряции UART
  UART_DeInit(SWS_UART);
  // Отключение UART - CВС
  UART_Cmd(SWS_UART,DISABLE);
  // Устанавливаем в 0 пин DE - Режим приёмника
  Pin_reset(SWS_DE);
  // Освобождаем пины по
  Pin_default (SWS_RX);
  Pin_default (SWS_TX);
  // Пин активации приёмопередатчика
  Pin_default (SWS_DE);
}

/**************************************************************************************************************
  SWS_GetPacket - Получение структуры пакета от СВС
***************************************************************************************************************/
SWS_Status SWS_GetPacket (SWS_Packet_Type_Union* SWS_Pack)
{
  SWS_Packet_Type_Union  Actual_SWS_Pack;      // Актуальные данные от СВС
  SWS_Status             status;               // Статус выполнения
  TimeoutType            timeout;              // Таймаут контроль
  uint32_t               crc = 0;              // Контрольная сумма
		
  /* Вычищаем FIFO от мусора */
  setTimeout (&timeout, SWS_CLR_TIMEOUT);
  while ((UART_GetFlagStatus (SWS_UART, UART_FLAG_RXFE) != SET) && (timeoutStatus(&timeout) != TIME_IS_UP)) 
    UART_ReceiveData(SWS_UART);	
	
  /* Подключаемся к СВС */
  SWS_RetargetPins();
  SWS_init();
	
  /* Будем производить несколько попыток получить данные */
  for(uint8_t attemptsCount = 0; attemptsCount < 3; attemptsCount++)
  {
    /* Ожидаем заголовок */
    setTimeout (&timeout, SWS_HANDLER_TIMEOUT);
    while(timeoutStatus(&timeout) != TIME_IS_UP)
    {
      /* Ловим первый байт заголовка */
      if (UARTReceiveByte(SWS_UART) != (uint8_t)(SWS_Handler)) continue; 
      /* Если первый байт заголовка не совпал, дальше не ждем, переходим к следующей итерации
         Если совпал, ловим следующий байт заголовка */
      if (UARTReceiveByte(SWS_UART) != (uint8_t)(SWS_Handler >> 8)) continue; 
      /* Если совпал, ловим следующий байт заголовка */
      if (UARTReceiveByte(SWS_UART) != (uint8_t)(SWS_Handler >> 16)) continue; 
      /* Если совпал, ловим следующий байт заголовка
        Если совпадёт и 4й байт, то переходим к приёму остального пакета */
      if (UARTReceiveByte(SWS_UART) == (uint8_t)(SWS_Handler >> 24)) break;
    }	
    /* Если был таймаут, связи нет */
    if(timeout.status == TIME_IS_UP)
    {
      /* Фиксируем код ошибки */
      status = SWS_TIMEOUT;
      /* Сразу переходим к новой попытке связаться */
      continue;
    }
    /* Если попали сюда, значит связь есть, заголовок совпал, 
       поэтому продублируем принятый заголовок в структуру */
    Actual_SWS_Pack.Struct.Handler = SWS_Handler;
    /* Принимаем остальную часть пакета */
    for(uint8_t i = 4; i < 56; i++)
      Actual_SWS_Pack.Buffer[i] = UARTReceiveByte(SWS_UART);
			
    /* Вычисляем контрольную сумму */
    crc = Calc_Crc32_Array(Actual_SWS_Pack.Buffer, 56);
		
    /* Сверяем контрольную сумму */
    if(crc == Actual_SWS_Pack.Struct.CRC)
    {
      /* Если сошлись фмксируем кодом успеха
      и выходим из цикла */
      status = SWS_OK;
      break;
    }
    else
    {
    /* Если не сошлись фмксируем код ошибки
    сразу переходим к новой попытке связаться */
      status = SWS_WRONG_CRC;
      continue;
    }
  }
  /* Отключаемся от СВС */
  SWS_deinit();
  /* Данные возвращаем, только если они успешно получены */
  if(status == SWS_OK)
    *SWS_Pack = Actual_SWS_Pack;
	
  /* Возвращаем статус выполнения */
  return status;
}

/**************************************************************************************************************
  UARTReceiveByte - Функция приёма байта данных по UART с контролем таймаутом
***************************************************************************************************************/
static int16_t UARTReceiveByte (MDR_UART_TypeDef* UARTx)
{
  TimeoutType timeout;
  uint16_t    byte = 0;
	
  // Ждем прихода информации
  setTimeout (&timeout, SWS_BYTE_TIMEOUT);
  while ((UART_GetFlagStatus (UARTx, UART_FLAG_RXFE) == SET) && (timeoutStatus(&timeout) != TIME_IS_UP));
  // Если выход из ожидания по таймауту - возвращаем ошибку и выходим
  if(timeout.status == TIME_IS_UP) 
    return 0xFF;
	
  // Иначе принимаем байт
  byte = UART_ReceiveData(UARTx);
  return byte;
}

/**************************************************************************************************************
  UARTSendByte - Функция отправки байта данных по UART с контролем таймаутом
***************************************************************************************************************/
static uint8_t UARTSendByte (MDR_UART_TypeDef* UARTx, uint16_t Byte)
{
  TimeoutType timeout;
	
  // Если FIFO передатчика заполнен, то ждем пока освободится
  setTimeout (&timeout, SWS_CLR_TIMEOUT);
  while ((UART_GetFlagStatus (UARTx, UART_FLAG_TXFF) == SET) && (timeoutStatus(&timeout) != TIME_IS_UP))
  // Если выход из ожидания по таймауту - возвращаем ошибку и выходим
  if(timeout.status == TIME_IS_UP)
    return 0xFF;
	
  // Иначе отправляем исходный символ
  UART_SendData(UARTx, Byte);
	
  return 0;
}
