#include "utils/files.h"

#include "bc.h"

#include "lexer.h"

#include "ast.h"

#include "packrat.h"
#include "parser_rules.h"

int main(int argc, const char **argv) {

#ifdef LOG_LEVEL
    log_set_level(LOG_LEVEL);
#endif
    log_set_level(LOG_INFO);
    if (argc < 2) {
        fprintf(stderr, "error: no input files\n");
        return -1;
    }

    printf("foo\n");

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
    parser.arena = bc_arena_new(1024 * 1024);

    Bc_Expr *e = bc_expect_rule(bc_expr_rule, &parser);
    if (e) {
        if (e->kind == BC_EXPR_KIND_BINARY_OP) {
            printf("found binop\n");

            Bc_Expr *left = e->binary->left;
            Bc_Expr *right = e->binary->right;

            if (left->kind == BC_EXPR_KIND_INTEGER_LIT &&
                right->kind == BC_EXPR_KIND_INTEGER_LIT) {

                char *left_str =
                    bc_token_alloc_copy_text(left->integer_literal->integer);
                char *right_str =
                    bc_token_alloc_copy_text(right->integer_literal->integer);

                int left_int = atoi(left_str);
                int right_int = atoi(right_str);

                Bc_Token *op = e->binary->operator;

                int ans = -1;
                switch (op->text_ptr[0]) {
                case '*':
                    ans = left_int * right_int;
                    break;
                case '/':
                    ans = left_int / right_int;
                    break;
                case '+':
                    ans = left_int + right_int;
                    break;
                case '-':
                    ans = left_int - right_int;
                    break;
                default:
                    break;
                }

                printf("%d %c %d = %d\n", left_int, op->text_ptr[0], right_int,
                       ans);
            }

        } else if (e->kind == BC_EXPR_KIND_INTEGER_LIT) {
            printf("found integer\n");
        }
    } else {
        printf("no match\n");
    }
    bc_arena_free(parser.arena);
    bc_list_free_data(&ts.lexer.token_list);
    bc_packrat_free_all_owned_memory(&parser);
    free(ts.lexer.input_string);
}
