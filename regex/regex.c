/*
 * Copyright 2015 Joseph Landry All Rights Reserved
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

enum OPERATIONS {
	OP_OR,
	OP_CAT,
	OP_STAR,
	OP_LITERAL,
};

struct ast_node {
	int op;
	struct ast_node *child0;
	struct ast_node *child1;
	char c;
};

struct ast_node *astOrNode(struct ast_node *child0, struct ast_node *child1){
	struct ast_node *n = calloc(1, sizeof(struct ast_node));

	n->op = OP_OR;
	n->child0 = child0;
	n->child1 = child1;
	return n;
}

struct ast_node *astCatNode(struct ast_node *child0, struct ast_node *child1){
	struct ast_node *n = calloc(1, sizeof(struct ast_node));

	n->op = OP_CAT;
	n->child0 = child0;
	n->child1 = child1;
	return n;
}

struct ast_node *astStarNode(struct ast_node *child){
	struct ast_node *n = calloc(1, sizeof(struct ast_node));

	n->op = OP_STAR;
	n->child0 = child;
	return n;
}

struct ast_node *astLiteralNode(char c){
	struct ast_node *n = calloc(1, sizeof(struct ast_node));

	n->op = OP_LITERAL;
	n->c = c;
	return n;
}


int graphNodeNumber;

int graphEmitNode(char *label){
	printf("\tnode%d [label = \"%s\", shape = circle]\n", graphNodeNumber, label);
	return graphNodeNumber++;
}

void graphEmitEdge(int from, int to){
	printf("\tnode%d -> node%d\n",  from, to);
}

int graphEmitLeaf(char c){
	if(isalnum(c)){
		printf("\tnode%d [label = \"%c\", shape = circle, style = filled, fillcolor = green]\n", graphNodeNumber, c);
	} else {
		printf("\tnode%d [label = \"0x%02x\", shape = circle, style = filled, fillcolor = green]\n", graphNodeNumber, c);
	}
	return graphNodeNumber++;
}

int graphASTNode(struct ast_node *node){
	int n;

	if(node->op == OP_OR){
		n = graphEmitNode("\\|");
		graphEmitEdge(n, graphASTNode(node->child0));
		graphEmitEdge(n, graphASTNode(node->child1));
		return n;
	} else if(node->op == OP_CAT){
		n = graphEmitNode("o");
		graphEmitEdge(n, graphASTNode(node->child0));
		graphEmitEdge(n, graphASTNode(node->child1));
		return n;
	} else if(node->op == OP_STAR){
		n = graphEmitNode("*");
		graphEmitEdge(n, graphASTNode(node->child0));
		return n;
	} else if(node->op == OP_LITERAL){
		n = graphEmitLeaf(node->c);
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
	while(lookAhead != '|' && lookAhead != '\0'){
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
	struct ast_node *n = astLiteralNode(lookAhead);
	nextToken();
	return n;
}


void test_regex1(){
	struct ast_node *node;

	strcpy(inputString, "aa*bb|ccdd");
	lookAheadIndex = 0;
	nextToken();
	node = orLevel();
	graphAST(node);
}

int main(int argc, char *argv[]){
	test_regex1();
}
