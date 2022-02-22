#pragma once

#include <stdlib.h>

#include "ast.h"
#include "lexer.h"
#include "utils/hash_map.h"

typedef struct {
    void *value;
    size_t end_pos;
} Bc_CacheEntry;

typedef struct {
    Bc_HashMap *map;
} Bc_PositionCache;

typedef struct {
    Bc_PositionCache *entries;
    size_t length;
} Bc_MemoizationCache;

void bc_memo_cache_init(Bc_MemoizationCache *cache, size_t length);

void bc_position_cache_init(Bc_PositionCache *cache);

Bc_CacheEntry *bc_memo_cache_get_value(Bc_MemoizationCache *cache, size_t index,
                                       size_t func_id);

typedef struct {
    Bc_MemoizationCache cache;
    TokenScanner *ts;
} Bc_Parser;

Bc_Token *bc_expect(TokenScanner *ts, Bc_TokenType);

Bc_Decl *bc_expect_decl(TokenScanner *ts);
Bc_Expr *bc_expect_expr(TokenScanner *ts);
Bc_Stmt *bc_expect_stmt(Bc_Parser *p);

typedef void *(expect_func)(Bc_Parser *p);

void *handle_cache(expect_func func, Bc_Parser *p, size_t func_id);

void store_in_cache(Bc_MemoizationCache *cache, size_t location, size_t func_id,
                    void *value);

void *handle_left_recursion(expect_func func, Bc_Parser *p, size_t func_id);