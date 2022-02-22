#include "parser_cache.h"

void store_in_cache(Bc_MemoizationCache *cache, size_t location,
                    const FuncId *func_id, void *value) {
    Bc_PositionCache *pos_cache = &cache->entries[location];
    if (!pos_cache->map) {
        printf("the first access to the cache was a write\n");
        bc_position_cache_init(pos_cache);
    }

    Bc_HashMapEntry *entry = bc_hash_map_put_ptr(pos_cache->map, &func_id);
    entry->value = value;
}

bool rc_memo_cache_keys_equal(const FuncId *a, const FuncId *b) {
    return (*a) == (*b);
}

size_t rc_memo_cache_hash_key(const FuncId *key) { return *key; }

void bc_memo_cache_init(Bc_MemoizationCache *cache, size_t length) {
    cache->entries =
        (Bc_PositionCache *)calloc(length, sizeof(Bc_PositionCache));
    cache->length = length;
}

void bc_position_cache_init(Bc_PositionCache *cache) {
    cache->map = bc_hm_new((bc_hm_hash_func)rc_memo_cache_hash_key,
                           (bc_hm_keys_equal_func)rc_memo_cache_keys_equal);
}

Bc_CacheEntry *bc_memo_cache_get_value(Bc_MemoizationCache *cache, size_t index,
                                       const FuncId *func_id) {
    printf("finding pos cache for index %zu\n", index);
    Bc_PositionCache *pos_cache = &cache->entries[index];
    if (!pos_cache->map) {
        bc_position_cache_init(pos_cache);
    }
    Bc_HashMapEntry *entry = bc_hash_map_get(pos_cache->map, func_id);
    if (!entry) {
        return NULL;
    } else {
        return (Bc_CacheEntry *)entry->value;
    }
}