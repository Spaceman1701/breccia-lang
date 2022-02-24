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

Bc_PackratRuleResult bc_grow_left_recursion(size_t pos, PackratRuleFunc rule,
                                            Bc_PackratParser *p) {
    printf("growing seed: %zu %p\n", pos, rule);
    Bc_PackratRuleResult last_result = BC_PACKRAT_FAILURE;
    size_t last_result_pos = pos;
    Bc_PackratCacheKey key = {
        .function = rule,
        .location = pos,
    };
    while (true) {
        bc_packrat_chache_print_value(&p->cache, key);
        bc_packrat_reset(p, pos);
        Bc_PackratRuleResult new_result = rule(p);
        if (!new_result.result ||
            new_result.success.position <= last_result_pos) {
            printf("breaking left recursion loop (%p)\n", rule);
            break; // failed recursion
        }
        printf("new position %zu\n", new_result.success.position);
        last_result_pos = new_result.success.position;
        last_result = new_result;
        bc_packrat_cache_put(&p->cache, key, new_result);
    }

    if (last_result.result) {
        printf("grew successfully and returned value\n");
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
    printf("read cache for %p: %p\n", rule, cached_value);
    if (!cached_value) {
        bc_packrat_cache_put(&p->cache, key, BC_PACKRAT_LR);
        bc_packrat_chache_print_value(&p->cache, key);
        Bc_PackratRuleResult new_result = rule(p);
        printf("seeded value:");
        bc_packrat_chache_print_value(&p->cache, key);
        Bc_PackratRuleResult updated_cache =
            *bc_packrat_cache_get(&p->cache, key);

        bc_packrat_cache_put(&p->cache, key, new_result);

        bool lr_found =
            (updated_cache.result == RC_PACKRAT_RESULT_LEFT_RECURSION) &&
            (updated_cache.left_recursion_detected = true);
        if (new_result.result && lr_found) {
            new_result = bc_grow_left_recursion(pos, rule, p);
            return bc_expect_return(&new_result, p);
        } else {
            return bc_expect_return(&new_result, p);
        }
    } else {
        if (cached_value->result == RC_PACKRAT_RESULT_LEFT_RECURSION) {
            cached_value->left_recursion_detected = true;
            // printf("lr detected %p\n", rule);
            return NULL;
        }
        return bc_expect_return(cached_value, p);
    }
}