#ifndef MAGNA_LEXER_HEADER_GUARD

#include <stdint.h>

enum tok_type {
	TOK_NOP,
	TOK_END,
	TOK_EQU,
	TOK_ADD,
	TOK_SUB,
	TOK_MUL,
	TOK_DIV,
	TOK_POW,
	TOK_OPN,
	TOK_CLS,

	TOK_ARG,

	TOK_SIZE
};

const char *tok_names[] = {
	"TOK_NOP",
	"TOK_END",
	"TOK_EQU",
	"TOK_ADD",
	"TOK_SUB",
	"TOK_MUL",
	"TOK_DIV",
	"TOK_POW",
	"TOK_OPN",
	"TOK_CLS"
};

struct line {
	const char *begin;
	size_t num;
};

struct lex {
	char *begin;
	char *end;
};

struct ident {
	void *dummy;
	/* dummy */
};

union unitype {
	struct ident *id;
	struct lex lex;
	float f;
	int i;
};

struct token {
	enum tok_type type;
	union unitype val;
};

#ifdef MAGNA_MODULE
extern void tokenize_next(enum tok_type *, const char **);
#else
#include "lexer.c"
#endif

#define MAGNA_LEXER_HEADER_GUARD
#endif
