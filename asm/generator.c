#include <stdlib.h>
#include <stdio.h>
#include "asm.h"
#include "../common/utils.h"

void generator_allocate(generator_t* generator) {
	generator->byte_code_max_size += 4096;
	generator->byte_code = (char*)realloc(generator->byte_code, generator->byte_code_max_size);
	generator->is_cpu_big_endian = is_cpu_big_endian();
}

void generator_destroy(generator_t* generator) {
	list_destroy(&generator->labels, label_destroy);
	list_destroy(&generator->forwards, label_destroy);
	free(generator->byte_code);
}


int8* generator_write32(generator_t* generator, int32 to_write) {
	int32 offset = generator->byte_code_base + generator->byte_code_offset;
	int8* current = generator->byte_code + offset;
	int32_bytes_t e = {to_write};

	if (generator->byte_code_max_size < offset + 4) {
		generator_allocate(generator);
	}
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
	int8* current = generator->byte_code + offset;
	int16_bytes_t e = {to_write};

	if (generator->byte_code_max_size < offset + 2) {
		generator_allocate(generator);
	}

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
	int8* current = generator->byte_code + offset;
	if (generator->byte_code_max_size < offset + 1) {
		generator_allocate(generator);
	}
	generator->byte_code[generator->byte_code_base + generator->byte_code_offset++] = to_write;
	return current;
}
