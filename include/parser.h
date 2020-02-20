#ifndef MAGNA_PARSER_HEADER_GUARD

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <math.h>

#define STACK_LEN 32

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
