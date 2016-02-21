#ifndef HOLBERTON_CORE_H
#define HOLBERTON_CORE_H

#include <stdint.h>

typedef int32_t   int32;
typedef int16_t		int16;
typedef char 			int8;

typedef uint32_t  uint32;
typedef uint16_t  uint16;
typedef uint8_t  	uint8;

#define CORE_REGISTER_COUNT						16
#define CORE_FILE_NAME_MAX_SIZE				128
#define CORE_FILE_COMMENT_MAX_SIZE		2048

#define CORE_FILE_VERSION         1

#define CORE_FILE_MAGIC						0x00ea83f3
#define CORE_FILE_INVERT_MAGIC    0xf383ea00

typedef struct core_file_header_s {
	int32 magic;

  char  name[CORE_FILE_NAME_MAX_SIZE];
  int32 code_size;
	char  comment[CORE_FILE_COMMENT_MAX_SIZE];

} core_file_header_t;


#define CORE_ARG_TYPE_REG 1
#define CORE_ARG_TYPE_IMM 2
#define CORE_ARG_TYPE_ADD 3


#define CORE_ASM_LABEL		':'
#define CORE_ASM_IMM			'%'
#define CORE_ASM_SEP			','
#define CORE_ASM_COMMENT	'#'


#define OP_ARG_TYPE_REG 1
#define OP_ARG_TYPE_IMM 2
#define OP_ARG_TYPE_ADD 4

typedef struct opcode_s {
  int8   opcode;
	char*  mnemonic;
	int32	 cycles;
	int32	 arg_count;
	int32	 arg_types[3];
} opcode_t;

opcode_t* holberton_core_get_opcodes();

#define VM_MAX_PROCESSES	65535
#define VM_MAX_CORES			4

#define VM_MEMORY_SIZE		8092
#define VM_CYCLE_DELTA		4
#define VM_CYCLE_TO_DIE		1536
#define VM_LIVE_COUNT			2048

#define VM_MEMORY_MODULO	512
#endif
