/*
 * Copyright 2015 Joseph Landry All Rights Reserved
 */

#ifndef PHASE1_H
#define PHASE1_H

#if 0
struct phase1 {
	FILE *sourceFile;
	int lookAhead[3];
};

int phase1Init(struct phase1 *p);
int phase1NextChar(struct phase1 *p);
#endif

struct list;
struct list *phase1(struct list *list);

#endif
