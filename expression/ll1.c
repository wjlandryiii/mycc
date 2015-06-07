/*
 * Copyright 2015 Joseph Landry All Rights Reserved
 */

/*
 * Parses grammar 4.28 from dragon book 2nd edition.
 *
 * 0: E  -> T Ep
 * 1: Ep -> + T Ep
 * 2: Ep -> ''
 * 3: T  -> F Tp
 * 4: Tp -> * F Tp
 * 5: Tp -> ''
 * 6: F  -> ( E )
 * 7: F  -> id
 */

#include <stdio.h>
#include <stdlib.h>

#define die() ({printf("die\n"); exit(1);})

char *ruleStrings[] = {
	"0:  E  -> T Ep",
	"1:  Ep -> + T Ep",
	"2:  Ep -> ''",
	"3:  T  -> F Tp",
	"4:  Tp -> * F Tp",
	"5:  Tp -> ''",
	"6:  F  -> ( E )",
	"7:  F  -> id"
};

enum TOKEN_NAMES {
	TOKEN_INVALID,
	TOKEN_ID,
	TOKEN_PLUS,
	TOKEN_ASTERISK,
	TOKEN_OPENPAREN,
	TOKEN_CLOSEPAREN,
	TOKEN_EOF,
	TOKEN_NAMES_COUNT
};

int lookahead;

void nextToken(){
	int c;

	do {
		c = getchar();
	} while(c == ' ' || c == '\t');

	if(c == '+'){
		lookahead = TOKEN_PLUS;
		return;
	} else if(c == '*'){
		lookahead = TOKEN_ASTERISK;
		return;
	} else if(c == '('){
		lookahead = TOKEN_OPENPAREN;
		return;
	} else if(c == ')'){
		lookahead = TOKEN_CLOSEPAREN;
		return;
	} else if(c == '\n'){
		lookahead = TOKEN_EOF;
	} else if(c == '_' || ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z')){
		do {
			c = getchar();
		} while(c == '_' || ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || ('0' <= c && c <= '9'));
		ungetc(c, stdin);
		lookahead = TOKEN_ID;
		return;
	} else {
		ungetc(c, stdin);
		lookahead = TOKEN_INVALID;
		return;
	}
}


int E();
int Ep();
int T();
int Tp();
int F();

int NODE = 0;

int E(){
	int node = NODE++;

	if(lookahead == TOKEN_ID || lookahead == TOKEN_OPENPAREN){
		// E  -> T E'
		int t, ep;

		printf("\tnode%d [ label = \"<f0> T | <f1> E'\"]\n", node);
		t = T();
		printf("\tnode%d:f0 -> node%d\n", node, t);
		ep = Ep();
		printf("\tnode%d:f1 -> node%d\n", node, ep);
		return node;
	} else {
		die();
	}
}

int Ep(){
	int node = NODE++;

	if(lookahead == TOKEN_PLUS){
		// 1:  Ep -> + T E'
		int plus, t, ep;

		printf("\tnode%d [ label = \"<f0> + | <f1> T | <f2> E'\"];\n", node);
		plus = NODE++;
		printf("\tnode%d [ label = \"+\", shape = oval, fillcolor=green, style=filled];\n", plus);
		printf("\tnode%d:f0 -> node%d\n", node, plus);
		nextToken();
		t = T();
		printf("\tnode%d:f1 -> node%d\n", node, t);
		ep = Ep(t+1);
		printf("\tnode%d:f2 -> node%d\n", node, ep);
		return node;
	} else if(lookahead == TOKEN_CLOSEPAREN || lookahead == TOKEN_EOF){
		// 2:  Ep -> {}
		printf("\tnode%d [ label = \"<f0> \\{\\}\"];\n", node);
		return node;
	} else {
		die();
	}
}

int T(){
	int node = NODE++;

	if(lookahead == TOKEN_ID || lookahead == TOKEN_OPENPAREN){
		// 3:  T  -> F Tp",
		int f, tp;

		printf("\tnode%d [ label = \"<f0> F | <f1> Tp\"];\n", node);
		f = F();
		printf("\tnode%d:f0 -> node%d\n", node, f);
		tp = Tp();
		printf("\tnode%d:f1 -> node%d\n", node, tp);
		return node;
	} else {
		die();
	}
}

int Tp(){
	int node = NODE++;

	if(lookahead == TOKEN_ASTERISK){
		// 4:  Tp -> * F Tp",
		int asterisk, f, tp;

		printf("\tnode%d [ label = \"<f0> * | <f1> F | <f2> Tp\"];\n", node);
		asterisk = NODE++;
		printf("\tnode%d [ label = \"*\", shape = oval, fillcolor=green, style=filled];\n", asterisk);
		printf("\tnode%d:f0 -> node%d\n", node, asterisk);
		nextToken();
		f = F();
		printf("\tnode%d:f1 -> node%d\n", node, f);
		tp = Tp();
		printf("\tnode%d:f2 -> node%d\n", node, tp);
		return node;
	} else if(lookahead == TOKEN_PLUS || lookahead == TOKEN_CLOSEPAREN || lookahead == TOKEN_EOF){
		// 5:  Tp -> {}",
		printf("\tnode%d [ label = \"<f0> \\{\\}\"]\n", node);
		return node;
	} else {
		die();
	}
}

int F(){
	int node = NODE++;

	if(lookahead == TOKEN_OPENPAREN){
		// 6:  F  -> ( E )
		int openparen, e, closeparen;

		printf("\tnode%d [ label = \"<f0> ( | <f1> E | <f2> )\"];\n", node);
		openparen = NODE++;
		printf("\tnode%d [ label = \"(\", shape = oval, fillcolor=green, style=filled];\n", openparen);
		printf("\tnode%d:f0 -> node%d\n", node, openparen);
		nextToken();
		e = E();
		printf("\tnode%d:f1 -> node%d\n", node, e);
		if(lookahead == TOKEN_CLOSEPAREN){
			closeparen = NODE++;
			printf("\tnode%d [ label = \"(\", shape = oval, fillcolor=green, style=filled];\n", closeparen);
			printf("\tnode%d:f2 -> node%d\n", node, closeparen);
			nextToken();
			return node;
		} else {
			die();
		}
	} else if(lookahead == TOKEN_ID){
		// 7:  F  -> id"
		int id;

		printf("\tnode%d [ label = \"<f0> id\"]\n", node);
		id = NODE++;
		printf("\tnode%d [ label = \"id\", shape = oval, fillcolor=green, style=filled];\n", id);
		printf("\tnode%d -> node%d\n", node, id);
		nextToken();
		return node;
	} else {
		die();
	}
}



int main(int argc, char *argv[]){
	int rootNode;

	printf("digraph {\n");
	printf("\tnode [shape = record];\n");
	nextToken();

	NODE = 0;
	rootNode = NODE++;
	printf("\tnode%d [ label = \"<f0> E\"];\n", rootNode);
	int e = E();
	printf("\tnode%d -> node%d\n", rootNode, e);
	printf("}\n");
	return 0;
}
