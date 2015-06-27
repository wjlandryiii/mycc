/*
 * Copyright 2014 Joseph Landry All Rights Reserved
 */

#ifndef PHASE4_H
#define PHASE4_H

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
struct token;

struct phase4 {
	struct phase3 *p3;
	int state;
};


int phase4Init(struct phase4 *p4);
struct token phase4NextToken(struct phase4 *p4);

#endif
