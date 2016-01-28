#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "asm.h"








///////////////////////////////////////////////////////////////////////////////


char* generate_output_file_name(char* input_file_name) {
  int   len   = strlen(input_file_name);
  int   split = len - 1;
  int   size  = 0;
  char* output_file_name = NULL;

  while (--split) {
    if (input_file_name[split] == '.')
      break;
  }

	split = split < 0 ? len : split;

  output_file_name = malloc(size + 7);
  strncpy(output_file_name, input_file_name, split);
  strcat(output_file_name, ".hcor");

  return output_file_name;
}

int find_label(char* label_name, generator_t* generator) {
	label_t* label = label_find_in_list(label_name, generator->labels);
	if (label) {
		return label->opcode_offset - generator->core.code_size;
	}
	else {
		label_t* forward = label_create(label_name, generator->byte_code_offset, generator->core.code_size);
		list_add(&generator->forwards, forward);
		return 0; // place holder value
	}
}

int32 parse_number(char* value) {
	if (*value == 'b') {
		return strtol(value + 1, NULL, 2);
	}
	return strtol(value, NULL, 0);
}

void generate_opcode(opcode_t* opcode, char** output, int count, generator_t* generator) {

	if ((count - 1) != opcode->args) {
		generator->error = ASM_INVALID_ARGUMENT_COUNT;
	}
	else {
		int* arg_type = opcode->arg_type;
		char* opcode_type;


		generator_write8(generator, opcode->opcode);
		opcode_type = generator_write8(generator, 0);

		while (*arg_type && (generator->error == ASM_OK)) {
			char* arg = *output++;

			int parsed_type = 0;
			int value;

			if (*arg == 'r') {
				value = atoi(arg + 1);
				if (value > CORE_REGISTER_COUNT || value < 1) {
					fprintf(stderr, "invalid register r%d line %d\n", value, generator->current_line);
					generator->error = ASM_INVALID_ARGUMENT;
				}
				parsed_type = CORE_ARG_TYPE_REG;
			}
			else if (strchr(arg, CORE_ASM_ADD) != NULL) {
				parsed_type = CORE_ARG_TYPE_ADD;
				value = parse_number(arg + 1);
			}
			else {
				parsed_type = CORE_ARG_TYPE_IMM;
				if (*arg == ':') {
					value = find_label(arg, generator);
				}
				else {
					value = parse_number(arg);
				}
			}

			if (generator->error == ASM_OK) {
				int32 arg_number = (int32) (arg_type - opcode->arg_type);
				if ((parsed_type & *arg_type) == 0) {
					fprintf(stderr, "invalid argument %d, wrong type line %d\n", arg_number + 1, generator->current_line);
					generator->error = ASM_INVALID_ARGUMENT;
				}
				else {
					*opcode_type |= parsed_type << (arg_number * 2);
					if (parsed_type == CORE_ARG_TYPE_REG) {
						generator_write8(generator, (int8)value);
					} else if (parsed_type == CORE_ARG_TYPE_ADD) {
						generator_write16(generator, (int16)value);
					} else {
						generator_write32(generator, value);
					}
				}
			}
			arg_type++;
		}
		generator->core.code_size = generator->byte_code_offset;
	}
}



void generate(char** output, int count, generator_t* generator) {
	if (strcmp(output[0], ".name") == 0) {
		strncpy(generator->core.name, output[1], CORE_FILE_NAME_MAX_SIZE - 1);
	}
	else if (*output[0] == ':') {
		label_t* label = label_create(output[0], generator->core.code_size, generator->core.code_size);
		list_add(&generator->labels, label);
	}
	else {
		opcode_t* opcode = holberton_core_get_opcodes();
		while (opcode->mnemonic) {
			if (strcmp(opcode->mnemonic, output[0]) == 0) {
				break;
			}
			opcode++;
		}
		if (opcode->mnemonic) {
			generate_opcode(opcode, output + 1, count, generator);
		}
		else {
			generator->error = ASM_INVALID_INSTRUCTION;
			fprintf(stderr, "ERROR: unrecognized instruction (%s) line %d\n", output[0], generator->current_line);
		}
	}
}


void write_output_file(char* input_file_name, generator_t* generator) {
	char* output_file_name = generate_output_file_name(input_file_name);
	if (!output_file_name) {
		generator->error = ASM_INVALID_FILE_NAME;
		fprintf(stderr, "ERROR: cannot generate output file name from %s.", input_file_name);
	} else {
		int32	output_size = generator->byte_code_base + generator->byte_code_offset;
		FILE* output = fopen(output_file_name, "wb");
		int32 i;

		generator->byte_code_base = 0;
		generator->byte_code_offset = 0;

		generator_write32(generator, generator->core.magic);
		generator_write32(generator, generator->core.version);
		for (i = 0; i < CORE_FILE_NAME_MAX_SIZE; ++i)
			generator_write8(generator, generator->core.name[i]);
		generator_write32(generator, generator->core.code_size);
		fclose(output);
	}
}

int assemble(char* input_file_name) {
  FILE* 				input;
	char					line[1024];
	generator_t		generator = { 0 };

	input = fopen(input_file_name, "rt");
  if (input) {
		generator_allocate(&generator);
		generator.byte_code_base = sizeof(core_file_header_t);
		while (fgets(line, 1024, input) && generator.error == ASM_OK) {
			char* output[10];
			generator.current_line++;
			int count = parse(line, output, 10);
			if (count > 0) {
				generate(output, count, &generator);
			}
			else if (count < 0) {
				generator.error = ASM_PARSER_ERROR;
				fprintf(stderr, "ERROR: parser fatal error line %d.", generator.current_line);
			}
		}
		fclose(input);

		if (generator.forwards) {
			fprintf(stderr, "ERROR: forward usage of labels not yet supported");
			generator.error = ASM_PARSER_ERROR;
		}

		if (generator.error == ASM_OK) {
			write_output_file(input_file_name, &generator);
		}

		generator_destroy(&generator);
		return generator.error;
  } else {
		fprintf(stderr, "ERROR: cannot open input file %s.", input_file_name);
		return ASM_INVALID_FILE_NAME;
	}
}


int main(int ac, char** av) {
  int i;

  for (i = 1; i < ac; i++) {
    assemble(av[i]);
  }
	return ASM_OK;
}
