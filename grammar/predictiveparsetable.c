/*
 * Copyright 2015 Joseph Landry All Rights Reserved
 */

#include <assert.h>
#include "parser.h"
#include "firstfollow.h"


int PPTABLE[MAX_SYMBOLS][MAX_SYMBOLS];

int predictiveparsetable(){
	int i, j;
	int rule;

	for(i = 0; i < MAX_SYMBOLS; i++){
		for(j = 0; j < MAX_SYMBOLS; j++){
			PPTABLE[i][j] = -1;
		}
	}

	for(rule = 0; rule < nRULES; rule++){
		int ruleName = RULENAME[rule];
		int nullable = RULENULLABLE[rule];

		for(i = 0; i < RULEFIRSTSIZE[rule]; i++){
			int first = RULEFIRST[rule][i];
			assert(PPTABLE[ruleName][first] < 0);
			PPTABLE[ruleName][first] = rule;
		}

		if(RULENULLABLE[rule]){
			for(i = 0; i < FOLLOWSIZE[ruleName]; i++){
				int follow = FOLLOW[ruleName][i];
				assert(PPTABLE[ruleName][follow] < 0);
				PPTABLE[ruleName][follow] = rule;
			}
		}
	}
	return 0;
}
