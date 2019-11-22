#include "RetargetPrintf.h"
#include "../Log_FS/Log_FS.h"



/*********************************************************************************
              Глобальный переключатель назначения printf
*********************************************************************************/
char SelectedTarget;


/*********************************************************************************
              Структура отправляемого сообщения в CAN
*********************************************************************************/
CAN_TxMsgTypeDef TxMsg_CanPrintf = {
	0,
	DISABLE, 
	CAN_ID_STD, 
	8, 
	0, 
	0
};


/*********************************************************************************
    Объявления приватных функций модуля
*********************************************************************************/
static int sendchar_Log_FS(int ch);
static int sendchar_CAN(int ch);


/*******************************************************************************
@brief  printf_switcher: Переключатель места вывода printf();
@param  TO_CAN - printf() выводит в CAN
				TO_LOG - printf() записывает в Лог
@retval NONE
@notes  Маскирует под функцию присвоение глобальной переменной данного 
        режима работы. (Для лучшей читаемости).
*******************************************************************************/
void printf_switcher (char trg, unsigned short id)
{
	SelectedTarget = trg;
	TxMsg_CanPrintf.ID = CAN_STDID_TO_EXTID(id);
}	




/*******************************************************************************
@brief  sendchar: Функция селектор направления вывода
@param  ch - символ для вывода
@retval NONE
@notes  Выбирает куда выводить символ, в зависимости от параметра 
        предварительно вызванной функции printf_switcher()
        Вызов этой функции происходит примерной так:
        printf() -> fputc() -> sendchar () -> sendchar_CAN();
                                           -> sendchar_Log_FS();
*******************************************************************************/
int sendchar(int ch)
{
	switch (SelectedTarget)
	{
		case TO_CAN:
			sendchar_CAN(ch);
			break;
		case TO_LOG:
			sendchar_Log_FS(ch);
			break;
		default:
			break;
	}
	
	return 0;
}




/*******************************************************************************
@brief  sendchar_CAN: Переправления вывода в СAN (вызывается из printf);
@param  int ch - символ для вывода
@retval 0 - Всегда
@notes  Реализовано ограничение в 80 символов за обращение в функции printf 
        с использованием буферизации, поэтому передаваемую функции строку
        необходимо завершать символом '\n'. По передаче этого символа,
        инициируется передача по CAN
*******************************************************************************/
static int sendchar_CAN(int ch)
{
	static unsigned char cnt = 0;
	static char buff[80];
	uint16_t per;
	uint32_t i, Buffer_number;
	if((ch != '\n') & (cnt < 80)) // Пока не пришло "\n" и буфер не заполнен - заполняем буфер
	{
		buff[cnt] = ch;
		cnt++;
	}
	else if(ch == '\n') // Пришел конец строки - начинаем отправлять, то что накопилось в буфере 
	{
		buff[cnt] = ch;
		cnt++;
		for(i = 0; i < cnt; i+=8)
		{	
			if((cnt/8) == (i/8)){
				TxMsg_CanPrintf.DLC = (cnt%8);
			}
			else{
				TxMsg_CanPrintf.DLC = 8;
			}
			// Заполняем структуру
			TxMsg_CanPrintf.Data[0]= buff[i];
			TxMsg_CanPrintf.Data[0]|= (buff[i+1] << 8);
			TxMsg_CanPrintf.Data[0]|= (buff[i+2] << 16);
			TxMsg_CanPrintf.Data[0]|= (buff[i+3] << 24);
			TxMsg_CanPrintf.Data[1]= buff[i+4];
			TxMsg_CanPrintf.Data[1]|= (buff[i+5] << 8);
			TxMsg_CanPrintf.Data[1]|= (buff[i+6] << 16);
			TxMsg_CanPrintf.Data[1]|= (buff[i+7] << 24);
			per = 0;
			
			// Спросим какой из буферов свободен для использования
			Buffer_number = CAN_GetDisabledBuffer (PRINTF_CAN);
			
			// Кладём сообщение в нужный буфер и ждем отправки
			CAN_Transmit(PRINTF_CAN, Buffer_number, &TxMsg_CanPrintf);
			// Ожидаем конца передачи, либо превышения времени ожидания
			while(((CAN_GetBufferStatus(PRINTF_CAN, Buffer_number) & CAN_STATUS_TX_REQ) != RESET) && (per != 0xFFF)) per++;
			// Вне зависимости от того, удалось отправить или нет, освобождаем буфер
			CAN_BufferRelease (PRINTF_CAN, Buffer_number);
		}	
		
		// Очищаем буфер 
		for(i=0; i < 80; i++)
		{
			buff[i] = 0;
		}
		cnt = 0; // Сбрасываем счетчик байт, буфер отправлен
	}
	if (cnt == 80) // Если буфер заполнен - начинаем отправлять
	{
		TxMsg_CanPrintf.DLC = 8;
		for(i = 0; i < cnt; i+=8)
		{
			// Заполняем структуру
			TxMsg_CanPrintf.Data[0]= buff[i];
			TxMsg_CanPrintf.Data[0]|= (buff[i+1] << 8);
			TxMsg_CanPrintf.Data[0]|= (buff[i+2] << 16);
			TxMsg_CanPrintf.Data[0]|= (buff[i+3] << 24);
			TxMsg_CanPrintf.Data[1]= buff[i+4];
			TxMsg_CanPrintf.Data[1]|= (buff[i+5] << 8);
			TxMsg_CanPrintf.Data[1]|= (buff[i+6] << 16);
			TxMsg_CanPrintf.Data[1]|= (buff[i+7] << 24);
			per = 0;
			
			// Спросим какой из буферов свободен для использования
			Buffer_number = CAN_GetDisabledBuffer (PRINTF_CAN);
			
			// Кладём сообщение в нужный буфер и ждем отправки
			CAN_Transmit(PRINTF_CAN, Buffer_number, &TxMsg_CanPrintf);
			// Ожидаем конца передачи, либо превышения времени ожидания
			while(((CAN_GetBufferStatus(PRINTF_CAN, Buffer_number) & CAN_STATUS_TX_REQ) != RESET) && (per != 0xFFF)) per++;
			// Вне зависимости от того, удалось отправить или нет, освобождаем буфер
			CAN_BufferRelease (PRINTF_CAN, Buffer_number);
		}	
		// Очищаем буфер 
		for(i=0; i < 80; i++)
		{
			buff[i] = 0;
		}
		cnt = 0; // Сбрасываем счетчик байт, буфер отправлен
	}

	return 0;
}




/*******************************************************************************
@brief  sendchar_Log_FS: Функция записи символа в ЛОГ Файловой системы Log_FS
@param  ch - символ для вывода
@retval NONE
@notes  Необходимо подключить модуль файловой системы
*******************************************************************************/
static int sendchar_Log_FS(int ch)
{
	uint8_t sym = (uint8_t)ch;
	LogFs_WriteToCurrentFile(&sym, 1);
	return 0;
}



/*******************************************************************************
@brief  getkey: заглушка для стандартных функциий ввода;
@param  None
@retval Считаннный символ из потока
@notes  Ввод не используем, поэтому заглушка
*******************************************************************************/
int getkey(void)
{
	return 0;
}  







