#include <stdlib.h>
#include "asm.h"

typedef union int32_bytes_s {
	int32 	i;
	char 		c[4];
} int32_bytes_t;

typedef union int16_bytes_s {
	int16 	i;
	char 		c[4];
} int16_bytes_t;

static int is_big_endian(void)
{
 	int32_bytes_t e = { 0x01000000 };
	return e.c[0];
}

void generator_allocate(generator_t* generator) {
	generator->byte_code_max_size += 4096;
	generator->byte_code = (char*)realloc(generator->byte_code, generator->byte_code_max_size);
	generator->is_cpu_big_endian = is_big_endian();
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
		generator->byte_code[generator->byte_code_base + generator->byte_code_offset++] = e.c[0];
		generator->byte_code[generator->byte_code_base + generator->byte_code_offset++] = e.c[1];
		generator->byte_code[generator->byte_code_base + generator->byte_code_offset++] = e.c[2];
		generator->byte_code[generator->byte_code_base + generator->byte_code_offset++] = e.c[3];
	} else {
		generator->byte_code[generator->byte_code_base + generator->byte_code_offset++] = e.c[3];
		generator->byte_code[generator->byte_code_base + generator->byte_code_offset++] = e.c[2];
		generator->byte_code[generator->byte_code_base + generator->byte_code_offset++] = e.c[1];
		generator->byte_code[generator->byte_code_base + generator->byte_code_offset++] = e.c[0];
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
		generator->byte_code[generator->byte_code_base + generator->byte_code_offset++] = e.c[0];
		generator->byte_code[generator->byte_code_base + generator->byte_code_offset++] = e.c[1];
	} else {
		generator->byte_code[generator->byte_code_base + generator->byte_code_offset++] = e.c[1];
		generator->byte_code[generator->byte_code_base + generator->byte_code_offset++] = e.c[0];
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
