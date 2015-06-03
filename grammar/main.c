/*
 * Copyright 2015 Joseph Landry All Rights Reserved
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "stringtable.h"
#include "symbols.h"
#include "lexer.h"
#include "parser.h"
#include "firstfollow.h"

void loadInput(char *fileName){
	long fileSize;
	FILE *f;

	f = fopen(fileName, "r");
	if(f == NULL){
		fprintf(stderr, "Error opening file: %s\n", fileName);
		exit(1);
	}

	fseek(f, 0, SEEK_END);
	fileSize = ftell(f);
	if(fileSize < 0){
		fprintf(stderr, "Error from ftell()\n");
		exit(1);
	}
	fseek(f, 0, SEEK_SET);

	if(fileSize >= sizeof(lexerInputBuffer) - 1){
		fprintf(stderr, "File size too big: %ld bytes\n", fileSize);
		exit(1);
	}

	if(fread(lexerInputBuffer, 1, fileSize, f) != fileSize){
		fprintf(stderr, "Error reading file\n");
		exit(1);
	}

	lexerInputBuffer[fileSize] = '\0';
	fclose(f);
}

void printParserOutput(){
	int i, j;
	int ruleName = -1;
	struct term *body;

	for(i = 0; i < ruleCount; i++){
		if(i == 0){
			ruleName = rules[i].nonterminalIndex;
			printf("<%s>\n", strings[nonterminals[ruleName]]);
			printf("\t:");
		} else if(ruleName != rules[i].nonterminalIndex){
			ruleName = rules[i].nonterminalIndex;
			printf("\t;\n\n");
			printf("<%s>\n", strings[nonterminals[ruleName]]);
			printf("\t:");
		} else {
			printf("\t|");
		}

		body = rules[i].body;
		for(j = 0; j < rules[i].bodyLength; j++){
			if(body[j].type == TERMTYPE_TERMINAL){
				printf(" \"%s\"", strings[terminals[body[j].index]]);
			} else if(body[j].type == TERMTYPE_NONTERMINAL){
				printf(" <%s>", strings[nonterminals[body[j].index]]);
			} else {
				printf(" ???");
			}
		}
		printf("\n");
	}
	printf("\t;\n");
}

void printFFSet(struct ff_set_node *n){
	if(n != NULL){
		while(n){
			int symbolIndex;
			int stringIndex;
			char *string;

			symbolIndex = n->symbolIndex;
			stringIndex = terminals[symbolIndex];
			string = strings[stringIndex];
			printf(" %s", string);
			n = n->next;
		}
		printf("\n");
	} else {
		printf(" (empty)\n");
	}
}

void printFirstFollow(){
	struct ff_node *p;
	int symbolIndex;
	int stringIndex;
	char *string;

	p = firstFollowSet;

	while(p){
		symbolIndex = p->nonTerminalSymbolIndex;
		stringIndex = nonterminals[symbolIndex];
		string = strings[stringIndex];
		printf("%s\n", string);
		printf("\tNullable  : %s\n", p->isNullable ? "YES" : "NO");
		printf("\tFIRST SET :");
		printFFSet(p->first);
		printf("\tFOLLOW SET:");
		printFFSet(p->follow);

		printf("\n");
		p = p->next;
	}
}

int main(int argc, char *argv[]){
	int result;
	char *fileName = "grammar.txt";
	//fileName = "grammar_3_12.txt";

	loadInput(fileName);
	result = tokenize();
	if(result != 0){
		fprintf(stderr, "lexer error: %d on line: %d\n",
				lexerErrorNumber, lexerErrorLineNumber);
		exit(1);
	}

	if(parse() != 0){
		fprintf(stderr, "Error parsing: %d\n", parserErrorNumber);
		exit(1);
	}

	printParserOutput();

	firstfollow();
	//printFirstFollow();

	return 0;
}
