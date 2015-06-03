/*
 * Copyright 2015 Joseph Landry All Rights Reserved
 */

#ifndef PARSER_H
#define PARSER_H

enum PARSER_TERMTYPES{
	TERMTYPE_UNKNOWN = 0,
	TERMTYPE_TERMINAL,
	TERMTYPE_NONTERMINAL,
};

struct term {
	int type;
	int index;
};

struct rule {
	int nonterminalIndex;
	int bodyLength;
	struct term *body;
};

extern struct rule rules[256];
extern int ruleCount;

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

enum PARSER_ERRORS {
	PARSERR_EXPECTED_NONTERMINAL,
	PARSERR_EXPECTED_TERMINAL,
	PARSERR_EXPECTED_TERMINAL_NONTERMINAL,
	PARSERR_EXPECTED_COLON,
	PARSERR_EXPECTED_SEMICOLON,
};

extern struct rule_set *parseTree;
extern int parserErrorNumber;

int parse(void);


#endif
