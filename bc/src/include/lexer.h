#include <stdint.h>
#include <stddef.h>

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
    uint32_t line;
    uint32_t column;
    uint32_t position;
} Bc_Token;


typedef struct {
    const char* cur;

} LexerState;

void lex(LexerState *s);