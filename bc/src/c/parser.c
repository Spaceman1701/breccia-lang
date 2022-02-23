#include "parser.h"

#include <stdlib.h>

#include "ast.h"
#include "lexer.h"
#include "utils/hash_map.h"

void *handle_cache(expect_func func, Bc_Parser *p, const FuncId *func_id) {
    Bc_CacheEntry *cached_value =
        bc_memo_cache_get_value(&p->cache, p->ts->cursor, func_id);
    if (cached_value && cached_value->set) {
        bc_tscanner_reset(p->ts, cached_value->end_pos);
        return cached_value->value;
    }
    return NULL;
}

void *handle_left_recursion(expect_func func, Bc_Parser *p,
                            const FuncId *func_id) {
    Bc_CacheEntry *cached_value =
        bc_memo_cache_get_value(&p->cache, p->ts->cursor, func_id);
    if (cached_value && cached_value->set) {
        return cached_value;
    }

    size_t pos = bc_tscanner_mark(p->ts);
    store_in_cache(&p->cache, pos, func_id, NULL,
                   0); // skip left recursive alternatives
    size_t last_pos = pos;
    void *last_result = NULL;
    for (;;) {
        printf("in loop\n");
        void *result = func(p);
        size_t new_pos = bc_tscanner_mark(p->ts);

        printf("new pos: %zu old pos: %zu", last_pos, new_pos);

        if (new_pos <= last_pos) { // failed to advance
            printf("loop break\n");
            break;
        }
        store_in_cache(&p->cache, last_pos, func_id, result, new_pos);
        last_pos = new_pos;
        last_result = result;
    }

    bc_tscanner_reset(p->ts, last_pos);
    return last_result;
}

Bc_Token *bc_expect(TokenScanner *ts, Bc_TokenType type) {
    Bc_Token *tk = bc_tscanner_peek(ts);
    if (tk->type == type) {
        bc_tscanner_next(ts);
        return tk;
    }
    return NULL;
}

const size_t bc_expect_binop_id = 1;
Bc_BinaryOpExpr *bc_expect_binop(Bc_Parser *p) {
    printf("expect binop\n");
    Bc_BinaryOpExpr *result = handle_left_recursion((expect_func)bc_expect_expr,
                                                    p, &bc_expect_binop_id);
    if (result) {
        return result;
    }
    Bc_Expr *left, *right;
    Bc_Token *op;
    size_t reset_pt = bc_tscanner_mark(p->ts);

    left = bc_expect_expr(p);
    op = bc_expect(p->ts, BC_OP_STAR);
    right = bc_expect_expr(p);
    printf("cursor: %zu\n", p->ts->cursor);
    if (left) {
        printf("left\n");
    }
    if (op) {
        printf("star");
    }
    if (right) {
        printf("right\n");
    }

    if (left && op && right) {
        printf("binop!\n");
        Bc_BinaryOpExpr *binop = malloc(sizeof(Bc_BinaryOpExpr));
        binop->left = left;
        binop->operator= op;
        binop->right = right;
        return binop;
    }

    bc_tscanner_reset(p->ts, reset_pt);

    return NULL;
}

const size_t bc_expect_expr_id = 0;
int recusion_depth = 0;
Bc_Expr *bc_expect_expr(Bc_Parser *p) {
    printf("expect_expr header %d\n", recusion_depth++);
    // printf("id ptr: %p\n", &bc_expect_expr_id);
    // Bc_Expr *result = handle_left_recursion((expect_func)bc_expect_expr, p,
    //                                         &bc_expect_expr_id);
    // if (result) {
    //     printf("non-null result\n");
    //     return result;
    // }
    Bc_Expr *result;
    size_t pos = bc_tscanner_mark(p->ts);
    printf("expect_expr main body\n");

    Bc_BinaryOpExpr *binop;
    if ((binop = bc_expect_binop(p))) {
        result = malloc(sizeof(Bc_Expr));
        result->kind = Bc_ExprBinaryOpType;
        result->binary = binop;
        return result;
    }

    bc_tscanner_reset(p->ts, pos);

    Bc_Token *int_token;
    if ((int_token = bc_expect(p->ts, BC_INTEGER))) {
        printf("cursor: %zu\n", p->ts->cursor);
        result = malloc(sizeof(Bc_Expr));
        result->kind = Bc_ExprIntegerKind;

        Bc_IntegerExpr *ex = malloc(sizeof(Bc_IntegerExpr));
        ex->integer = int_token;
        result->integer_literal = ex;
        return result;
    }
    bc_tscanner_reset(p->ts, pos);

    return result;
}