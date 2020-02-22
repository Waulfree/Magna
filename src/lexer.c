#ifdef MAGNA_MODULE
#include "chartools.h"
#include "lexer.h"
#endif

void next_line(struct token *tok)
{
	tok->line.begin = ++tok->lex.end;
	tok->line.num++;
}

void tokenize_next(struct token *tok)
{
	enum char_type ct;

next_char:
	switch(ct = get_char_type(*tok->lex.end)) {
	case CHAR_NULL:
		tok->type = TOK_EOF;
		goto end;
		
	case CHAR_LFEED:
		/* TODO handle missing quote error for strings */
		next_line(tok);
		goto next_char;
		
	case CHAR_SPACE:
		tok->lex.end++;
		goto next_char;
		
	case CHAR_SLASH:
		if (*++tok->lex.end != '/') {
			tok->type = TOK_DIV;
			goto end;
		}
		tok->type = TOK_COMMENT;
		tok->lex.begin = ++tok->lex.end;
		
		/* Skip comment until EOF or newline*/
		for (; *tok->lex.end && *tok->lex.end != '\n'; tok->lex.end++);
		goto end;
		
	case CHAR_CARET:
		if (*++tok->lex.end == '^') {
			tok->type = TOK_POW;
			tok->lex.end++;
		}
		goto end;
		
	case CHAR_DQUOTE:
		/* If closing quote detected */
		if (tok->type == TOK_STRING && *tok->lex.end == '\"') {
			tok->lex.end++;
			goto next_char;
		}
		tok->type = TOK_STRING;
		tok->lex.begin = ++tok->lex.end;
		
		/* Skip symbols and backslashed symbols
		 * until EOF, newline or quote*/
		for (; *tok->lex.end && *tok->lex.end != '\"'
			&& ((*tok->lex.end == '\\' && *++tok->lex.end)
			|| *tok->lex.end != '\n'); tok->lex.end++);
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
		tok->type = (enum tok_type)ct;
		tok->lex.end++;
		goto end;
	}

end:
	return;
}
