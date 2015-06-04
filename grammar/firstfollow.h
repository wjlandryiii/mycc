/*
 * Copyright 2015 Joseph Landry All Rights Reserved
 */

#ifndef FIRSTFOLLOW_H
#define FIRSTFOLLOW_H


struct ff_set {
	int count;
	int items[64];
};

struct ff_node {
	int nonterminal;
	int isNullable;
	struct ff_set firstSet;
	struct ff_set followSet;
};

extern struct ff_node ffNodes[128];
extern int ffNodeCount;

int firstfollow();

#endif
