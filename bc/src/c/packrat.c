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

Bc_AstNode bc_expect_return(Bc_PackratRuleResult *res, Bc_PackratParser *p) {
    if (res->result) {
        bc_packrat_reset(p, res->success.position);
        return res->success.node;
    }
    printf("found cache failure\n");
    return NULL;
}

Bc_PackratRuleResult bc_grow_left_recursion(Bc_PackratCacheKey key, size_t pos,
                                            PackratRuleFunc rule,
                                            Bc_PackratParser *p) {
    printf("growing seed: %p\n", rule);
    Bc_PackratRuleResult last_result = BC_PACKRAT_FAILURE;
    size_t last_result_pos = pos;
    while (true) {
        bc_packrat_reset(p, last_result_pos);
        last_result = rule(p);
        if (!last_result.result ||
            last_result.success.position <= last_result_pos) {
            printf("breaking left recursion loop (%p)\n", rule);
            break; // failed recursion
        }
        bc_packrat_cache_put(&p->cache, key, last_result);
    }

    return last_result;
}

Bc_AstNode bc_expect_rule(PackratRuleFunc rule, Bc_PackratParser *p) {
    printf("entering expect %p at pos %zu\n", rule, p->ts->cursor);
    size_t pos = bc_packrat_mark(p);
    Bc_PackratCacheKey key = {
        .function = rule,
        .location = p->ts->cursor,
    };
    Bc_PackratRuleResult *cached_value = bc_packrat_cache_get(&p->cache, key);
    if (!cached_value) {
        bc_packrat_cache_put(&p->cache, key, BC_PACKRAT_LR);
        Bc_PackratRuleResult new_result = rule(p);
        bc_packrat_cache_put(&p->cache, key, new_result);
        if (new_result.result == RC_PACKRAT_RESULT_LEFT_RECURSION &&
            new_result.left_recursion_detected) {
            new_result = bc_grow_left_recursion(key, pos, rule, p);
            return bc_expect_return(&new_result, p);
        } else {
            return bc_expect_return(&new_result, p);
        }

        Bc_PackratRuleResult result = bc_grow_left_recursion(key, pos, rule, p);
        return bc_expect_return(&result, p);
    } else {
        if (cached_value->result == RC_PACKRAT_RESULT_LEFT_RECURSION) {
            cached_value->left_recursion_detected = true;
            printf("lr detected\n");
            return NULL;
        }
        return bc_expect_return(cached_value, p);
    }
}