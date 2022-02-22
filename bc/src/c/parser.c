#include "parser.h"

#include <stdlib.h>

#include "ast.h"
#include "lexer.h"
#include "utils/hash_map.h"

void *handle_cache(expect_func func, Bc_Parser *p, FuncId *func_id) {
    Bc_CacheEntry *cached_value =
        bc_memo_cache_get_value(&p->cache, p->ts->cursor, func_id);
    if (cached_value) {
        bc_tscanner_reset(p->ts, cached_value->end_pos);
        return cached_value->value;
    }
    return NULL;
}

void *handle_left_recursion(expect_func func, Bc_Parser *p, FuncId *func_id) {
    size_t pos = bc_tscanner_mark(p->ts);
    Bc_CacheEntry *cached_value =
        bc_memo_cache_get_value(&p->cache, pos, func_id);
    if (cached_value) {
        bc_tscanner_reset(p->ts, cached_value->end_pos);
        return cached_value->value;
    }

    store_in_cache(&p->cache, pos, func_id,
                   NULL); // skip left recursive alternatives
    size_t last_pos = pos;
    void *last_result = NULL;
    for (;;) {
        void *result = func(p);
        size_t new_pos = bc_tscanner_mark(p->ts);

        if (new_pos <= last_pos) { // failed to advance
            break;
        }
        store_in_cache(&p->cache, last_pos, func_id, result);
        last_pos = new_pos;
        last_result = result;
    }

    bc_tscanner_reset(p->ts, last_pos);
    return last_result;
}