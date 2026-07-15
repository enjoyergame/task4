/*
парсим конфиг

Кучуков Ридаль Радикович
МК-101
*/
#include "config_parser.h"
#include <stdlib.h>
#include <string.h>

void config_init(AppConfig *config)
{
    if (config == NULL)
        return;

    config->minl = CONFIG_NOT_SET;
    config->maxl = CONFIG_NOT_SET;
    config->n = CONFIG_NOT_SET;
    config->c = CONFIG_NOT_SET;

    charset_init(&config->charset);

    config->has_s = false;
    config->has_i = false;
    config->has_a = false;
    config->has_C = false;
}

void config_free(AppConfig *config)
{
    if (config == NULL)
        return;
    charset_free(&config->charset);
}
