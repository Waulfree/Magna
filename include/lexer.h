#ifndef MAGNA_LEXER_HEADER_GUARD

#include <stdint.h>

enum tok_type {
	/* Tokens that are compatible with char_type */
	TOK_EQU = CHAR_EQUALS,
	TOK_SUB,
	TOK_ADD,
	TOK_MUL,
	TOK_ENTER,
	TOK_LEAVE,
	TOK_ENDSTAT,
	
	/* Free tokens */
	TOK_COMMENT,
	TOK_IDENT,
	TOK_NUM,
	TOK_DIV,
	TOK_POW,
	TOK_EOF,

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
	
	struct line line;
	struct lex lex;
};

#ifdef MAGNA_MODULE
/*extern void tokenize_next(enum tok_type *, const char **);*/
extern void tokenize_next(struct token **);
#else
#include "lexer.c"
#endif

#define MAGNA_LEXER_HEADER_GUARD
#endif
