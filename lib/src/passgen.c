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

PassgenError passgen_run(int argc, char *const argv[]) {
    TokenList tokens = {NULL, 0};
    AppConfig config;
    config_init(&config);
    ProbTable pt;
    prob_table_init(&pt);
    
    PassgenError err = cli_lexer_tokenize(argc, argv, &tokens);
    if (err != PASSGEN_OK) {
        fprintf(stderr, "%s\n", passgen_error_message(err));
        goto cleanup;
    }

    err = config_parse(&config, &tokens);
    if (err != PASSGEN_OK) {
        fprintf(stderr, "%s\n", passgen_error_message(err));
        goto cleanup;
    }

    err = prob_table_build(&pt, &config, &tokens);
    if (err != PASSGEN_OK) {
        fprintf(stderr, "%s\n", passgen_error_message(err));
        goto cleanup;
    }

    generator_init();

    for (int i = 0; i < config.c; i++) {
        int len = generator_determine_length(&config);
        
        char *buf = (char *)malloc(len + 1);
        if (buf == NULL) {
            err = PASSGEN_ERR_NO_MEMORY;
            fprintf(stderr, "%s\n", passgen_error_message(err));
            goto cleanup;
        }

        err = generator_create_password(&config, &pt, buf, len + 1);
        if (err != PASSGEN_OK) {
            fprintf(stderr, "%s\n", passgen_error_message(err));
            free(buf);
            goto cleanup;
        }

        printf("%s\n", buf);
        free(buf);
    }

cleanup:
    prob_table_free(&pt);
    config_free(&config);
    token_list_free(&tokens);

    return err;
}