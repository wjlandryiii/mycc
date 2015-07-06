/*
 * Copyright 2015 Joseph Landry All Rights Reserved
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>



enum MACRO_TYPES {
	OBJECT_MACRO,
	FUNCTION_MACRO,
};

struct macro {
	char *name;
	int type;
	struct pptoken_list *replacementList;
};

struct macro *newObjectMacro(char *name, struct list *replacementList){
	struct macro *macro;

	macro = calloc(1, sizeof(*macro));
	assert(macro != NULL);

	macro->name = name;
	macro->replacementList = replacementList;
	return macro;
}

struct list *objectMacroReplace(struct macro *macro, struct rtoken token){
	
}


#ifdef TESTING


int main(int argc, char *argv[]){

	return 0;
}

#endif
