#include "translation_unit.h"

#include "bc.h"

#include "utils/files.h"
#include "utils/list.h"

#include "cursor.h"
#include "parser_rules.h"
#include "pases.h"

Bc_TranslationUnit *bc_tu_new() {
    Bc_TranslationUnit *tu = malloc(sizeof(Bc_TranslationUnit));
    bc_list_new(sizeof(Bc_TuModule), 1, &tu->modules);
    tu->type_index = NULL;
    return tu;
}

void bc_tu_free(Bc_TranslationUnit *tu) {
    for (size_t i = 0; i < tu->modules.length; i++) {
        Bc_TuModule *module = bc_list_get(&tu->modules, i);
        bc_arena_free(module->parser->arena);
        bc_list_free_data(&module->parser->ts->lexer.token_list);
        bc_packrat_free_all_owned_memory(module->parser);
        free(module->parser);
    }
    bc_list_free_data(&tu->modules);
    bc_type_index_free(tu->type_index);
    free(tu);
}

void bc_tu_add_file(Bc_TranslationUnit *tu, TokenScanner *scanner) {
    Bc_TuModule *module = bc_list_append_ptr(&tu->modules);

    Bc_PackratParser *parser = bc_packrat_new(scanner);

    *module = (Bc_TuModule){
        .parsed_module = NULL,
        .parser = parser,
    };
}

void bc_tu_compile(Bc_TranslationUnit *tu) {
    tu->type_index = bc_type_index_new();
    log_info("compiling translation unit");
    for (size_t i = 0; i < tu->modules.length; i++) {
        Bc_TuModule *module = bc_list_get(&tu->modules, i);
        Bc_Module *mod = bc_expect_rule(bc_module_rule, module->parser);
        module->parsed_module = mod;
    }
    bc_create_type_index(tu);
}

Bc_Cursor bc_tu_get_module_cursor(Bc_TranslationUnit *tu, size_t module_index) {
    Bc_TuModule *mod = bc_list_get(&tu->modules, module_index);
    if (!mod) {
        log_error("cursor cannot be created for non-existent module");
        return (Bc_Cursor){
            .data = NULL,
            .kind = 0,
        };
    }
    return (Bc_Cursor){
        .data = mod->parsed_module,
        .kind = Bc_CursorKind_Module,
    };
}

size_t bc_tu_module_count(Bc_TranslationUnit *tu) { return tu->modules.length; }