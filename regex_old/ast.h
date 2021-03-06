/*
 * Copyright 2015 Joseph Landry All Rights Reserved
 */

#ifndef AST_H
#define AST_H

struct ast_node;
extern struct ast_node *AST;

enum OPS {
	OP_INVALID,
	OP_UNION,
	OP_CONCAT,
	OP_STAR,
	OP_PLUS,
	OP_QUESTIONMARK,
	OP_METACHAR,
	OP_LITERAL,
	OP_ANY,
	OP_EOS,
};

struct ast_node {
	int op;
	int value;
	struct ast_node *child1;
	struct ast_node *child2;
};


struct ast_node *astBinaryNode(int op, struct ast_node *child1, struct ast_node *child2);
struct ast_node *astUnaryNode(int op, struct ast_node *child1);
struct ast_node *astLeafNode(int op, int value);


int computeAST(void);
void graphAST(struct ast_node *node);

#endif
