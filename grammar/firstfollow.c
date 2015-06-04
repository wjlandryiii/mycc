/*
 *	 Copyright 2015 Joseph Landry All Rights Reserved
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "symbols.h"
#include "parser.h"
#include "firstfollow.h"


struct ff_node *firstFollowSet;

struct ff_node ffNodes[128];
int ffNodeCount;



static int isInSet(struct ff_set *set, int needle){
	int i;

	for(i = 0; i < set->count; i++){
		if(set->items[i] == needle){
			return 1;
		}
	}
	return 0;
}

static int insertIntoSet(struct ff_set *set, int needle){
	int i, j;
	int loc;

	assert(set->count < sizeof(set->items)/sizeof(*set->items));

	for(i = 0; i < set->count; i++){
		if(set->items[i] > needle){
			break;
		}
	}
	loc = i;
	for(i = set->count - 1; i >= loc; i--){
		assert(i+1 < 64);
		set->items[i+1] = set->items[i];
	}
	set->items[loc] = needle;
	set->count += 1;
	return 0;
}

static int insertUnionIntoSet(struct ff_set *dst, struct ff_set *src){
	int i;

	for(i = 0; i < src->count; i++){
		if(!isInSet(dst, src->items[i])){
			insertIntoSet(dst, src->items[i]);
		}
	}
	return 0;
}




static int printSet(struct ff_set *set){
	int i;
	printf("SET: ");
	for(i = 0; i < set->count; i++){
		if(i != 0){
			printf(", ");
		}
		printf("%d", set->items[i]);
	}
	printf("\n");
	return 0;
}

static int testInsert(struct ff_set *set, int value){
	if(isInSet(set, value)){
		printf("%d is already in set\n", value);
		return -1;
	}
	insertIntoSet(set, value);
	if(!isInSet(set, value)){
		printf("%d isn't in set\n", value);
		return -1;
	}
	printSet(set);
	return 0;
}


static int insertTests(){

	struct ff_set set;

	memset(&set, 0, sizeof(set));

	testInsert(&set, 4);
	testInsert(&set, 3);
	testInsert(&set, 2);
	testInsert(&set, 6);
	testInsert(&set, 7);
	testInsert(&set, 8);
	testInsert(&set, 5);
	return 0;
}

static int testInsertUnion(){
	int i;
	struct ff_set a;
	struct ff_set b;

	memset(&a, 0, sizeof(a));
	memset(&b, 0, sizeof(a));

	for(i = 0; i < 10; i++){
		if(i % 2 == 0){
			insertIntoSet(&a, i);
		} else {
			insertIntoSet(&b, i);
		}
	}
	printf("before:\n");
	printSet(&a);
	printSet(&b);
	insertUnionIntoSet(&a, &b);
	printf("after\n");
	printSet(&a);
	return 0;
}

static int testSetSizeLimit(){
	int i;
	struct ff_set set;

	memset(&set, 0, sizeof(set));

	for(i = 64; i > 0; i--){
		insertIntoSet(&set, i);
		printSet(&set);
	}
	return 0;
}



int isSubsetNullable(int ruleIndex, int start, int end){
	int i;

	struct rule *r = &rules[ruleIndex];

	printf("Rule: %d  length: %d  Start: %d  End: %d\n", ruleIndex, r->bodyLength, start, end);
	if(end < start){
		return 1;
	}

	for(i = start; i <= end; i++){
		struct term *t = &r->body[i];
		if(t->type == TERMTYPE_TERMINAL){
			return 0;
		} else if(ffNodes[t->index].isNullable == 0){
			return 0;
		}
	}
	return 1;
}

static int calculateFirstFollow(){
	int ruleIndex;
	int i, j;
	int changed;
	int iterations;

	for(i = 0; i < nonterminalCount; i++){
		struct ff_node *n = &ffNodes[i];
		n->nonterminal = i;
		n->isNullable = 0;
		memset(&n->firstSet, 0, sizeof(n->firstSet));
		memset(&n->followSet, 0, sizeof(n->followSet));
		ffNodeCount++;
	}

	iterations = 0;
	changed = 1;
	while(changed){
		changed = 0;

		for(ruleIndex = 0; ruleIndex < ruleCount; ruleIndex++){
			struct rule *r = &rules[ruleIndex];
			int X = r->nonterminalIndex;
			struct term *Y = r->body;
			int k = r->bodyLength;


			if(ffNodes[r->nonterminalIndex].isNullable == 0){
				if(isSubsetNullable(ruleIndex, 0, r->bodyLength - 1)){
					ffNodes[r->nonterminalIndex].isNullable = 1;
					changed = 1;
				}
			}

			for(i = 0; i < r->bodyLength; i++){
				struct term *t = &r->body[i];
				if(isSubsetNullable(ruleIndex, 0, i - 1)){
					if(t->type == TERMTYPE_TERMINAL){
					} else if(t->type == TERMTYPE_NONTERMINAL){
					} else {
						assert(0);
					}
				}
			}
		}
		iterations++;
		printf("Iterations: %d\n", iterations);
	}
	printf("RuleCount: %d\n", ruleCount);

	return 0;
}

int firstfollow(){
	int test = 0;
	if(test){
		insertTests();
		testInsertUnion();
		testSetSizeLimit();
	}
	calculateFirstFollow();
	return 0;
}
