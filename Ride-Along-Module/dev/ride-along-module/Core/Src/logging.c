#include "logging.h"

static FIL logFile;

FRESULT log_init()
{
	return f_open_append(&logFile, "latest.log");
}

FRESULT log_deinit()
{
	return f_close(&logFile);
}

FRESULT log_status(char* status, char* message)
{
	f_printf(&logFile, "%d [%s] %s\n", 	HAL_GetTick(), status, message);
	return f_sync(&logFile);
}
