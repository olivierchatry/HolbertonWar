#include "holberton-core.h"


static opcode_t g_opcodes[] = {
	{ 0x01, "live", 5, 1, {
			OP_ARG_TYPE_IMM
		}
	},
	{ 0x02, "ld",  5, 2, {
			OP_ARG_TYPE_ADD | OP_ARG_TYPE_IMM,
			OP_ARG_TYPE_REG
		}
	},
	{ 0x03, "st",  2, 2, {
			OP_ARG_TYPE_REG,
			OP_ARG_TYPE_ADD | OP_ARG_TYPE_REG
		}
	},

	{ 0x04, "add", 10, 3, {
			OP_ARG_TYPE_REG,
			OP_ARG_TYPE_REG,
			OP_ARG_TYPE_REG
		}
	},
	{ 0x05, "sub", 10, 3, {
			OP_ARG_TYPE_REG,
			OP_ARG_TYPE_REG,
			OP_ARG_TYPE_REG
		}
	},
	{ 0x06, "and", 10, 3,{
			OP_ARG_TYPE_REG | OP_ARG_TYPE_IMM | OP_ARG_TYPE_ADD,
			OP_ARG_TYPE_REG | OP_ARG_TYPE_IMM | OP_ARG_TYPE_ADD,
			OP_ARG_TYPE_REG
		}
	},
	{ 0x07, "or",  10, 3, {
			OP_ARG_TYPE_REG | OP_ARG_TYPE_IMM | OP_ARG_TYPE_ADD,
			OP_ARG_TYPE_REG | OP_ARG_TYPE_IMM | OP_ARG_TYPE_ADD,
			OP_ARG_TYPE_REG
		}
	},
	{ 0x08, "xor", 10, 3, {
			OP_ARG_TYPE_REG | OP_ARG_TYPE_IMM | OP_ARG_TYPE_ADD,
			OP_ARG_TYPE_REG | OP_ARG_TYPE_IMM | OP_ARG_TYPE_ADD,
			OP_ARG_TYPE_REG
		}
	},
	{ 0x09, "zjmp", 10, 1, {
			OP_ARG_TYPE_IMM
		}
	},
	{ 0x0a, "ldi",  25, 3, {
			OP_ARG_TYPE_REG | OP_ARG_TYPE_IMM | OP_ARG_TYPE_ADD,
			OP_ARG_TYPE_IMM | OP_ARG_TYPE_REG,
			OP_ARG_TYPE_REG
		}
	},
	{ 0x0b, "sti",  2, 3, {
			OP_ARG_TYPE_REG,
			OP_ARG_TYPE_REG | OP_ARG_TYPE_IMM | OP_ARG_TYPE_ADD,
			OP_ARG_TYPE_IMM | OP_ARG_TYPE_REG,
		}
	},
	{ 0x0c, "fork",  600, 1, {
			OP_ARG_TYPE_IMM
		}
	},
	{ 0x0d, "lld",  10, 2, { 
			OP_ARG_TYPE_ADD | OP_ARG_TYPE_IMM,
			OP_ARG_TYPE_REG
		}
	},
	{ 0x0e, "lldi",  50, 3, {
			OP_ARG_TYPE_REG | OP_ARG_TYPE_IMM | OP_ARG_TYPE_ADD,
			OP_ARG_TYPE_IMM | OP_ARG_TYPE_REG,
			OP_ARG_TYPE_REG
		}
	},
	{ 0x0f, "lfork",  1000, 1, {
			OP_ARG_TYPE_IMM
		}
	},
	{ 0x10, "aff", 2, 1, {
			OP_ARG_TYPE_IMM
		}
	},
	{ 0x11, "gtmd",  1, 5, {
			OP_ARG_TYPE_REG
		}
	},
	{ 0, 0 }
};





opcode_t* holberton_core_get_opcodes() {
  return g_opcodes;
}
