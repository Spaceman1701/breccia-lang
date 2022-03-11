#include "utils/files.h"

#include "bc.h"

#include "lexer.h"

#include "ast.h"

#include "packrat.h"
#include "parser_rules.h"

#include "cursor.h"
#include "translation_unit.h"

typedef struct {
    uint32_t foo;
} Test;

Bc_CursorVisitResult print_structs(Bc_Cursor parent, Bc_Cursor node,
                                   Test *data) {
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

Bc_CursorVisitResult print_stmts(Bc_Cursor parent, Bc_Cursor node,
                                 Bc_VisitorData data) {
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

    log_set_level(LOG_INFO);
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

    Bc_TranslationUnit *tu = bc_tu_new();
    bc_tu_add_file(tu, &ts);
    bc_tu_compile(tu);

    Bc_Module *module = bc_list_get(&tu->modules, 0);
    if (module) {
        printf("parsed module\n");
    }
}
