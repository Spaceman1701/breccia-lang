#include "arena.h"

#include "bc.h"

typedef struct {
    void *memory_base_ptr;
    size_t memory_size;

    size_t head_index;
} Arena;

Bc_Arena bc_arena_new(size_t capacity) {
    Arena *arena = malloc(sizeof(Arena));
    arena->memory_base_ptr = malloc(capacity);
    arena->memory_size = capacity;
    arena->head_index = 0;
    return arena;
}

void *bc_arena_alloc(Bc_Arena arena_in, size_t size) {
    Arena *arena = (Arena *)arena_in;
    if (size + arena->head_index > arena->memory_size) {
        log_error("arena alloc failed: not enough capacity");
        return NULL; // TODO: grow arena
    }
    void *out_ptr = arena->memory_base_ptr + arena->head_index;
    arena->head_index += size;
    return out_ptr;
}

void bc_arena_free(Bc_Arena arena_in) {
    Arena *arena = (Arena *)arena_in;
    free(arena->memory_base_ptr);
    free(arena);
}