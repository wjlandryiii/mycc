/*
 * Copyright 2015 Joseph Landry All Rights Reserved
 */

/*
 * From ISO 9899 Draft 2007:
 * Preprocessing directives are executed, macro invocations are expanded, and
 * *Pragma_* unary operators are executed. ...  A #include preprocessing
 *  directive causes the named header or source file to be processed from phase 1
 *  to phase 4 recursively.  All preprocessing directives are deleted.
 */

/*
 * Grammar:

<preprocessing-file>
	: <group>
	| {}
	;

<group>
	: <group-part>
	| <group> <group-part>
	;

<group-part>
	: <if-section>
	| <control-line>
	| <text-line>
	| "#" <non-directive>
	;

<if-section>
	: <if-group> <elif-groups>? <else-group>? <endif-line>
	;

<if-group>
	: "#" "if" <constant-expression> "\n" <group>?
	| "#" "ifdef" "%IDENTIFIER%" "\n" <group>?
	| "#" "ifndef"  "%IDENTIFIER%" "\n" <group>?
	;

<elif-groups>
	: <elif-group>
	| <elif-groups> <elif-group>
	;

<elif-group>
	: "#" "elif" <constant-expression> "\n" <group>?
	;

<else-group>
	: "#" "else" "\n" <group>?
	;

<endif-line>
	: "#" "endif" "\n"
	;

<control-line>
	: "#" "include" <pp-tokens> "\n"
	| "#" "define" "%IDENTIFIER%" <replacement-list> "\n"
	| "#" "define" "%IDENTIFIER%" "(" <identifier-list>? ")" <replacement-list> "\n"
	| "#" "define" "%IDENTIFIER%" "(" "..." ")" <replacement-list> "\n"
	| "#" "define" "%IDENTIFIER%" "(" <identifier-list> "," "..." ")" <replacement-list> "\n"
	| "#" "undef" "%IDENTIFIER%" "\n"
	| "#" "line" <pp-tokens> "\n"
	| "#" "error" <pp-tokens>? "\n"
	| "#" "pragma" <pp-tokens>? "\n"
	| "#" "\n"
	;

<text-line>
	: <pp-tokens>? "\n"
	;

<non-directive>
	: <pp-tokens> "\n"
	;

<lparen>
	: "(" # not immediatly preceeded by white space.
	;

<replacement-list>
	: <pp-tokens>?
	;

<pp-tokens>
	: <preprocessing-token>
	| <pp-tokens> <preprocessing-token>
	;

<new-line>
	: "\n"
	;


*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>


#include "phase4.h"
#include "phase3.h"
#include "tokens.h"
#include "statestack.h"

enum SIMPLE_STATES {
	START_LINE,
	TEXT_LINE,
};

int phase4Init(struct phase4 *p4){
	p4->stateStack = newStateStack();
	stateStackPush(p4->stateStack, START_LINE);
	p4->lookAhead[0] = phase3NextToken(p4->p3);
	p4->lookAhead[1] = phase3NextToken(p4->p3);
	p4->lookAhead[2] = phase3NextToken(p4->p3);
	p4->lookAhead[3] = phase3NextToken(p4->p3);
	return 0;
}

static struct pptoken eat(struct phase4 *p4){
	struct pptoken token;

	token = p4->lookAhead[0];
	p4->lookAhead[0] = p4->lookAhead[1];
	p4->lookAhead[1] = p4->lookAhead[2];
	p4->lookAhead[2] = p4->lookAhead[3];
	p4->lookAhead[3] = phase3NextToken(p4->p3);
	return token;
}

static int isLineIfSection(struct pptoken_list *lineList){
	/*
	 <if-section>
		: "#" "if" <constant-expression> "\n" <group>?
		| "#" "ifdef" "%IDENTIFIER%" "\n" <group>?
		| "#" "ifndef"  "%IDENTIFIER%" "\n" <group>?
		;
	*/

	struct pptoken token;
	int i;

	i = 0;

	// TODO: left off here
	//ppTokenListAtIndex(list, i

	return 0;
}


struct pptoken phase4NextToken(struct phase4 *p4){
	struct pptoken token;

	int state = stateStackPop(p4->stateStack);

nextState:
	if(state == START_LINE){
		goto startLine;
	} else if(state == TEXT_LINE){
		goto textLine;
	} else {
		fprintf(stderr, "unknown next state\n");
		exit(1);
	}

startLine:
	struct pptoken_list *lineList;
	lineList = newPPTokenList();

	token = eat();
	while(token.name != LL_NEWLINE && token.name != LT_EMTPY){
		ppTokenListAppend(lineList, token);
		token = eat();
	}

	if(isLineIfSection(lineList)){

	} else if(isLineControlLine(lineList)){

	} else if(isLineTextLine(lineList)){

	} else if(isLineNonDirective(lineList)){

	} else {
		fprintf(stderr, "unknown line type\n");
		exit(1);
	}


textLine:
	token = eat(p4);
	if(token.name != PPTN_NEWLINE){
		stateStackPush(p4->stateStack, TEXT_LINE);
		return token;
	} else {
		stateStackPush(p4->stateStack, START_LINE);
		return token;
	}

	return token;
}




#ifdef TESTING

#include "phase1.h"
#include "phase2.h"

int main(int argc, char *argv[]){
	struct phase1 p1;
	struct phase2 p2;
	struct phase3 p3;
	struct phase4 p4;
	struct pptoken token;

	p1.sourceFile = fopen("tests/defines.c", "r");
	phase1Init(&p1);

	p2.p1 = &p1;
	phase2Init(&p2);

	p3.p2 = &p2;
	phase3Init(&p3);

	p4.p3 = &p3;
	phase4Init(&p4);

	token = phase4NextToken(&p4);
	while(token.type != LT_EMPTY){
		printf("%s", token.lexeme);
		token = phase4NextToken(&p4);
	}
	return 0;
}
#endif

