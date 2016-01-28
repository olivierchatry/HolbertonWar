#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../common/holberton-core.h"

#define ASM_OK											0
#define ASM_INVALID_FILE_NAME				1
#define ASM_INVALID_INSTRUCTION			2
#define ASM_INVALID_ARGUMENT				3
#define ASM_INVALID_ARGUMENT_COUNT	4
#define ASM_PARSER_ERROR						5

void	 clean(char* line) {
	while (*line) {
		if (*line == ';' || *line == '\n')
			*line = 0;
		else
			line++;
	}
}

#define IS_SPACE(car) ( (car == ' ' ) || (car == '\t') )
#define IS_SEP(car) (car == CORE_ASM_SEP)

int parse(char* line, char** output, int max)
{
	int			count = 0;
	char**	ret = NULL;
	int			state = 0;
	
	clean(line);

	// first we read the opcode ( or command ) 
	while (*line && max > count && state != -1) {
		char c = *line;

		switch (state) {
		case 0:
			if (!IS_SPACE(c)) {
				output[0] = line;
				count++;
				state = 1;
			}
			break;
		case 1:
			if (IS_SPACE(c)) {
				*line = 0;
				if (*output[0] == '.') state = 2;
				else state = 3;
			}
			break;
		case 2:
			if (!IS_SPACE(c)) {
				output[1] = line;
				count++;
				state = -1;
			}
			break;
		case 3:
			if (!IS_SPACE(c)) {
				output[count++] = line;
				state = 4;
			}
			break;
		case 4:
			if (IS_SEP(c))
				*line = 0, state = 3;
			break;
		default:
			return -1;
		}
		line++;
	}
	return count;
}



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

typedef struct list_s {
	void*						data;
	struct list_s*	next;
} list_t;

void list_add(list_t** list, void* data) {
	list_t* new = (list_t*)malloc(sizeof(list_t));
	new->data = data;
	new->next = *list;
	*list = new;
}

void list_destroy(list_t** list, void(*fct)(void*)) {
	while (*list) {
		list_t* current = *list;
		fct(current->data);
		*list = current->next;
		free(current);
	}
	*list = NULL;
}

list_t* list_get(list_t*list, int(*fnt)(void*)) {
	while (list) {
		if (fnt(list->data))
			return list;
		list = list->next;
	}
	return NULL;
}

///////////////////////////////////////////////////////////////////////////////
typedef struct label_s {
	char* name;
	int32	offset;
	int32	opcode_offset;
} label_t;

label_t* label_create(char* name, int32 offset, int32 opcode_offset) {
	label_t* label = (label_t*)malloc(sizeof(label_t));
	label->name = strdup(name);
	label->offset = offset;
	label->opcode_offset = opcode_offset;
	return label;
}

void		label_destroy(label_t* label) {
	free(label->name);
	free(label);
}

typedef struct generator_s {
	list_t*							labels;
	list_t*							forwards;
	char*								generated;
	int									generated_max_size;
	int									current_line;
	core_file_header_t	core;
	int									error;
	int									current_offset;
} generator_t;

int find_label(char* label_name, generator_t* generator) {
	list_t* labels = generator->labels;
	 
	while (labels) {
		label_t* label = (label_t*)labels->data;
		if (strcmp(label->name, label_name) == 0) {
			return label->offset - generator->core.code_size;
		}
		labels = labels->next;
	}
	// no label found, might be forward
	{
		label_t* forward = label_create(label_name, generator->current_offset, generator->core.code_size);
		list_add(&generator->forwards, forward);
		return 0; // place holder value
	}
}

void generate_opcode(opcode_t* opcode, char** output, int count, generator_t* generator) {

	if ((count - 1) != opcode->args) {
		generator->error = ASM_INVALID_ARGUMENT_COUNT;
	}
	else {
		int* arg_type = opcode->arg_type;
		char* opcode_type;
		generator->generated[generator->current_offset++] = opcode->opcode;
		opcode_type = generator->generated + generator->current_offset++;
		*opcode_type = 0;
		while (*arg_type && (generator->error == ASM_OK)) {
			char* arg = output[1 + (int)(arg_type - opcode->arg_type)];

			int parsed_type = 0;
			int value;

			if (*arg == 'r') {
				value = atoi(arg + 1);
				if (value > CORE_REGISTER_COUNT || value < 1) {
					fprintf(stderr, "invalid register r%d line %d\n", value, generator->current_line);
					generator->error = ASM_INVALID_ARGUMENT;
				}
				else {
					parsed_type = CORE_ARG_TYPE_REG;
				}
			}
			else if (strchr(arg, CORE_ASM_ADD) != NULL) {
				parsed_type = CORE_ARG_TYPE_ADD;
				value = strtol(arg + 1, NULL, 16);
			}
			else {
				parsed_type = CORE_ARG_TYPE_IMM;
				if (*arg == ':') {
					value = find_label(arg, generator);
				}
				else {
					value = strtol(arg, NULL, 16);
				}
			}
			
			if (generator->error == ASM_OK) {
				if ((parsed_type & *arg_type) == 0) {
					fprintf(stderr, "invalid argument, wrong type line %d\n", generator->current_line);
					generator->error = ASM_INVALID_ARGUMENT;
				}
				else {
					*opcode_type |= parsed_type << ((int)(arg_type - opcode->arg_type) * 2);
					if (parsed_type == CORE_ARG_TYPE_REG) {
						generator->generated[generator->current_offset++] = (char)value;
					}
					else {
						generator->generated[generator->current_offset++] = (char)((value >> 0) & 0xff);
						generator->generated[generator->current_offset++] = (char)((value >> 8) & 0xff);
						generator->generated[generator->current_offset++] = (char)((value >> 16) & 0xff);
						generator->generated[generator->current_offset++] = (char)((value >> 24) & 0xff);
					}
				}
			}
			arg_type++;
		}
		generator->core.code_size += generator->current_offset;
	}
}



int generate(char** output, int count, generator_t* generator) {
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
			generate_opcode(opcode, output, count, generator);
		}
		else {
			generator->error = ASM_INVALID_INSTRUCTION;
			fprintf(stderr, "ERROR: unrecognized instruction (%s) line %d\n", output[0], generator->current_line);
		}
	}
}

void generator_allocate(generator_t* generator) {
	generator->generated_max_size += 4096;
	generator->generated = (char*)realloc(generator->generated, generator->generated_max_size);
}

void generator_destroy(generator_t* generator) {
	list_destroy(&generator->labels, label_destroy);
	list_destroy(&generator->forwards, label_destroy);
	free(generator->generated);
}

int assemble(char* input_file_name) {
  FILE* input;
	char								line[1024];
	generator_t					generator = { 0 };
	
	input = fopen(input_file_name, "rt");
  if (input) {	
		generator_allocate(&generator);
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
		generator_destroy(&generator);
    fclose(input);
		return generator.error;
  }
  return ASM_INVALID_FILE_NAME;
}


int main(int ac, char** av) {
  int i;

  for (i = 1; i < ac; i++) {
    assemble(av[i]);
  }
}
