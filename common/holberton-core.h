#ifndef HOLBERTON_CORE_H
#define HOLBERTON_CORE_H

typedef int   int32;
typedef short int16;
typedef char  int8;

#define CORE_REGISTER_COUNT				16
#define CORE_FILE_NAME_MAX_SIZE   64
#define CORE_FILE_VERSION         1

#define CORE_FILE_MAGIC						0xcacbcdce

typedef struct core_file_header_s {
  int32 magic;
  int32 version;

  char  name[CORE_FILE_NAME_MAX_SIZE];
  int32 code_size;
} core_file_header_t;


#define CORE_ARG_TYPE_REG 1
#define CORE_ARG_TYPE_IMM 2
#define CORE_ARG_TYPE_ADD 4

#define CORE_ASM_ADD	'%'
#define CORE_ASM_SEP	','
#define CORE_ASM_COMMENT ';'

typedef struct opcode_s {
  int8   opcode;
	char*  mnemonic;
	int32	 cycles;
	int32	 args;
	int32	 arg_type[3];
} opcode_t;

opcode_t* holberton_core_get_opcodes();

#endif
