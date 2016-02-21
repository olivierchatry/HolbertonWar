#include <sys/stat.h>
#include <fcntl.h>

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

#include "../common/utils.h"
#include "core.h"

float g_colors[4][4] = {
	{ 1.0f, 0.6f, 0.0f, 1.0f },
	{ 0.0f, 1.0f, 0.6f, 1.0f },
	{ 0.0f, 0.6f, 1.0f, 1.0f },
	{ 0.6f, 0.6f, 1.0f, 1.0f }
};

uint32 ftouint(float* f) {
	uint32 r = (uint32) (*f++ * 255.0f);
	uint32 g = (uint32)(*f++ * 255.0f);
	uint32 b = (uint32)(*f++ * 255.0f);
	uint32 a = (uint32)(*f++ * 255.0f);

	return a << 24 | b << 16 | g << 8  | r;
}

int32 g_core_count = 0;

int32 invert_int32(int32 value) {
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
	free(core->header);
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
		if (magic == CORE_FILE_MAGIC || magic == CORE_FILE_INVERT_MAGIC)
		{
			long size;
			int8* data;
			int32 inverted = (magic == CORE_FILE_INVERT_MAGIC);
			_lseek(fd, 0, SEEK_SET);
			size = _lseek(fd, 0, SEEK_END);

			data = malloc(size);
			_lseek(fd, 0, SEEK_SET);
			_read(fd, data, size);

			core = malloc(sizeof(core_t));
			memset(core, 0, sizeof(core_t));
			core->header = (core_file_header_t*)data;
			if (inverted) {
				core->header->code_size = invert_int32(core->header->code_size);
				core->header->magic = invert_int32(core->header->magic);
			}
			core->id = 0;
			core->live_count = 0;
			core->color = g_colors[g_core_count++];
			core->color_uint = ftouint(core->color);
			core->is_big_endian = is_cpu_big_endian() || inverted;
		}
	}

	_close(fd);
	return  core;
}
