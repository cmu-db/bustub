#ifndef WCWIDTH_H
#define WCWIDTH_H

#include "fort_utils.h"

#ifdef FT_HAVE_WCHAR
#include <wchar.h>

FT_INTERNAL
int mk_wcswidth(const wchar_t *pwcs, size_t n);

#endif /* FT_HAVE_WCHAR */

#endif /* WCWIDTH_H */
