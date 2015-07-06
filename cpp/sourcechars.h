/*
 * Copyright 2015 Joseph Landry All Rights Reserved
 */

#ifndef SOURCECHARS_H
#define SOURCECHARS_H

struct list;

// TODO: rename source_char to char_token or token_char?
struct source_char {
	int c;
	char *fileName;
	int lineNumber;
	int columnNumber;
};

struct list *sourceCharsFromFile(char *fileName);


#endif
