#include <stdlib.h>

#include "ast.h"
#include "lexer.h"

typedef struct {
    void *value;
    bool set;
    size_t end_pos;
} Rc_CacheEntry;

typedef struct {
    Rc_CacheEntry type_map[Bc_AST_NON_TERMINAL_COUNT];
} Rc_PositionCache;

typedef struct {
    Rc_PositionCache *entries;
} MemoizationCache;

typedef struct {
    MemoizationCache cache;
    TokenScanner *ts;
} Bc_Parser;

Bc_Token *bc_expect(TokenScanner *ts, Bc_TokenType);

Bc_Decl *bc_expect_decl(TokenScanner *ts);
Bc_Expr *bc_expect_expr(TokenScanner *ts);
Bc_Stmt *bc_expect_stmt(Bc_Parser *p);

#define CHECK_CACHE(TYPE, parser)                                              \
    do {                                                                       \
        TYPE *cached_result =                                                  \
            (TYPE *)parser->cache.entries[parser->ts->cursor].type_map[1];     \
        if (cached_result) {                                                   \
            return cached_result;                                              \
        }                                                                      \
    } while (0)

Bc_Stmt *bc_expect_stmt(Bc_Parser *p) {
    // CHECK_CACHE(Bc_Stmt, p);

    return NULL;
}

typedef void *(expect_func)(Bc_Parser *p);

void *handle_cache(expect_func func, Bc_Parser *p,
                   Bc_CachableAstNodeKinds kind) {
    Rc_CacheEntry cached_value = p->cache.entries[p->ts->cursor].type_map[kind];
    if (cached_value.set) {
        bc_tscanner_reset(p->ts, cached_value.end_pos);
        return cached_value.value;
    }
    return NULL;
}

void store_in_cache(Bc_Parser *p, size_t location, Bc_CachableAstNodeKinds kind,
                    void *value) {}

void *handle_left_recursion(expect_func func, Bc_Parser *p,
                            Bc_CachableAstNodeKinds kind) {
    size_t pos = bc_tscanner_mark(p->ts);
    Rc_CacheEntry cached_value = p->cache.entries[pos].type_map[kind];
    if (cached_value.set) {
        bc_tscanner_reset(p->ts, cached_value.end_pos);
        return cached_value.value;
    }

    store_in_cache(p, pos, kind, NULL); // skip left recursive alternatives
    size_t last_pos = pos;
    void *last_result = NULL;
    for (;;) {
        void *result = func(p);
        size_t new_pos = bc_tscanner_mark(p->ts);

        if (new_pos <= last_pos) { // failed to advance
            break;
        }
        store_in_cache(p, last_pos, kind, result);
        last_pos = new_pos;
        last_result = result;
    }

    bc_tscanner_reset(p->ts, last_pos);
    return last_result;
}