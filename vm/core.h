#ifndef CORE_H
#define CORE_H


#include "../common/holberton-core.h"


typedef struct core_s {
	core_file_header_t*	header;
	int32								id;
	int32								live_count;
	int32								live_last_cycle;
	float*  						color;
} core_t;

int8*			core_get_code_ptr(core_t* core);
core_t* 	core_load_from_file(const char* file_name);
void			core_destroy(core_t* core);

#endif
