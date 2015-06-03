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

enum PARSER_ERRORS {
	PARSERR_EXPECTED_NONTERMINAL,
	PARSERR_EXPECTED_TERMINAL,
	PARSERR_EXPECTED_TERMINAL_NONTERMINAL,
	PARSERR_EXPECTED_COLON,
	PARSERR_EXPECTED_SEMICOLON,
};
extern int parserErrorNumber;

int parse(void);


#endif
