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

static double parse_simple_double(const char *str, char **endptr)
{
    double result = 0.0;
    double fraction = 1.0;
    int has_digits = 0;

    while (isspace((unsigned char)*str))
        str++;

    while (isdigit((unsigned char)*str)) // если 12 result = 0.0 * 10.0 + (1) = 1.0 result = 1.0 * 10.0 +(2) = 12.0
    {
        result = result * 10.0 + (*str - '0');
        str++;
        has_digits = 1;
    }

    if (*str == '.' || *str == ',')
    {
        str++;
        while (isdigit((unsigned char)*str)) // тож самое но после точки делим на 10
        {
            fraction /= 10.0;
            result += (*str - '0') * fraction;
            str++;
            has_digits = 1;
        }
    }

    while (isspace((unsigned char)*str))
        str++;

    if (endptr)
        *endptr = (char *)str;

    return has_digits ? result : -1.0;
}
// получение значения из токена
static PassgenError parse_prob_value(const char *value, char *out_sym, double *out_prob)
{
    if (value == NULL || value[0] == '\0')
    {
        return PASSGEN_ERR_MISSING_VALUE;
    }

    *out_sym = value[0]; // сам символ
    const char *pstr = value + 1;

    // если символ после буквы не цифра и не точка/запятая то сдвигаемся
    if (!isdigit((unsigned char)*pstr) && *pstr != '.' && *pstr != ',')
    {
        pstr++;
    }

    if (*pstr == '\0')
    { // если нет числа
        return PASSGEN_ERR_NOT_A_NUMBER;
    }

    char *endptr = NULL;
    double val = parse_simple_double(pstr, &endptr);

    if (*endptr != '\0' || val < 0.0) // после значения должен быть пробел
    {
        return PASSGEN_ERR_NOT_A_NUMBER;
    }

    if (val > 1.0)
    {
        return PASSGEN_ERR_PROB_INVALID;
    }

    *out_prob = val;
    return PASSGEN_OK;
}
// инициализация
void prob_table_init(ProbTable *pt)
{
    if (pt == NULL)
        return;
    pt->items = NULL;
    pt->count = 0;
}
// очистка
void prob_table_free(ProbTable *pt)
{
    if (pt == NULL)
        return;
    if (pt->items != NULL)
    {
        free(pt->items);
        pt->items = NULL;
    }
    pt->count = 0;
}

static const struct
{
    int flag;
    char sym;
} GROUP_MAP[] = {
    {CHARSET_GROUP_LOWER, 'a'},
    {CHARSET_GROUP_UPPER, 'A'},
    {CHARSET_GROUP_DIGITS, 'D'},
    {CHARSET_GROUP_SPECIAL, 'S'}};
static const size_t GROUP_MAP_SIZE = sizeof(GROUP_MAP) / sizeof(GROUP_MAP[0]);
// функция что бы узнать сколько всего элементов будет в нашем алфавите и выделить под них память
static PassgenError populate_symbols(ProbTable *pt, const AppConfig *config)
{
    size_t count = 0;

    if (config->has_C) //-С бежим по группам
    {
        for (size_t i = 0; i < GROUP_MAP_SIZE; i++)
        {
            if (config->charset.groups & GROUP_MAP[i].flag) // проверяем стоит ли флажок на группу
                count++;
        }
    }
    else if (config->has_a)
    {
        count = config->charset.custom_len; //-а алфавит равен длине пользовательской строки
    }

    if (count == 0)
    {
        pt->count = 0;
        return PASSGEN_OK;
    }

    pt->items = (ProbItem *)calloc(count, sizeof(ProbItem));
    if (pt->items == NULL)
    {
        return PASSGEN_ERR_NO_MEMORY;
    }
    pt->count = count;

    size_t idx = 0;    // индекс по пт айтемс
    if (config->has_C) // группы
    {
        for (size_t i = 0; i < GROUP_MAP_SIZE; i++)
        {
            if (config->charset.groups & GROUP_MAP[i].flag)
            {
                pt->items[idx].symbol = GROUP_MAP[i].sym; // символ груп
                pt->items[idx].prob = -1.0;               // не задана вероятность
                idx++;
            }
        }
    }
    else if (config->has_a) // алфавит
    {
        for (size_t i = 0; i < config->charset.custom_len; i++)
        {
            pt->items[idx].symbol = config->charset.custom[i]; // символ
            pt->items[idx].prob = -1.0;                        // не проставлена вероятность
            idx++;
        }
    }

    return PASSGEN_OK;
}

PassgenError prob_table_build(ProbTable *pt, const AppConfig *config, const TokenList *tokens)
{
    if (pt == NULL || config == NULL || tokens == NULL)
        return PASSGEN_ERR_UNKNOWN;

    prob_table_free(pt);

    PassgenError err = populate_symbols(pt, config); // прошлись либо по алфавиту пользователя (мб дефолтному) либо по группам выделили память
    if (err != PASSGEN_OK)
        goto cleanup;

    if (pt->count == 0) // лив если пустой алфавит
        return PASSGEN_OK;

    for (size_t i = 0; i < tokens->count; i++)
    {
        if (strcmp(tokens->items[i].key, "-p") != 0) // все что не -p скипаем
            continue;
        // символ и вероятность
        char sym;
        double prob;

        if ((err = parse_prob_value(tokens->items[i].value, &sym, &prob)) != PASSGEN_OK) // пишем символ и вероятность
        {
            goto cleanup;
        }
        // ищем символ в нашей табличке
        bool found = false;
        for (size_t j = 0; j < pt->count; j++)
        {
            if (pt->items[j].symbol == sym)
            {
                if (pt->items[j].prob >= 0.0) // то есть записывали раньше
                {
                    err = PASSGEN_ERR_DUPLICATE_OPTION;
                    goto cleanup;
                }
                pt->items[j].prob = prob; // пишем вероятность символа если все норм
                found = true;
                break;
            }
        }

        if (!found) // другая табличка или алфавит
        {
            err = PASSGEN_ERR_UNKNOWN_SYMBOL;
            goto cleanup;
        }
    }

    double sum = 0.0;
    size_t unassigned = 0;

    for (size_t i = 0; i < pt->count; i++) // цикл по таблице +=у всех вероятностей и считаем скок не задано
    {
        if (pt->items[i].prob >= 0.0)
        {
            sum += pt->items[i].prob;
        }
        else
        {
            unassigned++;
        }
    }

    if (sum > 1.0 + PROB_EPSILON) // неправильные вероятности
    {
        err = PASSGEN_ERR_PROB_OVERFLOW;
        goto cleanup;
    }

    if (unassigned == 0 && fabs(1.0 - sum) > PROB_EPSILON)
    {
        err = PASSGEN_ERR_PROB_INVALID;
        goto cleanup;
    }

    if (unassigned > 0) // распределяем остатки равномерно
    {
        // ищем сколько осталось
        double remainder = fmax(0.0, 1.0 - sum);
        double even_prob = remainder / unassigned;
        for (size_t i = 0; i < pt->count; i++) // пишем во все незаданные значения
        {
            if (pt->items[i].prob < 0.0)
            {
                pt->items[i].prob = even_prob;
            }
        }
    }

    double current_cdf = 0.0;
    //заполняем cdf табличку
    for (size_t i = 0; i < pt->count; i++)
    {
        current_cdf += pt->items[i].prob;
        pt->items[i].cdf = current_cdf;
    }
    pt->items[pt->count - 1].cdf = 1.0;//последний элемент у нас занимает от предыдущего до 1 что бы случайно не переполнится

    return PASSGEN_OK;

cleanup:
    prob_table_free(pt);
    return err;
}