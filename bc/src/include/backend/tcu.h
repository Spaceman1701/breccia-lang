#pragma once

#include "arena.h"
#include "backend/breccia_c.h"
#include "utils/list.h"

typedef struct {
    Bc_List includes;
    Bc_List forward_decls;
    Bc_List extern_globals;
    Bc_List structs;
    Bc_List functions;
} Bc_CHeader;

typedef struct {
    Bc_List globals;
    Bc_List functions;
} Bc_CSource;

typedef struct {
    Bc_Arena memory;
    Bc_CHeader header;
    Bc_CSource source;
} Bc_CTranslationUnit;

Bc_CStruct *bc_ctu_add_struct(Bc_CTranslationUnit *tu, char *name);

void bc_write_c_tu(FILE *header, FILE *src, Bc_CTranslationUnit *tu);