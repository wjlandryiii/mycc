/*
 * Copyright 2015 Joseph Landry All Rights Reserved
 */

#include "lexer.h"
#include "symbols.h"
#include "parser.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>


int nRULES = 0;
int RULENAME[MAX_RULES];
int RULE[MAX_RULES][MAX_RULE_SIZE];
int RULESIZE[MAX_RULES];


static struct term terms[1024];
int termCount;

struct rule rules[256];
int ruleCount;

int parserErrorNumber = 0;

struct rule_set *parseTree = 0;

static int tokenIndex;


static int termList(struct term **termListOut, int *lengthOut){
	int result;
	struct term *t = 0;

	if(TOKENNAME[tokenIndex] == TOKNAME_TERMINAL
			|| TOKENNAME[tokenIndex] == TOKNAME_NONTERMINAL
			|| TOKENNAME[tokenIndex] == TOKNAME_SIGMA){

		if(TOKENNAME[tokenIndex] != TOKNAME_SIGMA){
			if(termCount >= sizeof(terms)/sizeof(*terms)){
				// TODO: set parse error
				return 0;
			}
			t = &terms[termCount++];
			if(TOKENNAME[tokenIndex] == TOKNAME_TERMINAL){
				t->type = TERMTYPE_TERMINAL;
				t->index = tokenStream[tokenIndex].terminalIndex;

				assert(RULESIZE[nRULES] < MAX_RULE_SIZE);
				RULE[nRULES][RULESIZE[nRULES]] = TOKENVALUE[tokenIndex];
				RULESIZE[nRULES]++;
			} else if(TOKENNAME[tokenIndex] == TOKNAME_NONTERMINAL){
				t->type = TERMTYPE_NONTERMINAL;
				t->index = tokenStream[tokenIndex].nonterminalIndex;

				assert(RULESIZE[nRULES] < MAX_RULE_SIZE);
				RULE[nRULES][RULESIZE[nRULES]] = TOKENVALUE[tokenIndex];
				RULESIZE[nRULES]++;
			}
		}

		tokenIndex++;

		if(TOKENNAME[tokenIndex] == TOKNAME_TERMINAL
				|| TOKENNAME[tokenIndex]== TOKNAME_NONTERMINAL){
			result = termList(termListOut, lengthOut);
			if(t){
				*lengthOut = *lengthOut + 1;
				*termListOut = t;
			}
			if(result){
				return result;
			}
			return 0;
		} else {
			if(t){
				*lengthOut = 1;
				*termListOut = t;
			} else {
				*lengthOut = 0;
				*termListOut = 0;
			}
			return 0;
		}
	} else {
		parserErrorNumber = PARSERR_EXPECTED_TERMINAL_NONTERMINAL;
		return -1;
	}
}

static int ruleList(int nonterminalIndex, int symbolIndex){
	struct term *t = 0;
	struct rule *r = 0;
	int length;
	int result;

	assert(nRULES < MAX_RULES);
	RULENAME[nRULES] = symbolIndex;


	if(ruleCount >= sizeof(rules)/sizeof(*rules)){
		// TODO: parser error
		return -1;
	}
	r = &rules[ruleCount++];
	r->nonterminalIndex = nonterminalIndex;

	result = termList(&t, &length);
	RULESIZE[nRULES] = length;
	nRULES += 1;

	if(result){
		return result;
	}
	r->bodyLength = length;
	r->body = t;

	if(TOKENNAME[tokenIndex] == TOKNAME_PIPE){
		tokenIndex++;
		result = ruleList(nonterminalIndex, symbolIndex);
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
	int nonterminalIndex = -1;
	int symbolIndex;

	if(TOKENNAME[tokenIndex] == TOKNAME_NONTERMINAL){
		nonterminalIndex = tokenStream[tokenIndex].nonterminalIndex;
		symbolIndex = TOKENVALUE[tokenIndex];
		tokenIndex++;

		if(TOKENNAME[tokenIndex] == TOKNAME_COLON){
			tokenIndex++;
			result = ruleList(nonterminalIndex, symbolIndex);
			if(result){
				return result;
			}

			if(TOKENNAME[tokenIndex] == TOKNAME_SEMICOLON){
				tokenIndex++;

				if(TOKENNAME[tokenIndex] == TOKNAME_NONTERMINAL){
					//rs->nextRuleSet = ruleSet();
					return ruleSet();
				} else {
					//rs->nextRuleSet = NULL;
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
