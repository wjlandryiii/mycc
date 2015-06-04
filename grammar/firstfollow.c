/*
 *	 Copyright 2015 Joseph Landry All Rights Reserved
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "firstfollow.h"


// INPUT
int nSYMBOLS = 0;
int SYMBOL[MAX_SYMBOLS];
int SYMBOLTYPE[MAX_SYMBOLS];

int nRULES = 0;
int RULENAME[MAX_RULES];
int RULE[MAX_RULES][MAX_RULE_SIZE];
int RULESIZE[MAX_RULES];

// OUTPUT
int NULLABLE[MAX_SYMBOLS];

int FIRST[MAX_SYMBOLS][MAX_FIRST_SET_SIZE];
int FIRSTSIZE[MAX_SYMBOLS];

int FOLLOW[MAX_SYMBOLS][MAX_FOLLOW_SET_SIZE];
int FOLLOWSIZE[MAX_SYMBOLS];


void computeNullable(){
	int rule, i, j;
	int changed;
	int iteration;

	for(i = 0; i < nSYMBOLS; i++){
		if(SYMBOLTYPE[i] == TYPE_TERMINAL){
			FIRST[i][0] = i;
			FIRSTSIZE[i] = 1;
		}
	}

	iteration = 1;
	changed = 1;
	while(changed){
		//printf("    *** ITERATION: %d ***\n", iteration);
		changed = 0;
		for(rule = 0; rule < nRULES; rule++){
			if(NULLABLE[RULENAME[rule]] == 0){
				for(i = 0; i < RULESIZE[rule]; i++){
					if(!NULLABLE[RULE[rule][i]]){
						break;
					}
				}
				if(i == RULESIZE[rule]){
					NULLABLE[RULENAME[rule]] = 1;
					changed = 1;
				}
			}
		}
		iteration += 1;
	}
	//printf("\n");
}

static int unionFirst(int dst, int src){
	int i, j;
	int changed = 0;

	for(i = 0; i < FIRSTSIZE[src]; i++){
		int symbol = FIRST[src][i];

		for(j = 0; j < FIRSTSIZE[dst]; j++){
			if(FIRST[dst][j] == symbol){
				break;
			}
		}
		if(j == FIRSTSIZE[dst]){
			assert(FIRSTSIZE[dst] < MAX_FIRST_SET_SIZE);
			FIRST[dst][j] = symbol;
			FIRSTSIZE[dst] += 1;
			changed = 1;
		}
	}
	return changed;
}

void computeFirst(){
	int rule, i, j;
	int changed;
	int iteration = 1;

	do {
		//printf("    *** ITERATION: %d ***\n", iteration);
		changed = 0;

		for(rule = 0; rule < nRULES; rule++){
			//printf("RULE: %d\n", rule);
			for(i = 0; i < RULESIZE[rule]; i++){
				if(unionFirst(RULENAME[rule], RULE[rule][i])){
					changed = 1;
				}
				if(!NULLABLE[RULE[rule][i]]){
					break;
				}
			}
		}
		iteration += 1;
	} while(changed);
}


static int unionFollow(int dst, int src){
	int i,j;
	int changed = 0;

	for(i = 0; i < FOLLOWSIZE[src]; i++){
		int symbol = FOLLOW[src][i];

		for(j = 0; j < FOLLOWSIZE[dst]; j++){
			if(FOLLOW[dst][j] == symbol){
				break;
			}
		}
		if(j == FOLLOWSIZE[dst]){
			assert(FOLLOWSIZE[dst] < MAX_FOLLOW_SET_SIZE);
			FOLLOW[dst][j] = symbol;
			FOLLOWSIZE[dst] += 1;
			changed = 1;
		}
	}
	return changed;
}


static int unionFollowFirst(int dst, int src){
	int i,j;
	int changed = 0;

	for(i = 0; i < FIRSTSIZE[src]; i++){
		int symbol = FIRST[src][i];

		for(j = 0; j < FOLLOWSIZE[dst]; j++){
			if(FOLLOW[dst][j] == symbol){
				break;
			}
		}
		if(j == FOLLOWSIZE[dst]){
			assert(FOLLOWSIZE[dst] < MAX_FOLLOW_SET_SIZE);
			FOLLOW[dst][j] = symbol;
			FOLLOWSIZE[dst] += 1;
			changed = 1;
		}
	}
	return changed;
}

void computeFollow(){
	int rule, i, j;
	int changed;
	int iteration = 1;

	do {
		//printf("    *** ITERATION: %d ***\n", iteration);
		changed = 0;

		for(rule = 0; rule < nRULES; rule++){

			for(i = RULESIZE[rule] - 1; i >= 0; i--){
				//printf("RULE: %d I: %d size: %d\n", rule, i, RULESIZE[rule]);
				if(unionFollow(RULE[rule][i], RULENAME[rule])){
					changed = 1;
				}
				if(!NULLABLE[RULE[rule][i]]){
					break;
				}
			}


			for(i = 0; i < RULESIZE[rule]; i++){
				for(j = i+1; j < RULESIZE[rule]; j++){
					if(unionFollowFirst(RULE[rule][i], RULE[rule][j])){
						changed = 1;
					}
					if(!NULLABLE[RULE[rule][j]]){
						break;
					}
				}
			}
		}
	} while(changed);
}

