/*!
 * @copyright
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT license.
 *
 * @header
 * Unit test checks. Each API takes a description of what is being checked. This
 * can be thought of as a noun to complete the sentence "checking the..."
 */
#ifndef __ZX_SYS_CHECK_H
#define __ZX_SYS_CHECK_H

#include <0xc/std/api.h>
#include <0xc/sys/errno.h>

#include <stdint.h>
#include <stdlib.h>
#include <stdarg.h>

__API_HEADER_BEGIN(c, nonnull, single)

// MARK: Forward Types
/*!
 * @typedef check_t
 * An opaque type representing the type of check being done.
 */
typedef struct _check check_t;

// MARK: Globals
/*!
 * @const CHECK_FATAL
 * A check which aborts all testing if failed.
 */
__export
const check_t _check_fatal;
#define CHECK_FATAL __CONSTANT(check, fatal)

/*!
 * @function checkuint
 * Checks the value of an unsigned integer.
 *
 * @param FMT
 * A format string describing the test case.
 *
 * @param ACTUAL
 * The value recorded.
 *
 * @param EXPECTED
 * The expected value.
 *
 * @param ...
 * The arguments corresponding to @FMT.
 */
#define checkuint(FMT, ACTUAL, EXPECTED, ...) ({ \
	_checkuint(CHECK_FATAL, "%s:%d: " FMT, \
			(ACTUAL), (EXPECTED), \
			__FILE_NAME__, __LINE__, ## __VA_ARGS__); \
})

/*!
 * @function checkint
 * Checks the value of a signed integer.
 *
 * @param FMT
 * A format string describing the test case.
 *
 * @param ACTUAL
 * The value recorded.
 *
 * @param EXPECTED
 * The expected value.
 *
 * @param ...
 * The arguments corresponding to @FMT.
 */
#define checkint(FMT, ACTUAL, EXPECTED, ...) ({ \
	_checkint(CHECK_FATAL, "%s:%d: " FMT, \
			(ACTUAL), (EXPECTED), \
			__FILE_NAME__, __LINE__, ## __VA_ARGS__); \
})

/*!
 * @function checkerrno
 * Checks the value of a POSIX error number.
 *
 * @param FMT
 * A description of the error being checked, e.g. "syscall return".
 *
 * @param ACTUAL
 * The value recorded.
 *
 * @param EXPECTED
 * The expected value.
 *
 * @param ...
 * The arguments corresponding to @FMT.
 */
#define checkerrno(FMT, ACTUAL, EXPECTED, ...) ({ \
	_checkerrno(CHECK_FATAL, "%s:%d: " FMT, \
			(ACTUAL), (EXPECTED), \
			__FILE_NAME__, __LINE__, ## __VA_ARGS__); \
})
/*!
 * @function checkcstr
 * Checks the value of a C string.
 *
 * @param FMT
 * A format string describing the test case.
 *
 * @param ACTUAL
 * The value recorded.
 *
 * @param EXPECTED
 * The expected value.
 *
 * @param ...
 * The arguments corresponding to @FMT.
 */
#define checkcstr(FMT, ACTUAL, EXPECTED, ...) ({ \
	_checkcstr(CHECK_FATAL, "%s:%d: " FMT, \
			(ACTUAL), (EXPECTED), \
			__FILE_NAME__, __LINE__, ## __VA_ARGS__); \
})

/*!
 * @function checkmem
 * Checks the bytes in a memory region.
 *
 * @param FMT
 * A format string describing the test case.
 *
 * @param ACTUAL
 * The value recorded. May be NULL.
 *
 * @param ACTUAL_len
 * The length of the buffer referred to by @ACTUAL.
 *
 * @param EXPECTED
 * The expected value. May be NULL.
 *
 * @param EXPECTED_len
 * The length of the buffer referred to by @EXPECTED.
 *
 * @param ...
 * The arguments corresponding to @FMT.
 */
#define checkmem(FMT, ACTUAL, ACTUAL_len, EXPECTED, EXPECTED_len, ...) ({ \
	_checkmem(CHECK_FATAL, "%s:%d: " FMT, \
			(const uint8_t *)(ACTUAL), (ACTUAL_len), \
			(const uint8_t *)(EXPECTED), (EXPECTED_len), \
			__FILE_NAME__, __LINE__, ## __VA_ARGS__); \
})

/*!
 * @function checkptr
 * Checks the value of a pointer.
 *
 * @param FMT
 * A format string describing the test case.
 *
 * @param ACTUAL
 * The value recorded.
 *
 * @param EXPECTED
 * The expected value.
 *
 * @param ...
 * The arguments corresponding to @FMT.
 */
#define checkptr(FMT, ACTUAL, EXPECTED, ...) ({ \
	_checkptr(CHECK_FATAL, "%s:%d: " FMT, \
			(ACTUAL), (EXPECTED), \
			__FILE_NAME__, __LINE__, ## __VA_ARGS__); \
})

/*!
 * @function checknull
 * Checks the value of a pointer against NULL.
 *
 * @param FMT
 * A format string describing the test case.
 *
 * @param ACTUAL
 * The value recorded.
 *
 * @param ...
 * The arguments corresponding to @FMT.
 */
#define checknull(FMT, ACTUAL, ...) ({ \
	_checknull(CHECK_FATAL, "%s:%d: " FMT, (ACTUAL), \
			__FILE_NAME__, __LINE__, ## __VA_ARGS__); \
})

/*!
 * @function checknonnull
 * Checks that the value of a pointer is non-NULL.
 *
 * @param FMT
 * A format string describing the test case.
 *
 * @param ACTUAL
 * The value recorded.
 *
 * @param ...
 * The arguments corresponding to @FMT.
 */
#define checknonnull(FMT, ACTUAL, ...) ({ \
	_checknonnull(CHECK_FATAL, "%s:%d: " FMT, (ACTUAL), \
			__FILE_NAME__, __LINE__, ## __VA_ARGS__); \
})

// MARK: Internal
/*!
 * @internal _checkuint
 * Implementation detail of @checkuint.
 */
__export __nonnull1 __nonnull2 __printflike(2, 5)
void
_checkuint(
	const check_t *chk,
	const char *__cstr fmt,
	uint64_t actual,
	uint64_t expected,
	...);

/*!
 * @internal _checkint
 * Implementation detail of @checkint.
 */
__export __nonnull1 __nonnull2 __printflike(2, 5)
void
_checkint(
	const check_t *chk,
	const char *__cstr fmt,
	int64_t actual,
	int64_t expected,
	...);

/*!
 * @internal _checkerrno
 * Implementation detail of @checkerrno.
 */
__export __nonnull1 __nonnull2 __printflike(2, 5)
void
_checkerrno(
	const check_t *chk,
	const char *__cstr fmt,
	errno_t actual,
	errno_t expected,
	...);

/*!
 * @internal _checkcstr
 * Implementation detail of @checkcstr.
 */
__export __nonnull1 __nonnull2 __nonnull3 __printflike(2, 5)
void
_checkcstr(
	const check_t *chk,
	const char *__cstr fmt,
	const char *__cstr actual,
	const char *__cstr expected,
	...);

/*!
 * @internal _checkptr
 * Implementation detail of @checkptr.
 */
__export __nonnull1 __nonnull2 __printflike(2, 5)
void
_checkptr(
	const check_t *chk,
	const char *__cstr fmt,
	const void *_Nullable actual,
	const void *_Nullable expected,
	...);

/*!
 * @internal _checkmem
 * Implementation detail of @checkmem.
 */
__export __nonnull1 __nonnull2 __nonnull3 __nonnull5  __printflike(2, 7)
void
_checkmem(
	const check_t *chk,
	const char *__cstr fmt,
	const uint8_t *__counted_by(actual_len) actual,
	size_t actual_len,
	const uint8_t *__counted_by(expected_len) expected,
	size_t expected_len,
	...);

/*!
 * @internal _checknull
 * Implementation detail of @checknull.
 */
__export __nonnull1 __nonnull2 __printflike(2, 4)
void
_checknull(
	const check_t *chk,
	const char *__cstr fmt,
	const void *_Nullable actual,
	...);

/*!
 * @internal _checknonnull
 * Implementation detail of @checknonnull.
 */
__export __nonnull1 __nonnull2 __printflike(2, 4)
void
_checknonnull(
	const check_t *chk,
	const char *__cstr fmt,
	const void *_Nullable actual,
	...);

__API_HEADER_END(c, nonnull, single)

#endif // __ZX_SYS_CHECK_H
