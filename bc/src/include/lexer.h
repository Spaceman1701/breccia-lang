#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include "utils/list.h"

// this file is implemented by src/re2c/BCeccia.re

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

    BC_OP_PLUS,
    BC_OP_MINUS,
    BC_OP_DIV,
    BC_OP_MOD,
    BC_OP_XOR,
    BC_OP_OR,
    BC_OP_AND,
    BC_OP_LOGICAL_AND,
    BC_OP_LOGICAL_OR,

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

    const char *text_ptr;
    size_t length;

    Bc_TokenType type;
} Bc_Token;

typedef struct {
    char *input_string;

    Bc_List token_list;

} LexerState;

int init_lexer(LexerState *lexer, char *input_string);

int bc_lexer_append_token(LexerState *lexer, Bc_Token token);

void bc_lexer_print_all_tokens(LexerState *lexer);

void bc_token_print(Bc_Token *tk, FILE *stream);

void bc_token_print_location(Bc_Token *tk, FILE *stream);

void bc_token_copy_text(Bc_Token *tk, char *buffer);

char *bc_token_alloc_copy_text(Bc_Token *tk);

void lex(LexerState *s);

typedef struct {
    LexerState lexer;
    size_t cursor;
} TokenScanner;

Bc_Token *bc_tscanner_peek(const TokenScanner *ts);
Bc_Token *bc_tscanner_next(TokenScanner *ts);

size_t bc_tscanner_mark(const TokenScanner *ts);
void bc_tscanner_reset(TokenScanner *ts, size_t pos);