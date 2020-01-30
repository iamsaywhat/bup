/**************************************************************************************************************
  crc32 - Модуль расчета контрольной суммы.
				
  Метод:     Табличный CRC32
  Полином:    0x04C11DB7
  Начальное значение:   0xFFFFFFFF
***************************************************************************************************************/
#ifndef _CRC32_H_
#define _CRC32_H_


/**************************************************************************************************************
  CRC32_calculate - Функция вычисления контрольной суммы
  Параметры:
            data – массив с данными;
            data_index – размер массива data в байтах.
***************************************************************************************************************/
unsigned int CRC32_calculate(unsigned char *data, unsigned int data_index);

#endif
