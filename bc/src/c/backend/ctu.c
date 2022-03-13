#include "backend/tcu.h"
#include <string.h>
Bc_CStruct *bc_ctu_add_struct(Bc_CTranslationUnit *tu, char *name) {
    Bc_CTypeDefDeclaration *forward_decl =
        bc_list_append_ptr(&tu->header.forward_decls);

    Bc_CTypeName symbol = {
        .kind = Bc_CTypeSymbolKind_Struct,
        .name = name,
    };

    *forward_decl = (Bc_CTypeDefDeclaration){
        .symbol_name =
            (Bc_CTypeAnnotation){
                .typename = symbol,
            },
        .type_name = name,
    };

    return bc_list_append_ptr(&tu->header.structs);
}

void bc_write_c_tu(FILE *header, FILE *src, Bc_CTranslationUnit *tu) {
    Bc_CHeader *header_model = &tu->header;

    for (size_t i = 0; i < header_model->forward_decls.length; i++) {
        Bc_CTypeDefDeclaration *td =
            bc_list_get(&header_model->forward_decls, i);
        bc_write_c_typedef(header, td);
    }

    for (size_t i = 0; i < header_model->structs.length; i++) {
        Bc_CStruct *s = bc_list_get(&header_model->structs, i);
        bc_write_c_struct(header, s);
    }
}