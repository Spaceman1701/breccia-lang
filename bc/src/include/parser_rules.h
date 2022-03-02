#pragma once

#include "packrat.h"

CREATE_RULE(bc_integer_expr_rule);
CREATE_RULE(bc_expr_rule);
CREATE_RULE(bc_mul_div_mod_op);

CREATE_RULE(bc_decl_rule);
CREATE_RULE(bc_struct_decl_rule);
CREATE_RULE(bc_struct_field_rule);

CREATE_RULE(bc_type_annotation_rule);

CREATE_RULE(bc_ident_rule);

CREATE_RULE(bc_func_decl_rule);
CREATE_RULE(bc_func_sig_rule);
CREATE_RULE(bc_block_rule);
CREATE_RULE(bc_var_decl_rule);

bool expect_helper(void **out, PackratRuleFunc func, Bc_PackratParser *p);
bool expect_token_helper(Bc_Token **out, Bc_TokenType type,
                         Bc_PackratParser *p);

#define EXPECT(name, rule) (expect_helper((void **)&name, rule, p))

#define EXPECT_TK(name, type) (expect_token_helper(&name, type, p))

#define START_ALTERNATIVE(name)                                                \
    {                                                                          \
        const char *alt_name = #name;                                          \
        size_t pos = bc_packrat_mark(p);                                       \
        log_trace("trying alternative: %s", alt_name);

#define END_ALTERNATIVE()                                                      \
    bc_packrat_reset(p, pos);                                                  \
    }

#define AST_ALLOC(type, variable)                                              \
    type *variable = bc_arena_alloc(p->arena, sizeof(type));                   \
    (*variable) = (type)
