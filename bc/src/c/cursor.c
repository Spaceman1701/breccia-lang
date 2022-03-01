#include "cursor.h"

#include "bc.h"

#include "ast.h"

#define kind(name) Bc_CursorKind_##name

bool perform_visit(Bc_Cursor parent, Bc_Cursor child,
                   Bc_CursorVisitor visitor) {
    Bc_CursorVisitResult result = visitor(parent, child);
    switch (result) {
    case Bc_CursorVisitResult_Recurse:
        bc_cursor_visit_children(child, visitor);
        return true;
    case Bc_CursorVisitResult_Breakm:
        return false;
    case Bc_CursorVisitResult_Continue:
        return true;
    }
}

void visit_expr(Bc_Cursor expr_cursor, Bc_CursorVisitor visitor) {
    const Bc_Expr *expr = (const Bc_Expr *)expr_cursor.data;
    Bc_Cursor child_cursor;
    switch (expr->kind) {
    case BC_EXPR_KIND_BINARY_OP: {
        child_cursor = (Bc_Cursor){
            .data = expr->binary,
            .kind = Bc_CursorKind_BinaryOperation,
        };
        break;
    }
    case BC_EXPR_KIND_FUNC_CALL:
        child_cursor = (Bc_Cursor){
            .data = expr->function_call,
            .kind = Bc_CursorKind_FunctionCall,
        };
        break;
    }

    perform_visit(expr_cursor, child_cursor, visitor);
}

void visit_decl(Bc_Cursor decl_cursor, Bc_CursorVisitor visitor) {
    const Bc_Decl *decl = (const Bc_Decl *)decl_cursor.data;
    Bc_Cursor child_cursor;
    switch (decl->kind) {
    case BC_DECL_KIND_STRUCT:
        child_cursor = (Bc_Cursor){
            .data = decl->struct_decl,
            .kind = Bc_CursorKind_StructDecl,
        };
        perform_visit(decl_cursor, child_cursor, visitor);
        break;
    }
}

void visit_struct_decl(Bc_Cursor struct_decl_cursor, Bc_CursorVisitor visitor) {
    const Bc_StructDecl *struct_decl =
        (const Bc_StructDecl *)struct_decl_cursor.data;

    Bc_Cursor child_cursor = {
        .data = struct_decl->name,
        .kind = Bc_CursorKind_Name,
    };

    bool should_continue =
        perform_visit(struct_decl_cursor, child_cursor, visitor);
    size_t field_index = 0;
    while (should_continue && (field_index < struct_decl->fields->length)) {
        child_cursor = (Bc_Cursor){
            .data = &struct_decl->fields->fields[field_index],
            .kind = Bc_CursorKind_StructField,
        };
        should_continue =
            perform_visit(struct_decl_cursor, child_cursor, visitor);
        field_index += 1;
    }
}

void visit_struct_field(Bc_Cursor struct_field_cursor,
                        Bc_CursorVisitor visitor) {
    const Bc_StructField *field =
        (const Bc_StructField *)struct_field_cursor.data;
    Bc_Cursor child_cursor = {
        .data = field->name,
        .kind = Bc_CursorKind_Name,
    };

    if (perform_visit(struct_field_cursor, child_cursor, visitor)) {
        child_cursor = (Bc_Cursor){
            .data = field->type,
            .kind = Bc_CursorKind_TypeAnnotation,
        };
        perform_visit(struct_field_cursor, child_cursor, visitor);
    }
}

void visit_type_annotation(Bc_Cursor type_annotation_cursor,
                           Bc_CursorVisitor visitor) {
    const Bc_TypeAnnotation *annotation = type_annotation_cursor.data;
    Bc_Cursor child_cursor = {
        .data = annotation->name,
        .kind = Bc_CursorKind_Name,
    };

    perform_visit(type_annotation_cursor, child_cursor, visitor);
}

#define dispatch(func)                                                         \
    func(cursor, visitor);                                                     \
    break;

void bc_cursor_visit_children(Bc_Cursor cursor, Bc_CursorVisitor visitor) {
    switch (cursor.kind) {
    case kind(Expr):
        dispatch(visit_expr);
    case kind(Decl):
        dispatch(visit_decl);
    case Bc_CursorKind_StructDecl:
        dispatch(visit_struct_decl);
    case Bc_CursorKind_StructField:
        dispatch(visit_struct_field);
    case Bc_CursorKind_Name:
        break;
    case Bc_CursorKind_TypeAnnotation:
        dispatch(visit_type_annotation);
    }
}