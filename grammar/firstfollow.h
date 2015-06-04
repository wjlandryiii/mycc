/*
 * Copyright 2015 Joseph Landry All Rights Reserved
 */

#ifndef FIRSTFOLLOW_H
#define FIRSTFOLLOW_H

#include "defs.h"

// INPUT


/*
#define MAX_SYMBOLS (256)
extern int nSYMBOLS;
extern int SYMBOL[MAX_SYMBOLS];
extern int SYMBOLTYPE[MAX_SYMBOLS];
*/

/*
#define MAX_RULES (256)
#define MAX_RULE_SIZE (128)
extern int nRULES;
extern int RULENAME[MAX_RULES];
extern int RULE[MAX_RULES][MAX_RULE_SIZE];
extern int RULESIZE[MAX_RULES];
*/

// OUTPUT

extern int NULLABLE[MAX_SYMBOLS];

extern int FIRST[MAX_SYMBOLS][MAX_FIRST_SET_SIZE];
extern int FIRSTSIZE[MAX_SYMBOLS];

extern int FOLLOW[MAX_SYMBOLS][MAX_FOLLOW_SET_SIZE];
extern int FOLLOWSIZE[MAX_SYMBOLS];


void computeNullable();
void computeFirst();
void computeFollow();
#endif
