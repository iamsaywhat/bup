#ifndef _MAP_FLASH_LAYOUT_
#define _MAP_FLASH_LAYOUT_

/* Адреса, по которым лежит информация по карте */
#define ADDRESS_TDP_LAT             0          // Адрес, по которому лежит ШИРОТА точки приземления
#define ADDRESS_TDP_LON             8          // Адрес, по которому лежит ДОЛГОТА точки приземления
#define ADDRESS_TDP_ALT             16         // Адрес, по которому лежит ВЫСОТА РАСКРЫТИЯ ПАРАШЮТА
#define ADDRESS_PACKET_NUM          18         // Адрес, по которому лежит КОЛИЧЕСТВО ПАКЕТОВ с картой
#define ADDRESS_LAT_COUNT           20         // Адрес, по которому лежит ЧИСЛО ШИРОТ ПАРАШЮТА
#define ADDRESS_LON_COUNT           22         // Адрес, по которому лежит ЧИСЛО ДОЛГОТ ПАРАШЮТА
#define ADDRESS_NULL_POINT_LAT      24         // Адрес, по которому лежит ШИРОТА НУЛЬ ТОЧКИ - Левая нижняя на карте
#define ADDRESS_NULL_POINT_LON      32         // Адрес, по которому лежит ДОЛГОТА НУЛЬ ТОЧКИ - Левая нижняя на карте
#define ADDRESS_STEP_LAT            40         // Адрес, по которому лежит ШАГ СЕТКИ ПО ШИРОТЕ
#define ADDRESS_STEP_LON            48         // Адрес, по которому лежит ШАГ СЕТКИ ПО ДОЛГОТЕ
#define ADDRESS_MAP_TAG             320056     // Адрес, по которому лежит ПРИЗНАК ЗАГРУЖЕНОСТИ КАРТЫ
#define ADDRESS_FIRST_ALT           56         // Адрес, по которому лежит НАЧАЛО ПЕРВОГО ПАКЕТА с высотами
#define ADDRESS_MAP_END             320060     // Адрес, с которого начинается незанятое пространство памяти
#define ADDRESS_0_PACKET            0          // Адрес по которому лежит информация из нулевого пакета (точка прилемления, масштабы карты)
#define ADDRESS_0_PACKET_END        55         // Конечный адрес по которому лежит информация из нулевого пакета (точка прилемления, масштабы карты)

/* Макрос, для быстрого получения адреса, по которому лежит i - й пакет с картой (i = 1...400) */
#define ADDRESS_n_PACKET_MAP(x)               (ADDRESS_0_PACKET_END+1+800*(x-1))    

/* Размеры пакетов */
#define SIZE_OF_0_PACKET_DATA       56         // Байт, только информационная часть пакета 0
#define SIZE_OF_0_PACKET_FULL       66         // Байт, полный размер пакета 0
#define SIZE_OF_n_PACKET_DATA       800        // Байт, только информационная часть пакета MAP
#define SIZE_OF_n_PACKET_FULL       810        // Байт, полный размер пакета MAP

#define NUMBER_OF_MAP_PACKET        400        // Количество количество пакетов
#define NUMBER_OF_DATA_IN PACKET    400        // Количество высот в одном пакете 

/* Признак наличия карты */
#define MAP_TAG                  0x4F4F4F4F    // Признак наличия карты


#endif // _MAP_FLASH_LAYOUT_
