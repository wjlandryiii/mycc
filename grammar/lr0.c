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


void printItem(int i);

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


	printf("CLOSURE INPUT: I = { ");
	for(i = 0; i < nIS; i++){
		printf(" %2d,", I[i]);
	}
	printf(" }\n" );

	for(i = 0; i < nIS; i++){
		J[i] = I[i];
	}
	nJS = nIS;

	iteration = 1;
	do {
		//printf("ITERATION: %d\n", iteration++);
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
				//printf("RULE:%d DOT:%d ", rule, dot);
				//printf("\tsymbol: %s(%d)(%d\n", STRING[SYMBOL[symbol]], SYMBOL[symbol], symbol);
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

	printf("CLOSURE OUTPUT: J = { ");
	for(i = 0; i < nJS; i++){
		printf(" %2d,", J[i]);
	}
	printf(" }\n" );


	return 0;
}

static int GOTO(){
	int i, j;

	nJS = 0;

	printf("GOTO INPUT: I = { ");
	for(i = 0; i < nIS; i++){
		printf(" %2d,", I[i]);
	}
	printf(" }  X = %2d\n", X);
	printf("\n");

	for(i = 0; i < nIS; i++){
		printf("i: %2d\t", i);
		printItem(I[i]);
		int rule = ITEM[I[i]];
		int dot = ITEMDOT[I[i]];
		int ruleSize = RULESIZE[rule];

		if(dot < ruleSize){
			int symbol = RULE[rule][dot];
			//printf("GOTO: {X:%2d} {symbol:%2d}\n", X, symbol);
			if(symbol == X){
				int item = ITEMINDEX[rule][dot+1];
				if(!isInJ(item)){
					insertIntoJ(item);
				}
			}
		}
	}

	if(nJS > 0){
		for(i = 0; i < nJS; i++){
			I[i] = J[i];
		}
		nIS = nJS;
		CLOSURE();
	}
	return 0;
}


static int nCS = 0;
static int C[MAX_SOMETHING][MAX_SOMETHING];
static int CSIZE[MAX_SOMETHING];

static void items(){
	int i, j, k, l;
	int changed;

	// INIT
	nCS = 0;
	for(i = 0; i < MAX_SOMETHING; i++){
		CSIZE[i] = 0;
		for(j = 0; j < MAX_SOMETHING; j++){
			C[i][j] = 0;
		}
	}

	// START
	nIS = 0;
	I[nIS] = 18;
	nIS += 1;
	CLOSURE();
	for(i = 0; i < nJS; i++){
		C[nCS][i] = J[i];
	}
	CSIZE[nCS] = nJS;
	nCS += 1;

	do {
		changed = 0;
		for(i = 0; i < nCS; i++){
			for(j = 0; j < nSYMBOLS; j++){
				nIS = 0;
				for(k = 0; k < CSIZE[i]; k++){
					I[nIS++] = C[i][k];
				}
				X = j;
				GOTO();
				printf("  nJS: %d  %s\n", nJS, STRING[SYMBOL[j]]);
				if(nJS > 0){
					int found = 0;
					for(k = 0; k < nCS; k++){
						if(CSIZE[k] == nJS){
							printf("found matching size\n");
							for(l = 0; l < nJS; l++){
								if(C[k][l] != J[l]){
									break;
								}
							}
							if(l == nJS){
								found = 1;
								break;
							}
						}
					}
					printf("found: %d\n", found);

					if(!found){
						// add J to C
						for(k = 0; k < nJS; k++){
							C[nCS][CSIZE[nCS]] = J[k];
							CSIZE[nCS] += 1;
						}
						nCS += 1;
						changed = 1;
					}

				}
			}
		}
	} while(0);

}


///////////////////      TESTING               ///////////////


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
		printf("J[%2d] = ", i);
		printItem(J[i]);
	}
}

void printItemSets(){
	int i, j, k;

	for(i = 0; i < nCS; i++){
		printf("%d:\n", i);

		for(j = 0; j < CSIZE[i]; j++){
			int item = C[i][j];
			int rule = ITEM[item];
			int dot = ITEMDOT[item];
			int ruleSize = RULESIZE[rule];

			printf("\t%2d:%2d:[dot:%2d] ", j, rule, dot);
			printf("%2s -> ", STRING[SYMBOL[RULENAME[rule]]]);
			for(k = 0; k < ruleSize; k++){
				printf(" %c %2s", k==dot ? '.' : ' ', STRING[SYMBOL[RULE[rule][k]]]);
			}
			if(k == dot){
				printf(" .");
			}
			printf("\n");

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
	"	;\n"
	"<Sp>\n"
	"	: <E>\n"
	"	;\n";

void test_grammar_4_1(){
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
	I[0] = 18;
	nIS = 1;
	CLOSURE();
	printJ();

	printf("********* GOTO(I,X) **************\n");
	/*
	 *  0:  E ->  .  E    +    T
	 *  1:  E ->     E .  +    T
	 *  2:  E ->     E    + .  T
	 *  3:  E ->     E    +    T .
	 *  4:  E ->  .  T
	 *  5:  E ->     T .
	 *  6:  T ->  .  T    *    F
	 *  7:  T ->     T .  *    F
	 *  8:  T ->     T    * .  F
	 *  9:  T ->     T    *    F .
	 * 10:  T ->  .  F
	 * 11:  T ->     F .
	 * 12:  F ->  .  (    E    )
	 * 13:  F ->     ( .  E    )
	 * 14:  F ->     (    E .  )
	 * 15:  F ->     (    E    ) .
	 * 16:  F ->  . %id%
	 * 17:  F ->    %id% .
	 * 18: Sp ->  .  E
	 * 19: Sp ->     E .
	 */
	/*
	nIS = 0;
	I[nIS++] = 18;
	I[nIS++] = 0;
	I[nIS++] = 4;
	I[nIS++] = 6;
	I[nIS++] = 10;
	I[nIS++] = 12;
	I[nIS++] = 16;
	*/

	for(i = 0; i < nJS; i++){
		I[i] = J[i];
	}
	nIS = nJS;

	X = 5;
	GOTO();
	for(i = 0; i < nJS; i++){
		printItem(J[i]);
	}
	printf("\n");

	printJ();


	printf("*********** items() ***************\n");
	items();
	printItemSets();
	printf("number of item sets: %d\n", nCS);
	printJ();


}

void printItem(int item){
	int i;
	int rule = ITEM[item];
	int dot = ITEMDOT[item];
	int ruleSize = RULESIZE[rule];

	printf("{item:%2d}, {rule:%2d}, {ruleSize:%2d} {dot:%2d}, ", item, rule, ruleSize, dot);

	printf("{%2s -> ", STRING[SYMBOL[RULENAME[rule]]]);
	for(i = 0; i < ruleSize; i++){
		printf(" %c %2s", i==dot ? '.' : ' ', STRING[SYMBOL[RULE[rule][i]]]);
	}
	if(i == dot){
		printf(" .");
	}
	printf("}\n");


}

int main(int argc, char *argv[]){
	test_grammar_4_1();
}
