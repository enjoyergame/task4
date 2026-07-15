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

static PassgenError parse_positive_int(const char *value, int *out_val)
{
    if (value == NULL || *value == '\0')
    {
        return PASSGEN_ERR_MISSING_VALUE;
    }

    char *endptr = NULL;
    long val = strtol(value, &endptr, 10);

    // Число должно состоять только из цифр и быть строго > 0
    if (*endptr != '\0' || val <= 0)
    {
        return PASSGEN_ERR_NOT_A_NUMBER;
    }

    *out_val = (int)val;
    return PASSGEN_OK;
}

PassgenError config_parse(AppConfig *config, const TokenList *tokens) {
    if (config == NULL || tokens == NULL) {
        return PASSGEN_ERR_UNKNOWN;
    }

    PassgenError err = PASSGEN_OK;

    for (size_t i = 0; i < tokens->count; i++) {
        const Token *t = &tokens->items[i];

        if (strcmp(t->key, "-minl") == 0) {
            if (config->minl != CONFIG_NOT_SET) {
                err = PASSGEN_ERR_DUPLICATE_OPTION;
                goto cleanup_on_error;
            }
            if ((err = parse_positive_int(t->value, &config->minl)) != PASSGEN_OK) {
                goto cleanup_on_error;
            }
        }
        else if (strcmp(t->key, "-maxl") == 0) {
            if (config->maxl != CONFIG_NOT_SET) {
                err = PASSGEN_ERR_DUPLICATE_OPTION;
                goto cleanup_on_error;
            }
            if ((err = parse_positive_int(t->value, &config->maxl)) != PASSGEN_OK) {
                goto cleanup_on_error;
            }
        }
        else if (strcmp(t->key, "-n") == 0) {
            if (config->n != CONFIG_NOT_SET) {
                err = PASSGEN_ERR_DUPLICATE_OPTION;
                goto cleanup_on_error;
            }
            if ((err = parse_positive_int(t->value, &config->n)) != PASSGEN_OK) {
                goto cleanup_on_error;
            }
        }
        else if (strcmp(t->key, "-c") == 0) {
            if (config->c != CONFIG_NOT_SET) {
                err = PASSGEN_ERR_DUPLICATE_OPTION;
                goto cleanup_on_error;
            }
            if ((err = parse_positive_int(t->value, &config->c)) != PASSGEN_OK) {
                goto cleanup_on_error;
            }
        }
        else if (strcmp(t->key, "-C") == 0) {
            if (config->has_C) {
                err = PASSGEN_ERR_DUPLICATE_OPTION;
                goto cleanup_on_error;
            }
            config->has_C = true;
            if ((err = charset_parse_groups(&config->charset, t->value)) != PASSGEN_OK) {
                goto cleanup_on_error;
            }
        }
        else if (strcmp(t->key, "-a") == 0) {
            if (config->has_a) {
                err = PASSGEN_ERR_DUPLICATE_OPTION;
                goto cleanup_on_error;
            }
            config->has_a = true;
            if ((err = charset_parse_custom(&config->charset, t->value)) != PASSGEN_OK) {
                goto cleanup_on_error;
            }
        }
        else if (strcmp(t->key, "-s") == 0) {
            if (config->has_s) {
                err = PASSGEN_ERR_DUPLICATE_OPTION;
                goto cleanup_on_error;
            }
            config->has_s = true;
        }
        else if (strcmp(t->key, "-i") == 0) {
            if (config->has_i) {
                err = PASSGEN_ERR_DUPLICATE_OPTION;
                goto cleanup_on_error;
            }
            config->has_i = true;
        }
        else if (strcmp(t->key, "-d") == 0 || strcmp(t->key, "-D") == 0) {
        }
        else if (strcmp(t->key, "-p") == 0) {
        }
    }

    if (config->n != CONFIG_NOT_SET && (config->minl != CONFIG_NOT_SET || config->maxl != CONFIG_NOT_SET)) {
        err = PASSGEN_ERR_CONFLICT_N_MINMAX;
        goto cleanup_on_error;
    }

    if (config->has_a && config->has_C) {
        err = PASSGEN_ERR_CONFLICT_A_C;
        goto cleanup_on_error;
    }

    bool has_pair = (config->has_s || config->has_i);
    if (has_pair && config->n != CONFIG_NOT_SET) {
        err = PASSGEN_ERR_CONFLICT_PAIR_N;
        goto cleanup_on_error;
    }
    if ((config->has_s && !config->has_i) || (!config->has_s && config->has_i)) {
        err = PASSGEN_ERR_INCOMPLETE_PAIR;
        goto cleanup_on_error;
    }

    if ((config->minl != CONFIG_NOT_SET && config->maxl == CONFIG_NOT_SET) ||
        (config->minl == CONFIG_NOT_SET && config->maxl != CONFIG_NOT_SET)) {
        err = PASSGEN_ERR_INCOMPLETE_RANGE;
        goto cleanup_on_error;
    }

    if (config->minl != CONFIG_NOT_SET && config->maxl != CONFIG_NOT_SET) {
        if (config->minl > config->maxl) {
            err = PASSGEN_ERR_INCOMPLETE_RANGE;
            goto cleanup_on_error;
        }
    }

    if (config->c == CONFIG_NOT_SET) {
        config->c = 1;
    }

    return PASSGEN_OK;

cleanup_on_error:
    config_free(config);
    return err;
}