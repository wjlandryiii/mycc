/*
 * Copyright 2015 Joseph Landry All Rights Reserved
 */

#ifndef DFA_H
#define DFA_H


struct dfa_state {
	int marked;
	struct set *list;
	int accepting;
	int transition[128];
	int index;
};

struct dfa {
	struct pointer_list *states;
};

struct dfa *dfaNew();
struct dfa_state *dfaAddState(struct dfa *dfa, struct set *set, int accepting);
int dfaMarkState(struct dfa_state *state);
struct dfa_state *dfaNextUnmarkedState(struct dfa *dfa);
struct dfa_state *dfaFindStateWithSet(struct dfa *dfa, struct set *set);
int dfaAddTransition(struct dfa *dfa, struct dfa_state *state, int symbol, struct dfa_state *nextState);
int dfaQueryTransition(struct dfa *dfa, int state, int symbol);
void graphDFA(struct dfa *dfa);

#endif
