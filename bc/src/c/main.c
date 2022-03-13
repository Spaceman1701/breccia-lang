#include "utils/files.h"

#include "bc.h"

#include "lexer.h"

#include "ast.h"

#include "packrat.h"
#include "parser_rules.h"

#include "cursor.h"
#include "translation_unit.h"

#include "backend/tcu.h"

#include "pases.h"

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

    Bc_CTranslationUnit *ctu = build_c_tu(tu);

    bc_write_c_tu(stdout, stdout, ctu);
}
