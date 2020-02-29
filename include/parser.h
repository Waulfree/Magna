#ifndef MAGNA_PARSER_HEADER_GUARD

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <math.h>

#define STACK_LEN 32

enum reductor_mode {
	REDUCE_STATEMENT, /* Reduce every operator in statement */
	REDUCE_INFIX_EXPRESSION, /* Reduce only top priority operators */
	REDUCE_POSTFIX_EXPRESSION,
	REDUCE_PREFIX_EXPRESSION
};

/*
enum fsm_state {
	PDA_INIT,
	PDA_OPERATOR,
	PDA_OPERAND,
	PDA_END,

	PDA_SIZE
};*/

#ifdef MAGNA_MODULE
extern void parse(const char *);
#else
#include "parser.c"
#endif

#define MAGNA_PARSER_HEADER_GUARD
#endif
