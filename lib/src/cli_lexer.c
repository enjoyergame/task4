/*
реализация анализа аргв

Кучуков Ридаль Радикович
МК-101
*/

#include "cli_lexer.h"

#include <stdlib.h>
#include <string.h>

// таблица известных префиксов опций статическая память
static const char *KNOWN_OPTIONS[] = {
    "-minl", "-maxl", "-n", "-c", "-a", "-C",
    "-d", "-D", "-p",
    "-s", "-i"};
static const size_t KNOWN_OPTIONS_COUNT =
    sizeof(KNOWN_OPTIONS) / sizeof(KNOWN_OPTIONS[0]);

#define MAX_SEPARATORS 16
typedef struct
{
    char chars[MAX_SEPARATORS]; // символы разделители
    size_t count;               // сколько их
} SeparatorSet;

static void separator_set_init_default(SeparatorSet *set) // выставляем дефолтные разделители
{
    set->chars[0] = '=';
    set->chars[1] = ':';
    set->count = 2;
}

static int separator_set_contains(const SeparatorSet *set, char c) // проверка символа на разделитель
{
    for (size_t i = 0; i < set->count; i++) // бежим пока не кончаться известные разделители
    {
        if (set->chars[i] == c) // разделитель
        {
            return 1;
        }
    }
    return 0; // нет
}

static void separator_set_add(SeparatorSet *set, char c) // добавление разделителя
{
    if (separator_set_contains(set, c)) // уже есть
    {
        return;
    }
    if (set->count < MAX_SEPARATORS)
    {
        set->chars[set->count++] = c; // добавляем в разделители
    }
}

static void separator_set_replace(SeparatorSet *set, const char *value)
{
    set->count = 0; // устанавливаем в 0 что бы перезаписать все старые
    for (const char *c = value; *c != '\0'; c++)
    {
        separator_set_add(set, *c);
    }
}

static const char *match_known_prefix(const char *token, size_t *out_len) // функция что бы узнать какой это префикс и префикс ли вообще
{
    const char *best = NULL;
    size_t best_len = 0;

    for (size_t i = 0; i < KNOWN_OPTIONS_COUNT; i++) // бежим по префиксам
    {
        size_t len = strlen(KNOWN_OPTIONS[i]);
        if (strncmp(token, KNOWN_OPTIONS[i], len) == 0 && len > best_len) // если совпадает лучше всего из всех что были до этого
        {//пишем
            best = KNOWN_OPTIONS[i];
            best_len = len;
        }
    }

    if (best != NULL)
    {
        *out_len = best_len;
    }
    return best;
}

static char *str_dup(const char *s)
{
    size_t len = strlen(s);
    char *copy = malloc(len + 1);
    if (copy == NULL)
    {
        return NULL;
    }
    memcpy(copy, s, len + 1);
    return copy;
}

static PassgenError token_list_push(TokenList *list, size_t *capacity,
                                    char *key, char *value)
{
    if (list->count == *capacity) // проверка есть ли место (либо емкость равна 0 при первом запуске)
    {
        size_t new_capacity = (*capacity == 0) ? 8 : (*capacity * 2);
        Token *new_items = realloc(list->items, new_capacity * sizeof(Token)); // перевыделяем память
        if (new_items == NULL)
        {
            return PASSGEN_ERR_NO_MEMORY;
        }
        list->items = new_items;
        *capacity = new_capacity;
    }
    // пишем ключ значение
    list->items[list->count].key = key;
    list->items[list->count].value = value;
    list->count++;
    return PASSGEN_OK;
}

void token_list_free(TokenList *list)
{
    if (list->items == NULL)
    {
        return;
    }
    for (size_t i = 0; i < list->count; i++)
    {
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

    if (remainder[0] == '\0') // если хвост аргумента пустой
    {
        if (i + 1 >= argc) // вышли за аргументы
        {
            return 0;
        }
        const char *next = argv[i + 1]; // берем аргумент (если -n 10 то поличили /0 перешли к 10)

        if (next[0] == '-') // некст аргумент опция
        {
            return 0;
        }

        *out_value = str_dup(next); // берем значение
        if (*out_value == NULL)     // не выделилась память
        {
            *out_err = PASSGEN_ERR_NO_MEMORY;
        }
        return 1;
    }

    const char *value_start = separator_set_contains(seps, remainder[0]) // если передали слитно или через разделитель
                                  ? remainder + 1                        // если да то сдвигаемся на 1 до значения
                                  : remainder;                           // слитно
    *out_value = str_dup(value_start);                                   // пишем значение
    if (*out_value == NULL)                                              // не выделилась память
    {
        *out_err = PASSGEN_ERR_NO_MEMORY;
    }
    return 0;
}

static void apply_separator_side_effect(const char *prefix, const char *value,
                                        SeparatorSet *seps)
{
    if (value == NULL)
    {
        return;
    }
    if (strcmp(prefix, "-d") == 0)
    {
        for (const char *c = value; *c != '\0'; c++) // добавляем все разделители что нам передали
        {
            separator_set_add(seps, *c);
        }
    }
    else if (strcmp(prefix, "-D") == 0)
    {
        separator_set_replace(seps, value); // удаляем старые добавляем только те что передали
    }
}

PassgenError cli_lexer_tokenize(int argc, char *const argv[], TokenList *out_list)
{
    out_list->items = NULL;
    out_list->count = 0;
    size_t capacity = 0;

    SeparatorSet seps;
    separator_set_init_default(&seps); // структурка с символами разделителями (по дефолту = и :)

    for (int i = 1; i < argc; i++)
    {
        const char *arg = argv[i];

        if (arg[0] != '-' || arg[1] == '\0')
        { // не начинается с минуса или ток минус
            continue;
        }

        size_t prefix_len = 0;                                     // длина опции
        const char *prefix = match_known_prefix(arg, &prefix_len); // ищем среди возможных префиксов
        if (prefix == NULL)                                        // неизвестная опция
        {
            continue;
        }
        // выставляем дефолт значения
        char *value = NULL;
        PassgenError extract_err = PASSGEN_OK;
        // consumed_next сюда кладем 1 если получили значение из следующего аргмента 0 если было передано слитно или через разделитель
        int consumed_next = extract_value(arg + prefix_len, argc, argv, i,
                                          &seps, &value, &extract_err); // arg + prefix_len что бы получить чиселку
        if (extract_err != PASSGEN_OK)
        { // чистим память возвращаем ошибку если чето не так
            token_list_free(out_list);
            return extract_err;
        }

        char *key = str_dup(prefix); // пишем опцию
        if (key == NULL)             // проверка на память
        {
            free(value);
            token_list_free(out_list);
            return PASSGEN_ERR_NO_MEMORY;
        }

        PassgenError push_err = token_list_push(out_list, &capacity, key, value); // пишем пары в список
        if (push_err != PASSGEN_OK)                                               // вернули ошибку
        {
            free(key);
            free(value);
            token_list_free(out_list);
            return push_err;
        }

        apply_separator_side_effect(prefix, value, &seps); // если -d -D

        if (consumed_next)
        {
            i++;
        }
    }

    return PASSGEN_OK;
}