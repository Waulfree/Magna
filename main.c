#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <math.h>

#define STACK_LEN 32

enum tok_type {
	TOK_NONE,
	TOK_OP,
	TOK_ARG,
	TOK_END,

	TOK_SIZE
};

const char *tok_names[] = {
	"TOK_NONE",
	"TOK_OP",
	"TOK_ARG",
	"TOK_END"
};

enum op_type {
	OP_NOP,
	OP_END,
	OP_EQU,
	OP_ADD,
	OP_SUB,
	OP_MUL,
	OP_DIV,
	OP_POW,
	OP_OPN,
	OP_CLS,

	OP_ARG,

	OP_SIZE
};

const char *op_names[] = {
	"OP_NOP",
	"OP_END",
	"OP_EQU",
	"OP_ADD",
	"OP_SUB",
	"OP_MUL",
	"OP_DIV",
	"OP_POW",
	"OP_OPN",
	"OP_CLS"
};

int prec[] = {-1, 1, 2, 3, 3, 4, 4, 5, 0, 0};

/*
enum fsm_state {
	PDA_INIT,
	PDA_OPERATOR,
	PDA_OPERAND,
	PDA_END,

	PDA_SIZE
};*/

void tokenize_next(enum op_type *op, const char **lex)
{
	if (*op != OP_NOP)
		(*lex)++;
	while (**lex == ' ') (*lex)++;

	switch (**lex) {
	case '=':
		*op = OP_EQU;
		break;
	case '+':
		*op = OP_ADD;
		break;
	case '-':
		*op = OP_SUB;
		break;
	case '*':
		*op = OP_MUL;
		break;
	case '/':
		*op = OP_DIV;
		break;
	case '^':
		*op = OP_POW;
		break;
	case '(':
		*op = OP_OPN;
		break;
	case ')':
		*op = OP_CLS;
		break;

	case ';':
	case '\0':
		*op = OP_NOP;
		break;
	
	default:
		*op = OP_ARG;
		break;
	}
}

#define OP_STACK_IS_NOT_EMPTY op_top < &op_stack[STACK_LEN]
#define OP_STACK_IS_EMPTY op_top == &op_stack[STACK_LEN]
#define TOP_P prec[*op_top]/*.prior*/
#define CUR_P prec[op]/*.prior*/
#define LOPR *(lex_top + 1)
#define ROPR *lex_top

void parse(const char *str)
{
	enum op_type op = OP_NOP;
	const char *lex = str;
	
	enum op_type op_stack[STACK_LEN];
	enum op_type *op_top = &op_stack[STACK_LEN - 1];

	int lex_stack[STACK_LEN];
	int *lex_top = &lex_stack[STACK_LEN];
	*op_top = OP_NOP;

next_token:
	if (*lex)
		tokenize_next(&op, &lex);
	else
		goto end;

	/* Inductor handlers */
	switch (op) {
	/* Argument handler */
	case OP_ARG:
		*--lex_top = atoi(lex);
		goto next_token;

	/* Operator handlers */
	case OP_EQU:
	case OP_POW:
	case OP_OPN:
		goto induce;
	case OP_CLS:
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
	case OP_NOP:
		printf("end\n");
		goto next_token;
	case OP_OPN:
		op_top++;
		goto next_token;

	case OP_EQU:
	case OP_ADD:
		printf("%i + %i = ", LOPR, ROPR);
		LOPR += ROPR;
		goto bop;
	case OP_SUB:
		printf("%i - %i = ", LOPR, ROPR);
		LOPR -= ROPR;
		goto bop;
	case OP_MUL:
		printf("%i * %i = ", LOPR, ROPR);
		LOPR *= ROPR;
		goto bop;
	case OP_DIV:
		printf("%i / %i = ", LOPR, ROPR);
		LOPR /= ROPR;
		goto bop;
	case OP_POW:
		printf("%i ^^ %i = ", LOPR, ROPR);
		LOPR = pow(LOPR, ROPR);
bop:
		printf("%i\n", LOPR);
		op_top++;
		lex_top++;
		goto reduce;

	default:	
		printf("%s\n", op_names[*op_top]);
		op_top++;
		goto reduce;
	}

induce:
	*--op_top = op;
	goto next_token;

end:
	return;
}

int main()
{
	parse("1 + 2 * 3 + (4 + 2 - (3 - 1)) * 3 + 4^3^2 * 7");

	return 0;
}
