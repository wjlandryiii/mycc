/*
 * Copyright 2015 Joseph Landry All Rights Reserved
 */

#ifndef PARSER_H
#define PARSER_H

#include "defs.h"

extern int nRULES;
extern int RULENAME[MAX_RULES];
extern int RULE[MAX_RULES][MAX_RULE_SIZE];
extern int RULESIZE[MAX_RULES];

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
