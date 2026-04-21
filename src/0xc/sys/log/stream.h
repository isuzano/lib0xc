/*!
 * @copyright
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT license.
 *
 * @header
 * Internal interfaces and definitions for logging support.
 */
#ifndef __ZX_SYS_LOG_STREAM_H
#define __ZX_SYS_LOG_STREAM_H

#include "0xc/std/api.h"
#include "0xc/sys/log.h"

#include <stdint.h>
#include <stdarg.h>

__API_HEADER_BEGIN(c, nonnull, single)

// MARK: Types
typedef log_stream_scheme_t (*log_stream_get_scheme_t)(
	const log_stream_t *st
);
typedef void (*log_stream_lock_t)(
	const log_stream_t *st
);

typedef void (*log_stream_unlock_t)(
	const log_stream_t *st
);

typedef void (*log_stream_write_t)(
	const log_stream_t *st,
	const log_label_t *_Nullable label,
	const char *__cstr fmt,
	va_list ap
);

// MARK: Object
struct _log_stream {
	log_stream_get_scheme_t st_get_scheme;
	log_stream_lock_t _Nullable st_lock;
	log_stream_unlock_t _Nullable st_unlock;
	log_stream_write_t st_write;
	const char *_Nullable st_prefix;
	char st_terminator;
	uintptr_t st_handle;
};
DECL_TYPE_4IWYU(log_stream_t);

__API_HEADER_END(c, nonnull, single)

#endif // __ZX_SYS_LOG_STREAM_H
