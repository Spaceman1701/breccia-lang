#include "packrat_cache.h"

#include "bc.h"

Bc_PackratHash bc_packrat_hash_key(Bc_PackratCacheKey k) {
    return ((size_t)k.function + k.location) % BC_PACKRAT_CACHE_BUCKET_COUNT;
}

bool bc_packrat_cache_keys_equal(Bc_PackratCacheKey a, Bc_PackratCacheKey b) {
    return (a.function == b.function) && (a.location == b.location);
}

void bc_packrat_position_entry_init(Bc_PackratPositionEntry *entry) {
    for (size_t i = 0; i < BC_PACKRAT_CACHE_BUCKET_COUNT; i++) {
        bc_list_new(sizeof(Bc_PackratCacheValue), 1, &entry->buckets[i]);
    }
    entry->allocated = true;
}

void free_node_if_set(Bc_PackratRuleResult *res) {
    if (res->result == RC_PACKRAT_RESULT_SUCCESS) {
        if (res->success.node) {
            free(res->success.node);
            res->success.node = NULL;
        }
    }
}

void bc_packrat_cache_init(Bc_PackratCache *cache, size_t length) {
    cache->positions = calloc(length, sizeof(Bc_PackratPositionEntry));
    cache->length = length;
    log_trace("initing packrat cache with %zu position entries", length);
}

void bc_packrat_position_entry_free_owned(Bc_PackratPositionEntry *entry) {
    if (entry->allocated) {
        for (size_t i = 0; i < BC_PACKRAT_CACHE_BUCKET_COUNT; i++) {
            bc_list_free_data(&entry->buckets[i]);
        }
    }
}

void bc_packrat_cache_init_position_if_needed(Bc_PackratCache *cache,
                                              size_t position) {
    log_trace("initing cache position: %zu\n", position);
    if (!cache->positions[position].allocated) {
        bc_packrat_position_entry_init(&cache->positions[position]);
    }
}

Bc_List *bc_packrat_cache_get_bucket(Bc_PackratCache *cache,
                                     Bc_PackratCacheKey key) {
    bc_packrat_cache_init_position_if_needed(cache, key.location);
    Bc_PackratHash hash = bc_packrat_hash_key(key);
    Bc_PackratPositionEntry *position = &cache->positions[key.location];
    return &position->buckets[hash];
}

Bc_PackratRuleResult *bc_packrat_cache_get(Bc_PackratCache *cache,
                                           Bc_PackratCacheKey key) {
    Bc_List *bucket = bc_packrat_cache_get_bucket(cache, key);

    for (size_t i = 0; i < bucket->length; i++) {
        Bc_PackratCacheValue *value = bc_list_get(bucket, i);
        if (value->is_set && bc_packrat_cache_keys_equal(key, value->key)) {
            return &value->result;
        }
    }

    return NULL;
}

void bc_packrat_cache_put(Bc_PackratCache *cache, Bc_PackratCacheKey key,
                          Bc_PackratRuleResult new_value) {
    Bc_List *bucket = bc_packrat_cache_get_bucket(cache, key);

    for (size_t i = 0; i < bucket->length; i++) {
        Bc_PackratCacheValue *value = bc_list_get(bucket, i);
        if (value->is_set && bc_packrat_cache_keys_equal(key, value->key)) {
            value->result = new_value;
            return;
        }
    }

    Bc_PackratCacheValue *append_ptr = bc_list_append_ptr(bucket);
    (*append_ptr) = (Bc_PackratCacheValue){
        .is_set = true,
        .key = key,
        .result = new_value,
    };
}