#include "lexer.h"

#include <stdint.h>
#include <stdio.h>
#include <stddef.h>


#define TOKEN(tk) handle_token(s, tk, tk_start, YYCURSOR); continue;

void handle_token(LexerState *s, Bc_TokenType tk, const char* tk_start, const char* tk_end) {
    ptrdiff_t len = tk_end - tk_start;
    printf("(%.*s)\n", (int)len, tk_start); //this will break if a token is more than 2^31 bytes. That's pretty unlikely
}

void lex(LexerState *s) {
    const char *YYCURSOR = s->cur;
    const char *YYMARKER = NULL;
    const char *YYLIMIT = NULL;
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

        name = [a-zA-Z][a-zA-Z0-9_]*;
        name {TOKEN(BC_NAME)}

        "//"            {continue;}

        [ \t\r\n]+ {continue;}        
        *       { printf("tk_unknown\n"); }
        $       {printf("done\n"); return;}

    */
    }

}

// int main() {
//     const char* in = "type Ʉ\n";
//     printf("%s", in);
//     LexerState s;
//     s.cur = in;
//     lex(&s);
// }
