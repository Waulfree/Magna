#ifdef MAGNA_MODULE
#include "lexer.h"
#include "parser.h"
#endif

#define PUSH_OP *--op_top = tok.type
#define PUSH_ARG *--arg_top = tok.val
#define POP_OP op_top++
#define POP_ARG arg_top++
#define LPRIOR prior[*op_top]/*.prior*/
#define RPRIOR prior[tok.type]/*.prior*/
#define LOPR (arg_top + 1)->i
#define ROPR arg_top->i

/* Token priorities */
int prior[] = {
	2,		/*TOK_EQU*/
	3,		/*TOK_SUB*/
	3,		/*TOK_ADD*/
	4,		/*TOK_MUL*/
	PRIOR_UNDEF,	/*TOK_ENTER*/
	PRIOR_UNDEF,	/*TOK_LEAVE*/
	PRIOR_UNDEF,	/*TOK_ENDSTAT*/
	PRIOR_UNDEF,	/*TOK_COMMENT*/
	PRIOR_UNDEF,	/*TOK_STRING*/
	PRIOR_UNDEF,	/*TOK_IDENT*/
	PRIOR_UNDEF,	/*TOK_NUM*/
	4,		/*TOK_DIV*/
	5,		/*TOK_POW*/
	PRIOR_UNDEF	/*TOK_EOF*/
};

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

inductor:
	if (tok.type == TOK_EOF) {
		printf("Fatal error: unexpected EOF.\n");
		goto end;
	} else {
		tokenize_next(&tok);
	}

	/* Inductor handlers */
	switch (tok.type) {
	/* Argument handler */
	case TOK_STRING:
	case TOK_IDENT:
	case TOK_NUM:
		*--arg_top = tok.val;
		numargs++;
		goto inductor;

	/* Operator handlers */
	case TOK_EQU:
	case TOK_POW:
	case TOK_ENTER:
		PUSH_OP;
		goto inductor;
	case TOK_ENDSTAT:
	case TOK_LEAVE:
		goto reductor;
	
	/* Binary operators */
	default:
		if (LPRIOR >= RPRIOR) {
			goto reductor;
		}
		PUSH_OP;
		goto inductor;
	}

reductor:
	if (LPRIOR < RPRIOR) {
		PUSH_OP;
		goto inductor;
	}
	/* Reductor operator handlers */
	switch (*op_top) {
	case TOK_EOF:
		if (tok.type != TOK_EOF)
			goto inductor;
		printf("Sucess.\n\n");
		goto end;

	case TOK_ENTER:
		POP_OP;
		goto inductor;

#define	BOP_EVAL(op, arg, name) if (numargs < 2) {\
		PUSH_OP; \
		goto inductor; \
		} \
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
		numargs--;
		POP_ARG;
		POP_OP;
		goto reductor;

	default:	
		printf("%s\n", tok_names[*op_top]);
		POP_OP;
		goto reductor;
	}

end:
	return;
}
