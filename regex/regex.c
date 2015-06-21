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

void printAST(struct ast_node *node){
	int i;
	struct set_iterator si;
	int value;

	if(node->leafNumber >= 0){
		printf("%d: nullable:[%s] ", node->leafNumber, node->nullable ? "YES" : "NO");
		printf("firstpos:[ ");
		si = setIterator(node->firstpos);
		while(nextSetItem(&si, &value)){
			printf("%d ", value);
		}
		printf("] lastpos:[ ");
		si = setIterator(node->lastpos);
		while(nextSetItem(&si, &value)){
			printf("%d ", value);
		}
		printf("] followpos:[ ");
		si = setIterator(node->followpos);
		while(nextSetItem(&si, &value)){
			printf("%d ", value);
		}
		printf("]\n");
	}
	if(node->child0){
		printAST(node->child0);
	}
	if(node->child1){
		printAST(node->child1);
	}
}


struct dfa *makeDFA(struct ast_node *n){
	int allMarked;
	int i, j, k, l;
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

	dfaAddState(dfa, n->firstpos, 0);
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
					struct dfa_state *nextState = dfaAddState(dfa, u, accepting);
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
	struct ast_node *node;

	strcpy(inputString, "(a|b)*abb#");
	lookAheadIndex = 0;
	nextToken();
	node = orLevel();

	//graphAST(node);

	//printAST(node);

	struct dfa *dfa = makeDFA(node);
	//graphDFA();
	test_match(dfa, "aaaaaabb");
	test_match(dfa, "killfuck");
}

void test_regex2(){
	struct ast_node *node;

	strcpy(inputString, "(killfuck)*");
	lookAheadIndex = 0;
	nextToken();
	node = orLevel();

	//graphAST(node);

	//printAST(node);

	struct dfa *dfa = makeDFA(node);
	//graphDFA();
	//graphDFA();
	test_match(dfa, "aaaaaabb");
	test_match(dfa, "killfuck");
}


void print_set(struct set *set){
	struct set_iterator si;
	int value;

	printf("{");
	si = setIterator(set);
	while(nextSetItem(&si, &value)){
		printf(" %d", value);
	}
	printf(" }\n");
}

void test_set1(){
	struct set *set;
	int i;

	set = newSet(); print_set(set);

	setInsert(set, 2); print_set(set);
	setInsert(set, 4); print_set(set);
	setInsert(set, 6); print_set(set);
	setInsert(set, 8); print_set(set);
	setInsert(set, 10); print_set(set);

	setInsert(set, 9); print_set(set);
	setInsert(set, 7); print_set(set);
	setInsert(set, 5); print_set(set);
	setInsert(set, 3); print_set(set);
	setInsert(set, 1); print_set(set);

	printf("\n");
	
	setRemove(set, 2); print_set(set);
	setRemove(set, 4); print_set(set);
	setRemove(set, 6); print_set(set);
	setRemove(set, 8); print_set(set);
	setRemove(set, 10); print_set(set);
	setRemove(set, 9); print_set(set);
	setRemove(set, 7); print_set(set);
	setRemove(set, 5); print_set(set);
	setRemove(set, 3); print_set(set);
	setRemove(set, 1); print_set(set);
}

void test_list1(){
	struct pointer_list_iterator pli;
	struct pointer_list *list = newPointerList();
	void *data;
	struct set *set;

	pointerListAppend(list, newSetFromInteger(4));
	pointerListAppend(list, newSetFromInteger(3));
	pointerListAppend(list, newSetFromInteger(2));
	pointerListAppend(list, newSetFromInteger(1));
	pointerListAppend(list, newSetFromInteger(0));

	pli = pointerListIterator(list);
	while(pointerListIteratorNextItem(&pli, &data)){
		set = data;
		print_set(set);
	}

	freePointerList(list);
}


int main(int argc, char *argv[]){
	test_regex1();
	//test_set1();
	//test_list1();
}
