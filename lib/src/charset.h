/*
наборы символов

Кучуков Ридаль Радикович
МК-101
*/

#ifndef PASSGEN_CHARSET_H
#define PASSGEN_CHARSET_H

#include "passgen/error.h"
#include <stddef.h>

// Флаги стандартных групп символов для опции -C
#define CHARSET_GROUP_LOWER (1 << 0)   // 'a' - маленькие латинские 0001
#define CHARSET_GROUP_UPPER (1 << 1)   // 'A' - большие латинские  0010
#define CHARSET_GROUP_DIGITS (1 << 2)  // 'D' - цифры 0100
#define CHARSET_GROUP_SPECIAL (1 << 3) // 'S' - спецсимволы 1000

typedef struct
{
    int groups;        // Битовая маска выбранных групп (-C)
    char *custom;      // Пользовательский алфавит (-a)
    size_t custom_len; // Длина пользовательского алфавита
} CharsetInfo;

void charset_init(CharsetInfo *cs);
void charset_free(CharsetInfo *cs);

PassgenError charset_parse_groups(CharsetInfo *cs, const char *value);

PassgenError charset_parse_custom(CharsetInfo *cs, const char *value);

PassgenError charset_use_default(CharsetInfo *cs);

#endif // PASSGEN_CHARSET_H