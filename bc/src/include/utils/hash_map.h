#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "utils/list.h"

#define BC_HASH_MAP_BUCKET_COUNT 512

typedef const void *bc_hm_key;
typedef const void *bc_hm_value;

typedef bool (*bc_hm_keys_equal_func)(bc_hm_key a, bc_hm_key b);
typedef size_t (*bc_hm_hash_func)(bc_hm_key key);

typedef struct {
    bool used;
    bc_hm_key key;
    bc_hm_value value;
} Bc_HashMapEntry;

typedef struct {
    size_t size;

    bc_hm_keys_equal_func equality_func;
    bc_hm_hash_func hash_funch;

    Bc_List buckets[BC_HASH_MAP_BUCKET_COUNT];
} Bc_HashMap;

Bc_HashMap *bc_hm_new(bc_hm_hash_func hash_function,
                      bc_hm_keys_equal_func equality_function);

Bc_HashMapEntry *bc_hash_map_put_ptr(Bc_HashMap *map, bc_hm_key key);
Bc_HashMapEntry *bc_hash_map_get(Bc_HashMap *map, bc_hm_key key);

void bc_hash_map_free(Bc_HashMap *map);