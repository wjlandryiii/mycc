/*
 * Copyright 2015 Joseph Landry
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "list.h"

struct list {
	void **buf;
	int size;
	int count;
};

struct list *newList(){
	struct list *list;

	list = calloc(1, sizeof(*list));
	list->size = 32;
	list->count = 0;
	list->buf = calloc(list->size, sizeof(void *));
	return list;
}

void freeList(struct list *list){
	free(list->buf);
	list->buf = NULL;
	free(list);
}

static int listIncreaseSize(struct list *list){
	int newSize;
	void **newBuf;

	newSize = list->size * 2;
	newBuf = realloc(list->buf, newSize * sizeof(void *));
	assert(newBuf != NULL);

	list->buf = newBuf;
	list->size = newSize;
	return 0;
}

int listPush(struct list *list, void *item){
	if(list->size <= list->count){
		listIncreaseSize(list);
	}
	assert(list->count < list->size);
	list->buf[list->count] = item;
	list->count += 1;
	return 0;
}

int listPop(struct list *list, void **itemOut){
	if(list->count > 0){
		if(itemOut){
			*itemOut = list->buf[list->count - 1];
		}
		list->count -= 1;
		return 0;
	} else {
		*itemOut = NULL;
		return -1;
	}
}

int listPopPeek(struct list *list, void **itemOut){
	if(list->count > 0){
		*itemOut = list->buf[list->count - 1];
		return 0;
	} else {
		*itemOut = NULL;
		return -1;
	}
}

int listEnqueue(struct list *list, void *item){
	if(list->size <= list->count){
		listIncreaseSize(list);
	}
	assert(list->count < list->size);
	/*
	if(list->count == 0){
		list->buf[0] = item;
		list->count += 1;
		return 0;
	} else {
		memmove(list->buf+1, list->buf, list->count * sizeof(void *));
		list->buf[0] = item;
		list->count += 1;
		return 0;
	}
	*/

	return listPush(list, item);
}

int listDequeue(struct list *list, void **itemOut){
	if(list->count > 0){
		if(itemOut){
			*itemOut = list->buf[0];
		}
		memmove(list->buf, list->buf+1, (list->count - 1) * sizeof(void *));
		list->count -= 1;
		return 0;
	} else {
		*itemOut = NULL;
		return -1;
	}

	//return listPop(list, itemOut);
}

int listDequeuePeek(struct list *list, void **itemOut){
	return listItemAtIndex(list, 0, itemOut);
}

int listItemAtIndex(struct list *list, int index, void **itemOut){
	assert(0 <= index && index < list->count);

	*itemOut = list->buf[index];
	return 0;
}

int listItemCount(struct list *list){
	return list->count;
}



#ifdef TESTING

char *testStrings[] = {
	"racecar",
	"watch",
	"worm",
	"rates",
	"hate",
	"charge",
	"free",
	"pie",
	"never",
	"monkey",
	"shift",
	"make",
	NULL,
};

void test_listQueue(){
	struct list *list;
	int i;
	char *s;

	list = newList();
	assert(list != NULL);

	assert(listDequeue(list, (void**)&s) != 0);

	i = 0;
	while(testStrings[i]){
		assert(listEnqueue(list, testStrings[i]) == 0);
		i += 1;
	}

	i = 0;
	while(testStrings[i]){
		listDequeuePeek(list, (void**)&s);
		assert(strcmp(s, testStrings[i]) == 0);
		s = NULL;
		listDequeue(list, (void**)&s);
		assert(strcmp(s, testStrings[i]) == 0);
		i += 1;
	}

	freeList(list);
}

void test_listStack(){
	struct list *list;
	int i;
	char *s;

	list = newList();
	assert(list != NULL);

	assert(listPop(list, (void **)&s) != 0);

	i = 0;
	while(testStrings[i]){
		assert(listPush(list, testStrings[i]) == 0);
		i += 1;
	}

	i -= 1;
	while(i >= 0){
		assert(listPopPeek(list, (void**)&s) == 0);
		assert(strcmp(s, testStrings[i]) == 0);
		s = NULL;
		assert(listPop(list, (void**)&s) == 0);
		assert(strcmp(s, testStrings[i]) == 0);
		i -= 1;
	}
}

int main(int argc, char *argv[]){
	test_listQueue();
	test_listStack();
	//test_listArray();

	printf("OK\n");

	return 0;
}

#endif
