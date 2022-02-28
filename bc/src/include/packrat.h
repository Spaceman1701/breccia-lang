#pragma once

#include <stdbool.h>
#include <stdlib.h>

#include "arena.h"
#include "lexer.h"
#include "packrat_cache.h"

/* see http://web.cs.ucla.edu/~todd/research/pepm08.pdf
 this file (and it's implementation) implement a LL(k) parser
 with support for DIRECT left recursion through the simplified seed-growing
 algorithm

 Rules with INDIRECT left recursion WILL PARSE INCORRECTLY
*/
typedef struct {
    TokenScanner *ts;
    Bc_PackratCache cache;
    Bc_Arena arena;
} Bc_PackratParser;

// utility functions
size_t bc_packrat_mark(Bc_PackratParser *p);
void bc_packrat_reset(Bc_PackratParser *p, size_t pos);

void bc_packrat_free_all_owned_memory(Bc_PackratParser *p);

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

#define BC_PACKRAT_LR                                                          \
    (Bc_PackratRuleResult) {                                                   \
        .result = RC_PACKRAT_RESULT_LEFT_RECURSION,                            \
        .left_recursion_detected = false                                       \
    }

#define BC_PACKRAT_SUCCESS(ast_node)                                           \
    (Bc_PackratRuleResult) {                                                   \
        .result = RC_PACKRAT_RESULT_SUCCESS,                                   \
        .success = (Bc_PackratRuleSuccess) {                                   \
            .node = ast_node, .position = bc_packrat_mark(p)                   \
        }                                                                      \
    }