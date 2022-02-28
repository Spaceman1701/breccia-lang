#include "parser_rules.h"

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

CREATE_RULE(bc_decl_rule) {}

CREATE_RULE(bc_struct_decl_rule);