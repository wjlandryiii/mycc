/*
 * Copyright 2015 Joseph Landry All Rights Reserved
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>


int setUnion(int *dst, const int size, int *n, const int *s1, const int n1, const int *s2, const int n2){
	// Merge algorithm
	int i, j;
	i = j = 0;
	*n = 0;
	while(i < n1 || j < n2){
		if(*n >= size){
			fprintf(stderr, "bump set size.\n");
			exit(1);
		}
		if(i < n1 && j < n2){
			if(s1[i] == s2[j]){
				dst[*n] = s1[i];
				*n += 1;
				i += 1;
				j += 1;
			} else if(s1[i] < s2[j]){
				dst[*n] = s1[i];
				*n += 1;
				i += 1;
			} else if(s1[i] > s2[j]){
				dst[*n] = s2[j];
				*n += 1;
				j += 1;
			}
		} else if(i < n1){
			dst[*n] = s1[i];
			*n += 1;
			i += 1;
		} else if(j < n2){
			dst[*n] = s2[j];
			*n += 1;
			j += 1;
		}
	}
	return 0;
}

int setUnionInplace(int *dst, const int size, int *n, const int *s1, const int n1){
	int i, j, k;

	i = j = 0;


	while(i < *n || j < n1){
		if(*n >= size){
			fprintf(stderr, "bump set size\n");
			exit(1);
		}
		if(i < *n && j < n1){
			if(dst[i] == s1[j]){
				i += 1;
				j += 1;
			} else if(dst[i] < s1[j]){
				i += 1;
			} else if(dst[i] > s1[j]){
				for(k = *n; k > i; k--){
					dst[k] = dst[k-1];
				}
				*n += 1;
				dst[i] = s1[j];
				i += 1;
				j += 1;
			}
		} else if(i < *n){
			break;
		} else if(j < n1){
			dst[i] = s1[j];
			*n += 1;
			i += 1;
			j += 1;
		}
	}
	return 0;
}


int setIsEqual(const int *s1, const int n1, const int *s2, const int n2){
	int i;

	if(n1 != n2){
		return 0;
	}

	for(i = 0; i < n1; i++){
		if(s1[i] != s2[i]){
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
	int firstPos[32];
	int nFirstPos;
	int lastPos[32];
	int nLastPos;
	int followPos[32];
	int nFollowPos;
};

struct ast_node *leafIndex[128];
int leafCount = 1;

void setFollowPos(int leafNumber, struct ast_node *firstNode){
	int i, j, k;
	struct ast_node *node;

	i = j = 0;

	node = leafIndex[leafNumber];
	assert(leafNumber < leafCount);

	setUnionInplace(node->followPos, 32, &node->nFollowPos, firstNode->firstPos, firstNode->nFirstPos);
}


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
	n->nFirstPos = 0;
	setUnion(n->firstPos, 32, &n->nFirstPos,
			child0->firstPos, child0->nFirstPos,
			child1->firstPos, child1->nFirstPos);

	// lastpos
	n->nLastPos = 0;
	setUnion(n->lastPos, 32, &n->nLastPos,
			child0->lastPos, child0->nLastPos,
			child1->lastPos, child1->nLastPos);
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
	n->nFirstPos = 0;
	if(child0->nullable){
		setUnion(n->firstPos, 32, &n->nFirstPos,
				child0->firstPos, child0->nFirstPos,
				child1->firstPos, child1->nFirstPos);
	} else {
		for(i = 0; i < child0->nFirstPos; i++){
			n->firstPos[i] = child0->firstPos[i];
		}
		n->nFirstPos = child0->nFirstPos;
	}

	// lastpos
	n->nLastPos = 0;
	if(child1->nullable){
		setUnion(n->lastPos, 32, &n->nLastPos,
				child0->lastPos, child0->nLastPos,
				child1->lastPos, child1->nLastPos);
	} else {
		for(i = 0; i < child1->nLastPos; i++){
			n->lastPos[i] = child1->lastPos[i];
		}
		n->nLastPos = child1->nLastPos;
	}

	// followpos
	for(i = 0; i < child0->nLastPos; i++){
		setFollowPos(child0->lastPos[i], child1);
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
	for(i = 0; i < child->nFirstPos; i++){
		n->firstPos[i] = child->firstPos[i];
	}
	n->nFirstPos = child->nFirstPos;

	// lastpos
	for(i = 0; i < child->nLastPos; i++){
		n->lastPos[i] = child->lastPos[i];
	}
	n->nLastPos = child->nLastPos;

	// followpos
	n->nFollowPos = 0;
	for(i = 0; i < n->nLastPos; i++){
		setFollowPos(n->lastPos[i], n);
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
	n->firstPos[0] = n->leafNumber;
	n->nFirstPos = 1;
	n->lastPos[0] = n->leafNumber;
	n->nLastPos = 1;
	n->nFollowPos = 0;

	leafIndex[leafCount++] = n;

	return n;
}

struct ast_node *astEpsilonNode(){
	struct ast_node *n = calloc(1, sizeof(struct ast_node));

	assert(leafCount < 128);

	n->op = OP_EPSILON;
	n->nullable = 1;
	n->leafNumber = leafCount;
	n->nFirstPos = 0;
	n->nLastPos = 0;
	n->nFollowPos = 0;

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
			for(i = 0; i < node->nFirstPos; i++){
				printf("%d ", node->firstPos[i]);
			}
			printf("]\\n");
		}
		printf("lastpos: ");
		{
			printf("[ ");
			for(i = 0; i < node->nLastPos; i++){
				printf("%d ", node->lastPos[i]);
			}
			printf("]\\n");
		}
		printf("followpos: ");
		{
			printf("[ ");
			for(i = 0; i < node->nFollowPos; i++){
				printf("%d ", node->followPos[i]);
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
		for(i = 0; i < node->nFirstPos; i++){
			printf("%d ", node->firstPos[i]);
		}
		printf("] lastpos:[ ");
		for(i = 0; i < node->nLastPos; i++){
			printf("%d ", node->lastPos[i]);
		}
		printf("] followpos:[ ");
		for(i = 0; i < node->nFollowPos; i++){
			printf("%d ", node->followPos[i]);
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
	int list[32];
	int nList;
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


int dfaAddState(struct dfa *dfa, int *s, int n){
	int stateIndex;
	struct DState *state;
	int i;

	assert(dfa->nstates < 128);
	assert(n < sizeof(dfa->states) / sizeof(dfa->states[0]));

	stateIndex = dfa->nstates++;
	state = &dfa->states[stateIndex];

	state->marked = 0;
	state->accepting = 0;
	state->nList = n;
	for(i = 0; i < n; i++){
		state->list[i] = s[i];
	}
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

struct DState *dfaFindStateWithSet(struct dfa *dfa, int *s, int n){
	int i, j;

	for(i = 0; i < dfa->nstates; i++){
		if(setIsEqual(dfa->states[i].list, dfa->states[i].nList, s, n)){
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
	dfaAddState(dfa, n->firstPos, n->nFirstPos);
	while((state = dfaNextUnmarkedState(dfa)) != NULL){
		dfaMarkState(state);

		for(j = 0; j < state->nList; j++){
			if(leafIndex[state->list[j]]->c == '#'){
				state->accepting = 1;
			}
		}

		for(symbol = 0; symbol < 128; symbol++){
			int u[32];
			int nu = 0;

			for(k = 0; k < state->nList; k++){
				int leaf = state->list[k];
				if(symbol == leafIndex[leaf]->c){
					setUnionInplace(u, 32, &nu, leafIndex[leaf]->followPos, leafIndex[leaf]->nFollowPos);
				}
			}

			if(nu > 0){
				struct DState *nextState = dfaFindStateWithSet(dfa, u, nu);

				if(nextState){
					dfaAddTransition(dfa, state, symbol, nextState);
				} else {
					int nextStateIndex = dfaAddState(dfa, u, nu);
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
		for(j = 0; j < dfa->states[i].nList; j++){
			printf(" %d", dfa->states[i].list[j]);
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

int main(int argc, char *argv[]){
	test_regex1();
}
