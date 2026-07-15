/*
тесты

Кучуков Ридаль Радикович
МК-101
*/

#include "unity.h"
#include "charset.h"

void setUp(void) {}
void tearDown(void) {}

void test_charset_groups_valid(void)
{
    CharsetInfo cs;
    charset_init(&cs);

    TEST_ASSERT_EQUAL_INT(PASSGEN_OK, charset_parse_groups(&cs, "aD"));
    TEST_ASSERT_TRUE(cs.groups & CHARSET_GROUP_LOWER);
    TEST_ASSERT_TRUE(cs.groups & CHARSET_GROUP_DIGITS);
    TEST_ASSERT_FALSE(cs.groups & CHARSET_GROUP_UPPER);
    TEST_ASSERT_FALSE(cs.groups & CHARSET_GROUP_SPECIAL);

    charset_free(&cs);
}

void test_charset_groups_duplicate_char(void)
{
    CharsetInfo cs;
    charset_init(&cs);

    TEST_ASSERT_EQUAL_INT(PASSGEN_ERR_DUPLICATE_CHAR, charset_parse_groups(&cs, "aDa"));

    charset_free(&cs);
}

void test_charset_groups_invalid_char(void)
{
    CharsetInfo cs;
    charset_init(&cs);

    TEST_ASSERT_EQUAL_INT(PASSGEN_ERR_UNKNOWN, charset_parse_groups(&cs, "aX"));

    charset_free(&cs);
}

void test_charset_custom_valid(void)
{
    CharsetInfo cs;
    charset_init(&cs);

    TEST_ASSERT_EQUAL_INT(PASSGEN_OK, charset_parse_custom(&cs, "123456"));
    TEST_ASSERT_EQUAL_STRING("123456", cs.custom);
    TEST_ASSERT_EQUAL_INT(6, cs.custom_len);

    charset_free(&cs);
}

void test_charset_custom_duplicate_char(void)
{
    CharsetInfo cs;
    charset_init(&cs);

    TEST_ASSERT_EQUAL_INT(PASSGEN_ERR_DUPLICATE_CHAR, charset_parse_custom(&cs, "1234561"));

    charset_free(&cs);
}

void test_charset_missing_value(void)
{
    CharsetInfo cs;
    charset_init(&cs);

    TEST_ASSERT_EQUAL_INT(PASSGEN_ERR_MISSING_VALUE, charset_parse_groups(&cs, NULL));
    TEST_ASSERT_EQUAL_INT(PASSGEN_ERR_MISSING_VALUE, charset_parse_custom(&cs, ""));

    charset_free(&cs);
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_charset_groups_valid);
    RUN_TEST(test_charset_groups_duplicate_char);
    RUN_TEST(test_charset_groups_invalid_char);
    RUN_TEST(test_charset_custom_valid);
    RUN_TEST(test_charset_custom_duplicate_char);
    RUN_TEST(test_charset_missing_value);
    return UNITY_END();
}