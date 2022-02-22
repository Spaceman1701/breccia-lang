#include "parser.h"

#include <stdlib.h>

#include "ast.h"
#include "lexer.h"
#include "utils/hash_map.h"

bool rc_memo_cache_keys_equal(size_t *a, size_t *b) { return (*a) == (*b); }

size_t rc_memo_cache_hash_key(size_t *key) { return *key; }

void bc_memo_cache_init(Bc_MemoizationCache *cache, size_t length) {
    cache->entries =
        (Bc_PositionCache *)calloc(length, sizeof(Bc_PositionCache));
    cache->length = length;
}

void bc_position_cache_init(Bc_PositionCache *cache) {
    cache->map = bc_hm_new(rc_memo_cache_keys_equal, rc_memo_cache_hash_key);
}

Bc_CacheEntry *bc_memo_cache_get_value(Bc_MemoizationCache *cache, size_t index,
                                       size_t func_id) {
    Bc_PositionCache *pos_cache = &cache->entries[index];
    if (!pos_cache->map) {
        bc_position_cache_init(pos_cache);
    }
    Bc_HashMapEntry *entry = bc_hash_map_get(cache, func_id);
    if (!entry) {
        return NULL;
    } else {
        return entry->value;
    }
}

void *handle_cache(expect_func func, Bc_Parser *p, size_t func_id) {
    Bc_CacheEntry *cached_value =
        bc_memo_cache_get_value(&p->cache, p->ts->cursor, func_id);
    if (cached_value) {
        bc_tscanner_reset(p->ts, cached_value->end_pos);
        return cached_value->value;
    }
    return NULL;
}

void store_in_cache(Bc_MemoizationCache *cache, size_t location, size_t func_id,
                    void *value) {
    Bc_PositionCache *pos_cache = &cache->entries[location];
    if (!pos_cache->map) {
        printf("the first access to the cache was a write\n");
        bc_position_cache_init(pos_cache);
    }

    Bc_HashMapEntry *entry = bc_hash_map_put_ptr(pos_cache->map, func_id);
    entry->value = value;
}

void *handle_left_recursion(expect_func func, Bc_Parser *p, size_t func_id) {
    size_t pos = bc_tscanner_mark(p->ts);
    Bc_CacheEntry *cached_value =
        bc_memo_cache_get_value(&p->cache, pos, func_id);
    if (cached_value) {
        bc_tscanner_reset(p->ts, cached_value->end_pos);
        return cached_value->value;
    }

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
        store_in_cache(p, last_pos, func_id, result);
        last_pos = new_pos;
        last_result = result;
    }

    bc_tscanner_reset(p->ts, last_pos);
    return last_result;
}