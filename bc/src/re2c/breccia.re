#include "lexer.h"

#include <stdint.h>
#include <stdio.h>
#include <stddef.h>

const size_t CODE_LOCATION_START_INDEX = 0;


#define TOKEN(tk) handle_token(s, tk, tk_start, YYCURSOR, cur_line, line_start_ptr); continue;

void handle_token(LexerState *s, Bc_TokenType tk, const char* tk_start, const char* tk_end, size_t line_num, const char* line_start_ptr) {
    size_t len = tk_end - tk_start;    
    size_t start_index = tk_start - s->input_string;

    size_t column = (tk_start - line_start_ptr) + CODE_LOCATION_START_INDEX;

    Bc_Token token = {
        .line = line_num,
        .column = column,
        .position = start_index,
        .length = len,
        .text_ptr = tk_start,
        .type = tk,
    };

    int err = bc_lexer_append_token(s, token);
    if (err) {
        fprintf(stderr, "error: failed to add token at position %zu\n", start_index);
    }
}

void lex(LexerState *s) {
    const char *YYCURSOR = s->input_string;
    const char *YYMARKER = NULL;
    const char *YYLIMIT = NULL;
    size_t cur_line = CODE_LOCATION_START_INDEX;
    const char* line_start_ptr = YYCURSOR;
    for (;;) {
        const char *tk_start = YYCURSOR;
    /*!re2c
        re2c:eof = 0;
        re2c:yyfill:enable = 0;
        re2c:define:YYCTYPE = 'unsigned char';
        
        "type"          {TOKEN(BC_KW_TYPE)}
        "interface"     {TOKEN(BC_KW_INTERFACE)}
        "struct"        {TOKEN(BC_KW_STRUCT)}
        "impl"          {TOKEN(BC_KW_IMPL)}
        "for"           {TOKEN(BC_KW_FOR)}
        "func"          {TOKEN(BC_KW_FUNC)}
        "alias"         {TOKEN(BC_KW_ALIAS)}
        "let"           {TOKEN(BC_KW_LET)}
        "if"            {TOKEN(BC_KW_IF)}
        "return"        {TOKEN(BC_KW_RETURN)}
        "mut"           {TOKEN(BC_KW_MUT)}

        "="             {TOKEN(BC_OP_ASSIGN)}
        "*"             {TOKEN(BC_OP_STAR)}
        "."             {TOKEN(BC_OP_DOT)}
        "::"            {TOKEN(BC_OP_SCOPE)}

        ","             {TOKEN(BC_COMMA)}
        "\""            {TOKEN(BC_QUOTE)}

        "{"             {TOKEN(BC_LCURLY)}
        "}"             {TOKEN(BC_RCURLY)}

        "("             {TOKEN(BC_LPAREN)}
        ")"             {TOKEN(BC_RPAREN)}

        ";"             {TOKEN(BC_SEMI)}

        name =          [a-zA-Z][a-zA-Z0-9_]*;
        name            {TOKEN(BC_NAME)}

        "//"            {continue;}
        [\n]            {cur_line++; line_start_ptr=YYCURSOR; continue;}
        [ \t\r]+        {continue;}        
        *               { printf("tk_unknown\n"); }
        $               {printf("done\n"); return;}

    */
    }

}
