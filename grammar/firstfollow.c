/*
 *	 Copyright 2015 Joseph Landry All Rights Reserved
 */

#include <stdlib.h>

#include "symbols.h"
#include "parser.h"
#include "firstfollow.h"


struct ff_node *firstFollowSet;

static int isNullable(int symbolIndex){
	struct ff_node *p = firstFollowSet;

	while(p){
		if(p->nonTerminalSymbolIndex == symbolIndex){
			return p->isNullable;
		}
		p = p->next;
	}
	return 0;
}

static int setIsNullable(int symbolIndex){
	struct ff_node *p = firstFollowSet;

	while(p){
		if(p->nonTerminalSymbolIndex == symbolIndex){
			p->isNullable = 1;
			return 0;
		}
		p = p->next;
	}
	return -1;
}

static int setFirstLiteral(int symbolIndex, int literal){
	struct ff_node *p = firstFollowSet;

	while(p){
		if(p->nonTerminalSymbolIndex == symbolIndex){
			struct ff_set_node *set = p->first;

			while(set){
				if(set->symbolIndex == literal){
					return 0;
				}
				set = set->next;
			}
			// NOT FOUND
			set = malloc(sizeof(*set));
			set->symbolIndex = literal;
			set->next = p->first;
			p->first = set;
			return 1;
		}
		p = p->next;
	}
	return 0;
}

static int unionFirstSets(int symbolIndex, int withSymbolIndex){
	struct ff_node *p = firstFollowSet;

	p = firstFollowSet;
	while(p){
		if(p->nonTerminalSymbolIndex == withSymbolIndex){
			struct ff_set_node *set = p->first;
			int change = 0;

			while(set){
				change |= setFirstLiteral(symbolIndex, set->symbolIndex);
				set = set->next;
			}
			return change;
		}
		p = p->next;
	}
	// DIDN'T FIND withSymbolIndex
	return 0;
}

static int initFirstFollow(){
	struct ff_node *list = 0;
	struct ff_node **parent = &list;

	//while(p){
	int nonterminalIndex;
	for(nonterminalIndex = 0; nonterminalIndex < nonterminalCount; nonterminalIndex++){
		struct ff_node *l = malloc(sizeof(struct ff_node));

		if(l == 0){
			// memory error
			return -1;
		}

		*parent = l;
		l->next = 0;
		l->nonTerminalSymbolIndex = nonterminalIndex;
		l->isNullable = 0;
		l->first = 0;
		l->follow = 0;

		parent = &l->next;
	}
	firstFollowSet = list;
	return 0;
}


int firstfollow(){
	int ruleIndex;
	int changed;

	initFirstFollow();

	changed = 1;
	while(changed){
		changed = 0;

		ruleIndex = 0;
		for(ruleIndex = 0; ruleIndex < ruleCount; ruleIndex++){
			int bodyIndex;
			int symbol = rules[ruleIndex].nonterminalIndex;
			int bodyLength = rules[ruleIndex].bodyLength;
			struct term *body = rules[ruleIndex].body;

			int allNullable = 1;

			for(bodyIndex = 0; bodyIndex < bodyLength; bodyIndex++){
				int type = body[bodyIndex].type;
				int index = body[bodyIndex].index;

				if(allNullable){
					if(type == TERMTYPE_NONTERMINAL){
						changed |= unionFirstSets(symbol, index);
					} else if(type == TERMTYPE_TERMINAL){
						changed |= setFirstLiteral(symbol, index);
					}
				}

				if(type == TERMTYPE_NONTERMINAL){
					allNullable &= isNullable(index);
				} else if(type == TERMTYPE_TERMINAL){
					allNullable = 0;
				} else {
					// TODO: was sigma
					// allNullable &= 1;
					;
				}
			}
			if(allNullable && !isNullable(symbol)){
				setIsNullable(symbol);
				changed = 1;
			}
		}
	}

	return 0;
}
