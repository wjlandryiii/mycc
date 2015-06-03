/*
 * Copyright 2015 Joseph Landry All Rights Reserved
 */

#include "lexer.h"
#include "symbols.h"
#include "parser.h"

#include <stdlib.h>

struct rule_set;
struct rule_list;
struct term_list;

struct rule_set {
	int ruleNameSymbol;
	struct rule_list *ruleList;
	struct rule_set *nextRuleSet;
};

struct rule_list {
	struct term_list *termList;
	struct rule_list *nextRuleList;
};

struct term_list {
	int termSymbol;
	struct term_list *nextTermList;
};


extern struct rule_set *parseTree;

// TODO: stop from .h file


static struct term terms[1024];
int termCount;

struct rule rules[256];
int ruleCount;

int parserErrorNumber = 0;

struct rule_set *parseTree = 0;

static int tokenIndex;

static struct term_list *termList(struct term **termListOut, int *lengthOut){
	struct term_list *tl = malloc(sizeof(struct term_list));
	struct term *t = 0;

	if(tokenStream[tokenIndex].name == TOKNAME_TERMINAL
			|| tokenStream[tokenIndex].name == TOKNAME_NONTERMINAL
			|| tokenStream[tokenIndex].name == TOKNAME_SIGMA){
		tl->termSymbol = tokenStream[tokenIndex].symbol;

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
			tl->nextTermList = termList(termListOut, lengthOut);
			if(t){
				*termListOut = t;
				*lengthOut = *lengthOut + 1;
			}
			return tl;
		} else {
			if(t){
				*lengthOut = 1;
				*termListOut = t;
			}
			tl->nextTermList = 0;
			return tl;
		}
	} else {
		parserErrorNumber = PARSERR_EXPECTED_TERMINAL_NONTERMINAL;
		return 0;
	}
}

static struct rule_list *ruleList(int nonterminalIndex){
	struct rule_list *rl = malloc(sizeof(struct rule_list));
	struct term *t = 0;
	struct rule *r = 0;
	int length;


	if(ruleCount >= sizeof(rules)/sizeof(*rules)){
		// TODO: parser error
		return 0;
	}
	r = &rules[ruleCount++];
	r->nonterminalIndex = nonterminalIndex;

	rl->termList = termList(&t, &length);
	r->bodyLength = length;
	r->body = t;

	if(tokenStream[tokenIndex].name == TOKNAME_PIPE){
		tokenIndex++;
		rl->nextRuleList = ruleList(nonterminalIndex);
		return rl;
	} else {
		rl->nextRuleList = 0;
		return rl;
	}
}

static struct rule_set *ruleSet(){
	struct rule_set *rs = malloc(sizeof(struct rule_set));
	int nonterminalIndex = -1;

	if(tokenStream[tokenIndex].name == TOKNAME_NONTERMINAL){
		rs->ruleNameSymbol = tokenStream[tokenIndex].symbol;
		nonterminalIndex = tokenStream[tokenIndex].nonterminalIndex;
		tokenIndex++;

		if(tokenStream[tokenIndex].name == TOKNAME_COLON){
			tokenIndex++;
			rs->ruleList = ruleList(nonterminalIndex);

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
