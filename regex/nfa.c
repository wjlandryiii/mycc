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
	nfa.acceptingState = 1;
	nfa.transitions[0] = Transition(0, 1, c);
	nfa.nTransitions = 1;
	return nfa;
}

struct nfa nfaUnion(struct nfa *nfaA, struct nfa *nfaB){
	int i;
	struct nfa nfa;
	int aShift;
	int bShift;
	int newStartingState;
	int newAcceptingState;

	aShift = 1;
	bShift = 1 + nfaA->states;
	newAcceptingState = 1 + nfaA->states + nfaB->states;
	newStartingState = 0;

	nfa.states = 1 + nfaA->states + nfaB->states + 1;
	nfa.startingState = 0;
	nfa.acceptingState = 1 + nfaA->states + nfaB->states;
	nfa.nTransitions = 0;	
	nfa.transitions[nfa.nTransitions++] = Transition(
			newStartingState,
			aShift + nfaA->startingState,
			NFAEPSILON);
	nfa.transitions[nfa.nTransitions++] = Transition(
			0,
			bShift + nfaB->startingState,
			NFAEPSILON);

	for(i = 0; i < nfaA->nTransitions; i++){
		nfa.transitions[nfa.nTransitions++] = Transition(
				aShift + nfaA->transitions[i].from,
				aShift + nfaA->transitions[i].to,
				nfaA->transitions[i].symbol);
	}

	for(i = 0; i < nfaB->nTransitions; i++){
		nfa.transitions[nfa.nTransitions++] = Transition(
				bShift + nfaB->transitions[i].from,
				bShift + nfaB->transitions[i].to,
				nfaB->transitions[i].symbol);
	}

	nfa.transitions[nfa.nTransitions++] = Transition(
			aShift + nfaA->acceptingState,
			newAcceptingState,
			NFAEPSILON);

	nfa.transitions[nfa.nTransitions++] = Transition(
			bShift + nfaB->acceptingState,
			newAcceptingState,
			NFAEPSILON);
	return nfa;
}

struct nfa nfaConcatenation(struct nfa *nfaA, struct nfa *nfaB){
	int i;
	struct nfa nfa;
	int aShift = 0;
	int bShift = nfaA->states;

	nfa.startingState = nfaA->startingState;
	nfa.states = nfaA->states + nfaB->states;

	nfa.nTransitions = 0;
	for(i = 0; i < nfaA->nTransitions; i++){
		nfa.transitions[nfa.nTransitions++] = nfaA->transitions[i];
	}
	for(i = 0; i < nfaB->nTransitions; i++){
		nfa.transitions[nfa.nTransitions++] = Transition(
				bShift + nfaB->transitions[i].from,
				bShift + nfaB->transitions[i].to,
				nfaB->transitions[i].symbol);
	}
	nfa.transitions[nfa.nTransitions++] = Transition(
			nfaA->acceptingState,
			bShift + nfaB->startingState,
			NFAEPSILON);
	nfa.acceptingState = bShift + nfaB->acceptingState;
	return nfa;
}

struct nfa nfaStar(struct nfa *nfaA){
	int i;
	int newEnd;
	struct nfa nfa;

	memcpy(&nfa, nfaA, sizeof(nfa));

	nfa.transitions[nfa.nTransitions++] = Transition(
			nfa.startingState,
			nfa.acceptingState,
			NFAEPSILON);
	nfa.transitions[nfa.nTransitions++] = Transition(
			nfa.acceptingState,
			nfa.startingState,
			NFAEPSILON);
	nfa.startingState = nfa.acceptingState;

	return nfa;
}


int graphNFA(const struct nfa *nfa){
	int i, j;

	printf("digraph G {\n");
	printf("\trankdir = LR\n");
	printf("\tnode [shape=circle]\n");
	for(i = 0; i < nfa->states; i++){
		if(i != nfa->acceptingState){
			printf("\ts%d\n", i);
		} else {
			printf("\ts%d [shape = doublecircle]\n", i);
		}
	}
	printf("\n");
	printf("\ts%d [xlabel=\"start\"]\n", nfa->startingState);
	printf("\n");

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
