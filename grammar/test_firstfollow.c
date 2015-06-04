/*
 * Copyright 2015 Joseph Landry All Rights Reserved
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <assert.h>

#include "firstfollow.h"


#define MAX_STRINGS (128)
int nSTRINGS = 0;
char *STRING[MAX_STRINGS];


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
	//loadInput_3_12();
	loadInput_3_15();
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
