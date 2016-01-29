#include <sys/stat.h>
#include <fcntl.h>

#include <stdio.h>
#include <stdlib.h>

#include "../common/utils.h"
#include "core.h"

float g_colors[4][4] = {
	{ 1.0f, 0.6f, 0.0f, 1.0f },
	{ 0.0f, 1.0f, 0.6f, 1.0f },
	{ 0.0f, 0.6f, 1.0f, 1.0f },
	{ 0.6f, 0.6f, 1.0f, 1.0f }
};

int32 g_core_count = 0;

int32 be_to_le_int32(int32 value) {
	int32 ret;
	int8* ptr = (int8*)&ret;

	*ptr++ = (int8)((value >> 24) & 0xff);
	*ptr++ = (int8)((value >> 16) & 0xff);
	*ptr++ = (int8)((value >> 8) & 0xff);
	*ptr   = (int8)((value)& 0xff);

	return ret;
}


int8*			core_get_code_ptr(core_t* core) {
	return (int8*)(core->header)+sizeof(core_file_header_t);
}

void			core_destroy(core_t* core) {
	free(core);
}

core_t* 	core_load_from_file(const char* file_name) {
	int fd = _open(file_name, OPEN_MODE);

	if (fd < 0)
		return NULL;

	core_t* core = NULL;

	int magic;

	if (_read(fd, &magic, 4) == 4)
	{
		if (magic == CORE_FILE_MAGIC)
		{
			long size;
			int8* data;

			_lseek(fd, 0, SEEK_SET);
			size = _lseek(fd, 0, SEEK_END);

			data = malloc(size);
			_lseek(fd, 0, SEEK_SET);
			_read(fd, data, size);

			core = malloc(sizeof(core_t));
			core->header = (core_file_header_t*)data;
			core->id = 0;
			core->live_count = 0;
			core->color = g_colors[g_core_count++];
		}
	}

	_close(fd);
	return  core;
}