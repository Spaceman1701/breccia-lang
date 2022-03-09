#include "type.h"

#include <memory.h>

#include "arena.h"
#include "bc.h"
#include "utils/hash_code.h"
#include "utils/list.h"

const size_t bucket_count = 256;

typedef struct Node Node;

struct Node {
    Bc_Type *type;
    Node *left;
    Node *right;
};

typedef struct {
    Bc_Arena memory;
    Bc_List *buckets;
} _TypeIndex;

Bc_TypeIndex bc_type_index_new() {
    Bc_Arena arena = bc_arena_new(1024);

    _TypeIndex *ti = bc_arena_alloc(arena, sizeof(_TypeIndex));
    *ti = (_TypeIndex){
        .memory = arena,
        .buckets = bc_arena_alloc(arena, sizeof(Bc_List) * bucket_count),
    };

    for (size_t i = 0; i < bucket_count; i++) {
        bc_list_new(sizeof(Bc_Type), 1, &ti->buckets[i]);
    }

    return ti;
}

void bc_type_index_free(Bc_TypeIndex index) {
    _TypeIndex *ti = (_TypeIndex *)index;
    if (ti == NULL) {
        return;
    }
    for (size_t i = 0; i < bucket_count; i++) {
        bc_list_free_data(&ti->buckets[i]);
    }
    bc_arena_free(ti->memory);
}

Bc_List *bc_type_index_find_bucket(_TypeIndex *index, size_t hash) {
    return &index->buckets[hash];
}

Bc_Type *bc_type_index_find_in_bucket(Bc_List *bucket, char *name) {
    for (size_t i = 0; i < bucket->length; i++) {
        Bc_Type *existing_type = bc_list_get(bucket, i);
        int cmp = strcmp(
            name, existing_type->name); // NOTE: I'm not sure if this returns
                                        // the correct ordering for unicode
        if (cmp == 0) {
            return existing_type;
        }
    }
    return NULL;
}

Bc_Type *bc_type_index_create_type(Bc_TypeIndex index, char *name) {
    _TypeIndex *ti = (_TypeIndex *)index;
    size_t hash = hash_code_string(name);
    Bc_List *bucket = bc_type_index_find_bucket(ti, hash);

    Bc_Type *existing_type = bc_type_index_find_in_bucket(bucket, name);
    if (existing_type) {
        return NULL;
    }

    Bc_Type *add_ptr = bc_list_append_ptr(bucket);
    add_ptr->name = name;
    return add_ptr;
}

Bc_Type *bc_type_index_get_type(Bc_TypeIndex index, char *name) {
    _TypeIndex *ti = (_TypeIndex *)index;
    size_t hash = hash_code_string(name);
    Bc_List *bucket = bc_type_index_find_bucket(ti, hash);

    return bc_type_index_find_in_bucket(bucket, name);
}