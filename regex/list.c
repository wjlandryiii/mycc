/*
 * Copyright 2015 Joseph Landry All Rights Reserved
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "list.h"

struct pointer_list {
	void **items;
	int used;
	int size;
};

struct pointer_list *newPointerList(void){
	struct pointer_list *list = calloc(1, sizeof(struct pointer_list));

	list->used = 0;
	list->size = 16;
	list->items = calloc(list->size, sizeof(void *));
	return list;
}

void freePointerList(struct pointer_list *list){
	free(list->items);
	free(list);
}

int pointerListBumpSize(struct pointer_list *list){
	int amount = 8;

	list->items = realloc(list->items, (list->size + amount) * sizeof(void *));
	assert(list->items != NULL);
	list->size += amount;
	printf("bump list!\n");
	return 0;
}

int pointerListAppend(struct pointer_list *list, void *data){
	if(list->used == list->size){
		pointerListBumpSize(list);
	}
	list->items[list->used] = data;
	return list->used++;
}

void *pointerAtIndex(struct pointer_list *list, int index){
	assert(0 <= index && index < list->used);
	return list->items[index];
}


struct pointer_list_iterator pointerListIterator(struct pointer_list *list){
	struct pointer_list_iterator pli;

	pli.list = list;
	pli.index = 0;
	return pli;
}

int pointerListIteratorNextItem(struct pointer_list_iterator *pli, void **data){
	if(pli->index < pli->list->used){
		*data = pli->list->items[pli->index];
		pli->index += 1;
		return 1;
	} else {
		return 0;
	}
}
