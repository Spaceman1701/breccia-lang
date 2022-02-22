#pragma once

#include <stdlib.h>

#include "ast.h"
#include "lexer.h"
#include "utils/hash_map.h"

typedef size_t FuncId;

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
                                       const FuncId *func_id);

void store_in_cache(Bc_MemoizationCache *cache, size_t location,
                    const FuncId *func_id, void *value);
