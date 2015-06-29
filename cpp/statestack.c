/*
 * Copyright 2015 Joseph Landry All Rights Reserved
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#ifdef TESTING
#define DEFAULT_SIZE (4)
#define DEFAULT_INC_SIZE (4)
#else
#define DEFAULT_SIZE (64)
#define DEFAULT_INC_SIZE (64)
#endif

struct state_stack {
	int *buf;
	int size;
	int topIndex;
};


struct state_stack *newStateStack(){
	struct state_stack *ss;

	ss = calloc(1, sizeof(*ss));
	assert(ss != NULL);


	ss->size = DEFAULT_SIZE;
	ss->buf = calloc(ss->size, sizeof(int));
	ss->topIndex = 0;
	return ss;
}

void freeStateStack(struct state_stack *ss){
	free(ss->buf);
	ss->buf = 0;
	free(ss);
}

int stateStackPeek(struct state_stack *ss){
	if(ss->topIndex > 0){
		return ss->buf[ss->topIndex-1];
	} else {
		return -1;
	}
}

static void stateStackIncreaseSize(struct state_stack *ss){
	int *buf;

	buf = realloc(ss->buf, (ss->size + DEFAULT_INC_SIZE) * sizeof(int));
	assert(buf != 0);
	ss->size += DEFAULT_INC_SIZE;
	ss->buf = buf;
}

int stateStackPush(struct state_stack *ss, int state){
	if(ss->topIndex >= ss->size){
		stateStackIncreaseSize(ss);
	}
	ss->buf[ss->topIndex] = state;
	ss->topIndex += 1;
	return 0;
}

int stateStackPop(struct state_stack *ss){
	int state;

	if(ss->topIndex > 0){
		state = ss->buf[ss->topIndex-1];
		ss->topIndex -= 1;
		return state;
	} else {
		return -1;
	}
}


#ifdef TESTING

int main(int argc, char *argv[]){
	struct state_stack *ss;

	ss = newStateStack();

	assert(stateStackPeek(ss) == -1);
	assert(stateStackPeek(ss) == -1);

	stateStackPush(ss, 10);
	stateStackPush(ss, 9);
	stateStackPush(ss, 8);
	stateStackPush(ss, 7);
	stateStackPush(ss, 6);
	stateStackPush(ss, 5);
	stateStackPush(ss, 4);
	stateStackPush(ss, 3);
	stateStackPush(ss, 2);
	stateStackPush(ss, 1);
	stateStackPush(ss, 0);

	assert(stateStackPop(ss) == 0);
	assert(stateStackPop(ss) == 1);
	assert(stateStackPop(ss) == 2);
	assert(stateStackPop(ss) == 3);
	assert(stateStackPop(ss) == 4);
	assert(stateStackPop(ss) == 5);
	assert(stateStackPop(ss) == 6);
	assert(stateStackPop(ss) == 7);
	assert(stateStackPop(ss) == 8);
	assert(stateStackPop(ss) == 9);
	assert(stateStackPop(ss) == 10);
	assert(stateStackPop(ss) == -1);
	assert(stateStackPop(ss) == -1);
	assert(stateStackPop(ss) == -1);

	printf("%-32s OK\n", argv[0]);
	return 0;
}

#endif
