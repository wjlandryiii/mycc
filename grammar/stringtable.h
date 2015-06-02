/*
 * Copyright 2015 Joseph Landry All Rights Reserved
 */

#ifndef STRINGTABLE_H
#define STRINGTABLE_H

extern char *strings[1024] = {stringBuf, 0};
extern int stringCount = 1;

int stringtableAddString(char *s);
char *stringtableGetString(int i);

#endif
