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

    START_ALTERNATIVE(func_decl)
    Bc_FuncDecl *func_decl;
    if (EXPECT(func_decl, bc_func_decl_rule)) {
        AST_ALLOC(Bc_Decl, decl){
            .kind = BC_DECL_KIND_FUNC,
            .func_decl = func_decl,
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
        bc_packrat_reset(p, pos);
        return BC_PACKRAT_FAILURE;
    }

    AST_ALLOC(Bc_StructFieldList, field_list){
        .fields = NULL,
        .length = 0,
    };

    EXPECT_LIST(Bc_StructField, bc_struct_field_rule, field_list->fields,
                field_list->length);

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
    Bc_Token *lparen;
    Bc_VarDeclList *args_list;
    Bc_Token *rparen;
    Bc_Block *impl;
    Bc_TypeAnnotation *ret_type;

    if (EXPECT_TK(kw_fn, BC_KW_FUNC) && EXPECT_TK(name, BC_NAME) &&
        EXPECT_TK(lparen, BC_LPAREN)) {
        EXPECT(args_list, bc_func_args_rule);
        if (EXPECT_TK(rparen, BC_RPAREN)) {
            EXPECT(ret_type, bc_type_annotation_rule);
            if (EXPECT(impl, bc_block_rule)) {
                AST_ALLOC(Bc_FuncSig, func_sig){
                    .kw_fn = kw_fn,
                    .name = name,
                    .params = args_list,
                    .return_type = ret_type,
                };
                AST_ALLOC(Bc_FuncDecl, func_decl){
                    .alias = NULL,
                    .equals = NULL,
                    .signature = func_sig,
                    .target = NULL,
                    .impl = impl,
                };
                return BC_PACKRAT_SUCCESS(func_decl);
            }
        }
    }

    END_ALTERNATIVE()

    return BC_PACKRAT_FAILURE;
}

CREATE_RULE(bc_func_args_rule) {
    START_ALTERNATIVE(args_list)
    Bc_VarDecl *var_decl;
    size_t arg_count = 0;
    if (EXPECT(var_decl, bc_var_decl_rule)) {
        arg_count = 1;
        Bc_VarDecl *temp_var;
        Bc_Token *temp_comma;
        size_t reset_pos = bc_packrat_mark(p);
        while (EXPECT_TK(temp_comma, BC_COMMA) &&
               EXPECT(temp_var, bc_var_decl_rule)) {
            arg_count += 1;
        }
        bc_packrat_reset(p, reset_pos);

        AST_ALLOC(Bc_VarDeclList, args_list){
            .length = arg_count,
            .params = bc_arena_alloc(p->arena, sizeof(Bc_VarDecl) * arg_count),
        };
        args_list->params[0] = *var_decl;
        for (size_t i = 1; i < arg_count; i++) {
            EXPECT_TK(temp_comma, BC_COMMA);
            EXPECT(temp_var, bc_var_decl_rule);
            args_list->params[i] = *temp_var;
        }

        return BC_PACKRAT_SUCCESS(args_list);
    }
    END_ALTERNATIVE()
    return BC_PACKRAT_FAILURE;
}

CREATE_RULE(bc_var_decl_rule) {
    START_ALTERNATIVE(var_decl)
    Bc_Token *name;
    Bc_TypeAnnotation *type_annotation;
    if (EXPECT_TK(name, BC_NAME) &&
        EXPECT(type_annotation, bc_type_annotation_rule)) {
        AST_ALLOC(Bc_VarDecl, var_decl){
            .name = name,
            .type = type_annotation,
        };
        return BC_PACKRAT_SUCCESS(var_decl);
    }
    END_ALTERNATIVE()
    return BC_PACKRAT_FAILURE;
}

CREATE_RULE(bc_block_rule) { return BC_PACKRAT_FAILURE; }