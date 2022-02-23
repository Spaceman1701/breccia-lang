#include "packrat.h"

#include <stdbool.h>
#include <stdio.h>

size_t bc_packrat_mark(Bc_PackratParser *p) { return p->ts->cursor; }
void bc_packrat_reset(Bc_PackratParser *p, size_t pos) { p->ts->cursor = pos; }

bool bc_rule_succees(Bc_PackratRuleResult result) {
    return result.result == RC_PACKRAT_RESULT_SUCCESS;
}

Bc_Token *bc_expect_tk(Bc_PackratParser *p, Bc_TokenType ty) {
    Bc_Token *t = bc_list_get(&p->ts->lexer.token_list, p->ts->cursor);
    if (t) {
        p->ts->cursor += 1;
        return t;
    }
    return NULL;
}

Bc_AstNode bc_expect_rule(PackratRuleFunc rule, Bc_PackratParser *p) {
    Bc_PackratCacheKey key = {
        .function = rule,
        .location = p->ts->cursor,
    };
    Bc_PackratRuleResult *cached_value;
    if ((cached_value = bc_packrat_cache_get(&p->cache, key))) {
        if (cached_value->result) {
            return cached_value->success.node;
        }
    }

    Bc_PackratRuleResult result = rule(p);
    bc_packrat_cache_put(&p->cache, key, result);

    if (result.result) {
        return result.success.node;
    }

    return NULL;
}