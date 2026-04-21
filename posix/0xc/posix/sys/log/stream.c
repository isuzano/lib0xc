/*!
 * @copyright
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT license.
 */
#include "0xc/sys/log/stream.h"

#include "0xc/std/api.h"
#include "0xc/sys/log.h"
#include "0xc/sys/posixdefs.h"

#include <stdarg.h>
#include <unistd.h>

// MARK: IWYU
DECL_FUNC_4IWYU(vdprintf);

// MARK: Callbacks
static log_stream_scheme_t
_posix_log_stream_get_scheme(
		__unused const log_stream_t *st)
{
	return LOG_STREAM_SCHEME_LINE;
}

__printflike(3, 0)
static void
_posix_log_stream_write(
		const log_stream_t *st,
		__unused const log_label_t *label,
		const char *__cstr fmt,
		va_list ap)
{
	fd_t fd = fdfrom(st->st_handle);
	vdprintf(fd, fmt, ap);
}

// MARK: Streams
const log_stream_t _log_stream_err = {
	.st_get_scheme = &_posix_log_stream_get_scheme,
	.st_lock = NULL,
	.st_unlock = NULL,
	.st_write = &_posix_log_stream_write,
	.st_prefix = "ERROR:   ",
	.st_terminator = '\n',
	.st_handle = STDERR_FILENO,
};

const log_stream_t _log_stream_info = {
	.st_get_scheme = &_posix_log_stream_get_scheme,
	.st_lock = NULL,
	.st_unlock = NULL,
	.st_write = &_posix_log_stream_write,
	.st_prefix = "INFO:    ",
	.st_terminator = '\n',
	.st_handle = STDERR_FILENO,
};

const log_stream_t _log_stream_debug = {
	.st_get_scheme = &_posix_log_stream_get_scheme,
	.st_lock = NULL,
	.st_unlock = NULL,
	.st_write = &_posix_log_stream_write,
	.st_prefix = "DEBUG:   ",
	.st_terminator = '\n',
	.st_handle = STDERR_FILENO,
};

const log_stream_t _log_stream_debug2 = {
	.st_get_scheme = &_posix_log_stream_get_scheme,
	.st_lock = NULL,
	.st_unlock = NULL,
	.st_write = &_posix_log_stream_write,
	.st_prefix = "DEBUG++: ",
	.st_terminator = '\n',
	.st_handle = STDERR_FILENO,
};
