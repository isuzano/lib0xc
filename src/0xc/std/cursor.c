/*!
 * @copyright
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT license.
 */
#include "cursor.h"

#include "0xc/std/api.h"
#include "0xc/sys/errno.h"
#include "0xc/sys/panic.h"
#include "0xc/std/int.h"
#include "0xc/std/string.h"

#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

// MARK: Utilities
static errno_t
_ccheckmode(
		const CURSOR *cur,
		const char *__null_terminated mode)
{
	if (cur->cur_mode[1] == '+') {
		return 0;
	}
	if (cur->cur_mode[0] != mode[0]) {
		return EACCES;
	}
	return 0;
}

// MARK: Public
CURSOR *
cmemopen(
		CURSOR *cur,
		void *__sized_by(p_len) p,
		size_t p_len,
		const char *mode)
{
	struct_version_assert(CURSOR, cur->cur_version, 0, CURSOR_VERSION);
	cur->cur_p = p;
	cur->cur_len = p_len;
	strscpy(cur->cur_mode, mode);

	switch (cur->cur_mode[0]) {
	case 'r':
	case 'w':
		break;
	default:
		panicx("invalid cursor mode: '%s'", mode);
	}

	switch (cur->cur_mode[1]) {
	case '+':
	case '\0':
		break;
	default:
		panicx("invalid cursor mode: '%s'", mode);
	}

	return cur;
}

errno_t
cprintf(
		CURSOR *cur,
		const char *__null_terminated fmt,
		...)
{
	errno_t __posix_error error = -1;
	va_list ap;

	va_start(ap, fmt);
	error = vcprintf(cur, fmt, ap);
	va_end(ap);

	return error;
}

errno_t
vcprintf(
		CURSOR *cur,
		const char *__null_terminated fmt,
		va_list ap)
{
	errno_t __posix_error error = -1;
	int ret = -1;
	size_t n = 0;

	error = _ccheckmode(cur, "w");
	if (error) {
		goto __out;
	}

	// vsnprintf(3) will set errno on failure in most POSIX environments, but in
	// embedded environments it may not, so we explicitly set it to zero here
	// and see if it changed. If no error condition was indicated, we just
	// assume it was a space issue.
	errno = 0;
	ret = vsnprintf(cur->cur_p, cur->cur_len, fmt, ap);
	if (ret < 0) {
		error = errno ? errno : EOVERFLOW;
		goto __out;
	}

	error = 0;
	n = __cast_signed_unsigned(size_t, ret);
	if (n >= cur->cur_len) {
		error = EOVERFLOW;
		n = cur->cur_len;
	}

	cur->cur_p += n;
	cur->cur_len -= n;

__out:
	return error;
}

int
ceob(
		const CURSOR *cur)
{
	if (cur->cur_len == 0) {
		return EOF;
	}
	return 0;
}

void
cclose(
		CURSOR *__single *__single cur)
{
	*cur = NULL;
}

void
cclose_check(
		CURSOR **cur,
		int ceob_expected)
{
	CURSOR *cur_scope = *cur;
	int ceob_actual = 0;

	ceob_actual = ceob(cur_scope);
	if (ceob_actual != ceob_expected) {
		panicx("cursor end-of-buffer check failed: actual = %d, expected = %d",
		       ceob_actual, ceob_expected);
	}

	cclose(cur);
}
