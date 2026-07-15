/*
реализация для работы с наборами символов

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