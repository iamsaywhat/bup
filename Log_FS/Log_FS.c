/****************************************************************************************************************************
*
*			Log_FS - Модуль облегченной "файловой системы" для записи логов в "черном ящике".
*
*****************************************************************************************************************************/


#include "Log_FS.h"
#include "Log_FS_llio.h"


/*----------------------------------- Приватная часть модуля---------------------------------------------------------------------*/


/************************************************************
   Управляющая структура для работы файловой системы        *
************************************************************/
Log_Fs_Info_Union Log_Fs_Info;

/************************************************************
    Структура для обеспечения доступа к файлам              *
************************************************************/
Log_Fs_FileProperties_Type Log_Fs_FileProperties;


/***************************************************************************************************

	   LogFs_DeleteOldestFile - Функция Удаления самого старого файла в директории. Является
	   приватной, ее самостоятельный вызов может привести к ошибке файловой системы.

	   Примечание:  В случае полной заполненности памяти, при создании файла - произойдет вызов
	   этой функции. Она удалит самый старый из имеющихся файлов и произведет очистку секторов,
	   которые он занимал.

	   Параметры:
					Log_Fs_Info - Управляющая структура файловой системы

	   Возвращает:
					FS_FINE - Файл был удачно удален из директории
					FS_ERROR - Неизвестное форматирование (файловая система имеет внутреннюю ошибку

***************************************************************************************************/
Log_Fs_Status LogFs_DeleteOldestFile(void);


/***************************************************************************************************

	   LogFs_GetFile_NumSectors - Узнать количество секторов которые занимает файл c началом
	   в секторе с номером Sector;

	   Параметры:
					Sector - Номер сектора в котором начинается файл (от 0 до FS_SECTORS_NUM - 1)

	   Возвращает:
					Количество секторов которые занимает файл

***************************************************************************************************/
uint16_t LogFs_GetFile_NumSectors(uint16_t Sector);



/*------------------------------------------Публичная часть модуля-------------------------------------------------------------*/



/***************************************************************************************************
	   LogFs_Formatting - Форматирование накопителя (полная очистка с потерей всех данных)
***************************************************************************************************/
void LogFs_Formatting(void)
{
	EraseChip();
}


/***************************************************************************************************
	   LogFs_GetFileNum - Получение информации о числе имеющихся файлов в директории
***************************************************************************************************/
uint16_t LogFs_GetFileNum(void)
{
	return Log_Fs_Info.Struct.Files_Num;
}



/***************************************************************************************************
	   LogFs_GetFileNum - Узнать порядковый последнего созданного файла
***************************************************************************************************/
uint16_t LogFs_GetNumLastFile(void)
{
	uint32_t Address;
	uint8_t  buff[4];
	uint16_t i;

	for (i = 0; i < 4; i++) buff[i] = 0;
	// Определяем стартовый адрес расположения последнего созданного файла
	Address = FS_SECTOR_SIZE * Log_Fs_Info.Struct.LastFile_Sector;
	// Читаем залоговок (2 байта) и номер файла (2 байта)
	MemoryRead(Address, buff, 4);

	// Возвращаем порядкой номер файла
	return *(uint16_t*)(buff + 2);
}



/***************************************************************************************************
	   LogFs_GetFile_NumSectors - Узнать количество секторов которые занимает файл c началом
	   в секторе с номером Sector
***************************************************************************************************/
uint16_t LogFs_GetFile_NumSectors(uint16_t Sector)
{
	uint32_t Address;
	uint8_t  buff[4];
	uint16_t NumSectors = 1;
	uint16_t i;
	uint16_t SECT = Sector;

	for (i = 0; i < 4; i++) buff[i] = 0;

	for (i = 1; i < FS_SECTORS_NUM; i++)
	{
		// Так как файловая система реализована по типу кольцевого буфера,
		// необходимо осуществлять переходы от старшего сектора к нулевому непрерывно
		// Чего и добиваемся этим условием
		if (SECT + i >= FS_SECTORS_NUM) SECT = 0;
		else SECT++;

		// Теперь вычисляем адрес сектора идущим следом за тем, который был передан аргументом
		Address = FS_SECTOR_SIZE * SECT;
		MemoryRead(Address, buff, 4);
		// Ожидаем находить заголовки продолжения файла
		// Любой другой будет говорить о том, что файл кончился
		if (*(uint16_t*)(buff) == FILE_CONTINUATION)
			NumSectors++;
		else break;
	}

	return NumSectors;
}




/***************************************************************************************************
	  Log_Fs_GetFileProperties - Функция позволяет узнать параметры файла, который был выбран
	   функцией "Log_Fs_FindFIle": Размер в байтах и порядковый номер в хранилище
***************************************************************************************************/
uint32_t Log_Fs_GetFileProperties(uint8_t CMD)
{
	uint32_t result = 0;

	// Необходимо проверить инициализирована ли структура выбора файла
	// Если нет, значит и файл не выбран, а значит и выводить нечего.
	//if (Log_Fs_FileProperties.InitStructState != FS_INIT_OK)
	//	return FS_ERROR;
	// Смотрим, какая информация интересует
	switch (CMD)
	{
		// Спрашивают номер файла
	case FILE_NUMBER:
		result = Log_Fs_FileProperties.FileNum;
		break;
		// Спрашивают размер файла файла
	case FILE_SIZE:
		result = Log_Fs_FileProperties.ByteSize;
		break;

	}
	return result;
}




/***************************************************************************************************
	   LogFs_Info - Функция проверки и инициализации файловой системы. Производит просмотр
	   и анализ файл файловой системы, ищет созданые файлы, определяет положения первого
	   и последнего файла (по порядковому номеру), определяет количество занимаемых ими секторов,
	   производит подсчет свободных секторов, подсчет имеющихся файлов, ищет свободное место
	   в котором может быть создан файл. Заполняет управляющую структуру Log_Fs_Info,
	   которая обеспечивает работу файловой системы.
***************************************************************************************************/
Log_Fs_Status LogFs_Info (void)
{
	uint32_t i;
	uint32_t Address;
	uint8_t  buff[4];
	uint32_t LastFileNum = 0;
	uint32_t OldestFileNum = 0xFFFFFFFF;
	uint8_t first_step_flag = 1;

	for (i = 0; i < 4; i++) buff[i] = 0;

	// Инициализируем структуру с информацией о файловой системе
	Log_Fs_Info.Struct.Files_Num = 0;
	Log_Fs_Info.Struct.FreeSectors_Num = FS_SECTORS_NUM;
	Log_Fs_Info.Struct.LastFile_Sector = 0;
	Log_Fs_Info.Struct.OldestFile_Sector = 0;
	Log_Fs_Info.Struct.CurrentFile_Sector = 0xFF;  // Выставляем некорректное значение для защиты
	Log_Fs_Info.Struct.CurrentWritePosition = 0;

	// Чтобы узнать состояние файловой системы необходимо из каждого сектора считать первые 4 байта
	for (i = 0; i < FS_SECTORS_NUM; i++)
	{
		// Вычисляем адрес сектора
		Address = FS_SECTOR_SIZE * i;
		// Читаем первые 4 байта сектора
		MemoryRead(Address, buff, 4);
		// Проверяем заголовок сектора: содержит ли сектор файл
		if (*(uint16_t*)(buff) == FILE_EXIST_HANDLER)
		{
			// Инкрементируем счетчик файлов
			Log_Fs_Info.Struct.Files_Num++;
			// Декрементируем счетчик свободных секторов
			Log_Fs_Info.Struct.FreeSectors_Num--;
			
			// Определяем наиболее старый файл из имеющихся, для возможной перезаписи
			// Ищем наименьший порядковый номер файла
			if (*(uint16_t*)(buff+2) <  OldestFileNum)
			{
				// Запоминаем номер файла
				OldestFileNum = *(uint16_t*)(buff+2);
				// Фиксируем номер сектора, где этот файл лежит
				Log_Fs_Info.Struct.OldestFile_Sector = i;
			}
			// Определяем последний созданный файл (номер сектора содержащий этот файл)
			// Ищем наибольший порядковый номер файла
			if(*(uint16_t*)(buff+2) > LastFileNum)
			{
				// Запоминаем номер файла
				LastFileNum = *(uint16_t*)(buff+2);
				// Фиксируем номер сектора, где этот файл лежит
				Log_Fs_Info.Struct.LastFile_Sector = i;
			}
		}
		// Возможно это продолжение файла с предыдущего сектора?
		else if (*(uint16_t*)(buff) == FILE_CONTINUATION)
		{
			Log_Fs_Info.Struct.FreeSectors_Num--;
		}
		// Тогда здесь должно быть пусто
		else if (*(uint16_t*)(buff) == FREE_SPACE_HANDLER)
		{
			// Если попали сюда, значит текущий сектор будет пустым
			if (first_step_flag) 
			{
				// Необходимо определиться со свободным местом для записи
				// Фиксируем первый попавшийся при анализе номер свободного сектора
				Log_Fs_Info.Struct.CurrentFile_Sector = i;
			}
			// Этим гарантируем, что инструкция выше выполнится единственный раз
			first_step_flag = 0;
		}
		// Иначе файловая система повреждена
		else return FS_ERROR;
	}
	// Дополнительно определим размеры в секторах файлов (самого старого и самого свежего)
	Log_Fs_Info.Struct.LastFile_SectorNum = LogFs_GetFile_NumSectors(Log_Fs_Info.Struct.LastFile_Sector);
	Log_Fs_Info.Struct.OldestFile_SectorNum = LogFs_GetFile_NumSectors(Log_Fs_Info.Struct.OldestFile_Sector);

	// Информация о диске получена, возвращаем успех			
	return FS_FINE;
}



/***************************************************************************************************
	   LogFs_DeleteOldestFile - Функция Удаления самого старого файла в директории. Является
	   приватной, ее самостоятельный вызов может привести к ошибке файловой системы.
***************************************************************************************************/
Log_Fs_Status LogFs_DeleteOldestFile(void)
{
	uint16_t i;
	uint16_t Count;
	uint32_t Address;
	uint32_t Sector;
	uint8_t  buff[4];

	for (i = 0; i < 4; i++) buff[i] = 0;

	// Определяем сколько секторов занимает файл
	Count = LogFs_GetFile_NumSectors(Log_Fs_Info.Struct.OldestFile_Sector);

	// Стираем сектора, которые занимает самый старый файл
	for(i = 0; i < Count; i++)
	{
		// Определяем адреса секторов, в которых он расположен
		Address =  FS_SECTOR_SIZE * (Log_Fs_Info.Struct.OldestFile_Sector + i);
		// Так как файловая система реализована по типу кольцевого буфера,
		// необходимо осуществлять переходы от старшего сектора к нулевому непрерывно
		// Чего и добиваемся этим условием
		if (Address >= FS_SECTOR_SIZE * FS_SECTORS_NUM) Address = Address % (FS_SECTOR_SIZE * FS_SECTORS_NUM);
		// Стираем сектор
		EraseSector(Address);
		// Обновляем информацию о свободных секторах - теперь их Count штук
		Log_Fs_Info.Struct.FreeSectors_Num = Count;
	}
	// Декрементируем счетчик файлов
	Log_Fs_Info.Struct.Files_Num--;

	// Удалили самый старый файл, теперь необходимо обновить информацию о файловой системе
	// Теперь самым старым файлом в директории будет файл с порядковым номером OldestFile_SectorNum + 1
	// Но необходимо найти начало этого файла
	// Делаем шаг вперед на Count секторов (Count - количество секторов на которых располагался файл) и смотрим заголовок и номер файла
	Sector = (Log_Fs_Info.Struct.OldestFile_Sector + Count) % FS_SECTORS_NUM;
	Address = FS_SECTOR_SIZE * Sector;
	MemoryRead(Address, buff, 4);
	// Если все в порядке, что здесь должен быть заголовок начала следующего файла
	if (*(uint16_t*)(buff) != FILE_EXIST_HANDLER) return FS_ERROR; // Если нет - файловая система имеет ошибку и завершаем
	// Фиксируем сектор нового "старого" файла
	Log_Fs_Info.Struct.OldestFile_Sector = Sector;

	// Теперь необходимо определить сколько секторов занимает новый "старый" файл
	// Для этого будем перебирать заголовски следующих секторов пока не увидим признак начала нового файла
	for (i = 1; i < FS_SECTORS_NUM; i++)
	{
		// Это условие так же для организации закольцованности файловой системы
		if (Sector + i >= FS_SECTORS_NUM) Sector = 0;
		else Sector++;

		// Смотрим заголовок следующих секторов в поисках заголовка начала
		Address = FS_SECTOR_SIZE * Sector;
		MemoryRead(Address, buff, 4);
		// Ищем окончание файла, который теперь является самым старым. Конец определяем по FILE_EXIST_HANDLER следующего за ним сектора
		if ((*(uint16_t*)(buff) == FILE_EXIST_HANDLER))
		{
			// Если нашли то обновляем структуру
			// Число секторов на котором размещен файл (который теперь самый старый в директории)
			Log_Fs_Info.Struct.OldestFile_SectorNum = i;
			break;
		}
	}
	// Если попали сюда, значит все хорошо
	return FS_FINE;
}




/***************************************************************************************************
	   LogFs_CreateFile - Функция создания сессии/файла. Ищет свободное место не диске, если
	   его нет, удаляет самый старший файл в директории, создает в свободном месте заголовок файла
	   и присваивает файлу порядковый номер с момета создания первого файла в ФС.
***************************************************************************************************/
void LogFs_CreateFile(void)
{
	uint32_t Address;
	uint8_t  buff[4];
	uint16_t i;

	for (i = 0; i < 4; i++) buff[i] = 0;

	// Чтобы узнать порядковый номер создаваемого файла
	// Воспользуемся информацией о последнем созданном файле
	// К его номеру сделаем икремент - и получим номер нового файла
	Address = FS_SECTOR_SIZE * Log_Fs_Info.Struct.LastFile_Sector;
	MemoryRead(Address, buff, 4);
	// В полученной информации сразу подменяем заголовок (на всякий случай)
	*(uint16_t*)(buff) = FILE_EXIST_HANDLER;
	// А тут к полученному номеру добавляем 1
	*(uint16_t*)(buff+2) = *(uint16_t*)(buff+2) + 1;
	
	// Если свободных секторов нет 
	if(Log_Fs_Info.Struct.FreeSectors_Num == 0)
	{
		// Попав сюда, необходимо освободить сектор(а) с самым старым файлом
		// На его месте будет создан новый файл, поэтому сразу фиксируем сектор старого файла, как текущий рабочий
		Log_Fs_Info.Struct.CurrentFile_Sector = Log_Fs_Info.Struct.OldestFile_Sector;
		// И стираем сектора со старым файлом
		LogFs_DeleteOldestFile();
	}
	// Создаем файл - Размещаем заголовок и номер на секторе диска
	Address = FS_SECTOR_SIZE * Log_Fs_Info.Struct.CurrentFile_Sector;
	// Записываем заголовок в файл
	MemoryWrite (Address, buff, 4);
	// Смещаем указатель для последующей записи на 4 байта (в каждом секторе первые 4 байта - резерв файловой системы)
	Log_Fs_Info.Struct.CurrentWritePosition = 4;
	// Раз заняли один сектор новым файлом, то нужно декрементировать счетчик свободных секторов
	Log_Fs_Info.Struct.FreeSectors_Num--;
	// Файл создан, нужно обновить счетчик файлов
	Log_Fs_Info.Struct.Files_Num++;
}



/***************************************************************************************************
	  LogFs_WriteToCurrentFile - Функция записи информации в файл
***************************************************************************************************/
void LogFs_WriteToCurrentFile(uint8_t* Buffer, uint32_t Size)
{
	uint32_t i;
	uint32_t Address;
	uint8_t  buff[4];
	uint32_t adr;

	for (i = 0; i < 4; i++)
	{
		buff[i] = 0;
	}

	// Определяем адрес для начала записи по выделенному сектору и смещению от его начала "CurrentWritePosition"
	Address = FS_SECTOR_SIZE * Log_Fs_Info.Struct.CurrentFile_Sector + Log_Fs_Info.Struct.CurrentWritePosition;
	
	// Проверяем хватит ли нам места в текущем секторе
	if ((Log_Fs_Info.Struct.CurrentWritePosition + Size) >= FS_SECTOR_SIZE )
	{
		// Не хватает
		// Посмотрим свободен ли следующий по порядку сектор
		if (Log_Fs_Info.Struct.FreeSectors_Num == 0)
		{
			// Нет, нужно освободить место, удалив самый старый из имеющихся файлов
			LogFs_DeleteOldestFile();
		}
		// Необходимо объявить о продолжении файла, создав в следующем секторе соответсвущий заголовок и дублировать номер файла
		// Узнаем номер текущего файла в каталоге
		adr = FS_SECTOR_SIZE * Log_Fs_Info.Struct.CurrentFile_Sector;
		MemoryRead(adr, buff, 4);// читаем первые 4 байта сектора
		// Информацию получили, теперь подменим заголовок на заголовок продолжения файла, номер остаётся таким же
		*(uint16_t*)(buff) = FILE_CONTINUATION;

		// Если сектор последний - нужно перейти на нулевой (кольцевой буфер)
		if (Log_Fs_Info.Struct.CurrentFile_Sector < (FS_SECTORS_NUM - 1))
			adr = FS_SECTOR_SIZE * (Log_Fs_Info.Struct.CurrentFile_Sector + 1);
		else
			adr = 0;
		// Размечаем этот сектор как продолжение нашего файла
		MemoryWrite(adr, buff, 4);
		// Следующий сектор резервируется, поэтому декрементируем кол-во свободных секторов
		Log_Fs_Info.Struct.FreeSectors_Num--;
		// Теперь все готово к записи
	}

	// Будем писать побайтово, так как нужен контроль над переходом на новый сектор
	// Там может быть, например, не пусто 
	for (i = 0; i < Size; i++)
	{
		//Определяем адрес места записи
		Address = FS_SECTOR_SIZE * Log_Fs_Info.Struct.CurrentFile_Sector + Log_Fs_Info.Struct.CurrentWritePosition;

		// Условие для контроля перехода между секторами
		// Определяем границу перехода по позиции в секторе, если она равна размеру сектора, это и есть граница
		if (Log_Fs_Info.Struct.CurrentWritePosition == FS_SECTOR_SIZE)
		{
			// Попали на новый сектор
			// Так же необходимо обновить информацию в структуре
			// Если сектор последний - нужно перейти на первый (кольцевой буфер)
			if (Log_Fs_Info.Struct.CurrentFile_Sector < (FS_SECTORS_NUM - 1))
				Log_Fs_Info.Struct.CurrentFile_Sector++;
			else
				Log_Fs_Info.Struct.CurrentFile_Sector = 0;
			// Задаем смещение на размер заголовка и номера от начала нового сектора
			Log_Fs_Info.Struct.CurrentWritePosition = 4;
			// Вычисляем адрес для записи
			Address = FS_SECTOR_SIZE * Log_Fs_Info.Struct.CurrentFile_Sector + Log_Fs_Info.Struct.CurrentWritePosition;
		}

		// Пишем в файл, и делаем инкремент смещения от начала сектора
		MemoryWrite(Address, &Buffer[i], 1);
		Log_Fs_Info.Struct.CurrentWritePosition++;
	}
}



/***************************************************************************************************
	Log_Fs_FindFIle - Функция поиска файлов в директории
***************************************************************************************************/
Log_Fs_Status Log_Fs_FindFIle(uint8_t CMD)
{
	uint32_t Address;
	uint8_t  buff[2];
	uint32_t i;
	static uint32_t FileCount = 0;

	// Посмотрим, что за команда
	// Если структура не была проинициализирована и вдруг приходит команда NEXT_FILE (Которая должна приходить только после FIRST_FILE)
	// Это ошибка
	if (CMD == NEXT_FILE && Log_Fs_FileProperties.InitStructState != FS_INIT_OK)
		return FS_ERROR;

	// Команда на запрос информации о самом старом файле в директории, равносильно команде на инициализацию структуры (отсчетной точкой будет старый файл)
	else if (CMD == FIRST_FILE)
	{
		Log_Fs_FileProperties.ByteSize = 0;
		Log_Fs_FileProperties.FileNum = 0;
		Log_Fs_FileProperties.SectorNum = Log_Fs_Info.Struct.OldestFile_SectorNum;
		Log_Fs_FileProperties.StartSector = Log_Fs_Info.Struct.OldestFile_Sector;
		Log_Fs_FileProperties.InitStructState = FS_INIT_OK;
		Log_Fs_FileProperties.ReadPosition = 0;

		// Сбрасываем счетчик просмотренных файлов
		FileCount = 0;
	}
	// Команда на запрос информации о самом новом файле в директории
	else if (CMD == LAST_FILE)
	{
		Log_Fs_FileProperties.ByteSize = 0;
		Log_Fs_FileProperties.FileNum = 0;
		Log_Fs_FileProperties.SectorNum = Log_Fs_Info.Struct.LastFile_SectorNum;
		Log_Fs_FileProperties.StartSector = Log_Fs_Info.Struct.LastFile_Sector;
		Log_Fs_FileProperties.InitStructState = FS_INIT_OK;
		Log_Fs_FileProperties.ReadPosition = 0;

		// Сбрасываем счетчик просмотренных файлов
		FileCount = Log_Fs_Info.Struct.Files_Num - 1;
	}
	// Переходим к следующему файлу, если структура была инициализирована (точка отсчета есть)
	else if (CMD == NEXT_FILE && Log_Fs_FileProperties.InitStructState == FS_INIT_OK)
	{
		Log_Fs_FileProperties.ByteSize = 0;
		Log_Fs_FileProperties.FileNum = 0;
		Log_Fs_FileProperties.ReadPosition = 0;
		// Переходим к началу следующего файла, отталкиваясь от сектора предыдущего и количества секторов которые он занимал
		Log_Fs_FileProperties.StartSector = (Log_Fs_FileProperties.StartSector + Log_Fs_FileProperties.SectorNum) % FS_SECTORS_NUM;
		
		// Теперь необходимо определить сколько секторов занимает этот файл
		for (i = 1; i < FS_SECTORS_NUM; i++)
		{
			Address = FS_SECTOR_SIZE * (Log_Fs_FileProperties.StartSector + i);
			// Контролируем переходы кольцевого буфера (чтобы избежать переполнения и обращения по несуществующему адресу)
			Address = Address % (FS_SECTORS_NUM * FS_SECTOR_SIZE);
			// Будем читать заголовки секторов после StartSector пока не найдем FILE_EXIST_HANDLER - признак начала следующего файла
			MemoryRead(Address, buff, 2);
			if (*(uint16_t*)(buff) == FILE_EXIST_HANDLER || *(uint16_t*)(buff) == 0xFFFF)
				break;

		}
		// Фиксируем число таких секторов
		Log_Fs_FileProperties.SectorNum = i;
	}
	// Иначе команда неопознана - возвращаем ошибку
	else
		return FS_ERROR;

	// Получаем адрес сектора расположения файла
	Address = FS_SECTOR_SIZE * Log_Fs_FileProperties.StartSector;

	// Проверим присутсвует ли по этому адресу заголовок начала файла
	MemoryRead(Address, buff, 2);
	if (*(uint16_t*)(buff) != FILE_EXIST_HANDLER)
		return FS_ERROR; // Заголовка нет, файловая система содержит ошибку

	// Узнаем порядковый номер этого файла в директории
	Address += 2;
	MemoryRead(Address, buff, 2);
	Log_Fs_FileProperties.FileNum = *(uint16_t*)(buff);

	//Если файл содержится на нескольких секторах, сразу переходим в последний
	if (Log_Fs_FileProperties.SectorNum > 1)
	{
		// Определяем адрес этого сектора
		Address = FS_SECTOR_SIZE * (Log_Fs_FileProperties.StartSector + Log_Fs_FileProperties.SectorNum - 1);
		// Контролируем переходы кольцевого буфера (чтобы избежать переполнения и обращения по несуществующему адресу)
		Address = Address % (FS_SECTORS_NUM * FS_SECTOR_SIZE);
		// А счетчик байт файла инкрементируем на (FS_SECTOR_SIZE - 4) * (Количество занятых файлом секторов - 1)
		Log_Fs_FileProperties.ByteSize = (FS_SECTOR_SIZE - 4) * (Log_Fs_FileProperties.SectorNum - 1);
		// Смещаем адрес на 4 заголовочных байта
		Address += 4;
	}
	// Если же файл содержится только в текущем секторе смещаем адрес еще на 2 байта (от номера файла)
	else Address += 2;

	// Дальше начинаем искать конец файла
	for (i = 0; i < FS_SECTOR_SIZE - 4; i++)
	{
		// Будем считывать байт за байтом из этого сектора и смотреть не упёрлись ли мы в конец (0xFF - форматированная ячейка)
		MemoryRead(Address, &buff[0], 1);
		// Если конец файла, то выходим
		if (*(uint8_t*)(buff) == 0xFF)
			break;
		// Если попали сюда, то полученный байт не признак конца и нужно инкрементировать счетчик байт и адрес
		Address++;
		Log_Fs_FileProperties.ByteSize++;
	}
	// Инкрементируем счетчик просмотренных файлов, так как если мы дошли сюда, значит файл просмотрен
	FileCount++;
	// Проверяем все ли файлы в хранилище были просмотрены
	if (Log_Fs_Info.Struct.Files_Num <= FileCount)
	{
		Log_Fs_FileProperties.InitStructState = FS_INIT_NO;
		return FS_ALL_FILES_SCROLLS;
	}
		
	return FS_NOT_OVER;
}




/***************************************************************************************************
	  LogFs_ReadFile - Функция чтения информации из файлов. Должна запускаться только после функции
	  Log_Fs_GetFileProperties, поскольку эта функция и позволяет нам определить какой файл будем
	  читать. То, какой файл будем читать содержится в структуре Log_Fs_FileProperties.
***************************************************************************************************/
Log_Fs_Status LogFs_ReadFile(uint8_t* Buffer, uint32_t Size)
{
	uint32_t Address;    
	uint32_t i;

	// Читать из одного файла можно в несколько обращений
	// Функция запоминает номер файла и номер байта, на котором чтение при текущем обращении
	// К фунции остановилось, чтобы при следующем обращении можно было продолжить с этого же места
	// Таким образом имеется возможность отказаться от буферизации всего файла, а вычитывать его по байтам


	for (i = 0; i < Size; i++)
	{
		// Получаем текущий адрес ячейки для чтения
		Address = (FS_SECTOR_SIZE * Log_Fs_FileProperties.StartSector + Log_Fs_FileProperties.ReadPosition) % (FS_SECTOR_SIZE * FS_SECTORS_NUM);
		// Проверим, вдруг адрес указывает на первые 4 байта заголовка сектора
		if (Address % FS_SECTOR_SIZE == 0)
		{
			// Они нам не нужны, поэтому сразу перепрыгиваем на 4 байта вперед
			Address += 4;
			// Делаем инкремент количества считанных байт
			Log_Fs_FileProperties.ReadPosition += 4;
		}
		// Читаем 
		MemoryRead(Address, &Buffer[i], 1);
		// Байт считали, делаем инкремент
		Log_Fs_FileProperties.ReadPosition++;
		// Проверяем был ли файл считан целиком
		if (Log_Fs_FileProperties.ReadPosition >= (Log_Fs_FileProperties.ByteSize + 4 * Log_Fs_FileProperties.SectorNum))
		{
			// Да, поэтому байт-позицизию чтения сбрасываем
			Log_Fs_FileProperties.ReadPosition = 0;
			// Завершаем цикл
			return FS_FILE_END;
		}
	}
	return FS_FINE;
}


/***************************************************************************************************
	   Log_Fs_FindFile_ByNum - Функция осуществляет поиск файла по номеру.
***************************************************************************************************/
Log_Fs_Status Log_Fs_FindFile_ByNum(uint16_t NUM)
{
	uint16_t files = 0;
	uint16_t min_num, max_num;
	uint16_t i;

	// Проверим существует ли такой номер
	// Посмотрим порядковый номер самого старого файла (номер должен быть самым маленьким)
	Log_Fs_FindFIle(FIRST_FILE);
	min_num = Log_Fs_GetFileProperties(FILE_NUMBER);
	// Посмотрим порядковый номер самого свежего файла (номер должен быть самым большим)
	Log_Fs_FindFIle(LAST_FILE);
	max_num = Log_Fs_GetFileProperties(FILE_NUMBER);
	// Номер запрашиваемого файла должен принадлжеать данному диапазону, иначе ошибка - файл не найден
	if (NUM > max_num || NUM < min_num)
		return FS_ERROR;
	
	// Получим число файлов в хранилище
	files = LogFs_GetFileNum();
	
	for (i = 0; i < files; i++)
	{
		if(i == 0) 
			Log_Fs_FindFIle(FIRST_FILE);
		else 
			Log_Fs_FindFIle(NEXT_FILE);
		// Сверяем номер файла. Если совпадает файл найден можно выходить
		if (Log_Fs_GetFileProperties(FILE_NUMBER) == NUM)
			return FS_FINE;

	}
	// Если дошли сюда, значит файл не найден - ошибка
	return FS_ERROR;
}









