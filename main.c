/*
 * Copyright 2015 Joseph Landry All Rights Reserved
 */

#include <stdio.h>
#include <assert.h>

int line_number;
int column_number;

int backbuf[4];
int backbuf_count;

int nextc(FILE *in){
	static initialized = 0;

	peek = getc(in);
	if(peek == '\n'){
		line_number++;
		column_number = 0;
	} else {
		column_number++;
	}
	return peek;
}


int preprocessor(FILE *in, FILE *out){

}

int main(int argc, char *argv[]){
	preprocessor(stdin, stdout);
	return 0;
}
