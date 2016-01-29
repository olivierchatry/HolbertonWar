#ifndef HOLBERTON_CORE_H
#define HOLBERTON_CORE_H

typedef int   int32;
typedef short int16;
typedef char  int8;

#define CORE_REGISTER_COUNT				16
#define CORE_FILE_NAME_MAX_SIZE   64
#define CORE_FILE_VERSION         1

#define CORE_FILE_MAGIC						0xcacbcdce

#define CORE_FLAG_BIG_ENDIAN			1

typedef struct core_file_header_s {
  int32 magic;
  int32 version;
	int32	flags;

  char  name[CORE_FILE_NAME_MAX_SIZE];
  int32 code_size;
} core_file_header_t;


#define CORE_ARG_TYPE_REG 1
#define CORE_ARG_TYPE_IMM 2
#define CORE_ARG_TYPE_ADD 3

#define CORE_ASM_ADD	'%'
#define CORE_ASM_SEP	','
#define CORE_ASM_COMMENT ';'


#define OP_ARG_TYPE_REG 1
#define OP_ARG_TYPE_IMM 2
#define OP_ARG_TYPE_ADD 4

typedef struct opcode_s {
  int8   opcode;
	char*  mnemonic;
	int32	 cycles;
	int32	 args;
	int32	 arg_type[3];
} opcode_t;

opcode_t* holberton_core_get_opcodes();

#define VM_MAX_PROCESSES	65535
#define VM_MAX_CORES			4

#define VM_MEMORY_SIZE		8192
#define VM_CYCLE_DELTA		4
#define VM_CYCLE_TO_DIE		1536
#define VM_LIVE_COUNT			2048

#endif
