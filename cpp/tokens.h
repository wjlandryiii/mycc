/*
 * Copyright 2015 Joseph Landry All Rights Reserved
 */

#ifndef TOKENS_H
#define TOKENS_H

struct pptoken {
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
/* line    */	PPTN_LINE,
/* error   */	PPTN_ERROR,
/* pragma  */	PPTN_PRAGMA,
/*  \t     */	PPTN_WHITESPACE,
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

extern char *PPTOKEN_STRINGS[PPTN_NTOKENS];
void printPPToken(struct pptoken pptoken);

#endif
