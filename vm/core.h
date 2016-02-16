#ifndef CORE_H
#define CORE_H


#include "../common/holberton-core.h"
#include "../common/memory_access.h"

typedef struct core_s {
	core_file_header_t*	header;
	int32								id;
	int32								internal_id;
	int32								live_count;
	int32								live_last_cycle;
	float*  						color;
	uint32							color_uint;
	bound_t							bound;
	int									start_address;
	int									is_big_endian;
} core_t;

int8*			core_get_code_ptr(core_t* core);
core_t* 	core_load_from_file(const char* file_name);
void			core_destroy(core_t* core);

#endif
