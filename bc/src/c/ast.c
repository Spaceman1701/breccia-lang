#include "ast.h"

#include "bc.h"

void bc_ast_free_ident(Bc_Ident *ident) {}

void bc_ast_free_expr(Bc_Expr *expr) {
    switch (expr->kind) {
    case BC_EXPR_KIND_INTEGER_LIT:
        free(expr->integer_literal);
        break;
    case BC_EXPR_KIND_REAL_LIT:
        free(expr->real_literal);
        break;
    case BC_EXPR_KIND_VAR:
        bc_ast_free_ident(expr->variable);
        break;
    case BC_EXPR_KIND_UNARY_OP:
        bc_ast_free_expr(expr->unary->expr);
        free(expr->unary);
        break;
    case BC_EXPR_KIND_BINARY_OP:
        bc_ast_free_expr(expr->binary->left);
        bc_ast_free_expr(expr->binary->right);
        free(expr->binary);
        break;
    case BC_EXPR_KIND_MEMBER_ACCESSOR:
        bc_ast_free_expr(expr->memeber_accessor->expr);
        free(expr->memeber_accessor);
        break;
    case BC_EXPR_KIND_FUNC_CALL:
        bc_ast_free_expr(expr->function_call->function);
        for (size_t i = 0; i < expr->function_call->args->length; i++) {
            bc_ast_free_expr(&expr->function_call->args->exprs[i]);
        }
        free(expr->function_call->args);
        free(expr->function_call);
        break;
    }
}