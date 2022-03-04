#include "parser_rules.h"

#include "ast.h"
#include "bc.h"

CREATE_RULE(bc_stmt_rule) {
    START_ALTERNATIVE(return_stmt)
    Bc_Token *kw_return;
    Bc_Expr *expr;
    Bc_Token *semi;
    if (EXPECT_TK(kw_return, BC_KW_RETURN) && EXPECT(expr, bc_expr_rule) &&
        EXPECT_TK(semi, BC_SEMI)) {
        AST_ALLOC(Bc_ReturnStmt, ret){
            .expr = expr,
            .kw_return = kw_return,
        };
        AST_ALLOC(Bc_Stmt, stmt){
            .kind = BC_STMT_KIND_RETURN,
            .return_stmt = ret,
        };
        return BC_PACKRAT_SUCCESS(stmt);
    }
    END_ALTERNATIVE()

    START_ALTERNATIVE(expr_stmt);
    Bc_Expr *expr;
    Bc_Token *semi;
    if (EXPECT(expr, bc_expr_rule) && EXPECT_TK(semi, BC_SEMI)) {
        AST_ALLOC(Bc_ExprStmt, expr_stmt){
            .expr = expr,
            .semi = semi,
        };
        AST_ALLOC(Bc_Stmt, stmt){
            .kind = BC_STMT_KIND_EXPR,
            .expr_stmt = expr_stmt,
        };
        return BC_PACKRAT_SUCCESS(stmt);
    }
    END_ALTERNATIVE()

    START_ALTERNATIVE(assignment_stmt);
    Bc_Assignable *assignable;
    Bc_Token *op_assign;
    Bc_Expr *expr;
    Bc_Token *semi;
    if (EXPECT(assignable, bc_assignable_rule) &&
        EXPECT_TK(op_assign, BC_OP_ASSIGN) && EXPECT(expr, bc_expr_rule) &&
        EXPECT_TK(semi, BC_SEMI)) {
        AST_ALLOC(Bc_AssignmentStmt, assignment){
            .assignable = assignable,
            .expr = expr,
            .semi = semi,
        };
        AST_ALLOC(Bc_Stmt, stmt){
            .kind = BC_STMT_KIND_ASSIGNMENT,
            .assignment_stmt = assignment,
        };
        return BC_PACKRAT_SUCCESS(stmt);
    }
    END_ALTERNATIVE()
    return BC_PACKRAT_FAILURE;
}

CREATE_RULE(bc_assignable_rule) {
    START_ALTERNATIVE(assignment_expr)
    Bc_Expr *expr;
    if (EXPECT(expr, bc_expr_rule)) {
        AST_ALLOC(Bc_Assignable, assignable){
            .expr = expr,
            .kw_let = NULL,
            .var_decl = NULL,
        };
        return BC_PACKRAT_SUCCESS(assignable);
    }
    END_ALTERNATIVE()

    START_ALTERNATIVE(let_binding)
    Bc_Token *kw_let;
    Bc_VarDecl *var_decl;
    if (EXPECT_TK(kw_let, BC_KW_LET) && EXPECT(var_decl, bc_var_decl_rule)) {
        AST_ALLOC(Bc_Assignable, assignable){
            .kw_let = kw_let,
            .var_decl = var_decl,
        };
        return BC_PACKRAT_SUCCESS(assignable);
    }
    END_ALTERNATIVE()
    return BC_PACKRAT_FAILURE;
}

CREATE_RULE(bc_block_rule) {
    START_ALTERNATIVE(block);
    Bc_Token *lcurly;
    Bc_Token *rcurly;
    if (EXPECT_TK(lcurly, BC_LCURLY)) {
        AST_ALLOC(Bc_Block, block){
            .statements = NULL, .length = 0, .lcurly = lcurly, .rcurly = NULL};

        EXPECT_LIST(Bc_Stmt, bc_stmt_rule, block->statements, block->length);
        if (EXPECT_TK(rcurly, BC_RCURLY)) {
            return BC_PACKRAT_SUCCESS(block);
        }
    }

    END_ALTERNATIVE()

    return BC_PACKRAT_FAILURE;
}