/*
заголовочник для error.c
Кучуков Ридаль Радикович
МК-101
*/
#ifndef PASSGEN_ERROR_H
#define PASSGEN_ERROR_H

typedef enum
{
    PASSGEN_OK = 0,

    // Ошибки лексера аргументов командной строки 
    PASSGEN_ERR_MISSING_VALUE,     // у опции нет значения
    PASSGEN_ERR_UNKNOWN_SEPARATOR, // некорректный синтаксис разделителя

    // Ошибки парсинга значений опций 
    PASSGEN_ERR_NOT_A_NUMBER,     // значение опции должно быть числом 
    PASSGEN_ERR_DUPLICATE_OPTION, // опция указана более одного раза 
    PASSGEN_ERR_DUPLICATE_CHAR,   // повтор символа внутри -C или -a 

    // Ошибки логики/конфликтов конфигурации 
    PASSGEN_ERR_CONFLICT_N_MINMAX, // -n вместе с -minl/-maxl
    PASSGEN_ERR_CONFLICT_A_C,      // -a вместе с -C 
    PASSGEN_ERR_CONFLICT_PAIR_N,   // парная опция вместе с -n
    PASSGEN_ERR_INCOMPLETE_PAIR,   // есть одна из парных опций без второй
    PASSGEN_ERR_INCOMPLETE_RANGE,  // задан -minl без -maxl или наоборот
    PASSGEN_ERR_RANGE_ORDER,       // -minl больше -maxl

    // Ошибки вероятностей 
    PASSGEN_ERR_PROB_OVERFLOW,  // сумма вероятностей превышает 1.0
    PASSGEN_ERR_PROB_INVALID,   // вероятность не в диапазоне [0,1]
    PASSGEN_ERR_UNKNOWN_SYMBOL, // символ из вероятностей не найден в алфавите

    // Ошибки генерации и чарсета
    PASSGEN_ERR_INVALID_CHARSET_GROUP, // некорректная группа в -C
    PASSGEN_ERR_EMPTY_ALPHABET,        // алфавит пуст
    PASSGEN_ERR_BUFFER_TOO_SMALL,      // буфер под пароль слишком мал

    // Общие 
    PASSGEN_ERR_NO_MEMORY, // ошибка выделения памяти
    PASSGEN_ERR_UNKNOWN    // запасной вариант
} PassgenError;

const char *passgen_error_message(PassgenError code);

#endif // PASSGEN_ERROR_H