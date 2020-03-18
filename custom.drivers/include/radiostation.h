/*****************************************************************************************************
  radiostation - Драйвер работы с радиостанцией Р-187-П1.

  Портативная радиостанция от "Ангстрем". Предоставляет для обмена информацией интерфейс RS-485;
	В составе САУ предполается использовать для изменения коодинат точки приземления находясь в полете,
	путем отправки SDS сообщения с земли.
	
	Данный программный модуль позволяет проверить соостояние связи и коректность (валидность ответов), 
  запрашивать количество SDS сообщений, их список, их текст, а так же удалять их с радиостанции.
  
  Настройки UART:
    Скорость передачи 9600 Бод;
    8 бит данных, 
    1 стоповый бит, 
    Без контроля четности. 
    (115200, 8-N-1)
		
	Примечания:
    используется подобие slip-протокола
		
    1) Тип обмена информацией с СНС - двунаправленный запрос/ответ;
					 
    2) Контрольная сумма: CRC16, маска 0х1021, начальное значение 0xFFFF; 
					 
    3) Поток данных разделяется на кадры (подобие SLIP протокола).
       Каждый кадр начинается символом FEND (0xC0). 
					
       Исходный пакет        Отправляем как
       |ПАКЕТ|     ->        |FEND||ПАКЕТ|
					 
       Если FEND (0xC0) содержится в данных  пакета, он посылается внутри кадра как FESC TFEND (0xDB 0xDC). 
					 
       Исходный пакет                      Отправляем как
       |ПАКЕТ.....C0.....ПАКЕТ|   ->   |FEND||ПАКЕТ.....[FESC,TFEND].....ПАКЕТ|
					 
       Если FESC (0xDB) содержится в данных пакета, он посылается внутри кадра как FESC TFESC (0xDB 0xDD). 
					 
       Исходный пакет                      Отправляем как
       |ПАКЕТ.....DB.....ПАКЕТ|   ->   |FEND||ПАКЕТ.....[FESC,TFESC].....ПАКЕТ|
					 
       Символ FESC с любым символом, за исключением TFEND или TFESC, является ошибкой. 
       Символы TFEND и TFESC являются
       обычными символами, если им не предшествует FESC. 
					 
    4) Данные в протоколе передаются старщим байтом вперед (big-endian)
  
*****************************************************************************************************/
#ifndef _RADIOSTATION_H_
#define _RADIOSTATION_H_

#include <stdint.h>

/************************************************************
  Аппаратнозависимая конфигурация
************************************************************/
#define RADIO_BAUDRATE            9600          // Бит/с скорость обмена с СНС
#define RADIO_BYTE_TIMEOUT        2             // Величина таймаута на приём байта, мс
#define RADIO_FIFO_TIMEOUT        16            // Величина таймаута на очистку FIFO, мс
#define RADIO_RECEIVE_TIMEOUT     10            // Величина таймаута на прием пакета, мс
#define RADIO_TRANSACTION_TIMEOUT 150           // Максимальное время на запрос + ответ к радиостанции
#define RADIO_SEND_PAUSE          1             // Пауза между любыми запросами к радиостанции, мс

#define PACKET_SIZE_LIMIT         200           // Ограничение на размер принимаемых пакетов, байт
#define RADIO_ADDRESS             0x01          // Адрес (id) радиостанции

/*****************
  Коды ошибок
******************/
typedef enum{
  RADIO_WRONG_CRC,               // Ошибка в контрольной сумме
	RADIO_WRONG_INDEX,             // Индекс кадра запроса и ответа не совпали
	RADIO_TIMEOUT,                 // Таймаут
	RADIO_SUCCESS,                 // Без ошибок/Положительный результат
	RADIO_FAILED,                  // Ошибка/Отрицательный результат
	RADIO_WRONG_ADDRESS,           // Адрес радиостанции не опознан
	RADIO_EXCESS_PACKET_SIZE,      // Превышен размер входящего пакета
	RADIO_COORDINATES_NOT_FOUND,   // В сообщении не найдены координаты
	RADIO_BLOCKED,                 // Обращения к радиостанции программно заблокированы
}RadioStatus;


uint8_t getSdsCount (void);
RadioStatus sendEmpty(void);
RadioStatus checkDeviceName(void);
RadioStatus checkManufacturerName(void);
RadioStatus updateSdsList(void);
RadioStatus getCoordinatesFromSds(int idSds, double *latitude, double *longitude);
RadioStatus deleteSds(uint8_t idSds);
RadioStatus deleteAllSds (void);
RadioStatus findCoordinateInSdsList(double *latitude, double *longitude);


#endif
