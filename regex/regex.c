/*
 * Copyright 2015 Joseph Landry All Rights Reserved
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>


static int insertSorted(int *list, const int size, const int n, const int value){
	int i;

	for(i = n; i > 0; i--){
		if(list[i-1] < value){
			list[i] = value;
			break;
		} else {
			list[i] = list[i-1];
		}
	}
	if(i == 0){
		list[i] = value;
	}
	return 0;
}

static int removeSorted(int *list, const int size, const int n, const int value){
	int i;

	for(i = 0; i < n; i++){
		if(list[i] == value){
			break;
		}
	}
	for(; i < n-1; i++){
		list[i] = list[i+1];
	}
	return 0;
}



struct set {
	int *items;
	int used;
	int size;
};


struct set *newSet(void){
	struct set *set;

	set = calloc(1, sizeof(struct set));
	assert(set != NULL);
	set->used = 0;
	set->size = 4;
	set->items = calloc(set->size, sizeof(int));
	assert(set->items != NULL);
	return set;
}

struct set *newSetFromSet(struct set *s1){
	struct set *set;

	set = calloc(1, sizeof(struct set));
	assert(set != NULL);
	set->used = s1->used;
	set->size = s1->size;
	set->items = calloc(set->size, sizeof(int));
	assert(set->items != NULL);
	memcpy(set->items, s1->items, sizeof(int)*set->used);
	return set;
}


void freeSet(struct set *set){
	free(set->items);
	set->items = 0;
	free(set);
}


int setBoostSize(struct set *set){
	int amount = 4;
	set->items = realloc(set->items, set->size + amount * sizeof(int));
	assert(set->items != NULL);
	set->size += amount;
	printf("bump!\n");
	return 0;
}


int setInsert(struct set *set, int value){
	int i;

	for(i = 0; i < set->used && set->items[i] != value; i++);
	if(i == set->used){
		if(set->used + 1 <= set->size){
			insertSorted(set->items, set->size, set->used, value);
			set->used += 1;
		} else {
			setBoostSize(set);
			insertSorted(set->items, set->size, set->used, value);
			set->used += 1;
		}
	} else {

	}
	return 0;
}

int setRemove(struct set *set, int value){
	int i;

	for(i = 0; i < set->used && set->items[i] != value; i++);
	if(i < set->used){
		removeSorted(set->items, set->size, set->used, value);
		set->used -= 1;
	}
	return 0;
}

int setAddSet(struct set *set, struct set *s1){
	int i;

	for(i = 0; i < s1->used; i++){
		setInsert(set, s1->items[i]);
	}
	return 0;
}

struct set *newSetFromUnion(struct set *s1, struct set *s2){
	struct set *set;

	set = newSet();
	assert(set != NULL);
	setAddSet(set, s1);
	setAddSet(set, s2);
	return set;
}

struct set *newSetFromInteger(int value){
	struct set *set;

	set = newSet();
	setInsert(set, value);
	return set;
}

int areSetsEqual(struct set *s1, struct set *s2){
	int i;

	if(s1->used != s2->used){
		return 0;
	}

	for(i = 0; i< s1->used; i++){
		if(s1->items[i] != s2->items[i]){
			return 0;
		}
	}
	return 1;
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
	for(i = 0; i < child0->lastpos->used; i++){
		int leafNumber = child0->lastpos->items[i];
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
	for(i = 0; i < n->lastpos->used; i++){
		int leafNumber = n->lastpos->items[i];
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

		printf("nullable: %s\\n", node->nullable ? "YES" : "NO");
		printf("firstpos: ");
		{
			printf("[ ");
			for(i = 0; i < node->firstpos->used; i++){
				printf("%d ", node->firstpos->items[i]);
			}
			printf("]\\n");
		}
		printf("lastpos: ");
		{
			printf("[ ");
			for(i = 0; i < node->lastpos->used; i++){
				printf("%d ", node->lastpos->items[i]);
			}
			printf("]\\n");
		}
		printf("followpos: ");
		{
			printf("[ ");
			for(i = 0; i < node->followpos->used; i++){
				printf("%d ", node->followpos->items[i]);
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

	if(node->leafNumber >= 0){
		printf("%d: nullable:[%s] ", node->leafNumber, node->nullable ? "YES" : "NO");
		printf("firstpos:[ ");
		for(i = 0; i < node->firstpos->used; i++){
			printf("%d ", node->firstpos->items[i]);
		}
		printf("] lastpos:[ ");
		for(i = 0; i < node->lastpos->used; i++){
			printf("%d ", node->lastpos->items[i]);
		}
		printf("] followpos:[ ");
		for(i = 0; i < node->followpos->used; i++){
			printf("%d ", node->followpos->items[i]);
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

struct DState {
	int marked;
	// int list[32];
	// int nList;
	struct set *list;
	int accepting;
};

struct dfa {
	int nstates;
	struct DState states[128];
	int transitions[128][128];
};

struct dfa *dfaNew(){
	int i, j;
	struct dfa *dfa = calloc(1, sizeof(struct dfa));

	for(i = 0; i < 128; i++){
		for(j = 0; j < 128; j++){
			dfa->transitions[i][j] = -1;
		}
	}
	return dfa;
}


int dfaAddState(struct dfa *dfa, struct set *set){
	int stateIndex;
	struct DState *state;
	int i;

	assert(dfa->nstates < 128);

	stateIndex = dfa->nstates++;
	state = &dfa->states[stateIndex];

	state->marked = 0;
	state->accepting = 0;
	// state->nList = n;
	// for(i = 0; i < n; i++){
	// 	state->list[i] = s[i];
	// }
	state->list = newSetFromSet(set);
	return stateIndex;
}

int dfaMarkState(struct DState *state){
	state->marked = 1;
	return 0;
}

struct DState *dfaNextUnmarkedState(struct dfa *dfa){
	int i;

	for(i = 0; i < dfa->nstates; i++){
		if(!dfa->states[i].marked){
			return &dfa->states[i];
		}
	}
	return NULL;
}

struct DState *dfaFindStateWithSet(struct dfa *dfa, struct set *set){
	int i, j;

	for(i = 0; i < dfa->nstates; i++){
		if(areSetsEqual(dfa->states[i].list, set)){
			return &dfa->states[i];
		}
	}
	return NULL;
}

int dfaAddTransition(struct dfa *dfa, struct DState *state, int symbol, struct DState *nextState){
	// TODO: check this pointer math.. Seems to work
	int stateIndex = state - dfa->states;
	int nextStateIndex = nextState - dfa->states;
	dfa->transitions[stateIndex][symbol] = nextStateIndex;
	return 0;
}

struct dfa *makeDFA(struct ast_node *n){
	int allMarked;
	int i, j, k, l;
	int symbol;

	struct dfa *dfa = dfaNew();

	struct DState *state;
	dfaAddState(dfa, n->firstpos);
	while((state = dfaNextUnmarkedState(dfa)) != NULL){
		dfaMarkState(state);

		for(j = 0; j < state->list->used; j++){
			if(leafIndex[state->list->items[j]]->c == '#'){
				state->accepting = 1;
			}
		}

		for(symbol = 0; symbol < 128; symbol++){
			struct set *u = newSet();

			for(k = 0; k < state->list->used; k++){
				int leaf = state->list->items[k];
				if(symbol == leafIndex[leaf]->c){
					setAddSet(u, leafIndex[leaf]->followpos);
				}
			}

			if(u->used > 0){
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
	return dfa;
}

void graphDFA(struct dfa *dfa){
	int i, j;

	printf("digraph G {\n");
	printf("\trankdir = LR\n");

	for(i = 0; i < dfa->nstates; i++){
		printf("\tnode%d [label=\"[", i);
		for(j = 0; j < dfa->states[i].list->used; j++){
			printf(" %d", dfa->states[i].list->items[j]);
		}
		printf(" ]\"");
		if(dfa->states[i].accepting){
			printf(", shape=doublecircle");
		} else {
			printf(", shape=circle");
		}
		printf("]\n");
	}

	for(i = 0; i < dfa->nstates; i++){
		for(j = 0; j < 128; j++){
			if(dfa->transitions[i][j] >= 0){
				printf("\tnode%d -> node%d [label=\"%c\"]\n", i, dfa->transitions[i][j], j);
			}
		}
	}

	printf("}\n");


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
	int i;

	printf("{");
	for(i = 0; i < set->used; i++){
		printf(" %d", set->items[i]);
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
