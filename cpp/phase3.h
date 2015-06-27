/*
 * Copyright 2015 Joseph Landry All Rights Reserved
 */

#ifndef PHASE3_H
#define PHASE3_H

struct phase2;
struct token {
	char *lexeme;
	int type;
	int name;
};

struct phase3 {
	struct phase2 *p2;
	int la[3];
	int lb[3];
};

int phase3Init(struct phase3 *p3);
struct token phase3NextToken(struct phase3 *p3);

#endif
