#include "arena.h"

#include "bc.h"

typedef struct {
    uintptr_t *blocks;
    size_t memory_size;

    size_t head_index;
    size_t block_count;
    size_t block_capacity;
} Arena;

void bc_arena_new_block(Arena *arena) {
    void *new_block = malloc(arena->block_capacity);
    size_t new_block_index = arena->block_count;
    size_t new_block_count = arena->block_count + 1;
    arena->blocks = realloc(arena->blocks, sizeof(uintptr_t) * new_block_count);
    arena->blocks[new_block_index] = (uintptr_t)new_block;
    arena->block_count = new_block_count;
    arena->memory_size += arena->block_capacity;

    arena->head_index = 0;
}

Bc_Arena bc_arena_new(size_t capacity) {
    log_trace("allocating arena block with %zu bytes capacity", capacity);
    Arena *arena = malloc(sizeof(Arena));

    (*arena) = (Arena){
        .block_count = 0,
        .head_index = 0,
        .memory_size = capacity,
        .blocks = NULL,
        .block_capacity = capacity,
    };
    bc_arena_new_block(arena);

    return arena;
}

void *bc_arena_alloc(Bc_Arena arena_in, size_t size) {
    // log_info("allocating %zu bytes", size);
    Arena *arena = (Arena *)arena_in;
    if ((size + arena->head_index) >= arena->block_capacity) {
        log_info("adding new arena block");
        bc_arena_new_block(arena);
    }
    void *cur_block = (void *)arena->blocks[arena->block_count - 1];
    void *out_ptr = cur_block + arena->head_index;
    arena->head_index += size;
    log_trace("arena usage: %zu/%zu", arena->head_index, arena->block_capacity);
    return out_ptr;
}

void bc_arena_free(Bc_Arena arena_in) {
    Arena *arena = (Arena *)arena_in;

    for (size_t i = 0; i < arena->block_count; i++) {
        free((void *)arena->blocks[i]);
    }

    free(arena->blocks);
    free(arena);
}