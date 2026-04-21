/*!
 * @copyright
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT license.
 *
 * @file
 * POSIX utilities.
 */
#ifndef __ZX_SYS_POSIXDEFS_H
#define __ZX_SYS_POSIXDEFS_H

#include "0xc/std/defs.h"
#include <0xc/std/api.h>
#include <0xc/sys/panic.h>
#include <0xc/std/int.h>

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <dirent.h>
#include <unistd.h>
#include <errno.h>
#include <sys/errno.h>

__API_HEADER_BEGIN(c, nonnull, single)

// MARK: IWYU
DECL_TYPE_4IWYU(errno_t);

// MARK: Types
/*!
 * @typedef fd_t
 * A convenience type representing a file descriptor. This mainly serves to make
 * it more obvious that a parameter or variable is expected to be a file
 * descriptor as opposed to a simple integer scalar.
 */
typedef int fd_t;

// MARK: Public
/*!
 * @function posix_switch
 * switch() construct for handling functions that set errno to provide error
 * information. This can only be used if the function returns a negative value
 * to indicate an error.
 *
 * @param RET
 * The return value from the POSIX function.
 *
 * @example
 *
 *     int r = dup(fd);
 *     posix_switch (r) {
 *     case 0:
 *         // success
 *         break;
 *     case EBADF:
 *         // Handle EBADF
 *         break;
 *     case EMFILE:
 *         // Handle EMFILE
 *         break;
 *     }
 */
#define posix_switch(RET) \
	if ((RET) < 0 || (errno = 0, 1)) switch (errno)

/*!
 * @function posix
 * Convenience wrapper for calling POSIX functions which set errno so that the
 * caller can obtain the error code directly as a return value.
 *
 * @param E
 * The function call expression.
 *
 * @result
 * If the function call returns a negative value, the return value of this macro
 * is the corresponding errno code. Otherwise, the return value of this macro is
 * the non-negative value returned by the function call.
 */
#define posix(E) ({ \
	errno_t error_0xc = 0; \
	int ret_0xc = -1; \
\
	ret_0xc = (E); \
	if (ret_0xc < 0) { \
		error_0xc = errno; \
	} \
	error_0xc; \
})

/*!
 * @function posix_acquire
 * Convenience wrapper for calling POSIX functions which return a resource
 * handle in the form of an integer return value and treat -1 as the indication
 * to check errno.
 *
 * @param E
 * The function call expression.
 *
 * @param P
 * A pointer to an integer variable where the acquired resource handle will be
 * stored if the function call is successful.
 *
 * @result
 * If the POSIX function succeeded, the return value of this macro is zero and
 * the resource handle is written to *@P. Otherwise, the return value of this
 * macro is the errno code from the POSIX function and *@P is not modified.
 */
#define posix_acquire(E, P) \
	__posix_acquire((E), (P))

/*!
 * @function posix_assert
 * Asserts on a POSIX return code and records the corresponding errno.
 *
 * @param WHAT
 * The name of the operation, e.g. "mmap", "dup".
 *
 * @param RET
 * The return code from the operation. If this value is negative, the assertion
 * fails.
 */
#define posix_assert(WHAT, RET) \
	if ((RET) < 0) { \
		panicno(errno, "%s", WHAT); \
	}

/*!
 * @function posix_assert_zero
 * Asserts on an errno_t returned directly by a function.
 *
 * @param WHAT
 * The name of the operation, e.g. "mmap", "dup".
 *
 * @param ERROR
 * The error returned from the operation. If this value is non-zero, the
 * assertion fails.
 */
#define posix_assert_zero(WHAT, ERROR) \
	if ((ERROR)) { \
		panicno((ERROR), "operation failed: %s", WHAT); \
	}

/*!
 * @function fdfrom
 * Validates that an input value is a valid file descriptor.
 *
 * @param V
 * The integer value.
 *
 * @result
 * If @V is a valid file descriptor number, it is returned as an @fd_t.
 * Otherwise, -1 is returned.
 *
 * @discussion
 * The implementation will perform a system call to determine the process'
 * descriptor table size.
 */
#define fdfrom(V) (fd_t)_Generic((V), \
	_Generic_match_unsigned_int(fdfromu), \
	_Generic_match_signed_int(fdfromi) \
)(V)

// MARK: Attributes
/*!
 * @define __posix_close
 * When applied to a variable of type @fd_t, this attribute causes the variable
 * to be passed to close(2) when it goes out of scope. If the variable's value
 * does not refer to a valid file descriptor number at the time of scope exit,
 * this is a no-op.
 *
 * @example
 *
 *     // `fd` will be automatically closed when it goes out of scope. If the
 *     // scope exits before `fd` is assigned, the cleanup does nothing.
 *     fd_t __posix_close fd = -1;
 *
 *     ...
 *
 *     fd = open("file.txt", O_RDONLY);
 */
#define __posix_close __cleanup(__posix_cleanup_fdclose)
__inline_impl __nonnull1
static inline void
__posix_cleanup_fdclose(
		int *__fd)
{
	fd_t fd = *__fd;
	if (fd < 0) {
		return;
	}

	posix_assert("fdclose", close(fd));
}

/*!
 * @define __posix_fclose
 * When applied to a variable of type @FILE *, this attribute causes the
 * variable to be passed to fclose(3) when it goes out of scope. If the
 * variable's value does not refer to a valid FILE * at the time of scope exit,
 * this is a no-op.
 *
 * @example
 *
 *     // `f` will be automatically closed when it goes out of scope. If the
 *     // scope exits before `f` is assigned, the cleanup does nothing.
 *     FILE *__posix_fclose f = NULL;
 *
 *     ...
 *
 *     f = fopen("file.txt", "r");
 */
#define __posix_fclose __cleanup(__posix_cleanup_fclose)
__inline_impl __nonnull1
static inline void
__posix_cleanup_fclose(
		FILE *_Nullable *_Nonnull __fp)
{
	FILE *f = *__fp;
	int ret = -1;

	if (!f) {
		return;
	}

	ret = fclose(f);
	if (ret == EOF) {
		assert_zero("fclose", errno);
	}
}

/*!
 * @define __posix_closedir
 * When applied to a variable of type @DIR *, this attribute causes the variable
 * to be passed to closedir(3) when it goes out of scope. If the variable's
 * value does not refer to a valid DIR * at the time of scope exit, this is a
 * no-op.
 *
 * @example
 *
 *     // `dp` will be automatically closed when it goes out of scope. If the
 *     // scope exits before `dp` is assigned, the cleanup does nothing.
 *     DIR *__posix_closedir dp = NULL;
 *
 *     ...
 *
 *     dp = opendir("directory");
 *
 */
#define __posix_closedir __cleanup(__posix_cleanup_closedir)
__inline_impl __nonnull1
static inline void
__posix_cleanup_closedir(
		DIR *_Nullable *_Nonnull __dp)
{
	DIR *dp = *__dp;

	if (!dp) {
		return;
	}

	posix_assert("closedir", closedir(dp));
}

/*!
 * @internal
 * Implementation details.
 */
// MARK: Internal
__inline_impl __warn_result
static inline fd_t
fdfromu(uint64_t v)
{
	int tbsize = getdtablesize();
	uint64_t tbsize_unsigned = __cast_signed_unsigned(uint64_t, tbsize);

	if (v >= tbsize_unsigned) {
		return -1;
	}
	return (fd_t)v;
}

__inline_impl __warn_result
static inline fd_t
fdfromi(int64_t v)
{
	if (v >= getdtablesize()) {
		return -1;
	}
	return (fd_t)v;
}

__inline_impl __warn_result __nonnull2
static inline errno_t
__posix_acquire(
		int result,
		int *r)
{
	if (result < 0) {
		return errno;
	}

	*r = result;
	return 0;
}

__API_HEADER_END(c, nonnull, single)

#endif // __ZX_SYS_POSIXDEFS_H
