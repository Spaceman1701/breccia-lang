#include "cursor.h"

#include "bc.h"

#include "ast.h"

#define VISIT_AND_RET(parent, child, visitor, visitor_data)                    \
    if (!perform_visit(parent, child, visitor, visitor_data)) {                \
        return;                                                                \
    }

bool perform_visit(Bc_Cursor parent, Bc_Cursor child, Bc_CursorVisitor visitor,
                   Bc_VisitorData visitor_data) {
    Bc_CursorVisitResult result = visitor(parent, child, visitor_data);
    switch (result) {
    case Bc_CursorVisitResult_Recurse:
        bc_cursor_visit_children(child, visitor, visitor_data);
        return true;
    case Bc_CursorVisitResult_Break:
        return false;
    case Bc_CursorVisitResult_Continue:
        return true;
    }
}

void visit_module(Bc_Cursor cursor, Bc_CursorVisitor visitor,
                  Bc_VisitorData visitor_data) {
    const Bc_Module *module = cursor.data;

    for (size_t i = 0; i < module->length; i++) {
        Bc_Cursor child = {
            .data = &module->decls[i],
            .kind = Bc_CursorKind_Decl,
        };
        VISIT_AND_RET(cursor, child, visitor, visitor_data);
    }
}

void visit_expr(Bc_Cursor expr_cursor, Bc_CursorVisitor visitor,
                Bc_VisitorData visitor_data) {
    const Bc_Expr *expr = (const Bc_Expr *)expr_cursor.data;
    Bc_Cursor child_cursor;
    switch (expr->kind) {
    case BC_EXPR_KIND_BINARY_OP: {
        child_cursor = (Bc_Cursor){
            .data = expr->binary,
            .kind = Bc_CursorKind_BinaryOpExpr,
        };
        break;
    }
    case BC_EXPR_KIND_FUNC_CALL:
        child_cursor = (Bc_Cursor){
            .data = expr->function_call,
            .kind = Bc_CursorKind_FuncCallExpr,
        };
        break;

    case BC_EXPR_KIND_INTEGER_LIT:
        child_cursor = (Bc_Cursor){
            .data = expr->integer_literal,
            .kind = Bc_CursorKind_IntegerExpr,
        };
        break;
    case BC_EXPR_KIND_REAL_LIT:
        child_cursor = (Bc_Cursor){
            .data = expr->real_literal,
            .kind = Bc_CursorKind_RealExpr,
        };
        break;
    case BC_EXPR_KIND_VAR:
        child_cursor = (Bc_Cursor){
            .data = expr->variable,
            .kind = Bc_CursorKind_Ident,
        };
        break;
    case BC_EXPR_KIND_UNARY_OP:
        child_cursor = (Bc_Cursor){
            .data = expr->unary,
            .kind = Bc_CursorKind_UnaryOpExpr,
        };
        break;
    case BC_EXPR_KIND_MEMBER_ACCESSOR:
        child_cursor = (Bc_Cursor){
            .data = expr->memeber_accessor,
            .kind = Bc_CursorKind_MemberAccessExpr,
        };
        break;
    case BC_EXPR_KIND_PARENS:
        child_cursor = (Bc_Cursor){
            .data = expr->parens_expr,
            .kind = Bc_CursorKind_Expr,
        };
        break;
    }

    perform_visit(expr_cursor, child_cursor, visitor, visitor_data);
}

void visit_decl(Bc_Cursor decl_cursor, Bc_CursorVisitor visitor,
                Bc_VisitorData visitor_data) {
    const Bc_Decl *decl = (const Bc_Decl *)decl_cursor.data;
    Bc_Cursor child_cursor;
    switch (decl->kind) {
    case BC_DECL_KIND_STRUCT:
        child_cursor = (Bc_Cursor){
            .data = decl->struct_decl,
            .kind = Bc_CursorKind_StructDecl,
        };
        break;
    case BC_DECL_KIND_INTERFACE:
        child_cursor = (Bc_Cursor){
            .data = decl->iface_decl,
            .kind = Bc_CursorKind_InterfaceDecl,
        };
    case BC_DECL_KIND_FUNC:
        child_cursor = (Bc_Cursor){
            .data = decl->func_decl,
            .kind = Bc_CursorKind_FuncDecl,
        };
        break;
    case BC_DECL_KIND_IMPL:
        child_cursor = (Bc_Cursor){
            .data = decl->impl_decl,
            .kind = Bc_CursorKind_ImplDecl,
        };
        break;
    }

    perform_visit(decl_cursor, child_cursor, visitor, visitor_data);
}

void visit_struct_decl(Bc_Cursor struct_decl_cursor, Bc_CursorVisitor visitor,
                       Bc_VisitorData visitor_data) {
    const Bc_StructDecl *struct_decl =
        (const Bc_StructDecl *)struct_decl_cursor.data;

    Bc_Cursor child_cursor = {
        .data = struct_decl->name,
        .kind = Bc_CursorKind_Name,
    };

    bool should_continue =
        perform_visit(struct_decl_cursor, child_cursor, visitor, visitor_data);
    size_t field_index = 0;
    while (should_continue && (field_index < struct_decl->fields->length)) {
        child_cursor = (Bc_Cursor){
            .data = &struct_decl->fields->fields[field_index],
            .kind = Bc_CursorKind_StructField,
        };
        should_continue = perform_visit(struct_decl_cursor, child_cursor,
                                        visitor, visitor_data);
        field_index += 1;
    }
}

void visit_struct_field(Bc_Cursor struct_field_cursor, Bc_CursorVisitor visitor,
                        Bc_VisitorData visitor_data) {
    const Bc_StructField *field =
        (const Bc_StructField *)struct_field_cursor.data;
    Bc_Cursor child_cursor = {
        .data = field->name,
        .kind = Bc_CursorKind_Name,
    };

    if (perform_visit(struct_field_cursor, child_cursor, visitor,
                      visitor_data)) {
        child_cursor = (Bc_Cursor){
            .data = field->type,
            .kind = Bc_CursorKind_TypeAnnotation,
        };
        perform_visit(struct_field_cursor, child_cursor, visitor, visitor_data);
    }
}

void visit_type_annotation(Bc_Cursor type_annotation_cursor,
                           Bc_CursorVisitor visitor,
                           Bc_VisitorData visitor_data) {
    const Bc_TypeAnnotation *annotation = type_annotation_cursor.data;
    Bc_Cursor child_cursor = {
        .data = annotation->name,
        .kind = Bc_CursorKind_Name,
    };

    perform_visit(type_annotation_cursor, child_cursor, visitor, visitor_data);
}

void visit_stmt(Bc_Cursor type_annotation_cursor, Bc_CursorVisitor visitor,
                Bc_VisitorData visitor_data) {
    const Bc_Stmt *stmt = type_annotation_cursor.data;
    Bc_Cursor child_curosr;
    switch (stmt->kind) {
    case BC_STMT_KIND_ASSIGNMENT:
        child_curosr = (Bc_Cursor){
            .data = stmt->assignment_stmt,
            .kind = Bc_CursorKind_AssignmentStmt,
        };
        break;

    case BC_STMT_KIND_EXPR:
        child_curosr = (Bc_Cursor){
            .data = stmt->expr_stmt,
            .kind = Bc_CursorKind_ExprStmt,
        };
        break;
    case BC_STMT_KIND_IF:
        child_curosr = (Bc_Cursor){
            .data = stmt->if_stmt,
            .kind = Bc_CursorKind_IfStmt,
        };
        break;

    case BC_STMT_KIND_RETURN:
        child_curosr = (Bc_Cursor){
            .data = stmt->return_stmt,
            .kind = Bc_CursorKind_ReturnStmt,
        };
        break;
    }
}

void visit_unary_operation(Bc_Cursor unary_op, Bc_CursorVisitor visitor,
                           Bc_VisitorData visitor_data) {
    const Bc_UnaryOpExpr *unary = unary_op.data;
    Bc_Cursor child_cursor = {
        .data = unary->operator,
        .kind = Bc_CursorKind_Operator,
    };
    if (!perform_visit(unary_op, child_cursor, visitor, visitor_data)) {
        return;
    }

    child_cursor = (Bc_Cursor){
        .data = unary->expr,
        .kind = Bc_CursorKind_Expr,
    };
    perform_visit(unary_op, child_cursor, visitor, visitor_data);
}

void visit_binary_operation(Bc_Cursor binary_op_cursor,
                            Bc_CursorVisitor visitor,
                            Bc_VisitorData visitor_data) {
    const Bc_BinaryOpExpr *binary_op = binary_op_cursor.data;
    Bc_Cursor child_cursor = {
        .data = binary_op->operator,
        .kind = Bc_CursorKind_Operator,
    };
    if (!perform_visit(binary_op_cursor, child_cursor, visitor, visitor_data)) {
        return;
    }
    child_cursor = (Bc_Cursor){
        .data = binary_op->left,
        .kind = Bc_CursorKind_Expr,
    };
    if (!perform_visit(binary_op_cursor, child_cursor, visitor, visitor_data)) {
        return;
    }
    child_cursor = (Bc_Cursor){
        .data = binary_op->right,
        .kind = Bc_CursorKind_Expr,
    };
    perform_visit(binary_op_cursor, child_cursor, visitor, visitor_data);
}

void visit_member_accessor(Bc_Cursor member_accessor_cursor,
                           Bc_CursorVisitor visitor,
                           Bc_VisitorData visitor_data) {
    const Bc_MemberAccessExpr *member_accessor = member_accessor_cursor.data;
    Bc_Cursor child_cursor = {
        .data = member_accessor->expr,
        .kind = Bc_CursorKind_Expr,
    };
    if (!perform_visit(member_accessor_cursor, child_cursor, visitor,
                       visitor_data)) {
        return;
    }

    child_cursor = (Bc_Cursor){
        .data = member_accessor->member_name,
        .kind = Bc_CursorKind_Name,
    };
    perform_visit(member_accessor_cursor, child_cursor, visitor, visitor_data);
}

void visit_function_call(Bc_Cursor cursor, Bc_CursorVisitor visitor,
                         Bc_VisitorData visitor_data) {
    const Bc_FuncCallExpr *func_call = cursor.data;
    Bc_Cursor child_cursor = {
        .data = func_call->function,
        .kind = Bc_CursorKind_Expr,
    };
    if (!perform_visit(cursor, child_cursor, visitor, visitor_data)) {
        return;
    }

    for (size_t i = 0; i < func_call->args->length; i++) {
        child_cursor = (Bc_Cursor){
            .data = &func_call->args->exprs[i],
            .kind = Bc_CursorKind_Expr,
        };
        if (!perform_visit(cursor, child_cursor, visitor, visitor_data)) {
            return;
        }
    }
}

void visit_interface_decl(Bc_Cursor cursor, Bc_CursorVisitor visitor,
                          Bc_VisitorData visitor_data) {
    const Bc_InterfaceDecl *decl = cursor.data;
    Bc_Cursor child = {
        .data = decl->name,
        .kind = Bc_CursorKind_Name,
    };

    VISIT_AND_RET(cursor, child, visitor, visitor_data);

    for (size_t i = 0; i < decl->functions->length; i++) {
        child = (Bc_Cursor){
            .data = &decl->functions->functions[i],
            .kind = Bc_CursorKind_FuncDecl,
        };
        VISIT_AND_RET(cursor, child, visitor, visitor_data);
    }
}

void visit_impl_decl(Bc_Cursor cursor, Bc_CursorVisitor visitor,
                     Bc_VisitorData visitor_data) {
    const Bc_ImplDecl *decl = cursor.data;
    Bc_Cursor child = {
        .data = decl->target,
        .kind = Bc_CursorKind_ImplDecl,
    };
    VISIT_AND_RET(cursor, child, visitor, visitor_data);

    for (size_t i = 0; i < decl->decls->length; i++) {
        child = (Bc_Cursor){
            .data = &decl->decls->decls[i],
            .kind = Bc_CursorKind_Decl,
        };
        VISIT_AND_RET(cursor, child, visitor, visitor_data);
    }
}

void visit_function_decl(Bc_Cursor cursor, Bc_CursorVisitor visitor,
                         Bc_VisitorData visitor_data) {
    const Bc_FuncDecl *decl = cursor.data;
    Bc_Cursor child = (Bc_Cursor){
        .data = decl->signature,
        .kind = Bc_CursorKind_FuncSig,
    };
    VISIT_AND_RET(cursor, child, visitor, visitor_data);

    child = (Bc_Cursor){
        .data = decl->impl,
        .kind = Bc_CursorKind_Block,
    };
    VISIT_AND_RET(cursor, child, visitor, visitor_data);
}

void visit_var_decl(Bc_Cursor cursor, Bc_CursorVisitor visitor,
                    Bc_VisitorData visitor_data) {
    const Bc_VarDecl *decl = cursor.data;
    Bc_Cursor child = {
        .data = decl->name,
        .kind = Bc_CursorKind_Name,
    };
    VISIT_AND_RET(cursor, child, visitor, visitor_data);
    child = (Bc_Cursor){
        .data = decl->type,
        .kind = Bc_CursorKind_TypeAnnotation,
    };
    VISIT_AND_RET(cursor, child, visitor, visitor_data);
}

void visit_return_stmt(Bc_Cursor cursor, Bc_CursorVisitor visitor,
                       Bc_VisitorData visitor_data) {
    const Bc_ReturnStmt *stmt = cursor.data;
    Bc_Cursor child = {
        .data = stmt->expr,
        .kind = Bc_CursorKind_Expr,
    };
    VISIT_AND_RET(cursor, child, visitor, visitor_data);
}

void visit_expr_stmt(Bc_Cursor cursor, Bc_CursorVisitor visitor,
                     Bc_VisitorData visitor_data) {
    const Bc_ExprStmt *stmt = cursor.data;
    Bc_Cursor child = {
        .data = stmt->expr,
        .kind = Bc_CursorKind_Expr,
    };
    VISIT_AND_RET(cursor, child, visitor, visitor_data);
}

void visit_assignment_stmt(Bc_Cursor cursor, Bc_CursorVisitor visitor,
                           Bc_VisitorData visitor_data) {
    const Bc_AssignmentStmt *stmt = cursor.data;
    Bc_Cursor child = {
        .data = stmt->assignable,
        .kind = Bc_CursorKind_Assignable,
    };
    VISIT_AND_RET(cursor, child, visitor, visitor_data);

    child = (Bc_Cursor){
        .data = stmt->expr,
        .kind = Bc_CursorKind_Expr,
    };
    VISIT_AND_RET(cursor, child, visitor, visitor_data);
}

void visit_if_stmt(Bc_Cursor cursor, Bc_CursorVisitor visitor,
                   Bc_VisitorData visitor_data) {
    log_error("visit if statement is not implemented");
}

void visit_function_signature(Bc_Cursor cursor, Bc_CursorVisitor visitor,
                              Bc_VisitorData visitor_data) {
    const Bc_FuncSig *sig = cursor.data;
    Bc_Cursor child = {
        .data = sig->name,
        .kind = Bc_CursorKind_Name,
    };
    VISIT_AND_RET(cursor, child, visitor, visitor_data);

    for (size_t i = 0; i < sig->params->length; i++) {
        child = (Bc_Cursor){
            .data = &sig->params->params[i],
            .kind = Bc_CursorKind_VarDecl,
        };
        VISIT_AND_RET(cursor, child, visitor, visitor_data);
    }

    child = (Bc_Cursor){
        .data = sig->return_type,
        .kind = Bc_CursorKind_TypeAnnotation,
    };
    VISIT_AND_RET(cursor, child, visitor, visitor_data);
}

void visit_assignable(Bc_Cursor cursor, Bc_CursorVisitor visitor,
                      Bc_VisitorData visitor_data) {
    const Bc_Assignable *assignable = cursor.data;
    Bc_Cursor child;
    if (assignable->expr) {
        child = (Bc_Cursor){
            .data = assignable->expr,
            .kind = Bc_CursorKind_Expr,
        };
    } else {
        child = (Bc_Cursor){
            .data = assignable->var_decl,
            .kind = Bc_CursorKind_VarDecl,
        };
    }

    perform_visit(cursor, child, visitor, visitor_data);
}

void visit_block(Bc_Cursor cursor, Bc_CursorVisitor visitor,
                 Bc_VisitorData visitor_data) {
    const Bc_Block *block = cursor.data;

    for (size_t i = 0; i < block->length; i++) {
        Bc_Cursor child = {
            .data = &block->statements[i],
            .kind = Bc_CursorKind_Stmt,
        };
        VISIT_AND_RET(cursor, child, visitor, visitor_data);
    }
}

#define dispatch(func)                                                         \
    func(cursor, visitor, visitor_data);                                       \
    break;

void bc_cursor_visit_children(Bc_Cursor cursor, Bc_CursorVisitor visitor,
                              Bc_VisitorData visitor_data) {
    if (cursor.data == NULL) {
        return;
    }
    switch (cursor.kind) {
    case Bc_CursorKind_Module:
        dispatch(visit_module);
    case Bc_CursorKind_Expr:
        dispatch(visit_expr);
    case Bc_CursorKind_Stmt:
        dispatch(visit_stmt);
    case Bc_CursorKind_Decl:
        dispatch(visit_decl);
    case Bc_CursorKind_UnaryOpExpr:
        dispatch(visit_unary_operation);
    case Bc_CursorKind_BinaryOpExpr:
        dispatch(visit_binary_operation);
    case Bc_CursorKind_MemberAccessExpr:
        dispatch(visit_member_accessor);
    case Bc_CursorKind_FuncCallExpr:
        dispatch(visit_function_call);
    case Bc_CursorKind_StructDecl:
        dispatch(visit_struct_decl);
    case Bc_CursorKind_InterfaceDecl:
        dispatch(visit_interface_decl);
    case Bc_CursorKind_ImplDecl:
        dispatch(visit_impl_decl);
    case Bc_CursorKind_FuncDecl:
        dispatch(visit_function_decl);
    case Bc_CursorKind_VarDecl:
        dispatch(visit_var_decl);
    case Bc_CursorKind_ReturnStmt:
        dispatch(visit_return_stmt);
    case Bc_CursorKind_ExprStmt:
        dispatch(visit_expr_stmt);
    case Bc_CursorKind_AssignmentStmt:
        dispatch(visit_assignment_stmt);
    case Bc_CursorKind_IfStmt:
        dispatch(visit_if_stmt);
    case Bc_CursorKind_TypeAnnotation:
        dispatch(visit_type_annotation);
    case Bc_CursorKind_FuncSig:
        dispatch(visit_function_signature);
    case Bc_CursorKind_StructField:
        dispatch(visit_struct_field);
    case Bc_CursorKind_Assignable:
        dispatch(visit_assignable);
    case Bc_CursorKind_Block:
        dispatch(visit_block);
    default:
        break;
    }
}

#define cursor_getter(type, name)                                              \
    Bc_##type *name(Bc_Cursor c) {                                             \
        if (c.kind == Bc_CursorKind_##type) {                                  \
            return c.data;                                                     \
        } else {                                                               \
            return NULL;                                                       \
        }                                                                      \
    }

#include "cursor_getter_templates.h"
#undef cursor_getter

Bc_Token *bc_cursor_Name(Bc_Cursor c) {
    if (c.kind == Bc_CursorKind_Name) {
        return c.data;
    } else {
        return NULL;
    }
}
Bc_Token *bc_cursor_Operator(Bc_Cursor c) {
    if (c.kind == Bc_CursorKind_Operator) {
        return c.data;
    } else {
        return NULL;
    }
}