/*
 * Copyright 2015 Joseph Landry All Rights Reserved
 */

#ifndef SET_H
#define SET_H

struct set {
	int *items;
	int used;
	int size;
};

struct set *newSet(void);
struct set *newSetFromSet(struct set *s1);
void freeSet(struct set *set);
int setInsert(struct set *set, int value);
int setRemove(struct set *set, int value);
int setAddSet(struct set *set, struct set *s1);
struct set *newSetFromUnion(struct set *s1, struct set *s2);
struct set *newSetFromInteger(int value);
int areSetsEqual(struct set *s1, struct set *s2);

#endif
