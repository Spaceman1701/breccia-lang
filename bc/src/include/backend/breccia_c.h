#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

typedef enum {
    Bc_CTypeSymbolKind_Struct,  // prefix with struct
    Bc_CTypeSymbolKind_Enum,    // prefix with enum
    Bc_CTypeSymbolKind_Typedef, // no prefix
    Bc_CTypeSymbolKind_Void,    // void
} Bc_CTypeSymbolKind;

typedef struct {
    Bc_CTypeSymbolKind kind;
    char *name;
} Bc_CTypeName;

typedef struct {
    Bc_CTypeName typename;
} Bc_CTypeAnnotation;

typedef struct {
    char *name;
    Bc_CTypeAnnotation type;
} Bc_CVariable;

typedef struct {
    char *name;
    Bc_CTypeAnnotation *ret_type;
    Bc_CTypeAnnotation *arguments;
    size_t arg_count;
} Bc_CFuncSig;

typedef struct {
    Bc_CFuncSig *signature;
} Bc_CFunction;

typedef struct {
    // typedef symbol_name type_name;
    Bc_CTypeAnnotation symbol_name;
    char *type_name;
} Bc_CTypeDefDeclaration;

typedef struct {
    char *name;
    Bc_CVariable *fields;
    size_t field_count;
} Bc_CStruct;

void bc_write_c_typename(FILE *os, Bc_CTypeName *t);
void bc_write_c_type(FILE *os, Bc_CTypeAnnotation *t);
void bc_write_c_typedef(FILE *os, Bc_CTypeDefDeclaration *t);
void bc_write_c_struct(FILE *os, Bc_CStruct *s);
