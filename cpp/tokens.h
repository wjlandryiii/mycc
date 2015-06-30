/*
 * Copyright 2015 Joseph Landry All Rights Reserved
 */

#ifndef TOKENS_H
#define TOKENS_H

struct pptoken {
	char *whiteSpace;
	char *lexeme;
	int type;
	int name;
};

enum PPTOKEN_NAMES {
/* EOF     */	PPTN_EOF,
/* <foo.h> */	PPTN_HEADERNAME,
/* ABCD    */	PPTN_IDENTIFIER,
/* 1234    */	PPTN_PPNUMBER,
/* 'A'     */	PPTN_CHARACTERCONSTANT,
/* "asdf"  */	PPTN_STRINGLITERAL,
/* if      */	PPTN_IF,
/* ifdef   */	PPTN_IFDEF,
/* ifndef  */	PPTN_IFNDEF,
/* elif    */	PPTN_ELIF,
/* else    */	PPTN_ELSE,
/* endif   */	PPTN_ENDIF,
/* include */	PPTN_INCLUDE,
/* define  */	PPTN_DEFINE,
/* undef   */	PPTN_UNDEF,
/* line    */	PPTN_LINE,
/* error   */	PPTN_ERROR,
/* pragma  */	PPTN_PRAGMA,
/* \n      */	PPTN_NEWLINE,
/* !=      */	PPTN_NE_OP,
/* !       */	PPTN_EXLAMATIONMARK,
/* ##      */	PPTN_GLUE_OP,
/* #       */   PPTN_HASH,
/* &&      */   PPTN_AND_OP,
/* &=      */   PPTN_AND_ASSIGN,
/* &       */	PPTN_AMPERSAND,
/* %=      */	PPTN_MOD_ASSIGN,
/* %       */	PPTN_PERCENT,
/* (       */	PPTN_LPAREN,
/* )       */	PPTN_RPAREN,
/* *=      */	PPTN_MUL_ASSIGN,
/* *       */   PPTN_ASTERISK,
/* ++      */	PPTN_INC_OP,
/* +=      */	PPTN_ADD_ASSIGN,
/* +       */   PPTN_PLUS_SIGN,
/* ,       */	PPTN_COMMA,
/* --      */	PPTN_DEC_OP,
/* -=      */	PPTN_SUB_ASSIGN,
/* ->      */	PPTN_PTR_OP,
/* -       */	PPTN_MINUS_SIGN,
/* ...     */	PPTN_ELLIPSIS,
/* .       */	PPTN_PERIOD,
/* /=      */	PPTN_DIV_ASSIGN,
/* /       */	PPTN_SLASH,
/* :       */	PPTN_COLON,
/* ;       */	PPTN_SEMICOLON,
/* <<=     */	PPTN_SL_ASSIGN,
/* <<      */	PPTN_SL_OP,
/* <=      */	PPTN_LE_OP,
/* <       */	PPTN_LCHEVRON,
/* ==      */	PPTN_EQ_OP,
/* =       */	PPTN_EQUAL,
/* >>=     */	PPTN_SR_ASSIGN,
/* >>      */	PPTN_SR_OP,
/* >=      */	PPTN_GE_OP,
/* >       */	PPTN_RCHEVRON,
/* ?       */	PPTN_QUESTION_MARK,
/* [       */	PPTN_LBRACKET,
/* ]       */	PPTN_RBRACKET,
/* ^=      */	PPTN_XOR_ASSIGN,
/* ^       */	PPTN_CIRCUMFLEX,
/* {       */	PPTN_LBRACE,
/* |=      */	PPTN_OR_ASSIGN,
/* ||      */	PPTN_OR_OP,
/* |       */	PPTN_PIPE,
/* }       */	PPTN_RBRACE,
/* ~       */	PPTN_TILDE,
		PPTN_NTOKENS,
};

/*
 * preprocessing-token:
 *	header-name
 *	identifier
 *	pp-number
 *	character-constant
 *	string-literal
 *	punctuator
 *	whitespace
 *	newline
 */

enum LEXEME_TYPES {
	LT_EMPTY,
	LT_HEADERNAME,
	LT_IDENTIFIER,
	LT_PPNUMBER,
	LT_CHARACTERCONSTANT,
	LT_STRINGLITERAL,
	LT_PUNCTUATOR,
	LT_NEWLINE,
};

extern char *PPTOKEN_STRINGS[PPTN_NTOKENS];
void printPPToken(struct pptoken pptoken);


struct pptoken_queue;

struct pptoken_queue *newPPTokenQueue();
void freePPTokenQueue(struct pptoken_queue *queue);
int ppTokenQueueEnqueue(struct pptoken_queue *queue, struct pptoken token);
int ppTokenQueueDequeue(struct pptoken_queue *queue, struct pptoken *tokenOut);


struct pptoken_list;

struct pptoken_list *newPPTokenList();
void freePPTokenList(struct pptoken_list *list);
int ppTokenListAtIndex(struct pptoken_list *list, int index, struct pptoken *tokenOut);
int ppTokenListAppend(struct pptoken_list *list, struct pptoken token);


#endif
