/*
 * Copyright 2015 Joseph Landry All Rights Reserved
 */

#include "lexer.h"
#include "symbols.h"
#include "parser.h"

#include <stdlib.h>

int parserErrorNumber = 0;

struct rule_set *parseTree = 0;

static int tokenIndex;

static struct term_list *termList(){
	struct term_list *tl = malloc(sizeof(struct term_list));

	if(tokenStream[tokenIndex].name == TOKNAME_TERMINAL
			|| tokenStream[tokenIndex].name == TOKNAME_NONTERMINAL){
		tl->termSymbol = tokenStream[tokenIndex].symbol;
		tokenIndex++;

		if(tokenStream[tokenIndex].name == TOKNAME_TERMINAL
				|| tokenStream[tokenIndex].name == TOKNAME_NONTERMINAL){
			tl->nextTermList = termList();
			return tl;
		} else {
			tl->nextTermList = NULL;
			return tl;
		}
	} else {
		parserErrorNumber = PARSERR_EXPECTED_TERMINAL_NONTERMINAL;
		return 0;
	}
}

static struct rule_list *ruleList(){
	struct rule_list *rl = malloc(sizeof(struct rule_list));

	rl->termList = termList();

	if(tokenStream[tokenIndex].name == TOKNAME_PIPE){
		tokenIndex++;
		rl->nextRuleList = ruleList();
		return rl;
	} else {
		rl->nextRuleList = 0;
		return rl;
	}
}

static struct rule_set *ruleSet(){
	struct rule_set *rs = malloc(sizeof(struct rule_set));

	if(tokenStream[tokenIndex].name == TOKNAME_NONTERMINAL){
		rs->ruleNameSymbol = tokenStream[tokenIndex].symbol;
		tokenIndex++;

		if(tokenStream[tokenIndex].name == TOKNAME_COLON){
			tokenIndex++;
			rs->ruleList = ruleList();

			if(tokenStream[tokenIndex].name == TOKNAME_SEMICOLON){
				tokenIndex++;

				if(tokenStream[tokenIndex].name == TOKNAME_NONTERMINAL){
					rs->nextRuleSet = ruleSet();
					return rs;
				} else {
					rs->nextRuleSet = NULL;
					return rs;
				}
			} else {
				parserErrorNumber = PARSERR_EXPECTED_SEMICOLON;
				return 0;
			}

		} else {
			parserErrorNumber = PARSERR_EXPECTED_COLON;
			return 0;
		}
	} else {
		parserErrorNumber = PARSERR_EXPECTED_NONTERMINAL;
		return 0;
	}
}

int parse(void){
	struct rule_set *root;

	tokenIndex = 0;
	root = ruleSet();

	if(tokenStream[tokenIndex].name == TOKNAME_EOF){
		parseTree = root;
		return 0;
	} else {
		return -1;
	}
}
