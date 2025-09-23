#include "fileio.h"

FRESULT f_open_append(FIL* fObj, const char* path)
{
	FRESULT result;

	result = f_open(fObj, path, FA_WRITE | FA_OPEN_ALWAYS);
	if (result == FR_OK)
	{
		result = f_lseek(fObj, f_size(fObj));
		if (result != FR_OK)
		{
			f_close(fObj);
		}
	}

	return result;
}
