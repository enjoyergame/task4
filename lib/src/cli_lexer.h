/*
анализ аргв

Кучуков Ридаль Радикович
МК-101
*/

#ifndef PASSGEN_CLI_LEXER_H
#define PASSGEN_CLI_LEXER_H

#include <stddef.h>
#include "passgen/error.h"

// имя опции (с '-') и её значение
// value может быть NULL, если опция без значения
typedef struct
{
    char *key;
    char *value;
} Token;

// Список токенов
typedef struct
{
    Token *items;
    size_t count;
} TokenList;

PassgenError cli_lexer_tokenize(int argc, char *const argv[], TokenList *out_list);

void token_list_free(TokenList *list);

#endif // PASSGEN_CLI_LEXER_H