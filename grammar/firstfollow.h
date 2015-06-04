/*
 * Copyright 2015 Joseph Landry All Rights Reserved
 */

#ifndef FIRSTFOLLOW_H
#define FIRSTFOLLOW_H

// INPUT
#define MAX_SYMBOLS (256)
#define MAX_RULES (256)
#define MAX_RULE_SIZE (128)

#define TYPE_TERMINAL (1)
#define TYPE_NONTERMINAL (2)


extern int nSYMBOLS;
extern int SYMBOL[MAX_SYMBOLS];
extern int SYMBOLTYPE[MAX_SYMBOLS];

extern int nRULES;
extern int RULENAME[MAX_RULES];
extern int RULE[MAX_RULES][MAX_RULE_SIZE];
extern int RULESIZE[MAX_RULES];

// OUTPUT
#define MAX_FIRST_SET_SIZE (128)
#define MAX_FOLLOW_SET_SIZE (128)

extern int NULLABLE[MAX_SYMBOLS];

extern int FIRST[MAX_SYMBOLS][MAX_FIRST_SET_SIZE];
extern int FIRSTSIZE[MAX_SYMBOLS];

extern int FOLLOW[MAX_SYMBOLS][MAX_FOLLOW_SET_SIZE];
extern int FOLLOWSIZE[MAX_SYMBOLS];


void computeNullable();
void computeFirst();
void computeFollow();
#endif
