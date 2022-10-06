#ifndef CELL_H
#define CELL_H

#include "fort_utils.h"

FT_INTERNAL
f_cell_t *create_cell(void);

FT_INTERNAL
void destroy_cell(f_cell_t *cell);

FT_INTERNAL
f_cell_t *copy_cell(f_cell_t *cell);

FT_INTERNAL
size_t cell_vis_width(const f_cell_t *cell, const f_context_t *context);

FT_INTERNAL
size_t cell_invis_codes_width(const f_cell_t *cell, const f_context_t *context);

FT_INTERNAL
size_t hint_height_cell(const f_cell_t *cell, const f_context_t *context);

FT_INTERNAL
void set_cell_type(f_cell_t *cell, enum f_cell_type type);

FT_INTERNAL
enum f_cell_type get_cell_type(const f_cell_t *cell);

FT_INTERNAL
int cell_printf(f_cell_t *cell, size_t row, f_conv_context_t *cntx, size_t cod_width);

FT_INTERNAL
f_status fill_cell_from_string(f_cell_t *cell, const char *str);

#ifdef FT_HAVE_WCHAR
FT_INTERNAL
f_status fill_cell_from_wstring(f_cell_t *cell, const wchar_t *str);
#endif

FT_INTERNAL
f_status fill_cell_from_buffer(f_cell_t *cell, const f_string_buffer_t *buf);

FT_INTERNAL
f_string_buffer_t *cell_get_string_buffer(f_cell_t *cell);

#endif /* CELL_H */
