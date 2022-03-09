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

typedef struct {
    char *name;
    Bc_TypeReferenceKind kind;
} Bc_TypeReference;

bool bc_type_ref_is_ptr(Bc_TypeReference ref);
bool bc_type_ref_is_mut(Bc_TypeReference ref);

typedef struct {
    char *name;
    Bc_Type *type;
} Bc_TypeStructField;

struct Bc_TypeStruct {
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

Bc_Type *bc_type_index_find_type(Bc_TypeIndex index,
                                 Bc_TypeReference reference);
Bc_Type *bc_type_index_create_type(Bc_TypeIndex index, char *name);