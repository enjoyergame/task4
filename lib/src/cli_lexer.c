/*
реализация анализа аргв

Кучуков Ридаль Радикович
МК-101
*/

#include "cli_lexer.h"

#include <stdlib.h>
#include <string.h>

// Таблица известных префиксов опций.
static const char *KNOWN_OPTIONS[] = {
    "-minl", "-maxl", "-n", "-c", "-a", "-C",
    "-d", "-D", "-p",
    "-s", "-i"
};
static const size_t KNOWN_OPTIONS_COUNT =
    sizeof(KNOWN_OPTIONS) / sizeof(KNOWN_OPTIONS[0]);

#define MAX_SEPARATORS 16
typedef struct {
    char chars[MAX_SEPARATORS];
    size_t count;
} SeparatorSet;

static void separator_set_init_default(SeparatorSet *set)
{
    set->chars[0] = '=';
    set->chars[1] = ':';
    set->count = 2;
}

static int separator_set_contains(const SeparatorSet *set, char c)
{
    for (size_t i = 0; i < set->count; i++) {
        if (set->chars[i] == c) {
            return 1;
        }
    }
    return 0;
}

static void separator_set_add(SeparatorSet *set, char c)
{
    if (separator_set_contains(set, c)) {
        return;
    }
    if (set->count < MAX_SEPARATORS) {
        set->chars[set->count++] = c;
    }
}

static void separator_set_replace(SeparatorSet *set, const char *value)
{
    set->count = 0;
    for (const char *c = value; *c != '\0'; c++) {
        separator_set_add(set, *c);
    }
}

static const char *match_known_prefix(const char *token, size_t *out_len)
{
    const char *best = NULL;
    size_t best_len = 0;

    for (size_t i = 0; i < KNOWN_OPTIONS_COUNT; i++) {
        size_t len = strlen(KNOWN_OPTIONS[i]);
        if (strncmp(token, KNOWN_OPTIONS[i], len) == 0 && len > best_len) {
            best = KNOWN_OPTIONS[i];
            best_len = len;
        }
    }

    if (best != NULL) {
        *out_len = best_len;
    }
    return best;
}

static char *str_dup(const char *s)
{
    size_t len = strlen(s);
    char *copy = malloc(len + 1);
    if (copy == NULL) {
        return NULL;
    }
    memcpy(copy, s, len + 1);
    return copy;
}

static PassgenError token_list_push(TokenList *list, size_t *capacity,
                                     char *key, char *value)
{
    if (list->count == *capacity) {
        size_t new_capacity = (*capacity == 0) ? 8 : (*capacity * 2);
        Token *new_items = realloc(list->items, new_capacity * sizeof(Token));
        if (new_items == NULL) {
            return PASSGEN_ERR_NO_MEMORY;
        }
        list->items = new_items;
        *capacity = new_capacity;
    }

    list->items[list->count].key = key;
    list->items[list->count].value = value;
    list->count++;
    return PASSGEN_OK;
}

void token_list_free(TokenList *list)
{
    if (list->items == NULL) {
        return;
    }
    for (size_t i = 0; i < list->count; i++) {
        free(list->items[i].key);
        free(list->items[i].value);
    }
    free(list->items);
    list->items = NULL;
    list->count = 0;
}

static int extract_value(const char *remainder, int argc, char *const argv[],
                          int i, const SeparatorSet *seps,
                          char **out_value, PassgenError *out_err)
{
    *out_value = NULL;
    *out_err = PASSGEN_OK;

    if (remainder[0] == '\0') {
        if (i + 1 >= argc) {
            return 0;
        }
        const char *next = argv[i + 1];
        
        // Исправление по ревью: если начинается с '-', это опция, а не значение
        if (next[0] == '-') {
            return 0;
        }
        
        *out_value = str_dup(next);
        if (*out_value == NULL) {
            *out_err = PASSGEN_ERR_NO_MEMORY;
        }
        return 1;
    }

    const char *value_start = separator_set_contains(seps, remainder[0])
                                   ? remainder + 1
                                   : remainder;
    *out_value = str_dup(value_start);
    if (*out_value == NULL) {
        *out_err = PASSGEN_ERR_NO_MEMORY;
    }
    return 0;
}

static void apply_separator_side_effect(const char *prefix, const char *value,
                                         SeparatorSet *seps)
{
    if (value == NULL) {
        return;
    }
    if (strcmp(prefix, "-d") == 0) {
        for (const char *c = value; *c != '\0'; c++) {
            separator_set_add(seps, *c);
        }
    } else if (strcmp(prefix, "-D") == 0) {
        separator_set_replace(seps, value);
    }
}

PassgenError cli_lexer_tokenize(int argc, char *const argv[], TokenList *out_list)
{
    out_list->items = NULL;
    out_list->count = 0;
    size_t capacity = 0;

    SeparatorSet seps;
    separator_set_init_default(&seps);

    for (int i = 1; i < argc; i++) {
        const char *arg = argv[i];

        if (arg[0] != '-' || arg[1] == '\0') {
            continue;
        }

        size_t prefix_len = 0;
        const char *prefix = match_known_prefix(arg, &prefix_len);
        if (prefix == NULL) {
            continue;
        }

        char *value = NULL;
        PassgenError extract_err = PASSGEN_OK;
        int consumed_next = extract_value(arg + prefix_len, argc, argv, i,
                                           &seps, &value, &extract_err);
        if (extract_err != PASSGEN_OK) {
            token_list_free(out_list);
            return extract_err;
        }

        char *key = str_dup(prefix);
        if (key == NULL) {
            free(value);
            token_list_free(out_list);
            return PASSGEN_ERR_NO_MEMORY;
        }

        PassgenError push_err = token_list_push(out_list, &capacity, key, value);
        if (push_err != PASSGEN_OK) {
            free(key);
            free(value);
            token_list_free(out_list);
            return push_err;
        }

        apply_separator_side_effect(prefix, value, &seps);

        if (consumed_next) {
            i++;
        }
    }

    return PASSGEN_OK;
}