/*
 * Copyright 2015 Joseph Landry All Rights Reserved
 */

#include <stdio.h>
#include <string.h>
#include "stringtable.h"

void runTest(void){
	int i;
	char *p;
	
	p = stringtableGetString(0);
	if(p == NULL){
		printf("missing empty string\n");
		return;
	}
	if(strcmp("", p) != 0){
		printf("emty string is not empty\n");
		return;
	}
	p = stringtableGetString(1);
	if(p != NULL){
		printf("found a string that doesn't exist\n");
		return;
	}
	i = stringtableAddString("asdf");
	p = stringtableGetString(i);
	if(strcmp("asdf", p) != 0){
		printf("test string doesn't match\n");
		return;
	}
}

int main(int argc, char *argv[]){
	runTest();
	return 0;
}
