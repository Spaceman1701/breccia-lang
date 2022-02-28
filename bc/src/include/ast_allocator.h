#pragma once

#include "utils/list.h"

#include "ast.h"

#define BC_AST_ALLOCATOR_BUCKET_COUNT 512

typedef void *AstNode;
typedef void (*AstFreeFunc)(AstNode *);

typedef struct {
    AstNode ref;
    int32_t count; // signed for bug catching
    AstFreeFunc free_func;
} Bc_AstRefCount;

typedef struct {
    Bc_List buckets[BC_AST_ALLOCATOR_BUCKET_COUNT];
} Bc_AstMemory;

void bc_ast_memory_init(Bc_AstMemory *memory);

void *bc_ast_alloc(Bc_AstMemory *mem, size_t bytes, AstFreeFunc free_func);
void bc_ast_retain(Bc_AstMemory *mem, AstNode *node);
void bc_ast_release(Bc_AstMemory *mem, AstNode *node);

void bc_ast_free_unused(Bc_AstMemory *mem);
