#include "ast_allocator.h"

#include "bc.h"

void bc_ast_memory_init(Bc_AstMemory *memory) {
    for (size_t i = 0; i < BC_AST_ALLOCATOR_BUCKET_COUNT; i++) {
        bc_list_new(sizeof(Bc_AstRefCount), 1, &memory->buckets[i]);
    }
}

size_t hash_ptr(AstNode node) {
    return (size_t)node % BC_AST_ALLOCATOR_BUCKET_COUNT;
}

Bc_AstRefCount *find_ref_count(Bc_AstMemory *mem, AstNode node) {
    size_t hash = hash_ptr(node);
    Bc_List *bucket = &mem->buckets[hash];

    for (size_t i = 0; i < bucket->length; i++) {
        Bc_AstRefCount *search_node = bc_list_get(bucket, i);
        if (search_node->ref == node) {
            return search_node;
        }
    }

    return NULL;
}

Bc_AstRefCount *new_ref_count(Bc_AstMemory *mem, AstNode node) {
    size_t hash = hash_ptr(node);
    Bc_List *bucket = &mem->buckets[hash];

    Bc_AstRefCount *new_ref = bc_list_append_ptr(bucket);
    (*new_ref) = (Bc_AstRefCount){
        .count = 0,
        .free_func = NULL,
        .ref = NULL,
    };

    return new_ref;
}

void *bc_ast_alloc(Bc_AstMemory *mem, size_t bytes, AstFreeFunc free_func) {
    AstNode node = malloc(bytes);
    Bc_AstRefCount *ref_count = find_ref_count(mem, node);
    if (ref_count != NULL) {
        log_error("double allocation of ast node");
        free(node);
        return NULL;
    }
    ref_count = new_ref_count(mem, node);
    ref_count->ref = node;
    ref_count->free_func = free_func;
    return node;
}

void bc_ast_retain(Bc_AstMemory *mem, AstNode *node) {
    Bc_AstRefCount *ref_count = find_ref_count(mem, node);
    if (!ref_count) {
        log_error("trying to retain untracked pointer");
        return;
    }
    ref_count->count += 1;
    log_info("ref count %p: %d", node, ref_count->count);
}
void bc_ast_release(Bc_AstMemory *mem, AstNode *node) {
    Bc_AstRefCount *ref_count = find_ref_count(mem, node);
    if (!ref_count) {
        log_error("trying to release untracked pointer %p", node);
        return;
    }
    ref_count->count -= 1;
    log_info("ref count %p: %d", node, ref_count->count);
}

void bc_ast_free_unused(Bc_AstMemory *mem) {
    for (size_t b = 0; b < BC_AST_ALLOCATOR_BUCKET_COUNT; b++) {
        Bc_List *list = &mem->buckets[b];
        for (size_t i = 0; i < list->length; i++) {
            Bc_AstRefCount *ref_count = bc_list_get(list, i);
            if (ref_count <= 0) {
                log_info("freeing %p", ref_count->ref);
                ref_count->free_func(ref_count->ref);
            }
        }
    }
}