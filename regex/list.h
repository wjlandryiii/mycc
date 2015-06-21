/*
 * Copyright 2015 Joseph Landry All Rights Reserved
 */

#ifndef LIST_H
#define LIST_H

struct pointer_list;

struct pointer_list *newPointerList(void);
void freePointerList(struct pointer_list *list);
int pointerListAppend(struct pointer_list *list, void *data);
void *pointerAtIndex(struct pointer_list *list, int index);

struct pointer_list_iterator {
	struct pointer_list *list;
	int index;
};

struct pointer_list_iterator pointerListIterator(struct pointer_list *list);
int pointerListIteratorNextItem(struct pointer_list_iterator *pli, void **data);

#endif
