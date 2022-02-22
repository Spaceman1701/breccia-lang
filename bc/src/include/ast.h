#pragma once

#include "lexer.h"

#define FORWARD_DECLARE(name) typedef struct name name;

typedef enum Bc_ExprKind Bc_ExprKind;
FORWARD_DECLARE(Bc_Expr)

typedef enum Bc_StmtKind Bc_StmtKind;
FORWARD_DECLARE(Bc_Stmt)

typedef enum Bc_DeclKind Bc_DeclKind;
FORWARD_DECLARE(Bc_Decl)

FORWARD_DECLARE(Bc_Ident)
FORWARD_DECLARE(Bc_TypeAnnotation)

FORWARD_DECLARE(Bc_StructDecl)
FORWARD_DECLARE(Bc_StructFieldList)
FORWARD_DECLARE(Bc_StructField)
FORWARD_DECLARE(Bc_InterfaceDecl)
FORWARD_DECLARE(Bc_ImplDecl)
FORWARD_DECLARE(Bc_FuncSigList)
FORWARD_DECLARE(Bc_FuncDecl)
FORWARD_DECLARE(Bc_FuncSig)
FORWARD_DECLARE(Bc_VarDeclList)
FORWARD_DECLARE(Bc_VarDecl)

FORWARD_DECLARE(Bc_ReturnStmt)
FORWARD_DECLARE(Bc_ExprStmt)
FORWARD_DECLARE(Bc_AssignmentStmt)
FORWARD_DECLARE(Bc_IfStmt)
FORWARD_DECLARE(Bc_IfStmtBranch)
FORWARD_DECLARE(Bc_IfStmtBranchList)
FORWARD_DECLARE(Bc_Block)
FORWARD_DECLARE(Bc_Assignable)

FORWARD_DECLARE(Bc_IntegerExpr)
FORWARD_DECLARE(Bc_RealExpr)
FORWARD_DECLARE(Bc_UnaryOpExpr)
FORWARD_DECLARE(Bc_BinaryOpExpr)
FORWARD_DECLARE(Bc_MemberAccessExpr)
FORWARD_DECLARE(Bc_FuncCallExpr)
FORWARD_DECLARE(Bc_ExprList)

typedef enum { Bc_AstExpr, Bc_AstEnd } Bc_CachableAstNodeKinds;

enum Bc_DeclKind {
    Bc_DeclStructKind,
    Bc_DeclInterfaceKind,
    Bc_DeclImplKind,
    Bc_DeclFuncKind,
};

struct Bc_Decl {
    union {
        Bc_StructDecl *struct_decl;
        Bc_InterfaceDecl *iface_decl;
        Bc_ImplDecl *impl_decl;
        Bc_FuncDecl *func_decl;
    };

    Bc_DeclKind kind;
};

// Expr tagged union
enum Bc_ExprKind {
    Bc_ExprIntegerKind,
    Bc_ExprRealKind,
    Bc_ExprVariableKind,
    Bc_ExprUnaryOpType,
    Bc_ExprBinaryOpType,
    Bc_ExprMemoryAccessType,
    Bc_ExprFunctionCallType,
};

struct Bc_Expr {
    union {
        Bc_IntegerExpr *integer_literal;
        Bc_RealExpr *real_literal;
        Bc_Ident *variable;
        Bc_UnaryOpExpr *unary;
        Bc_BinaryOpExpr *binary;
        Bc_MemberAccessExpr *memeber_accessor;
        Bc_FuncCallExpr *function_call;
    };

    Bc_ExprKind kind;
};

enum Bc_StmtKind {
    Bc_StmtReturnKind,
    Bc_StmtExprKind,
    Bc_AssignmentStmtKind,
    Bc_IfStmtKind,
};

struct Bc_Stmt {
    union {
        Bc_ReturnStmt *return_stmt;
        Bc_ExprStmt *expr_stmt;
        Bc_AssignmentStmt *assignment_stmt;
        Bc_IfStmt *if_stmt;
    };

    Bc_StmtKind kind;
};

// potentially qualified ident
struct Bc_Ident {
    Bc_Token *name;
    Bc_Ident *ident;
    Bc_Token *op_scope;
};

// type as it appears in source code
struct Bc_TypeAnnotation {
    Bc_Token *name;
};

// struct type declaration
struct Bc_StructDecl {
    Bc_Token *kw_type;
    Bc_Token *name;
    Bc_Token *kw_struct;
    Bc_StructFieldList *fields;
    Bc_Token *rcurly;
};

// struct field declaration
struct Bc_StructField {
    Bc_Token *name;
    Bc_TypeAnnotation *type;
};

// list of struct fields (only occurs in struct decls)
struct Bc_StructFieldList {
    Bc_StructField *fields;
    size_t length;
};

struct Bc_InterfaceDecl {
    Bc_Token *kw_type;
    Bc_Token *name;
    Bc_FuncSigList *functions;
    Bc_Token *rcurly;
};

struct Bc_FuncSigList {
    Bc_FuncSig *functions;
    size_t length;
};

struct Bc_FuncDecl {
    Bc_FuncSig *signature;
    Bc_Token *alias;
    Bc_Token *equals;
    Bc_Ident *target;
};

struct Bc_FuncSig {
    Bc_Token *kw_fn;
    Bc_Token *name;
    Bc_VarDecl *params;
    Bc_TypeAnnotation *return_type;
};

struct Bc_VarDeclList {
    Bc_VarDecl *params;
    size_t length;
};

struct Bc_VarDecl {
    Bc_Token *name;
    Bc_TypeAnnotation *type;
};

struct Bc_ReturnStmt {
    Bc_Token *kw_return;
    Bc_Expr *expr;
};

struct Bc_AssignmentStmt {
    Bc_Assignable *assignable;
    Bc_Expr *expr;
    Bc_Token *semi;
};

struct ExprStmt {
    Bc_Expr *expr;
    Bc_Token *semi;
};

struct IfStmt {
    Bc_Token *kw_if;
    Bc_IfStmtBranch *main_branch;
    Bc_IfStmtBranchList *elif_branches;
    Bc_IfStmtBranch *else_branch;
    Bc_Token *rcurly;
};

struct Bc_IfStmtBranch {
    Bc_Expr *expr;
    Bc_Block *block;
};

struct Bc_IfStmtBranchList {
    Bc_IfStmtBranch *branches;
    size_t length;
};

struct Bc_Block {
    Bc_Token *lcurly;
    Bc_Stmt *statements;
    size_t length;
    Bc_Token *rcurly;
};

struct Bc_Assignable {
    Bc_Ident *ident;
    Bc_Expr *expr;
    Bc_Token *kw_let;
    Bc_VarDecl *var_decl;
};

struct Bc_UnaryOpExpr {
    Bc_Token *operator;
    Bc_Expr *expr;
};

struct Bc_BinaryOpExpr {
    Bc_Token *operator;
    Bc_Expr *left;
    Bc_Expr *right;
};

struct Bc_MemberAccessExpr {
    Bc_Expr *expr;
    Bc_Token *member_name;
};

struct Bc_FuncCallExpr {
    Bc_Expr function;
    Bc_ExprList *args;
};

struct Bc_ExprList {
    Bc_Expr *exprs;
    size_t length;
};

struct Bc_IntegerExpr {
    Bc_Token *integer;
};

struct Bc_RealExpr {
    Bc_Token *real_number;
};
