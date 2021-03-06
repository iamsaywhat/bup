/*******************************************************************************************************

    kmonshelf - Драйвер, предоставляющий функции нижнего уровня для работы с спутниковой 
    навигационной системой (СНС).
							 
    kmonshelf СНС - Комплект модуля ориентации и навигации от АО "Шельф" , работает со спутниками 
    Глонасс и GPS. Позволяет в получать данные геолокации и используется для выполнения полетных 
    заданий. Более подробное описание о данных, предоставляемых эти модулем можно найти в документации
    на устройство (см. так же ниже раздел "Структура ответа  СНС" и описание полей структур
    ответов). СНС поключается к UART порту по RS-232 интерфейсу. 
					 					 
					 
    Настройки UART:
        Скорость передачи 115200 Бод;
        8 бит данных, 
        1 стоповый бит, 
        Без контроля четности. 
        (115200, 8-N-1)
					 
					 
    Особенности работы с СНС:
					 
        1) Тип обмена информацией с СНС - двунаправленный запрос/ответ;
					 
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
					 
        Пример использования:

        SNS_updateDataState ();
        SNS_updateDeviceInformation ();
        SNS_updateOrientationData ();
        SNS_updatePositionData ();
        SNS_getLongitude ();
				
				 		 
*******************************************************************************************************/
#ifndef _SNS_H_
#define _SNS_H_

#include "stdint.h"
 
/************************************************************
  Аппаратнозависимая конфигурация
************************************************************/
#define BAUDRATE_SNS              115200                    // Бит/с скорость обмена с СНС
#define SNS_REQUESTS_CNT          5                         // Количество повторных запросов (при отсутствии ответа)
#define SNS_BYTE_TIMEOUT          2                         // Величина таймаута на приём байта, мс
#define SNS_CLR_TIMEOUT           2                         // Величина таймаута на очистку буфера, мс
#define SNS_RESPONSE_TIMEOUT      40                        // Величина таймаута на ожидание ответа, мс
#define SNS_TURN_ON_DELAY         5000                      // Задержка на включение модуля, мс

/************************************************************
  КОМАНДЫ ЗАПРОСОВ ДАННЫХ
************************************************************/
#define DIR  0x0            // Запрос информации об устройстве
#define DSR  0x1            // Запрос доступных данных
#define PDR  0x2            // Запрос данных местоположения
#define ODR  0x3            // Запрос данных ориентации
#define GCE  0x20           // Команда начала калибровки гироскопа
#define GCR  0x21           // Команда сброса калибровки гироскопа
#define MCE  0x30           // Команда начала калибровки магнитометра
#define MCR  0x31           // Команда сброса калибровки магнитометра
#define HCE  0x32           // Команда включения горизонтальной коррекции
#define HCD  0x33           // Команда выключения горизонтальной коррекции


// Размеры ответов на запросы к СНС
#define SizeAnsDIR    22
#define SizeAnsDSR    15
#define SizeAnsPDR    55
#define SizeAnsODR    21


/************************************************************
    Типы возвращаемых значений    
************************************************************/
typedef enum{
  SNS_OK         = 1,      // Обмен успешно завершен
  SNS_TIMEOUT    = 0,      // Ошибка, истекло время на отправке/приёме
  SNS_WRONG_CRC  = 2,      // Ошибка в контрольной сумме принятого ответа
  SNS_CORPACKET  = 3,      // Пакет испорчен
}SNS_Status;



// Здесь используются объединения (union), для того, чтобы к структуре можно было обратиться как массиву unint8
// Что будет удобно для расчета CRC и заполнения структуры прямиком из FIFO UART - побайтово.
// Хотя можно использовать указатели, для такого же эффекта.
// Есть проблема с выравниванием структуры в памяти, которая решается добавлением  "__packed" перед struct.

/************************************************************
  Структура запроса данных от СНС
************************************************************/
typedef struct{
  uint16_t      CRC;
  uint8_t       Command;
}SNS_Request_Type;

typedef union {    
  SNS_Request_Type Struct;
  uint8_t Buffer[3];
}SNS_Request_Union;

/************************************************************
  Структура ответа  СНС    
************************************************************/
/************************************************************
  Структура ответа: Информация об устройстве     
************************************************************/
typedef __packed struct{
  uint16_t      CRC;                            // CRC16 сообщения начиная со следующего поля включительно
  uint8_t       Response;                       // Тип ответа
  uint8_t       Frame_count;                    // Счетчик фреймов (количество отправленных ответов
  uint8_t       Firmware_version[3];            // Версия программного обеспечения
  uint8_t       Hardware_version[3];            // Версия аппаратного обеспечения
  uint16_t      Serial_number;                  // Серийный номер
  uint64_t      Local_time_stamp;               // Локальная метка времени
  uint8_t       Hardware_state;                 // Состояние основных подсистем
  uint8_t       Firmware_state;                 // Текущий режим работы КМОН
}SNS_Device_Information_Response_Type;

typedef union { 
  SNS_Device_Information_Response_Type Struct;
  uint8_t Buffer[SizeAnsDIR];
}SNS_Device_Information_Response_Union;


/************************************************************
  Структура ответа: Доступные данные    
************************************************************/
typedef __packed struct{
  uint16_t   CRC;                                // CRC16 сообщения начиная со следующего поля включительно
  uint8_t    Response;                           // Тип ответа
  uint8_t    Frame_count;                        // Счетчик фреймов (количество отправленных ответов
  uint8_t    Data_State;                         // Состояние доступных данных указывает на наличие новых измерений
  uint64_t   Local_time_stamp;                   // Локальная метка времени
  uint8_t    Hardware_state;                     // Состояние основных подсистем
  uint8_t    Firmware_state;                     // Текущий режим работы КМОН			
}SNS_Available_Data_Response_Type;

typedef union {
  SNS_Available_Data_Response_Type Struct;
  uint8_t Buffer[SizeAnsDSR];
}SNS_Available_Data_Response_Union;


/************************************************************
  Структура ответа: Данные местоположения    
************************************************************/
typedef __packed struct{
  uint16_t   CRC;                                // CRC16 сообщения начиная со следующего поля включительно
  uint8_t    Response;                           // Тип ответа
  uint8_t    Frame_count;                        // Счетчик фреймов (количество отправленных ответов
  uint8_t    Quality;                            // Статус навигационного решения
  uint8_t    Gln_sat_num;                        // Число спутников ГЛОНАСС участвующих в решении
  uint8_t    Gps_sat_num;                        // Число спутников GPS участвующих в решении
  uint64_t   Time_stamp;                         // Метка времени по сигналам ГНСС - кол-во секунд с 00 часов 00 минут 1 января 1970 года.
  int64_t    Pos_lat;                            // Долгота по сигналам ГНСС. Восточная долгота выражается положительными числами, западная долгота - отрицательными
  int64_t    Pos_lon;                            // Широта по сигналам ГНСС. Северная широта выражается положительными числами, южная широта - отрицательными.
  int64_t    Pos_alt;                            // Высота над уровнем моря по сигналам ГНСС
  int32_t    Course;                             // Путевой угол по сигналам ГНСС
  int32_t    Vel_lat;                            // Скорость по широте
  int32_t    Vel_lon;                            // Скорость по долготе
  int32_t    Vel_alt;                            // Скорость по высоте
}SNS_Position_Data_Response_Type;	

typedef union {
  SNS_Position_Data_Response_Type Struct;
  uint8_t Buffer[SizeAnsPDR];
}SNS_Position_Data_Response_Union;	


/************************************************************
  Структура ответа: Данные ориентации
************************************************************/
typedef __packed struct{
  uint16_t    CRC;                                // CRC16 сообщения начиная со следующего поля включительно
  uint8_t     Response;                           // Тип ответа
  uint8_t     Frame_count;                        // Счетчик фреймов (количество отправленных ответов
  int32_t     Heading_true;                       // Истинный курс, полученный на основе магнитного, с учетом склонения расчитанного по последнему решению ГНСС приемника.
  int32_t     Heading_mgn;                        // Магнитный курс
  int32_t     Pitch;                              // Тангаж
  int32_t     Roll;                               // Крен
  uint8_t     Hor_corr_EN;                        // Горизонтальная коррекция по показаниям магнитометра (1-вкл/0-выкл).
}SNS_Orientation_Data_Response_Type;	
	
typedef union { 
  SNS_Orientation_Data_Response_Type Struct;
  uint8_t Buffer[SizeAnsODR];
}SNS_Orientation_Data_Response_Union;

/**************************************************************
  Глобальные экземпляры данных от СНС в принимаемом виде
**************************************************************/
extern SNS_Device_Information_Response_Union   SNS_deviceInformation;
extern SNS_Available_Data_Response_Union       SNS_availableData;
extern SNS_Position_Data_Response_Union        SNS_position;
extern SNS_Orientation_Data_Response_Union     SNS_orientation;

/**************************************************************************************************************
  SNS_updateDeviceInformation - Обновление информации о девайсе от СНС
  Параметры:
            SNS_DeviceInformation - структура пакета данных ответа на запрос DIR.
  Возвращает:
            SNS_OK        - Если информация успешно получена;
            SNS_TIMEOUT   - Время на отправку запроса или приёма ответа вышло;
            SNS_WRONG_CRC - Неверная контрольная сумма на приёме.
***************************************************************************************************************/
SNS_Status SNS_updateDeviceInformation(void);

/**************************************************************************************************************
  SNS_updateDataState - Обновление информации о доступных данных от СНС
  Параметры:
            SNS_DataState - структура пакета данных ответа на запрос DSR.
  Возвращает:
            SNS_OK        - Если информация успешно получена;
            SNS_TIMEOUT   - Время на отправку запроса или приёма ответа вышло;
            SNS_WRONG_CRC - Неверная контрольная сумма на приёме.
***************************************************************************************************************/
SNS_Status SNS_updateDataState (void);

/**************************************************************************************************************
  SNS_updatePositionData - Обновление информации о местоположении от СНС
  Параметры:
            SNS_PositionData - структура пакета данных ответа на запрос PDR.
  Возвращает:
            SNS_OK        - Если информация успешно получена;
            SNS_TIMEOUT   - Время на отправку запроса или приёма ответа вышло;
            SNS_WRONG_CRC - Неверная контрольная сумма на приёме.
***************************************************************************************************************/
SNS_Status SNS_updatePositionData (void);

/**************************************************************************************************************
  SNS_updateOrientationData - Обновление информации об ориентации от СНС
  Параметры:
            SNS_OrientationData - структура пакета данных ответа на запрос PDR.
  Возвращает:
            SNS_OK        - Если информация успешно получена;
            SNS_TIMEOUT   - Время на отправку запроса или приёма ответа вышло;
            SNS_WRONG_CRC - Неверная контрольная сумма на приёме.
***************************************************************************************************************/
SNS_Status SNS_updateOrientationData (void);

/**************************************************************************************************************
  SNS_startGyroCalibration - Команда начала калибровки гироскопа
  Возвращает:
            SNS_OK        - Если информация успешно получена;
            SNS_TIMEOUT   - Время на отправку запроса или приёма ответа вышло.
***************************************************************************************************************/
SNS_Status SNS_startGyroCalibration (void);

/**************************************************************************************************************
  SNS_resetGyroCalibration - Команда сброса калибровки гироскопа
  Возвращает:
            SNS_OK        - Если информация успешно получена;
            SNS_TIMEOUT   - Время на отправку запроса или приёма ответа вышло.
***************************************************************************************************************/
SNS_Status SNS_resetGyroCalibration (void);

/**************************************************************************************************************
  SNS_startMagnetometerCalibration - Команда начала калибровки магнитометра
  Возвращает:
            SNS_OK        - Если информация успешно получена;
            SNS_TIMEOUT   - Время на отправку запроса или приёма ответа вышло. 
***************************************************************************************************************/
SNS_Status SNS_startMagnetometerCalibration (void);

/**************************************************************************************************************
  SNS_resetMagnetometerCalibration - Команда сброса калибровки магнитометра
  Возвращает:
            SNS_OK        - Если информация успешно получена;
            SNS_TIMEOUT   - Время на отправку запроса или приёма ответа вышло.
***************************************************************************************************************/
SNS_Status SNS_resetMagnetometerCalibration (void);

/**************************************************************************************************************
  SNS_enableHorizontalCorrection - Команда включения горизонтальной коррекции
  Возвращает:
            SNS_OK        - Если информация успешно получена;
            SNS_TIMEOUT   - Время на отправку запроса или приёма ответа вышло.
***************************************************************************************************************/
SNS_Status SNS_enableHorizontalCorrection (void);

/**************************************************************************************************************
  SNS_disableHorizontalCorrection - Команда отключения горизонтальной коррекции
  Возвращает:
            SNS_OK        - Если информация успешно получена;
            SNS_TIMEOUT   - Время на отправку запроса или приёма ответа вышло.
***************************************************************************************************************/
SNS_Status SNS_disableHorizontalCorrection (void);

/**************************************************************************************************************
  SNS_getLatitude - Получить широту, град   
***************************************************************************************************************/
double SNS_getLatitude(void);

/**************************************************************************************************************
  SNS_getLongitude - Получить долготу, град  
***************************************************************************************************************/
double SNS_getLongitude(void);

/**************************************************************************************************************
  SNS_getAltitude - Получить высоту, м   
***************************************************************************************************************/
double SNS_getAltitude(void);

/**************************************************************************************************************
  SNS_getGroundTrack - Получить путевой курс по сигналам спутника, рад  
***************************************************************************************************************/
double SNS_getGroundTrack(void);

/**************************************************************************************************************
  SNS_getTrueCourse - Получить истиный курс, рад
***************************************************************************************************************/
double SNS_getTrueCourse(void);

/**************************************************************************************************************
  SNS_getMagnetCourse - Получить магнитный курс, рад
***************************************************************************************************************/
double SNS_getMagnetCourse(void);

/**************************************************************************************************************
  SNS_getLatitudeVelocity - Получить скорость по широте, м/с
***************************************************************************************************************/
double SNS_getLatitudeVelocity(void);

/**************************************************************************************************************
  SNS_getLongitudeVelocity - Получить скорость по долготе, м/с
***************************************************************************************************************/
double SNS_getLongitudeVelocity(void);

/**************************************************************************************************************
  SNS_getAltitudeVelocity - Получить скорость по высоте, м/с
***************************************************************************************************************/
double SNS_getAltitudeVelocity(void);

/**************************************************************************************************************
  SNS_getPitch - Получить угол тангажа, рад
***************************************************************************************************************/
double SNS_getPitch(void);

/**************************************************************************************************************
  SNS_getPitch - Получить угол крена, рад
***************************************************************************************************************/
double SNS_getRoll(void);

#endif
