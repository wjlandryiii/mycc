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

void printTermList(struct term_list *tl){
	int symbolIndex;
	int stringIndex;
	char *string;

	if(tl == NULL){
		fprintf(stderr, "unexpected NULL in printTermList()\n");
		exit(1);
	}

	symbolIndex = tl->termSymbol;
	stringIndex = symbolList[symbolIndex].string;
	string = strings[stringIndex];

	if(symbolList[symbolIndex].type == SYMTYPE_NONTERMINAL){
		printf(" <%s>", string);
	} else {
		printf(" \"%s\"", string);
	}

	if(tl->nextTermList != NULL){
		printTermList(tl->nextTermList);
	} else {
		printf("\n");
	}
}

void printRuleList(struct rule_list *rl, int count){
	if(rl == NULL){
		fprintf(stderr, "unexpeded NULL in printRuleList()\n");
		exit(1);
	}
	if(count == 0){
		printf("\t:");
	} else {
		printf("\t|");
	}
	printTermList(rl->termList);
	if(rl->nextRuleList != NULL){
		printRuleList(rl->nextRuleList, count+1);
	} else {
		printf("\t;\n\n");
	}
}

void printRuleSet(struct rule_set *rs){
	int symbolIndex;
	int stringIndex;
	char *string;

	if(rs == NULL){
		fprintf(stderr, "unexpeded NULL in printRuleSet()\n");
		exit(1);
	}

	symbolIndex = rs->ruleNameSymbol;
	stringIndex = symbolList[symbolIndex].string;
	string = strings[stringIndex];
	printf("<%s>\n", string);

	printRuleList(rs->ruleList, 0);

	if(rs->nextRuleSet == NULL){
		return;
	} else {
		printRuleSet(rs->nextRuleSet);
		return;
	}
}

void printParseTree(){
	printRuleSet(parseTree);
}

void printFFSet(struct ff_set_node *n){
	if(n != NULL){
		while(n){
			int symbolIndex;
			int stringIndex;
			char *string;

			symbolIndex = n->symbolIndex;
			stringIndex = symbolList[symbolIndex].string;
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
		stringIndex = symbolList[symbolIndex].string;
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
	fileName = "grammar_3_12.txt";

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

	printParseTree();

	firstfollow();
	printFirstFollow();

	return 0;
}
