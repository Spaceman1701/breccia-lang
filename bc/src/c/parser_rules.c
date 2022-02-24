#include "parser_rules.h"

#include <stdbool.h>

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

CREATE_RULE(bc_expr_rule) {

    {
        size_t pos = bc_packrat_mark(p);

        Bc_Expr *left;
        Bc_Token *op;
        Bc_Expr *right;
        if (EXPECT(left, bc_expr_rule) && EXPECT_TK(op, BC_OP_STAR) &&
            EXPECT(right, bc_expr_rule)) {
            printf("foud binop at position %zu (ending at %zu)\n", pos,
                   p->ts->cursor);

            Bc_BinaryOpExpr *binop = malloc(sizeof(Bc_BinaryOpExpr));
            *binop = (Bc_BinaryOpExpr){
                .left = left,
                .right = right,
                .operator= op,
            };

            Bc_Expr *expr = malloc(sizeof(Bc_Expr));
            *expr = (Bc_Expr){
                .binary = binop,
                .kind = Bc_ExprBinaryOpType,
            };

            return BC_PACKRAT_SUCCESS(expr);
        }

        bc_packrat_reset(p, pos);
    }

    {
        size_t pos = bc_packrat_mark(p);
        Bc_IntegerExpr *integer;
        if (EXPECT(integer, bc_integer_expr_rule)) {
            Bc_Expr *expr = malloc(sizeof(Bc_Expr));
            *expr = (Bc_Expr){
                .integer_literal = integer,
                .kind = Bc_ExprIntegerKind,
            };
            printf("found integer! New pos: %zu\n", bc_packrat_mark(p));
            return BC_PACKRAT_SUCCESS(expr);
        }
        bc_packrat_reset(p, pos);
    }

    return BC_PACKRAT_FAILURE;
}