/*
 * Copyright 2015 Joseph Landry
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "set.h"

static int insertSorted(int *list, const int size, const int n, const int value){
	int i;

	for(i = n; i > 0; i--){
		if(list[i-1] < value){
			list[i] = value;
			break;
		} else {
			list[i] = list[i-1];
		}
	}
	if(i == 0){
		list[i] = value;
	}
	return 0;
}

static int removeSorted(int *list, const int size, const int n, const int value){
	int i;

	for(i = 0; i < n; i++){
		if(list[i] == value){
			break;
		}
	}
	for(; i < n-1; i++){
		list[i] = list[i+1];
	}
	return 0;
}




struct set *newSet(void){
	struct set *set;

	set = calloc(1, sizeof(struct set));
	assert(set != NULL);
	set->used = 0;
	set->size = 4;
	set->items = calloc(set->size, sizeof(int));
	assert(set->items != NULL);
	return set;
}

struct set *newSetFromSet(struct set *s1){
	struct set *set;

	set = calloc(1, sizeof(struct set));
	assert(set != NULL);
	set->used = s1->used;
	set->size = s1->size;
	set->items = calloc(set->size, sizeof(int));
	assert(set->items != NULL);
	memcpy(set->items, s1->items, sizeof(int)*set->used);
	return set;
}


void freeSet(struct set *set){
	free(set->items);
	set->items = 0;
	free(set);
}


static int setBoostSize(struct set *set){
	int amount = 4;
	set->items = realloc(set->items, set->size + amount * sizeof(int));
	assert(set->items != NULL);
	set->size += amount;
	printf("bump!\n");
	return 0;
}


int setInsert(struct set *set, int value){
	int i;

	for(i = 0; i < set->used && set->items[i] != value; i++);
	if(i == set->used){
		if(set->used + 1 <= set->size){
			insertSorted(set->items, set->size, set->used, value);
			set->used += 1;
		} else {
			setBoostSize(set);
			insertSorted(set->items, set->size, set->used, value);
			set->used += 1;
		}
	} else {

	}
	return 0;
}

int setRemove(struct set *set, int value){
	int i;

	for(i = 0; i < set->used && set->items[i] != value; i++);
	if(i < set->used){
		removeSorted(set->items, set->size, set->used, value);
		set->used -= 1;
	}
	return 0;
}

int setAddSet(struct set *set, struct set *s1){
	int i;

	for(i = 0; i < s1->used; i++){
		setInsert(set, s1->items[i]);
	}
	return 0;
}

struct set *newSetFromUnion(struct set *s1, struct set *s2){
	struct set *set;

	set = newSet();
	assert(set != NULL);
	setAddSet(set, s1);
	setAddSet(set, s2);
	return set;
}

struct set *newSetFromInteger(int value){
	struct set *set;

	set = newSet();
	setInsert(set, value);
	return set;
}

int areSetsEqual(struct set *s1, struct set *s2){
	int i;

	if(s1->used != s2->used){
		return 0;
	}

	for(i = 0; i< s1->used; i++){
		if(s1->items[i] != s2->items[i]){
			return 0;
		}
	}
	return 1;
}
