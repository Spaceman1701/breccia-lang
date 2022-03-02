#include "parser_rules.h"

#include <string.h>

#include "bc.h"

#include "ast.h"
#include "packrat.h"

bool expect_helper(void **out, PackratRuleFunc func, Bc_PackratParser *p) {
    size_t pos = bc_packrat_mark(p);
    bool success = ((*out) = bc_expect_rule(func, p));
    if (!success) {
        bc_packrat_reset(p, pos);
    }
    return success;
}

bool expect_token_helper(Bc_Token **out, Bc_TokenType type,
                         Bc_PackratParser *p) {
    size_t pos = bc_packrat_mark(p);
    bool success = ((*out) = bc_expect_tk(p, type));
    if (!success) {
        bc_packrat_reset(p, pos);
    }
    return success;
}
