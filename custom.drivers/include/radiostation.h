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


typedef __packed struct{
	uint8_t  index;
	uint8_t  address;
	uint16_t length;
	uint16_t crc;
}RadioBaseFrame;

typedef __packed struct{
  uint8_t  type;
	uint16_t length;
	uint8_t* data;
}RadioDataFrame;

typedef union { 
  RadioBaseFrame Struct;
  uint8_t Buffer[6];
}RadioBaseFrameType;

typedef union { 
  RadioDataFrame Struct;
  uint8_t        Buffer[7];
}RadioDataFrameType;

typedef enum{
  RADIO_WRONG_CRC,
	RADIO_WRONG_INDEX,
	RADIO_TIMEOUT,
	RADIO_SUCCESS,
	RADIO_FAILED,
}RadioStatus;


void Radio_send(uint8_t index, uint8_t *data, uint8_t size);


void getDeviceName(void);
void sendEmpty(void);


#endif
