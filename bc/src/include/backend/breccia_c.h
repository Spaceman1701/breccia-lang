#pragma once

#include <stddef.h>
#include <stdint.h>

typedef struct {

} Bc_CTypeAnnotation;

typedef struct {
    char *name;
    Bc_CTypeAnnotation *type;
} Bc_CVariable;

typedef struct {

} Bc_CFunction;

typedef struct {
    // typedef symbol_name type_name;
    char *symbol_name;
    char *type_name;
} Bc_CTypeDefDeclaration;

typedef struct {
    char *name;
    Bc_CVariable *fields;
    size_t field_count;
} Bc_CStruct;