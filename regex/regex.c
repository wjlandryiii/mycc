/*
 * Copyright 2015 Joseph Landry All Rights Reserved
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"
#include "nfa.h"
#include "ast.h"

void test_regex(void){
	int result;
	strcpy(REGEXSTRING, "(a.)+\\.a(z|q)at?|a(bbbb)*");
	strcpy(REGEXSTRING, "a[A-Z]b");
	strcpy(REGEXSTRING, "a[^A-Z]b");
	strcpy(REGEXSTRING, "a[ABC-M]b");
	strcpy(REGEXSTRING, "a[^ABC-M]b");
	strcpy(REGEXSTRING, "a[^AB\\?-M]b");
	strcpy(REGEXSTRING, "abc+[d-z]?");
	result = parse();
	graphParseTree();
}


void test_ast(void){
	struct ast_node *node;

	node = astBinaryNode(OP_CONCAT,
			astLeafNode(OP_LITERAL, 'A'),
			astBinaryNode(OP_CONCAT, 
				astLeafNode(OP_LITERAL, 'B'),
				astLeafNode(OP_LITERAL, 'C')));
	

	graphAST(node);	
}


void test_nfa(){
	int test = 2;

	if(test == 0){
		struct nfa nfa = elementaryOneCharacter('A');
		graphNFA(&nfa);
	} else if(test == 1){
		struct nfa nfa = elementaryAny();
		graphNFA(&nfa);
	} else if(test == 2){
		char set[128] = {0};
		set['I'] = 1;
		set['J'] = 1;
		set['K'] = 1;
		set['Z'] = 1;
		struct nfa nfa = elementarySet(set);
		graphNFA(&nfa);
	} else if(test == 3){
		
	}
}

int main(int argc, char *argv[]){
	//test_regex();
	test_ast();
	//test_nfa();

	return 0;
}
