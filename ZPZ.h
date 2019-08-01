/*******************************************************************************************************************

           ZPZ.c/h - Модуль обслуживания загрузчика полётного задания (ЗПЗ).
								
					 Предоставляет пользователю со стороны ПК следующий функционал:
						 - Проверка связи с ПК
						 - Загрузка карты высоты рельефа и точку приземления
						 - Выгрузка карты высоты рельефа и точку приземления
						 - Управление БИМами с ПК
						 - Проверка состояния шпилек
						 - Проверка состояния системы
						 - Передача в CAN произвольного сообщения
						 - Передача данных с СНС
						 - Передача данных с СВС
						 - Работа с хранилищем полетных логов
								
					 ЗПЗ поключается к UART порту по RS-232 интерфейсу.
					 
					 Настройки UART:
						Скорость передачи 115200 Бод;
						8 бит данных, 
						1 стоповый бит, 
						Без контроля четности. 
						(115200, 8-N-1)
					 
					 
					 Особенности работы с ЗПЗ:
					 
					 1) Тип обмена информацией с ЗПЗ - двунаправленный запрос/ответ;
					 
					 2) Контрольная сумма: CRC16, маска 0х1021, начальное значение 0xFFFF; 
					 
					 3)	Поток данных разделяется на кадры по RFC 1055 (SLIP протокол).
					 Каждый кадр начинается изаканчивается символом FEND (0xC0). 
					
					 Исходный пакет        Отправляем как
							|ПАКЕТ|     ->     |FEND||ПАКЕТ||FEND|
					 
					 Если FEND (0xC0) содержится в данных  пакета, он посылается внутри кадра как FESC TFEND (0xDB 0xDC). 
					 
					 Исходный пакет                      Отправляем как
							|ПАКЕТ.....C0.....ПАКЕТ|   ->   |FEND||ПАКЕТ.....[FESC,TFEND].....ПАКЕТ||FEND|
					 
					 Если FESC (0xDB) содержится в данных пакета, он посылается внутри кадра как FESC TFESC (0xDB 0xDD). 
					 
					 Исходный пакет                      Отправляем как
							|ПАКЕТ.....DB.....ПАКЕТ|   ->   |FEND||ПАКЕТ.....[FESC,TFESC].....ПАКЕТ||FEND|
					 
					 Символ FESC с любым символом, за исключением TFEND или TFESC, является ошибкой. 
					 Символы TFEND и TFESC являются
					 обычными символами, если им не предшествует FESC. 
					 
					 4) Данные в протоколе передаются младшим байтом вперед.
					 
					 Используемые драйвера и модули:
					 
					 1636PP52Y.h   - Драйвер для внешней SPI памяти (загрузка полётного задания и карты высот)
					 crc16.h       - Табличный метод расчета контрольной суммы
					 BIM.h         - Драйвер работы с БИМами
					 Log_FS.h      - Хранилище полетных логов
					 SNS.h         - Драйвер работы с СНС
					 SWS.h         - Драйвер работы с СВС
					 discrete_io.h - Драйвер управления дискретами и реле 
					 
*******************************************************************************************************************/
#ifndef _ZPZ_H_
#define _ZPZ_H_

#include "stdint.h"

// Конфигурация ZPZ модуля (аппаратная привязка)
#define ZPZ_RX	                  PORT_Pin_0             // PORTF.0 - UART2_RX
#define ZPZ_TX	                  PORT_Pin_1	           // PORTF.1 - UART2_TX
#define ZPZ_UART                  MDR_UART2              // UART - на котором сидит передатчик
#define ZPZ_PORT                  MDR_PORTF              // Порт, к которому подключен передатчик 
#define RST_CLK_PCLK_ZPZ_PORT     RST_CLK_PCLK_PORTF     // Идентификатор Порта, для включения тактирования
#define RST_CLK_PCLK_ZPZ_UART     RST_CLK_PCLK_UART2     // Идентификатор UART, для включения тактирования
#define BAUDRATE_ZPZ              115200                 // Бит/с - скорость обмена с ZPZ по UART
#define ZPZ_IRQn									UART2_IRQn             // Идентификатор источника прерывания
#define ZPZ_CAN                   MDR_CAN1               // Идентификатор CAN модуля, к которому ZPZ имеет доступ



// Заголовки - идентификаторы пакетов
typedef enum {
	HANDLER_PC	     =	0x4350,				// Заголовок пакетов от ЗПЗ к БУПу (в символах на самом деле эквивалентно CP)
	HANDLER_P		     =	0x50,					// Младший байт заголовка PC
	HANDLER_C        =  0x43,					// Старший байт заголовка PC
	HANDLER_BU       =  0x5542,				// Заголовок пакетов от БУПа к ЗПЗ (в символах на самом деле эквивалентно UB)
	HANDLER_B				 =  0x42,					// Младший байт заголовка BU
	HANDLER_U        =  0x55,					// Старший байт заголовка BU
} ZPZ_Handlers;


// Идентификаторы команд
typedef enum {
	START_DOWNLOAD    = 0x42,				// "B" Команда инициирования процедуры загрузки карты
	MAP_DOWNLOAD      = 0x4D,				// "M" Команда загрузки карты
	START_UPLOAD      = 0x62,				// "b" Команда инициирования процедуры выгрузки карты
	MAP_UPLOAD        = 0x6D,				// "m" Команда выгрузки карты
	CHECK_CONNECT     = 0x50,       // "P" Команда проверки связи
	BIM_CONTROL       = 0x45,       // "E" Команда управления БИМами
	BIM_STATUS        = 0x65,       // "e" Команда статуса БИМов
	LOG_FORMAT        = 0x46,       // "F" Команда форматирорования черного ящика
	LOG_FILES         = 0x49,       // "I" Команда запроса информации о файлах
	LOG_UPLOAD        = 0x66,       // "f" Команда выгрузки файла
	REQ_SWS           = 0x57,       // "W" Команда запроса данных с SWS
	REQ_SNS_POS       = 0x4E,				// "N" Команда запроса местоположения (данных с СНС) 
	REQ_SNS_STATE     = 0x6E,       // "n" Команда запроса доступных от SNS данных
	REQ_SNS_SETTINGS  = 0x63,       // "c" Команда запуска калибровки SNS
	PIN_STATE         = 0x70,       // "p" Команда запроса состояния шпилек
	SYSTEM_STATE      = 0x73,       // "s" Команда запроса состояния системы
	CAN_TRANSMIT      = 0x43,       // "C" Команда перенаправления посылки в CAN
} ZPZ_CMD;



// Индентификаторы статуса для короткого ответа ЗПЗ
typedef enum {
	SUCCES = 0x00,          // Ошибок нет												
	FAIL   = 0xFF,          // Сбой при обмене
} ZPZ_Short_State;


// Статусы работы модуля ZPZ
typedef enum {
	ZPZ_OK,                 // Все в порядке
	ZPZ_WRONG_CRC,          // Ошибка в контрольной сумме
	ZPZ_TIMEOUT             // Превышено время ожидания
} ZPZ_STATUS_CODE;


// Размеры пакетов
#define PACKET0_DATASIZE          54            // Байт, только информационная часть пакета 0
#define PACKET0_ALLSIZE           64            // Байт, полный размер пакета 0
#define PACKETnMAP_DATASIZE       800           // Байт, только информационная часть пакета MAP
#define PACKETnMAP_ALLSIZE        810           // Байт, полный размер пакета MAP




// Адреса хранения полетного задания и карты рельефа
#define PACKET0_ADR               0             // Адрес по которому лежит информация из нулевого пакета (точка прилемления, масштабы карты)
#define PACKET0_ADR_END           53            // Конечный адрес по которому лежит информация из нулевого пакета (точка прилемления, масштабы карты)

// Макрос, для быстрого получения адреса, по которому лежит i - й пакет с картой (i = 1...400)
#define nMAP_ADR(x)               (PACKET0_ADR_END+1+800*(x-1))                 

// Определения для работы с БИМами
#define L_BIM          0x4C             // Идентификатор левого БИМа
#define R_BIM          0x52             // Идентификатор правого БИМа

#define LINE_MAIN      0x01             // Основная линия
#define LINE_RESERV    0x02             // Запасная линия
#define LINE_DEBUG     0x03             // Отладочная линия 



/******************************************************************
               Структура основной части всех пакетов              *
******************************************************************/
typedef __packed struct{
	uint16_t			Handler;				// Заголовок
	uint16_t			PacketSize;			// Размер пакета
	uint8_t       Command;				// Код команды
	uint16_t			Count;					// Номер пакета
	uint16_t			CRC;				  	// Контрольная сумма
	}ZPZ_BasePacket_Type;

typedef union {    
	ZPZ_BasePacket_Type   Struct;
	uint8_t               Buffer[9];
}ZPZ_BasePacket_Union;


/******************************************************************
							Структура простого ответа                           *
******************************************************************/
typedef __packed struct{
	uint16_t			Handler;				// Заголовок
	uint16_t			PacketSize;	    // Размер пакета
	uint8_t       Command;        // Код команды
	uint16_t			Count;					// Номер пакета
	uint8_t       Error;          // Статусный байт
	uint16_t      CRC;				  	// Контрольная сумма
	}ZPZ_Response_Type;

typedef union {    
	ZPZ_Response_Type   Struct;
	uint8_t             Buffer[10];
}ZPZ_Response_Union;
/*****************************************************************/
	



/******************************************************************
	Структура информационного кадра запроса на управление БИМами    *
******************************************************************/
typedef __packed struct{
	uint8_t			Side;              // Какой из БИМОВ
	uint8_t     State;             // Вкл(0x01)/Выкл(0x00)
	uint8_t     Position;          // Положение стропы
	}ZPZ_RequestControlBIM_Type;

typedef union {    
	ZPZ_RequestControlBIM_Type   Struct;
	uint8_t                      Buffer[3];
}ZPZ_RequestControlBIM_Union;
/*****************************************************************/


/******************************************************************
	Структура информационного кадра запроса на управление БИМами    *
******************************************************************/
typedef __packed struct{
	uint8_t     Error;             // Резерв        
	uint8_t     Side;              // Какой из БИМОВ
	uint8_t     Line;              // CAN линия
	uint16_t    Err;               // Статус-флаги БИМ (см. протокол работы с БИМ)
	uint8_t     Position;          // Положение двигателя
	uint8_t     Voltage;           // Напряжение двигателя
	uint8_t     ElAngle;           // Электрический угол
	uint8_t     Current;           // Ток двигателя
  uint8_t     Speed;             // Скорость двигателя
	}ZPZ_ResponseStatusBIM_Type;

typedef union {    
	ZPZ_ResponseStatusBIM_Type   Struct;
	uint8_t                      Buffer[10];
}ZPZ_ResponseStatusBIM_Union;
/*****************************************************************/





/******************************************************************
	            Структура первого инициирующего пакета              *
******************************************************************/
typedef __packed struct{
	uint16_t			Handler;				// Заголовок
	uint16_t			PacketSize;			// Размер пакета
	uint8_t       Command;				// Код команды
	uint16_t			Count;					// Номер пакета
	uint8_t 			Reserved;				// Резервированный байт
	double				LatTDP; 				// Широта точки приземления
	double				LonTDP; 				// Долгота точки приземления
	uint16_t			NumPackets;			// Число пакетов
	uint16_t			NumLats;				// Количество широт
	uint16_t			NumLons;				// Количество долгот
	double				LatBT;					// Начало "плитки" (левый нижний угол прямоугольника): Широта
	double				LonBT;					// Начало "плитки" (левый нижний угол прямоугольника): Долгота
	double				LatScale;				// Масштаб по широте (в одном делении количество секунд)
	double				LonScale;				// Масштаб по долготе (в одном делении количество секунд)
	uint16_t      CRC;						// Контрольная сумма
	}ZPZ_InitiationPacket_Type;

typedef union {    
	ZPZ_InitiationPacket_Type Struct;
	uint8_t Buffer[64];
}ZPZ_InitiationPacket_Union;
/*****************************************************************/




/**************************************************************************************************************
                                                                                                              * 
						ZPZ_RetargetPins - Функция переопределения UART на другие пины, для работы по ZPZ                 *
						Параметры:  NONE                                                                                  *
											                                                                                        * 
***************************************************************************************************************/
void ZPZ_RetargetPins (void);

	
	
/**************************************************************************************************************
                                                                                                              * 
						ZPZ_init - Инициализация UART под ZPZ                                                             *
						Параметры:  NONE                                                                                  *
											                                                                                        * 
***************************************************************************************************************/
void ZPZ_init (void);


/**************************************************************************************************************
																																																							*	
						ZPZ_ShortResponse - Отправка простого пакета подтвержений/ошибки к ЗПЗ                            *
						                                                                                                  *
						Параметры:                                                                                        *
											Command - Идентификатор команды                                                         *
											Count   - Номер сообщения                                                               *
											Error   - Статус (SUCCES или FAIL)                                                      *
						                                                                                                  *
**************************************************************************************************************/
void ZPZ_ShortResponse(uint8_t Command, uint16_t Count, uint8_t Error);



/**************************************************************************************************************
																																																							*
          ZPZ_WriteIntoCSnUnion - Функция записи производит объединение 4х микросхем SPI-памяти в единое      *
                                  адресное пространство 0x00000 - 0x7FFFF, используя драйвер "1636PP52Y.h".   *
																	Позволяет абстрагироваться от периферийного уровня и управления CSn.        *			
						Параметры:                                                                                        *
											Address     - Адрес места записи;                                                       *
											Destination - Указатель на буфер источника;                                             *
											Size        - Размер записываемой области памяти в байтах;                              *
						Возвращает: 																										 																	*
											 NONE																								                                    *
																																																							*	
**************************************************************************************************************/
void ZPZ_WriteIntoCSnUnion(uint32_t Address, uint8_t* Source, uint32_t Size);


/**************************************************************************************************************
																																																							*	
          ZPZ_ReadIntoCSnUnion - Функция чтения производит объединение 4х микросхем SPI-памяти в единое       *
                                  адресное пространство 0x00000 - 0x7FFFF, используя драйвер "1636PP52Y.h".   *
																	Позволяет абстрагироваться от периферийного уровня и управления CSn.        *	
						Параметры:                                                                                        *
											Address     - Адрес чтения;                                                             *
											Destination - Указатель на буфер места назначения;                                      *
											Size        - Размер читаемой области памяти в байтах;                                  *
						Возвращает: 																										 																	*
											 NONE																								                                    *	
																																																							*												 
**************************************************************************************************************/
void ZPZ_ReadIntoCSnUnion(uint32_t Address, uint8_t* Destination, uint32_t Size);


/**************************************************************************************************************
																																																							*	
            ZPZ_ChipEraseCSnUnion - Форматирование носителя с полетным заданием и картой высот.               *
																																																							*	
**************************************************************************************************************/
void ZPZ_ChipEraseCSnUnion(void);

/**************************************************************************************************************
																																																							*	
					ZPZ_Service - Функция обслуживания команд от ЗПЗ (Основной режим)                                   *
																																																							*	
          Примечание: Пока запущена - устройство работает в режиме ЗПЗ                                        *
																																																							*	
**************************************************************************************************************/
uint8_t ZPZ_Service (void);

#endif
