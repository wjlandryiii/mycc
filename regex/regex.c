/*
 * Copyright 2015 Joseph Landry All Rights Reserved
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>


#include "set.h"
#include "dfa.h"


struct pointer_list {
	void **items;
	int used;
	int size;
};

struct pointer_list *newPointerList(void){
	struct pointer_list *list = calloc(1, sizeof(struct pointer_list));

	list->used = 0;
	list->size = 8;
	list->items = calloc(list->size, sizeof(void *));
	return list;
}




enum OPERATIONS {
	OP_OR,
	OP_CAT,
	OP_STAR,
	OP_LITERAL,
	OP_EPSILON,
};

struct ast_node {
	int op;
	struct ast_node *child0;
	struct ast_node *child1;
	char c;
	int leafNumber;
	int nullable;
	struct set *firstpos;
	struct set *lastpos;
	struct set *followpos;
};

struct ast_node *leafIndex[128];
int leafCount = 1;


struct ast_node *astOrNode(struct ast_node *child0, struct ast_node *child1){
	struct ast_node *n = calloc(1, sizeof(struct ast_node));
	int i;

	n->op = OP_OR;
	n->child0 = child0;
	n->child1 = child1;
	n->leafNumber = -1;

	// nullable
	n->nullable = child0->nullable || child1->nullable;

	// firstpos
	n->firstpos = newSetFromUnion(child0->firstpos, child1->firstpos);
	

	// lastpos
	n->lastpos = newSetFromUnion(child0->lastpos, child1->lastpos);

	// followpos
	n->followpos = newSet();
	return n;
}

struct ast_node *astCatNode(struct ast_node *child0, struct ast_node *child1){
	struct ast_node *n = calloc(1, sizeof(struct ast_node));
	int i, j;

	n->op = OP_CAT;
	n->child0 = child0;
	n->child1 = child1;
	n->leafNumber = -1;

	// nullable
	n->nullable = child0->nullable && child1->nullable;

	// firstpos
	if(child0->nullable){	
		n->firstpos = newSetFromUnion(child0->firstpos, child1->firstpos);
	} else {
		n->firstpos = newSetFromSet(child0->firstpos);
	}
	
	// lastpos
	if(child1->nullable){
		n->lastpos = newSetFromUnion(child0->lastpos, child1->lastpos);
	} else {
		n->lastpos = newSetFromSet(child1->lastpos);
	}

	// followpos
	n->followpos = newSet();

	struct set_iterator si;
	int leafNumber;
	// YOU LEFT OFF HERE!
	si = setIterator(child0->lastpos);

	while(nextSetItem(&si, &leafNumber)){
		struct ast_node *node;

		assert(leafNumber < leafCount);
		node = leafIndex[leafNumber];

		setAddSet(node->followpos, child1->firstpos);
	}
	return n;
}

struct ast_node *astStarNode(struct ast_node *child){
	struct ast_node *n = calloc(1, sizeof(struct ast_node));
	int i;

	n->op = OP_STAR;
	n->leafNumber = -1;
	n->child0 = child;
	n->child1 = 0;

	// nullable
	n->nullable = 1;

	// firstpos
	n->firstpos = newSetFromSet(child->firstpos);	

	// lastpos
	n->lastpos = newSetFromSet(child->lastpos);

	// followpos
	n->followpos = newSet();

	struct set_iterator si;
	int leafNumber;

	si = setIterator(n->lastpos);
	while(nextSetItem(&si, &leafNumber)){
		struct ast_node *node;

		assert(leafNumber < leafCount);
		node = leafIndex[leafNumber];

		setAddSet(node->followpos, n->firstpos);
	}
	return n;
}


struct ast_node *astLiteralNode(char c){
	struct ast_node *n = calloc(1, sizeof(struct ast_node));

	assert(leafCount < 128);

	n->op = OP_LITERAL;
	n->c = c;
	n->nullable = 0;
	n->leafNumber = leafCount;
	n->firstpos = newSetFromInteger(n->leafNumber);
	n->lastpos = newSetFromInteger(n->leafNumber);
	n->followpos = newSet();

	leafIndex[leafCount++] = n;

	return n;
}

struct ast_node *astEpsilonNode(){
	struct ast_node *n = calloc(1, sizeof(struct ast_node));

	assert(leafCount < 128);

	n->op = OP_EPSILON;
	n->nullable = 1;
	n->leafNumber = leafCount;
	n->firstpos = newSet();
	n->followpos = newSet();

	leafIndex[leafCount++] = n;

	return n;
}


int graphNodeNumber;

int graphEmitNode(struct ast_node *node){
	int i;

	printf("\tnode%d ", graphNodeNumber);
	printf("[ ");
	{
		printf("style = filled");
		printf(", shape = rectangle");
		printf(", label = \"");
		if(node->op == OP_OR){
			printf("\\|\\n");
		} else if(node->op == OP_CAT){
			printf("o\\n");
		} else if(node->op == OP_STAR){
			printf("*\\n");
		} else if(node->op == OP_LITERAL){
			if(isalnum(node->c)){
				printf("%c:%d\n", node->c, node->leafNumber);
			} else if(isgraph(node->c)){
				printf("\\%c:%d\\n", node->c, node->leafNumber);
			} else {
				printf("0x%02x:%d\\n", node->c, node->leafNumber);
			}
		} else if(node->op == OP_EPSILON){
			printf("\\{\\}\\n");
		} else {
			fprintf(stderr, "graphEmitNode() invalid op: %d\n", node->op);
			exit(1);
		}

		struct set_iterator si;
		int value;
		printf("nullable: %s\\n", node->nullable ? "YES" : "NO");
		printf("firstpos: ");
		{
			printf("[ ");
			si = setIterator(node->firstpos);
			while(nextSetItem(&si, &value)){
				printf("%d ", value);
			}	
			printf("]\\n");
		}
		printf("lastpos: ");
		{
			printf("[ ");
			si = setIterator(node->lastpos);
			while(nextSetItem(&si, &value)){
				printf("%d ", value);
			}
			printf("]\\n");
		}
		printf("followpos: ");
		{
			printf("[ ");
			si = setIterator(node->followpos);
			while(nextSetItem(&si, &value)){
				printf("%d ", value);
			}
			printf("]\\n");
		}

		printf("\"");

		if(node->leafNumber >= 0){
			printf(", fillcolor = green");
		} else {
			printf(", fillcolor = white");
		}
	}
	printf("]\n");

	//printf("\tnode%d [label = \"%s\", shape = rectangle, style = filled, fillcolor = %s]\n", graphNodeNumber, label, nullable ? "gray" : "white");
	return graphNodeNumber++;
}

void graphEmitEdge(int from, int to){
	printf("\tnode%d -> node%d\n",  from, to);
}

int graphEmitLeaf(char c, int nullable){
	if(isalnum(c)){
		printf("\tnode%d [label = \"%c\", shape = rectangle, style = filled, fillcolor = green]\n", graphNodeNumber, c);
	} else {
		printf("\tnode%d [label = \"0x%02x\", shape = rectangle, style = filled, fillcolor = green]\n", graphNodeNumber, c);
	}
	return graphNodeNumber++;
}

int graphEmitEpsilonLeaf(int nullable){
	printf("\tnode%d [label = \"\\{\\}\", shape = rectangle, style = filled, fillcolor = darkgreen]\n", graphNodeNumber);
	return graphNodeNumber++;
}

int graphASTNode(struct ast_node *node){
	int n;

	if(node->op == OP_OR){
		n = graphEmitNode(node);
		graphEmitEdge(n, graphASTNode(node->child0));
		graphEmitEdge(n, graphASTNode(node->child1));
		return n;
	} else if(node->op == OP_CAT){
		n = graphEmitNode(node);
		graphEmitEdge(n, graphASTNode(node->child0));
		graphEmitEdge(n, graphASTNode(node->child1));
		return n;
	} else if(node->op == OP_STAR){
		n = graphEmitNode(node);
		graphEmitEdge(n, graphASTNode(node->child0));
		return n;
	} else if(node->op == OP_LITERAL){
		n = graphEmitNode(node);
		return n;
	} else if(node->op == OP_EPSILON){
		n = graphEmitNode(node);
		return n;
	} else {
		fprintf(stderr, "invalid operation: %d\n", node->op);
		exit(1);
	}
}

void graphAST(struct ast_node *node){
	int synth;
	graphNodeNumber = 0;
	printf("digraph G {\n");
	graphASTNode(node);
	printf("}\n");
}




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

	struct dfa *dfa = dfaNew();
	struct set *u = newSet();

	struct DState *state;
	dfaAddState(dfa, n->firstpos);
	while((state = dfaNextUnmarkedState(dfa)) != NULL){
		dfaMarkState(state);

		struct set_iterator si;
		int leafNumber;
		si = setIterator(state->list);
		while(nextSetItem(&si, &leafNumber)){
			if(leafIndex[leafNumber]->c == '#'){
				state->accepting = 1;
			}
		}

		assert(u != NULL);

		for(symbol = 0; symbol < 128; symbol++){
			clearSet(u);

			si = setIterator(state->list);
			while(nextSetItem(&si, &leafNumber)){
				if(symbol == leafIndex[leafNumber]->c){
					setAddSet(u, leafIndex[leafNumber]->followpos);
				}
			}

			if(!isSetEmtpy(u)){
				struct DState *nextState = dfaFindStateWithSet(dfa, u);

				if(nextState){
					dfaAddTransition(dfa, state, symbol, nextState);
				} else {
					int nextStateIndex = dfaAddState(dfa, u);
					dfaAddTransition(dfa, state, symbol, &dfa->states[nextStateIndex]);
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
		state = dfa->transitions[state][s[i]];
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


int main(int argc, char *argv[]){
	test_regex1();
	//test_set1();
}
