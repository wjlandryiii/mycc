/*
 * Copyright 2015 Joseph Landry
 */

enum OPS {
	OP_UNION,
	OP_CONCAT,
	OP_STAR,
	OP_LITERAL,
};

struct ast_node {
	int op;
	struct ast_node *child1;
	struct ast_node *child2;
};

