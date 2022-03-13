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

    _TypeIndex *ti = malloc(sizeof(_TypeIndex));
    *ti = (_TypeIndex){
        .memory = arena,
        .buckets = malloc(sizeof(Bc_List) * bucket_count),
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
    free(ti);
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
    size_t hash = hash_code_string(name, bucket_count);
    Bc_List *bucket = bc_type_index_find_bucket(ti, hash);
    log_trace("looked up bucket %p for symbol %s", bucket, name);
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
    size_t hash = hash_code_string(name, bucket_count);
    Bc_List *bucket = bc_type_index_find_bucket(ti, hash);

    return bc_type_index_find_in_bucket(bucket, name);
}

char *bc_type_index_alloc_token_text(Bc_TypeIndex index, Bc_Token *tk) {
    _TypeIndex *ti = (_TypeIndex *)index;
    Bc_Arena arena = ti->memory;

    size_t alloc_size = tk->length + 1; // str length + null terminator
    char *dest = bc_arena_alloc(arena, alloc_size);
    bc_token_copy_text(tk, dest);

    return dest;
}

void *bc_type_index_alloc(Bc_TypeIndex index, size_t size) {
    _TypeIndex *ti = (_TypeIndex *)index;
    Bc_Arena arena = ti->memory;
    return bc_arena_alloc(arena, size);
}

void bc_type_index_resolve_reference(Bc_TypeIndex index, Bc_TypeReference ref) {
    if (!ref.state) {
        Bc_Type *resolved_type =
            bc_type_index_get_type(index, ref.unresolved_name);
        if (resolved_type) {
            ref.state = Bc_TypeReferenceState_Resolved;
            ref.resolved_type = resolved_type;
        } else {
            log_error("couldn't reosolve reference to %s", ref.unresolved_name);
        }
    }
}

void resolve_struct(_TypeIndex *ti, Bc_TypeStruct *type) {
    for (size_t i = 0; i < type->field_count; i++) {
        Bc_TypeStructField *field = &type->fields[i];
        bc_type_index_resolve_reference(ti, field->type);
    }
}

void resolve_type(_TypeIndex *ti, Bc_Type *type) {
    switch (type->kind) {
    case Bc_TypeKind_Struct:
        resolve_struct(ti, &type->struct_type);
        break;

    default:
        break;
    }
}

void bc_type_index_resolve(Bc_TypeIndex index) {
    _TypeIndex *ti = (_TypeIndex *)index;
    for (size_t bindex = 0; bindex < bucket_count; bindex++) {
        Bc_List *bucket = &ti->buckets[bindex];
        for (size_t i = 0; i < bucket->length; i++) {
            Bc_Type *type = bc_list_get(bucket, i);
            resolve_type(ti, type);
        }
    }
}

void bc_init_type_iterator(Bc_TypeIterator *iterator) {
    *iterator = (Bc_TypeIterator){
        .bucket = 0,
        .index = 0,
    };
}

Bc_Type *bc_type_iter_next(Bc_TypeIterator *iter, Bc_TypeIndex index) {
    _TypeIndex *ti = (_TypeIndex *)index;
    size_t next_index = iter->index;
    size_t bucket = iter->bucket;
    if (bucket < bucket_count) {
        if (next_index < ti->buckets[bucket].length) {
            iter->index = next_index + 1;
            return bc_list_get(&ti->buckets[bucket], next_index);
        } else {
            iter->bucket += 1;
            iter->index = 0;
            return bc_type_iter_next(iter, index);
        }
    }

    return NULL;
}