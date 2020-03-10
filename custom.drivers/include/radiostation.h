#ifndef _RADIOSTATION_H_
#define _RADIOSTATION_H_

#include <stdint.h>

/************************************************************
  Аппаратнозависимая конфигурация
************************************************************/
#define RADIO_BAUDRATE           9600          // Бит/с скорость обмена с СНС
#define RADIO_BYTE_TIMEOUT       1             // Величина таймаута на приём байта, мс
#define RADIO_FIFO_TIMEOUT       2             // Величина таймаута на очистку FIFO, мс
#define RADIO_RECEIVE_TIMEOUT    2

//#define SNS_REQUESTS_CNT          5                         // Количество повторных запросов (при отсутствии ответа)
//#define SNS_BYTE_TIMEOUT          2                         // Величина таймаута на приём байта, мс
//#define SNS_CLR_TIMEOUT           2                         // Величина таймаута на очистку буфера, мс
//#define SNS_RESPONSE_TIMEOUT      40                        // Величина таймаута на ожидание ответа, мс
//#define SNS_TURN_ON_DELAY         5000                      // Задержка на включение модуля, мс


typedef __packed struct{
	uint8_t  index;
	uint8_t  address;
	uint16_t lenght;
	uint8_t  dataType;
	uint16_t dataLenght;
	uint16_t crc;
}RadioBaseFrame;

typedef union { 
  RadioBaseFrame Struct;
  uint8_t Buffer[9];
}RadioBaseFrameType;




void getDeviceName(void);


#endif
