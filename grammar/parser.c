/*
 * Copyright 2015 Joseph Landry All Rights Reserved
 */

#include "lexer.h"
#include "symbols.h"
#include "parser.h"

#include <stdio.h>
#include <stdlib.h>

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

	if(tokenStream[tokenIndex].name == TOKNAME_TERMINAL
			|| tokenStream[tokenIndex].name == TOKNAME_NONTERMINAL
			|| tokenStream[tokenIndex].name == TOKNAME_SIGMA){

		if(tokenStream[tokenIndex].name != TOKNAME_SIGMA){
			if(termCount >= sizeof(terms)/sizeof(*terms)){
				// TODO: set parse error
				return 0;
			}
			t = &terms[termCount++];
			if(tokenStream[tokenIndex].name == TOKNAME_TERMINAL){
				t->type = TERMTYPE_TERMINAL;
				t->index = tokenStream[tokenIndex].terminalIndex;
			} else if(tokenStream[tokenIndex].name == TOKNAME_NONTERMINAL){
				t->type = TERMTYPE_NONTERMINAL;
				t->index = tokenStream[tokenIndex].nonterminalIndex;
			}
		}

		tokenIndex++;

		if(tokenStream[tokenIndex].name == TOKNAME_TERMINAL
				|| tokenStream[tokenIndex].name == TOKNAME_NONTERMINAL){
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

static int ruleList(int nonterminalIndex){
	struct term *t = 0;
	struct rule *r = 0;
	int length;
	int result;


	if(ruleCount >= sizeof(rules)/sizeof(*rules)){
		// TODO: parser error
		return -1;
	}
	r = &rules[ruleCount++];
	r->nonterminalIndex = nonterminalIndex;

	result = termList(&t, &length);
	if(result){
		return result;
	}
	r->bodyLength = length;
	r->body = t;

	if(tokenStream[tokenIndex].name == TOKNAME_PIPE){
		tokenIndex++;
		result = ruleList(nonterminalIndex);
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

	if(tokenStream[tokenIndex].name == TOKNAME_NONTERMINAL){
		nonterminalIndex = tokenStream[tokenIndex].nonterminalIndex;
		tokenIndex++;

		if(tokenStream[tokenIndex].name == TOKNAME_COLON){
			tokenIndex++;
			result = ruleList(nonterminalIndex);
			if(result){
				return result;
			}

			if(tokenStream[tokenIndex].name == TOKNAME_SEMICOLON){
				tokenIndex++;

				if(tokenStream[tokenIndex].name == TOKNAME_NONTERMINAL){
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

	if(tokenStream[tokenIndex].name == TOKNAME_EOF){
		return 0;
	} else {
		// TODO: parser error
		return -1;
	}
}
