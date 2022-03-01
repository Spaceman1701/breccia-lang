#include "parser_rules.h"

#include <string.h>

#include "bc.h"

#include "ast.h"
#include "packrat.h"

bool expect_helper(void **out, PackratRuleFunc func, Bc_PackratParser *p) {
    size_t pos = bc_packrat_mark(p);
    bool success = ((*out) = bc_expect_rule(func, p));
    if (!success) {
        bc_packrat_reset(p, pos);
    }
    return success;
}

bool expect_token_helper(Bc_Token **out, Bc_TokenType type,
                         Bc_PackratParser *p) {
    size_t pos = bc_packrat_mark(p);
    bool success = ((*out) = bc_expect_tk(p, type));
    if (!success) {
        bc_packrat_reset(p, pos);
    }
    return success;
}

#define EXPECT(name, rule) (expect_helper((void **)&name, rule, p))

#define EXPECT_TK(name, type) (expect_token_helper(&name, type, p))

#define START_ALTERNATIVE(name)                                                \
    {                                                                          \
        const char *alt_name = #name;                                          \
        size_t pos = bc_packrat_mark(p);                                       \
        log_trace("trying alternative: %s", alt_name);

#define END_ALTERNATIVE()                                                      \
    bc_packrat_reset(p, pos);                                                  \
    }

#define AST_ALLOC(type, variable)                                              \
    type *variable = bc_arena_alloc(p->arena, sizeof(type));                   \
    (*variable) = (type)

CREATE_RULE(bc_integer_expr_rule) {
    size_t pos = bc_packrat_mark(p);
    Bc_Token *tk = NULL;
    if ((tk = bc_expect_tk(p, BC_INTEGER))) {
        Bc_IntegerExpr *expr = bc_arena_alloc(p->arena, sizeof(Bc_IntegerExpr));
        expr->integer = tk;
        return BC_PACKRAT_SUCCESS(expr);
    }
    bc_packrat_reset(p, pos);
    return BC_PACKRAT_FAILURE;
}

CREATE_RULE(bc_expr_rule) {
    START_ALTERNATIVE(binary_operation_mul_mod_div)
    Bc_Expr *left;
    Bc_Token *op;
    Bc_Expr *right;
    if (EXPECT(left, bc_expr_rule) &&
        (EXPECT_TK(op, BC_OP_STAR) || EXPECT_TK(op, BC_OP_DIV) ||
         EXPECT_TK(op, BC_OP_MOD)) &&
        EXPECT(right, bc_expr_rule)) {

        Bc_BinaryOpExpr *binop =
            bc_arena_alloc(p->arena, sizeof(Bc_BinaryOpExpr));
        *binop = (Bc_BinaryOpExpr){
            .left = left,
            .right = right,
            .operator= op,
        };

        Bc_Expr *expr = bc_arena_alloc(p->arena, sizeof(Bc_Expr));
        *expr = (Bc_Expr){
            .binary = binop,
            .kind = BC_EXPR_KIND_BINARY_OP,
        };

        return BC_PACKRAT_SUCCESS(expr);
    }
    END_ALTERNATIVE()

    START_ALTERNATIVE(binary_operation_add_sub)
    Bc_Expr *left;
    Bc_Token *op;
    Bc_Expr *right;
    if (EXPECT(left, bc_expr_rule) &&
        (EXPECT_TK(op, BC_OP_PLUS) | EXPECT_TK(op, BC_OP_MINUS)) &&
        EXPECT(right, bc_expr_rule)) {

        Bc_BinaryOpExpr *binop = malloc(sizeof(Bc_BinaryOpExpr));
        *binop = (Bc_BinaryOpExpr){
            .left = left,
            .right = right,
            .operator= op,
        };

        Bc_Expr *expr = bc_arena_alloc(p->arena, sizeof(Bc_Expr));
        *expr = (Bc_Expr){
            .binary = binop,
            .kind = BC_EXPR_KIND_BINARY_OP,
        };

        return BC_PACKRAT_SUCCESS(expr);
    }
    END_ALTERNATIVE()

    START_ALTERNATIVE(integer_literal)
    Bc_IntegerExpr *integer;
    if (EXPECT(integer, bc_integer_expr_rule)) {
        Bc_Expr *expr = bc_arena_alloc(p->arena, sizeof(Bc_Expr));
        *expr = (Bc_Expr){
            .integer_literal = integer,
            .kind = BC_EXPR_KIND_INTEGER_LIT,
        };
        return BC_PACKRAT_SUCCESS(expr);
    }
    END_ALTERNATIVE()

    return BC_PACKRAT_FAILURE;
}

CREATE_RULE(bc_mul_div_mod_op) {
    Bc_Token *op;
    START_ALTERNATIVE(mul)
    if (EXPECT_TK(op, BC_OP_STAR)) {
        return BC_PACKRAT_SUCCESS(op);
    }
    END_ALTERNATIVE()

    START_ALTERNATIVE(div)
    if (EXPECT_TK(op, BC_OP_DIV)) {
        return BC_PACKRAT_SUCCESS(op);
    }
    END_ALTERNATIVE()

    START_ALTERNATIVE(div)
    if (EXPECT_TK(op, BC_OP_MOD)) {
        return BC_PACKRAT_SUCCESS(op);
    }
    END_ALTERNATIVE()

    return BC_PACKRAT_FAILURE;
}

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