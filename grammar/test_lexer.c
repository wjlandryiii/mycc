/*
 * Copyright 2015 Joseph Landry All Rights Reserved
 */

#include <stdio.h>
#include <string.h>

#include "lexer.h"
#include "strings.h"
#include "symbols.h"


void test_lexer(char *s, int name){
	int result;

	strcpy(lexerInputBuffer, s);

	result = tokenize();
	if(result < 0){
		printf("lexer error on string _%s_\n", s);
		return;
	}
	if(tokenStreamLength <= 0 || tokenStream[0].name != name){
		printf("lexer failed: _%s_\n", s);
		return;
	}
}

void test_tokens(void){
	test_lexer(" <hello-world> ", TOKNAME_NONTERMINAL);
	test_lexer(" \"hello world\" ", TOKNAME_TERMINAL);
	test_lexer(" \'hello world\' ", TOKNAME_TERMINAL);
	test_lexer(" : ", TOKNAME_COLON);
	test_lexer(" | ", TOKNAME_PIPE);
	test_lexer(" ; ", TOKNAME_SEMICOLON);
	test_lexer("  ", TOKNAME_EOF);
	test_lexer("# comment <aaa> [ ] ::= ; \"moo\"\n<hello> : ", TOKNAME_NONTERMINAL);
}


int main(int argc, char *argv[]){
	test_tokens();
	return 0;
}
