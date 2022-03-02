#pragma once

typedef enum {
    Bc_CursorKind_Expr,
    Bc_CursorKind_Stmt,
    Bc_CursorKind_Decl,
    Bc_CursorKind_IntegerLiteral,
    Bc_CursorKind_RealLiteral,
    Bc_CursorKind_Ident,
    Bc_CursorKind_UnaryOperation,
    Bc_CursorKind_BinaryOperation,
    Bc_CursorKind_MemberAccessor,
    Bc_CursorKind_FunctionCall,
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
    Bc_CursorKind_FunctionSignature,
    Bc_CursorKind_StructField,
    Bc_CursorKind_Assignable,
    Bc_CursorKind_Name
} Bc_CursorKind;

typedef enum {
    Bc_CursorVisitResult_Recurse,
    Bc_CursorVisitResult_Continue,
    Bc_CursorVisitResult_Breakm,
} Bc_CursorVisitResult;

typedef struct {
    Bc_CursorKind kind;
    const void *data;
} Bc_Cursor;

typedef Bc_CursorVisitResult (*Bc_CursorVisitor)(Bc_Cursor parent,
                                                 Bc_Cursor cursor);

void bc_cursor_visit_children(Bc_Cursor cursor, Bc_CursorVisitor visitor);