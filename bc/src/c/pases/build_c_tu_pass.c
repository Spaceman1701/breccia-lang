#include "bc.h"

#include "ast.h"
#include "cursor.h"
#include "translation_unit.h"

#include "backend/tcu.h"

void convert_struct_type(Bc_Type *s, Bc_CTranslationUnit *ctu) {
    Bc_CStruct *cstruct = bc_ctu_add_struct(ctu, s->name);
    cstruct->field_count = s->struct_type.field_count;
    cstruct->name = s->name;
    cstruct->fields = bc_arena_alloc(ctu->memory, sizeof(Bc_CVariable) *
                                                      cstruct->field_count);
    for (size_t i = 0; i < s->struct_type.field_count; i++) {
        Bc_TypeStructField *field = &s->struct_type.fields[i];

        cstruct->fields[i] = (Bc_CVariable){
            .name = field->name,
            .type =
                (Bc_CTypeAnnotation){
                    .typename =
                        (Bc_CTypeName){
                            .kind = Bc_CTypeSymbolKind_Typedef,
                            .name = "tbd",
                        },
                },
        };
    }
}

Bc_CTranslationUnit *build_c_tu(Bc_TranslationUnit *tu) {
    Bc_CTranslationUnit *ctu = malloc(sizeof(Bc_CTranslationUnit));
    ctu->memory = bc_arena_new(1024);
    Bc_TypeIterator iter;
    bc_init_type_iterator(&iter);
    log_info("building c files");
    Bc_Type *type;
    while ((type = bc_type_iter_next(&iter, tu->type_index))) {
        switch (type->kind) {
        case Bc_TypeKind_Struct:
            convert_struct_type(type, ctu);
            break;

        default:
            break;
        }
    }

    return ctu;
}