#include "tests.h"
#include "fort.h"


void test_error_codes(void)
{
    // Nonnegative code is success
    {
        assert_str_equal(ft_strerror(FT_SUCCESS), "Success");
        assert_str_equal(ft_strerror(0), "Success");
        assert_str_equal(ft_strerror(1), "Success");
        assert_str_equal(ft_strerror(2), "Success");
        assert_str_equal(ft_strerror(42), "Success");
        assert_str_equal(ft_strerror(INT_MAX), "Success");
    }

    // Error codes
    {
        assert_str_equal(ft_strerror(FT_MEMORY_ERROR), "Out of memory");
        assert_str_equal(ft_strerror(FT_EINVAL), "Invalid argument");
        assert_str_equal(ft_strerror(FT_INTERN_ERROR), "Internal libfort error");
        assert_str_equal(ft_strerror(FT_GEN_ERROR), "General error");

        assert_str_equal(ft_strerror(-42), "Unknown error code");
        assert_str_equal(ft_strerror(-666), "Unknown error code");
        assert_str_equal(ft_strerror(-INT_MAX), "Unknown error code");
    }
}
