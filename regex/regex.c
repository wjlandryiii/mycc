/*
 * Copyright 2015 Joseph Landry All Rights Reserved
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

#include "set.h"
#include "list.h"
#include "dfa.h"
#include "ast.h"


char inputString[1024];
int lookAheadIndex;
char lookAhead;

void nextToken(){
	lookAhead = inputString[lookAheadIndex++];
}

struct ast_node *orLevel();
struct ast_node *catLevel();
struct ast_node *starLevel();
struct ast_node *atomLevel();

struct ast_node *orLevel(){
	struct ast_node *synth;

	synth = catLevel();
	while(lookAhead == '|'){
		nextToken();
		synth = astOrNode(synth, catLevel());
	}
	return synth;
}

struct ast_node *catLevel(){
	struct ast_node *synth;
	struct ast_node *catNode;

	synth = starLevel();
	while(lookAhead != '|' && lookAhead != ')' && lookAhead != '\0'){
		synth = astCatNode(synth, starLevel());
	}
	return synth;
}

struct ast_node *starLevel(){
	struct ast_node *synth;

	synth = atomLevel();
	if(lookAhead == '*'){
		nextToken();
		synth = astStarNode(synth);
	}
	return synth;
}

struct ast_node *atomLevel(){
	struct ast_node *n;

	if(lookAhead != '('){
		n = astLiteralNode(lookAhead);
		nextToken();
	} else if(lookAhead == '('){
		nextToken();
		n = orLevel();
		if(lookAhead != ')'){
			fprintf(stderr, "Expected ')'\n");
			exit(1);
		}
		nextToken();
	}
	return n;
}


struct dfa *makeDFA(struct ast *ast){
	int allMarked;
	int symbol;

	struct dfa *dfa;
	struct set *u;

	struct dfa_state *state;
	struct set_iterator si;
	int leafNumber;

	dfa = dfaNew();
	assert(dfa != NULL);
	u = newSet();
	assert(u != NULL);

	dfaAddState(dfa, ast->root->firstpos, 0);
	while((state = dfaNextUnmarkedState(dfa)) != NULL){
		dfaMarkState(state);

		for(symbol = 0; symbol < 128; symbol++){
			clearSet(u);

			si = setIterator(state->list);
			while(nextSetItem(&si, &leafNumber)){
				if(symbol == leafIndex[leafNumber]->c){
					setAddSet(u, leafIndex[leafNumber]->followpos);
				}
			}

			if(!isSetEmtpy(u)){
				struct dfa_state *nextState = dfaFindStateWithSet(dfa, u);
				struct set_iterator si;

				if(nextState){
					dfaAddTransition(dfa, state, symbol, nextState);
				} else {
					int accepting = 0;
					si = setIterator(u);
					while(nextSetItem(&si, &leafNumber)){
						if(leafIndex[leafNumber]->c == '#'){
							accepting = 1;
						}
					}
					struct dfa_state *nextState;
					nextState = dfaAddState(dfa, u, accepting);
					dfaAddTransition(dfa, state, symbol, nextState);
				}
			}
		}
	}
	freeSet(u);
	return dfa;
}


int match(struct dfa *dfa, char *s){
	int i;
	int state;

	state = 0;
	for(i = 0; s[i] != '\0'; i++){

		state = dfaQueryTransition(dfa, state, s[i]);
		if(state < 0){
			return 0;
		}
	}
	return 1;
}

void test_match(struct dfa *dfa, char *s){
	printf("trying to match: %s\n", s);
	if(match(dfa, s)){
		printf("MATCHED!\n");
	} else {
		printf("DID NOT MATCH!\n");
	}
}

void test_regex1(){
	struct ast *ast;

	strcpy(inputString, "(a|b)*abb#");
	lookAheadIndex = 0;
	nextToken();
	ast = newAST();
	ast->root = orLevel();

	//astLabelLeafs(ast, ast->root);
	annotateAST(ast);
	//graphAST(node);

	//printAST(node);

	struct dfa *dfa = makeDFA(ast);
	//graphDFA();
	test_match(dfa, "aaaaaabb");
	test_match(dfa, "killfuck");
}




int main(int argc, char *argv[]){
	test_regex1();
	return 0;
}
