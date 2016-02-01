#include "holberton-core.h"

static opcode_t g_opcodes[] = {
  {0x01, "add", 10, 3, {
      OP_ARG_TYPE_REG | OP_ARG_TYPE_IMM,
      OP_ARG_TYPE_REG | OP_ARG_TYPE_IMM,
      OP_ARG_TYPE_REG
    }
  },
  {0x02, "sub", 10, 3, {
      OP_ARG_TYPE_REG | OP_ARG_TYPE_IMM,
      OP_ARG_TYPE_REG | OP_ARG_TYPE_IMM,
      OP_ARG_TYPE_REG
    }
  },
  {0x03, "and", 10, 3, {
      OP_ARG_TYPE_REG | OP_ARG_TYPE_IMM,
      OP_ARG_TYPE_REG | OP_ARG_TYPE_IMM,
      OP_ARG_TYPE_REG
    }
  },
  {0x04, "or",  10, 3, {
      OP_ARG_TYPE_REG | OP_ARG_TYPE_IMM,
      OP_ARG_TYPE_REG | OP_ARG_TYPE_IMM,
      OP_ARG_TYPE_REG
    }
  },
  {0x05, "xor", 10, 3, {
      OP_ARG_TYPE_REG | OP_ARG_TYPE_IMM,
      OP_ARG_TYPE_REG | OP_ARG_TYPE_IMM,
      OP_ARG_TYPE_REG
    }
  },
  {0x06, "jnz", 10, 1, {
      OP_ARG_TYPE_REG | OP_ARG_TYPE_IMM
    },
  },
  {0x07, "jmp", 8, 1, {
      OP_ARG_TYPE_IMM
    },
  },
  {0x08, "ld",  5, 2, {
      OP_ARG_TYPE_ADD | OP_ARG_TYPE_IMM | OP_ARG_TYPE_REG,
			OP_ARG_TYPE_REG
    }
  },
  {0x09, "st",  2, 2, {
      OP_ARG_TYPE_REG,
      OP_ARG_TYPE_IMM
    }
  },
  {0x0a, "fork",  600, 1, {
      OP_ARG_TYPE_IMM
    }
  },
  {0x0b, "die", 5, 0},
  {0x0c, "live", 5, 1, {
			OP_ARG_TYPE_IMM
		}
	},
	{0x0d, "ldi",  25, 3, {
      OP_ARG_TYPE_REG | OP_ARG_TYPE_IMM | OP_ARG_TYPE_ADD,
			OP_ARG_TYPE_IMM | OP_ARG_TYPE_REG,
			OP_ARG_TYPE_REG
    }
  },
  {0x0e, "sti",  2, 3, {
      OP_ARG_TYPE_REG,
			OP_ARG_TYPE_REG | OP_ARG_TYPE_IMM | OP_ARG_TYPE_ADD,
      OP_ARG_TYPE_IMM | OP_ARG_TYPE_REG,
    }
  },
	{ 0x0f, "cycles",  1, 3,{
			OP_ARG_TYPE_REG
		}
	},
	{0, 0}
};

opcode_t* holberton_core_get_opcodes() {
  return g_opcodes;
}
