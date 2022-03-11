
#include "bc.h"

#include "ast.h"
#include "cursor.h"
#include "translation_unit.h"
#include "type.h"

Bc_TypeReference resolve_type_reference(Bc_TypeIndex type_index,
                                        Bc_TypeAnnotation *type_annotation) {
    return (Bc_TypeReference){
        .unresolved_name =
            bc_type_index_alloc_token_text(type_index, type_annotation->name),
        .kind = Bc_TypeReferenceKind_Value,
    };
}

Bc_CursorVisitResult types_visitor(Bc_Cursor parent, Bc_Cursor node,
                                   Bc_TypeIndex type_index) {
    switch (node.kind) {
    case Bc_CursorKind_StructDecl: {
        Bc_StructDecl *decl = bc_cursor_StructDecl(node);
        char *name = bc_type_index_alloc_token_text(type_index, decl->name);
        log_trace("visiting struct decl %s (%p)", name, decl);
        Bc_Type *type = bc_type_index_create_type(type_index, name);
        log_trace("created new type %p", type);
        type->kind = Bc_TypeKind_Struct;
        type->struct_type = (Bc_TypeStruct){
            .decl = decl,
            .field_count = decl->fields->length,
            .fields = bc_type_index_alloc(
                type_index, sizeof(Bc_TypeStructField) * decl->fields->length),
        };
        for (size_t i = 0; i < decl->fields->length; i++) {
            Bc_StructField *field = &decl->fields->fields[i];
            Bc_TypeStructField *resolved_field = &type->struct_type.fields[i];
            *resolved_field = (Bc_TypeStructField){
                .name = bc_type_index_alloc_token_text(type_index, field->name),
                .type = resolve_type_reference(type_index, field->type),
            };
        }
        log_trace("done visiting struct decl %s (%p)", name, decl);
        return Bc_CursorVisitResult_Continue;
    }
    case Bc_CursorKind_InterfaceDecl: {
        return Bc_CursorVisitResult_Continue;
    }
    case Bc_CursorKind_FuncDecl: {
        return Bc_CursorVisitResult_Continue;
    }
    default:
        return Bc_CursorVisitResult_Recurse;
    }
}

void add_module_types_to_index(Bc_Cursor module_cursor,
                               Bc_TypeIndex type_index) {
    log_trace("starting inital decl visitors");
    bc_cursor_visit_children(module_cursor, (Bc_CursorVisitor)types_visitor,
                             type_index);
}

void bc_create_type_index(Bc_TranslationUnit *tu) {
    for (size_t i = 0; i < bc_tu_module_count(tu); i++) {
        log_trace("generating type information for module %zu", i);
        add_module_types_to_index(bc_tu_get_module_cursor(tu, i),
                                  tu->type_index);
    }
    bc_type_index_resolve(tu->type_index);
}