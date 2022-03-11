#include "messenger.h"

#include <stdarg.h>
#include <stdio.h>

void print_level(FILE *os, Bc_MessageKind kind) {
    switch (kind) {
    case Bc_MessageKind_Error:
        fprintf(os, "error: ");
        break;
    case Bc_MessageKind_Warning:
        fprintf(os, "warning: ");
        break;
    case Bc_MessageKind_Info:
        break;
    case Bc_MessageKind_Developer:
        fprintf(os, "debug: ");
        break;
    default:
        break;
    }
}

FILE *get_ostream(Bc_MessageKind kind) {
    switch (kind) {
    case Bc_MessageKind_Error:
    case Bc_MessageKind_Warning:
        return stderr;
    default:
        return stdout;
    }
}

void bc_msg_with_debug(Bc_MessageKind kind, const char *file, size_t line,
                       const char *fmt, ...) {
    va_list args;
    FILE *os = get_ostream(kind);

    fprintf(os, "%s:%zu: ", file, line);
    print_level(os, kind);
    vfprintf(os, fmt, args);
    fprintf(os, "\n");
}
void bc_msg_stripped(Bc_MessageKind kind, const char *fmt, ...) {
    va_list args;
    FILE *os = get_ostream(kind);

    print_level(os, kind);
    vfprintf(os, fmt, args);
    fprintf(os, "\n");
}