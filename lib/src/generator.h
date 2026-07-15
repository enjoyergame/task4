/*
Ядро генератора паролей

Кучуков Ридаль Радикович
МК-101
*/

#ifndef PASSGEN_GENERATOR_H
#define PASSGEN_GENERATOR_H

#include "config_parser.h"
#include "prob_math.h"
#include "passgen/error.h"
#include <stddef.h>

void generator_init(void);

int generator_determine_length(const AppConfig *config);

PassgenError generator_create_password(const AppConfig *config, const ProbTable *pt, char *out_buffer, size_t buffer_size);

#endif // PASSGEN_GENERATOR_H