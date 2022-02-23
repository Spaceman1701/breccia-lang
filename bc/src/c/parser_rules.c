#include "parser_rules.h"

#include "ast.h"
#include "packrat.h"

CREATE_RULE(bc_integer_expr_rule) {
    size_t pos = bc_packrat_mark(p);
    printf("in integer_expr rule at pos %zu\n", pos);
    Bc_Token *tk = NULL;
    if ((tk = bc_expect_tk(p, BC_INTEGER))) {
        Bc_IntegerExpr *expr = malloc(sizeof(Bc_IntegerExpr));
        expr->integer = tk;
        return BC_PACKRAT_SUCCESS(expr);
    }
    bc_packrat_reset(p, pos);
    printf("integer_expr rule failed\n");
    return BC_PACKRAT_FAILURE;
}
CREATE_RULE(bc_binop_expr_rule) {
    size_t pos = bc_packrat_mark(p);
    Bc_IntegerExpr *left;
    Bc_Token *op;
    Bc_IntegerExpr *right;
    if ((left = bc_expect_rule(bc_integer_expr_rule, p)) &&
        (op = bc_expect_tk(p, BC_OP_STAR)) &&
        (right = bc_expect_rule(bc_integer_expr_rule, p))) {

        Bc_BinaryOpExpr *expr = malloc(sizeof(Bc_BinaryOpExpr));
        *expr = (Bc_BinaryOpExpr){
            .left = NULL,
            .right = NULL,
            .operator= op,
        };

        return BC_PACKRAT_SUCCESS(expr);
    }
    bc_packrat_reset(p, pos);
    return BC_PACKRAT_FAILURE;
}

CREATE_RULE(bc_expr_rule);