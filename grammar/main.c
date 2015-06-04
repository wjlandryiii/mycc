/*
 * Copyright 2015 Joseph Landry All Rights Reserved
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

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

void loadFirstFollowInput(){
	int i,j;

	for(i = 0; i < nonterminalCount; i++){
		assert(nSYMBOLS < MAX_SYMBOLS);
		SYMBOL[nSYMBOLS] = nonterminals[i];
		SYMBOLTYPE[nSYMBOLS] = SYMBOLTYPE_NONTERMINAL;
		nSYMBOLS += 1;
	}
	for(i = 0; i < terminalCount; i++){
		assert(nSYMBOLS < MAX_SYMBOLS);
		SYMBOL[nSYMBOLS] = terminals[i];
		SYMBOLTYPE[nSYMBOLS] = SYMBOLTYPE_TERMINAL;
		nSYMBOLS += 1;
	}

	for(i = 0; i < ruleCount; i++){
		assert(nRULES < MAX_RULES);
		RULENAME[nRULES] = rules[i].nonterminalIndex;
		for(j = 0; j < rules[i].bodyLength; j++){
			assert(j < MAX_RULE_SIZE);
			if(rules[i].body[j].type == TERMTYPE_NONTERMINAL){
				RULE[nRULES][RULESIZE[nRULES]] = rules[i].body[j].index;
			} else if(rules[i].body[j].type == TERMTYPE_TERMINAL){
				RULE[nRULES][RULESIZE[nRULES]] = rules[i].body[j].index + nonterminalCount;
			} else {
				printf("type: %d\n", rules[i].body[j].type);
				fflush(stdout);
				assert(0);
			}
			RULESIZE[nRULES] += 1;
		}
		nRULES += 1;
	}
}

void printFirstFollowInput(){
	int i,j;

	for(i = 0; i < nSYMBOLS; i++){
		printf("%d:%d: %s\n", i, SYMBOL[i], strings[SYMBOL[i]]);
	}
	printf("\n");

	for(i = 0; i < nRULES; i++){
		printf("%2d: %s(%d) ->  ", i, strings[SYMBOL[RULENAME[i]]], SYMBOL[RULENAME[i]]);
		for(j = 0; j < RULESIZE[i]; j++){
			printf(" %s:%d", strings[SYMBOL[RULE[i][j]]], SYMBOL[RULE[i][j]]);
		}
		printf("\n");
	}
	printf("\n");
}


void printFirstFollowOutput(){
	int i, j;

	for(i = 0; i < nSYMBOLS; i++){
		printf("NULLABLE(%s) => %s\n", strings[SYMBOL[i]], NULLABLE[i] ? "YES" : "NO");
	}

	printf("\n");

	for(i = 0; i < nSYMBOLS; i++){
		printf("FIRST(%s) => ", strings[SYMBOL[i]]);
		for(j = 0; j < FIRSTSIZE[i]; j++){
			printf(" %s:%d", strings[SYMBOL[FIRST[i][j]]], SYMBOL[FIRST[i][j]]);
		}
		printf("\n");
	}
	
	printf("\n");
	
	for(i = 0; i < nSYMBOLS; i++){
		printf("FOLLOW(%s) => ", strings[SYMBOL[i]]);
		for(j = 0; j < FOLLOWSIZE[i]; j++){
			printf(" %s:%d", strings[SYMBOL[FOLLOW[i][j]]], SYMBOL[FOLLOW[i][j]]);
		}
		printf("\n");
	}
	printf("\n");
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

	//printParserOutput();

	//loadFirstFollowInput();
	printf("***********   INPUT   **********\n");
	printFirstFollowInput();
	computeNullable();
	computeFirst();
	computeFollow();
	printf("***********   OUTPUT   **********\n");
	printFirstFollowOutput();

	return 0;
}
