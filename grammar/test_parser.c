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
	strcpy(lexerInputBuffer, grammar);
	result = tokenize();
	if(result != 0){
		printf("lexer error: %d\n", parserErrorNumber);
		exit(1);
	}
	result = parse();
	if(result != 0){
		printf("parser error: %d\n", parserErrorNumber);
	}

	if(ruleCount != 6){
		printf("Invalid rule count: %d\n", ruleCount);
		exit(1);
	}
}

void print_parser_results(){
	int i, j;
	int nonterminalIndex = -1;
	struct term *body;
	int index;
	char *string;

	for(i = 0; i < ruleCount; i++){
		if(nonterminalIndex != rules[i].nonterminalIndex){
			nonterminalIndex = rules[i].nonterminalIndex;
			int stringIndex = nonterminals[nonterminalIndex];
			char *string = strings[stringIndex];
			printf("<%s>\n\t", string);
		} else {
			printf("\t");
		}
		body = rules[i].body;
		printf(":");
		for(j = 0; j < rules[i].bodyLength; j++){
			if(body[j].type == TERMTYPE_TERMINAL){
				index = body[j].index;
				index = terminals[index];
				string = strings[index];
				printf(" '%s'", string);
			} else if(body[j].type == TERMTYPE_NONTERMINAL){
				index = body[j].index;
				index = nonterminals[index];
				string = strings[index];
				printf(" '%s'", string);
			} else {
				printf("INVALID TYPE: %d\n", body[j].type);
				return;
			}
		}
		if(rules[i].bodyLength == 0){
			printf(" ''");
		}
		printf("\n");
	}
}


int main(int argc, char *argv[]){
	test_parser();
	print_parser_results();
	return 0;
}
