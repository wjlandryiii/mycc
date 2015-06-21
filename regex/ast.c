/*
 * Copyright 2015 Joseph Landry All Rights Reserved
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>

#include "ast.h"
#include "list.h"
#include "set.h"


struct ast_node *leafIndex[128];
int leafCount = 1;


struct ast *newAST(void){
	struct ast *ast;

	ast = calloc(1, sizeof(struct ast));
	assert(ast != NULL);
	ast->leafList = newPointerList();
	assert(ast->leafList != NULL);
	return ast;
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

	return graphNodeNumber++;
}

void graphEmitEdge(int from, int to){
	printf("\tnode%d -> node%d\n",  from, to);
}

int graphEmitLeaf(char c, int nullable){
	if(isalnum(c)){
		printf("\tnode%d [label = \"%c\", "
				"shape = rectangle, style = filled, fillcolor = green]\n",
				graphNodeNumber, c);
	} else {
		printf("\tnode%d [label = \"0x%02x\", "
				"shape = rectangle, style = filled, fillcolor = green]\n",
			       	graphNodeNumber, c);
	}
	return graphNodeNumber++;
}

int graphEmitEpsilonLeaf(int nullable){
	printf("\tnode%d [label = \"\\{\\}\", "
			"shape = rectangle, style = filled, fillcolor = darkgreen]\n",
			graphNodeNumber);
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
