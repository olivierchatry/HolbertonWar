#ifndef UTILS_H
#define UTILS_H

#include <fcntl.h>
#include <stdint.h>

#if defined(_WIN32)
#	include <io.h>
#	define OPEN_MODE O_RDONLY | O_BINARY
#else
#	include <unistd.h>
#	define OPEN_MODE O_RDONLY
#	define	_write write
#	define _open open
#	define _read read
#	define _close close
#	define _lseek lseek
#	define _strdup strdup
#endif

char* file_to_memory(char* file_name, int* size);


typedef union int32_bytes_s {
	int32_t i;
	char 		c[4];
} int32_bytes_t;

typedef union int16_bytes_s {
	int16_t	i;
	char 		c[2];
} int16_bytes_t;

int is_cpu_big_endian(void);

#endif
