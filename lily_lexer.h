#ifndef LILY_LEXER_H
# define LILY_LEXER_H

#include <stdio.h>

typedef enum {
    tk_invalid,
    tk_word,
    tk_left_parenth,
    tk_right_parenth,
    tk_double_quote
} lily_tok_type;

typedef struct {
    lily_tok_type tok_type;
    char *word_buffer;
} lily_token;

lily_token *lily_lexer_token();
void lily_init_lexer(char *);
void lily_lexer(void);

#endif
