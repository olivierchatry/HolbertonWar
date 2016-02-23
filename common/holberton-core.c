#include "holberton-core.h"

/*
#define HCORE_LIVE_CYCLES			5
#define HCORE_LD_CYCLES				5
#define HCORE_ST_CYCLES				2
#define HCORE_ADD_CYCLES			10
#define HCORE_SUB_CYCLES			10
#define HCORE_AND_CYCLES			6
#define HCORE_OR_CYCLES				6
#define HCORE_XOR_CYCLES			6
#define HCORE_ZJMP_CYCLES			10
#define HCORE_LDI_CYCLES			25
#define HCORE_STI_CYCLES			2
#define HCORE_FORK_CYCLES			600
#define HCORE_LLD_CYCLES			10
#define HCORE_LLDI_CYCLES			50
#define HCORE_LFORK_CYCLES		1000
#define HCORE_AFF_CYCLES			2
#define HCORE_GTMD_CYCLES			5
*/

#define HCORE_LIVE_CYCLES			10
#define HCORE_LD_CYCLES				5
#define HCORE_ST_CYCLES				5
#define HCORE_ADD_CYCLES			10
#define HCORE_SUB_CYCLES			10
#define HCORE_AND_CYCLES			6
#define HCORE_OR_CYCLES				6
#define HCORE_XOR_CYCLES			6
#define HCORE_ZJMP_CYCLES			20
#define HCORE_LDI_CYCLES			25
#define HCORE_STI_CYCLES			25
#define HCORE_FORK_CYCLES			800
#define HCORE_LLD_CYCLES			10
#define HCORE_LLDI_CYCLES			50
#define HCORE_LFORK_CYCLES		1000
#define HCORE_AFF_CYCLES			2
#define HCORE_GTMD_CYCLES			5

static opcode_t g_opcodes[] = {
	{ HCORE_LIVE_OPCODE, "live", ASM_PROCESSING_NO_TYPES, HCORE_LIVE_CYCLES, 1, {
			OP_ARG_TYPE_IMM
		}
	},
	{ HCORE_LD_OPCODE, "ld", ASM_PROCESSING_NORMAL, HCORE_LD_CYCLES, 2, {
			OP_ARG_TYPE_ADD | OP_ARG_TYPE_IMM,
			OP_ARG_TYPE_REG
		}
	},
	{ HCORE_ST_OPCODE, "st", ASM_PROCESSING_NORMAL, HCORE_ST_CYCLES, 2, {
			OP_ARG_TYPE_REG,
			OP_ARG_TYPE_ADD | OP_ARG_TYPE_REG
		}
	},

	{ HCORE_ADD_OPCODE, "add", ASM_PROCESSING_NORMAL, HCORE_ADD_CYCLES, 3, {
			OP_ARG_TYPE_REG,
			OP_ARG_TYPE_REG,
			OP_ARG_TYPE_REG
		}
	},
	{ HCORE_SUB_OPCODE, "sub", ASM_PROCESSING_NORMAL, HCORE_SUB_CYCLES, 3, {
			OP_ARG_TYPE_REG,
			OP_ARG_TYPE_REG,
			OP_ARG_TYPE_REG
		}
	},
	{ HCORE_AND_OPCODE, "and", ASM_PROCESSING_NORMAL, HCORE_AND_CYCLES, 3,{
			OP_ARG_TYPE_REG | OP_ARG_TYPE_IMM | OP_ARG_TYPE_ADD,
			OP_ARG_TYPE_REG | OP_ARG_TYPE_IMM | OP_ARG_TYPE_ADD,
			OP_ARG_TYPE_REG
		}
	},
	{ HCORE_OR_OPCODE, "or",  ASM_PROCESSING_NORMAL, HCORE_OR_CYCLES, 3, {
			OP_ARG_TYPE_REG | OP_ARG_TYPE_IMM | OP_ARG_TYPE_ADD,
			OP_ARG_TYPE_REG | OP_ARG_TYPE_IMM | OP_ARG_TYPE_ADD,
			OP_ARG_TYPE_REG
		}
	},
	{ HCORE_XOR_OPCODE, "xor", ASM_PROCESSING_NORMAL, HCORE_XOR_CYCLES, 3, {
			OP_ARG_TYPE_REG | OP_ARG_TYPE_IMM | OP_ARG_TYPE_ADD,
			OP_ARG_TYPE_REG | OP_ARG_TYPE_IMM | OP_ARG_TYPE_ADD,
			OP_ARG_TYPE_REG
		}
	},
	{ HCORE_ZJMP_OPCODE, "zjmp", ASM_PROCESSING_NO_TYPES | ASM_PROCESSING_IMM_AS_ADD, HCORE_ZJMP_CYCLES, 1, {
			OP_ARG_TYPE_IMM
		}
	},
	{ HCORE_LDI_OPCODE, "ldi", ASM_PROCESSING_IMM_AS_ADD, HCORE_LDI_CYCLES, 3, {
			OP_ARG_TYPE_REG | OP_ARG_TYPE_IMM | OP_ARG_TYPE_ADD,
			OP_ARG_TYPE_IMM | OP_ARG_TYPE_REG,
			OP_ARG_TYPE_REG
		}
	},
	{ HCORE_STI_OPCODE, "sti", ASM_PROCESSING_IMM_AS_ADD, HCORE_STI_CYCLES, 3, {
			OP_ARG_TYPE_REG,
			OP_ARG_TYPE_REG | OP_ARG_TYPE_IMM | OP_ARG_TYPE_ADD,
			OP_ARG_TYPE_IMM | OP_ARG_TYPE_REG,
		}
	},
	{ HCORE_FORK_OPCODE, "fork", ASM_PROCESSING_NO_TYPES | ASM_PROCESSING_IMM_AS_ADD, HCORE_FORK_CYCLES, 1, {
			OP_ARG_TYPE_IMM
		}
	},
	{ HCORE_LLD_OPCODE, "lld", ASM_PROCESSING_NO_MOD, HCORE_LLD_CYCLES, 2, {
			OP_ARG_TYPE_ADD | OP_ARG_TYPE_IMM,
			OP_ARG_TYPE_REG
		}
	},
	{ HCORE_LLDI_OPCODE, "lldi", ASM_PROCESSING_NO_MOD|ASM_PROCESSING_IMM_AS_ADD, HCORE_LLDI_CYCLES, 3, {
			OP_ARG_TYPE_REG | OP_ARG_TYPE_IMM | OP_ARG_TYPE_ADD,
			OP_ARG_TYPE_IMM | OP_ARG_TYPE_REG,
			OP_ARG_TYPE_REG
		}
	},
	{ HCORE_LFORK_OPCODE, "lfork", ASM_PROCESSING_NO_MOD|ASM_PROCESSING_NO_TYPES|ASM_PROCESSING_IMM_AS_ADD, HCORE_LFORK_CYCLES, 1, {
			OP_ARG_TYPE_IMM
		}
	},
	{ HCORE_AFF_OPCODE, "aff", ASM_PROCESSING_NORMAL, HCORE_AFF_CYCLES, 1, {
			OP_ARG_TYPE_REG
		}
	},
	{ HCORE_GTMD_OPCODE, "gtmd", ASM_PROCESSING_NORMAL, HCORE_GTMD_CYCLES, 1, {
			OP_ARG_TYPE_REG
		}
	},
	{ 0, 0 }
};

opcode_t* holberton_core_get_opcodes() {
  return g_opcodes;
}


