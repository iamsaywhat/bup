/***********************************************************************************************************

    SWS - Драйвер, предоставляющий функции нижнего уровня для работы с СВС.
						 
    СВС - Система воздушных сигналов. В полете даёт информацию о давлении, скорости
    и высоте над уровнем моря (более подробное описание см в документации).
    СВС поключается к UART порту по RS-485 интерфейсу. 
		 
    Настройки UART:
        Скорость передачи 115200 Бод;
        8 бит данных, 
        1 стоповый бит, 
        Без контроля четности. 
        (115200, 8-N-1)
						 
    Особенности работы СВС: 
						 
        1) Первые 3 секунды после подачи питания на СВС, он находится в режиме ожидания 
            команд для обновления внутренней программы. После чего переходит в нормальный режим работы
            и готов в выдаче информации.
        2) Обмен информацией односторонний (СВС только посылает данные).
        3) Данные отсылаются 50 раз в секунду без запроса.
						 
    Для работы необходимо:
	
        1) Выбрать конфигурацию в разделе "Аппаратно зависимая конфигурация"; 
        2) Запустить SWS_RetargetPins - выполнить настройку используемых пинов
        3) Вызвать SWS_init - выполнит конфигурирование использоваемого UART
        4) SWS_GetPacket (SWS_Packet_Type_Union* SWS_Pack) - получить данные от СВС
						 
						 
    Пример использования:
						 
        SWS_Packet_Type_Union  SWS_Packet;
        SWS_GetPacket (&SWS_Packet);

***********************************************************************************************************/
#ifndef _SWS_H_
#define _SWS_H_

#include "MDR32F9Qx_port.h"
#include "MDR32F9Qx_dma.h"
#include "MDR32F9Qx_uart.h"
#include "MDR32F9Qx_rst_clk.h"


/************************************************************
    Аппаратно зависимая конфигурация
************************************************************/
#define SWS_DE	                  PORT_Pin_6             // PORTD.6 - 485_DE Активация приёмопередатчика
#define SWS_RX	                  PORT_Pin_7             // PORTD.6 - UART1_RX
#define SWS_TX	                  PORT_Pin_8             // PORTD.6 - UART1_TX
#define SWS_UART                  MDR_UART1              // UART - на котором сидит передатчик
#define SWS_PORT                  MDR_PORTD              // Порт, к которому подключен передатчик 
#define RST_CLK_PCLK_SWS_PORT     RST_CLK_PCLK_PORTD     
#define RST_CLK_PCLK_SWS_UART     RST_CLK_PCLK_UART1
#define BAUDRATE_SWS              115200                 // Бит/с скорость обмена с СВС 
#define SWS_BYTE_TIMEOUT          2                      // Величина таймаута на приём байта, мс
#define SWS_CLR_TIMEOUT           2                      // Величина таймаута на очистку буфера, мс
#define SWS_HANDLER_TIMEOUT       20                     // Величина таймаута на ожидание заголовка, мс

/************************************************************
    Заголовок пакета данных от СВС
************************************************************/
#define SWS_HandlerReverse 0x29101820
#define SWS_Handler        0x20181029


/************************************************************
    Статусы работы СВС
************************************************************/
typedef enum{
  SWS_OK,        // Успешно
	SWS_TIMEOUT,   // Истекло время ожидания
	SWS_WRONG_CRC  // Ошибка контрольной суммы
}SWS_Status;



/************************************************************
    Структура пакета данных от СВС
************************************************************/
typedef __packed struct{
    uint32_t Handler;              // Заголовок
    uint32_t Counter;              // Счетчик пакетов
    float StaticPressure;          // Статическое давление
    float DynamicPressure;         // Динамическое давление
    float AbsoluteHeight;          // Абсолютная высота
    float InstrumentSpeed;         // Инструментальная скорость
    float TrueSpeed;               // Истинная скорость
    uint32_t Reserved[5];          // Резерв
    uint32_t StateFlags;           // Флаги состояний
    uint32_t CRC;                  // Контрольная сумма
}SWS_Packet_Type;

typedef union { 
    SWS_Packet_Type Struct;
    uint8_t Buffer[56];
}SWS_Packet_Type_Union;


/**************************************************************************************************************
    SWS_RetargetPins - Функция переопределения UART1 на другие пины, для работы SWS
    Параметры:  NONE
***************************************************************************************************************/
void SWS_RetargetPins (void);


/**************************************************************************************************************
    SWS_init - Запуск процедуры обмена с SWS
    Параметры:  NONE
***************************************************************************************************************/
void SWS_init (void);


/**************************************************************************************************************
    SWS_deinit - Деинициализация СВС
***************************************************************************************************************/
void SWS_deinit (void);


/**************************************************************************************************************
    SWS_GetPacket - Получение структуры пакета от СВС
    (Фактически производит упаковку буфера в который пишет DMA,
    проверяет загололок и контрольную сумму пакета данных)

    Параметры:  SWS_Pack - Указатель на структуру пакета от СВС

    Возвращает: 0 - если ошибка проверки заголовка и контрольной суммы
                1 - если проверка верна
***************************************************************************************************************/
SWS_Status SWS_GetPacket (SWS_Packet_Type_Union* SWS_Pack);


#endif 
