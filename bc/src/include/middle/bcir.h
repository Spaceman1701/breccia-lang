#pragma once

#include "arena.h"
#include "bc.h"
#include "lexer.h"
#include "type.h"
#include "utils/list.h"

typedef enum {
    BcIr_Const, // args: value

    // memory ops
    BcIr_Deref,      // args: variable (reference)
    BcIr_MakeRef,    // args: variable
    BcIr_ReadField,  // args variable, literal-symbol
    BcIr_WriteField, // args variable, literal-symbol, variable

    // boolean ops; args: variable (bool), variable (bool)
    BcIr_BoolAnd,
    BcIr_BoolOr,
    BcIr_BoolNot,

    // generic math bin ops; variable, variable
    BcIr_AddAny,
    BcIr_SubAny,
    BcIr_MulAny,
    BcIr_DivAny,
    BcIr_ModAny,

    // generic bitwise bin ops; variable, variable
    BcIr_AndAny,
    BcIr_OrAny,
    BcIr_ComplimentAny,

    // function call ops;
    BcIr_StaticFuncCall,
    BcIr_InterfaceFuncCall, // a function call to an interface is different than
                            // any other function call

    BcIr_CastToInterface,

    // branching & loops
    BcIr_If,

} BcIr_OpKind;

typedef struct BcIr_Block BcIr_Block;

typedef struct {
    BcIr_OpKind kind;
    Bc_List arguments;
    Bc_Type *result_type;
    BcIr_Block *block;
    Bc_TokenPos src_pos;
} BcIr_Op;

typedef struct BcIr_Node BcIr_Node;

typedef enum {
    BcIr_ArgKind_Node,
    BcIr_ArgKind_Literal,
} BcIr_ArgKind;

typedef struct {
    BcIr_ArgKind kind;
    Bc_Type *type;
    union {
        BcIr_Node *node;
        // TODO: literal
    };

} BcIr_Arg;

struct BcIr_Node {
    uint64_t name;
    BcIr_Op *op;
    size_t block_index;

    Bc_List parents;
    Bc_List children;
};

struct BcIr_Block {
    char *
        name; // if this block is function-like it'll be referenced by this name
    Bc_List nodes;
};

typedef struct {
    Bc_Arena mem;
    Bc_List blocks;
} Bc_IrTu;

typedef struct {
    size_t start;
    size_t end;
} BcIr_Span;

typedef struct {
    BcIr_Span span;
    BcIr_Block *old_block;
    BcIr_Block *new_block;
} BcIr_BlockRewriter;

typedef BcIr_Span (*BcIr_Pattern)(BcIr_Block *block);

typedef bool (*BcIr_RewriteRule)(BcIr_BlockRewriter *);