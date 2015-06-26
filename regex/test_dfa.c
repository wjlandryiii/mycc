/*
 * Copyright 2015 Joseph Landry All Rights Reserved
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

char *keywords[] = {
	"auto",
	"break",
	"case",
	"char",
	"const",
	"continue",
	"default",
	"do",
	"double",
	"else",
	"enum",
	"extern",
	"float",
	"for",
	"goto",
	"if",
	"int",
	"long",
	"register",
	"return",
	"short",
	"sizeof",
	"static",
	"struct",
	"switch",
	"typedef",
	"union",
	"unsigned",
	"void",
	"volatile",
	"while",
	0,
};

struct dfa_state {
	int accepting;
	int name;
	int counted;
	int visited;
	struct dfa_state *transitions[128];
};


struct dfa_state *newDFAState(){
	struct dfa_state *state;
	int i;

	state = calloc(1, sizeof(*state));
	assert(state != 0);
	state->accepting = 0;
	state->name = -1;
	for(i = 0; i < 128; i++){
		state->transitions[i] = 0;
	}
	return state;
}


void dfaInsertStringPattern(struct dfa_state *startingState, char *s, int name){
	struct dfa_state *state;
	struct dfa_state *newState;
	int i;

	state = startingState;

	for(i = 0; s[i] != 0; i++){
		if(state->transitions[s[i]] != 0){
			state = state->transitions[s[i]];
		} else {
			newState = newDFAState();
			state->transitions[s[i]] = newState;
			state = newState;
		}
	}
	state->accepting = 1;
	state->name = name;
}


void test1(){
	struct dfa_state *startingState;
	struct dfa_state *wsState;
	int i;
	int j;

	startingState = newDFAState();
	wsState = newDFAState();
	assert(startingState != 0);
	assert(wsState != 0);

	startingState->transitions[' '] = wsState;
	startingState->transitions['\t'] = wsState;
	startingState->transitions['\n'] = wsState;

	wsState->transitions[' '] = wsState;
	wsState->transitions['\t'] = wsState;
	wsState->transitions['\n'] = wsState;
	wsState->accepting = 1;
	wsState->name = -2;

	for(i = 0; keywords[i] != 0; i++){
		dfaInsertStringPattern(startingState, keywords[i], i);
	}

	for(i = 0; keywords[i] != 0; i++){
		char *keyword = keywords[i];
		struct dfa_state *state = startingState;
		struct dfa_state *nextState;

		for(int j = 0; keyword[j] != 0; j++){
			int c = keyword[j];
			nextState = state->transitions[keyword[j]];
			if(nextState == NULL){
				fprintf(stderr, "invalid state\n");
				exit(1);
			}
			state = nextState;
		}

		if(state->accepting){
			printf("name: %s\n", keywords[state->name]);
		}
	}
}


int main(int argc, char *argv[]){
	test1();
	return 0;
}
