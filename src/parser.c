#ifdef MAGNA_MODULE
#include "lexer.h"
#include "parser.h"
#endif

#define PUSH_OP *--op_top = tok.type
#define PUSH_ARG *--arg_top = tok.val
#define POP_OP op_top++
#define POP_ARG arg_top++
#define LPRIOR op_pty[*op_top].prior
#define RPRIOR op_pty[tok.type].prior
#define LARGS op_pty[*op_top].numargs
#define RARGS op_pty[tok.type].numargs
#define LOPR (arg_top + 1)->i
#define ROPR arg_top->i

struct op_properties {
	int prior;
	int numargs;
};

struct op_properties op_pty[] = {
	{2, 2},			/*TOK_EQU*/
	{3, 2},			/*TOK_SUB*/
	{3, 2},			/*TOK_ADD*/
	{4, 2},			/*TOK_MUL*/
	{PRIOR_UNDEF, 0},	/*TOK_ENTER*/
	{PRIOR_UNDEF, 0},	/*TOK_LEAVE*/
	{PRIOR_UNDEF, 0},	/*TOK_ENDSTAT*/
	{PRIOR_UNDEF, 0},	/*TOK_COMMENT*/
	{PRIOR_UNDEF, 0},	/*TOK_STRING*/
	{PRIOR_UNDEF, 0},	/*TOK_IDENT*/
	{PRIOR_UNDEF, 0},	/*TOK_NUM*/
	{4, 2},			/*TOK_DIV*/
	{5, 2},			/*TOK_POW*/
	{PRIOR_UNDEF, 0}	/*TOK_EOF*/
};

void parse(const char *str)
{
	/* Declarations */
	enum reductor_mode mode;
	struct token tok;
	int numargs = 0;

	/* Stacks */
	enum tok_type op_stack[STACK_LEN];
	enum tok_type *op_top = &op_stack[STACK_LEN - 1];
	union unitype arg_stack[STACK_LEN];
	union unitype *arg_top = &arg_stack[STACK_LEN];
	
	/* Init */
	tok.lex.begin = tok.lex.end = (char *)str;
	*op_top = TOK_EOF;

induce_next:
	tokenize_next(&tok);

inductor:
	switch (tok.type) {
	/* Argument handlers */
	case TOK_STRING:
	case TOK_IDENT:
	case TOK_NUM:
		numargs++;
		/* Used to handle straight (PREFIX) polish notation.
		 * When the number of arguments exceed after operator,
		 * We consider this expression as prefix one. */
		if (LARGS && numargs > LARGS) {
			numargs -= 2;
			mode = REDUCE_PREFIX_EXPRESSION;
			goto reductor;
		}
		PUSH_ARG;
		goto induce_next;

	/* Right-associative operator handlers */
	case TOK_EQU:
	case TOK_POW:
	case TOK_ENTER:
		if (numargs >= 1)
			numargs--;
		PUSH_OP;
		goto induce_next;

	/* Left-associative operator handlers */
	case TOK_ENDSTAT:
	case TOK_LEAVE:
	case TOK_EOF:
		mode = REDUCE_STATEMENT;
		goto reductor;
	
	default:
		/* Used to handle reversed (POSTFIX) polish notation. 
		 * When number of arguments exceed before operator,
		 * We consider this expression as postfix */
		if (numargs > 2) {
			numargs--;
			PUSH_OP;
			mode = REDUCE_POSTFIX_EXPRESSION;
			goto reductor;
		/* Used to handle standart (INFIX) notation */
		} else if (LPRIOR > RPRIOR && (mode == REDUCE_PREFIX_EXPRESSION
			|| numargs >= 1)) {
			numargs--;
			mode = REDUCE_INFIX_EXPRESSION;
			goto reductor;
		} else if (numargs >= 1) {
			numargs--;
		}
		PUSH_OP;
		goto induce_next;
	} /* * * * 1 2 3 4 + + 1 2 3 */
	/* 10 + + 1 ((* + 2 3 4) + 5 * 6) 7 * */

reductor:
	/* Reductor operator handlers */
	switch (*op_top) {
	case TOK_EOF:
		if (tok.type != TOK_EOF) {
			goto induce_next;
		}
		printf("Sucess.\n\n");
		goto end;

	case TOK_ENTER:
		POP_OP;
		goto induce_next;

#define	BOP_EVAL(op, arg, name) printf("%i %s %i = ", LOPR, name, ROPR); \
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
		POP_ARG;
		POP_OP;
		goto selector;

	default:	
		printf("%s\n", tok_names[*op_top]);
		POP_OP;
		goto selector;
	}

selector:
	switch (mode) {
	case REDUCE_STATEMENT:
		if (LARGS > &arg_stack[STACK_LEN] - arg_top) {
			printf("Missing arguments\n");
			goto end;
		}
		/* Reduce until end of statement */
		if (LPRIOR == PRIOR_UNDEF)
			mode = REDUCE_POSTFIX_EXPRESSION;
		goto reductor;

	case REDUCE_INFIX_EXPRESSION:
		if (LPRIOR >= RPRIOR) 
			goto reductor;
		PUSH_OP;
		goto induce_next;

	case REDUCE_POSTFIX_EXPRESSION:
		goto induce_next;
	
	/* After reduction of prefix expression
	 * We will run inductor at the same state again
	 * To push our exceed argument onto the stack.*/
	case REDUCE_PREFIX_EXPRESSION:
		goto inductor;
	}

end:
	return;
}
