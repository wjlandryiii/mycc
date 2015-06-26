/*
 * Copyright 2015 Joseph Landry All Rights Reserved
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "list.h"
#include "set.h"


struct nfa_state {
	
	struct pointer_set *
};

struct nfa {
	struct nfa_state *startingState;
	struct pointer_set *acceptingStates;	
};
