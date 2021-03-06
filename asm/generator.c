#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include "asm.h"
#include "../common/utils.h"


int32 generator_int32(generator_t* generator, int32 value) {
	if (generator->write_big_endian != generator->is_cpu_big_endian) {
		int32_bytes_t e = {value};
		int32_bytes_t p = {0};

		p.c[0] = e.c[3];
		p.c[1] = e.c[2];
		p.c[2] = e.c[1];
		p.c[3] = e.c[0];

		return p.i;
	}
	return value;
}

int32 generator_int16(generator_t* generator, int32 value) {
	if (generator->write_big_endian != generator->is_cpu_big_endian) {
		int16_bytes_t e = {value};
		int16_bytes_t p = {0};

		p.c[0] = e.c[1];
		p.c[1] = e.c[0];

		return p.i;
	}
	return value;
}

void generator_allocate(generator_t* generator) {
	generator->byte_code_max_size += 4096;
	generator->byte_code = (char*)realloc(generator->byte_code, generator->byte_code_max_size);
	memset(&generator->core, 0, sizeof(generator->core));
	generator->is_cpu_big_endian = is_cpu_big_endian();
}

void generator_destroy(generator_t* generator) {
	list_destroy(&generator->labels, label_destroy);
	list_destroy(&generator->forwards, label_destroy);
	free(generator->byte_code);
}


int8* generator_write32(generator_t* generator, int32 to_write) {
	int32 offset = generator->byte_code_base + generator->byte_code_offset;
	int8* current;
	int32_bytes_t e = {to_write};

	if (generator->byte_code_max_size < offset + 4) {
		generator_allocate(generator);
	}
	current = generator->byte_code + offset;
	if (generator->write_big_endian != generator->is_cpu_big_endian) {
		generator->byte_code[generator->byte_code_base + generator->byte_code_offset++] = e.c[3];
		generator->byte_code[generator->byte_code_base + generator->byte_code_offset++] = e.c[2];
		generator->byte_code[generator->byte_code_base + generator->byte_code_offset++] = e.c[1];
		generator->byte_code[generator->byte_code_base + generator->byte_code_offset++] = e.c[0];
	} else {
		generator->byte_code[generator->byte_code_base + generator->byte_code_offset++] = e.c[0];
		generator->byte_code[generator->byte_code_base + generator->byte_code_offset++] = e.c[1];
		generator->byte_code[generator->byte_code_base + generator->byte_code_offset++] = e.c[2];
		generator->byte_code[generator->byte_code_base + generator->byte_code_offset++] = e.c[3];
	}
	return current;
}

int8* generator_write16(generator_t* generator, int16 to_write) {
	int32 offset = generator->byte_code_base + generator->byte_code_offset;
	int8* current;
	int16_bytes_t e = {to_write};

	if (generator->byte_code_max_size < offset + 2) {
		generator_allocate(generator);
	}
	current = generator->byte_code + offset;
	if (generator->write_big_endian != generator->is_cpu_big_endian) {
		generator->byte_code[generator->byte_code_base + generator->byte_code_offset++] = e.c[1];
		generator->byte_code[generator->byte_code_base + generator->byte_code_offset++] = e.c[0];
	} else {
		generator->byte_code[generator->byte_code_base + generator->byte_code_offset++] = e.c[0];
		generator->byte_code[generator->byte_code_base + generator->byte_code_offset++] = e.c[1];
	}
	return current;
}

int8* generator_write8(generator_t* generator, int8 to_write) {
	int32 offset = generator->byte_code_base + generator->byte_code_offset;
	int8* current;
	if (generator->byte_code_max_size < offset + 1) {
		generator_allocate(generator);
	}
	current = generator->byte_code + offset;
	generator->byte_code[generator->byte_code_base + generator->byte_code_offset++] = to_write;
	return current;
}
