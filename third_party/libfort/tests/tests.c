#include "tests.h"


void run_test_suite(const char *test_suite_name, int n_tests, struct test_case test_suite[])
{
    fprintf(stderr, " ==  RUNNING %s ==\n", test_suite_name);
    fprintf(stderr, "[==========] Running %d test(s).\n", n_tests);
    int i;
    for (i = 0; i < n_tests; ++i) {
        fprintf(stderr, "[ RUN      ] %s\n", test_suite[i].name);
        test_suite[i].test();
        fprintf(stderr, "[       OK ] %s\n", test_suite[i].name);
    }
    fprintf(stderr, "[==========] %d test(s) run.\n", n_tests);
    fprintf(stderr, "[  PASSED  ] %d test(s).\n", n_tests);
}
