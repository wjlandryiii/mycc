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
int leafCount = 0;


struct ast *newAST(void){
	struct ast *ast;

	ast = calloc(1, sizeof(struct ast));
	assert(ast != NULL);
	ast->root = NULL;
	ast->leafNodeList = newPointerList();
	assert(ast->leafNodeList != NULL);
	return ast;
}



struct ast_node *astOrNode(struct ast_node *child0, struct ast_node *child1){
	struct ast_node *n = calloc(1, sizeof(struct ast_node));
	int i;

	n->op = OP_OR;
	n->child0 = child0;
	n->child1 = child1;
	n->leafNumber = -1;
	return n;
}

struct ast_node *astCatNode(struct ast_node *child0, struct ast_node *child1){
	struct ast_node *n = calloc(1, sizeof(struct ast_node));
	int i, j;

	n->op = OP_CAT;
	n->child0 = child0;
	n->child1 = child1;
	n->leafNumber = -1;
	return n;
}

struct ast_node *astStarNode(struct ast_node *child){
	struct ast_node *n = calloc(1, sizeof(struct ast_node));
	int i;

	n->op = OP_STAR;
	n->leafNumber = -1;
	n->child0 = child;
	n->child1 = 0;
	return n;
}


struct ast_node *astLiteralNode(char c){
	struct ast_node *n = calloc(1, sizeof(struct ast_node));

	assert(leafCount < 128);

	n->op = OP_LITERAL;
	n->c = c;
	n->nullable = 0;
	n->leafNumber = leafCount;

	leafIndex[leafCount++] = n;

	return n;
}

struct ast_node *astEpsilonNode(){
	struct ast_node *n = calloc(1, sizeof(struct ast_node));

	assert(leafCount < 128);

	n->op = OP_EPSILON;
	n->nullable = 1;
	n->leafNumber = leafCount;

	leafIndex[leafCount++] = n;

	return n;
}

int astLabelLeafNodes(struct ast *ast, struct ast_node *node){
	if(node->op == OP_OR){
		astLabelLeafNodes(ast, node->child0);
		astLabelLeafNodes(ast, node->child1);
		return 0;
	} else if(node->op == OP_CAT){
		astLabelLeafNodes(ast, node->child0);
		astLabelLeafNodes(ast, node->child1);
		return 0;
	} else if(node->op == OP_STAR){
		astLabelLeafNodes(ast, node->child0);
		return 0;
	} else if(node->op == OP_LITERAL){
		node->leafNumber = pointerListAppend(ast->leafNodeList, node);
		return 0;
	} else if(node->op == OP_EPSILON){
		return 0;
	} else {
		fprintf(stderr, "invalid op in  nullable()\n");
		exit(1);
	}
}

int astNullableAttribute(struct ast *ast, struct ast_node *node){
	if(node->op == OP_OR){
		astNullableAttribute(ast, node->child0);
		astNullableAttribute(ast, node->child1);
		node->nullable = node->child0->nullable || node->child1->nullable;
		return 0;
	} else if(node->op == OP_CAT){
		astNullableAttribute(ast, node->child0);
		astNullableAttribute(ast, node->child1);
		node->nullable = node->child0->nullable && node->child1->nullable;
		return 0;
	} else if(node->op == OP_STAR){
		astNullableAttribute(ast, node->child0);
		node->nullable = 1;
		return 0;
	} else if(node->op == OP_LITERAL){
		node->nullable = 0;
		return 0;
	} else if(node->op == OP_EPSILON){
		node->nullable = 1;
		return 0;
	} else {
		fprintf(stderr, "invalid op in  nullable()\n");
		exit(1);
	}
}

int astFirstposAttribute(struct ast *ast, struct ast_node *node){
	if(node->op == OP_OR){
		astFirstposAttribute(ast, node->child0);
		astFirstposAttribute(ast, node->child1);
		node->firstpos = newSetFromUnion(node->child0->firstpos, node->child1->firstpos);
		return 0;
	} else if(node->op == OP_CAT){
		astFirstposAttribute(ast, node->child0);
		astFirstposAttribute(ast, node->child1);
		if(node->child0->nullable){
			node->firstpos = newSetFromUnion(
					node->child0->firstpos,
					node->child1->firstpos);
		} else {
			node->firstpos = newSetFromSet(node->child0->firstpos);
		}
		return 0;
	} else if(node->op == OP_STAR){
		astFirstposAttribute(ast, node->child0);
		node->firstpos = newSetFromSet(node->child0->firstpos);
		return 0;
	} else if(node->op == OP_LITERAL){
		node->firstpos = newSetFromInteger(node->leafNumber);
		return 0;
	} else if(node->op == OP_EPSILON){
		node->firstpos = newSetFromInteger(node->leafNumber);
		// TODO: wtf here?
		return 0;
	} else {
		fprintf(stderr, "invalid op in  nullable()\n");
		exit(1);
	}
}



int astLastposAttribute(struct ast *ast, struct ast_node *node){
	if(node->op == OP_OR){
		astLastposAttribute(ast, node->child0);
		astLastposAttribute(ast, node->child1);

		node->lastpos = newSetFromUnion(node->child0->lastpos, node->child1->lastpos);
		return 0;
	} else if(node->op == OP_CAT){
		astLastposAttribute(ast, node->child0);
		astLastposAttribute(ast, node->child1);

		if(node->child1->nullable){
			node->lastpos = newSetFromUnion(node->child0->lastpos,
					node->child1->lastpos);
		} else {
			node->lastpos = newSetFromSet(node->child1->lastpos);
		}
		return 0;
	} else if(node->op == OP_STAR){
		astLastposAttribute(ast, node->child0);
		node->lastpos = newSetFromSet(node->child0->lastpos);
		return 0;
	} else if(node->op == OP_LITERAL){
		node->lastpos = newSetFromInteger(node->leafNumber);
		return 0;
	} else if(node->op == OP_EPSILON){
		// TODO: wtf here?
		node->lastpos = newSet();
		return 0;
	} else {
		fprintf(stderr, "invalid op in  nullable()\n");
		exit(1);
	}
}

int astFollowposAttribute(struct ast *ast, struct ast_node *node){
	if(node->op == OP_OR){
		astFollowposAttribute(ast, node->child0);
		astFollowposAttribute(ast, node->child1);

		node->followpos = newSet();
		return 0;
	} else if(node->op == OP_CAT){
		astFollowposAttribute(ast, node->child0);
		astFollowposAttribute(ast, node->child1);

		node->followpos = newSet();
		struct set_iterator si;
		int leafNumber;
		si = setIterator(node->child0->lastpos);
		while(nextSetItem(&si, &leafNumber)){
			struct ast_node *leafNode;
			assert(leafNumber < leafCount);
			leafNode = leafIndex[leafNumber];
			setAddSet(leafNode->followpos, node->child1->firstpos);
		}
		return 0;
	} else if(node->op == OP_STAR){
		astFollowposAttribute(ast, node->child0);

		node->followpos = newSet();
		struct set_iterator si;
		int leafNumber;

		si = setIterator(node->lastpos);
		while(nextSetItem(&si, &leafNumber)){
			struct ast_node *leafNode;
			assert(leafNumber < leafCount);
			leafNode = leafIndex[leafNumber];
			setAddSet(leafNode->followpos, node->firstpos);
		}
		return 0;
	} else if(node->op == OP_LITERAL){
		node->followpos = newSet();
		return 0;
	} else if(node->op == OP_EPSILON){
		node->followpos = newSet();
		return 0;
	} else {
		fprintf(stderr, "unknown op: %d\n", node->op);
		exit(1);
	}
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



int annotateAST(struct ast *ast){
	astLabelLeafNodes(ast, ast->root);
	astNullableAttribute(ast, ast->root);
	astFirstposAttribute(ast, ast->root);
	astLastposAttribute(ast, ast->root);
	astFollowposAttribute(ast, ast->root);
	return 0;
}
