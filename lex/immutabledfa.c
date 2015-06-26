/*
 * Copyright 2015 Joseph Landry All Rights Reserved
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct immutable_dfa {
	int nstates;
	struct immutable_dfa_state *states;
	int *transitions;
};


struct immutable_dfa *loadImmutableDFA(char *filename){
	FILE *f;
	int nStates;
	int size;
	struct immutable_dfa *dfa;

	f = fopen(filename, "r");

	if(f == NULL){
		fprintf(stderr, "error on fopen()\n");
		exit(1);
	}

	if(fscanf(f, "NSTATES %d", &nStates) != 1){
		fprintf(stderr, "error reading NSTATES\n");
		exit(1);
	}

	size = 0;
	size += sizeof(struct immutable_dfa);
	size += sizeof(struct immutable_dfa_state) * nStates;
	size += sizeof(int) * nStates * 128;
	dfa = malloc(size);
	memset(dfa, 0, size);
	dfa->nstates = nStates;
	dfa->states = dfa+1;
	dfa->transitions = dfa->states+nStates;

	for(i = 0; i < nStates; i++){
		for(j = 0; j < 128; j++){
			dfa->transitions[i*128+j] = -1;
		}
	}
}
