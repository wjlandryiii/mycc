/*
 * Copyright 2015 Joseph Landry All Rights Reserved
 */

#ifndef PHASE3_H
#define PHASE3_h

struct phase2;
struct token;

struct phase3 {
	struct phase2 *p2;


};

int phase3Init(struct phase3 *p3);
void phase3NextToken(struct phase3 *p3);

#endif
