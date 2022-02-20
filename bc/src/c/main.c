#include <stdio.h>
#include "utils/files.h"

#include "lexer.h"

int main(int argc, const char** argv) {
    if (argc < 2) {
        fprintf(stderr, "error: no input files\n");
        return -1;
    }

    FileBuffer input_file;
    bc_files_read_file(argv[1], &input_file);
    printf("%s\n", input_file.data);

    LexerState s;
    s.cur = input_file.data;
    lex(&s);
}