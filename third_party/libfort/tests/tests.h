#ifndef TESTS_H
#define TESTS_H

#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <assert.h>
#include <wchar.h>
#include <locale.h>
#include <string.h>
#include <stdio.h>

#define WHEN(...)
#define THEN(...)
#define SCENARIO(...)

#if !defined(FT_CONGIG_DISABLE_WCHAR)
#define FT_HAVE_WCHAR
#endif

struct test_case {
    char name [128];
    void (*test)(void);
};

/*
 *  Test utility funcitons
 */

#define assert_true(args) assert(args)

#define d_assert_str_equal(description, str1, str2) \
    if (strcmp(str1, str2) != 0) \
    { \
        fprintf(stderr, "%s:%d(%s):Abort! %s failed. Not equals strings:\n",__FILE__,__LINE__, __func__, description); \
        fprintf(stderr, "Left string(len = %d):\n%s\n", (int)strlen(str1), str1); \
        fprintf(stderr, "Right string(len = %d):\n%s\n", (int)strlen(str2), str2); \
        exit(EXIT_FAILURE); \
    }

#define assert_str_equal(str1, str2) \
    d_assert_str_equal("Test", str1, str2)

#define assert_wcs_equal(str1, str2) \
    if (wcscmp(str1, str2) != 0) \
    { \
        fprintf(stderr, "%s:%d(%s):Abort! Not equals strings:\n",__FILE__,__LINE__, __func__); \
        setlocale(LC_CTYPE, ""); \
        fwprintf(stdout, L"Left string(len = %d):\n%ls\n", (int)wcslen(str1), str1); \
        fwprintf(stdout, L"Right string(len = %d):\n%ls\n", (int)wcslen(str2), str2); \
        fflush(stdout); \
        exit(EXIT_FAILURE); \
    }

#define assert_string_equal(str1, str2) assert_str_equal(str1.c_str(), str2.c_str())



#ifdef __cplusplus
extern "C" {
#endif
void run_test_suite(const char *test_suite_name, int n_tests, struct test_case test_suite[]);
#ifdef __cplusplus
}
#endif


#endif // TESTS_H
