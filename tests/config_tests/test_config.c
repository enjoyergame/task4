/*
тесты

Кучуков Ридаль Радикович
МК-101
*/

#include "unity.h"
#include "config_parser.h"

void setUp(void) {}
void tearDown(void) {}

void test_config_valid_basic(void)
{
    AppConfig cfg;
    config_init(&cfg);

    Token tokens[] = {
        {(char *)"-n", (char *)"10"},
        {(char *)"-c", (char *)"5"},
        {(char *)"-C", (char *)"aD"}};
    TokenList list = {tokens, 3};

    TEST_ASSERT_EQUAL_INT(PASSGEN_OK, config_parse(&cfg, &list));
    TEST_ASSERT_EQUAL_INT(10, cfg.n);
    TEST_ASSERT_EQUAL_INT(5, cfg.c);
    TEST_ASSERT_TRUE(cfg.has_C);
    TEST_ASSERT_TRUE(cfg.charset.groups & CHARSET_GROUP_LOWER);
    TEST_ASSERT_TRUE(cfg.charset.groups & CHARSET_GROUP_DIGITS);

    config_free(&cfg);
}

void test_config_conflict_n_minmax(void)
{
    AppConfig cfg;
    config_init(&cfg);

    Token tokens[] = {
        {(char *)"-n", (char *)"10"},
        {(char *)"-minl", (char *)"5"},
        {(char *)"-maxl", (char *)"15"}};
    TokenList list = {tokens, 3};

    TEST_ASSERT_EQUAL_INT(PASSGEN_ERR_CONFLICT_N_MINMAX, config_parse(&cfg, &list));
}

void test_config_conflict_a_C(void)
{
    AppConfig cfg;
    config_init(&cfg);

    Token tokens[] = {
        {(char *)"-a", (char *)"abc"},
        {(char *)"-C", (char *)"A"}};
    TokenList list = {tokens, 2};

    TEST_ASSERT_EQUAL_INT(PASSGEN_ERR_CONFLICT_A_C, config_parse(&cfg, &list));
}

void test_config_conflict_pair_n(void)
{
    AppConfig cfg;
    config_init(&cfg);

    Token tokens[] = {
        {(char *)"-s", NULL},
        {(char *)"-i", NULL},
        {(char *)"-n", (char *)"10"}};
    TokenList list = {tokens, 3};

    TEST_ASSERT_EQUAL_INT(PASSGEN_ERR_CONFLICT_PAIR_N, config_parse(&cfg, &list));
}

void test_config_incomplete_pair(void)
{
    AppConfig cfg;
    config_init(&cfg);

    Token tokens[] = {
        {(char *)"-s", NULL},
        {(char *)"-minl", (char *)"5"},
        {(char *)"-maxl", (char *)"10"}};
    TokenList list = {tokens, 3};

    TEST_ASSERT_EQUAL_INT(PASSGEN_ERR_INCOMPLETE_PAIR, config_parse(&cfg, &list));
}

void test_config_incomplete_range(void)
{
    AppConfig cfg;
    config_init(&cfg);

    Token tokens[] = {
        {(char *)"-minl", (char *)"5"}};
    TokenList list = {tokens, 1};

    TEST_ASSERT_EQUAL_INT(PASSGEN_ERR_INCOMPLETE_RANGE, config_parse(&cfg, &list));
}

void test_config_duplicate_option(void)
{
    AppConfig cfg;
    config_init(&cfg);

    Token tokens[] = {
        {(char *)"-n", (char *)"10"},
        {(char *)"-n", (char *)"20"}};
    TokenList list = {tokens, 2};

    TEST_ASSERT_EQUAL_INT(PASSGEN_ERR_DUPLICATE_OPTION, config_parse(&cfg, &list));
}

void test_config_not_a_number(void)
{
    AppConfig cfg;
    config_init(&cfg);

    Token tokens[] = {
        {(char *)"-n", (char *)"10abc"}};
    TokenList list = {tokens, 1};

    TEST_ASSERT_EQUAL_INT(PASSGEN_ERR_NOT_A_NUMBER, config_parse(&cfg, &list));
}

void test_config_invalid_number_zero(void)
{
    AppConfig cfg;
    config_init(&cfg);

    Token tokens[] = {
        {(char *)"-n", (char *)"0"}};
    TokenList list = {tokens, 1};

    TEST_ASSERT_EQUAL_INT(PASSGEN_ERR_NOT_A_NUMBER, config_parse(&cfg, &list));
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_config_valid_basic);
    RUN_TEST(test_config_conflict_n_minmax);
    RUN_TEST(test_config_conflict_a_C);
    RUN_TEST(test_config_conflict_pair_n);
    RUN_TEST(test_config_incomplete_pair);
    RUN_TEST(test_config_incomplete_range);
    RUN_TEST(test_config_duplicate_option);
    RUN_TEST(test_config_not_a_number);
    RUN_TEST(test_config_invalid_number_zero);
    return UNITY_END();
}