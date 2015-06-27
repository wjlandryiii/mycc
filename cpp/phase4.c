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

#include "phase3.h"

enum SIMPLE_STATES {
	SS_GROUPPART,
	SS_IFSECTION,
	SS_CONTROLSECTION,
	SS_TEXTLINE,
	SS_NONDIRECTIVE,
};

int phase4Init(struct phase4 *p4){
	p4->state = SS_GROUPPART;
	return 0;
}

struct token phase4NextToken(struct phase4 *p4){
	struct token token;

	switch(p4->state){
	case SS_GROUPPART:
	case SS_IFSECTION:
	case SS_CONTROLSECTION:
	case SS_TEXTLINE:
	case SS_NONDIRECTIVE:
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
	struct token token;

	p1.sourceFile = fopen("tests/defines.c", "r");
	phase1Init(&p1);

	p2.p1 = &p1;
	phase2Init(&p2);

	p3.p2 = &p2;
	phase3Init(&p3);

	token = phase3NextToken(&p3);
	while(token.type != LT_EMPTY){
		printf("LEXEME:%s\n", token.lexeme);
		token = phase3NextToken(&p3);
	}
	return 0;
}
#endif

