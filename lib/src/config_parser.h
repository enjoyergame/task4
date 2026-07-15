/*
конфиг

Кучуков Ридаль Радикович
МК-101
*/
#ifndef PASSGEN_CONFIG_PARSER_H
#define PASSGEN_CONFIG_PARSER_H

#include "passgen/error.h"
#include "cli_lexer.h"
#include "charset.h"
#include <stdbool.h>

#define CONFIG_NOT_SET -1

typedef struct
{
    int minl;
    int maxl;
    int n;
    int c;

    CharsetInfo charset;

    bool has_s;
    bool has_i;
    bool has_a;
    bool has_C;
} AppConfig;

void config_init(AppConfig *config);
void config_free(AppConfig *config);

// Парсит список токенов в конфигурацию и проверяет все логические конфликты
PassgenError config_parse(AppConfig *config, const TokenList *tokens);

#endif // PASSGEN_CONFIG_PARSER_H