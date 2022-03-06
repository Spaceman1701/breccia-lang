#include "utils/files.h"

#include "bc.h"

#include "lexer.h"

#include "ast.h"

#include "packrat.h"
#include "parser_rules.h"

#include "cursor.h"

Bc_CursorVisitResult print_structs(Bc_Cursor parent, Bc_Cursor node) {
    char text_buf[256];
    if (parent.kind == Bc_CursorKind_StructDecl) {
        if (node.kind == Bc_CursorKind_Name) {
            Bc_Token *name = bc_cursor_Name(node);
            bc_token_copy_text(name, text_buf);
            printf("struct %s\n", text_buf);
            return Bc_CursorVisitResult_Continue;
        }
        if (node.kind == Bc_CursorKind_StructField) {
            Bc_StructField *field = bc_cursor_StructField(node);
            printf("  field ");
            bc_token_copy_text(field->name, text_buf);
            printf("%s ", text_buf);
            bc_token_copy_text(field->type->name, text_buf);
            printf("%s\n", text_buf);
            return Bc_CursorVisitResult_Continue;
        }
    }
    return Bc_CursorVisitResult_Recurse;
}

Bc_CursorVisitResult print_stmts(Bc_Cursor parent, Bc_Cursor node) {
    if (node.kind == Bc_CursorKind_FuncDecl) {
        printf("found function decl\n");
    }
    if (node.kind == Bc_CursorKind_Block) {
        printf("found block\n");
    }
    if (node.kind == Bc_CursorKind_Stmt) {
        const Bc_Stmt *stmt = node.data;
        printf("stmt kind: %d\n", stmt->kind);
        return Bc_CursorVisitResult_Continue;
    }
    return Bc_CursorVisitResult_Recurse;
}

int main(int argc, const char **argv) {

#ifdef LOG_LEVEL
    log_set_level(LOG_LEVEL);
#endif
    log_set_level(LOG_ERROR);
    if (argc < 2) {
        fprintf(stderr, "error: no input files\n");
        return -1;
    }

    printf("foo\n");

    FileBuffer input_file;
    bc_files_read_file(argv[1], &input_file);

    LexerState s;
    init_lexer(&s, input_file.data);

    lex(&s);

    TokenScanner ts;
    ts.lexer = s;
    ts.cursor = 0;

    bc_lexer_print_all_tokens(&ts.lexer);

    Bc_PackratParser parser = {.ts = &ts};
    bc_packrat_cache_init(&parser.cache, ts.lexer.token_list.length);
    parser.arena = bc_arena_new(4096);

    Bc_Module *module = bc_expect_rule(bc_module_rule, &parser);
    printf("found %zu decls in file %s\n", module->length, input_file.path);

    Bc_Cursor module_cursor = {
        .data = module,
        .kind = Bc_CursorKind_Module,
    };

    bc_cursor_visit_children(module_cursor, print_structs);
    printf("\n\n");
    bc_cursor_visit_children(module_cursor, print_stmts);

    bc_arena_free(parser.arena);
    bc_list_free_data(&ts.lexer.token_list);
    bc_packrat_free_all_owned_memory(&parser);
    bc_files_free_owned_memory(&input_file);
}
