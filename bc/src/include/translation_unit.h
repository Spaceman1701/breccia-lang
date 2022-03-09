#pragma once

#include "bc.h"

#include "utils/files.h"
#include "utils/list.h"

#include "ast.h"
#include "lexer.h"
#include "packrat.h"
#include "type.h"

typedef struct {
    Bc_List modules;
    Bc_TypeIndex *type_index;
} Bc_TranslationUnit;

typedef struct {
    FileBuffer *file;
    Bc_PackratParser *parser;
    Bc_Module *parsed_module;
} Bc_TuModule;

Bc_TranslationUnit *bc_tu_new();
void bc_tu_free(Bc_TranslationUnit *tu);

void bc_tu_add_file(Bc_TranslationUnit *tu, TokenScanner *scanner);

void bc_tu_compile();