/*
 * Copyright 2014 Joseph Landry All Rights Reserved
 */

#ifndef PHASE4_H
#define PHASE4_H

#include "tokens.h"

/*

\n
(
)
...
,
#

if

elif
else
line

endif
error
ifdef

define
ifndef
pragma

include

%HEADERNAME%
%IDENTIFIER%
%PPNUMBER%
%CHARACTERCONSTANT%
%STRINGLITERAL%
%PUNCTUATOR%
%WHITESPACE%
*/



struct phase3;
struct pptoken;
struct state_stack;
struct list;

struct phase4 {
	struct phase3 *p3;
	struct state_stack *stateStack;
	struct pptoken lookAhead[4];
	struct hash_table *symbolTable;
	struct list *tokenQueue;
};


int phase4Init(struct phase4 *p4);
struct pptoken phase4NextToken(struct phase4 *p4);

#endif
