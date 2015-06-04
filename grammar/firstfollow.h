/*
 * Copyright 2015 Joseph Landry All Rights Reserved
 */

#ifndef FIRSTFOLLOW_H
#define FIRSTFOLLOW_H

#include "defs.h"

extern int NULLABLE[MAX_SYMBOLS];

extern int FIRST[MAX_SYMBOLS][MAX_FIRST_SET_SIZE];
extern int FIRSTSIZE[MAX_SYMBOLS];

extern int FOLLOW[MAX_SYMBOLS][MAX_FOLLOW_SET_SIZE];
extern int FOLLOWSIZE[MAX_SYMBOLS];

extern int RULENULLABLE[MAX_RULES];
extern int RULEFIRST[MAX_RULES][MAX_FIRST_SET_SIZE];
extern int RULEFIRSTSIZE[MAX_RULES];
extern int RULEFOLLOW[MAX_RULES][MAX_FOLLOW_SET_SIZE];
extern int RULEFOLLOWSIZE[MAX_RULES];

int firstfollow();

#endif
