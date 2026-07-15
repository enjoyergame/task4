/*
Тесты для модуля error

Кучуков Ридаль Радикович
МК-101
*/

#include "unity.h"
#include "passgen/error.h"
#include <string.h>

void setUp(void) {}
void tearDown(void) {}

void test_ok_returns_empty_string(void)
{
    TEST_ASSERT_EQUAL_STRING("", passgen_error_message(PASSGEN_OK));
}

void test_known_error_returns_nonempty_message(void)
{
    TEST_ASSERT_TRUE(strlen(passgen_error_message(PASSGEN_ERR_NOT_A_NUMBER)) > 0);
}

void test_unknown_error_has_fallback_message(void)
{
    TEST_ASSERT_TRUE(strlen(passgen_error_message(PASSGEN_ERR_UNKNOWN)) > 0);
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_ok_returns_empty_string);
    RUN_TEST(test_known_error_returns_nonempty_message);
    RUN_TEST(test_unknown_error_has_fallback_message);
    return UNITY_END();
}