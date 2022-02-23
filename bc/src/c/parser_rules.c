#include "parser_rules.h"

#include "ast.h"
#include "packrat.h"

CREATE_RULE(bc_integer_expr_rule) {
    size_t pos = bc_packrat_mark(p);
    Bc_Token *tk = NULL;
    if ((tk = bc_expect_tk(p, BC_INTEGER))) {
        Bc_IntegerExpr *expr = malloc(sizeof(Bc_IntegerExpr));
        expr->integer = tk;
        return BC_PACKRAT_SUCCESS(expr);
    }
    bc_packrat_reset(p, pos);
    return BC_PACKRAT_FAILURE;
}
CREATE_RULE(bc_binop_expr_rule) {
    size_t pos = bc_packrat_mark(p);
    Bc_Expr *left;
    Bc_Token *op;
    Bc_Expr *right;
    if ((left = bc_expect_rule(bc_expr_rule, p)) &&
        (op = bc_expect_tk(p, BC_OP_STAR)) &&
        (right = bc_expect_rule(bc_expr_rule, p))) {

        Bc_BinaryOpExpr *expr = malloc(sizeof(Bc_BinaryOpExpr));
        // Bc_Expr *left_e = malloc(sizeof(Bc_Expr));
        // Bc_Expr *right_e = malloc(sizeof(Bc_Expr));
        // left_e->kind = Bc_ExprIntegerKind;
        // right_e->kind = Bc_ExprIntegerKind;
        // left_e->integer_literal = left;
        // right_e->integer_literal = right;
        *expr = (Bc_BinaryOpExpr){
            .left = left,
            .right = right,
            .operator= op,
        };

        return BC_PACKRAT_SUCCESS(expr);
    }
    bc_packrat_reset(p, pos);
    return BC_PACKRAT_FAILURE;
}

CREATE_RULE(bc_expr_rule) {
    size_t pos = bc_packrat_mark(p);

    Bc_BinaryOpExpr *binop;
    if ((binop = bc_expect_rule(bc_binop_expr_rule, p))) {
        Bc_Expr *expr = malloc(sizeof(Bc_Expr));
        *expr = (Bc_Expr){
            .binary = binop,
            .kind = Bc_ExprBinaryOpType,
        };
        return BC_PACKRAT_SUCCESS(expr);
    }
    bc_packrat_reset(p, pos);

    Bc_IntegerExpr *integer;
    if ((integer = bc_expect_rule(bc_integer_expr_rule, p))) {
        Bc_Expr *expr = malloc(sizeof(Bc_Expr));
        *expr = (Bc_Expr){
            .integer_literal = integer,
            .kind = Bc_ExprIntegerKind,
        };
        return BC_PACKRAT_SUCCESS(expr);
    }
    bc_packrat_reset(p, pos);

    return BC_PACKRAT_FAILURE;
}