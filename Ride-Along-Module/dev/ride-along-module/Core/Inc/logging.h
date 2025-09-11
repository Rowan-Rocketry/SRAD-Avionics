#ifndef LOG_H
#define LOG_H

#include "fileio.h"

FRESULT log_init();
FRESULT log_deinit();

FRESULT log_status(char* status, char* message);

#endif
