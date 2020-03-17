#ifndef _RADIOSTATION_H_
#define _RADIOSTATION_H_

#include <stdint.h>

/************************************************************
  Аппаратнозависимая конфигурация
************************************************************/
#define RADIO_BAUDRATE           9600          // Бит/с скорость обмена с СНС
#define RADIO_BYTE_TIMEOUT       2             // Величина таймаута на приём байта, мс
#define RADIO_FIFO_TIMEOUT       16            // Величина таймаута на очистку FIFO, мс
#define RADIO_RECEIVE_TIMEOUT    10

//#define SNS_REQUESTS_CNT          5                         // Количество повторных запросов (при отсутствии ответа)
//#define SNS_BYTE_TIMEOUT          2                         // Величина таймаута на приём байта, мс
//#define SNS_CLR_TIMEOUT           2                         // Величина таймаута на очистку буфера, мс
//#define SNS_RESPONSE_TIMEOUT      40                        // Величина таймаута на ожидание ответа, мс
//#define SNS_TURN_ON_DELAY         5000                      // Задержка на включение модуля, мс


#define PACKET_SIZE_LIMIT 256
#define RADIO_ADDRESS     0x01

#define RADIO_SEND_PAUSE    1
#define RADIO_REQUEST_DELAY 20
#define RADIO_TRANSACTION_TIMEOUT 150


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
