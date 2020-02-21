#ifdef MAGNA_MODULE
#include "chartools.h"
#include "lexer.h"
#endif

/*void tokenize_next(enum tok_type *op, const char **lex)
{
	if (*op != TOK_NOP)
		(*lex)++;
	while (**lex == ' ') (*lex)++;

	switch (**lex) {
	case '=':
		*op = TOK_EQU;
		break;
	case '+':
		*op = TOK_ADD;
		break;
	case '-':
		*op = TOK_SUB;
		break;
	case '*':
		*op = TOK_MUL;
		break;
	case '/':
		*op = TOK_DIV;
		break;
	case '^':
		*op = TOK_POW;
		break;
	case '(':
		*op = TOK_OPN;
		break;
	case ')':
		*op = TOK_CLS;
		break;

	case ';':
	case '\0':
		*op = TOK_NOP;
		break;
	
	default:
		*op = TOK_ARG;
		break;
	}
}*/

void tokenize_next(struct token *tok) {
	enum char_type ct;

next_char:
	switch(ct = get_char_type(*tok->lex.end)) {
	case CHAR_NULL:
		tok->type = TOK_EOF;
		goto end;
		
	case CHAR_LFEED:
		goto line_feed;
		
	case CHAR_SPACE:
		tok->lex.end++;
		goto next_char;
		
	case CHAR_SLASH:
		tok->lex.end++;
		if (*tok->lex.end == '/') {
			tok->type = TOK_COMMENT;
			tok->lex.begin = ++tok->lex.end;
			goto tokenize_comment;
		} else {
			tok->type = TOK_DIV;
		}
		goto end;
		
	case CHAR_CARET:
		tok->lex.end++;
		if (*tok->lex.end == '^') {
			tok->type = TOK_POW;
			tok->lex.end++;
		}
		goto end;
		
	case CHAR_DQUOTE:
		/* TODO STRING TOKENIZER */
		tok->type = TOK_EOF;
		goto end;
		
	case CHAR_ALPHA:
		tok->type = TOK_IDENT;
		tok->lex.begin = tok->lex.end++;
		while (char_is_alpha(*tok->lex.end)
			|| char_is_num(*tok->lex.end)
			|| char_is_other(*tok->lex.end))
			tok->lex.end++;
		goto end;
		
	case CHAR_NUM:
		tok->type = TOK_NUM;
		tok->lex.begin = tok->lex.end;
		tok->val.i = 0;
		while (char_is_num(*tok->lex.end)) {
			tok->val.i = tok->val.i * 10 + *tok->lex.end - '0';
			tok->lex.end++;
		}
		goto end;
	
	default:
		/* Be careful, tok_type must be compatible with char_type */
		tok->type = ct;
		tok->lex.end++;
		goto end;
	}

tokenize_comment:
	for (; *tok->lex.end; tok->lex.end++) {
		if (*tok->lex.end == '\n')
			goto line_feed;
		else
			goto end;
	}

line_feed:
	tok->line.begin = ++tok->lex.end;
	tok->line.num++;

end:
	return;
}
