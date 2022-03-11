#pragma once

#include "ast.h"
#include "bc.h"
#include "lexer.h"

typedef struct Bc_Type Bc_Type;
typedef struct Bc_TypeStruct Bc_TypeStruct;
typedef struct Bc_TypeInterface Bc_TypeInterface;
typedef struct Bc_TypePrimitive Bc_TypePrimitive;
typedef struct Bc_TypeAlias Bc_TypeAlias;
typedef struct Bc_TypeFunction Bc_TypeFunction;

typedef enum {
    Bc_TypeKind_Interface,
    Bc_TypeKind_Struct,
    Bc_TypeKind_Primitive,
    Bc_TypeKind_Alias,
    Bc_TypeKind_Function,
} Bc_TypeKind;

typedef enum { // these values are intended to be used in bitwise operations
    Bc_TypeReferenceKind_Value = 0,
    Bc_TypeReferenceKind_Pointer = 1,
    Bc_TypeReferenceKind_Mutable = 2,
} Bc_TypeReferenceKind;

typedef enum {
    Bc_TypeReferenceState_Unresolved = 0, // for use in bool exprs
    Bc_TypeReferenceState_Resolved = 1,
} Bc_TypeReferenceState;

typedef struct {
    Bc_TypeReferenceState state;
    Bc_TypeReferenceKind kind;
    union {
        char *unresolved_name;
        Bc_Type *resolved_type;
    };
} Bc_TypeReference;

bool bc_type_ref_is_ptr(Bc_TypeReference ref);
bool bc_type_ref_is_mut(Bc_TypeReference ref);

typedef struct {
    char *name;
    Bc_TypeReference type;
} Bc_TypeStructField;

struct Bc_TypeStruct {
    size_t field_count;
    Bc_TypeStructField *fields;
    Bc_StructDecl *decl;
};

struct Bc_TypePrimitive {};

struct Bc_Type {
    char *name;
    Bc_TypeKind kind;
    size_t size;
    Bc_TypeReference *implemented_interfaces;
    union {
        Bc_TypeStruct struct_type;
        Bc_TypePrimitive primitive_type;
    };
};

typedef void *Bc_TypeIndex;

Bc_TypeIndex bc_type_index_new();
void bc_type_index_free(Bc_TypeIndex index);

Bc_Type *bc_type_index_find_type(Bc_TypeIndex index,
                                 Bc_TypeReference reference);
Bc_Type *bc_type_index_create_type(Bc_TypeIndex index, char *name);

char *bc_type_index_alloc_token_text(Bc_TypeIndex index, Bc_Token *tk);

void *bc_type_index_alloc(Bc_TypeIndex index, size_t size);

void bc_type_index_resolve_reference(Bc_TypeIndex index, Bc_TypeReference ref);

void bc_type_index_resolve(Bc_TypeIndex index);