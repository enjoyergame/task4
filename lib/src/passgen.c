/*
реализация passgen

Кучуков Ридаль Радикович
МК-101
*/

#include "passgen/passgen.h"
#include "cli_lexer.h"
#include "config_parser.h"
#include "prob_math.h"
#include "generator.h"
#include <stdio.h>
#include <stdlib.h>

PassgenError passgen_run(int argc, char *argv[])
{
    TokenList tokens = {NULL, 0};
    AppConfig config;
    config_init(&config);
    ProbTable pt;
    prob_table_init(&pt);
    char *buf = NULL;

    PassgenError err = cli_lexer_tokenize(argc, argv, &tokens);
    if (err != PASSGEN_OK)
    {
        fprintf(stderr, "%s\n", passgen_error_message(err));
        goto cleanup;
    }

    err = config_parse(&config, &tokens);
    if (err != PASSGEN_OK)
    {
        fprintf(stderr, "%s\n", passgen_error_message(err));
        goto cleanup;
    }

    err = prob_table_build(&pt, &config, &tokens);
    if (err != PASSGEN_OK)
    {
        fprintf(stderr, "%s\n", passgen_error_message(err));
        goto cleanup;
    }

    generator_init();

    int max_len = CONFIG_DEFAULT_LENGTH;
    if (config.n != CONFIG_NOT_SET)
    {
        max_len = config.n;
    }
    else if (config.maxl != CONFIG_NOT_SET)
    {
        max_len = config.maxl;
    }

    buf = (char *)malloc(max_len + 1);
    if (buf == NULL)
    {
        err = PASSGEN_ERR_NO_MEMORY;
        fprintf(stderr, "%s\n", passgen_error_message(err));
        goto cleanup;
    }

    for (int i = 0; i < config.c; i++)
    {
        err = generator_create_password(&config, &pt, buf, max_len + 1);
        if (err != PASSGEN_OK)
        {
            fprintf(stderr, "%s\n", passgen_error_message(err));
            goto cleanup;
        }

        printf("%s\n", buf);
    }

cleanup:
    if (buf != NULL)
    {
        free(buf);
    }
    prob_table_free(&pt);
    config_free(&config);
    token_list_free(&tokens);

    return err;
}