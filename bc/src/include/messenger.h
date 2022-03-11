#pragma once

#include <stdio.h>

typedef enum {
    Bc_MessageKind_Error = 0,     // fatal errors
    Bc_MessageKind_Warning = 1,   // warnings
    Bc_MessageKind_Info = 2,      // normal messages to the user
    Bc_MessageKind_Verbose = 3,   // verbose messages (for infra debugging)
    Bc_MessageKind_Developer = 4, // messages intended for compiler developers
} Bc_MessageKind;

void bc_msg_with_debug(Bc_MessageKind kind, const char *file, size_t line,
                       const char *fmt, ...);
void bc_msg_stripped(Bc_MessageKind kind, const char *fmt, ...);

#ifdef DEBUG
#define bc_msg(kind, ...)                                                      \
    bc_msg_with_debug(kind, __FILE__, __LINE__, __VA_ARGS__)
#else
#define bc_msg(kind, ...) bc_msg_stripped(kind, __VA_ARGS__)
#endif

#define bc_msg_error(...) bc_msg(Bc_MessageKind_Error, __VA_ARGS__)
#define bc_msg_warn(...) bc_msg(Bc_MessageKind_Warning, __VA_ARGS__)
#define bc_msg_info(...) bc_msg(Bc_MessageKind_Info, __VA_ARGS__)
#define bc_msg_verbose(...) bc_msg(Bc_MessageKind_Verbose, __VA_ARGS__)
#define bc_msg_dev(...) bc_msg(Bc_MessageKind_Developer, __VA_ARGS__)
