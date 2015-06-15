/*
 * Copyright 2015 Joseph Landry All Rightes Reserved
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "nfa.h"

static void nfaAddTransition(struct nfa *nfa, int from, int to, int symbol){
	int i;
	int t;

	assert(0 <= symbol && symbol < 256);

	for(i = 0; i < nfa->nTransitions; i++){
		if(nfa->transitions[i].from == from && nfa->transitions[i].to == to){
			nfa->transitions[i].symbols[symbol] = 1;
			return;
		}
	}

	assert(nfa->nTransitions < 1024);

	t = nfa->nTransitions;
	nfa->nTransitions += 1;

	nfa->transitions[t].from = from;
	nfa->transitions[t].to = to;
	for(i = 0; i < 256; i++){
		nfa->transitions[t].symbols[i] = 0;
	}
	nfa->transitions[t].symbols[symbol] = 1;
	return;
}

struct nfa elementaryOneCharacter(char c){
	struct nfa nfa = {0};
	nfa.states = 2;
	nfa.startingState = 0;
	nfa.acceptingState = 1;
	nfaAddTransition(&nfa, nfa.startingState, nfa.acceptingState, c);
	return nfa;
}

struct nfa elementaryAny(){
	int i;
	struct nfa nfa;

	nfa.states = 2;
	nfa.startingState = 0;
	nfa.acceptingState = 1;
	nfa.nTransitions = 0;

	// TODO: what range is really any?
	for(i = 0x20; i < 0x7F; i++){
		nfaAddTransition(&nfa, nfa.startingState, nfa.acceptingState, i);
	}
	fprintf(stderr, "die\n");
	return nfa;
}

struct nfa elementarySet(char *set){
	int i;
	struct nfa nfa;

	nfa.states = 2;
	nfa.startingState = 0;
	nfa.acceptingState = 1;
	nfa.nTransitions = 0;
	for(i = 0; i < 128; i++){
		if(set[i]){
			nfaAddTransition(&nfa, nfa.startingState, nfa.acceptingState, i);
		}
	}
	return nfa;
}

struct nfa nfaUnion(struct nfa *nfaA, struct nfa *nfaB){
	int i, j;
	struct nfa nfa;
	int aShift;
	int bShift;
	int newAcceptingState;

	aShift = 1;
	bShift = 1 + nfaA->states;
	newAcceptingState = 1 + nfaA->states + nfaB->states;

	nfa.states = 1 + nfaA->states + nfaB->states + 1;
	nfa.startingState = 0;
	nfa.acceptingState = 1 + nfaA->states + nfaB->states;
	nfa.nTransitions = 0;
	nfaAddTransition(&nfa, nfa.startingState, aShift + nfaA->startingState, NFAEPSILON);
	nfaAddTransition(&nfa, nfa.startingState, bShift + nfaB->startingState, NFAEPSILON);

	for(i = 0; i < nfaA->nTransitions; i++){
		for(j = 0; j < 256; j++){
			if(nfaA->transitions[i].symbols[j]){
				nfaAddTransition(
						&nfa,
						aShift + nfaA->transitions[i].from,
						aShift + nfaA->transitions[i].to,
						j);
			}
		}
	}

	for(i = 0; i < nfaB->nTransitions; i++){
		for(j = 0; j < 256; j++){
			if(nfaB->transitions[i].symbols[j]){
				nfaAddTransition(
						&nfa,
						bShift + nfaB->transitions[i].from,
						bShift + nfaB->transitions[i].to,
						j);
			}
		}
	}

	nfaAddTransition(&nfa, aShift + nfaA->acceptingState, nfa.acceptingState, NFAEPSILON);
	nfaAddTransition(&nfa, bShift + nfaB->acceptingState, nfa.acceptingState, NFAEPSILON);
	return nfa;
}

struct nfa nfaConcatenation(struct nfa *nfaA, struct nfa *nfaB){
	int i, j;
	struct nfa nfa;
	int aShift = 0;
	int bShift = nfaA->states;

	nfa.startingState = nfaA->startingState;
	nfa.states = nfaA->states + nfaB->states;

	nfa.nTransitions = 0;
	for(i = 0; i < nfaA->nTransitions; i++){
		for(j = 0; j < 256; j++){
			if(nfaA->transitions[i].symbols[j]){
				nfaAddTransition(
						&nfa, 
						nfaA->transitions[i].from, 
						nfaA->transitions[i].to,
						j);
			}
		}
	}
	for(i = 0; i < nfaB->nTransitions; i++){
		for(j = 0; j < 256; j++){
			if(nfaB->transitions[i].symbols[j]){
				nfaAddTransition(
						&nfa, 
						bShift + nfaB->transitions[i].from,
						bShift + nfaB->transitions[i].to,
						j);
			}
		}
	}

	nfaAddTransition(&nfa, nfaA->acceptingState, bShift + nfaB->startingState, NFAEPSILON);
	nfa.acceptingState = bShift + nfaB->acceptingState;
	return nfa;
}

struct nfa nfaStar(struct nfa *nfaA){
	int i;
	int newEnd;
	struct nfa nfa;

	memcpy(&nfa, nfaA, sizeof(nfa));

	nfaAddTransition(&nfa, nfa.startingState, nfa.acceptingState, NFAEPSILON);
	nfaAddTransition(&nfa, nfa.acceptingState, nfa.startingState, NFAEPSILON);

	nfa.startingState = nfa.acceptingState;

	return nfa;
}


void graphTransition(const struct transition *t){
	int i, j;

	int ranges[256];
	int nRanges = 0;

	for(i = 0; i < 256; i++){
		if(t->symbols[i]){
			ranges[nRanges*2] = i;
			if(i < 255 && t->symbols[i+1]){
				for(j = i+1; j < 256; j++){
					if(!t->symbols[j]){
						break;
					}
				}
				ranges[nRanges*2+1] = j-1;
				i = j;
			} else {
				ranges[nRanges*2+1] = i;
			}
			nRanges++;
		}
	}

	printf("\ts%d -> s%d [label = \"[", t->from, t->to);

	for(i = 0; i < nRanges; i++){
		if(i>0){
			printf(",");
		}
		if(ranges[i*2] == ranges[i*2+1]){
			printf("0x%02x", ranges[i*2]);
		} else {
			printf("0x%02x-0x%02x", ranges[i*2], ranges[i*2+1]);
		}
	}
	printf("]\"]\n");
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
		graphTransition(&nfa->transitions[i]);
	}
	printf("\n");
	printf("}\n");
	return 0;
}
