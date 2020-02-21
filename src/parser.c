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
	
	enum tok_type op_stack[STACK_LEN];
	enum tok_type *op_top = &op_stack[STACK_LEN - 1];

	union unitype arg_stack[STACK_LEN];
	union unitype *arg_top = &arg_stack[STACK_LEN];
	
	tok.lex.begin = tok.lex.end = (char *)str;
	tok.type = TOK_SIZE;
	*op_top = TOK_EOF;

next_token:
	if (tok.type == TOK_EOF)
		goto end;
	else
		tokenize_next(&tok);
		
	/* Inductor handlers */
	switch (tok.type) {
	/* Argument handler */
	case TOK_NUM:
		*--arg_top = tok.val;
		goto next_token;

	/* Operator handlers */
	case TOK_EQU:
	case TOK_POW:
	case TOK_ENTER:
		goto induce;
	case TOK_LEAVE:
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
	case TOK_EOF:
		printf("end\n");
		goto next_token;
	case TOK_ENTER:
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
