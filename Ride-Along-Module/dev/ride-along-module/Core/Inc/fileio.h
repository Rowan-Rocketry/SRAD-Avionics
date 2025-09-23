#ifndef FILEIO_H
#define FILEIO_H

#include "fatfs.h"
#include <string.h>

// Open or create a file in append mode
FRESULT f_open_append(FIL* file, const char* path);

#endif
