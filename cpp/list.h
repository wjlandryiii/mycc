/*
 * Copyright 2015 Joseph Landry All Rights Reserved
 */

#ifndef LIST_H
#define LIST_H

struct list;

struct list *newList();
void freeList(struct list *list);
int listPush(struct list *list, void *item);
int listPop(struct list *list, void **itemOut);
int listPopPeek(struct list *list, void **itemOut);
int listEnqueue(struct list *list, void *item);
int listDequeue(struct list *list, void **itemOut);
int listDequeuePeek(struct list *list, void **itemOut);

#endif
