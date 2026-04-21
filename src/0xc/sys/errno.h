/*!
 * @copyright
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT license.
 *
 * @header
 * POSIX error code utilities. C99 requires that errno.h be present and define
 * EDOM, ERANGE, and EILSEQ as well as errno itself.
 */
#ifndef __ZX_SYS_ERRNO_H
#define __ZX_SYS_ERRNO_H

#include <0xc/std/api.h>

// IWYU pragma: begin_exports
#include <errno.h>
#if __has_include(<sys/errno.h>)
#include <sys/errno.h>
#endif
// IWYU pragma: end_exports

__API_HEADER_BEGIN(c, nonnull, single);

// MARK: External Declarations
__export __no_tail_call __noreturn __nonnull2 __printflike(2, 3)
void
panic(
	int error,
	const char *__cstr fmt,
	...);

// MARK: Supporting Types
#if !defined(__STDC_LIB_EXT1__)
// errno_t is part of Annex K. Strictly speaking, we probably don't even need to
// guard this, since errno_t is always going to be an int (it's defined that way
// in the annex, and errno itself must be an int), and C is fine with multiple
// typedef instances that refer to the same underlying type.
//
// Note that environments may provide errno_t without providing the rest of the
// Annex K functionality, which is mostly _s variants of certain string
// functions.
typedef int errno_t;
#endif

// This appears to be the last error code defined in Linux, which does not
// itself define ELAST. There are some kernel-only codes starting at 512, but we
// don't consider those.
#if !defined(ELAST)
#define ELAST 133
#endif

// MARK: Attributes
/*!
 * @define __posix_error
 * When applied to a variable of type errno_t, this attribute causes the
 * variable to be passed to a cleanup function which checks whether the value is
 * a valid errno code and aborts if it is not. This is useful for ensuring that
 * a function always returns a valid error code in all paths.
 */
#define __posix_error __cleanup(__posix_cleanup_error)
__inline_impl __nonnull1
static inline void
__posix_cleanup_error(
		errno_t *__unsafe_indexable __e)
{
	errno_t error = *__e;

	if (error < 0 || error >= ELAST) {
		panic(error, "invalid errno value");
	}
}

// MARK: Public
/*!
 * @function symerror
 * Returns the symbol associated with the given error code.
 *
 * @param error
 * The errno value.
 *
 * @result
 * The symbol defined for the error (e.g. for 2, the string "ENOENT" is
 * returned).
 */
__export __warn_result
const char *__cstr
symerror(
	errno_t error);

__API_HEADER_END(c, nonnull, single);

#endif // __ZX_SYS_ERRNO_H
