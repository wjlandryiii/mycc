/*
 * Copyright 2015 Joseph Landry All Rights Reserved
 */

#include <stdio.h>
#include <stdlib.h>

struct token {
	int type;
};

enum TOK_TYPES {
	TOK_INVALID,
	TOK_AUTO,
	TOK_REGISTER,
	TOK_STATIC,
	TOK_EXTERN,
	TOK_TYPEDEF,
	TOK_VOID,
	TOK_CHAR,
	TOK_SHORT,
	TOK_INT,
	TOK_LONG,
	TOK_FLOAT,
	TOK_DOUBLE,
	TOK_SIGNED,
	TOK_UNSIGNED,
	TOK_CONST,
	TOK_VOLATILE,
	TOK_IDENTIFIER,
	TOK_OBRACKET,
	TOK_CBRACKET,
	TOK_OPAREN,
	TOK_CPAREN,
	TOK_EOF,
};

int ctok;
struct token tokens[64];

int storage_class_specifier(){
	if(tokens[ctok].type == TOK_AUTO){
		ctok++;
		return 0;
	} else if(tokens[ctok].type == TOK_REGISTER){
		ctok++;
		return 0;
	} else if(tokens[ctok].type == TOK_STATIC){
		ctok++;
		return 0;
	} else if(tokens[ctok].type == TOK_EXTERN){
		ctok++;
		return 0;
	} else if(tokens[ctok].type == TOK_TYPEDEF){
		ctok++;
		return 0;
	} else {
		return -1;
	}
}

int type_specifier(){
	if(tokens[ctok].type == TOK_VOID){
		ctok++;
		return 0;
	} else if(tokens[ctok].type == TOK_CHAR){
		ctok++;
		return 0;
	} else if(tokens[ctok].type == TOK_SHORT){
		ctok++;
		return 0;
	} else if(tokens[ctok].type == TOK_INT){
		ctok++;
		return 0;
	} else if(tokens[ctok].type == TOK_LONG){
		ctok++;
		return 0;
	} else if(tokens[ctok].type == TOK_FLOAT){
		ctok++;
		return 0;
	} else if(tokens[ctok].type == TOK_DOUBLE){
		ctok++;
		return 0;
	} else if(tokens[ctok].type == TOK_SIGNED){
		ctok++;
		return 0;
	} else {
		return -1;
	}
}

int main(int argc, char *argv[]){
	

	return 0;
}
