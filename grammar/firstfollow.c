/*
 * Copyright 2015 Joseph Landry All Rights Reserved
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

int firstfollow(){
	struct rule_set *p = parseTree;
	struct ff_node *list = 0;
	struct ff_node **parent = &list;
	int changed;

	while(p){
		struct ff_node *l = malloc(sizeof(struct ff_node));
		
		if(l == 0){
			// memory error
			return -1;
		}

		*parent = l;
		l->next = 0;
		l->nonTerminalSymbolIndex = p->ruleNameSymbol;
		l->isNullable = 0;
		l->first = 0;
		l->follow = 0;

		p = p->nextRuleSet;
		parent = &l->next;
	}

	firstFollowSet = list;
	changed = 1;
	while(changed){
		changed = 0;

		p = parseTree;
		while(p){
			int symbol = p->ruleNameSymbol;
			struct rule_list *ruleList = p->ruleList;

			while(ruleList){
				struct term_list *termList = ruleList->termList;
				int allNullable = 1;
				while(termList){
					int type = symbolList[termList->termSymbol].type;
					if(allNullable){
						int sindex;
						sindex = termList->termSymbol;
						type = symbolList[sindex].type;
						if(type == SYMTYPE_NONTERMINAL){
							// union FIRST(Yi)
							changed |= unionFirstSets(symbol, sindex);
						} else if(type == SYMTYPE_TERMINAL){
							changed |= setFirstLiteral(symbol, sindex);
						}
					}

					type = symbolList[termList->termSymbol].type;
					if(type == SYMTYPE_NONTERMINAL){
						allNullable &= isNullable(termList->termSymbol);
					} else if(type == SYMTYPE_TERMINAL){
						allNullable = 0;
					} else if(type == SYMTYPE_SIGMA){
						allNullable &= 1;
					}

					/*
					if(allNullable && termList->nextTermList){
						int sindex;
						sindex = termList->nextTermList->termSymbol;
						type = symbolList[sindex].type;
						if(type == SYMTYPE_NONTERMINAL){
							// union FIRST(Yi)
							changed |= unionFirstSets(symbol, sindex);
						} else if(type == SYMTYPE_TERMINAL){
							changed |= setFirstLiteral(symbol, sindex);
						}
					}
					*/
					termList = termList->nextTermList;
				}
				if(allNullable && !isNullable(symbol)){
					setIsNullable(symbol);
					changed = 1;
				}
				ruleList = ruleList->nextRuleList;
			}
			p = p->nextRuleSet;
		}
	}



	return 0;
}
