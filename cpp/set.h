/*
 * Copyright 2015 Joseph Landry All Rights Reserved
 */

#ifndef SET_H
#define SET_H

struct string_set;

struct string_set *newStringSet(void);
void freeStringSet(struct string_set *stringSet);
int stringSetInsertString(struct string_set *set, char *s);
int stringSetIsMember(struct string_set *set, char *s);

#endif
