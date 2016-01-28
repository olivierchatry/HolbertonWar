#ifndef __LIST_H__
#define __LIST_H__

typedef struct list_s {
	void*						data;
	struct list_s*	next;
} list_t;

void    list_add(list_t** list, void* data);
void    list_destroy(list_t** list, void(*fct)(void*));
list_t* list_get(list_t*list, void* value, int(*fnt)(void*,void*));

#endif
