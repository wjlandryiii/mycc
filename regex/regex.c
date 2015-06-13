/*
 * Copyright 2015 Joseph Landry All Rights Reserved
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"
#include "nfa.h"

int main(int argc, char *argv[]){
	int i;
	int result;

	/*
	strcpy(REGEXSTRING, "(a.)+\\.a(z|q)at?|a(bbbb)*");
	strcpy(REGEXSTRING, "a[A-Z]b");
	strcpy(REGEXSTRING, "a[^A-Z]b");
	strcpy(REGEXSTRING, "a[ABC-M]b");
	strcpy(REGEXSTRING, "a[^ABC-M]b");
	strcpy(REGEXSTRING, "a[^AB\\?-M]b");
	strcpy(REGEXSTRING, "abc+[d-z]?");
	strcpy(REGEXSTRING, "A|B");
	result = parse();
	*/

	struct nfa nfaA = elementaryOneCharacter('A');
	struct nfa nfaB = elementaryOneCharacter('B');
	struct nfa nfaC = nfaUnion(&nfaA, &nfaB);
	struct nfa nfaD = elementaryOneCharacter('B');
	struct nfa nfaE = nfaConcatenation(&nfaC, &nfaD);
	struct nfa nfaF = nfaStar(&nfaE);
	graphNFA(&nfaF);

	return 0;
}
