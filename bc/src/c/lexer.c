#include "lexer.h"

#include "bc.h"

#include "utils/list.h"

int init_lexer(LexerState *lexer, const char *input_string) {
    lexer->input_string = input_string;
    return bc_list_new(sizeof(Bc_Token), 8, &lexer->token_list);
}

int bc_lexer_append_token(LexerState *lexer, Bc_Token token) {
    Bc_Token *append = bc_list_append_ptr(&lexer->token_list);
    if (!append) {
        return -1;
    }
    (*append) = token;
    return 0;
}

void bc_lexer_print_all_tokens(LexerState *lexer) {
    for (size_t i = 0; i < lexer->token_list.length; i++) {
        Bc_Token *token = bc_list_get(&lexer->token_list, i);
        bc_token_print(token, stdout);
        printf(" ");
        bc_token_print_location(token, stdout);
        printf("\n");
    }
    printf("\n");
}

void bc_token_print(Bc_Token *tk, FILE *stream) {
    fprintf(stream, "%.*s", (int)tk->length, tk->text_ptr);
}

void bc_token_print_location(Bc_Token *tk, FILE *stream) {
    fprintf(stream, "{line: %zu, column: %zu} (%zu)", tk->line, tk->column,
            tk->position);
}

void bc_token_copy_text(Bc_Token *tk, char *buffer) {
    for (size_t i = 0; i < tk->length; i++) {
        buffer[i] = tk->text_ptr[i];
    }
    buffer[tk->length] = 0;
}

char *bc_token_alloc_copy_text(Bc_Token *tk) {
    char *out_buf = malloc(tk->length + 1);
    bc_token_copy_text(tk, out_buf);
    return out_buf;
}

Bc_Token *bc_tscanner_peek(const TokenScanner *ts) {
    return bc_list_get(&ts->lexer.token_list, ts->cursor);
}

Bc_Token *bc_tscanner_next(TokenScanner *ts) {
    Bc_Token *tk = bc_tscanner_peek(ts);
    ts->cursor++;
    return tk;
}

size_t bc_tscanner_mark(const TokenScanner *ts) { return ts->cursor; }

void bc_tscanner_reset(TokenScanner *ts, size_t pos) { ts->cursor = pos; }