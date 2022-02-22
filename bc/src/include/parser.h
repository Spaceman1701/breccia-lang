#pragma once

#include <stdlib.h>

#include "ast.h"
#include "lexer.h"
#include "parser_cache.h"
#include "utils/hash_map.h"

typedef struct {
    Bc_MemoizationCache cache;
    TokenScanner *ts;
} Bc_Parser;

Bc_Token *bc_expect(TokenScanner *ts, Bc_TokenType);

Bc_Decl *bc_expect_decl(TokenScanner *ts);
Bc_Expr *bc_expect_expr(TokenScanner *ts);
Bc_Stmt *bc_expect_stmt(Bc_Parser *p);

typedef void *(expect_func)(Bc_Parser *p);

void *handle_cache(expect_func func, Bc_Parser *p, FuncId *func_id);

void *handle_left_recursion(expect_func func, Bc_Parser *p, FuncId *func_id);