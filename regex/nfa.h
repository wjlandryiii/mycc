/*
 * Copyright 2015 Joseph Landry
 */

#ifndef NFA_H
#define NFA_H


#define NFAEPSILON (-1)

struct transition {
	int from;
	int to;
	char symbol;
};

struct nfa {
	int startingState;
	int states;
	struct transition transitions[1024];
	int nTransitions;
	int acceptingStates[1024];
	int nAcceptingStates;
};

struct nfa elementaryOneCharacter(char c);
struct nfa nfaUnion(struct nfa *nfaA, struct nfa *nfaB);
struct nfa nfaConcatenation(struct nfa *nfaA, struct nfa *nfaB);
struct nfa nfaStar(struct nfa *nfaA);
int graphNFA(const struct nfa *nfa);

#endif