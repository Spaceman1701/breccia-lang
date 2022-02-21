#include "utils/files.h"
#include <stdio.h>

#include "lexer.h"

int main(int argc, const char **argv) {
    if (argc < 2) {
        fprintf(stderr, "error: no input files\n");
        return -1;
    }

    FileBuffer input_file;
    bc_files_read_file(argv[1], &input_file);

    LexerState s;
    init_lexer(&s, input_file.data);

    lex(&s);

    bc_lexer_print_all_tokens(&s);
}