#ifdef MAGNA_MODULE
#include "lexer.h"
#include "parser.h"
#endif

#define TOK_STACK_IS_NOT_EMPTY op_top < &op_stack[STACK_LEN]
#define TOK_STACK_IS_EMPTY op_top == &op_stack[STACK_LEN]
#define TOP_P prec[*op_top]/*.prior*/
#define CUR_P prec[op]/*.prior*/
#define LOPR *(lex_top + 1)
#define ROPR *lex_top

int prec[] = {-1, 1, 2, 3, 3, 4, 4, 5, 0, 0};

void parse(const char *str)
{
	enum tok_type op = TOK_NOP;
	const char *lex = str;
	
	enum tok_type op_stack[STACK_LEN];
	enum tok_type *op_top = &op_stack[STACK_LEN - 1];

	int lex_stack[STACK_LEN];
	int *lex_top = &lex_stack[STACK_LEN];
	*op_top = TOK_NOP;

next_token:
	if (*lex)
		tokenize_next(&op, &lex);
	else
		goto end;

	/* Inductor handlers */
	switch (op) {
	/* Argument handler */
	case TOK_ARG:
		*--lex_top = atoi(lex);
		goto next_token;

	/* Operator handlers */
	case TOK_EQU:
	case TOK_POW:
	case TOK_OPN:
		goto induce;
	case TOK_CLS:
		goto reduce;

	default:
		if (TOP_P >= CUR_P)
			goto reduce;
		goto induce;
	}

reduce:
	if (TOP_P < CUR_P)
		goto induce;

	/* Reductor operator handlers */
	switch (*op_top) {
	case TOK_NOP:
		printf("end\n");
		goto next_token;
	case TOK_OPN:
		op_top++;
		goto next_token;

	case TOK_EQU:
	case TOK_ADD:
		printf("%i + %i = ", LOPR, ROPR);
		LOPR += ROPR;
		goto bop;
	case TOK_SUB:
		printf("%i - %i = ", LOPR, ROPR);
		LOPR -= ROPR;
		goto bop;
	case TOK_MUL:
		printf("%i * %i = ", LOPR, ROPR);
		LOPR *= ROPR;
		goto bop;
	case TOK_DIV:
		printf("%i / %i = ", LOPR, ROPR);
		LOPR /= ROPR;
		goto bop;
	case TOK_POW:
		printf("%i ^^ %i = ", LOPR, ROPR);
		LOPR = pow(LOPR, ROPR);
bop:
		printf("%i\n", LOPR);
		op_top++;
		lex_top++;
		goto reduce;

	default:	
		printf("%s\n", tok_names[*op_top]);
		op_top++;
		goto reduce;
	}

induce:
	*--op_top = op;
	goto next_token;

end:
	return;
}
