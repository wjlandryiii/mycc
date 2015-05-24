/*
 * Copyright 2015 Joseph Landry All Rights Reserved
 */

#include <stdio.h>
#include <stdlib.h>

struct token {
	int type;
	int value;
};

enum TOK_TYPES {
	TOK_INVALID,
	TOK_CONSTANT,
	TOK_PLUS,
	TOK_MINUS,
	TOK_EOF
};

int ctok;
struct token tokens[64];


void constant(){
	if(tokens[ctok].type == TOK_CONSTANT){
		ctok++;
	} else {
		printf("Expected constant\n");
		exit(1);
	}
}

void term(){
	constant();
}

void rest_expression(){
	if(tokens[ctok].type == TOK_PLUS){
		ctok++;
		term();
		rest_expression();

	} else if(tokens[ctok].type == TOK_MINUS){
		ctok++;
		term();
		rest_expression();
	} else {
		// NULL production rule
	}
}

void expression(){
	term();
	rest_expression();
}

int main(int argc, char *argv[]){

	tokens[0].type = TOK_CONSTANT;
	tokens[0].value = 5;
	tokens[1].type = TOK_PLUS;
	tokens[2].type = TOK_CONSTANT;
	tokens[2].value = 10;
	tokens[3].type = TOK_EOF;

	ctok = 0;
	expression();
	printf("%d\n", ctok);

	return 0;
}
