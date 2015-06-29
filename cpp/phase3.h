/*
 * Copyright 2015 Joseph Landry All Rights Reserved
 */

#ifndef PHASE3_H
#define PHASE3_H

struct phase2;
struct pptoken;

struct phase3 {
	int commentNewLineCount;
	struct phase2 *p2;
	int la[4];
	int lb[3];
};

int phase3Init(struct phase3 *p3);
struct pptoken phase3NextToken(struct phase3 *p3);

#endif
