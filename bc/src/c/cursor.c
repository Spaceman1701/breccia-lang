#include "cursor.h"

#include "ast.h"

#define kind(name) Bc_CursorKind_##name

void bc_cursor_visit_children(Bc_Cursor cursor, Bc_CursorVisitor visitor) {
    // Bc_Cursor parent = cursor;
    // Bc_Cursor current;
    // Bc_CursorVisitResult result = Bc_CursorVisitResult_Recurse;
    // while (result != Bc_CursorVisitResult_Breakm) {
    //     switch (cursor.kind) {
    //     case kind(BinaryOperation): {
    //         Bc_BinaryOpExpr *expr = (Bc_BinaryOpExpr *)parent.data;
    //         Bc_Cursor new = {
    //             .data = expr->left,
    //             .kind = kind(Expr),
    //         };
    //         visitor(cursor, new);
    //         break;
    //     }
    //     };
    // }
}