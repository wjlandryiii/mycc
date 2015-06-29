/*
 * Copyright 2015 Joseph Landry All Rights Reserved
 */

#ifndef STATESTACK_H
#define STATESTACK_H

struct state_stack;

struct state_stack *newStateStack();
void freeStateStack(struct state_stack *ss);
int stateStackPeek(struct state_stack *ss);
int stateStackPush(struct state_stack *ss, int state);
int stateStackPop(struct state_stack *ss);

#endif
