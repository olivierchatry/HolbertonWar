#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include "asm.h"

char* generate_output_file_name(char* input_file_name) {
	int   len   = strlen(input_file_name);
	int   split = len - 1;
	char* output_file_name = NULL;

	while (--split) {
		if (input_file_name[split] == '.')
			break;
	}

	split = split < 0 ? len : split;

	output_file_name = malloc(split + 7);
	memset(output_file_name, 0, split + 7);
	strncpy(output_file_name, input_file_name, split);
	strcat(output_file_name, ".hcor");

	return output_file_name;
}

int label_get_offset(char* label_name, generator_t* generator, int32 type) {
	label_t* label = label_find_in_list(label_name, generator->labels);
	if (label) {
		return label->opcode_offset - generator->core.code_size;
	}
	else {
		label_t* forward = label_forward_create(label_name + 1,
			generator->byte_code_offset,
			generator->core.code_size,
			type,
			generator->current_line);
		list_add(&generator->forwards, forward);
		return 0; // place holder value
	}
}

static int32 parse_number(generator_t* generator, char* value, int32 type) {
	int32 ret;
	char* end;

	if (*value == ':') {
		return label_get_offset(value, generator, type);
	}

	if (*value == 'b') {
		ret = (int32) strtoll(value + 1, &end, 2);
	} else {
		ret = (int32) strtoll(value, &end, 0);
	}
	if (*end != 0) {
		generator->error = ASM_INVALID_NUMBER;
	}
	return ret;
}
	

void generate_instruction(opcode_t* opcode, char** output, int count, generator_t* generator) {
	if ((count - 1) != opcode->arg_count) {
		generator->error = ASM_INVALID_ARGUMENT_COUNT;
		fprintf(stderr, "ERROR: invalid number of arguments (%d, expected %d) line %d\n",
			count - 1,
			opcode->arg_count,
			generator->current_line);
	}
	else {
		char	fake_opcode_type;
		char* opcode_type = &fake_opcode_type;
		int32 processing_flags = opcode->processing_flags;
		int32 arg_number = 0;

		generator_write8(generator, opcode->opcode);
		if ((processing_flags & ASM_PROCESSING_NO_TYPES) != ASM_PROCESSING_NO_TYPES) {
			opcode_type = generator_write8(generator, 0);
		}

		while ( (arg_number < opcode->arg_count) && (generator->error == ASM_OK)) {
			char* arg = *output++;

			int32 parsed_type = 0;
			int32 value;

			if (*arg == 'r') {
				value = atoi(arg + 1);
				if (value > CORE_REGISTER_COUNT || value < 1) {
					fprintf(stderr, "ERROR: invalid register r%d line %d\n", value, generator->current_line);
					generator->error = ASM_INVALID_ARGUMENT;
				}
				parsed_type = OP_ARG_TYPE_REG;
			}
			else if (strchr(arg, CORE_ASM_IMM) != NULL) {
				parsed_type = OP_ARG_TYPE_IMM;
				value = parse_number(generator, arg + 1, (processing_flags & ASM_PROCESSING_IMM_AS_ADD) ? OP_ARG_TYPE_ADD : OP_ARG_TYPE_IMM);
			}
			else {
				parsed_type = OP_ARG_TYPE_ADD;
				value = parse_number(generator, arg, parsed_type);
			}

			if (generator->error == ASM_OK) {
				if ((parsed_type & opcode->arg_types[arg_number]) == 0) {
					fprintf(stderr, "ERROR: invalid argument %d, wrong type line %d\n", arg_number + 1, generator->current_line);
					generator->error = ASM_INVALID_ARGUMENT;
				}
				else {
					int8 type;
					if (parsed_type == OP_ARG_TYPE_REG) {
						type = CORE_ARG_TYPE_REG;
						generator_write8(generator, (int8)value);
					} else if (parsed_type == OP_ARG_TYPE_ADD) {
						type = CORE_ARG_TYPE_ADD;
						generator_write16(generator, (int16)value);
					} else {
						type = CORE_ARG_TYPE_IMM;
						if (processing_flags & ASM_PROCESSING_IMM_AS_ADD) {
							generator_write16(generator, value);
						}
						else {
							generator_write32(generator, value);
						}
					}
					*opcode_type |= (type << (6 - (arg_number * 2)));
				}
			} else if ( generator->error == ASM_INVALID_NUMBER ) {
				fprintf(stderr, "ERROR: invalid number format (%s) for argument %d line %d\n",
					arg,
					arg_number + 1,
					generator->current_line);
			}
			arg_number++;
		}
		generator->core.code_size = generator->byte_code_offset;
	}
}



void generate(char** output, int count, generator_t* generator) {
	if (strcmp(output[0], ".name") == 0) {
		strncpy(generator->core.name, output[1], CORE_FILE_NAME_MAX_SIZE - 1);
	}
	else if (strcmp(output[0], ".comment") == 0) {
		strncpy(generator->core.comment, output[1], CORE_FILE_COMMENT_MAX_SIZE - 1);
	}
	else {
		int offset = 0;
		if (label_is_valid(output[offset])) {
			label_t* label = label_find_in_list(output[offset], generator->labels);
			if (label) {
				generator->error = ASM_LABEL_REDEFINITION;
				fprintf(stderr, "ERROR: label (%s) defined at line %d redefined at line %d\n", output[offset], label->line, generator->current_line);
				offset++;
			}
			else {
				label = label_create(output[offset++], generator->core.code_size, generator->core.code_size);
				label->line = generator->current_line;
				list_add(&generator->labels, label);
			}
		}
		if (offset < count) {
			opcode_t* opcode = holberton_core_get_opcodes();
			while (opcode->mnemonic) {
				if (strcmp(opcode->mnemonic, output[offset]) == 0) {
					break;
				}
				opcode++;
			}
			if (opcode->mnemonic) {
				generate_instruction(opcode, output + offset + 1, count - offset, generator);
			}
			else {
				generator->error = ASM_INVALID_INSTRUCTION;
				fprintf(stderr, "ERROR: unrecognized instruction (%s) line %d\n", output[0], generator->current_line);
			}
		}
	}
}

void write_output_file(char* input_file_name, generator_t* generator) {
	char* output_file_name = generate_output_file_name(input_file_name);
	if (!output_file_name) {
		generator->error = ASM_INVALID_FILE_NAME;
		fprintf(stderr, "ERROR: cannot generate output file name from %s.\n", input_file_name);
	} else {
		int32	output_size = generator->byte_code_base + generator->byte_code_offset;
		FILE* output = fopen(output_file_name, "wb");
		int32 i;
		if (output == NULL) {
			generator->error = ASM_INVALID_OUTPUT_FILE;
			fprintf(stderr, "ERROR: cannot open output file %s.\n", output_file_name);
		}
		else {
			generator->byte_code_base = 0;
			generator->byte_code_offset = 0;

			generator_write32(generator, CORE_FILE_MAGIC);

			for (i = 0; i < CORE_FILE_NAME_MAX_SIZE; ++i) {
				generator_write8(generator, generator->core.name[i]);
			}
			generator_write32(generator, generator->core.code_size);
			for (i = 0; i < CORE_FILE_COMMENT_MAX_SIZE; ++i) {
				generator_write8(generator, generator->core.comment[i]);
			}
			fwrite(generator->byte_code, 1, output_size, output);
			fclose(output);
		}
	}
}

void handle_forward_labels(generator_t* generator) {
	list_t* forwards = generator->forwards;
	int32 	byte_code_offset = generator->byte_code_offset;

	while (forwards && generator->error == ASM_OK) {
		label_t* forward = (label_t*) forwards->data;
		label_t* label = label_find_in_list(forward->name, generator->labels);

		if (!label) {
			fprintf(stderr, "ERROR: label %s not found line %d\n", forward->name, forward->line);
			generator->error = ASM_LABEL_NOT_FOUND;
		} else {
			int32 value = label->opcode_offset - forward->opcode_offset;
			generator->byte_code_offset = forward->offset;
			if (forward->type == OP_ARG_TYPE_ADD)
				generator_write16(generator, (int16) value);
			else
				generator_write32(generator, value);
			forwards = forwards->next;
		}
	}
	generator->byte_code_offset = byte_code_offset;
}

int assemble(char* input_file_name, int big_endian) {
  FILE* 				input;
	char					line[1024];
	generator_t		generator = { 0 };

	input = fopen(input_file_name, "rt");
	if (input) {
		generator_allocate(&generator);
		generator.write_big_endian = big_endian;
		generator.byte_code_base = sizeof(core_file_header_t);
		while (!feof(input) && generator.error == ASM_OK) {
			char* output[10];
			memset(line, 0, 1024);
			fgets(line, 1024, input);
			generator.current_line++;
			int count = parse(line, output, 10);
			if (count > 0) {
				generate(output, count, &generator);
			}
			else if (count < 0) {
				generator.error = ASM_PARSER_ERROR;
				fprintf(stderr, "ERROR: parser fatal error line %d.\n", generator.current_line);
			}
		}
		fclose(input);

		if (generator.forwards) {
			handle_forward_labels(&generator);
		}
		if (generator.error == ASM_OK) {
			write_output_file(input_file_name, &generator);
		}
		generator_destroy(&generator);

		return generator.error;
	} else {
		fprintf(stderr, "ERROR: cannot open input file %s.\n", input_file_name);
		return ASM_INVALID_FILE_NAME;
	}
}


int main(int ac, char** av) {
	int i;
	int big_endian = 0;

	for (i = 1; i < ac; ++i) {
		if (av[i]) {
			if (strcmp(av[i], "-be") == 0)
				big_endian = 1;
			else if (strcmp(av[i], "-le") == 0)
				big_endian = 0;
			else {
				printf("assembling %s to %s endian\n", av[i], big_endian ? "big" : "little");
				assemble(av[i], big_endian);
			}
		}
	}
	return ASM_OK;
}
