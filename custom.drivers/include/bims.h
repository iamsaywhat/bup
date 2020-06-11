/****************************************************************************************************

    bims - Драйвер, предоставляющий функции нижнего уровня для работы с БИМами.
					
    БИМ - Блок исполнительного механизма, двигатели с постоянными магнитами 
    с редуктором. Управление выполняется по CAN шине. 
					
    Особенности работы с БИМ:
    1) Тип обмена информацией с СНС - двунаправленный запрос/ответ;
    2) Скорость обмена по CAN 125кбит/с
    3) Имеется управление реле питания БИМов 
    Реализовано через макросы BIM_Supply_ON(); и BIM_Supply_OFF;
    4) Инициализирует 0-15 буфера на приём сообщений, 16-31 на передачу
    5) 0, 1 буфера резервируются этим драйвером (настроен фильтр на
    приём сообщений только от БИМ): 0 буфер - 100 ID - Левый БИМ
                                    1 буфер - 101 ID - Правый БИМ;
    Остальные доступны для использования сторонними модулями
					
    Пример работы с БИМ:
					
    // Настраиваем используемые пины 
    BIM_RetargetPins();
    // Конфигурируем CAN модуль
    BIM_CAN_init();
    // Подаём питание и разрешаем работу
    BIM_Supply_ON();
					
    // Посылаем запрос на управление: устройство 100 включить, положение стропы 255, 
    // счетчик запроса 6 (любой может быть), 
    // ограничение скорости 255, ограничение тока 200
    BIM_SendRequest (DEVICE_100, BIM_CMD_ON, 255, 6, 255, 200);
    // То же самое для устройства 101
    BIM_SendRequest (DEVICE_101, BIM_CMD_ON, 255, 6, 255, 200);		

****************************************************************************************************/
#ifndef _BIM_H_
#define _BIM_H_

#include <stdint.h>
#include "bupboard.h"

/************************************************************
  Аппаратно зависимая конфигурация
************************************************************/	
#define BAUDRATE_BIM              125000                // Cкорость обмена по CAN, бит/c
#define BIM_SEND_TIMEOUT          2                     // Таймаут на отправку сообщения, мс
#define BIM_RECEIVE_TIMEOUT       3                     // Таймаут на приём ответа, мс
#define BUFFER_200                0
#define BUFFER_201                1


/************************************************************
  Адреса устройств БИМ на CAN ШИНЕ
************************************************************/
typedef enum{
  DEVICE_100    = 0x100,    // Левый БИМ
  DEVICE_101    = 0x101,    // Правый БИМ
  LEFT_BIM      = 0x100,    // Левый БИМ
  RIGHT_BIM     = 0x101,    // Правый БИМ
}Bim_devices;


/************************************************************
  Список команд управления БИМами
************************************************************/
typedef enum{
  BIM_CMD_OFF    = 0x00,    // Команда выключения          
  BIM_CMD_ON     = 0x01,    // Команда включения
  BIM_CMD_REQ    = 0x0F,    // Запрос ответа
}Bim_command;

typedef enum{
  BIM_DONE,
  BIM_ERROR,
  BIM_ALREADY_ON_POSITION,
  BIM_NEED_TO_STOP,
  BIM_DONT_NEED_TO_STOP,
}Bim_status;

/************************************************************
  Макросы для выделения флагов состояния
************************************************************/
typedef enum {
  SENSOR_FAULT      = 0x0001,  // Отказ датчика положения 
  OVERCURRENT       = 0x0002,  // Превышение тока преобразователя 
  OVERVOLT          = 0x0004,  // Превышение силовое питание
  UNDER_VOLT        = 0x0008,  // Недостаточное силовое питание
  OVERTEMPERATURE   = 0x0010,  // Перегрев двигателя
  OVERLOAD          = 0x0020,  // Перегруз двигателя
  POSITION_ERR      = 0x0080,  // Флаг рассогласования положения
  HALT_OK           = 0x0100,  // Преобразователь выключен
  RESERVED          = 0x7E00,  // Резерв
  READY             = 0x8000,  // Флаг готовности (исправности)
}Bim_flags;

#define CHECK_SENSOR_FAULT(x)       ((x&SENSOR_FAULT))           // Выделение флага "Отказ датчика положения"
#define CHECK_OVERCURRENT(x)        ((x&OVERCURRENT) >> 1)       // Выделение флага "Превышение тока преобразователя" 
#define	CHECK_OVERVOLT(x)           ((x&OVERVOLT) >> 2)          // Выделение флага "Превышение силовое питание"
#define CHECK_UNDER_VOLT(x)         ((x&UNDER_VOLT) >> 3)        // Выделение флага "Недостаточное силовое питание"
#define CHECK_OVERTEMPERATURE(x)    ((x&OVERTEMPERATURE) >> 4)   // Выделение флага "Перегрев двигателя"
#define CHECK_OVERLOAD(x)           ((x&OVERLOAD) >> 5)          // Выделение флага "Перегруз двигателя"
#define CHECK_POSITION_ERR(x)       ((x&POSITION_ERR) >> 7)      // Выделение флага "Флаг рассогласования положения"
#define CHECK_HALT_OK(x)            ((x&HALT_OK) >> 8)           // Выделение флага "Преобразователь выключен"
#define CHECK_READY(x)              ((x&READY) >> 15)            // Выделение флага "Флаг готовности (исправности)"

/************************************************************
  Структура ответа BIM
************************************************************/
typedef __packed struct{
  uint16_t  StatusFlags;        // Флаги состояния
  uint8_t   StrapPosition;      // Значение положения стропы
  uint8_t   ReqCount;           // Счетчик запроса
  uint8_t   Voltage;            // Напряжение питания
  int8_t    Current;            // Ток двигателя
  int8_t    Speed;              // Скорость двигателя
  uint8_t   Reserve;            // Резерв
}BIM_Response_Type;

typedef __packed union {
  BIM_Response_Type Struct;
  uint32_t          Buffer[2];
}BIM_Response_UnionType;

/**************************************************************************************************************
  BIM_enableSupply() - Макрос подачи питания на БИМы
**************************************************************************************************************/
#define BIM_enableSupply()     Pin_set(RELAY_BIM)

/**************************************************************************************************************
  BIM_disableSupply() - Макрос отключения питания на БИМа
**************************************************************************************************************/
#define BIM_disableSupply()    Pin_reset(RELAY_BIM)

/**************************************************************************************************************
  BIM_CAN_initialize - Инициализация драйвера для работы с БИМами по CAN протоколу
  Параметры:  NONE	
**************************************************************************************************************/
void BIM_CAN_initialize (void);

/**************************************************************************************************************
  BIM_controlCommand - Команда управления положением БИМа
  Параметры:
            DeviceID 	- Идентификатор БИМ-устройства
            StrapPosition  - Положение стропы (0..255)            
  Возвращает: 
            BIM_ERROR               - Команда не выполнена
            BIM_DONE                - Команда успешно выполнена	 	
            BIM_ALREADY_ON_POSITION - БИМ уже находится в положении StrapPosition         
**************************************************************************************************************/
Bim_status BIM_controlCommand (uint16_t DeviceID, uint8_t StrapPosition);

/**************************************************************************************************************
  BIM_stopCommand - Команда на остановку БИМа
  Параметры:
            DeviceID 	- Идентификатор БИМ-устройства												
  Возвращает: 
            BIM_ERROR - Команда не выполнена
            BIM_DONE  - Команда успешно выполнена	 		
**************************************************************************************************************/
Bim_status BIM_stopCommand (uint16_t DeviceID);

/**************************************************************************************************************
  BIM_updateCommand - Обновление данных о состоянии БИМа
  Параметры:
            DeviceID 	- Идентификатор БИМ-устройства												
  Возвращает: 
            BIM_ERROR - Обновление данных не выполнено
            BIM_DONE  - Данные о состоянии БИМа обновлены	 		
**************************************************************************************************************/
Bim_status BIM_updateCommand (uint16_t DeviceID);

/**************************************************************************************************************
  BIM_checkNeedToStop - Проверка необходимости предварительно остановить БИМ перед командой управления
  Параметры:
            DeviceID 	- Идентификатор БИМ-устройства
            StrapPosition  - Положение стропы (0..255)  												
  Возвращает: 
            BIM_DONT_NEED_TO_STOP - БИМ перед отправкой этой команды не нужно остановить
            BIM_NEED_TO_STOP - БИМ перед отправкой этой команды необходимо остановить	 	
  Примечание:
            Эта функция реализует функционал который почему-то не был заложен разработчкиком БИМ.
            Если БИМ вращается в одном направлении, а приходит команда, согласно которой БИМ должен 
            начать вращаться в другом направлении, то БИМ предварительно не останавливается,
            а включается в реверсе, при этом отдаёт противо-ЭДС в сеть, и по сумме напряжений получает
            превышение питания, из-за чего повисает. 
**************************************************************************************************************/
Bim_status BIM_checkNeedToStop (uint16_t DeviceID, uint8_t StrapPosition);

/**************************************************************************************************************
  BIM_sendRequest - Функция отправки запроса БИМам с командами
  Параметры:
            DeviceID       - Адрес БИМа на CAN шине
            CMD            - Команда (BIM_CMD_ON, BIM_CMD_OFF или BIM_CMD_REQ)
            StrapPosition  - Положение стропы (0..255)
            ReqCount       - Счетчик опроса (индентификатор запроса)
            SpeedLimit     - Ограничение скорости двигателя (0..255)
            CurrentLimit   - Ограничение тока (0..255)
    
  Возвращает: 
            BIM_ERROR - Если ошибка при передаче (таймаут передачи)
            BIM_DONE  - Если отправлено успешно	 		
**************************************************************************************************************/
Bim_status BIM_sendRequest (uint16_t DeviceID, 
                            uint8_t  CMD, 
                            uint8_t  StrapPosition, 
                            uint8_t  ReqCount, 
                            uint8_t  SpeedLimit, 
                            uint8_t  CurrentLimit);

/**************************************************************************************************************
  BIM_checkConnection - Проверка связи с БИМами
  Параметры:
            DeviceID 	- Идентификатор БИМ-устройства												
  Возвращает: 
            BIM_ERROR - Связь нарущена
            BIM_DONE  - Связь с БИМом в порядке	 		
**************************************************************************************************************/
Bim_status BIM_checkConnection (uint16_t DeviceID);

/**************************************************************************************************************
  BIM_getStrapPosition - Получить текущее положение стропы
  Параметры:
            DeviceID 	- Идентификатор БИМ-устройства.
  Возвращает: Положение стропы устройства с DeviceID.
**************************************************************************************************************/
uint8_t BIM_getStrapPosition (uint16_t DeviceID);

/**************************************************************************************************************
  BIM_getVoltage - Получить текущее значение напряжения
  Параметры:
            DeviceID 	- Идентификатор БИМ-устройства.
  Возвращает:
            Текущее значение напряжения.
**************************************************************************************************************/
uint8_t BIM_getVoltage (uint16_t DeviceID);

/**************************************************************************************************************
  BIM_getCurrent - Получить текущее значение тока
  Параметры:
            DeviceID 	- Идентификатор БИМ-устройства
  Возвращает:
            Текущее значение тока
**************************************************************************************************************/
int8_t BIM_getCurrent (uint16_t DeviceID);

/**************************************************************************************************************
  BIM_getSpeed - Получить текущее значение скорости
  Параметры:
            DeviceID 	- Идентификатор БИМ-устройства.
  Возвращает:
            Текущее значение скорости.
**************************************************************************************************************/
int8_t BIM_getSpeed (uint16_t DeviceID);

/**************************************************************************************************************
  BIM_getStatusFlags - Получить актуальные флаги состояния устройства  
  Параметры:
            DeviceID 	- Идентификатор БИМ-устройства.
  Возвращает:
            Актуальные флаги состояния устройства.
**************************************************************************************************************/
uint16_t BIM_getStatusFlags (uint16_t DeviceID);

#endif
