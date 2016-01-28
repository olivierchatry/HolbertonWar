#include <stdlib.h>
#include <string.h>
#include "asm.h"


label_t* label_create(char* name, int32 offset, int32 opcode_offset) {
	label_t* label = (label_t*)malloc(sizeof(label_t));
	label->name = strdup(name);
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
