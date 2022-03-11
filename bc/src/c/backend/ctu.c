
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
    Bc_CHeader header;
    Bc_CSource source;
} Bc_CTranslationUnit;

void bc_ctu_add_struct(Bc_CTranslationUnit *tu, Bc_CStruct s) {}