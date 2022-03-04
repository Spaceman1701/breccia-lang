#pragma once

#include "packrat.h"

CREATE_RULE(bc_module_rule);

CREATE_RULE(bc_integer_expr_rule);
CREATE_RULE(bc_expr_rule);
CREATE_RULE(bc_real_expr_rule);

CREATE_RULE(bc_decl_rule);
CREATE_RULE(bc_struct_decl_rule);
CREATE_RULE(bc_struct_field_rule);

CREATE_RULE(bc_type_annotation_rule);

CREATE_RULE(bc_ident_rule);

CREATE_RULE(bc_func_decl_rule);
CREATE_RULE(bc_func_sig_rule);
CREATE_RULE(bc_block_rule);
CREATE_RULE(bc_var_decl_rule);

CREATE_RULE(bc_func_args_rule);

CREATE_RULE(bc_stmt_rule);
CREATE_RULE(bc_assignable_rule);

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

#define EXPECT_LIST(entry_type, rule, out_data, out_len)                       \
    {                                                                          \
        size_t before_list = bc_packrat_mark(p);                               \
        entry_type *temp_entry;                                                \
        size_t count = 0;                                                      \
        while (EXPECT(temp_entry, rule)) {                                     \
            count += 1;                                                        \
        }                                                                      \
        bc_packrat_reset(p, before_list);                                      \
        out_data = bc_arena_alloc(p->arena, sizeof(entry_type) * count);       \
        out_len = count;                                                       \
        for (size_t i = 0; i < count; i++) {                                   \
            temp_entry = (entry_type *)bc_expect_rule(rule, p);                \
            out_data[i] = *temp_entry;                                         \
        }                                                                      \
    }