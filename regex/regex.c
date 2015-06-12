/*
 * Copyright 2015 Joseph Landry All Rights Reserved
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"

int main(int argc, char *argv[]){
	int result;

	strcpy(REGEXSTRING, "(a.)+\\.a(z|q)at?|a(bbbb)*");
	strcpy(REGEXSTRING, "a[A-Z]b");
	strcpy(REGEXSTRING, "a[^A-Z]b");
	strcpy(REGEXSTRING, "a[ABC-M]b");
	strcpy(REGEXSTRING, "a[^ABC-M]b");
	strcpy(REGEXSTRING, "a[^AB\\?-M]b");
	result = parse();

	return 0;
}
