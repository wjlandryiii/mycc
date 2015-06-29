/*
 * Copyright 2015 Joseph Landry All Rights Reserved
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "tokens.h"
#include "escstr.h"

char *PPTOKEN_STRINGS[PPTN_NTOKENS] = {
	"PPTN_EOF",
	"PPTN_HEADERNAME",
	"PPTN_IDENTIFIER",
	"PPTN_PPNUMBER",
	"PPTN_CHARACTERCONSTANT",
	"PPTN_STRINGLITERAL",
	"PPTN_IF",
	"PPTN_IFDEF",
	"PPTN_IFNDEF",
	"PPTN_ELIF",
	"PPTN_ELSE",
	"PPTN_ENDIF",
	"PPTN_INCLUDE",
	"PPTN_DEFINE",
	"PPTN_LINE",
	"PPTN_ERROR",
	"PPTN_PRAGMA",
	"PPTN_NEWLINE",
	"PPTN_NE_OP",
	"PPTN_EXLAMATIONMARK",
	"PPTN_GLUE_OP",
	"PPTN_HASH",
	"PPTN_AND_OP",
	"PPTN_AND_ASSIGN",
	"PPTN_AMPERSAND",
	"PPTN_MOD_ASSIGN",
	"PPTN_PERCENT",
	"PPTN_LPAREN",
	"PPTN_RPAREN",
	"PPTN_MUL_ASSIGN",
	"PPTN_ASTERISK",
	"PPTN_INC_OP",
	"PPTN_ADD_ASSIGN",
	"PPTN_PLUS_SIGN",
	"PPTN_COMMA",
	"PPTN_DEC_OP",
	"PPTN_SUB_ASSIGN",
	"PPTN_PTR_OP",
	"PPTN_MINUS_SIGN",
	"PPTN_ELLIPSIS",
	"PPTN_PERIOD",
	"PPTN_DIV_ASSIGN",
	"PPTN_SLASH",
	"PPTN_COLON",
	"PPTN_SEMICOLON",
	"PPTN_SL_ASSIGN",
	"PPTN_SL_OP",
	"PPTN_LE_OP",
	"PPTN_LCHEVRON",
	"PPTN_EQ_OP",
	"PPTN_EQUAL",
	"PPTN_SR_ASSIGN",
	"PPTN_SR_OP",
	"PPTN_GE_OP",
	"PPTN_RCHEVRON",
	"PPTN_QUESTION_MARK",
	"PPTN_LBRACKET",
	"PPTN_RBRACKET",
	"PPTN_XOR_ASSIGN",
	"PPTN_CIRCUMFLEX",
	"PPTN_LBRACE",
	"PPTN_OR_ASSIGN",
	"PPTN_OR_OP",
	"PPTN_PIPE",
	"PPTN_RBRACE",
	"PPTN_TILDE",
};

void printPPToken(struct pptoken pptoken){
	printf("[lexeme:\"");
	puts_escaped(pptoken.lexeme, pptoken.lexeme + strlen(pptoken.lexeme));
	printf("\", name:%s]\n", PPTOKEN_STRINGS[pptoken.name]);
}


#define PPTOKEN_QUEUE_DEFAULT_SIZE (2)

struct pptoken_queue {
	struct pptoken *buf;
	int head;
	int tail;
	int size;
};

struct pptoken_queue *newPPTokenQueue(){
	struct pptoken_queue *queue;

	queue = calloc(1, sizeof(*queue));
	assert(queue != NULL);

	queue->size = PPTOKEN_QUEUE_DEFAULT_SIZE;
	queue->buf = calloc(queue->size, sizeof(struct pptoken));
	assert(queue->buf != NULL);

	queue->head = 0;
	queue->tail = 0;
	return queue;
}

void freePPTokenQueue(struct pptoken_queue *queue){
	free(queue->buf);
	free(queue);
}


static int ppTokenQueueShift(struct pptoken_queue *queue){
	memmove(queue->buf, queue->buf+queue->head,
			(queue->tail - queue->head) * sizeof(struct pptoken));
	queue->tail = queue->tail - queue->head;
	queue->head = 0;
	return 0;
}

static int ppTokenQueueIncreaseSize(struct pptoken_queue *queue){
	int newSize = queue->size + 32;
	struct pptoken *newBuf;

	newBuf = realloc(queue->buf, newSize * sizeof(struct pptoken));
	assert(newBuf != NULL);

	queue->buf = newBuf;
	queue->size = newSize;
	return 0;
}

int ppTokenQueueEnqueue(struct pptoken_queue *queue, struct pptoken token){
	if(queue->tail >= queue->size){
		if(queue->head > 0){
			ppTokenQueueShift(queue);
		} else {
			ppTokenQueueIncreaseSize(queue);
		}
	}
	assert(queue->tail < queue->size);

	queue->buf[queue->tail] = token;
	queue->tail += 1;
	return 0;
}

int ppTokenQueueDequeue(struct pptoken_queue *queue, struct pptoken *tokenOut){
	if(queue->head < queue->tail){
		*tokenOut = queue->buf[queue->head];
		queue->head += 1;
		return 0;
	} else {
		return -1;
	}
}


#define PPTOKENLIST_DEFAULT_SIZE (32)
#define PPTOKENLIST_INC_SIZE (32)

struct pptoken_list {
	struct pptoken *buf;
	int size;
	int count;
};


struct pptoken_list *newPPTokenList(){
	struct pptoken_list *list;

	list = calloc(1, sizeof(*list));
	assert(list != NULL);
	list->count = 0;
	list->size = PPTOKENLIST_DEFAULT_SIZE;
	list->buf = calloc(list->size, sizeof(struct pptoken));
	return list;
}

void freePPTokenList(struct pptoken_list *list){
	free(list->buf);
	list->buf = NULL;
	free(list);
}

int ppTokenListAtIndex(struct pptoken_list *list, int index, struct pptoken *tokenOut){
	assert(0 <= index || index < list->count);
	*tokenOut = list->buf[index];
	return 0;
}

static int ppTokenListIncreaseSize(struct pptoken_list *list){
	int newSize;
	struct pptoken *newBuf;

	newSize = list->size + PPTOKENLIST_INC_SIZE;
	newBuf = realloc(list->buf, newSize);
	assert(newBuf != NULL);
	list->buf = newBuf;
	list->size = newSize;
	return 0;
}

int ppTokenListAppend(struct pptoken_list *list, struct pptoken token){
	if(list->size <= list->count){
		ppTokenListIncreaseSize(list);
	}
	assert(list->count < list->size);

	list->buf[list->count] = token;
	list->count += 1;
	return 0;
}






#ifdef TESTING

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

void test_PPTokenQueue(){
	struct pptoken_queue *queue;
	struct pptoken token;

	queue = newPPTokenQueue();
	token.name = 1; ppTokenQueueEnqueue(queue, token);
	token.name = 2; ppTokenQueueEnqueue(queue, token);
	token.name = 3; ppTokenQueueEnqueue(queue, token);
	token.name = 4; ppTokenQueueEnqueue(queue, token);

	assert(ppTokenQueueDequeue(queue, &token) == 0);
	assert(token.name == 1);
	assert(ppTokenQueueDequeue(queue, &token) == 0);
	assert(token.name == 2);
	assert(ppTokenQueueDequeue(queue, &token) == 0);
	assert(token.name == 3);
	assert(ppTokenQueueDequeue(queue, &token) == 0);
	assert(token.name == 4);
	freePPTokenQueue(queue);
	printf("OK\n");
}




void test_PPTokenList(){
	struct pptoken_list *list;
	struct pptoken token;

	list = newPPTokenList();

	token.name = PPTN_HASH;
	ppTokenListAppend(list, token);
	token.name = PPTN_DEFINE;
	ppTokenListAppend(list, token);
	token.name = PPTN_IDENTIFIER;
	ppTokenListAppend(list, token);
	token.name = PPTN_PPNUMBER;
	ppTokenListAppend(list, token);
	token.name = PPTN_NEWLINE;
	ppTokenListAppend(list, token);

	assert(ppTokenListAtIndex(list, 0, &token) == 0);
	assert(token.name == PPTN_HASH);
	assert(ppTokenListAtIndex(list, 1, &token) == 0);
	assert(token.name == PPTN_DEFINE);
	assert(ppTokenListAtIndex(list, 2, &token) == 0);
	assert(token.name == PPTN_IDENTIFIER);
	assert(ppTokenListAtIndex(list, 4, &token) == 0);
	assert(token.name == PPTN_PPNUMBER);
	assert(ppTokenListAtIndex(list, 6, &token) == 0);
	assert(token.name == PPTN_NEWLINE);

	printf("OK\n");
}



int main(int argc, char *argv[]){
	test_PPTokenQueue();
	test_PPTokenList();
	return 0;
}

#endif
