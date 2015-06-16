/*
 * Copyright 2015 Joseph Landry All Rights Reserved
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>



int setUnion(int *dst, int size, int *n, int *s1, int n1, int *s2, int n2){
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

void setFollowPos(struct ast_node *node, int leafNumber, struct ast_node *firstNode){
	int i, j, k;

	if(node->leafNumber == leafNumber){
		i = j = 0;
		while(i < node->nFollowPos || j < firstNode->nFirstPos){
			if(node->nFollowPos >= 32){
				fprintf(stderr, "bump followpos\n");
				exit(1);
			}
			if(i < node->nFollowPos && j < firstNode->nFirstPos){
				if(node->followPos[i] == firstNode->firstPos[j]){
					i += 1;
					j += 1;
				} else if(node->followPos[i] < firstNode->firstPos[j]){
					i += 1;
				} else if(node->followPos[i] > firstNode->firstPos[j]){
					for(k = node->nFollowPos; k > i; k--){
						node->followPos[k] = node->followPos[k-1];
					}
					node->nFollowPos += 1;
					node->followPos[i] = firstNode->firstPos[j];
					i += 1;
					j += 1;
				}
			} else if(i < node->nFollowPos){
				break;
			} else if(j < firstNode->nFirstPos){
				node->followPos[node->nFollowPos] = firstNode->firstPos[j];
				node->nFollowPos += 1;
				i += 1;
				j += 1;
			}
		}
	} else {
		if(node->child0){
			setFollowPos(node->child0, leafNumber, firstNode);
		}
		if(node->child1){
			setFollowPos(node->child1, leafNumber, firstNode);
		}
	}
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
		setFollowPos(child0, child0->lastPos[i], child1);
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
		setFollowPos(n->child0, n->lastPos[i], n);
	}
	return n;
}


int leafCount = 1;

struct ast_node *astLiteralNode(char c){
	struct ast_node *n = calloc(1, sizeof(struct ast_node));

	n->op = OP_LITERAL;
	n->c = c;
	n->nullable = 0;
	n->leafNumber = leafCount++;
	n->firstPos[0] = n->leafNumber;
	n->nFirstPos = 1;
	n->lastPos[0] = n->leafNumber;
	n->nLastPos = 1;
	n->nFollowPos = 0;
	return n;
}

struct ast_node *astEpsilonNode(){
	struct ast_node *n = calloc(1, sizeof(struct ast_node));

	n->op = OP_EPSILON;
	n->nullable = 1;
	n->leafNumber = leafCount++;
	n->nFirstPos = 0;
	n->nLastPos = 0;
	n->nFollowPos = 0;
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


void test_regex1(){
	struct ast_node *node;

	strcpy(inputString, "(a|b)*abb#");
	lookAheadIndex = 0;
	nextToken();
	node = orLevel();
	graphAST(node);
	//printAST(node);
}

int main(int argc, char *argv[]){
	test_regex1();
}
