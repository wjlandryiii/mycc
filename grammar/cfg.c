/*
 * Copyright 2015 Joseph Landry
 */

struct term {
	char *term;
	int type;
	struct term *nextTerm;
};

struct rule {
	struct term *firstTerm;	
	struct rule *nextRule;
};

struct non_terminal {
	char *name;
	struct rule *firstRule;
};

struct grammar {
	struct non_terminal *firstNonTerminal;
};

struct context_free_grammar {
	struct string_table *stringTable;
};

