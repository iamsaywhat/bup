#include "SelfTesting.h"

#include "discrete_io.h"
#include "1636PP52Y.h"
#include "25Q64FV.h"
#include "SWS.h"
#include "SNS.h"
#include "BIM.h"
#include "Log_FS/Log_FS.h"
#include "HeightMap/Heightmap_conf_and_pd.h"


/***************************************
      Состояние системы
***************************************/
uint16_t SystemState = 0;


/************************************************************************************
				SelfTestingFull - Тестирование всей периферии и програмных модулей
				
				Примечание: Запускает все ниже описанные функции
************************************************************************************/
void SelfTestingFull (void)
{
	// Проверка 4х внешних SPI-flash от Миландра
	SelfTesting_1636PP52Y();
	// Проверка SPI-flash от WINBOND
	SelfTesting_25Q64FV();
	// Загружена ли карта и полетное задание
	SelfTesting_MapNtask();
	// Проверка файловой системы
	SelfTesting_LogFs();
	// Проверка левого БИМа
	SelfTesting_LEFT_BIM();
	// Проверка правого БИМа
	SelfTesting_RIGHT_BIM();
	// Проверка состояния шпильки 1
	SelfTesting_PIN1();
	// Проверка состояния шпильки 2
	SelfTesting_PIN2();
	// Проверка состояния реле пиропатрона
	SelfTesting_PYRO();
	// Проверка состояния шторки
	SelfTesting_BLIND();
	// Проверка связи с SNS
	SelfTesting_SNS();
	// Проверка связи с SWS
	SelfTesting_SWS();
}


/************************************************************************************
       SelfTestingOnline - Оперативная проверка состояния системы

       Примечание: должна периодически запускаться, например, по таймеру  
************************************************************************************/
void SelfTestingOnline (void)
{
	// Проверка 4х внешних SPI-flash от Миландра
	SelfTesting_1636PP52Y();
	// Проверка SPI-flash от WINBOND
	SelfTesting_25Q64FV();
	// Загружена ли карта и полетное задание
	SelfTesting_MapNtask();
	// Проверка левого БИМа
	SelfTesting_LEFT_BIM();
	// Проверка правого БИМа
	SelfTesting_RIGHT_BIM();
	// Проверка состояния шпильки 1
	SelfTesting_PIN1();
	// Проверка состояния шпильки 2
	SelfTesting_PIN2();
	// Проверка состояния реле пиропатрона
	SelfTesting_PYRO();
	// Проверка состояния шторки
	SelfTesting_BLIND();
}

/************************************************************************************
				SelfTesting_1636PP52Y - Тестирование 4х внешних SPI-flash от Миландра
				
				Возвращает: ST_OK    - Если память работает
				            ST_FAULT - Если память недоступна
************************************************************************************/
SelfTesing_STATUS SelfTesting_1636PP52Y(void)
{
  // Проверяем просто по ID микросхемы (если ID не совпадёт - это признак неисправности)
	SelfTesting_SET_OK(ST_1636PP52Y);
	if(SPI_1636PP52Y_ReadID (SPI_1636PP52Y_CS1) != _1636PP52Y_ID)
		SelfTesting_SET_FAULT(ST_1636PP52Y); 
	if(SPI_1636PP52Y_ReadID (SPI_1636PP52Y_CS2) != _1636PP52Y_ID)
   	SelfTesting_SET_FAULT(ST_1636PP52Y); 	
	if(SPI_1636PP52Y_ReadID (SPI_1636PP52Y_CS3) != _1636PP52Y_ID)
		SelfTesting_SET_FAULT(ST_1636PP52Y); 
	if(SPI_1636PP52Y_ReadID (SPI_1636PP52Y_CS4) != _1636PP52Y_ID)
		SelfTesting_SET_FAULT(ST_1636PP52Y); 
	
	return (SelfTesing_STATUS)SelfTesting_STATUS(ST_1636PP52Y); 
}


/************************************************************************************
				SelfTesting_25Q64FV - Тестирование SPI-flash от WINBOND
				
				Возвращает: ST_OK    - Если память работает
				            ST_FAULT - Если память недоступна
************************************************************************************/
SelfTesing_STATUS SelfTesting_25Q64FV(void)
{
	// Проверяем просто по ID микросхемы (если ID не совпадёт - это признак неисправности)
	SelfTesting_SET_FAULT(ST_25Q64FV); 
	if(SPI_25Q64FV_ReadID (SPI_25Q64FV_CSn) == _25Q64FV_ID)
		SelfTesting_SET_OK(ST_25Q64FV);
	
	return (SelfTesing_STATUS)SelfTesting_STATUS(ST_25Q64FV);
}


/************************************************************************************
        SelfTesting_MapNtask - Проверка наличия карты рельефа и полетного задания
				
				Возвращает: ST_OK    - Если задание и карта загружены
				            ST_FAULT - Если карта и задание отсутствует или повреждены
************************************************************************************/
SelfTesing_STATUS SelfTesting_MapNtask(void)
{
	if(CheckMap())
		SelfTesting_SET_OK(ST_MAP);
	else 
		SelfTesting_SET_FAULT(ST_MAP);
	
	return (SelfTesing_STATUS)SelfTesting_STATUS(ST_MAP);
}


/************************************************************************************
        SelfTesting_LogFs - Проверка готовности файловой системы
				
				Возвращает: ST_OK    - Если файловая система исправна
				            ST_FAULT - Если форматирование повреждено
************************************************************************************/
SelfTesing_STATUS SelfTesting_LogFs(void)
{
	// Если файловая система ответила FINE - она готова к работе
	if(LogFs_Info() == FS_FINE)
		SelfTesting_SET_OK(ST_LogFS);
	// Иначе разметка файловой системы повреждена, либо проблемы с spi-flash 
	else
		SelfTesting_SET_FAULT(ST_LogFS);
	
	return (SelfTesing_STATUS)SelfTesting_STATUS(ST_LogFS);
}


/************************************************************************************
        SelfTesting_LEFT_BIM - Проверка исправности левого БИМа
				
				Возвращает: ST_OK    - Если БИМ исправен
				            ST_FAULT - Если БИМ неисправен, либо связь отсутствует
************************************************************************************/
SelfTesing_STATUS SelfTesting_LEFT_BIM(void)
{
	if(BIM_CheckConnection (LEFT_BIM))
	  SelfTesting_SET_OK(ST_Left_BIM);
	else 
		SelfTesting_SET_FAULT(ST_Left_BIM);
	
	return (SelfTesing_STATUS)SelfTesting_STATUS(ST_Left_BIM);
}


/************************************************************************************
        SelfTesting_RIGHT_BIM - Проверка исправности правого БИМа
				
				Возвращает: ST_OK    - Если БИМ исправен
				            ST_FAULT - Если БИМ неисправен, либо связь отсутствует
************************************************************************************/
SelfTesing_STATUS SelfTesting_RIGHT_BIM(void)
{
	if(BIM_CheckConnection (RIGHT_BIM))
	  SelfTesting_SET_OK(ST_Right_BIM);
	else 
		SelfTesting_SET_FAULT(ST_Right_BIM);
	
	return (SelfTesing_STATUS)SelfTesting_STATUS(ST_Right_BIM);
}


/************************************************************************************
				SelfTesting_PIN1 - Проверка состояния шпильки 1
				
				Возвращает: ST_OK    - Если шпилька 1 вставлена
				            ST_FAULT - Если шпилька 1 отсутствует
************************************************************************************/
SelfTesing_STATUS SelfTesting_PIN1(void)
{
	if(PIN1_CHECK)
		SelfTesting_SET_OK(ST_pin1);
	else
		SelfTesting_SET_FAULT(ST_pin1);
	
	return (SelfTesing_STATUS)SelfTesting_STATUS(ST_pin1);
}


/************************************************************************************
				SelfTesting_PIN2 - Проверка состояния шпильки 2
				
				Возвращает: ST_OK    - Если шпилька 2 вставлена
				            ST_FAULT - Если шпилька 2 отсутствует
************************************************************************************/
SelfTesing_STATUS SelfTesting_PIN2(void)
{
	if(PIN2_DIR_CHECK && !PIN2_INV_CHECK)
		SelfTesting_SET_OK(ST_pin2);
	else
		SelfTesting_SET_FAULT(ST_pin2);
	
	return (SelfTesing_STATUS)SelfTesting_STATUS(ST_pin2);
}


/************************************************************************************
				SelfTesting_PYRO - Проверка состояния реле пиропатрона
				
				Возвращает: ST_OK    - Если реле пиропатрона включено
				            ST_FAULT - Если реле пиропатрона выключено
************************************************************************************/
SelfTesing_STATUS SelfTesting_PYRO(void)
{
	if(PYRO_CHECK)
		SelfTesting_SET_OK(ST_pyro);
  else
		SelfTesting_SET_FAULT(ST_pyro);
	
	return (SelfTesing_STATUS)SelfTesting_STATUS(ST_pyro);
}


/************************************************************************************
        SelfTesting_BLIND - Проверка состояния реле створки
				
				Возвращает: ST_OK    - Если реле створки включено
				            ST_FAULT - Если реле створки выключено
************************************************************************************/
SelfTesing_STATUS SelfTesting_BLIND(void)
{
	if(BLIND_CTRL_CHECK && BLIND_CHECK)
		SelfTesting_SET_OK(ST_blind);
	else
		SelfTesting_SET_FAULT(ST_blind);
	
	return (SelfTesing_STATUS)SelfTesting_STATUS(ST_blind);
}


/************************************************************************************
        SelfTesting_SNS - Проверка связи с СНС
				
				Возвращает: ST_OK    - Если связь с СНС есть
				            ST_FAULT - Если связь с СНС отсутствует
************************************************************************************/
SelfTesing_STATUS SelfTesting_SNS(void)
{
	SNS_Device_Information_Response_Union SNS_DeviceInformation;
	
	if(SNS_GetDeviceInformation(&SNS_DeviceInformation))
		SelfTesting_SET_OK(ST_sns);
	else
		SelfTesting_SET_FAULT(ST_sns);
	
	return (SelfTesing_STATUS)SelfTesting_STATUS(ST_sns);
}


/************************************************************************************
        SelfTesting_SWS - Проверка связи с СВС
				
				Возвращает: ST_OK    - Если связь с СВС есть
				            ST_FAULT - Если связь с СВС отсутствует
************************************************************************************/
SelfTesing_STATUS SelfTesting_SWS(void)
{
	SWS_Packet_Type_Union SWS_Pack;
	
	if(SWS_GetPacket (&SWS_Pack))
		SelfTesting_SET_FAULT(ST_sws);
	else
		SelfTesting_SET_OK(ST_sws);
	
	return (SelfTesing_STATUS)SelfTesting_STATUS(ST_sws);
}




