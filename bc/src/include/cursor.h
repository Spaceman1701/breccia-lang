#pragma once

#include "ast.h"
#include "lexer.h"

typedef enum {
    Bc_CursorKind_Module,
    Bc_CursorKind_Expr,
    Bc_CursorKind_Stmt,
    Bc_CursorKind_Decl,
    Bc_CursorKind_IntegerExpr,
    Bc_CursorKind_RealExpr,
    Bc_CursorKind_Ident,
    Bc_CursorKind_UnaryOpExpr,
    Bc_CursorKind_BinaryOpExpr,
    Bc_CursorKind_MemberAccessExpr,
    Bc_CursorKind_FuncCallExpr,
    Bc_CursorKind_StructDecl,
    Bc_CursorKind_InterfaceDecl,
    Bc_CursorKind_ImplDecl,
    Bc_CursorKind_FuncDecl,
    Bc_CursorKind_VarDecl,
    Bc_CursorKind_ReturnStmt,
    Bc_CursorKind_ExprStmt,
    Bc_CursorKind_AssignmentStmt,
    Bc_CursorKind_IfStmt,
    Bc_CursorKind_TypeAnnotation,
    Bc_CursorKind_FuncSig,
    Bc_CursorKind_StructField,
    Bc_CursorKind_Assignable,
    Bc_CursorKind_Name,
    Bc_CursorKind_Operator,
    Bc_CursorKind_Block,

} Bc_CursorKind;

typedef enum {
    Bc_CursorVisitResult_Recurse,
    Bc_CursorVisitResult_Continue,
    Bc_CursorVisitResult_Break,
} Bc_CursorVisitResult;

typedef struct {
    Bc_CursorKind kind;
    void *data;
} Bc_Cursor;

typedef void *Bc_VisitorData;

typedef Bc_CursorVisitResult (*Bc_CursorVisitor)(Bc_Cursor parent,
                                                 Bc_Cursor cursor,
                                                 Bc_VisitorData visitor_data);

void bc_cursor_visit_children(Bc_Cursor cursor, Bc_CursorVisitor visitor,
                              Bc_VisitorData visitor_data);

#define cursor_getter(type, name) Bc_##type *name(Bc_Cursor c);
#include "cursor_getter_templates.h"
#undef cursor_getter

Bc_Token *bc_cursor_Name(Bc_Cursor c);
Bc_Token *bc_cursor_Operator(Bc_Cursor c);