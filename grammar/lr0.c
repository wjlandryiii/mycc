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

#define MAX_SOMETHING (1024)

static int nITEMS = 0;
static int ITEM[MAX_SOMETHING];
static int ITEMDOT[MAX_SOMETHING];
static int ITEMINDEX[MAX_SOMETHING][MAX_SOMETHING];

static void initLR0(){
	int i, j;

	nITEMS = 0;
	for(i = 0; i < MAX_SOMETHING; i++){
		ITEM[i] = -1;
		ITEMDOT[i] = -1;
		for(j = 0; j < MAX_SOMETHING; j++){
			ITEMINDEX[i][j] = -1;
		}
	}
}

static int addItem(int rule, int dot){
	assert(nITEMS < MAX_SOMETHING);
	ITEM[nITEMS] = rule;
	ITEMDOT[nITEMS] = dot;
	ITEMINDEX[rule][dot] = nITEMS;
	return nITEMS++;
}

static int generateItems(){
	int i, j;

	for(i = 0; i < nRULES; i++){
		for(j = 0; j < RULESIZE[i] + 1; j++){
			addItem(i, j);
		}
	}
	return 0;
}



static int I[MAX_SOMETHING];
static int nIS;
static int X;
static int J[MAX_SOMETHING];
static int nJS;


static int isInJ(int x){
	int i;

	for(i = 0; i < nJS; i++){
		if(J[i] == x){
			return 1;
		}
	}
	return 0;
}

static int insertIntoJ(int x){
	assert(nJS < MAX_SOMETHING);
	J[nJS] = x;
	return nJS++;
}

static int CLOSURE(){
	int h, i, j;
	int changed;
	int iteration;

	for(i = 0; i < nIS; i++){
		J[i] = I[i];
	}
	nJS = nIS;

	iteration = 1;
	do {
		printf("ITERATION: %d\n", iteration++);
		changed = 0;

		for(i = 0; i < nJS; i++){
			int rule = ITEM[J[i]];
			int dot = ITEMDOT[J[i]];
			int rulesize = RULESIZE[rule];

			if(dot < rulesize){
				int symbol = RULE[rule][dot];
				if(SYMBOLTYPE[symbol] == SYMBOLTYPE_TERMINAL){
					continue;
				}
				printf("RULE:%d DOT:%d ", rule, dot);
				printf("\tsymbol: %s(%d)(%d\n", STRING[SYMBOL[symbol]], SYMBOL[symbol], symbol);
				for(j = 0; j < nRULES; j++){
					if(RULENAME[j] == symbol){
						int rule = j;
						int dot = 0;
						int newItem = ITEMINDEX[rule][dot];
						if(!isInJ(newItem)){
							insertIntoJ(newItem);
							changed = 1;
						}
					}
				}
			}
		}
	}while(changed);
	return 0;
}

static int GOTO(){
	int i, j;

	nJS = 0;

	for(i = 0; i < nIS; i++){
		int rule = ITEM[I[i]];
		int dot = ITEMDOT[I[i]];
		int ruleSize = RULESIZE[rule];

		if(dot <= ruleSize){
			int symbol = RULE[rule][dot];
			if(symbol == X){
				int item = ITEMINDEX[rule][dot+1];
				insertIntoJ(item);
			}
		}
	}

	for(i = 0; i < nJS; i++){
		I[i] = J[i];
	}
	nIS = nJS;
	CLOSURE();
	return 0;
}


void printitems(){
	int i, j;

	for(i = 0; i < nITEMS; i++){
		int rule = ITEM[i];
		int dot = ITEMDOT[i];
		int ruleHead = RULENAME[rule];

		printf("%2d: %2s -> ", i, STRING[SYMBOL[RULENAME[rule]]]);

		for(j = 0; j < RULESIZE[rule]; j++){
			printf(" %c %2s", j == dot ? '.' : ' ', STRING[SYMBOL[RULE[rule][j]]]);
		}
		if(j == dot){
			printf(" .");
		}
		printf("\n");
	}
}

void printJ(){
	int i, j;

	for(i = 0; i < nJS; i++){
		int rule = ITEM[J[i]];
		int dot = ITEMDOT[J[i]];

		printf("%2d: %2s -> ", i, STRING[SYMBOL[RULENAME[rule]]]);

		for(j = 0; j < RULESIZE[rule]; j++){
			printf(" %c %2s", j == dot ? '.' : ' ', STRING[SYMBOL[RULE[rule][j]]]);
		}
		if(j == dot){
			printf(" .");
		}
		printf("\n");
	}
}


char *grammar_4_1 =
	"# Grammar 4.1 from 'Compilers: Priciples, Techniques, & Tools' Second Edition page 193\n"
	"\n"
	"<E>\n"
	"	: <E> '+' <T>\n"
	"	| <T>\n"
	"	;\n"
	"\n"
	"<T>\n"
	"	: <T> '*' <F>\n"
	"	| <F>\n"
	"	;\n"
	"\n"
	"<F>\n"
	"	: '(' <E> ')'\n"
	"	| '%id%'\n"
	"	;\n";


int main(int argc, char *argv[]){
	int i;

	strcpy(INPUTSTRING, grammar_4_1);
	tokenize();
	parse();

	printf("********** SYMBOLS ***********\n");
	for(i = 0; i < nSYMBOLS; i++){
		printf("%2d: %s\n", i, STRING[SYMBOL[i]]);
	}
	printf("\n");

	// START
	initLR0();
	generateItems();


	printf("********* ALL ITEMS **************\n");

	printitems();

	printf("********* CLOSURE(I) **************\n");
	I[0] = 13;
	nIS = 1;
	CLOSURE();
	printJ();

	printf("********* GOTO(I,X) **************\n");
	I[0] = 1;
	nIS = 1;
	X = 1;
	GOTO();

	printJ();
}
