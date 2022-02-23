#include "utils/hash_map.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

Bc_HashMap *bc_hm_new(bc_hm_hash_func hash_function,
                      bc_hm_keys_equal_func equality_function) {
    Bc_HashMap *map = malloc(sizeof(Bc_HashMap));
    map->size = 0;
    map->equality_func = equality_function;
    map->hash_funch = hash_function;
    for (size_t i = 0; i < BC_HASH_MAP_BUCKET_COUNT; i++) {
        if (bc_list_new(sizeof(Bc_HashMapEntry), 1, &map->buckets[i])) {
            bc_hash_map_free(map);
            return NULL;
        }
    }
    return map;
}

size_t bc_hash_map_compute_hash(Bc_HashMap *map, const bc_hm_key key) {
    return map->hash_funch(key) % BC_HASH_MAP_BUCKET_COUNT;
}

Bc_HashMapEntry *bc_hm_bucket_find_entry(Bc_HashMap *map, Bc_List *bucket,
                                         const bc_hm_key key) {

    Bc_HashMapEntry *map_entry;
    for (size_t i = 0; i < bucket->length; i++) {
        map_entry = (Bc_HashMapEntry *)bc_list_get(bucket, i);

        bool keys_equal = map->equality_func(map_entry->key, key);

        if (map_entry->used && keys_equal) {
            return map_entry;
        }
    }

    return NULL;
}

Bc_HashMapEntry *bc_hash_map_put_ptr(Bc_HashMap *map, const bc_hm_key key) {
    size_t hash = bc_hash_map_compute_hash(map, key);
    Bc_List *bucket = &map->buckets[hash];

    Bc_HashMapEntry *map_entry = bc_hm_bucket_find_entry(map, bucket, key);

    if (!map_entry) {
        map_entry = bc_list_append_ptr(bucket);
        (*map_entry) = (Bc_HashMapEntry){
            .used = true,
            .key = key,
            .value = NULL,
        };
    }

    return map_entry;
}

Bc_HashMapEntry *bc_hash_map_get(Bc_HashMap *map, const bc_hm_key key) {
    size_t hash = bc_hash_map_compute_hash(map, key);
    Bc_List *bucket = &map->buckets[hash];
    return bc_hm_bucket_find_entry(map, bucket, key);
}

void bc_hash_map_free(Bc_HashMap *map) {
    for (size_t i = 0; i < BC_HASH_MAP_BUCKET_COUNT; i++) {
        bc_list_free_data(&map->buckets[i]);
    }
    free(map);
}