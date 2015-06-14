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
	strcpy(REGEXSTRING, "a");
	result = parse();
	graphParseTree();
}

void test_nfa(void){
	int i;

	struct nfa nfaAny = elementaryAny();
	
	graphNFA(&nfaAny);
	/*
	struct nfa nfaD = elementaryOneCharacter('B');
	struct nfa nfaE = nfaConcatenation(&nfaC, &nfaD);
	struct nfa nfaF = nfaStar(&nfaE);
	graphNFA(&nfaF);
	*/
}

void test_ast(void){
	strcpy(REGEXSTRING, "\\.*|(012)?");
	strcpy(REGEXSTRING, "a+b@+a+(\\.c+)+");
	strcpy(REGEXSTRING, "aba.b$");
	parse();

	if(0){
		graphParseTree();
	} else {
		computeAST();
		graphAST();
	}
}

int main(int argc, char *argv[]){
	//test_regex();
	test_nfa();
	//test_ast();

	return 0;
}
