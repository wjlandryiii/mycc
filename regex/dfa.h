/*
 * Copyright 2015 Joseph Landry All Rights Reserved
 */

#ifndef DFA_H
#define DFA_H


struct DState {
	int marked;
	struct set *list;
	int accepting;
};

struct dfa {
	int nstates;
	struct DState states[128];
	int transitions[128][128];
};

struct dfa *dfaNew();
int dfaAddState(struct dfa *dfa, struct set *set);
int dfaMarkState(struct DState *state);
struct DState *dfaNextUnmarkedState(struct dfa *dfa);
struct DState *dfaFindStateWithSet(struct dfa *dfa, struct set *set);
int dfaAddTransition(struct dfa *dfa, struct DState *state, int symbol, struct DState *nextState);
void graphDFA(struct dfa *dfa);

#endif
