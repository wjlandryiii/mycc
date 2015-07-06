/*
 * Copyright 2015 Joseph Landry All Rights Reserved
 */

#ifndef PHASE2_H
#define PHASE2_H

#if 0
struct phase1;

struct phase2 {
	struct phase1 *p1;
	int lookAhead[2];
	int spliceCount;
};


int phase2Init(struct phase2 *p2);
int phase2NextChar(struct phase2 *p2);
#endif

struct list;
struct list *phase2(struct list *list);


#endif
