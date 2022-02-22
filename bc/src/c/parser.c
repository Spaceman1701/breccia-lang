#include "parser.h"

#include <stdlib.h>

#include "ast.h"
#include "lexer.h"
#include "utils/hash_map.h"

void *handle_cache(expect_func func, Bc_Parser *p, const FuncId *func_id) {
    printf("in handle cache %p\n", p);
    Bc_CacheEntry *cached_value =
        bc_memo_cache_get_value(&p->cache, p->ts->cursor, func_id);
    printf("in handle cache %p\n", p);
    if (cached_value) {
        bc_tscanner_reset(p->ts, cached_value->end_pos);
        return cached_value->value;
    }
    return NULL;
}

void *handle_left_recursion(expect_func func, Bc_Parser *p,
                            const FuncId *func_id) {
    Bc_CacheEntry *cached_value = handle_cache(func, p, func_id);
    if (cached_value) {
        return cached_value;
    }

    size_t pos = bc_tscanner_mark(p->ts);
    store_in_cache(&p->cache, pos, func_id,
                   NULL); // skip left recursive alternatives
    size_t last_pos = pos;
    void *last_result = NULL;
    for (;;) {
        void *result = func(p);
        size_t new_pos = bc_tscanner_mark(p->ts);

        if (new_pos <= last_pos) { // failed to advance
            break;
        }
        store_in_cache(&p->cache, last_pos, func_id, result);
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

const size_t bc_expect_expr_id = 0;
Bc_Expr *bc_expect_expr(Bc_Parser *p) {
    expect_func f = (expect_func)bc_expect_expr;
    printf("checking the l recursion parser cache\n");
    Bc_Expr *result = handle_cache(f, p, &bc_expect_expr_id);
    if (result) {
        return result;
    }
    printf("there was no cache entry\n");

    Bc_Token *left = bc_expect(p->ts, BC_INTEGER);
    Bc_Token *op = bc_expect(p->ts, BC_OP_STAR);
    Bc_Token *right = bc_expect(p->ts, BC_INTEGER);

    if (left && op && right) {
        Bc_BinaryOpExpr *binop = malloc(sizeof(Bc_BinaryOpExpr));
        binop->left = NULL;
        binop->operator= op;
        binop->right = NULL;

        result = malloc(sizeof(Bc_Expr));
        result->binary = binop;
        result->kind = Bc_ExprBinaryOpType;
    }

    return result;
}