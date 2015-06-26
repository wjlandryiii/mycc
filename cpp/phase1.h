/*
 * Copyright 2015 Joseph Landry All Rights Reserved
 */

#ifndef PHASE1_H
#define PHASE1_H

struct phase1 {
	FILE *sourceFile;
	int lookAhead[3];
};

int phase1Init(struct phase1 *p);
int phase1NextChar(struct phase1 *p);

#endif
