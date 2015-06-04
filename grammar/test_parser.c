/*
 * Copyright 2015 Joseph Landry All Rights Reserved
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lexer.h"
#include "stringtable.h"
#include "symbols.h"
#include "parser.h"


void test_parser(){
	char *grammar =
		"<Z>	: \"d\"\n"
		"	| <X> <Y> <Z>\n"
		"	;\n"
		"\n"
		"<Y>	: \"\"\n"
		"	| \"c\"\n"
		"	;\n"
		"\n"
		"<X>	: <Y>\n"
		"	| \"a\"\n"
		"	;\n"
		"\n";
	int result;
	strcpy(INPUTSTRING, grammar);
	result = tokenize();
	if(result != 0){
		printf("lexer error: %d\n", parserErrorNumber);
		exit(1);
	}
	result = parse();
	if(result != 0){
		printf("parser error: %d\n", parserErrorNumber);
		exit(1);
	}

	if(nRULES != 6){
		printf("Invalid rule count: %d\n", nRULES);
		exit(1);
	}
}


void printParserOutput(){
	int i, j;
	for(i = 0; i < nRULES; i++){
		printf("%s: (%d items) ", STRING[SYMBOL[RULENAME[i]]], RULESIZE[i]);
		for(j = 0; j < RULESIZE[i]; j++){
			printf(" %s", STRING[SYMBOL[RULE[i][j]]]);
		}
		printf("\n");
	}
	printf("\n");
}


int main(int argc, char *argv[]){
	test_parser();
	printParserOutput();
	return 0;
}
