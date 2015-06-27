/*
 * Copyright 2015 Joseph Landry All Rights Reserved
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "stringbuf.h"

#define DEFAULT_SIZE (8)
#define DEFAULT_BUMP_SIZE (16)

struct string_buf {
	char *buf;
	int size;
	int length;
};


struct string_buf *newStringBufWithSize(int size){
	struct string_buf *sb;

	sb = calloc(1, sizeof(*sb));
	assert(sb != NULL);

	sb->size = size;
	sb->buf = calloc(size, sizeof(*(sb->buf)));
	assert(sb->buf != NULL);
	sb->length = 0;
	return sb;
}

struct string_buf *newStringBuf(){
	return newStringBufWithSize(DEFAULT_SIZE);
}

void freeStringBuf(struct string_buf *sb){
	memset(sb->buf, 0, sb->size);
	free(sb->buf);
	sb->buf = NULL;
	sb->size = 0;
	sb->length = 0;
	free(sb);
}

static int stringBufIncreaseSize(struct string_buf *sb, int size){
	char *p;

	p = realloc(sb->buf, sb->size + size);
	assert(p != 0);
	sb->buf = p;
	sb->size += size;
	return 0;
}

static int stringBufBumpSize(struct string_buf *sb){
	return stringBufIncreaseSize(sb, DEFAULT_BUMP_SIZE);
}

int stringBufAppendChar(struct string_buf *sb, int c){
	if(sb->length >= sb->size){
		stringBufBumpSize(sb);
	}
	sb->buf[sb->length] = c;
	sb->length += 1;
	return 0;
}

char *stringBufToSZ(struct string_buf *sb){
	char *s;
	s = malloc(sb->length + 1);
	assert(s != NULL);
	memcpy(s, sb->buf, sb->length);
	s[sb->length] = '\0';
	return s;
}

int stringBufCompareSZ(struct string_buf *sb, char *sz){

	int i;
	for(i = 0; i < sb->length; i++){
		if(sb->buf[i] != *sz){
			return sb->buf[i] - *sz;
		}
		sz++;
	}
	if(*sz != 0){
		return 0 - *sz;
	} else {
		return 0;
	}
}


#ifdef TESTING


int main(int argc, char *argv[]){
	struct string_buf *sb;
	char *s;

	sb = newStringBuf();

	stringBufAppendChar(sb, 'H');
	stringBufAppendChar(sb, 'e');
	stringBufAppendChar(sb, 'l');
	stringBufAppendChar(sb, 'l');
	stringBufAppendChar(sb, 'o');
	stringBufAppendChar(sb, ' ');
	stringBufAppendChar(sb, 'W');
	stringBufAppendChar(sb, 'o');
	stringBufAppendChar(sb, 'r');
	stringBufAppendChar(sb, 'l');
	stringBufAppendChar(sb, 'd');
	stringBufAppendChar(sb, '!');

	puts(stringBufToSZ(sb));

	return 0;
}

#endif
