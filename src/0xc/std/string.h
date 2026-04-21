/*!
 * @copyright
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT license.
 *
 * @header
 * String functions.
 */
#ifndef __ZX_STD_STRING_H
#define __ZX_STD_STRING_H

#include <0xc/std/api.h>
#include <0xc/std/array.h>
#include <0xc/std/pointer.h>
#include <0xc/std/type.h>
#include <0xc/sys/panic.h>

#include <stdlib.h>

// IWYU pragma: begin_exports
#include <string.h>
// IWYU pragma: end_exports

__API_HEADER_BEGIN(c, nonnull, single)

// MARK: Public
/*!
 * @function smemcpy
 * Variant of memcpy(3) which operates on a statically-allocated destination
 * buffer.
 *
 * @param DST
 * The destination buffer.
 *
 * @param SRC
 * The source buffer.
 *
 * @param N
 * The number of bytes to copy. If the destination buffer is smaller than this
 * size, the copy will be truncated.
 *
 * @result
 * The result of the underlying memcpy(3) is returned.
 */
#define smemcpy(DST, SRC, N) ({ \
	size_t n_0xc = (N); \
\
	__assert_static_array(DST, "dst must be statically allocated"); \
	if (sizeof(DST) < (n_0xc)) { \
		n_0xc = sizeof(DST); \
	} \
	memcpy((DST), (SRC), n_0xc); \
})

/*!
 * @function smemset
 * Variant of memset(3) which operates on a statically-allocated destination
 * buffer.
 *
 * @param DST
 * The destination buffer.
 *
 * @param C
 * The value to set.
 *
 * @result
 * The result of the underlying memset(3) is returned.
 */
#define smemset(DST, C) ({ \
	__assert_static_array(DST, "dst must be statically allocated"); \
	memset((DST), (C), sizeof(DST)); \
})

/*!
 * @function omemset
 * Variant of memset(3) which operates on an object.
 *
 * @param DST
 * The object.
 *
 * @param C
 * The value to set.
 *
 * @result
 * The result of the underlying memset(3) is returned.
 */
#define omemset(DST, C) ({ \
	memset(&(DST), (C), sizeof(DST)); \
})

/*!
 * @function memassign
 * Assigns the content of one statically-allocated array to another. Both arrays
 * must be of compatible types and sizes.
 *
 * @param DST
 * The left-hand side of the assignment.
 *
 * @param SRC
 * The right-hand side of the assignment.
 *
 * @result
 * The result of the underlying memcpy(3) is returned.
 */
#define memassign(DST, SRC) ({ \
	__assert_static_array(DST, "dst must be statically allocated"); \
	__assert_static_array(SRC, "src must be statically allocated"); \
	_Static_assert(sizeof(DST) >= sizeof(SRC), \
			"dst must be large enough to hold src"); \
	memcpy((DST), (SRC), sizeof(DST)); \
})

/*!
 * @function strscpy
 * Variant of strcpy(3) which operates on a statically-allocated destination
 * buffer.
 *
 * @param DST
 * The destination buffer.
 *
 * @param SRC
 * The source string.
 *
 * @result
 * The result of the underlying strlcpy(3) is returned.
 */
#define strscpy(DST, SRC) ({ \
	__assert_static_array(DST, "dst must be statically allocated"); \
	strlcpy((DST), (SRC), sizeof(DST)); \
})

/*!
 * @function strslen
 * Variant of strlen(3) which returns the length of a string literal or
 * character array.
 *
 * @param S
 * The string literal.
 *
 * @result
 * The length of the string literal. This macro will abort if the string is not
 * null-terminated.
 *
 * @discussion
 * This macro cannot be used with a pointer which has been assigned the value of
 * a string literal; it must be used with an array type to impose the correct
 * boundary on the underlying strnlen(3) call.
 *
 * Note that because this macro still performs an underlying call to strnlen(3),
 * the following works correctly:
 *
 *     char s[64] = "hello world";
 *     size_t len = strslen(s); // len == 11
 */
#define strslen(S) ({ \
	size_t len_0xc = 0; \
\
	__assert_type_compat(typeof(S), char [], "string must be a char"); \
	__assert_static_array(S, "string must be statically allocated"); \
\
	len_0xc = strnlen(S, sizeof(S)); \
	if (len_0xc == sizeof(S)) { \
		panicx("string not null-terminated: buffer size = %zu", sizeof(S)); \
	} \
	(len_0xc); \
})

/*!
 * @function cstr
 * Convenience macro which obtains a null-terminated pointer to a statically-
 * allocated buffer.
 *
 * @param S
 * The statically-allocated buffer.
 *
 * @result
 * A null-terminated pointer to the buffer.
 */
#define cstr(S) __forge(char *, (S), zterm)

__API_HEADER_END(c, nonnull, single)

#endif // __ZX_STD_STRING_H
