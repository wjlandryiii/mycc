/*
 * Copyright 2015 Joseph Landry All Rights Reserved
 */

#ifndef AST_H
#define AST_H


extern struct ast_node *leafIndex[128];
extern int leafCount;


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

struct ast {
	struct ast_node *root;
	struct pointer_list *leafList;
};

struct ast *newAST(void);
struct ast_node *astOrNode(struct ast_node *child0, struct ast_node *child1);
struct ast_node *astCatNode(struct ast_node *child0, struct ast_node *child1);
struct ast_node *astStarNode(struct ast_node *child);
struct ast_node *astLiteralNode(char c);
struct ast_node *astEpsilonNode();

#endif
