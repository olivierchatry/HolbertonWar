#ifndef __ASM_H__
#define __ASM_H__

#include "../common/holberton-core.h"
#include "../common/list.h"

#define ASM_OK											0
#define ASM_INVALID_FILE_NAME				1
#define ASM_INVALID_INSTRUCTION			2
#define ASM_INVALID_ARGUMENT				3
#define ASM_INVALID_ARGUMENT_COUNT	4
#define ASM_PARSER_ERROR						5


typedef struct label_s {
	char* name;
	int32	offset;
	int32	opcode_offset;
} label_t;

typedef struct generator_s {
	list_t*							labels;
	list_t*							forwards;

	int8*								byte_code;
	int32								byte_code_offset;
	int32								byte_code_base;

	int32								byte_code_max_size;
	int32								current_line;
	core_file_header_t	core;
	int									error;

	int32								write_big_endian;
	int32								is_cpu_big_endian;
} generator_t;

void 			generator_allocate(generator_t* generator);
void 			generator_destroy(generator_t* generator);
int8*			generator_write32(generator_t* generator, int32 to_write);
int8*			generator_write16(generator_t* generator, int16 to_write);
int8*			generator_write8(generator_t* generator, int8 to_write);

label_t* 	label_create(char* name, int32 offset, int32 opcode_offset);
void			label_destroy(void* label);
label_t*  label_find_in_list(char* name, list_t* labels);

int       parse(char* line, char** output, int max);




#endif
