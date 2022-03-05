#include "parser_rules.h"

#include "bc.h"

#include "ast.h"
#include "packrat.h"

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

CREATE_RULE(bc_real_expr_rule) {
    size_t pos = bc_packrat_mark(p);
    Bc_Token *tk = NULL;
    if ((tk = bc_expect_tk(p, BC_REAL))) {
        Bc_RealExpr *expr = bc_arena_alloc(p->arena, sizeof(Bc_RealExpr));
        expr->real_number = tk;
        return BC_PACKRAT_SUCCESS(expr);
    }
    bc_packrat_reset(p, pos);
    return BC_PACKRAT_FAILURE;
}

CREATE_RULE(bc_unary_op_rule) {
    START_ALTERNATIVE(unary_op)
    Bc_Token *op;
    Bc_Expr *expr;
    if (EXPECT_TK(op, BC_OP_STAR) && EXPECT(expr, bc_expr_rule)) {
        AST_ALLOC(Bc_UnaryOpExpr, unary_op){
            .expr = expr,
            .operator= op,
        };
        return BC_PACKRAT_SUCCESS(unary_op);
    }
    END_ALTERNATIVE()
    return BC_PACKRAT_FAILURE;
}

CREATE_RULE(bc_expr_rule) {
    START_ALTERNATIVE(unary_operators)
    Bc_UnaryOpExpr *unary_op;
    if (EXPECT(unary_op, bc_unary_op_rule)) {
        AST_ALLOC(Bc_Expr, expr){
            .unary = unary_op,
            .kind = BC_EXPR_KIND_UNARY_OP,
        };
        return BC_PACKRAT_SUCCESS(expr);
    }
    END_ALTERNATIVE()

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
        (EXPECT_TK(op, BC_OP_PLUS) || EXPECT_TK(op, BC_OP_MINUS)) &&
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

    START_ALTERNATIVE(func_call_expr)
    Bc_Expr *func_expr;
    Bc_ExprList *args;
    if (EXPECT(func_expr, bc_expr_rule) &&
        EXPECT(args, bc_func_call_args_rule)) {

        AST_ALLOC(Bc_FuncCallExpr, func_call){
            .args = args,
            .function = func_expr,
        };
        AST_ALLOC(Bc_Expr, expr){
            .function_call = func_call,
            .kind = BC_EXPR_KIND_FUNC_CALL,
        };
        return BC_PACKRAT_SUCCESS(expr);
    }
    END_ALTERNATIVE()

    START_ALTERNATIVE(member_access_expr);
    Bc_Expr *parent;
    Bc_Token *member_name;
    if (EXPECT(parent, bc_expr_rule) && EXPECT_TK(member_name, BC_NAME)) {
        AST_ALLOC(Bc_MemberAccessExpr, member_accessor){
            .expr = parent,
            .member_name = member_name,
        };
        AST_ALLOC(Bc_Expr, expr){
            .memeber_accessor = member_accessor,
            .kind = BC_EXPR_KIND_MEMBER_ACCESSOR,
        };
        return BC_PACKRAT_SUCCESS(expr);
    }
    END_ALTERNATIVE()

    START_ALTERNATIVE(parens_expr)
    Bc_Token *lparen;
    Bc_Expr *expr;
    Bc_Token *rparen;
    if (EXPECT_TK(lparen, BC_LPAREN) && EXPECT(expr, bc_expr_rule) &&
        EXPECT_TK(rparen, BC_RPAREN)) {

        AST_ALLOC(Bc_Expr, outer){
            .kind = BC_EXPR_KIND_PARENS,
            .parens_expr = expr,
        };
        return BC_PACKRAT_SUCCESS(outer);
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

    START_ALTERNATIVE(real_literal)
    Bc_RealExpr *real;
    if (EXPECT(real, bc_real_expr_rule)) {
        AST_ALLOC(Bc_Expr, expr){
            .kind = BC_EXPR_KIND_REAL_LIT,
            .real_literal = real,
        };
        return BC_PACKRAT_SUCCESS(expr);
    }
    END_ALTERNATIVE()

    START_ALTERNATIVE(ident_expr)
    Bc_Ident *ident;
    if (EXPECT(ident, bc_ident_rule)) {
        AST_ALLOC(Bc_Expr, expr){
            .variable = ident,
            .kind = BC_EXPR_KIND_VAR,
        };
        return BC_PACKRAT_SUCCESS(expr);
    }
    END_ALTERNATIVE()

    return BC_PACKRAT_FAILURE;
}

CREATE_RULE(bc_func_call_args_rule) {
    START_ALTERNATIVE(func_call_args)
    Bc_Token *lparen;
    Bc_Expr *first_arg;
    Bc_Expr *temp_arg;
    Bc_Token *rparen;

    if (EXPECT_TK(lparen, BC_LPAREN)) {
        AST_ALLOC(Bc_ExprList, args_list){
            .exprs = NULL,
            .length = 0,
        };

        if (EXPECT(first_arg, bc_expr_rule)) {
            size_t pos = bc_packrat_mark(p);
            Bc_Token *temp_comma;
            args_list->length = 1;
            while (EXPECT_TK(temp_comma, BC_COMMA) &&
                   EXPECT(temp_arg, bc_expr_rule)) {
                args_list->length += 1;
            }
            bc_packrat_reset(p, pos);
            args_list->exprs =
                bc_arena_alloc(p->arena, sizeof(Bc_Expr) * args_list->length);
            args_list->exprs[0] = *first_arg;
            for (size_t i = 1; i < args_list->length; i++) {
                EXPECT_TK(temp_comma, BC_COMMA);
                EXPECT(temp_arg, bc_expr_rule);
                args_list->exprs[i] = *temp_arg;
            }
        }

        if (EXPECT_TK(rparen, BC_RPAREN)) {
            return BC_PACKRAT_SUCCESS(args_list);
        }
    }

    END_ALTERNATIVE()
    return BC_PACKRAT_FAILURE;
}