#pragma once

#define kind(name) Bc_CursorKind_##name
typedef enum {
    kind(Expr),
    kind(Stmt),
    kind(Decl),
    kind(IntegerLiteral),
    kind(RealLiteral),
    kind(Ident),
    kind(UnaryOperation),
    kind(BinaryOperation),
    kind(MemberAccessor),
    kind(FunctionCall),
    kind(StructDecl),
    kind(InterfaceDecl),
    kind(ImplDecl),
    kind(FuncDecl),
    kind(VarDecl),
    kind(ReturnStmt),
    kind(ExprStmt),
    kind(AssignmentStmt),
    kind(IfStmt),
    kind(TypeAnnotation),
    kind(FunctionSignature),
    kind(StructField),
    kind(Assignable),
} Bc_CursorKind;
#undef kind

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