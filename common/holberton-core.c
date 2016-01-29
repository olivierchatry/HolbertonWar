#include "holberton-core.h"

static opcode_t g_opcodes[] = {
  {0x01, "add", 1, 3, {
      OP_ARG_TYPE_REG | OP_ARG_TYPE_IMM,
      OP_ARG_TYPE_REG | OP_ARG_TYPE_IMM,
      OP_ARG_TYPE_REG
    }
  },
  {0x02, "sub", 1, 3, {
      OP_ARG_TYPE_REG | OP_ARG_TYPE_IMM,
      OP_ARG_TYPE_REG | OP_ARG_TYPE_IMM,
      OP_ARG_TYPE_REG
    }
  },
  {0x03, "and", 1, 3, {
      OP_ARG_TYPE_REG | OP_ARG_TYPE_IMM,
      OP_ARG_TYPE_REG | OP_ARG_TYPE_IMM,
      OP_ARG_TYPE_REG
    }
  },
  {0x04, "or",  1, 3, {
      OP_ARG_TYPE_REG | OP_ARG_TYPE_IMM,
      OP_ARG_TYPE_REG | OP_ARG_TYPE_IMM,
      OP_ARG_TYPE_REG
    }
  },
  {0x05, "xor", 1, 3, {
      OP_ARG_TYPE_REG | OP_ARG_TYPE_IMM,
      OP_ARG_TYPE_REG | OP_ARG_TYPE_IMM,
      OP_ARG_TYPE_REG
    }
  },
  {0x06, "jnz", 1, 1, {
      OP_ARG_TYPE_REG | OP_ARG_TYPE_IMM
    },
  },
  {0x07, "jmp", 1, 1, {
      OP_ARG_TYPE_IMM
    },
  },
  {0x08, "ld",  1, 2, {
      OP_ARG_TYPE_ADD | OP_ARG_TYPE_IMM | OP_ARG_TYPE_REG,
			OP_ARG_TYPE_REG
    }
  },
  {0x09, "st",  1, 2, {
      OP_ARG_TYPE_REG,
      OP_ARG_TYPE_IMM
    }
  },
  {0x0a, "fork",  1, 1, {
      OP_ARG_TYPE_IMM
    }
  },
  {0x0b, "die", 1, 0},
  {0x0c, "live", 1, 1, {
			OP_ARG_TYPE_IMM
		}
	},
	{0x0d, "ldi",  1, 3, {
      OP_ARG_TYPE_REG | OP_ARG_TYPE_IMM | OP_ARG_TYPE_ADD,
			OP_ARG_TYPE_IMM | OP_ARG_TYPE_REG,
			OP_ARG_TYPE_REG
    }
  },
  {0x0e, "sti",  1, 3, {
      OP_ARG_TYPE_REG,
			OP_ARG_TYPE_REG | OP_ARG_TYPE_IMM | OP_ARG_TYPE_ADD,
      OP_ARG_TYPE_IMM | OP_ARG_TYPE_REG,
    }
  },
	{0, 0}
};

opcode_t* holberton_core_get_opcodes() {
  return g_opcodes;
}
