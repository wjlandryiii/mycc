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


#if 0
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
#endif

struct macro {
	char *macroName;
	int isFunctionMacro;
	struct list *identifierList;
	struct list *replacementList;
};


static struct pptoken *eofToken(){
	struct pptoken *token;

	token = calloc(1, sizeof(*token));
	token->whiteSpace = "";
	token->startsLine = 1;
	token->isDirective = 0;
	token->lexeme = "";
	token->type = 0;
	token->name = PPTN_EOF;
	return token;
}

static struct pptoken *lookAhead(struct list *input, int i){
	int count;
	struct pptoken *token;

	count = listItemCount(input);
	if(i < count){
		listItemAtIndex(input, i, (void**)&token);
		return token;
	} else {
		return eofToken();
	}
}

static int isDirectiveLine(struct pptoken *la1, struct pptoken *la2){
	if(la1->name != PPTN_HASH)
		return 0;
	if(!la1->startsLine)
		return 0;
	if(la2->startsLine)
		return 0;
	if(!la2->isDirective)
		return 0;
	return 1;
}

static struct macro *parseObjectMacro(struct list *line){
	struct list *replacementList;
	struct macro *macro;
	struct pptoken *macroName;

	listDequeue(line, NULL); // #
	listDequeue(line, NULL); // define
	listDequeue(line, (void**)&macroName);
	replacementList = line;
	macro = calloc(1, sizeof(*macro));
	assert(macro != NULL);
	macro->macroName = macroName->lexeme;
	macro->isFunctionMacro = 0;
	macro->identifierList = NULL;
	macro->replacementList = replacementList;
	return macro;
}
/*
static struct macro *parseFunctionMacro(struct list *line){
	
}
*/

static int defineLine(struct hash_table *macros, struct list *line){
	struct pptoken *token;
	struct pptoken *macroName;
	struct list *identifierList;
	struct list *replacementList;
	struct pptoken *la1;
	struct pptoken *la2;
	struct pptoken *la3;
	struct pptoken *la4;

	la1 = lookAhead(line, 0);
	la2 = lookAhead(line, 1);
	la3 = lookAhead(line, 2);
	la4 = lookAhead(line, 3);

	if(la1->name != PPTN_HASH || !la1->startsLine){
		fprintf(stderr, "define line missing '#'\n");
		exit(1);
	}
	if(la2->name != PPTN_DEFINE){
		fprintf(stderr, "define line missing 'define' keyword\n");
		exit(1);
	}
	if(la3->name != PPTN_IDENTIFIER){
		fprintf(stderr, "define line expected identifier\n");
		exit(1);
	}

	if(la4->name == PPTN_LPAREN && strlen(la4->whiteSpace) == 0){
		identifierList = newList();
		assert(identifierList != NULL);
		listDequeue(line, NULL); // #
		listDequeue(line, NULL); // define
		listDequeue(line, (void**)&macroName);
		listDequeue(line, NULL); // (

		la1 = lookAhead(line, 0);
		la2 = lookAhead(line, 1);

		while(la1->name == PPTN_IDENTIFIER){
			listDequeue(line, (void**)&token);
			listEnqueue(identifierList, token);
			if(la2->name == PPTN_COMMA){
				listDequeue(line, NULL);
			}
			la1 = lookAhead(line, 0);
			la2 = lookAhead(line, 1);
		}
		if(la1->name == PPTN_RPAREN){
			listDequeue(line, NULL);
		} else if(la1->name == PPTN_PERIOD_PERIOD_PERIOD && la2->name == PPTN_RPAREN){
			listDequeue(line, NULL);
			listDequeue(line, NULL);
		} else {
			fprintf(stderr, "function macro expected ')' or \"...)\"\n");
			exit(1);
		}
		replacementList = line;
	} else {
		struct macro *macro;
		int found;
		listDequeue(line, NULL); // #
		listDequeue(line, NULL); // define
		listDequeue(line, (void**)&macroName);
		replacementList = line;

		hashTableGetValue(macros, macroName->lexeme, (void**)&macro, &found);
		if(found){
			fprintf(stderr, "define line replacemnt not impllemented\n");
			exit(1);
		} else {
			macro = calloc(1, sizeof(*macro));
			assert(macro != NULL);
			macro->macroName = macroName->lexeme;
			macro->isFunctionMacro = 0;
			macro->identifierList = NULL;
			macro->replacementList = replacementList;
			hashTableSetValue(macros, macro->macroName, macro);
		}
	}
	return 0;
}

struct list *objectMacroReplacement(struct macro *macro, struct pptoken *token){
	int i;
	struct list *output;
	struct pptoken *outToken;
	struct pptoken *rtoken;

	output = newList();

	for(i = 0; i < listItemCount(macro->replacementList); i++){
		listItemAtIndex(macro->replacementList, i, (void**)&rtoken);
		outToken = calloc(1, sizeof(*outToken));
		assert(outToken != NULL);

		if(i == 0){
			outToken->whiteSpace = token->whiteSpace;
			outToken->startsLine = token->startsLine;
		} else {
			outToken->whiteSpace = rtoken->whiteSpace;
			outToken->startsLine = 0;
		}
		outToken->lexeme = rtoken->lexeme;
		outToken->type = rtoken->type;
		outToken->name = rtoken->name;
		if(token->replacements){
			outToken->replacements = newStringSetFromStringSet(token->replacements);
		} else {
			outToken->replacements = newStringSet();
		}
		stringSetInsertString(outToken->replacements, macro->macroName);
		listPush(output, outToken);
	}
	return output;
}

static int tryExpandingMacros(struct hash_table *macros, struct list *input){
	struct pptoken *la1;
	struct pptoken *token;
	struct macro *macro;
	int found;

	la1 = lookAhead(input, 0);

	if(la1->name == PPTN_IDENTIFIER){
		hashTableGetValue(macros, la1->lexeme, (void**)&macro, &found);
		if(found){
			if(!la1->replacements || !stringSetIsMember(la1->replacements, macro->macroName)){
				if(!macro->isFunctionMacro){
					struct list *l;
					listDequeue(input, (void**)&token);
					l = objectMacroReplacement(macro, token);
					listUnshiftList(input, l);
					return 1;
				} else {
					fprintf(stderr, "function macro not implemented\n");
					exit(1);
				}
			} else {
				return 0;
			}
		} else {
			return 0;
		}
	} else {
		return 0;
	}
}

struct list *phase4(struct hash_table *macros, struct list *input){
	struct list *output;
	struct list *line;
	struct pptoken *token;
	struct pptoken *la1;
	struct pptoken *la2;
	struct pptoken *la3;
	struct pptoken *la4;
	int name;

	output = newList();
	assert(output != NULL);

	la1 = lookAhead(input, 0);
	la2 = lookAhead(input, 1);
	la3 = lookAhead(input, 2);
	la4 = lookAhead(input, 3);

	while(listItemCount(input) > 0){
		if(isDirectiveLine(la1, la2)){
			name = la2->name;
			line = newList();
			assert(line != NULL);

			do {
				listDequeue(input, (void**)&token);
				listEnqueue(line, token);
				la1 = lookAhead(input, 0);
			} while(!la1->startsLine);


			if(name){
				if(name == PPTN_IF){
				} else if(name == PPTN_IFDEF){
				} else if(la2->name == PPTN_IFNDEF){
				} else if(la2->name == PPTN_ELIF){
				} else if(la2->name == PPTN_ELSE){
				} else if(la2->name == PPTN_ENDIF){
				} else if(la2->name == PPTN_INCLUDE){
				} else if(la2->name == PPTN_DEFINE){
					defineLine(macros, line);
				} else if(la2->name == PPTN_UNDEF){
				} else if(la2->name == PPTN_LINE){
				} else if(la2->name == PPTN_ERROR){
				} else if(la2->name == PPTN_PRAGMA){
				} else {
				}
			}
		} else {
			la1 = lookAhead(input, 0);
			do{
				if(la1->name == PPTN_IDENTIFIER){
					while(tryExpandingMacros(macros, input))
						;
				}
				listDequeue(input, (void**)&token);
				listEnqueue(output, token);
				la1 = lookAhead(input, 0);
			}while(!la1->startsLine);
		}

		la1 = lookAhead(input, 0);
		la2 = lookAhead(input, 1);
		la3 = lookAhead(input, 2);
		la4 = lookAhead(input, 3);
	}
	return output;
}


#ifdef TESTING

#include "sourcechars.h"
#include "phase1.h"
#include "phase2.h"

#if 0
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

void test_phase4(void){
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
}
#endif

void test_phase4(void){
	struct pptoken *token;
	struct list *source;
	struct list *p1;
	struct list *p2;
	struct list *p3;
	struct list *p4;
	struct hash_table *macros;

	//source = sourceCharsFromFile("tests/trigraphs.c");
	//source = sourceCharsFromFile("tests/doubledefine.c");
	//source = sourceCharsFromFile("tests/recursivedefine.c");
	source = sourceCharsFromFile("tests/functionmacro.c");
	p1 = phase1(source);
	p2 = phase2(p1);
	p3 = phase3(p2);

	macros = newHashTable();
	assert(macros != NULL);

	p4 = phase4(macros, p3);

	while(listDequeue(p4, (void**)&token) == 0){
		fputs(token->whiteSpace, stdout);
		if(token->name != PPTN_EOF){
			putchar('{');
			printf("\033[35m");
			fputs(token->lexeme, stdout);
			printf("\033[39m");
			putchar('}');
		} else {
			break;
		}
	}
}


void test_objectMacro(void){
	struct list *inputList;
	struct macro macro;
	struct pptoken t1;
	struct pptoken t2;
	struct pptoken t3;
	struct pptoken inputToken;
	struct list *r;
	struct pptoken *t;

	macro.macroName = "FOO";
	macro.isFunctionMacro = 0;
	macro.identifierList = NULL;
	macro.replacementList = newList();
	assert(macro.replacementList != NULL);

	t1.whiteSpace = " ";
	t1.startsLine = 0;
	t1.isDirective = 0;
	t1.lexeme = "5";
	t1.name = PPTN_PPNUMBER;
	t1.replacements = NULL;

	t2.whiteSpace = " ";
	t2.startsLine = 0;
	t2.isDirective = 0;
	t2.lexeme = "+";
	t2.name = PPTN_PLUS;
	t2.replacements = NULL;

	t3.whiteSpace = " ";
	t3.startsLine = 0;
	t3.isDirective = 0;
	t3.lexeme = "60";
	t3.name = PPTN_PLUS;
	t3.replacements = NULL;

	listPush(macro.replacementList, &t1);
	listPush(macro.replacementList, &t2);
	listPush(macro.replacementList, &t3);

	inputList = newList();
	assert(inputList != NULL);

	inputToken.whiteSpace = "\n";
	inputToken.startsLine = 1;
	inputToken.isDirective = 0;
	inputToken.lexeme = "FOO";
	inputToken.name = PPTN_IDENTIFIER;
	inputToken.replacements = NULL;

	r = objectMacroReplacement(&macro, &inputToken);
	printf("items: %d\n", listItemCount(r));
	while(!listDequeue(r, (void**)&t)){
		printf("%s", t->whiteSpace);
		printf("%s", t->lexeme);
	}
}

int main(int argc, char *argv[]){
	test_phase4();
	//test_objectMacro();
	return 0;
}
#endif
