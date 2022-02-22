#include "utils/files.h"
#include <stdio.h>

#include "lexer.h"

#include "ast.h"
#include "parser.h"

int main(int argc, const char **argv) {
    if (argc < 2) {
        fprintf(stderr, "error: no input files\n");
        return -1;
    }

    FileBuffer input_file;
    bc_files_read_file(argv[1], &input_file);

    LexerState s;
    init_lexer(&s, input_file.data);

    lex(&s);

    TokenScanner ts;
    ts.lexer = s;
    ts.cursor = 0;

    Bc_Parser parser;
    parser.ts = &ts;
    bc_memo_cache_init(&parser.cache, ts.lexer.token_list.length);

    Bc_Expr *expr = bc_expect_expr(&parser);

    if (expr) {
        printf("found an expr!\n");
    }

    bc_lexer_print_all_tokens(&s);
}