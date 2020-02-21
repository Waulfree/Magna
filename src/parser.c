#ifdef MAGNA_MODULE
#include "lexer.h"
#include "parser.h"
#endif

#define TOK_STACK_IS_NOT_EMPTY op_top < &op_stack[STACK_LEN]
#define TOK_STACK_IS_EMPTY op_top == &op_stack[STACK_LEN]
#define TOP_P prec[*op_top]/*.prior*/
#define CUR_P prec[tok.type]/*.prior*/
#define LOPR (arg_top + 1)->i
#define ROPR arg_top->i

int prec[] = {2, 3, 3, 4, 0, 0, 1, 0, 0, 0, 4, 5, -1};

void parse(const char *str)
{
	struct token tok;
	int numargs = 0;
	
	enum tok_type op_stack[STACK_LEN];
	enum tok_type *op_top = &op_stack[STACK_LEN - 1];
	union unitype arg_stack[STACK_LEN];
	union unitype *arg_top = &arg_stack[STACK_LEN];
	
	tok.lex.begin = tok.lex.end = (char *)str;
	tok.type = TOK_SIZE;
	*op_top = TOK_EOF;

next_token:
	if (tok.type == TOK_EOF) {
		printf("Fatal error: unexpected eof.\n");
		goto end;
	} else {
		tokenize_next(&tok);
	}
		
	/* Inductor handlers */
	switch (tok.type) {
	/* Argument handler */
	case TOK_IDENT:
	case TOK_NUM:
		*--arg_top = tok.val;
		numargs++;
		goto next_token;

	/* Operator handlers */
	case TOK_EQU:
	case TOK_POW:
	case TOK_ENTER:
		goto induce;
	case TOK_LEAVE:
		goto reduce;
	
	/* Binary operators */
	default:
		if (TOP_P >= CUR_P && numargs > 0) {
			goto reduce;
		}
		goto induce;
	}

reduce:
	if (TOP_P < CUR_P)
		goto induce;

	/* Reductor operator handlers */
	switch (*op_top) {
	case TOK_EOF:
		printf("end\n");
		goto end;
	case TOK_ENTER:
		op_top++;
		goto next_token;

#define	BOP_EVAL(op, arg, name) if (numargs < 2) \
		goto next_token; \
		printf("%i %s %i = ", LOPR, name, ROPR); \
		LOPR op arg; \
		goto bop;
	case TOK_EQU:
		/* TODO */
	case TOK_ADD:
		BOP_EVAL(+=, ROPR, "+")
	case TOK_SUB:
		BOP_EVAL(-=, ROPR, "-")
	case TOK_MUL:
		BOP_EVAL(*=, ROPR, "*")
	case TOK_DIV:
		BOP_EVAL(/=, ROPR, "*")
	case TOK_POW:
		BOP_EVAL(=, pow(LOPR, ROPR), "^^");
bop:	
		printf("%i\n", LOPR);
		numargs--;;
		op_top++;
		arg_top++;
		goto reduce;

	default:	
		printf("%s\n", tok_names[*op_top]);
		op_top++;
		goto reduce;
	}

induce:
	*--op_top = tok.type;
	goto next_token;

end:
	return;
}
