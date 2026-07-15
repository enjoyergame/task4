/*
реализация работы с наборами символов

Кучуков Ридаль Радикович
МК-101
*/

#include "charset.h"
#include <stdlib.h>
#include <string.h>

void charset_init(CharsetInfo *cs) {
    if (cs == NULL) return;
    cs->groups = 0;
    cs->custom = NULL;
    cs->custom_len = 0;
}

void charset_free(CharsetInfo *cs) {
    if (cs == NULL) return;
    if (cs->custom != NULL) {
        free(cs->custom);
        cs->custom = NULL;
    }
    cs->custom_len = 0;
    cs->groups = 0;
}

PassgenError charset_parse_groups(CharsetInfo *cs, const char *value) {
    if (cs == NULL) return PASSGEN_ERR_UNKNOWN;
    if (value == NULL || *value == '\0') return PASSGEN_ERR_MISSING_VALUE;

    for (const char *p = value; *p != '\0'; p++) {
        int flag = 0;
        switch (*p) {
            case 'a': flag = CHARSET_GROUP_LOWER; break;
            case 'A': flag = CHARSET_GROUP_UPPER; break;
            case 'D': flag = CHARSET_GROUP_DIGITS; break;
            case 'S': flag = CHARSET_GROUP_SPECIAL; break;
            default:
                return PASSGEN_ERR_UNKNOWN; // Символ не входит в допустимый набор {a, A, D, S}
        }

        if ((cs->groups & flag) != 0) {
            return PASSGEN_ERR_DUPLICATE_CHAR;
        }
        cs->groups |= flag;
    }

    return PASSGEN_OK;
}

PassgenError charset_parse_custom(CharsetInfo *cs, const char *value) {
    if (cs == NULL) return PASSGEN_ERR_UNKNOWN;
    if (value == NULL || *value == '\0') return PASSGEN_ERR_MISSING_VALUE;

    size_t len = strlen(value);
    
    bool seen[256] = {false};

    for (size_t i = 0; i < len; i++) {
        unsigned char c = (unsigned char)value[i]; 
        if (seen[c]) {
            return PASSGEN_ERR_DUPLICATE_CHAR;
        }
        seen[c] = true;
    }

    if (cs->custom != NULL) {
        free(cs->custom);
        cs->custom = NULL;
    }

    cs->custom = (char *)malloc(len + 1);
    if (cs->custom == NULL) {
        return PASSGEN_ERR_NO_MEMORY;
    }

    memcpy(cs->custom, value, len + 1);
    cs->custom_len = len;

    return PASSGEN_OK;
}