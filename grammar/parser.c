/*
 * Copyright 2015 Joseph Landry All Rights Reserved
 */

#include "lexer.h"
#include "symbols.h"
#include "parser.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdarg.h>


int nRULES = 0;
int RULENAME[MAX_RULES];
int RULE[MAX_RULES][MAX_RULE_SIZE];
int RULESIZE[MAX_RULES];



int parserErrorNumber = 0;

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



static int tokenIndex;


static int termList(){
	int result;

	if(TOKENNAME[tokenIndex] == TOKNAME_TERMINAL
			|| TOKENNAME[tokenIndex] == TOKNAME_NONTERMINAL
			|| TOKENNAME[tokenIndex] == TOKNAME_SIGMA){

		if(TOKENNAME[tokenIndex] != TOKNAME_SIGMA){
			if(TOKENNAME[tokenIndex] == TOKNAME_TERMINAL){
				assert(RULESIZE[nRULES] < MAX_RULE_SIZE);
				RULE[nRULES][RULESIZE[nRULES]] = TOKENVALUE[tokenIndex];
				RULESIZE[nRULES]++;
			} else if(TOKENNAME[tokenIndex] == TOKNAME_NONTERMINAL){
				assert(RULESIZE[nRULES] < MAX_RULE_SIZE);
				RULE[nRULES][RULESIZE[nRULES]] = TOKENVALUE[tokenIndex];
				RULESIZE[nRULES]++;
			}
		}

		tokenIndex++;

		if(TOKENNAME[tokenIndex] == TOKNAME_TERMINAL
				|| TOKENNAME[tokenIndex]== TOKNAME_NONTERMINAL){
			result = termList();
			if(result){
				return result;
			}
			return 0;
		} else {
			return 0;
		}
	} else {
		parserErrorNumber = PARSERR_EXPECTED_TERMINAL_NONTERMINAL;
		return -1;
	}
}

static int ruleList(int symbolIndex){
	int length;
	int result;

	assert(nRULES < MAX_RULES);
	RULENAME[nRULES] = symbolIndex;


	result = termList();
	nRULES += 1;

	if(result){
		return result;
	}

	if(TOKENNAME[tokenIndex] == TOKNAME_PIPE){
		tokenIndex++;
		result = ruleList(symbolIndex);
		if(result){
			return result;
		}
		return 0;
	} else {
		return 0;
	}
}

static int ruleSet(){
	int result;
	int symbolIndex;

	if(TOKENNAME[tokenIndex] == TOKNAME_NONTERMINAL){
		symbolIndex = TOKENVALUE[tokenIndex];
		tokenIndex++;

		if(TOKENNAME[tokenIndex] == TOKNAME_COLON){
			tokenIndex++;
			result = ruleList(symbolIndex);
			if(result){
				return result;
			}

			if(TOKENNAME[tokenIndex] == TOKNAME_SEMICOLON){
				tokenIndex++;

				if(TOKENNAME[tokenIndex] == TOKNAME_NONTERMINAL){
					return ruleSet();
				} else {
					return 0;
				}
			} else {
				parserErrorNumber = PARSERR_EXPECTED_SEMICOLON;
				return -1;
			}

		} else {
			parserErrorNumber = PARSERR_EXPECTED_COLON;
			return -1;
		}
	} else {
		parserErrorNumber = PARSERR_EXPECTED_NONTERMINAL;
		return -1;
	}
}

int parse(void){
	int result;

	tokenIndex = 0;
	result = ruleSet();
	if(result){
		printf("Token Index: %d\n", tokenIndex);
		return result;
	}

	if(TOKENNAME[tokenIndex] == TOKNAME_EOF){
		return 0;
	} else {
		// TODO: parser error
		return -1;
	}
}
