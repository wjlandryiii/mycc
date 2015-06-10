/*
 * Copyright 2015 Joseph Landry All Rights Reserved
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"

int main(int argc, char *argv[]){
	int result;

	strcpy(REGEXSTRING, "(.)+a(z|q)at?|a(bbbb)*");
	result = parse();

	if(result == 0){
		printf("Successful parse!\n");
	} else {
		printf("Parse was unsuccessful\n");
	}
	return 0;
}
