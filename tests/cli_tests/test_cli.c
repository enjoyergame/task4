/*
тесты

ФИО: Кучуков Ридаль Радикович
Группа: МК-101
*/
#include "unity.h"
#include "cli_lexer.h"
#include <stdlib.h>

#define ARGC(argv_array) (sizeof(argv_array) / sizeof(argv_array[0]))

void setUp(void) {}
void tearDown(void) {}

// Тест 1: Классическая передача через пробел и слитно
void test_lexer_spaces_and_merged(void)
{
    char *const argv[] = {
        (char *)"generate.exe",
        (char *)"-minl", (char *)"10", // через пробел
        (char *)"-maxl20"              // слитно
    };
    int argc = ARGC(argv);
    TokenList list;

    PassgenError err = cli_lexer_tokenize(argc, argv, &list);

    TEST_ASSERT_EQUAL_INT(0, err); // PASSGEN_OK
    TEST_ASSERT_EQUAL_INT(2, list.count);

    TEST_ASSERT_EQUAL_STRING("-minl", list.items[0].key);
    TEST_ASSERT_EQUAL_STRING("10", list.items[0].value);

    TEST_ASSERT_EQUAL_STRING("-maxl", list.items[1].key);
    TEST_ASSERT_EQUAL_STRING("20", list.items[1].value);

    token_list_free(&list);
}

// Тест 2: Использование дефолтных разделителей (= и :)
void test_lexer_default_delimiters(void)
{
    char *const argv[] = {
        (char *)"app",
        (char *)"-minl=15",
        (char *)"-n:50"};
    int argc = ARGC(argv);
    TokenList list;

    PassgenError err = cli_lexer_tokenize(argc, argv, &list);

    TEST_ASSERT_EQUAL_INT(0, err);
    TEST_ASSERT_EQUAL_INT(2, list.count);

    TEST_ASSERT_EQUAL_STRING("-minl", list.items[0].key);
    TEST_ASSERT_EQUAL_STRING("15", list.items[0].value);

    TEST_ASSERT_EQUAL_STRING("-n", list.items[1].key);
    TEST_ASSERT_EQUAL_STRING("50", list.items[1].value);

    token_list_free(&list);
}

// Тест 3: Опции-флаги (без значений) не захватывают следующие опции
void test_lexer_boolean_options(void)
{
    char *const argv[] = {
        (char *)"app",
        (char *)"-a",
        (char *)"-maxl", (char *)"20"};
    int argc = ARGC(argv);
    TokenList list;

    PassgenError err = cli_lexer_tokenize(argc, argv, &list);

    TEST_ASSERT_EQUAL_INT(0, err);
    TEST_ASSERT_EQUAL_INT(2, list.count);

    // Опция -a должна распарситься со значением NULL
    TEST_ASSERT_EQUAL_STRING("-a", list.items[0].key);
    TEST_ASSERT_NULL(list.items[0].value);

    TEST_ASSERT_EQUAL_STRING("-maxl", list.items[1].key);
    TEST_ASSERT_EQUAL_STRING("20", list.items[1].value);

    token_list_free(&list);
}

// Тест 4: Добавление нового разделителя через -d
void test_lexer_add_custom_delimiter(void)
{
    char *const argv[] = {
        (char *)"app",
        (char *)"-d!",
        (char *)"-minl!10", // работает новый разделитель
        (char *)"-maxl=20"  // старый = тоже продолжает работать
    };
    int argc = ARGC(argv);
    TokenList list;

    PassgenError err = cli_lexer_tokenize(argc, argv, &list);

    TEST_ASSERT_EQUAL_INT(0, err);
    TEST_ASSERT_EQUAL_INT(3, list.count);

    TEST_ASSERT_EQUAL_STRING("-d", list.items[0].key);
    TEST_ASSERT_EQUAL_STRING("!", list.items[0].value);

    TEST_ASSERT_EQUAL_STRING("-minl", list.items[1].key);
    TEST_ASSERT_EQUAL_STRING("10", list.items[1].value);

    TEST_ASSERT_EQUAL_STRING("-maxl", list.items[2].key);
    TEST_ASSERT_EQUAL_STRING("20", list.items[2].value);

    token_list_free(&list);
}

// Тест 5: Полная замена разделителей через -D (сложный кейс из задания)
void test_lexer_replace_custom_delimiter(void)
{
    char *const argv[] = {
        (char *)"app",
        (char *)"-D=-",
        (char *)"-c-10",
        (char *)"-d=",
        (char *)"-C=A"};
    int argc = ARGC(argv);
    TokenList list;

    PassgenError err = cli_lexer_tokenize(argc, argv, &list);

    TEST_ASSERT_EQUAL_INT(0, err);
    TEST_ASSERT_EQUAL_INT(4, list.count);

    TEST_ASSERT_EQUAL_STRING("-D", list.items[0].key);
    TEST_ASSERT_EQUAL_STRING("-", list.items[0].value); // '=' был съеден как дефолтный разделитель

    TEST_ASSERT_EQUAL_STRING("-c", list.items[1].key);
    TEST_ASSERT_EQUAL_STRING("10", list.items[1].value); // '-' сработал как новый разделитель

    TEST_ASSERT_EQUAL_STRING("-d", list.items[2].key);
    TEST_ASSERT_EQUAL_STRING("=", list.items[2].value);

    TEST_ASSERT_EQUAL_STRING("-C", list.items[3].key);
    TEST_ASSERT_EQUAL_STRING("A", list.items[3].value);

    token_list_free(&list);
}

// Тест 6: Игнорирование неизвестных опций
void test_lexer_ignore_unknown(void)
{
    char *const argv[] = {
        (char *)"app",
        (char *)"-garbage", (char *)"123",
        (char *)"-minl", (char *)"5",
        (char *)"random_text"};
    int argc = ARGC(argv);
    TokenList list;

    PassgenError err = cli_lexer_tokenize(argc, argv, &list);

    TEST_ASSERT_EQUAL_INT(0, err);
    // Должно распарситься ровно 1 нормальное значение (-minl), остальное игнорируется
    TEST_ASSERT_EQUAL_INT(1, list.count);

    TEST_ASSERT_EQUAL_STRING("-minl", list.items[0].key);
    TEST_ASSERT_EQUAL_STRING("5", list.items[0].value);

    token_list_free(&list);
}

// Главная функция запуска тестов
int main(void)
{
    UNITY_BEGIN();

    RUN_TEST(test_lexer_spaces_and_merged);
    RUN_TEST(test_lexer_default_delimiters);
    RUN_TEST(test_lexer_boolean_options);
    RUN_TEST(test_lexer_add_custom_delimiter);
    RUN_TEST(test_lexer_replace_custom_delimiter);
    RUN_TEST(test_lexer_ignore_unknown);

    return UNITY_END();
}