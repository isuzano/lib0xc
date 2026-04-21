/*!
 * @copyright
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT license.
 */
#include "stream.h"
#include "0xc/sys/log.h"

#include "0xc/std/defs.h"

#include <stdarg.h>

// MARK: Internal
log_stream_scheme_t
log_stream_get_scheme(
		const log_stream_t *st)
{
	return st->st_get_scheme(st);
}

const char *
log_stream_get_prefix(
		const log_stream_t *st)
{
	if (!st->st_prefix) {
		return "";
	}
	return st->st_prefix;
}

char
log_stream_get_terminator(
		const log_stream_t *st)
{
	return st->st_terminator;
}

void
log_stream_lock(
		const log_stream_t *st)
{
	st->st_lock(st);
}

void
log_stream_unlock(
		const log_stream_t *st)
{
	st->st_unlock(st);
}

void
log_stream_write(
		const log_stream_t *st,
		const log_label_t *label,
		const char *__cstr fmt,
		...)
{
	va_list ap;

	// Just in case an empty trailer format is passed in the __zxlog macro, we
	// short-circuit here.
	if (fmt[0] == 0) {
		return;
	}

	va_start(ap, fmt);
	st->st_write(st, label, fmt, ap);
	va_end(ap);
}
