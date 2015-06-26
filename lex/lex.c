/*
 * Copyright 2015 Joseph Landry All Rights Reserved
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


char inputString[1024];

enum ACTION {
	FAIL = 0,
	SKIP,
	RETURN,
};

struct dfa_state {
	int accepting;
	int action;
};


struct dfa {
	int nstates;
	struct dfa_state states[128];
	int transitions[128][128];
};

struct dfa *loadDFA(){
	/*
	 * 0: START
	 * 1: white space
	 * 2: Number
	 * 3: "+"
	 * 4: "-"
	 * 5: "*'
	 * 6: "/"
	 */

	struct dfa *dfa = calloc(1, sizeof(struct dfa));
	int i;
	int j;

	for(i = 0; i < 128; i++){
		for(j = 0; j < 128; j++){
			dfa->transitions[i][j] = -1;
		}
	}

	dfa->transitions[0][' '] = 1;
	dfa->transitions[0]['\t'] = 1;
	dfa->transitions[0]['\n'] = 1;

	dfa->transitions[0]['0'] = 2;
	dfa->transitions[0]['1'] = 2;
	dfa->transitions[0]['2'] = 2;
	dfa->transitions[0]['3'] = 2;
	dfa->transitions[0]['4'] = 2;
	dfa->transitions[0]['5'] = 2;
	dfa->transitions[0]['6'] = 2;
	dfa->transitions[0]['7'] = 2;
	dfa->transitions[0]['8'] = 2;

	dfa->transitions[0]['+'] = 3;
	dfa->transitions[0]['-'] = 4;
	dfa->transitions[0]['*'] = 5;
	dfa->transitions[0]['/'] = 6;
	dfa->states[0].accepting = 0;
	dfa->states[0].action = FAIL;

	dfa->transitions[1][' '] = 1;
	dfa->transitions[1]['\t'] = 1;
	dfa->transitions[1]['\n'] = 1;
	dfa->states[1].accepting = 1;
	dfa->states[1].action = SKIP;

	dfa->transitions[2]['9'] = 2;
	dfa->transitions[2]['0'] = 2;
	dfa->transitions[2]['1'] = 2;
	dfa->transitions[2]['2'] = 2;
	dfa->transitions[2]['3'] = 2;
	dfa->transitions[2]['4'] = 2;
	dfa->transitions[2]['5'] = 2;
	dfa->transitions[2]['6'] = 2;
	dfa->transitions[2]['7'] = 2;
	dfa->transitions[2]['8'] = 2;
	dfa->transitions[2]['9'] = 2;
	dfa->states[2].accepting = 1;
	dfa->states[2].action = RETURN;

	dfa->states[3].accepting = 1;
	dfa->states[3].action = RETURN;
	dfa->states[4].accepting = 1;
	dfa->states[4].action = RETURN;
	dfa->states[5].accepting = 1;
	dfa->states[5].action = RETURN;
	dfa->states[6].accepting = 1;
	dfa->states[6].action = RETURN;

	dfa->nstates = 7;

	return dfa;
}

int printLexeme(int start, int end){
	int i;

	for(i = start; i < end; i++){
		if(inputString[i] == '\t'){
			printf("\\t");
		} else if(inputString[i] == '\n'){
			printf("\\n");
		} else {
			putchar(inputString[i]);
		}
	}
	printf("\n");
	return 0;
}

int tokenize(struct dfa *dfa){
	int c;
	int i,j;
	int state;
	int nextState;

	i = 0;
	while(inputString[i] != 0){
		state = 0;
		j = i;
		for(j = i; ; j++){
			c = inputString[j];
			nextState = dfa->transitions[state][c];
			if(nextState == -1){
				if(dfa->states[state].accepting){
					if(dfa->states[state].action == RETURN){
						printLexeme(i, j);
					} else if(dfa->states[state].action == SKIP){
						break;
					} else {
						fprintf(stderr, "invalid action\n");
					}
				} else {
					fprintf(stderr, "not accepting state\n");
					exit(1);
				}
				break;
			} else {
				state = nextState;
			}
		}
		i = j;
	}
	return 0;
}



int main(int argc, char *argv[]){
	struct dfa *dfa;

	strcpy(inputString, "7 + 14 * 2");
	dfa = loadDFA();
	tokenize(dfa);
	return 0;
}
