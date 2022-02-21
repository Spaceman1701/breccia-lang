#include "lexer.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

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
