/*
реализация таблицы вероятностей и распределения

Кучуков Ридаль Радикович
МК-101
*/

#include "prob_math.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>

#define PROB_EPSILON 1e-5

static double parse_simple_double(const char *str, char **endptr) {
    double result = 0.0;
    double fraction = 1.0;
    int has_digits = 0;

    while (isspace((unsigned char)*str)) str++;

    while (isdigit((unsigned char)*str)) {
        result = result * 10.0 + (*str - '0');
        str++;
        has_digits = 1;
    }

    if (*str == '.' || *str == ',') {
        str++;
        while (isdigit((unsigned char)*str)) {
            fraction /= 10.0;
            result += (*str - '0') * fraction;
            str++;
            has_digits = 1;
        }
    }

    while (isspace((unsigned char)*str)) str++;

    if (endptr) *endptr = (char *)str;
    
    return has_digits ? result : -1.0; 
}

static PassgenError parse_prob_value(const char *value, char *out_sym, double *out_prob) {
    if (value == NULL || value[0] == '\0') {
        return PASSGEN_ERR_MISSING_VALUE;
    }

    *out_sym = value[0];
    const char *pstr = value + 1;

    if (*pstr == '=' || *pstr == ':') {
        pstr++;
    }

    if (*pstr == '\0') {
        return PASSGEN_ERR_NOT_A_NUMBER;
    }

    char *endptr = NULL;
    double val = parse_simple_double(pstr, &endptr);

    if (*endptr != '\0' || val < 0.0) {
        return PASSGEN_ERR_NOT_A_NUMBER;
    }

    if (val > 1.0) {
        return PASSGEN_ERR_PROB_INVALID;
    }

    *out_prob = val;
    return PASSGEN_OK;
}

void prob_table_init(ProbTable *pt) {
    if (pt == NULL) return;
    pt->items = NULL;
    pt->count = 0;
}

void prob_table_free(ProbTable *pt) {
    if (pt == NULL) return;
    if (pt->items != NULL) {
        free(pt->items);
        pt->items = NULL;
    }
    pt->count = 0;
}

static const struct {
    int flag;
    char sym;
} GROUP_MAP[] = {
    { CHARSET_GROUP_LOWER, 'a' },
    { CHARSET_GROUP_UPPER, 'A' },
    { CHARSET_GROUP_DIGITS, 'D' },
    { CHARSET_GROUP_SPECIAL, 'S' }
};
static const size_t GROUP_MAP_SIZE = sizeof(GROUP_MAP) / sizeof(GROUP_MAP[0]);

static PassgenError populate_symbols(ProbTable *pt, const AppConfig *config) {
    size_t count = 0;

    if (config->has_C) {
        for (size_t i = 0; i < GROUP_MAP_SIZE; i++) {
            if (config->charset.groups & GROUP_MAP[i].flag) count++;
        }
    } else if (config->has_a) {
        count = config->charset.custom_len;
    }

    if (count == 0) {
        pt->count = 0;
        return PASSGEN_OK;
    }

    pt->items = (ProbItem *)calloc(count, sizeof(ProbItem));
    if (pt->items == NULL) {
        return PASSGEN_ERR_NO_MEMORY;
    }
    pt->count = count;

    size_t idx = 0;
    if (config->has_C) {
        for (size_t i = 0; i < GROUP_MAP_SIZE; i++) {
            if (config->charset.groups & GROUP_MAP[i].flag) {
                pt->items[idx].symbol = GROUP_MAP[i].sym;
                pt->items[idx].prob = -1.0;
                idx++;
            }
        }
    } else if (config->has_a) {
        for (size_t i = 0; i < config->charset.custom_len; i++) {
            pt->items[idx].symbol = config->charset.custom[i];
            pt->items[idx].prob = -1.0;
            idx++;
        }
    }

    return PASSGEN_OK;
}

PassgenError prob_table_build(ProbTable *pt, const AppConfig *config, const TokenList *tokens) {
    if (pt == NULL || config == NULL || tokens == NULL) return PASSGEN_ERR_UNKNOWN;

    prob_table_free(pt);

    PassgenError err = populate_symbols(pt, config);
    if (err != PASSGEN_OK) goto cleanup;
    
    if (pt->count == 0) return PASSGEN_OK;

    // Парсим токены -p
    for (size_t i = 0; i < tokens->count; i++) {
        if (strcmp(tokens->items[i].key, "-p") != 0) continue;

        char sym;
        double prob;
        if ((err = parse_prob_value(tokens->items[i].value, &sym, &prob)) != PASSGEN_OK) {
            goto cleanup;
        }

        bool found = false;
        for (size_t j = 0; j < pt->count; j++) {
            if (pt->items[j].symbol == sym) {
                if (pt->items[j].prob >= 0.0) {
                    err = PASSGEN_ERR_DUPLICATE_OPTION;
                    goto cleanup;
                }
                pt->items[j].prob = prob;
                found = true;
                break;
            }
        }
        
        if (!found) {
            err = PASSGEN_ERR_PROB_INVALID; 
            goto cleanup;
        }
    }

    double sum = 0.0;
    size_t unassigned = 0;

    for (size_t i = 0; i < pt->count; i++) {
        if (pt->items[i].prob >= 0.0) {
            sum += pt->items[i].prob;
        } else {
            unassigned++;
        }
    }

    if (sum > 1.0 + PROB_EPSILON) {
        err = PASSGEN_ERR_PROB_OVERFLOW;
        goto cleanup;
    }

    if (unassigned == 0 && fabs(1.0 - sum) > PROB_EPSILON) {
        err = PASSGEN_ERR_PROB_INVALID;
        goto cleanup;
    }

    if (unassigned > 0) {
        double remainder = fmax(0.0, 1.0 - sum);
        double even_prob = remainder / unassigned;
        for (size_t i = 0; i < pt->count; i++) {
            if (pt->items[i].prob < 0.0) {
                pt->items[i].prob = even_prob;
            }
        }
    }

    double current_cdf = 0.0;
    for (size_t i = 0; i < pt->count; i++) {
        current_cdf += pt->items[i].prob;
        pt->items[i].cdf = current_cdf;
    }
    pt->items[pt->count - 1].cdf = 1.0;

    return PASSGEN_OK;

cleanup:
    prob_table_free(pt);
    return err;
}