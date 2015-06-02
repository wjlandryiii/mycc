/*
 * Copyright Joseph Landry 2015 All Rights Reserved
 */

#include "stringtable.h"

#include <stdlib.h>
#include <strings.h>

static char stringBuf[16*1024];
static int bytesUsed = 1;

char *strings[1024] = {stringBuf, 0};
int stringCount = 1;


static char *appendString(char *s){
	char *p;
	int l;

	l = strlen(s);
	if(l+1 < sizeof(stringBuf) - bytesUsed){
		p = &stringBuf[bytesUsed];
		strcpy(p, s);
		bytesUsed += l + 1;
		return p;
	} else {
		return 0;
	}
}

static int lookupString(char *s){
	int i;

	for(i = 0; i < stringCount; i++){
		if(strcmp(strings[i], s) == 0){
			return i;
		}
	}
	return -1;
}

int stringtableAddString(char *s){
	char *p;
	int i;

	i = lookupString(s);
	if(i < 0){
		p = appendString(s);
		if(p){
			strings[stringCount] = p;
			return stringCount++;
		} else {
			return -1;
		}
	} else {
		return i;
	}
}
