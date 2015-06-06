/*
 * Copyright 2015 Joseph Landry All Rights Reserved
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "stringtable.h"
#include "symbols.h"
#include "parser.h"
#include "firstfollow.h"


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

	int X = insertSymbol(insertString("X"), SYMBOLTYPE_NONTERMINAL);
	int Y = insertSymbol(insertString("Y"), SYMBOLTYPE_NONTERMINAL);
	int Z = insertSymbol(insertString("Z"), SYMBOLTYPE_NONTERMINAL);
	int a = insertSymbol(insertString("a"), SYMBOLTYPE_TERMINAL);
	int b = insertSymbol(insertString("b"), SYMBOLTYPE_TERMINAL);
	int c = insertSymbol(insertString("c"), SYMBOLTYPE_TERMINAL);
	int d = insertSymbol(insertString("d"), SYMBOLTYPE_TERMINAL);

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
	int S  = insertSymbol(insertString("S"),  SYMBOLTYPE_NONTERMINAL);
	int E  = insertSymbol(insertString("E"),  SYMBOLTYPE_NONTERMINAL);
	int Ep = insertSymbol(insertString("Ep"), SYMBOLTYPE_NONTERMINAL);
	int T  = insertSymbol(insertString("T"),  SYMBOLTYPE_NONTERMINAL);
	int Tp = insertSymbol(insertString("Tp"), SYMBOLTYPE_NONTERMINAL);
	int F  = insertSymbol(insertString("F"),  SYMBOLTYPE_NONTERMINAL);

	int plus    = insertSymbol(insertString("+"),   SYMBOLTYPE_TERMINAL);
	int minus   = insertSymbol(insertString("-"),   SYMBOLTYPE_TERMINAL);
	int times   = insertSymbol(insertString("*"),   SYMBOLTYPE_TERMINAL);
	int divide  = insertSymbol(insertString("/"),   SYMBOLTYPE_TERMINAL);
	int id      = insertSymbol(insertString("id"),  SYMBOLTYPE_TERMINAL);
	int num     = insertSymbol(insertString("num"), SYMBOLTYPE_TERMINAL);
	int popen   = insertSymbol(insertString("("),   SYMBOLTYPE_TERMINAL);
	int pclose  = insertSymbol(insertString(")"),   SYMBOLTYPE_TERMINAL);
	int dollar  = insertSymbol(insertString("$"),   SYMBOLTYPE_TERMINAL);

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
	printf("*********** COMPUTE  **************\n");
	firstfollow();
	printf("*********** OUTPUT **************\n");
	printOutput();
	return 0;
}
