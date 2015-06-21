/*
 * Copyright 2015 Joseph Landry All Rights Reserved
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "set.h"
#include "dfa.h"


struct dfa *dfaNew(){
	int i, j;
	struct dfa *dfa = calloc(1, sizeof(struct dfa));

	for(i = 0; i < 128; i++){
		for(j = 0; j < 128; j++){
			dfa->transitions[i][j] = -1;
		}
	}
	return dfa;
}


int dfaAddState(struct dfa *dfa, struct set *set){
	int stateIndex;
	struct DState *state;
	int i;

	assert(dfa->nstates < 128);

	stateIndex = dfa->nstates++;
	state = &dfa->states[stateIndex];

	state->marked = 0;
	state->accepting = 0;
	// state->nList = n;
	// for(i = 0; i < n; i++){
	// 	state->list[i] = s[i];
	// }
	state->list = newSetFromSet(set);
	return stateIndex;
}

int dfaMarkState(struct DState *state){
	state->marked = 1;
	return 0;
}

struct DState *dfaNextUnmarkedState(struct dfa *dfa){
	int i;

	for(i = 0; i < dfa->nstates; i++){
		if(!dfa->states[i].marked){
			return &dfa->states[i];
		}
	}
	return NULL;
}

struct DState *dfaFindStateWithSet(struct dfa *dfa, struct set *set){
	int i, j;

	for(i = 0; i < dfa->nstates; i++){
		if(areSetsEqual(dfa->states[i].list, set)){
			return &dfa->states[i];
		}
	}
	return NULL;
}

int dfaAddTransition(struct dfa *dfa, struct DState *state, int symbol, struct DState *nextState){
	// TODO: check this pointer math.. Seems to work
	int stateIndex = state - dfa->states;
	int nextStateIndex = nextState - dfa->states;
	dfa->transitions[stateIndex][symbol] = nextStateIndex;
	return 0;
}


void graphDFA(struct dfa *dfa){
	int i, j;

	printf("digraph G {\n");
	printf("\trankdir = LR\n");

	for(i = 0; i < dfa->nstates; i++){
		printf("\tnode%d [label=\"[", i);
		struct set_iterator si;
		int value;
		si = setIterator(dfa->states[i].list);
		while(nextSetItem(&si, &value)){
			printf(" %d", value);
		}
		printf(" ]\"");
		if(dfa->states[i].accepting){
			printf(", shape=doublecircle");
		} else {
			printf(", shape=circle");
		}
		printf("]\n");
	}

	for(i = 0; i < dfa->nstates; i++){
		for(j = 0; j < 128; j++){
			if(dfa->transitions[i][j] >= 0){
				printf("\tnode%d -> node%d [label=\"%c\"]\n", i, dfa->transitions[i][j], j);
			}
		}
	}

	printf("}\n");


}
