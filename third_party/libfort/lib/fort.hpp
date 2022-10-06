/*
libfort

MIT License

Copyright (c) 2017 - 2020 Seleznev Anton

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

/**
 * @file fort.hpp
 * @brief Main header file describing libfort C++ API .
 *
 * This files contains C++ wrappers around libfort API that can
 * be used in C++ code.
 */
#ifndef LIBFORT_HPP
#define LIBFORT_HPP

#include <iomanip>
#include <sstream>
#include <string>
#include <stdexcept>
#include <type_traits>

#include "fort.h"

namespace fort
{

/**
 * Alignment of cell content.
 */
enum class text_align {
    left   = FT_ALIGNED_LEFT,
    center = FT_ALIGNED_CENTER,
    right  = FT_ALIGNED_RIGHT
};

/**
 * Type of table row. Determines appearance of row.
 */
enum class row_type {
    common = FT_ROW_COMMON,
    header = FT_ROW_HEADER
};

/**
 * Adding strategy.
 *
 * Determines what happens with old content if current cell is not empty after
 * adding data to it. Default strategy is 'replace'.
 */
enum class add_strategy {
    replace = FT_STRATEGY_REPLACE,
    insert  = FT_STRATEGY_INSERT
};

/**
 * Colors.
 */
enum class color {
    default_color  = FT_COLOR_DEFAULT,
    black          = FT_COLOR_BLACK,
    red            = FT_COLOR_RED,
    green          = FT_COLOR_GREEN,
    yellow         = FT_COLOR_YELLOW,
    blue           = FT_COLOR_BLUE,
    magenta        = FT_COLOR_MAGENTA,
    cyan           = FT_COLOR_CYAN,
    light_gray     = FT_COLOR_LIGHT_GRAY,
    dark_gray      = FT_COLOR_DARK_GRAY,
    light_red      = FT_COLOR_LIGHT_RED,
    light_green    = FT_COLOR_LIGHT_GREEN,
    light_yellow   = FT_COLOR_LIGHT_YELLOW,
    light_blue     = FT_COLOR_LIGHT_BLUE,
    light_magenta  = FT_COLOR_LIGHT_MAGENTA,
    light_cyan     = FT_COLOR_LIGHT_CYAN,
    light_whyte    = FT_COLOR_LIGHT_WHYTE
};

/**
 * Text styles.
 */
enum class text_style {
    default_style = FT_TSTYLE_DEFAULT,
    bold          = FT_TSTYLE_BOLD,
    dim           = FT_TSTYLE_DIM,
    italic        = FT_TSTYLE_ITALIC,
    underlined    = FT_TSTYLE_UNDERLINED,
    blink         = FT_TSTYLE_BLINK,
    inverted      = FT_TSTYLE_INVERTED,
    hidden        = FT_TSTYLE_HIDDEN
};


enum class table_type {
    character,
#ifdef FT_HAVE_UTF8
    utf8
#endif /* FT_HAVE_UTF8 */
};

/**
 * Table manipulator.
 *
 * Table manipulators can be used to change current cell and change appearance
 * of cells.
 */
class table_manipulator
{
public:
    explicit table_manipulator(int i)
        : value(i)
    {
    }
    template <table_type TT>
    friend class table;
private:
    int value;
};

/**
 * Table manipulator to set current row as a header.
 */
const table_manipulator header(0);

/**
 * Table manipulator to move current cell to the first cell of the next row.
 */
const table_manipulator endr(1);

/**
 * Table manipulator to add separator to the table.
 */
const table_manipulator separator(2);

// Utility functions for internal library usage.
namespace detail
{
template <typename T>
constexpr bool is_stream_manipulator_impl() noexcept
{
    using Tdec = typename std::decay<T>::type;
    // Manipulators for integral types
    return std::is_same<Tdec, typename std::decay<decltype(std::dec)>::type>::value
           || std::is_same<Tdec, typename std::decay<decltype(std::hex)>::type>::value
           || std::is_same<Tdec, typename std::decay<decltype(std::oct)>::type>::value
           // Floating-point manipulators
           || std::is_same<Tdec, typename std::decay<decltype(std::fixed)>::type>::value
           || std::is_same<Tdec, typename std::decay<decltype(std::scientific)>::type>::value
           // Misc
           || std::is_same<Tdec, typename std::decay<decltype(std::setbase(0))>::type>::value
           || std::is_same<Tdec, typename std::decay<decltype(std::setfill('\0'))>::type>::value
           || std::is_same<Tdec, typename std::decay<decltype(std::setprecision(0))>::type>::value
           || std::is_same<Tdec, typename std::decay<decltype(std::setw(0))>::type>::value;
}
}

/**
 * Utility function that is used internally by the library to check if argument
 * passed to operator<< is a manipulator. In case default behaviour is not
 * enough write custom specialization of this function.
 */
template <typename T>
constexpr bool is_stream_manipulator() noexcept
{
    return detail::is_stream_manipulator_impl<T>();
}

/**
 * Property owner.
 *
 * property_owner is a base class for all objects (table, row, column, cell) for
 * which user can specify properties.
 */
template <typename table>
class property_owner
{
public:

    property_owner(std::size_t row_idx, std::size_t coll_idx, table *tbl, bool def = false)
        : ps_row_idx_(row_idx), ps_coll_idx_(coll_idx),
          ps_table_(tbl), set_default_properties_(def) {}

    /**
     * Set min width for the specified cell of the table.
     *
     * @param value
     *   Value of the min width.
     * @return
     *   - true: Success; cell property was changed.
     *   - false: In case of error
     */
    bool set_cell_min_width(unsigned value)
    {
        return set_property(FT_CPROP_MIN_WIDTH, value);
    }

    /**
     * Set text alignment for the specified cell of the table.
     *
     * @param value
     *   Value of the text alignment.
     * @return
     *   - true: Success; cell property was changed.
     *   - false: In case of error
     */
    bool set_cell_text_align(fort::text_align value)
    {
        return set_property(FT_CPROP_TEXT_ALIGN, static_cast<int>(value));
    }

    /**
     * Set top padding for the specified cell of the table.
     *
     * @param value
     *   Value of the top padding.
     * @return
     *   - true: Success; cell property was changed.
     *   - false: In case of error.
     */
    bool set_cell_top_padding(unsigned value)
    {
        return set_property(FT_CPROP_TOP_PADDING, value);
    }

    /**
     * Set bottom padding for the specified cell of the table.
     *
     * @param value
     *   Value of the bottom padding.
     * @return
     *   - true: Success; cell property was changed.
     *   - false: In case of error.
     */
    bool set_cell_bottom_padding(unsigned value)
    {
        return set_property(FT_CPROP_BOTTOM_PADDING, value);
    }

    /**
     * Set left padding for the specified cell of the table.
     *
     * @param value
     *   Value of the left padding.
     * @return
     *   - true: Success; cell property was changed.
     *   - false: In case of error.
     */
    bool set_cell_left_padding(unsigned value)
    {
        return set_property(FT_CPROP_LEFT_PADDING, value);
    }

    /**
     * Set right padding for the specified cell of the table.
     *
     * @param value
     *   Value of the left padding.
     * @return
     *   - true: Success; cell property was changed.
     *   - false: In case of error.
     */
    bool set_cell_right_padding(unsigned value)
    {
        return set_property(FT_CPROP_RIGHT_PADDING, value);
    }

    /**
     * Set row type for the specified cell of the table.
     *
     * @param value
     *   Value of the row type.
     * @return
     *   - true: Success; cell property was changed.
     *   - false: In case of error.
     */
    bool set_cell_row_type(fort::row_type value)
    {
        return set_property(FT_CPROP_ROW_TYPE, static_cast<int>(value));
    }

    /**
     * Set empty string height for the specified cell of the table.
     *
     * @param value
     *   Value of the empty string height.
     * @return
     *   - true: Success; cell property was changed.
     *   - false: In case of error.
     */
    bool set_cell_empty_str_height(unsigned value)
    {
        return set_property(FT_CPROP_EMPTY_STR_HEIGHT, value);
    }

    /**
     * Set content foreground color for the specified cell of the table.
     *
     * @param value
     *   Color.
     * @return
     *   - true: Success; cell property was changed.
     *   - false: In case of error.
     */
    bool set_cell_content_fg_color(fort::color value)
    {
        return set_property(FT_CPROP_CONT_FG_COLOR, static_cast<int>(value));
    }

    /**
     * Set cell background color for the specified cell of the table.
     *
     * @param value
     *   Color.
     * @return
     *   - true: Success; cell property was changed.
     *   - false: In case of error.
     */
    bool set_cell_bg_color(fort::color value)
    {
        return set_property(FT_CPROP_CELL_BG_COLOR, static_cast<int>(value));
    }

    /**
     * Set content background color for the specified cell of the table.
     *
     * @param value
     *   Color.
     * @return
     *   - true: Success; cell property was changed.
     *   - false: In case of error.
     */
    bool set_cell_content_bg_color(fort::color value)
    {
        return set_property(FT_CPROP_CONT_BG_COLOR, static_cast<int>(value));
    }

    /**
     * Set cell text style for the specified cell of the table.
     *
     * @param value
     *   Text style.
     * @return
     *   - true: Success; cell property was changed.
     *   - false: In case of error.
     */
    bool set_cell_text_style(fort::text_style value)
    {
        return set_property(FT_CPROP_CELL_TEXT_STYLE, static_cast<int>(value));
    }

    /**
     * Set content text style for the specified cell of the table.
     *
     * @param value
     *   Text style.
     * @return
     *   - true: Success; cell property was changed.
     *   - false: In case of error.
     */
    bool set_cell_content_text_style(fort::text_style value)
    {
        return set_property(FT_CPROP_CONT_TEXT_STYLE, static_cast<int>(value));
    }

protected:
    std::size_t ps_row_idx_;
    std::size_t ps_coll_idx_;
    table *ps_table_;
    bool set_default_properties_;

    bool set_property(uint32_t property, int value)
    {
        int status;
        if (set_default_properties_) {
            status = ft_set_default_cell_prop(property, value);
        } else {
            status = ft_set_cell_prop(ps_table_->table_, ps_row_idx_, ps_coll_idx_, property, value);
        }
        return FT_IS_SUCCESS(status);
    }
};


/**
 * Formatted table.
 *
 * Table template class is a C++ wrapper around struct {@link ft_table}.
 * Template parameter is {@link table_type}. Useful instantiations of table
 * template class are {@link char_table} and {@link utf8_table}.
 */
template <table_type TT = table_type::character>
class table: public property_owner<table<TT>>
{
    /**
     *  Utility types.
     */
    using table_t = table<TT>;
    using property_owner_t = property_owner<table_t>;

public:

    /**
     * Default constructor.
     */
    table()
        : property_owner_t(FT_ANY_ROW, FT_ANY_COLUMN, this),
          table_(ft_create_table())
    {
        if (table_ == NULL)
            throw std::bad_alloc();
    }

    /**
     * Destructor.
     */
    ~table()
    {
        ft_destroy_table(table_);
    }

    /**
     * Copy contstructor.
     */
    table(const table &tbl)
        : property_owner_t(FT_ANY_ROW, FT_ANY_COLUMN, this), table_(NULL)
    {
        if (tbl.table_) {
            ft_table_t *table_copy = ft_copy_table(tbl.table_);
            if (table_copy == NULL)
                throw std::runtime_error("Error during table copy");

            stream_.str(std::string());
            if (tbl.stream_.tellp() >= 0) {
                stream_ << tbl.stream_.str();
            }
            table_ = table_copy;
        }
    }

    /**
     * Move contstructor.
     */
    table(table &&tbl)
        : property_owner_t(FT_ANY_ROW, FT_ANY_COLUMN, this), table_(tbl.table_)
    {
        if (tbl.stream_.tellp() >= 0) {
            stream_ << tbl.stream_.str();
            tbl.stream_.str(std::string());
        }
        tbl.table_ = 0;
    }

    /**
     * Copy assignment operator.
     */
    table &operator=(const table &tbl)
    {
        if (&tbl == this)
            return *this;

        if (tbl.table_) {
            ft_table_t *table_copy = ft_copy_table(tbl.table_);
            if (table_copy == NULL)
                throw std::runtime_error("Error during table copy");

            stream_.str(std::string());
            if (tbl.stream_.tellp() >= 0) {
                stream_ << tbl.stream_.str();
            }
            ft_destroy_table(table_);
            table_ = table_copy;
        }
        return *this;
    }

    /**
     * Move assignment operator.
     */
    table &operator=(table &&tbl)
    {
        if (&tbl == this)
            return *this;

        if (tbl.table_) {
            stream_.str(std::string());
            if (tbl.stream_.tellp() >= 0) {
                stream_ << tbl.stream_.str();
                tbl.stream_.str(std::string());
            }
            ft_destroy_table(table_);
            table_ = tbl.table_;
            tbl.table_ = NULL;
        }
        return *this;
    }

    /**
     * Convert table to string representation.
     *
     * @return
     *   - String representation of formatted table, on success.
     *   - In case of error std::runtime_error is thrown.
     */
    std::string to_string() const
    {
        const char *str = c_str();
        if (str == NULL)
            throw std::runtime_error("Error during table to string conversion");
        return str;
    }

    /**
     * Convert table to string representation.
     *
     * Table object has ownership of the returned pointer. So there is no need to
     * free it. To take ownership user should explicitly copy the returned
     * string with strdup or similar functions.
     *
     * Returned pointer may be later invalidated by:
     * - Calling destroying the table;
     * - Other invocations of c_str or to_string.
     *
     * @return
     *   - The pointer to the string representation of formatted table, on success.
     *   - NULL on error.
     */
    const char *c_str() const
    {
#ifdef FT_HAVE_UTF8
        return (TT == table_type::character)
               ? ft_to_string(table_)
               : (const char *)ft_to_u8string(table_);
#else
        return ft_to_string(table_);
#endif
    }

    /**
     * Write provided object to the table.
     *
     * To convert object to the string representation conversion for
     * std::ostream is used.
     *
     * @param arg
     *   Obect that would be inserted in the current cell.
     * @return
     *   - Reference to the current table.
     */
    template <typename T>
    table &operator<<(const T &arg)
    {
        constexpr bool is_manip = fort::is_stream_manipulator<typename std::decay<T>::type>();
        stream_ << arg;
        if (stream_.tellp() >= 0 && !is_manip) {
#ifdef FT_HAVE_UTF8
            if (TT == table_type::character) {
                ft_nwrite(table_, 1, stream_.str().c_str());
            } else {
                ft_u8nwrite(table_, 1, (const void *)stream_.str().c_str());
            }
#else
            ft_nwrite(table_, 1, stream_.str().c_str());
#endif

            stream_.str(std::string());
        }
        return *this;
    }

    table &operator<<(const table_manipulator &arg)
    {
        if (arg.value == header.value)
            ft_set_cell_prop(table_, FT_CUR_ROW, FT_ANY_ROW, FT_CPROP_ROW_TYPE, FT_ROW_HEADER);
        else if (arg.value == endr.value)
            ft_ln(table_);
        else if (arg.value == separator.value)
            ft_add_separator(table_);
        return *this;
    }

    /**
     * Write string to the the table.
     *
     * Write specified string to the current cell.
     *
     * @param str
     *   String to write.
     * @return
     *   - 0: Success; data were written
     *   - (<0): In case of error
     */
    bool write(const char *str)
    {
#ifdef FT_HAVE_UTF8
        if (TT == table_type::character) {
            return FT_IS_SUCCESS(ft_write(table_, str));
        } else {
            return FT_IS_SUCCESS(ft_u8write(table_, (const void *)str));
        }
#else
        return FT_IS_SUCCESS(ft_write(table_, str));
#endif
    }

    /**
     * Write string to the the table and go to the next line.
     *
     * Write specified string to the current cell and move current position to
     * the first cell of the next line(row).
     *
     * @param str
     *   String to write.
     * @return
     *   - 0: Success; data were written
     *   - (<0): In case of error
     */
    bool write_ln(const char *str)
    {
#ifdef FT_HAVE_UTF8
        if (TT == table_type::character) {
            return FT_IS_SUCCESS(ft_write_ln(table_, str));
        } else {
            return FT_IS_SUCCESS(ft_u8write_ln(table_, str));
        }
#else
        return FT_IS_SUCCESS(ft_write_ln(table_, str));
#endif
    }

    /**
     * Write string to the the table.
     *
     * Write specified string to the current cell.
     *
     * @param str
     *   String to write.
     * @return
     *   - 0: Success; data were written
     *   - (<0): In case of error
     */
    bool write(const std::string &str)
    {
        return write(str.c_str());
    }

    /**
     * Write string to the the table and go to the next line.
     *
     * Write specified string to the current cell and move current position to
     * the first cell of the next line(row).
     *
     * @param str
     *   String to write.
     * @return
     *   - 0: Success; data were written
     *   - (<0): In case of error
     */
    bool write_ln(const std::string &str)
    {
        return write_ln(str.c_str());
    }

#ifdef __cpp_variadic_templates
    /**
     * Write strings to the table.
     *
     * Write specified strings to the same number of consecutive cells in the
     * current row.
     *
     * @param str
     *   String to write.
     * @param strings
     *   Strings to write.
     * @return
     *   - 0: Success; data were written
     *   - (<0): In case of error
     */
    template <typename T, typename ...Ts>
    bool write(const T &str, const Ts &...strings)
    {
        return write(str) && write(strings...);
    }

    /**
     * Write strings to the table and go to the next line.
     *
     * Write specified strings to the same number of consecutive cells in the
     * current row  and move current position to the first cell of the next
     * line(row).
     *
     * @param str
     *   String to write.
     * @param strings
     *   Strings to write.
     * @return
     *   - 0: Success; data were written
     *   - (<0): In case of error
     */
    template <typename T, typename ...Ts>
    bool write_ln(const T &str, const Ts &...strings)
    {
        return write(str) && write_ln(strings...);
    }
#else /* __cpp_variadic_templates */

    template <typename T_0, typename T_1>
    bool write(const T_0 &arg_0, const T_1 &arg_1)
    {
        return write(arg_0) && write(arg_1);
    }

    template <typename T_0, typename T_1, typename T_2>
    bool write(const T_0 &arg_0, const T_1 &arg_1, const T_2 &arg_2)
    {
        return write(arg_0) && write(arg_1, arg_2);
    }

    template <typename T_0, typename T_1, typename T_2, typename T_3>
    bool write(const T_0 &arg_0, const T_1 &arg_1, const T_2 &arg_2, const T_3 &arg_3)
    {
        return write(arg_0) && write(arg_1, arg_2, arg_3);
    }

    template <typename T_0, typename T_1, typename T_2, typename T_3, typename T_4>
    bool write(const T_0 &arg_0, const T_1 &arg_1, const T_2 &arg_2, const T_3 &arg_3, const T_4 &arg_4)
    {
        return write(arg_0) && write(arg_1, arg_2, arg_3, arg_4);
    }

    template <typename T_0, typename T_1, typename T_2, typename T_3, typename T_4, typename T_5>
    bool write(const T_0 &arg_0, const T_1 &arg_1, const T_2 &arg_2, const T_3 &arg_3, const T_4 &arg_4, const T_5 &arg_5)
    {
        return write(arg_0) && write(arg_1, arg_2, arg_3, arg_4, arg_5);
    }

    template <typename T_0, typename T_1, typename T_2, typename T_3, typename T_4, typename T_5, typename T_6>
    bool write(const T_0 &arg_0, const T_1 &arg_1, const T_2 &arg_2, const T_3 &arg_3, const T_4 &arg_4, const T_5 &arg_5, const T_6 &arg_6)
    {
        return write(arg_0) && write(arg_1, arg_2, arg_3, arg_4, arg_5, arg_6);
    }

    template <typename T_0, typename T_1, typename T_2, typename T_3, typename T_4, typename T_5, typename T_6, typename T_7>
    bool write(const T_0 &arg_0, const T_1 &arg_1, const T_2 &arg_2, const T_3 &arg_3, const T_4 &arg_4, const T_5 &arg_5, const T_6 &arg_6, const T_7 &arg_7)
    {
        return write(arg_0) && write(arg_1, arg_2, arg_3, arg_4, arg_5, arg_6, arg_7);
    }


    template <typename T_0, typename T_1>
    bool write_ln(const T_0 &arg_0, const T_1 &arg_1)
    {
        return write(arg_0) && write_ln(arg_1);
    }

    template <typename T_0, typename T_1, typename T_2>
    bool write_ln(const T_0 &arg_0, const T_1 &arg_1, const T_2 &arg_2)
    {
        return write(arg_0) && write_ln(arg_1, arg_2);
    }

    template <typename T_0, typename T_1, typename T_2, typename T_3>
    bool write_ln(const T_0 &arg_0, const T_1 &arg_1, const T_2 &arg_2, const T_3 &arg_3)
    {
        return write(arg_0) && write_ln(arg_1, arg_2, arg_3);
    }

    template <typename T_0, typename T_1, typename T_2, typename T_3, typename T_4>
    bool write_ln(const T_0 &arg_0, const T_1 &arg_1, const T_2 &arg_2, const T_3 &arg_3, const T_4 &arg_4)
    {
        return write(arg_0) && write_ln(arg_1, arg_2, arg_3, arg_4);
    }

    template <typename T_0, typename T_1, typename T_2, typename T_3, typename T_4, typename T_5>
    bool write_ln(const T_0 &arg_0, const T_1 &arg_1, const T_2 &arg_2, const T_3 &arg_3, const T_4 &arg_4, const T_5 &arg_5)
    {
        return write(arg_0) && write_ln(arg_1, arg_2, arg_3, arg_4, arg_5);
    }

    template <typename T_0, typename T_1, typename T_2, typename T_3, typename T_4, typename T_5, typename T_6>
    bool write_ln(const T_0 &arg_0, const T_1 &arg_1, const T_2 &arg_2, const T_3 &arg_3, const T_4 &arg_4, const T_5 &arg_5, const T_6 &arg_6)
    {
        return write(arg_0) && write_ln(arg_1, arg_2, arg_3, arg_4, arg_5, arg_6);
    }

    template <typename T_0, typename T_1, typename T_2, typename T_3, typename T_4, typename T_5, typename T_6, typename T_7>
    bool write_ln(const T_0 &arg_0, const T_1 &arg_1, const T_2 &arg_2, const T_3 &arg_3, const T_4 &arg_4, const T_5 &arg_5, const T_6 &arg_6, const T_7 &arg_7)
    {
        return write(arg_0) && write_ln(arg_1, arg_2, arg_3, arg_4, arg_5, arg_6, arg_7);
    }

#endif /* __cpp_variadic_templates */

    /**
     * Write elements from range to the table.
     *
     * Write objects from range to consecutive cells in the current row.
     *
     * @param first, last
     *   Range of elements.
     * @return
     *   - 0: Success; data were written
     *   - (<0): In case of error
     */
    template <typename InputIt>
    bool range_write(InputIt first, InputIt last)
    {
        while (first != last) {
            *this << *first;
            ++first;
        }
        return true;
    }

    /**
     * Write elements from range to the table and go to the next line.
     *
     * Write objects from range to consecutive cells in the current row and
     * move current position to the first cell of the next line(row).
     *
     * @param first, last
     *   Range of elements.
     * @return
     *   - 0: Success; data were written
     *   - (<0): In case of error
     */
    template <typename InputIt>
    bool range_write_ln(InputIt first, InputIt last)
    {
        while (first != last) {
            *this << *first;
            ++first;
        }
        ft_ln(table_);
        return true;
    }

    /**
     * Set border style for the table.
     *
     * @param style
     *   Pointer to border style.
     * @return
     *   - True: Success; table border style was changed.
     *   - False: Error
     */
    bool set_border_style(const struct ft_border_style *style)
    {
        return FT_IS_SUCCESS(ft_set_border_style(table_, style));
    }

    /**
     * Set current cell position.
     *
     * Current cell - cell that will be edited with all modifiing functions.
     *
     * @param row_i
     *   New row number for the current cell.
     * @param col_i
     *   New row number for the current cell.
     */
    void set_cur_cell(size_t row_i, size_t col_i)
    {
        ft_set_cur_cell(table_, row_i, col_i);
    }


    /**
     * Set table left margin.
     *
     * @param value
     *   Left margin.
     * @return
     *   - true: Success; table property was changed.
     *   - false: In case of error.
     */
    bool set_left_margin(unsigned value)
    {
        return FT_IS_SUCCESS(ft_set_tbl_prop(table_, FT_TPROP_LEFT_MARGIN, value));
    }

    /**
     * Set table top margin.
     *
     * @param value
     *   Top margin.
     * @return
     *   - true: Success; table property was changed.
     *   - false: In case of error.
     */
    bool set_top_margin(unsigned value)
    {
        return FT_IS_SUCCESS(ft_set_tbl_prop(table_, FT_TPROP_TOP_MARGIN, value));
    }

    /**
     * Set table right margin.
     *
     * @param value
     *   Right margin.
     * @return
     *   - true: Success; table property was changed.
     *   - false: In case of error.
     */
    bool set_right_margin(unsigned value)
    {
        return FT_IS_SUCCESS(ft_set_tbl_prop(table_, FT_TPROP_RIGHT_MARGIN, value));
    }

    /**
     * Set table bottom margin.
     *
     * @param value
     *   Bottom margin.
     * @return
     *   - true: Success; table property was changed.
     *   - false: In case of error.
     */
    bool set_bottom_margin(unsigned value)
    {
        return FT_IS_SUCCESS(ft_set_tbl_prop(table_, FT_TPROP_BOTTOM_MARGIN, value));
    }

    /**
     * Set table adding strategy.
     *
     * @param value
     *   Adding strategy.
     * @return
     *   - true: Success; table property was changed.
     *   - false: In case of error.
     */
    bool set_adding_strategy(fort::add_strategy value)
    {
        return FT_IS_SUCCESS(ft_set_tbl_prop(table_,
                                             FT_TPROP_ADDING_STRATEGY,
                                             static_cast<int>(value)));
    }
private:
    ft_table_t *table_;
    mutable std::stringstream stream_;
    friend class property_owner<table>;


public:

    /* Iterators */
    /* todo: implement chains like table[0][0] = table [0][1] = "somethings" */

    /**
     * Table cell.
     */
    class table_cell: public property_owner_t
    {
        using property_owner_t::ps_coll_idx_;
        using property_owner_t::ps_row_idx_;
        using property_owner_t::ps_table_;
        using property_owner_t::set_default_properties_;
    public:
        table_cell(std::size_t row_idx, std::size_t coll_idx, table &tbl)
            : property_owner_t(row_idx, coll_idx, &tbl) {}

        table_cell &operator=(const char *str)
        {
            ft_set_cur_cell(ps_table_->table_, ps_row_idx_, ps_coll_idx_);
            ps_table_->write(str);
            return *this;
        }

        table_cell &operator=(const std::string &str)
        {
            return operator=(str.c_str());
        }

        /**
         * Set column span for the specified cell of the table.
         *
         * @param hor_span
         *   Column span.
         * @return
         *   - true: Success; cell span was changed.
         *   - false: In case of error.
         */
        bool set_cell_span(size_t hor_span)
        {
            if (set_default_properties_)
                return false;

            return FT_IS_SUCCESS(ft_set_cell_span(ps_table_->table_, ps_row_idx_, ps_coll_idx_, hor_span));
        }
    };

    /**
     * Table row.
     */
    class table_row: public property_owner_t
    {
        using property_owner_t::ps_row_idx_;
        using property_owner_t::ps_table_;
    public:
        table_row(std::size_t row_idx, table &tbl)
            : property_owner_t(row_idx, FT_ANY_COLUMN, &tbl) {}

        class table_cell
            operator[](std::size_t coll_idx)
        {
            return table_cell(ps_row_idx_, coll_idx, *ps_table_);
        }

        void erase()
        {
            if (FT_IS_ERROR(ft_erase_range(ps_table_->table_,
                                           property_owner_t::ps_row_idx_, 0,
                                           property_owner_t::ps_row_idx_, FT_MAX_COL_INDEX))) {
                throw std::runtime_error("Failed to erase row");
            }
        }
    };

    /**
     * Table column.
     */
    class table_column: public property_owner_t
    {
        using property_owner_t::ps_coll_idx_;
        using property_owner_t::ps_table_;
    public:
        table_column(std::size_t col_idx, table &tbl)
            : property_owner_t(FT_ANY_ROW, col_idx, &tbl) {}

        void erase()
        {
            if (FT_IS_ERROR(ft_erase_range(ps_table_->table_,
                                           0, ps_coll_idx_,
                                           FT_MAX_ROW_INDEX, ps_coll_idx_))) {
                throw std::runtime_error("Failed to erase column");
            }
        }
    };

    /**
     * Range of cells.
     *
     * @note: at the moment function of propery owener will work only on the
     * top left cell.
     * @todo: Implement their work on the whole range.
     */
    class cell_range: public property_owner_t
    {
        using property_owner_t::ps_coll_idx_;
        using property_owner_t::ps_row_idx_;
        using property_owner_t::ps_table_;
    public:
        cell_range(size_t top_left_row, size_t top_left_col,
                   size_t bottom_right_row, size_t bottom_right_col,
                   table &tbl)
            : property_owner_t(top_left_row, top_left_col, &tbl),
              bottom_right_row_(bottom_right_row),
              bottom_right_col_(bottom_right_col)
        {}

        void erase()
        {
            if (FT_IS_ERROR(ft_erase_range(ps_table_->table_,
                                           ps_row_idx_, ps_coll_idx_,
                                           bottom_right_row_, bottom_right_col_))) {
                throw std::runtime_error("Failed to erase column");
            }
        }

    private:
        std::size_t bottom_right_row_;
        std::size_t bottom_right_col_;
    };


    class default_properties: public property_owner_t
    {
    public:
        default_properties(table *tbl)
            : property_owner_t(FT_ANY_ROW, FT_ANY_COLUMN, tbl, true) {}
    };

    class table_row
        operator[](std::size_t row_idx)
    {
        return table_row(row_idx, *this);
    }


    /**
     * Get cell.
     *
     * @param row_idx
     *   Row index.
     * @param col_idx
     *   Column index.
     * @return
     *   table_cell object.
     */
    class table_cell
        cell(std::size_t row_idx, std::size_t col_idx)
    {
        return (*this)[row_idx][col_idx];
    }

    /**
     * Get column number of the current cell.
     *
     * @return
     *   Column number of the current cell.
     */
    size_t
    cur_col() const noexcept
    {
        return ft_cur_col(table_);
    }

    /**
     * Get row number of the current cell.
     *
     * @return
     *   Row number of the current cell.
     */
    size_t
    cur_row() const noexcept
    {
        return ft_cur_row(table_);
    }

    /**
     * Check if table is empty.
     *
     * @return
     *   true - table is empty
     *   false - some data has been inserted
     */
    bool
    is_empty() const noexcept
    {
        return ft_is_empty(table_);
    }

    /**
     * Get number of rows in the table.
     *
     * @return
     *   Number of rows in the table.
     */
    std::size_t
    row_count() const noexcept
    {
        return ft_row_count(table_);
    }

    /**
     * Get number of columns in the table.
     *
     * @return
     *   Number of columns in the table.
     */
    std::size_t
    col_count() const noexcept
    {
        return ft_col_count(table_);
    }

    /**
     * Get current cell.
     *
     * @return
     *   Current cell.
     */
    class table_cell
        cur_cell()
    {
        return cell(cur_row(), cur_col());
    }

    /**
     * Get row.
     *
     * @param row_idx
     *   Row index.
     * @return
     *   table_row object.
     */
    class table_row
        row(std::size_t row_idx)
    {
        return table_row(row_idx, *this);
    }

    /**
     * Get column.
     *
     * @param col_idx
     *   Column index.
     * @return
     *   table_column object.
     */
    class table_column
        column(std::size_t col_idx)
    {
        return table_column(col_idx, *this);
    }

    /**
     * Get range of cells.
     *
     * @param col_idx
     *   Column index.
     * @return
     *   table_column object.
     */
    class cell_range
        range(std::size_t top_left_row, std::size_t top_left_col,
              std::size_t bottom_right_row, std::size_t bottom_right_col)
    {
        return cell_range(top_left_row, top_left_col,
                          bottom_right_row, bottom_right_col,
                          *this);
    }

    static class default_properties
        default_props()
    {
        return default_properties(NULL);
    }
};

/**
 * Formatted table containing common char content.
 *
 * Content of the table is treated as a string where each byte represesents a
 * character. Should work for ascii characters. In case of usage of different
 * international symbols it is recommended to use {@link utf8_table}.
 */
using char_table = table<table_type::character>;

#ifdef FT_HAVE_UTF8
/**
 * Formatted table containing utf-8 content.
 */
using utf8_table = table<table_type::utf8>;
#endif

/**
 * Set default border style for all new formatted tables.
 *
 * @param style
 *   Pointer to border style.
 * @return
 *   - True: Success; table border style was changed.
 *   - False: Error
 */
inline bool set_default_border_style(struct ft_border_style *style)
{
    return FT_IS_SUCCESS(ft_set_default_border_style(style));
}


} // namespace fort

#endif // LIBFORT_HPP
