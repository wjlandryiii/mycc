/*
 * Copyright 2015 Joseph Landry All Rights Reserved
 */

#include <assert.h>

#include "symbols.h"
#include "parser.h"
#include "firstfollow.h"


int NULLABLE[MAX_SYMBOLS];

int FIRST[MAX_SYMBOLS][MAX_FIRST_SET_SIZE];
int FIRSTSIZE[MAX_SYMBOLS];

int FOLLOW[MAX_SYMBOLS][MAX_FOLLOW_SET_SIZE];
int FOLLOWSIZE[MAX_SYMBOLS];

int RULENULLABLE[MAX_RULES];
int RULEFIRST[MAX_RULES][MAX_FIRST_SET_SIZE];
int RULEFIRSTSIZE[MAX_RULES];
int RULEFOLLOW[MAX_RULES][MAX_FOLLOW_SET_SIZE];
int RULEFOLLOWSIZE[MAX_RULES];


static void computeNullable(){
	int rule, i, j;
	int changed;
	int iteration;

	for(i = 0; i < nSYMBOLS; i++){
		if(SYMBOLTYPE[i] == SYMBOLTYPE_TERMINAL){
			FIRST[i][0] = i;
			FIRSTSIZE[i] = 1;
		}
	}

	iteration = 1;
	changed = 1;
	while(changed){
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

static void computeFirst(){
	int rule, i, j;
	int changed;
	int iteration = 1;

	do {
		changed = 0;

		for(rule = 0; rule < nRULES; rule++){
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

static void computeFollow(){
	int rule, i, j;
	int changed;
	int iteration = 1;

	do {
		changed = 0;

		for(rule = 0; rule < nRULES; rule++){
			for(i = RULESIZE[rule] - 1; i >= 0; i--){
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

static void computeRuleNullable(){
	int rule;
	int i;

	for(rule = 0; rule < nRULES; rule++){
		if(RULESIZE[rule] > 0){
			for(i = 0; i < RULESIZE[rule]; i++){
				if(!NULLABLE[RULE[rule][i]]){
					break;
				}
			}
			if(i == RULESIZE[rule]){
				RULENULLABLE[rule] = 1;
			} else {
				RULENULLABLE[rule] = 0;
			}
		} else {
			// Explicitly for Sigma
			RULENULLABLE[rule] = 1;
		}
	}
}

static void computeRuleFirst(){
	int rule;
	int i, j, k;

	for(rule = 0; rule < nRULES; rule++){
		for(i = 0; i < RULESIZE[rule]; i++){
			int symbol = RULE[rule][i];
			for(j = 0; j < FIRSTSIZE[symbol]; j++){
				int first = FIRST[symbol][j];
				for(k = 0; k < RULEFIRSTSIZE[rule]; k++){
					if(RULEFIRST[rule][k] == first){
						break;
					}
				}
				if(k == RULEFIRSTSIZE[rule]){
					assert(RULEFIRSTSIZE[rule] < MAX_FIRST_SET_SIZE);
					RULEFIRST[rule][RULEFIRSTSIZE[rule]] = first;
					RULEFIRSTSIZE[rule] += 1;
				}
			}

			if(!NULLABLE[RULE[rule][i]]){
				break;
			}
		}
	}
}

static void computeRuleFollow(){
	int rule;
	int i,j,k;

	// TODO: verify the corectness of this implementation.
	for(rule = 0; rule < nRULES; rule++){
		for(i = RULESIZE[rule] - 1; i >= 0; i++){
			int symbol = RULE[rule][i];
			for(j = 0; j < FOLLOWSIZE[symbol]; j++){
				int follow = FOLLOW[symbol][j];
				for(k = 0; k < RULEFOLLOWSIZE[rule]; k++){
					if(RULEFOLLOW[rule][k] == follow){
						break;
					}
				}
				if(k == RULEFOLLOWSIZE[rule]){
					assert(RULEFOLLOWSIZE[rule] < MAX_FOLLOW_SET_SIZE);
					RULEFOLLOW[rule][RULEFOLLOWSIZE[rule]] = follow;
					RULEFOLLOWSIZE[rule] += 1;
				}
			}

			if(!NULLABLE[RULE[rule][i]]){
				break;
			}
		}
	}
}

int firstfollow(){
	computeNullable();
	computeFirst();
	computeFollow();
	computeRuleNullable();
	computeRuleFirst();
	computeRuleFollow();
	return 0;
}
