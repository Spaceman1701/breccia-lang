#include "backend/breccia_c.h"
#include "bc.h"
#include <stdio.h>

void bc_write_c_typename(FILE *os, Bc_CTypeName *t) {
    switch (t->kind) {
    case Bc_CTypeSymbolKind_Struct:
        fprintf(os, "struct ");
        break;
    default:
        break;
    }
    fprintf(os, "%s", t->name);
}
void bc_write_c_type(FILE *os, Bc_CTypeAnnotation *t) {
    bc_write_c_typename(os, &t->typename);
}
void bc_write_c_typedef(FILE *os, Bc_CTypeDefDeclaration *t) {
    fprintf(os, "typedef ");
    bc_write_c_type(os, &t->symbol_name);
    fprintf(os, " %s;\n", t->type_name);
}

void bc_write_c_vardecl(FILE *os, Bc_CVariable *v) {
    bc_write_c_type(os, &v->type);
    fprintf(os, " %s", v->name);
}

void bc_write_c_struct(FILE *os, Bc_CStruct *s) {
    fprintf(os, "struct %s {\n", s->name);
    for (size_t i = 0; i < s->field_count; i++) {
        bc_write_c_vardecl(os, &s->fields[i]);
        fprintf(os, ";\n");
    }
    fprintf(os, "}\n");
}