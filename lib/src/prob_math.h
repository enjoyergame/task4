/*
модуль математики вероятностей

Кучуков Ридаль Радикович
МК-101
*/

#ifndef PASSGEN_PROB_MATH_H
#define PASSGEN_PROB_MATH_H

#include "passgen/error.h"
#include "config_parser.h"
#include "cli_lexer.h"
#include <stddef.h>

typedef struct
{
    char symbol; // символ (из пользовательского алфавита или группа a/A/D/S)
    double prob; // заданная или вычисленная вероятность [0.0, 1.0]
    double cdf;  // кумулятивная вероятность (для алгоритма генерации)
} ProbItem;

typedef struct
{
    ProbItem *items;
    size_t count;
} ProbTable;//табличка с айтемами

void prob_table_init(ProbTable *pt);
void prob_table_free(ProbTable *pt);

// Сборка таблицы вероятностей на основе конфига и токенов -p
PassgenError prob_table_build(ProbTable *pt, const AppConfig *config, const TokenList *tokens);

#endif // PASSGEN_PROB_MATH_H