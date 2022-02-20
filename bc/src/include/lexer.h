#include <stdint.h>
#include <stddef.h>
#include <stdio.h>

#include "utils/list.h"

//this file is implemented by src/re2c/BCeccia.re

typedef enum {
    BC_ERR_UNKNOWN,
    BC_KW_TYPE,
    BC_KW_INTERFACE,
    BC_KW_STRUCT,
    BC_KW_IMPL,
    BC_KW_FOR,
    BC_KW_FUNC,
    BC_KW_ALIAS,
    BC_KW_LET,
    BC_KW_IF,
    BC_KW_RETURN,
    BC_KW_MUT,

    BC_OP_ASSIGN,
    BC_OP_STAR,
    BC_OP_DOT,

    BC_OP_SCOPE,
    
    BC_COMMA,
    BC_QUOTE,

    BC_LCURLY,
    BC_RCURLY,

    BC_LPAREN,
    BC_RPAREN,

    BC_LSQUARE,
    BC_RSQUARE,

    BC_SEMI,

    BC_INTEGER,
    BC_REAL,

    BC_NAME,

} Bc_TokenType;


typedef struct {
    size_t line;
    size_t column;
    size_t position;

    const char* text_ptr;
    size_t length;

    Bc_TokenType type;
} Bc_Token;

typedef struct {
    const char* input_string;
    
    Bc_List token_list;
    
} LexerState;

int init_lexer(LexerState* lexer, const char* input_string);

int bc_lexer_append_token(LexerState* lexer, Bc_Token token);

void bc_lexer_print_all_tokens(LexerState* lexer);

void bc_token_print(Bc_Token *tk, FILE* stream);

void lex(LexerState *s);