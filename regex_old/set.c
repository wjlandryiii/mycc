/*
 * Copyright 2015 Joseph Landry All Rights Reserved
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


struct symbol_set {
	short int ranges[128][2];
	int nRanges;
};

struct symbol_set symSetSingle(int symbol){
	struct symbol_set set;

	set.nRanges = 1;
	set.ranges[0][0] = symbol;
	set.ranges[0][1] = symbol;
	return set;
}

struct symbol_set symSetRange(int start, int end){
	struct symbol_set set;

	set.nRanges = 1;
	set.ranges[0][0] = start;
	set.ranges[0][1] = end;
	return set;
}


struct symbol_set symSetUnion(struct symbol_set s1, struct symbol_set s2){
	struct symbol_set set;
	int i, j;

	set.nRanges = 0;
	i = 0;
	j = 0;

	while(i < s1.nRanges && j < s.nRanges){
		if(s1.ranges[i][0] < s2.ranges[j][1]){
			set.ranges[set.nRanges][0] = s1.ranges[i][0];
			set.ranges[set.nRanges][1] = s1.ranges[i][0];
			set.nRanges += 1;
			i += 1;
		} else if(s2.ranges[j][0] > s1.ranges[i][1]){
			set.ranges[set.nRanges][0] = s2.ranges[j][0];
			set.ranges[set.nRanges][1] = s2.ranges[j][0];
			set.nRanges += 1;
			j += 1;
		} else {
			int min;
			int max;

			min = s1.ranges[i][0] < s2.ranges[i][0] ? s1.ranges[i][0] : s2.ranges[i][0];
			max = s1.ranges[i][1] > s2.ranges[i][1] ? s1.ranges[i][1] : s2.ranges[i][1];
i
			set.ranges[set.nRanges][0] = min;
			set.ranges[set.nRanges][1] = max;
			set.nRanges += 1;
			i += 1;
			j += 1;
		}
	}

	while(i < s1.nRanges){
		set.ranges[set.nRanges][0] = s1.ranges[i][0];
		set.ranges[set.nRanges][1] = s1.ranges[i][1];
		set.nRanges += 1;
		i += 1;
	}

	while(j < s2.nRanges){
		set.ranges[set.nRanges][0] = s2.ranges[j][0];
		set.ranges[set.nRanges][1] = s2.ranges[j][1];
		set.nRanges += 1;
		j += 1;
	}
	return set;
}


void test_set(void){
	struct index_set set = {0};

	printf("is 0 in set: %d\n", isInIndexSet(&set, 0));
	printf("insert 0\n"); addIndex(&set, 0);
	printf("is 0 in set: %d\n", isInIndexSet(&set, 0));
}

int main(int argc, char *argv[]){
	test_set();
	return 0;
}
