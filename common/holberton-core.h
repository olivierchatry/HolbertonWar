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
#define CORE_FILE_NAME_MAX_SIZE				(128)
#define CORE_FILE_COMMENT_MAX_SIZE		(2048)

#define CORE_FILE_VERSION         1

#define CORE_FILE_MAGIC						0x00ea83f3
#define CORE_FILE_INVERT_MAGIC    0xf383ea00

#pragma pack(push, 1)
typedef struct core_file_header_s {
	int32 magic;

  char  name[CORE_FILE_NAME_MAX_SIZE];
  int32 code_size;
	char  comment[CORE_FILE_COMMENT_MAX_SIZE];

} core_file_header_t;
#pragma pack(pop)

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
  int8	opcode;
	char*	mnemonic;
	int32	processing_flags;
	int32	cycles;
	int32	arg_count;
	int32	arg_types[3];
} opcode_t;

#define ASM_PROCESSING_NORMAL			0
#define ASM_PROCESSING_IMM_AS_ADD	1
#define ASM_PROCESSING_NO_TYPES		2
#define ASM_PROCESSING_NO_MOD			3



opcode_t* holberton_core_get_opcodes();

#define VM_MAX_PROCESSES	65535
#define VM_MAX_CORES			4

#define VM_MEMORY_SIZE		(12*1024)
#define VM_CYCLE_DELTA		4
#define VM_CYCLE_TO_DIE		1536
#define VM_LIVE_COUNT			2048

#define VM_MEMORY_MODULO	512

#define HCORE_LIVE_OPCODE			0x01
#define HCORE_LD_OPCODE				0x02
#define HCORE_ST_OPCODE				0x03
#define HCORE_ADD_OPCODE			0x04
#define HCORE_SUB_OPCODE			0x05
#define HCORE_AND_OPCODE			0x06
#define HCORE_OR_OPCODE				0x07
#define HCORE_XOR_OPCODE			0x08
#define HCORE_ZJMP_OPCODE			0x09
#define HCORE_LDI_OPCODE			0x0a
#define HCORE_STI_OPCODE			0x0b
#define HCORE_FORK_OPCODE			0x0c
#define HCORE_LLD_OPCODE			0x0d
#define HCORE_LLDI_OPCODE			0x0e
#define HCORE_LFORK_OPCODE		0x0f
#define HCORE_AFF_OPCODE			0x10
#define HCORE_GTMD_OPCODE			0x11

#endif
