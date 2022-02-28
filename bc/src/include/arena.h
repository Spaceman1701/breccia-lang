#pragma once

#include "bc.h"

// Rc_Arena allocs memory like malloc but contiguously
// Rc_Arena is freed all at once
typedef void *Bc_Arena;

enum Rc_AreaStatus {
    RC_ARENA_SUCCESS = 0,
    RC_ARENA_SYSTEM_ALLOC_FAIELD = 1,
};

Bc_Arena bc_arena_new(size_t capacity);

void *bc_arena_alloc(Bc_Arena arena, size_t size);
void bc_arena_free(Bc_Arena arena);