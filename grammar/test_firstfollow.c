/*
 * Copyright 2015 Joseph Landry All Rights Reserved
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <assert.h>

#include "firstfollow.h"


// INPUT
#define MAX_STRINGS (128)
#define MAX_SYMBOLS (128)
#define MAX_RULES (128)
#define MAX_RULE_SIZE (128)

#define TYPE_TERMINAL (1)
#define TYPE_NONTERMINAL (2)

int nSTRINGS = 0;
char *STRING[MAX_STRINGS];

int nSYMBOLS = 0;
int SYMBOL[MAX_SYMBOLS];
int SYMBOLTYPE[MAX_SYMBOLS];

int nRULES = 0;
int RULENAME[MAX_RULES];
int RULE[MAX_RULES][MAX_RULE_SIZE];
int RULESIZE[MAX_RULES];

// OUTPUT
#define MAX_FIRST_SET_SIZE (128)
#define MAX_FOLLOW_SET_SIZE (128)

int NULLABLE[MAX_SYMBOLS];

int FIRST[MAX_SYMBOLS][MAX_FIRST_SET_SIZE];
int FIRSTSIZE[MAX_SYMBOLS];

int FOLLOW[MAX_SYMBOLS][MAX_FOLLOW_SET_SIZE];
int FOLLOWSIZE[MAX_SYMBOLS];


int addString(char *s){
	assert(nSTRINGS < MAX_STRINGS);
	STRING[nSTRINGS] = s;
	return nSTRINGS++;
}

int addSymbol(int value, int type){
	assert(nSYMBOLS < MAX_SYMBOLS);
	assert(type == TYPE_TERMINAL || type == TYPE_NONTERMINAL);
	SYMBOL[nSYMBOLS] = value;
	SYMBOLTYPE[nSYMBOLS] = type;
	return nSYMBOLS++;
}

void addRule(int ruleName, ...){
	va_list ap;
	int symbol;
	int n;

	assert(nRULES < MAX_RULES);

	RULENAME[nRULES] = ruleName;
	RULESIZE[nRULES] = 0;

	va_start(ap, ruleName);

	n = 0;
	symbol = va_arg(ap, int);
	while(symbol > -1){
		assert(n < MAX_RULE_SIZE);
		RULE[nRULES][n] = symbol;
		n += 1;
		symbol = va_arg(ap, int);
	}
	RULESIZE[nRULES] = n;
	nRULES += 1;
}

void loadInput_3_12(){
	/*
	 * Grammar 3.12 from "Modern Compiler Implementation in C" page 48.
	 * X -> Y
	 * X -> a
	 * 
	 * Y ->
	 * Y -> c
	 *
	 * Z -> d
	 * Z -> X Y Z
	 *
	 */

	int X = addSymbol(addString("X"), TYPE_NONTERMINAL);
	int Y = addSymbol(addString("Y"), TYPE_NONTERMINAL);
	int Z = addSymbol(addString("Z"), TYPE_NONTERMINAL);
	int a = addSymbol(addString("a"), TYPE_TERMINAL);
	int b = addSymbol(addString("b"), TYPE_TERMINAL);
	int c = addSymbol(addString("c"), TYPE_TERMINAL);
	int d = addSymbol(addString("d"), TYPE_TERMINAL);

	addRule(X, Y, -1);
	addRule(X, a, -1);

	addRule(Y, -1);
	addRule(Y, c, -1);

	addRule(Z, d, -1);
	addRule(Z, X, Y, Z, -1);
}

void loadInput_3_15(){
	/*
	 * Grammar 3.15 from "Modern Compiler Implementation in C" page 53.
	 *
	 * S -> E $
	 * E -> T E'
	 *
	 * E' -> + T E'
	 * E' -> - T E'
	 * E' ->
	 *
	 * T -> F T'
	 *
	 * T' -> * F T'
	 * T' -> / F T'
	 * T' ->
	 *
	 * F -> id
	 * F -> num
	 * F -> ( E )
	 *
	 */
	int S  = addSymbol(addString("S"), TYPE_NONTERMINAL);
	int E  = addSymbol(addString("E"), TYPE_NONTERMINAL);
	int Ep = addSymbol(addString("Ep"), TYPE_NONTERMINAL);
	int T  = addSymbol(addString("T"), TYPE_NONTERMINAL);
	int Tp = addSymbol(addString("Tp"), TYPE_NONTERMINAL);
	int F  = addSymbol(addString("F"), TYPE_NONTERMINAL);
	
	int plus    = addSymbol(addString("+"), TYPE_TERMINAL);
	int minus   = addSymbol(addString("-"), TYPE_TERMINAL);
	int times   = addSymbol(addString("*"), TYPE_TERMINAL);
	int divide  = addSymbol(addString("/"), TYPE_TERMINAL);
	int id      = addSymbol(addString("id"), TYPE_TERMINAL);
	int num     = addSymbol(addString("num"), TYPE_TERMINAL);
	int popen   = addSymbol(addString("("), TYPE_TERMINAL);
	int pclose  = addSymbol(addString(")"), TYPE_TERMINAL);
	int dollar  = addSymbol(addString("$"), TYPE_TERMINAL);

	addRule(S, E, dollar, -1);

	addRule(E, T, Ep, -1);

	addRule(Ep, plus, T, Ep, -1);
	addRule(Ep, minus, T, Ep, -1);
	addRule(Ep, -1);

	addRule(T, F, Tp, -1);

	addRule(Tp, times, F, Tp, -1);
	addRule(Tp, divide, F, Tp, -1);
	addRule(Tp, -1);

	addRule(F, id, -1);
	addRule(F, num, -1);
	addRule(F, popen, E, pclose, -1);
}

void printInput(){
	int i,j;

	for(i = 0; i < nSTRINGS; i++){
		printf("STRING[%d]: %s\n", i, STRING[i]);
	}

	for(i = 0; i < nSYMBOLS; i++){
		printf("SYMBOL[%d]: %d\n", i, SYMBOL[i]);
	}


	for(i = 0; i < nRULES; i++){
		printf("%2d: %s ->  ", i, STRING[RULENAME[i]]);
		for(j = 0; j < RULESIZE[i]; j++){
			printf(" %s", STRING[RULE[i][j]]);
		}
		printf("\n");
	}
	printf("\n");
}


int areAllNullable(int rule, int start, int stop){
	int i;

	// assert(start >= 0);
	// assert(start < RULESIZE[rule]);
	// assert(stop >= 0);
	// assert(stop < RULESIZE[rule]);

	printf("areAllNullable(%2d, %2d, %2d): ", rule, start, stop);
	for(i = start; i <= stop; i++){
		if(!NULLABLE[RULE[rule][i]]){
			printf("0\n");
			return 0;
		}
	}
	printf("1\n");
	return 1;
}

void computeNullable(){
	int rule, i, j;
	int changed;
	int iteration;

	for(i = 0; i < nSYMBOLS; i++){
		if(SYMBOLTYPE[i] == TYPE_TERMINAL){
			FIRST[i][0] = i;
			FIRSTSIZE[i] = 1;
		}
	}

	iteration = 1;
	changed = 1;
	while(changed){
		printf("    *** ITERATION: %d ***\n", iteration);
		changed = 0;
		for(rule = 0; rule < nRULES; rule++){
			if(NULLABLE[RULENAME[rule]] == 0){
				if(areAllNullable(rule, 0, RULESIZE[rule] - 1)){
					NULLABLE[RULENAME[rule]] = 1;
					changed = 1;
				}
			}
		}
		iteration += 1;
	}
	printf("\n");
}

int unionFirst(int dst, int src){
	int i, j;
	int changed = 0;

	printf("FIRST[%s] <= FIRST[%s] U FIRST[%s]\n",
		       STRING[SYMBOL[dst]],
		       STRING[SYMBOL[dst]],
		       STRING[SYMBOL[src]]);
	for(i = 0; i < FIRSTSIZE[src]; i++){
		int symbol = FIRST[src][i];

		for(j = 0; j < FIRSTSIZE[dst]; j++){
			if(FIRST[dst][j] == symbol){
				break;
			}
		}
		if(j == FIRSTSIZE[dst]){
			assert(FIRSTSIZE[dst] < MAX_FIRST_SET_SIZE);
			FIRST[dst][j] = symbol;
			FIRSTSIZE[dst] += 1;
			changed = 1;
		}
	}
	return changed;
}

void computeFirst(){
	int rule, i, j;
	int changed;
	int iteration = 1;

	do {
		printf("    *** ITERATION: %d ***\n", iteration);
		changed = 0;

		for(rule = 0; rule < nRULES; rule++){
			printf("RULE: %d\n", rule);
			for(i = 0; i < RULESIZE[rule]; i++){
				if(unionFirst(RULENAME[rule], RULE[rule][i])){
					changed = 1;
				}
				if(!NULLABLE[RULE[rule][i]]){
					break;
				}
			}
		}
		iteration += 1;
	} while(changed);
}


int unionFollow(int dst, int src){
	int i,j;
	int changed = 0;

	printf("FOLLOW[%s] <= FOLLOW[%s] U FOLLOW[%s]\n",
		       STRING[SYMBOL[dst]],
		       STRING[SYMBOL[dst]],
		       STRING[SYMBOL[src]]);

	for(i = 0; i < FOLLOWSIZE[src]; i++){
		int symbol = FOLLOW[src][i];

		for(j = 0; j < FOLLOWSIZE[dst]; j++){
			if(FOLLOW[dst][j] == symbol){
				break;
			}
		}
		if(j == FOLLOWSIZE[dst]){
			assert(FOLLOWSIZE[dst] < MAX_FOLLOW_SET_SIZE);
			FOLLOW[dst][j] = symbol;
			FOLLOWSIZE[dst] += 1;
			changed = 1;
		}
	}
	return changed;
}


int unionFollowFirst(int dst, int src){
	int i,j;
	int changed = 0;

	printf("FOLLOW[%s] <= FOLLOW[%s] U FIRST[%s]\n",
		       STRING[SYMBOL[dst]],
		       STRING[SYMBOL[dst]],
		       STRING[SYMBOL[src]]);

	for(i = 0; i < FIRSTSIZE[src]; i++){
		int symbol = FIRST[src][i];

		for(j = 0; j < FOLLOWSIZE[dst]; j++){
			if(FOLLOW[dst][j] == symbol){
				break;
			}
		}
		if(j == FOLLOWSIZE[dst]){
			assert(FOLLOWSIZE[dst] < MAX_FOLLOW_SET_SIZE);
			FOLLOW[dst][j] = symbol;
			FOLLOWSIZE[dst] += 1;
			changed = 1;
		}
	}
	return changed;
}

void computeFollow(){
	int rule, i, j;
	int changed;
	int iteration = 1;

	do {
		printf("    *** ITERATION: %d ***\n", iteration);
		changed = 0;

		for(rule = 0; rule < nRULES; rule++){
	
			for(i = RULESIZE[rule] - 1; i >= 0; i--){
				printf("RULE: %d I: %d size: %d\n", rule, i, RULESIZE[rule]);
				if(unionFollow(RULE[rule][i], RULENAME[rule])){
					changed = 1;
				}
				if(!NULLABLE[RULE[rule][i]]){
					break;
				}
			}


			for(i = 0; i < RULESIZE[rule]; i++){
				for(j = i+1; j < RULESIZE[rule]; j++){
					if(unionFollowFirst(RULE[rule][i], RULE[rule][j])){
						changed = 1;
					}
					if(!NULLABLE[RULE[rule][j]]){
						break;
					}
				}
			}
		}
	} while(changed);

}

void printOutput(){
	int i, j;

	for(i = 0; i < nSYMBOLS; i++){
		printf("NULLABLE(%s) => %s\n", STRING[SYMBOL[i]], NULLABLE[i] ? "YES" : "NO");
	}

	printf("\n");

	for(i = 0; i < nSYMBOLS; i++){
		printf("FIRST(%s) => ", STRING[SYMBOL[i]]);
		for(j = 0; j < FIRSTSIZE[i]; j++){
			printf(" %s", STRING[FIRST[i][j]]);
		}
		printf("\n");
	}
	
	printf("\n");
	
	for(i = 0; i < nSYMBOLS; i++){
		printf("FOLLOW(%s) => ", STRING[SYMBOL[i]]);
		for(j = 0; j < FOLLOWSIZE[i]; j++){
			printf(" %s", STRING[FOLLOW[i][j]]);
		}
		printf("\n");
	}
	printf("\n");
}

int main(int argc, char *argv[]){
	loadInput_3_12();
	//loadInput_3_15();
	printf("*********** INPUT **************\n");
	printInput();
	printf("*********** COMPUTE NULLABLE **************\n");
	computeNullable();
	printf("*********** COMPUTE FIRST **************\n");
	computeFirst();
	printf("*********** COMPUTE FOLLOW **************\n");
	computeFollow();
	printf("*********** OUTPUT **************\n");
	printOutput();
	return 0;
}
