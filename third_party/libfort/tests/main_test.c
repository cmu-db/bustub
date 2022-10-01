#include "tests.h"
#include <stdio.h>
#include "fort.h"

/* Test cases */
void test_bug_fixes(void);
void test_vector_basic(void);
void test_vector_stress(void);
void test_string_buffer(void);
void test_table_sizes(void);
void test_table_counts(void);
void test_table_geometry(void);
void test_table_basic(void);
void test_table_copy(void);
void test_table_changing_cell(void);
void test_table_erase(void);
#ifdef FT_HAVE_WCHAR
void test_wcs_table_boundaries(void);
#endif
void test_table_write(void);
void test_table_insert_strategy(void);
void test_table_border_style(void);
void test_table_builtin_border_styles(void);
void test_table_cell_properties(void);
void test_table_text_styles(void);
void test_table_tbl_properties(void);
void test_memory_errors(void);
void test_error_codes(void);
#ifdef FT_HAVE_UTF8
void test_utf8_table(void);
#endif


#ifdef FORT_WB_TESTING_ENABLED
struct test_case wb_test_suite [] = {
    {"test_vector_basic", test_vector_basic},
    {"test_vector_stress", test_vector_stress},
    {"test_string_buffer", test_string_buffer},
    {"test_table_sizes", test_table_sizes},
    {"test_table_counts", test_table_counts},
    {"test_table_geometry", test_table_geometry},
    {"test_table_copy", test_table_copy},
};
#endif


struct test_case bb_test_suite [] = {
    {"test_bug_fixes", test_bug_fixes},
    {"test_table_basic", test_table_basic},
#ifdef FT_HAVE_WCHAR
    {"test_wcs_table_boundaries", test_wcs_table_boundaries},
#endif
#ifdef FT_HAVE_UTF8
    {"test_utf8_table", test_utf8_table},
#endif
    {"test_table_write", test_table_write},
    {"test_table_insert_strategy", test_table_insert_strategy},
    {"test_table_changing_cell", test_table_changing_cell},
    {"test_table_erase", test_table_erase},
    {"test_table_border_style", test_table_border_style},
    {"test_table_builtin_border_styles", test_table_builtin_border_styles},
    {"test_table_cell_properties", test_table_cell_properties},
    {"test_table_tbl_properties", test_table_tbl_properties},
    {"test_table_text_styles", test_table_text_styles},
    {"test_memory_errors", test_memory_errors},
    {"test_error_codes", test_error_codes},
};

#ifdef FORT_WB_TESTING_ENABLED
int run_wb_test_suite(void)
{
    int wb_n_tests = sizeof(wb_test_suite) / sizeof(wb_test_suite[0]);
    run_test_suite("WHITE BOX TEST SUITE", wb_n_tests, wb_test_suite);
    return 0;
}
#endif


int run_bb_test_suite(void)
{
    int bb_n_tests = sizeof(bb_test_suite) / sizeof(bb_test_suite[0]);
    run_test_suite("BLACK BOX TEST SUITE", bb_n_tests, bb_test_suite);
    return 0;
}

int main(void)
{
    int status = 0;

    /*
     * Essential for OSX, because swprintf can fail in case of real unicode
     * chars.
     */
#if defined(FT_HAVE_WCHAR)
    setlocale(LC_CTYPE, "");
#endif

#ifdef FORT_WB_TESTING_ENABLED
    status |= run_wb_test_suite();
    fprintf(stderr, "\n");
#endif
    status |= run_bb_test_suite();


    return status;
}
