/*
 * Copyright 2015 Joseph Landry All Rights Reserved
 */

#ifndef FIRSTFOLLOW_H
#define FIRSTFOLLOW_H

struct ff_set_node {
	struct ff_set_node *next;
	int symbolIndex;
};

struct ff_node {
	struct ff_node *next;
	int nonTerminalSymbolIndex;
	int isNullable;
	struct ff_set_node *first;
	struct ff_set_node *follow;
};

extern struct ff_node *firstFollowSet;

int firstfollow();

#endif
