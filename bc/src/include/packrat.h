#pragma once

#include <stdbool.h>
#include <stdlib.h>

#include "lexer.h"
#include "packrat_cache.h"

typedef struct {
    TokenScanner *ts;
    Bc_PackratCache cache;
} Bc_PackratParser;

// utility functions
size_t bc_packrat_mark(Bc_PackratParser *p);
void bc_packrat_reset(Bc_PackratParser *p, size_t pos);

typedef Bc_PackratRuleResult (*PackratRuleFunc)(Bc_PackratParser *);

// rule result processing
bool bc_rule_succees(Bc_PackratRuleResult result);

// expect functions
Bc_Token *bc_expect_tk(Bc_PackratParser *p, Bc_TokenType ty);
Bc_AstNode bc_expect_rule(PackratRuleFunc rule, Bc_PackratParser *p);

#define CREATE_RULE(function_name)                                             \
    Bc_PackratRuleResult function_name(Bc_PackratParser *p)

#define BC_PACKRAT_FAILURE                                                     \
    (Bc_PackratRuleResult) { .result = RC_PACKRAT_RESULT_FAILURE }

#define BC_PACKRAT_SUCCESS(ast_node)                                           \
    (Bc_PackratRuleResult) {                                                   \
        .result = RC_PACKRAT_RESULT_SUCCESS,                                   \
        .success = (Bc_PackratRuleSuccess) {                                   \
            .node = ast_node, .position = bc_packrat_mark(p)                   \
        }                                                                      \
    }