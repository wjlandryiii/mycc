/*
 * Copyright 2015 Joseph Landry All Rights Reserved
 */

#ifndef STRINGBUF_H
#define STRINGBUF_H

struct string_buf;

struct string_buf *newStringBufWithSize(int size);
struct string_buf *newStringBuf();
void freeStringBuf(struct string_buf *sb);
int stringBufAppendChar(struct string_buf *sb, int c);
char *stringBufToSZ(struct string_buf *sb);
int stringBufCompareSZ(struct string_buf *sb, char *sz);

#endif
