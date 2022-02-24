#include "utils/files.h"
#include <stdio.h>

#include "lexer.h"

#include "ast.h"

#include "packrat.h"
#include "parser_rules.h"

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

    bc_lexer_print_all_tokens(&ts.lexer);

    Bc_PackratParser parser = {.ts = &ts};
    bc_packrat_cache_init(&parser.cache, ts.lexer.token_list.length);

    printf("expr: %p\n", bc_expr_rule);
    printf("integer: %p\n", bc_integer_expr_rule);

    Bc_Expr *e = bc_expect_rule(bc_expr_rule, &parser);
    if (e) {
        if (e->kind == Bc_ExprBinaryOpType) {
            printf("found binop\n");
        } else if (e->kind == Bc_ExprIntegerKind) {
            printf("found integer\n");
        }
    } else {
        printf("no match\n");
    }
}
