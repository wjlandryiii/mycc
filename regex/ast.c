/*
 * Copyright 2015 Joseph Landry All Rights Reserved
 */

#include <stdio.h>
#include <stdlib.h>

#include "parser.h"
#include "ast.h"

struct ast_node *AST;

static struct ast_node *astBinaryNode(int op, struct ast_node *child1, struct ast_node *child2){
	struct ast_node *node = calloc(1, sizeof(struct ast_node));
	node->op = op;
	node->child1 = child1;
	node->child2 = child2;
	return node;
}

static struct ast_node *astUnaryNode(int op, struct ast_node *child1){
	struct ast_node *node = calloc(1, sizeof(struct ast_node));
	node->op = op;
	node->child1 = child1;
	return node;
}

static struct ast_node *astLeafNode(int op, int value){
	struct ast_node *node = calloc(1, sizeof(struct ast_node));
	node->op = op;
	node->value = value;
	return node;
}

static void notImplemented(char *s, int rule){
	fprintf(stderr, "notImplemented <%s> rule: %d\n", s, rule);
	exit(1);
}

static void invalidRule(char *s, int rule){
	fprintf(stderr, "invalid <%s> rule: %d\n", s, rule);
	exit(1);
}

static void ast_re(struct re *re, struct ast_node **synth);
static void ast_union(struct union_ *union_, struct ast_node *inherit, struct ast_node **synth);
static void ast_simple_re(struct simple_re *simple_re, struct ast_node **synth);
static void ast_concatenation(struct concatenation *concatenation,
		struct ast_node *inherit,
		struct ast_node **synth);
static void ast_basic_re(struct basic_re *basic_re, struct ast_node **synth);
static void ast_repeat(
		struct repeat *item,
		struct ast_node *inherit,
		struct ast_node **synth);
static void ast_elementary_re(struct elementary_re *item, struct ast_node **synth);
static void ast_character(struct character *item, struct ast_node **synth);
static void ast_nonmetachar(struct nonmetachar *item, struct ast_node **synth);


static void ast_re(struct re *re, struct ast_node **synth){
	struct ast_node *simple_re_out;
	struct ast_node *union_out;

	if(re->rule == 1){
		ast_simple_re(re->simple_re, &simple_re_out);
		ast_union(re->union_, simple_re_out, &union_out);
		*synth = union_out;
	} else {
		invalidRule("re", re->rule);
	}	
}

static void ast_union(struct union_ *union_, struct ast_node *inherit, struct ast_node **synth){
	int rule = union_->rule;

	struct ast_node *simple_re_synth;

	struct ast_node *union_node;


	if(rule == 1){
		ast_simple_re(union_->simple_re, &simple_re_synth);
		ast_union(union_->union_, simple_re_synth, &union_node);
		*synth = astBinaryNode(OP_UNION, inherit, union_node);
	} else if(rule == 2){
		*synth = inherit;
	} else {
		invalidRule("union", rule);
	}
}

static void ast_simple_re(struct simple_re *simple_re, struct ast_node **synth){
	int rule = simple_re->rule;

	struct ast_node *basic_re_out;
	struct ast_node *concatenation_out;

	if(rule == 1){
		ast_basic_re(simple_re->basic_re, &basic_re_out);
		ast_concatenation(simple_re->concatenation, basic_re_out, &concatenation_out);
		*synth = concatenation_out;
	} else {
		invalidRule("simple_re", rule);
	}
}

static void ast_concatenation(struct concatenation *concatenation,
		struct ast_node *inherit,
		struct ast_node **synth){
	int rule = concatenation->rule;

	struct ast_node *basic_re_out;
	struct ast_node *concatenation_out;

	if(rule == 1){
		ast_basic_re(concatenation->basic_re, &basic_re_out);
		ast_concatenation(concatenation->concatenation, basic_re_out, &concatenation_out);
		*synth = astBinaryNode(OP_CONCAT, inherit, concatenation_out);
	} else if(rule == 2){
		*synth = inherit;
	} else {
		invalidRule("concatenation", rule);
	}
}

static void ast_basic_re(struct basic_re *basic_re, struct ast_node **synth){
	int rule = basic_re->rule;	
	struct ast_node *elementary_synth;
	struct ast_node *repeat_synth;

	if(rule == 1){
		ast_elementary_re(basic_re->elementary_re, &elementary_synth);
		ast_repeat(
				basic_re->repeat,
				elementary_synth,
				&repeat_synth);
		*synth = repeat_synth;
	} else {
		invalidRule("basic_re", rule);
	}
}

static void ast_repeat(
		struct repeat *item,
		struct ast_node *inherit,
		struct ast_node **synth){

	if(item->rule == 1){
		notImplemented("repeat", item->rule);
	} else if(item->rule == 2){
		notImplemented("repeat", item->rule);
	} else if(item->rule == 3){
		notImplemented("repeat", item->rule);
	} else if(item->rule == 4){
		*synth = inherit;		
	} else {
		invalidRule("repeat", item->rule);
	}
}

static void ast_elementary_re(struct elementary_re *item, struct ast_node **synth){
	if(item->rule == 1){
		notImplemented("elementary_re", item->rule);
	} else if(item->rule == 2){
		notImplemented("elementary_re", item->rule);
	} else if(item->rule == 3){
		notImplemented("elementary_re", item->rule);
	} else if(item->rule == 4){
		ast_character(item->character, synth);
	} else if(item->rule == 5){
		notImplemented("elementary_re", item->rule);
	} else {
		invalidRule("elementary_re", item->rule);
	}
}

static void ast_character(struct character *item, struct ast_node **synth){
	if(item->rule == 1){
		ast_nonmetachar(item->nonmetachar, synth);
	} else if(item->rule == 2){
		notImplemented("character", item->rule);
	} else {
		invalidRule("character", item->rule);
	}
}

static void ast_nonmetachar(struct nonmetachar *item, struct ast_node **synth){
	if(item->rule == 1){
		*synth = astLeafNode(OP_LITERAL, item->token);
	} else {
		invalidRule("nonmetachar", item->rule);
	}
}


int computeAST(){
	ast_re(PARSETREE, &AST);
	return 0;
}


static int nodeNumber;

static int emitNode(char *s){
	int node = nodeNumber++;
	printf("\tnode%d [ label = \"%s\"]\n", node, s);
	return node;
}

static int emitLeafNode(int value){
	int node = nodeNumber++;
	printf("\tnode%d [ label = \"%c\"", node, (char)value);
	printf(", shape = oval, fillcolor=green, style=filled];\n");
	return node;
}

static int emitEdge(int n1, int n2){
	printf("\tnode%d -> node%d\n", n1, n2);
	return 0;
}


static int graph(struct ast_node *node){
	int n;
	int c1;
	int c2;

	if(node->op == OP_INVALID){
		fprintf(stderr, "invalid op\n");
		exit(1);
	} else if(node->op == OP_UNION){
		n = emitNode("union");
		c1 = graph(node->child1);
		c2 = graph(node->child2);
		emitEdge(n, c1);
		emitEdge(n, c2);
		return n;
	} else if(node->op == OP_CONCAT){
		n = emitNode("concat");
		c1 = graph(node->child1);
		c2 = graph(node->child2);
		emitEdge(n, c1);
		emitEdge(n, c2);
		return n;
	} else if(node->op == OP_STAR){
		n = emitNode("star");
		c1 = graph(node->child1);
		emitEdge(n, c1);
		return n;
	} else if(node->op == OP_LITERAL){
		n = emitLeafNode(node->value);
		return n;
	} else {
		fprintf(stderr, "unknown op type: %d\n", node->op);
		exit(1);
	}
}

void graphAST(){
	int n;

	nodeNumber = 0;
	printf("digraph G {\n");
	printf("\tnode [shape=record]\n");
	n = graph(AST);
	printf("}\n");
}
