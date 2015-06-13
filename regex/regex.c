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

	struct nfa nfaA = elementaryOneCharacter('A');
	struct nfa nfaB = elementaryOneCharacter('B');
	struct nfa nfaC = elementaryOneCharacter('C');
	struct nfa nfaD = nfaUnion(&nfaA, &nfaB);
	struct nfa nfaE = nfaUnion(&nfaD, &nfaC);
	graphNFA(&nfaE);
	/*
	struct nfa nfaD = elementaryOneCharacter('B');
	struct nfa nfaE = nfaConcatenation(&nfaC, &nfaD);
	struct nfa nfaF = nfaStar(&nfaE);
	graphNFA(&nfaF);
	*/
}

void test_ast(void){
	strcpy(REGEXSTRING, "AB|C");
	parse();
	//graphParseTree();
	computeAST();
	graphAST();
}

int main(int argc, char *argv[]){
	//test_regex();
	test_ast();

	return 0;
}
