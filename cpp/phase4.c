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


/*
# <non-directive>

#if <constant-expression> "\n" <group>?
#ifdef "%IDENTIFIER%" "\n" <group>?
#ifndef  "%IDENTIFIER%" "\n" <group>?
#elif <constant-expression> "\n" <group>?
#else "\n" <group>?
#endif "\n"
#include <pp-tokens> "\n"
#define "%IDENTIFIER%" <replacement-list> "\n"
#define "%IDENTIFIER%" "(" <identifier-list>? ")" <replacement-list> "\n"
#define "%IDENTIFIER%" "(" "..." ")" <replacement-list> "\n"
#define "%IDENTIFIER%" "(" <identifier-list> "," "..." ")" <replacement-list> "\n"
#undef "%IDENTIFIER%" "\n"
#line <pp-tokens> "\n"
#error <pp-tokens>? "\n"
#pragma <pp-tokens>? "\n"
#\n
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>


#include "phase4.h"
#include "phase3.h"
#include "tokens.h"
#include "statestack.h"
#include "hashtable.h"
#include "list.h"
#include "set.h"

enum MACRO_TYPES {
	OBJECT_MACRO,
	FUNCTION_MACRO,
};

struct macro_entry {
	char *name;
	int type;
	struct pptoken_list *replacementList;
};

struct repl_token {
	struct pptoken token;
	struct string_set *macroSet;
};


enum SIMPLE_STATES {
	START_LINE,
	TEXT_LINE,
};

int phase4Init(struct phase4 *p4){
	p4->tokenQueue = newList();
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

static struct pptoken ppDefineLine(struct phase4 *p4){
	/*
	#define ID <replacement-list>\n
	#define ID( <identifier-list>? ")" <replacement-list>\n
	#define ID(...) <replacement-list>\n"
	#define ID( <identifier-list>, ...) <replacement-list>\n
	*/

	struct pptoken nameToken;
	struct pptoken_list *identifierList;
	struct pptoken_list *replacementList;
	struct macro_entry *macroEntry;
	int found;

	if(p4->lookAhead[0].name == PPTN_HASH){
		eat(p4);
	} else {
		fprintf(stderr, "define line didn't begin with '#'\n");
		exit(1);
	}
	if(p4->lookAhead[0].name == PPTN_DEFINE){
		eat(p4);
	} else {
		fprintf(stderr, "define line didn't have 'define' keyword\n");
		exit(1);
	}
	if(p4->lookAhead[0].name == PPTN_IDENTIFIER){
		nameToken = eat(p4);
	}

	if(p4->lookAhead[0].name == PPTN_LPAREN
			&& (p4->lookAhead[0].whiteSpace == NULL
				|| strlen(p4->lookAhead[0].whiteSpace) == 0)){
		fprintf(stderr, "function style macros not implemented yet!\n");
		exit(1);
	} else {
		replacementList = newPPTokenList();
		while(p4->lookAhead[0].name != PPTN_NEWLINE && p4->lookAhead[0].name != PPTN_EOF){
			ppTokenListAppend(replacementList, eat(p4));
		}

		hashTableGetValue(p4->symbolTable, nameToken.lexeme, (void**)&macroEntry, &found);
		if(!found){
			macroEntry = calloc(1, sizeof(struct macro_entry));
			assert(macroEntry != NULL);
			macroEntry->name = nameToken.lexeme;
			macroEntry->type = OBJECT_MACRO;
			macroEntry->replacementList = replacementList;
			hashTableSetValue(p4->symbolTable, nameToken.lexeme, macroEntry);
		} else {
			fprintf(stderr, "macro already defined! %s\n", nameToken.lexeme);
			exit(1);
		}
	}
	fprintf(stderr, "DEFINE: %s\n", nameToken.lexeme);
	return eat(p4);
}

static struct pptoken ppUndefLine(struct phase4 *p4){
	struct pptoken nameToken;
	struct macro_entry *macroEntry;
	int found;

	if(p4->lookAhead[0].name == PPTN_HASH){
		eat(p4);
	} else {
		fprintf(stderr, "undef line didn't begin with '#'\n");
		exit(1);
	}
	if(p4->lookAhead[0].name == PPTN_UNDEF){
		eat(p4);
	} else {
		fprintf(stderr, "undef line didn't have 'undef' keyword\n");
		exit(1);
	}

	if(p4->lookAhead[0].name == PPTN_IDENTIFIER){
		nameToken = eat(p4);
	} else {
		fprintf(stderr, "undef expected identifier");
		exit(1);
	}

	hashTableRemove(p4->symbolTable, nameToken.lexeme, (void**)&macroEntry, &found);
	if(found){
		freePPTokenList(macroEntry->replacementList);
		free(macroEntry);
	}


	if(p4->lookAhead[0].name == PPTN_NEWLINE){
		fprintf(stderr, "UNDEF %s\n", nameToken.lexeme);
		return eat(p4);
	} else {
		fprintf(stderr, "undef line contained extra tokens\n");
		exit(1);
	}
}

static struct pptoken ppLineUnimplemented(struct phase4 *p4){
	struct pptoken token;

	token = eat(p4);
	while(token.name != PPTN_EOF && token.name != PPTN_NEWLINE){
		token = eat(p4);
	}
	return token;
}

static struct list *macroReplacement(struct phase4 *p4,
		struct macro_entry *macro, struct repl_token *token){
	struct list *list;
	struct macro_entry *macroEntry;
	int found;

	list = newList();
	assert(list != NULL);

	listEnqueue(list, (void*)token);

	while(1){
		struct repl_token *t;
		listDequeuePeek(list, (void **)&t);
		hashTableGetValue(p4->symbolTable, t->token.lexeme, (void**)&macroEntry, &found);
		if(!found){
			return list;
		} else {
			
		}
	}
}


static struct pptoken ppTextLine(struct phase4 *p4){
	struct pptoken token;
	struct macro_entry *macroEntry;
	int found;

	token = eat(p4);

	if(token.name == PPTN_IDENTIFIER){
		hashTableGetValue(p4->symbolTable, token.lexeme, (void**)&macroEntry, &found);
		if(found){
			if(macroEntry->type == OBJECT_MACRO){
				struct repl_token *t = calloc(1, sizeof(struct repl_token));
				assert(t != 0);
				t->token = token;
				t->macroSet = newStringSet();

			} else {
				fprintf(stderr, "function macros arn't yet implemented in %s\n",
						__FUNCTION__);
				exit(1);
			}
		}
	}
	return token;
}

struct pptoken phase4NextToken(struct phase4 *p4){
#define la1 (p4->lookAhead[0].name)
#define la2 (p4->lookAhead[1].name)
#define la3 (p4->lookAhead[2].name)
#define la4 (p4->lookAhead[2].name)
	struct pptoken token;

	//int state = stateStackPop(p4->stateStack);

	if(la1 == PPTN_HASH && la2 == PPTN_IF){
		return ppLineUnimplemented(p4);
	} else if(la1 == PPTN_HASH && la2 == PPTN_IFDEF){
		return ppLineUnimplemented(p4);

	} else if(la1 == PPTN_HASH && la2 == PPTN_IFNDEF){
		return ppLineUnimplemented(p4);

	} else if(la1 == PPTN_HASH && la2 == PPTN_ELIF){
		return ppLineUnimplemented(p4);

	} else if(la1 == PPTN_HASH && la2 == PPTN_ELSE){
		return ppLineUnimplemented(p4);

	} else if(la1 == PPTN_HASH && la2 == PPTN_ENDIF){
		return ppLineUnimplemented(p4);

	} else if(la1 == PPTN_HASH && la2 == PPTN_INCLUDE){
		return ppLineUnimplemented(p4);

	} else if(la1 == PPTN_HASH && la2 == PPTN_DEFINE){
		return ppDefineLine(p4);

	} else if(la1 == PPTN_HASH && la2 == PPTN_UNDEF){
		return ppUndefLine(p4);

	} else if(la1 == PPTN_HASH && la2 == PPTN_LINE){
		return ppLineUnimplemented(p4);

	} else if(la1 == PPTN_HASH && la2 == PPTN_ERROR){
		return ppLineUnimplemented(p4);

	} else if(la1 == PPTN_HASH && la2 == PPTN_PRAGMA){
		return ppLineUnimplemented(p4);

	} else if(la1 == PPTN_HASH && la2 == PPTN_NEWLINE){
		return ppLineUnimplemented(p4);

	} else if(la1 == PPTN_HASH){
		return ppLineUnimplemented(p4);

	} else {
		// text line
		return ppTextLine(p4);

	}

#undef la1
#undef la2
#undef la3
#undef la4
}




#ifdef TESTING

#include "phase1.h"
#include "phase2.h"


struct pptoken defineTokens[] = {
	{"", "#", LT_PUNCTUATOR, PPTN_HASH},
	{"", "define", LT_IDENTIFIER, PPTN_DEFINE},
	{" ", "TABLESIZE", LT_IDENTIFIER, PPTN_IDENTIFIER},
	{" ", "32", LT_PPNUMBER, PPTN_PPNUMBER},
	{"","\n", LT_NEWLINE, PPTN_NEWLINE},
	{"","int", LT_IDENTIFIER, PPTN_IDENTIFIER},
	{" ", "table", LT_IDENTIFIER, PPTN_IDENTIFIER},
	{"", "[", LT_PUNCTUATOR, PPTN_LBRACKET},
	{"", "TABLESIZE", LT_IDENTIFIER, PPTN_IDENTIFIER},
	{"", "]", LT_PUNCTUATOR, PPTN_RBRACKET},
	{"", "\n", LT_NEWLINE, PPTN_NEWLINE},
	{"", "", LT_EMPTY, PPTN_EOF},
};

int test_define(){
	return 0;
}

int main(int argc, char *argv[]){
	struct phase1 p1;
	struct phase2 p2;
	struct phase3 p3;
	struct phase4 p4;
	struct pptoken token;

	//p1.sourceFile = fopen("phase3.c", "r");
	//p1.sourceFile = fopen("tests/defines.c", "r");
	p1.sourceFile = fopen("tests/define.c", "r");
	phase1Init(&p1);

	p2.p1 = &p1;
	phase2Init(&p2);

	p3.p2 = &p2;
	phase3Init(&p3);

	p4.p3 = &p3;
	p4.symbolTable = newHashTable();
	phase4Init(&p4);

	token = phase4NextToken(&p4);
	while(token.name != PPTN_EOF){
		printf("%s", token.whiteSpace);
		printf("%s", token.lexeme);
		token = phase4NextToken(&p4);
	}
	return 0;
}
#endif
