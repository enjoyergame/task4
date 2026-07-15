#include "generator.h"
#include "config_parser.h"

#include <stdlib.h>
#include <time.h>

void generator_init(void) 
{
    srand((unsigned int)time(NULL));
}

int generator_determine_length(const AppConfig *config) 
{
    if (config == NULL) {
        return 0;
    }

    if (config->n != CONFIG_NOT_SET) {
        return config->n;
    }

    if (config->minl != CONFIG_NOT_SET && config->maxl != CONFIG_NOT_SET) {
        int range = config->maxl - config->minl;
        if (range < 0) {
            range = 0;
        }
        return config->minl + (rand() % (range + 1));
    }

    return CONFIG_DEFAULT_LENGTH;
}

static char pick_char_by_cdf(const ProbTable *pt, double r) 
{
    for (size_t j = 0; j < pt->count; j++) {
        if (r <= pt->items[j].cdf) {
            return pt->items[j].symbol;
        }
    }
    return pt->items[pt->count - 1].symbol;
}

PassgenError generator_create_password(const AppConfig *config, const ProbTable *pt, char *out_buffer, size_t buffer_size) 
{
    if (config == NULL || pt == NULL || out_buffer == NULL) {
        return PASSGEN_ERR_UNKNOWN;
    }
    
    if (pt->count == 0) {
        return PASSGEN_ERR_EMPTY_ALPHABET;
    }

    int len = generator_determine_length(config);

    if ((size_t)len >= buffer_size) {
        return PASSGEN_ERR_BUFFER_TOO_SMALL;
    }

    for (int i = 0; i < len; i++) {
        double r = (double)rand() / (double)RAND_MAX;
        out_buffer[i] = pick_char_by_cdf(pt, r);
    }
    
    out_buffer[len] = '\0';

    return PASSGEN_OK;
}