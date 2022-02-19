#include <stdint.h>
#include <stdio.h>
#include <stddef.h>
typedef enum {
    BR_ERR_UNKNOWN,
    BR_KW_TYPE,
    BR_KW_INTERFACE,
    BR_KW_STRUCT,
} Bc_TokenType;


typedef struct {
    uint32_t line;
    uint32_t column;
    uint32_t position;
} Bc_Token;


typedef struct {
    const char* cur;

} LexerState;


void print_token(const char* tk_start, const char* tk_end) {
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
        
        kw_type = "type";
        kw_type {printf("tk_type "); print_token(tk_start, YYCURSOR); continue;}

        name = [a-z]+;
        name {printf("tk_name "); print_token(tk_start, YYCURSOR);continue;}

        [ ]+ {continue;}
        [\u0244] {printf("tk_name "); print_token(tk_start, YYCURSOR);continue;}
        
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
