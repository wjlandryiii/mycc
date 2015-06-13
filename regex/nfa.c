/*
 * Copyright 2015 Joseph Landry All Rightes Reserved
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "nfa.h"

static struct transition Transition(int from, int to, char symbol){
	struct transition transition;

	transition.from = from;
	transition.to = to;
	transition.symbol = symbol;
	return transition;
}

struct nfa elementaryOneCharacter(char c){
	struct nfa nfa;
	nfa.states = 2;
	nfa.startingState = 0;
	nfa.acceptingStates[0] = 1;
	nfa.nAcceptingStates = 1;
	nfa.transitions[0] = Transition(0, 1, c);
	nfa.nTransitions = 1;
	return nfa;
}

struct nfa nfaUnion(struct nfa *nfaA, struct nfa *nfaB){
	int i;
	struct nfa nfa;

	nfa.states = 1 + nfaA->states + nfaB->states + 1;
	nfa.startingState = 0;
	nfa.acceptingStates[0] = 1 + nfaA->states + nfaB->states;
	nfa.nAcceptingStates = 1;
	nfa.nTransitions = 0;	
	nfa.transitions[nfa.nTransitions++] = Transition(0, nfaA->startingState+1, NFAEPSILON);
	nfa.transitions[nfa.nTransitions++] = Transition(0, 1+nfaA->states + nfaB->startingState, NFAEPSILON);

	for(i = 0; i < nfaA->nTransitions; i++){
		nfa.transitions[nfa.nTransitions++] = Transition(nfaA->transitions[i].from+1,
				nfaA->transitions[i].to+1,
				nfaA->transitions[i].symbol);
	}

	for(i = 0; i < nfaB->nTransitions; i++){
		nfa.transitions[nfa.nTransitions++] = Transition(nfaA->states + nfaB->transitions[i].from+1,
				nfaA->states + nfaB->transitions[i].to+1,
				nfaB->transitions[i].symbol);
	}

	nfa.nAcceptingStates = 1;
	for(i = 0; i < nfaA->nAcceptingStates; i++){
		nfa.transitions[nfa.nTransitions++] = Transition(
				nfaA->acceptingStates[i] + 1,
				nfa.states-1,
				NFAEPSILON);
	}

	for(i = 0; i < nfaB->nAcceptingStates; i++){
		nfa.transitions[nfa.nTransitions++] = Transition(
				nfaA->states + nfaB->acceptingStates[i] + 1,
				nfa.states-1,
				NFAEPSILON);
	}
	return nfa;
}

struct nfa nfaConcatenation(struct nfa *nfaA, struct nfa *nfaB){
	int i;
	struct nfa nfa;

	nfa.startingState = nfaA->startingState;
	nfa.states = nfaA->states + nfaB->states;

	nfa.nTransitions = 0;
	for(i = 0; i < nfaA->nTransitions; i++){
		nfa.transitions[nfa.nTransitions++] = nfaA->transitions[i];
	}
	for(i = 0; i < nfaB->nTransitions; i++){
		nfa.transitions[nfa.nTransitions++] = Transition(
				nfaA->states + nfaB->transitions[i].from,
				nfaA->states + nfaB->transitions[i].to,
				nfaB->transitions[i].symbol);
	}
	for(i = 0; i < nfaA->nAcceptingStates; i++){
		nfa.transitions[nfa.nTransitions++] = Transition(
				nfaA->acceptingStates[i],
				nfaA->states + nfaB->startingState,
				NFAEPSILON);
	}
	nfa.nAcceptingStates = 0;
	for(i = 0; i < nfaB->nAcceptingStates; i++){
		nfa.acceptingStates[nfa.nAcceptingStates++] = nfaA->states + nfaB->acceptingStates[i];
	}
	return nfa;
}

struct nfa nfaStar(struct nfa *nfaA){
	int i;
	int newEnd;
	struct nfa nfa;

	memcpy(&nfa, nfaA, sizeof(nfa));

	if(nfa.nAcceptingStates > 1){
		fprintf(stderr, "nfaStar() feature not implemented\n");
		exit(1);	
	} else if(nfa.nAcceptingStates == 1){
		nfa.transitions[nfa.nTransitions++] = Transition(
				nfa.startingState,
				nfa.acceptingStates[0],
				NFAEPSILON);
		nfa.transitions[nfa.nTransitions++] = Transition(
				nfa.acceptingStates[0],
				nfa.startingState,
				NFAEPSILON);
		nfa.startingState = nfa.acceptingStates[0];
	}


	return nfa;
}


int graphNFA(const struct nfa *nfa){
	int i, j;

	printf("digraph G {\n");
	printf("\trankdir = LR\n");
	printf("\tnode [shape=circle]\n");
	for(i = 0; i < nfa->states; i++){
		printf("\ts%d\n", i);
	}
	printf("\n");
	printf("\ts%d [xlabel=\"start\"]\n", nfa->startingState);
	printf("\n");
	for(i = 0; i < nfa->nAcceptingStates; i++){
		printf("\ts%d [shape = doublecircle]\n", nfa->acceptingStates[i]);
	}

	for(i = 0; i < nfa->nTransitions; i++){
		if(nfa->transitions[i].symbol == NFAEPSILON){
			printf("\ts%d -> s%d [label = \"\\{\\}\"]\n",
					nfa->transitions[i].from,
					nfa->transitions[i].to);
		} else {
			printf("\ts%d -> s%d [label = \"%c\"]\n",
					nfa->transitions[i].from,
					nfa->transitions[i].to,
					nfa->transitions[i].symbol);
		}
	}
	printf("\n");
	printf("}\n");
	return 0;
}
