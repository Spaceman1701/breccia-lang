#pragma once

#include <stdbool.h>
#include <stdlib.h>

#include "utils/list.h"

#define BC_PACKRAT_CACHE_BUCKET_COUNT 16

typedef size_t Bc_FunctionId; // this might be best as a function ptr address
typedef size_t Bc_PackratHash;
typedef void *Bc_AstNode;

typedef enum {
    RC_PACKRAT_RESULT_FAILURE = 0,
    RC_PACKRAT_RESULT_SUCCESS = 1,
} Bc_PackratRuleResultType;

typedef struct {
    Bc_AstNode node;
    size_t position;
} Bc_PackratRuleSuccess;

typedef struct {
    // Eventually this will have a failure side
    Bc_PackratRuleSuccess success;
    Bc_PackratRuleResultType result;
} Bc_PackratRuleResult;

// cache key
typedef struct {
    size_t location;
    Bc_FunctionId function;
} Bc_PackratCacheKey;

typedef struct {
    Bc_PackratCacheKey key;
    Bc_PackratRuleResult result;
    bool is_set;
} Bc_PackratCacheValue;

Bc_PackratHash bc_packrat_hash_key(Bc_PackratCacheKey k);
bool bc_packrat_cache_keys_equal(Bc_PackratCacheKey a, Bc_PackratCacheKey b);

typedef struct {
    bool allocated;
    Bc_List buckets[BC_PACKRAT_CACHE_BUCKET_COUNT];
} Bc_PackratPositionEntry;

void bc_packrat_position_entry_init(Bc_PackratPositionEntry *entry);

// cache
typedef struct {
    Bc_PackratPositionEntry *positions;
} Bc_PackratCache;

void bc_packrat_cache_init(Bc_PackratCache *cache, size_t length);
Bc_PackratRuleResult *bc_packrat_cache_get(Bc_PackratCache *cache,
                                           Bc_PackratCacheKey key);
void bc_packrat_cache_put(Bc_PackratCache *cache, Bc_PackratCacheKey key,
                          Bc_PackratRuleResult value);
