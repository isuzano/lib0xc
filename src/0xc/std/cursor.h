/*!
 * @copyright
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT license.
 *
 * @header
 * Cursor object.
 */
#ifndef __ZX_STD_CURSOR_H
#define __ZX_STD_CURSOR_H

#include <0xc/std/api.h>
#include <0xc/sys/errno.h>

#include <stdlib.h>
#include <stdarg.h>

__API_HEADER_BEGIN(c, nonnull, single)

/*!
 * @struct CURSOR
 * A type representing a cursor tracking a position in a buffer. This type is
 * somewhat analogous to the FILE type used by standard C I/O functions,
 * specifically the use case represented by fmemopen(3), where a FILE object can
 * be used to read and write to a memory buffer.
 *
 * Unfortunately, the FILE type is not guaranteed to be available in all
 * environments due to its API reliance on allocation (because the structure is
 * opaque). This type is provided a lightweight alternative which is
 * consistently available.
 *
 * While this structure is not opaque, its fields should be treated as private
 * and not accessed directly.
 *
 * @field cur_version
 * The version of the structure.
 *
 * @field cur_p
 * A pointer to the current position in the buffer.
 *
 * @field cur_len
 * The length of the buffer starting from @cur_p.
 *
 * @field cur_mode
 * The mode string used to open the cursor.
 */
typedef struct _CURSOR {
	struct_version_t __fldinit cur_version;
	char *__counted_by(cur_len) cur_p;
	size_t cur_len;
	char cur_mode[3];
} CURSOR;
#define CURSOR_INIT (CURSOR){ \
	.cur_version = CURSOR_VERSION, \
}

/*!
 * @const CURSOR_VERSION
 * The version of the @CURSOR structure supported by the implementation.
 */
#define CURSOR_VERSION STRUCT_VERSION(0)

// MARK: Public
/*!
 * @function cursor_new
 * Allocates a new cursor object with the appropriate version.
 *
 * @result
 * A pointer to the object. This object resides in automatic storage and must be
 * initialized with @cmemopen or similar to be used.
 */
#define cursor_new() __new(CURSOR, \
	.cur_version = CURSOR_VERSION, \
)

/*!
 * @function cmemopen
 * Opens a memory buffer as a cursor object.
 *
 * @param cur
 * A pointer to storage for the cursor object.
 *
 * @param p
 * A pointer to the memory buffer.
 *
 * @param p_len
 * The length of the buffer referred to by @p.
 *
 * @param mode
 * The mode string, similar to that used by fopen(3). Only "r" and "w" modes are
 * supported, along with optional "+" characters to indicate read/write access.
 *
 * @result
 * A cursor object which can be used to access the buffer.
 */
__export __warn_result __nonnull1
CURSOR *
cmemopen(
	CURSOR *cur,
	void *__sized_by(p_len) _Nullable p,
	size_t p_len,
	const char *mode);

/*!
 * @function cbuffopen
 * Opens a statically-allocated buffer as a CURSOR object.
 *
 * @param C
 * A pointer to storage for the cursor object.
 *
 * @param B
 * The buffer to open.
 *
 * @param MODE
 * The mode string, cf. @cmemopen.
 *
 * @result
 * A cursor object which can be used to access the buffer.
 */
#define cbuffopen(C, B, MODE) ({ \
	__assert_type_compat(typeof(B), char [], "buffer must be a char"); \
	__assert_static_array(B, "buffer must be statically allocated"); \
	cmemopen((C), (B), sizeof(B), (MODE)); \
})

/*!
 * @function cprintf
 * Print formatted data to a cursor.
 *
 * @param cur
 * The cursor to print to.
 *
 * @param fmt
 * The printf(3)-like format string.
 *
 * @param ...
 * The arguments corresponding to @fmt.
 *
 * @result
 * Upon success, zero is returned, and the cursor is advanced according to the
 * length of the data printed. Otherwise, the implementation may directly return
 * one of the following POSIX error codes:
 *
 *     [EACCES]       The cursor is not open for writing
 *     [EOVERFLOW]    The data to be written exceeds the remaining length of the
 * 				      cursor's buffer -- in this case, as much data was written
 * 				      as possible, and the cursor has no remaining space
 *     [EILSEQ]       An invalid wide character was encountered, and the cursor
 *                    was not advanced
 */
__export __nonnull1 __nonnull2 __printflike(2, 3)
errno_t
cprintf(
	CURSOR *cur,
	const char *__null_terminated fmt,
	...);

/*!
 * @function vcprintf
 * Variant of @cprintf which accepts a va_list.
 *
 * @param cur
 * The cursor to print to.
 *
 * @param fmt
 * The printf(3)-like format string.
 *
 * @param ap
 * The arguments corresponding to @fmt.
 *
 * @result
 * This function's return behavior is identical to @cprintf.
 */
__export __nonnull1 __nonnull2 __nonnull3 __printflike(2, 0)
errno_t
vcprintf(
	CURSOR *cur,
	const char *__null_terminated fmt,
	va_list ap);

/*!
 * @function ceob
 * Checks whether the cursor has reached the end of its buffer.
 *
 * @param cur
 * The cursor to check.
 *
 * @result
 * If the buffer has space remaining, zero is returned. Otherwise, EOF is
 * returned.
 */
__export __warn_result __nonnull1
int
ceob(
	const CURSOR *cur);

/*!
 * @function cclose
 * Closes a cursor. No resources are freed by this function.
 *
 * @param cur
 * A pointer to the cursor. Upon return, this will be set to NULL.
 *
 * @discussion
 * If @cur is NULL, this function is a no-op.
 */
__export __nonnull1
void
cclose(
	CURSOR *_Nullable *_Nonnull cur);

/*!
 * @function cclose_check
 * Checks the result of @ceob on a cursor and closes it.
 *
 * @param cur
 * A pointer to the cursor. Upon return, this will be set to NULL.
 *
 * @param ceob_expected
 * The expected result of a call to @ceob. If the result does not match the
 * expected value, the implementation will abort.
 *
 * @discussion
 * If @cur is NULL, this function is a no-op.
 */
__export __nonnull1
void
cclose_check(
	CURSOR *_Nullable * _Nonnull cur,
	int ceob_expected);

__API_HEADER_END(c, nonnull, single)

#endif // __ZX_STD_CURSOR_H
