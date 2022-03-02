#include "parser_rules.h"

#include "bc.h"

#include "ast.h"
#include "packrat.h"

CREATE_RULE(bc_decl_rule) {
    START_ALTERNATIVE(struct_decl)
    Bc_StructDecl *struct_decl;
    if (EXPECT(struct_decl, bc_struct_decl_rule)) {
        Bc_Decl *decl = bc_arena_alloc(p->arena, sizeof(Bc_Decl));
        (*decl) = (Bc_Decl){
            .kind = BC_DECL_KIND_STRUCT,
            .struct_decl = struct_decl,
        };
        return BC_PACKRAT_SUCCESS(decl);
    }
    END_ALTERNATIVE()

    return BC_PACKRAT_FAILURE;
}

CREATE_RULE(bc_struct_decl_rule) {
    START_ALTERNATIVE(struct_decl_rule)
    Bc_Token *kw_type;
    Bc_Token *name;
    Bc_Token *kw_struct;
    Bc_Token *lcurly;
    Bc_Token *rcurly;

    bool decl_header_match =
        EXPECT_TK(kw_type, BC_KW_TYPE) && EXPECT_TK(name, BC_NAME) &&
        EXPECT_TK(kw_struct, BC_KW_STRUCT) && EXPECT_TK(lcurly, BC_LCURLY);
    log_trace("checking decl header %d", decl_header_match);
    if (!decl_header_match) {
        return BC_PACKRAT_FAILURE;
    }

    size_t before_fields = bc_packrat_mark(p);
    Bc_StructField *struct_field;
    size_t field_count = 0;
    while (EXPECT(struct_field, bc_struct_field_rule)) {
        field_count += 1;
        log_trace("struct fields %zu", field_count);
    }
    log_info("discovered %zu fields", field_count);
    bc_packrat_reset(p, before_fields);

    AST_ALLOC(Bc_StructFieldList, field_list){
        .fields =
            bc_arena_alloc(p->arena, sizeof(Bc_StructField) * field_count),
        .length = field_count,
    };

    for (size_t i = 0; i < field_count; i++) {
        struct_field =
            (Bc_StructField *)bc_expect_rule(bc_struct_field_rule, p);
        field_list->fields[i] = *struct_field;
    }
    log_trace("expected field list is %zu", field_list->length);

    if (EXPECT_TK(rcurly, BC_RCURLY)) {

        AST_ALLOC(Bc_StructDecl, decl){
            .fields = field_list,
            .kw_struct = kw_struct,
            .name = name,
            .kw_type = kw_type,
            .rcurly = rcurly,
        };
        return BC_PACKRAT_SUCCESS(decl);
    }
    END_ALTERNATIVE()
    return BC_PACKRAT_FAILURE;
}

CREATE_RULE(bc_struct_field_rule) {
    START_ALTERNATIVE(struct_field)
    Bc_Token *name;
    Bc_TypeAnnotation *type_annotation;
    Bc_Token *semi;

    if (EXPECT_TK(name, BC_NAME) &&
        EXPECT(type_annotation, bc_type_annotation_rule) &&
        EXPECT_TK(semi, BC_SEMI)) {

        AST_ALLOC(Bc_StructField, field){
            .name = name,
            .type = type_annotation,
        };
        log_trace("found struct field");
        return BC_PACKRAT_SUCCESS(field);
    }
    log_trace("didn't find struct field");
    END_ALTERNATIVE()
    return BC_PACKRAT_FAILURE;
}

CREATE_RULE(bc_type_annotation_rule) {
    START_ALTERNATIVE(simple_type)
    Bc_Token *name;
    if (EXPECT_TK(name, BC_NAME)) {
        AST_ALLOC(Bc_TypeAnnotation, ty){
            .name = name,
        };
        return BC_PACKRAT_SUCCESS(ty);
    }
    END_ALTERNATIVE()
    return BC_PACKRAT_FAILURE;
}

CREATE_RULE(bc_ident_rule) { return BC_PACKRAT_FAILURE; }

CREATE_RULE(bc_func_decl_rule) {
    START_ALTERNATIVE(function_impl)
    Bc_Token *kw_fn;
    Bc_Token *name;
    Bc_FuncSigList *sig_list;
    Bc_Token *lcurly;
    Bc_Block *impl;
    Bc_Token *rcurly;

    END_ALTERNATIVE()

    return BC_PACKRAT_FAILURE;
}