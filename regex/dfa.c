/*
 * Copyright 2015 Joseph Landry All Rights Reserved
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "set.h"
#include "list.h"
#include "dfa.h"


struct dfa *dfaNew(){
	struct dfa *dfa = calloc(1, sizeof(struct dfa));
	assert(dfa != NULL);
	dfa->states = newPointerList();
	assert(dfa->states != NULL);
	return dfa;
}


struct dfa_state *dfaAddState(struct dfa *dfa, struct set *set, int accepting){
	int stateIndex;
	struct dfa_state *state;
	int i;

	state = calloc(1, sizeof(struct dfa_state));
	assert(state != NULL);

	state->marked = 0;
	state->accepting = accepting;
	state->list = newSetFromSet(set);
	for(i = 0; i < 128; i++){
		state->transition[i] = -1;
	}

	state->index =  pointerListAppend(dfa->states, state);
	return state;
}

int dfaMarkState(struct dfa_state *state){
	state->marked = 1;
	return 0;
}

struct dfa_state *dfaNextUnmarkedState(struct dfa *dfa){
	struct pointer_list_iterator pli;
	struct dfa_state *state;

	pli = pointerListIterator(dfa->states);
	while(pointerListIteratorNextItem(&pli, (void**)&state)){
		if(state->marked == 0){
			return state;
		}
	}
	return NULL;
}

struct dfa_state *dfaFindStateWithSet(struct dfa *dfa, struct set *set){
	int i, j;

	struct pointer_list_iterator pli;
	struct dfa_state *state;
	pli = pointerListIterator(dfa->states);
	while(pointerListIteratorNextItem(&pli, (void**)&state)){
		if(areSetsEqual(state->list, set)){
			return state;
		}
	}
	return NULL;
}

int dfaAddTransition(struct dfa *dfa, struct dfa_state *s, int symbol, struct dfa_state *toState){
	s->transition[symbol] = toState->index;
	return 0;
}

int dfaQueryTransition(struct dfa *dfa, int stateIndex, int symbol){
	struct dfa_state *state;

	state = pointerAtIndex(dfa->states, stateIndex);
	assert(state != NULL);
	assert(0 <= symbol && symbol < 128);
	return state->transition[symbol];
}


void graphDFA(struct dfa *dfa){
	struct pointer_list_iterator pli;
	struct dfa_state *state;
	struct set_iterator si;
	int value;
	int j;

	printf("digraph G {\n");
	printf("\trankdir = LR\n");

	pli = pointerListIterator(dfa->states);
	while(pointerListIteratorNextItem(&pli, (void**)&state)){
		printf("\tnode%d [label=\"[", pli.index);

		si = setIterator(state->list);
		while(nextSetItem(&si, &value)){
			printf(" %d", value);
		}
		printf(" ]\"");
		if(state->accepting){
			printf(", shape=doublecircle");
		} else {
			printf(", shape=circle");
		}
		printf("]\n");
	}

	pli = pointerListIterator(dfa->states);
	while(pointerListIteratorNextItem(&pli, (void**)&state)){
		for(j = 0; j < 128; j++){
			if(state->transition[j] >= 0){
				printf("\tnode%d -> node%d [label=\"%c\"]\n",
						state->index,
						state->transition[j],
						j);
			}
		}
	}

	printf("}\n");
}
