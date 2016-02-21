#include <stdlib.h>
#include <string.h>
#include "asm.h"
#include "../common/utils.h"

label_t* label_forward_create(char* name, int32 offset, int32 opcode_offset, int32 type, int32 line) {
	label_t* label = (label_t*)malloc(sizeof(label_t));
	label->name = _strdup(name);
	label->offset = offset;
	label->opcode_offset = opcode_offset;
	label->type = type;
	label->line = line;
	return label;
}

label_t* label_create(char* name, int32 offset, int32 opcode_offset) {
	label_t* label = (label_t*)malloc(sizeof(label_t));
	label->name = _strdup(name);
	label->offset = offset;
	label->opcode_offset = opcode_offset;
	return label;
}

void    label_destroy(void* data) {
	label_t* label = (label_t*)(data);
	free(label->name);
	free(label);
}


label_t*  label_find_in_list(char* name, list_t* labels) {
	while (labels) {
		label_t* label = (label_t*)labels->data;
		if (strcmp(label->name, name) == 0) {
			return label;
		}
		labels = labels->next;
	}
	return NULL;
}

int				label_is_valid(char* name) {
	int len = strlen(name);
	char* end = name + len - 1;

	if (*end == CORE_ASM_LABEL) {
		*end = 0;
		return 1;
	}
	return 0;
}