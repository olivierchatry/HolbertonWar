#include "list.h"
#include <stdlib.h>

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

list_t* list_get(list_t*list, void* value, int(*fnt)(void*,void*)) {
	while (list) {
		if (fnt(value,list->data) == 0)
			return list;
		list = list->next;
	}
	return NULL;
}
