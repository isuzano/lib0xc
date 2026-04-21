/*!
 * @copyright
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT license.
 *
 * @header
 * Standard input/output.
 */
#ifndef __ZX_STD_IO_H
#define __ZX_STD_IO_H

#include <0xc/std/api.h>
#include <0xc/std/type.h>
#include <0xc/std/array.h>
#include <0xc/std/int.h>
#include <0xc/sys/errno.h>

#include <stdarg.h>

// IWYU pragma: begin_exports
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
// IWYU pragma: end_exports

__API_HEADER_BEGIN(c, nonnull, single)

// MARK: Definitions
#define DIGEST_BYTES_CNT (64u)
#define DIGEST_STRING_SIZE ((DIGEST_BYTES_CNT * 2) + 1)

// MARK: Types
/*!
 * @typedef digest_bytes_t
 * Bytes representing a digest.
 */
typedef uint8_t digest_bytes_t[DIGEST_BYTES_CNT];

/*!
 * @typedef digest_string_t
 * A digest represented as a string of hexadecimal numbers.
 */
typedef char digest_string_t[DIGEST_STRING_SIZE];

// MARK: Public
/*!
 * @section
 * Family of functions which converts the given integer to the platform's
 * representation of standard C integer types. These are meant to be used in
 * conjunction with printf(3), which does not have recognized specifiers for
 * defined-width integer types (e.g. uint32_t). Using such integers with
 * printf(3) in cross-platform code can often by painful, as different platforms
 * may have different widths for e.g. the 'int' type which is specified by the
 * '%d' specifier.
 *
 * Rather than managing different format strings per platform (or use awkward
 * macros to insert the correct formatting specifier), these macros operate on
 * the argument to the string to convert it to the appropriate representation
 * using the macros in 0xc/std/int.h.
 *
 * The "oxo" prefix can be read as the '%' character.
 *
 * @example
 *
 *     int32_t n = 255;
 *     size_t d = LONG_MAX;
 *     printf("statistics: n = %d, d = %lu", oxod(n), oxolu(d));
 */
// MARK: printf(3) Integer Specifiers
/*!
 * @function oxohu
 * @function oxou
 * @function oxolu
 * @function oxollu
 * These functions convert the given value to the corresponding type implied by
 * the formatting specifier indicated by their names, e.g. 'oxohd' will conver
 * the value to an 'unsigned short'.
 *
 * @param V
 * The value to convert.
 *
 * @result
 * The value as the named type.
 *
 * @discussion
 * If @V cannot be represented in the desired type, the implementation will
 * abort.
 */
#define oxohhu(V) __hlzux(unsigned char, V)
#define oxohu(V) __hlzux(unsigned short int, V)
#define oxou(V) __hlzux(unsigned int, V)
#define oxolu(V) __hlzux(unsigned long, V)
#define oxollu(V) __hlzux(unsigned long long, V)

/*!
 * @function oxohd
 * @function oxod
 * @function oxold
 * @function oxolld
 * These functions convert the given value to the corresponding type implied by
 * the formatting specifier indicated by their names, e.g. 'oxold' will convert
 * the value to a 'signed long'.
 *
 * @param V
 * The value to convert.
 *
 * @result
 * The value as the named type.
 *
 * @discussion
 * If @V cannot be represented in the desired type, the implementation will
 * halt.
 */
#define oxohhd(V) __hlzd(signed char, V)
#define oxohd(V) __hlzd(signed short int, V)
#define oxod(V) __hlzd(signed int, V)
#define oxold(V) __hlzd(signed long, V)
#define oxolld(V) __hlzd(signed long long, V)

/*!
 * @function oxozu
 * @function oxozd
 * These functions convert the given value to the corresponding type implied by
 * the formatting specifier indicated by their names, e.g. 'oxozu' will conver
 * the value to a 'size_t'.
 *
 * @param V
 * The value to convert.
 *
 * @result
 * The value as the named type.
 *
 * @discussion
 * If @V cannot be represented in the desired type, the implementation will
 * abort.
 */
#define oxozu(V) __hlzux(size_t, V)
#define oxozd(V) __hlzd(ssize_t, V)

/*!
 * @function sprintf_check_and_consume
 * Checks the return value from snprintf(3)-style functions, increments a buffer
 * pointer, and decrements a size. This routine is intended to be used in simple
 * printing flows where truncation is acceptable.
 *
 * @param s
 * The buffer pointer.
 *
 * @param len
 * A pointer to the length of the memory referred to by @s. On return, this
 * value will be decremented by the difference between @s and the return value
 * of the function.
 *
 * @param n
 * The return value from a call to an snprintf(3)-style function.
 *
 * @param t
 * On return, will be incremented by @n. May be NULL. This is primarily useful
 * when implementing snprintf(3)-style functions which return the total number
 * of characters required for a formatted string in cases where the provided
 * buffer is too small.
 *
 * @result
 * If @n does not indicate an error or truncation, the value @s will be
 * incremented by @n and returned. If @n indicates that the formatted string was
 * too large for the buffer, @s will be incremented by @len and returned. If an
 * error occurred, e.g. a wide character was encountered, @s will be incremented
 * by @n and returned.
 *
 * @example
 *
 *     char buff[128];
 *     size_t len = sizeof(buff);
 *     char *p = buff;
 *     int n = 0;
 *
 *     n = snprintf(p, len, "Hello, %s!", "world");
 *     p = sprintf_check_and_consume(p, &len, n, NULL);
 *
 *     n = snprintf(p, len, " Here is a number: %llx", ULLONG_MAX);
 *     p = sprintf_check_and_consume(p, &len, n, NULL);
 *
 *     // If the string overflows the buffer at this point, snprintf(3) will not
 *     // write the complete string and will return a value larger than 'len'.
 *     // In this case, @sprintf_check_and_consume will move 'p' to the end of
 *     // the buffer and set 'len' to zero.
 *     n = snprintf(p, len, " Maybe this will overflow,");
 *     p = sprintf_check_and_consume(p, &len, n, NULL);
 *
 *     // And so when we get here, we'll give snprintf(3) a zero-length buffer,
 *     // which is completely safe.
 *     n = snprintf(p, len, " maybe it won't");
 *     p = sprintf_check_and_consume(p, &len, n, NULL);
 */
__export __warn_result __nonnull1 __nonnull2
char *__sized_by(*len)
sprintf_check_and_consume(
	char *__sized_by(*len) s,
	size_t *len,
	int n,
	int *_Nullable t);

/*!
 * @function ssprintf
 * Print a formatted string to a statically-sized string.
 *
 * @param S
 * The destination buffer.
 *
 * @param FMT
 * The printf(3)-like format string.
 *
 * @param ...
 * The arguments corresponding to @FMT.
 */
#define ssprintf(S, FMT, ...) \
	__assert_type_compat(typeof(S), char [], "string must be a char"); \
	__assert_static_array(S, "string must be statically allocated"); \
	snprintf((S), sizeof(S), FMT, ## __VA_ARGS__)

/*!
 * @function vssprintf
 * Print a formatted string to a statically-sized string.
 *
 * @param S
 * The destination buffer.
 *
 * @param FMT
 * The printf(3)-like format string.
 *
 * @param AP
 * The arguments corresponding to @FMT.
 */
#define vssprintf(S, FMT, AP) \
	__assert_type_compat(typeof(S), char [], "string must be a char"); \
	__assert_static_array(S, "string must be statically allocated"); \
	vsnprintf((S), sizeof(S), FMT, AP)

/*!
 * @function snprintdgst
 * Prints a digest to the given buffer. The bytes are printed as lower-case
 * hexadecimal characters.
 *
 * @param S
 * The destination buffer.
 *
 * @param DGST
 * A pointer to the digest bytes.
 *
 * @param LEN
 * The length of the bytes referred to by @DGST.
 */
__export __nonnull1 __nonnull2
void
snprintdgst(
	__typed_array(digest_string_t, _Nonnull, S),
	const uint8_t *__counted_by(LEN) DGST,
	size_t LEN);

/*!
 * @function sprintdgst
 * Variant of @sprintdgst which prints the digest to a statically-allocated
 * buffer.
 *
 * @param S
 * The destination buffer.
 *
 * @param DGST
 * The digest buffer.
 */
#define sprintdgst(S, DGST) \
	__assert_type_compat(typeof(S), char [], "string must be a char"); \
	__assert_static_array(S, "string must be statically allocated"); \
	_Static_assert(sizeof(DGST) <= sizeof(digest_bytes_t), \
			"digest buffer must not exceed 64 bytes"); \
	snprintdgst((S), (DGST), sizeof(DGST))

/*!
 * @function snprintmem
 * Prints a memory buffer to the given string buffer. The bytes are printed as
 * lower-case hexadecimal characters.
 *
 * @param S
 * The destination buffer.
 *
 * @param LEN
 * The length of the buffer referred to by @S. This includes the null
 * terminator, similar to snprintf(3).
 *
 * @param P
 * A pointer to the memory to print.
 *
 * @param P_LEN
 * The length of the buffer referred to by @P.
 */
__export __nonnull1 __nonnull3
void
snprintmem(
	char *__counted_by(LEN) S,
	size_t LEN,
	const uint8_t *__counted_by(P_LEN) P,
	size_t P_LEN);

/*!
 * @function ssprintmem
 * Variant of @snprintmem which prints the bytes to a statically-allocated
 * buffer.
 *
 * @param S
 * The destination buffer.
 *
 * @param P
 * The memory buffer to print.
 *
 * @param P_LEN
 * The length of the buffer referred to by @P.
 */
#define ssprintmem(S, P, P_LEN) \
	__assert_type_compat(typeof(S), char [], "string must be a char"); \
	__assert_static_array(S, "string must be statically allocated"); \
	snprintmem((S), sizeof(S), (P), (P_LEN))

/*!
 * @internal
 * Implementation details.
 */
// MARK: Internal
#define __hlzux(T, V) (T)_Generic(((T)0), \
	unsigned char : __cast_unsigned_unsigned_char(V, #T), \
	unsigned short int : __cast_unsigned_unsigned_short(V, #T), \
	unsigned int : __cast_unsigned_unsigned_int(V, #T), \
	unsigned long int : __cast_unsigned_unsigned_long(V, #T), \
	unsigned long long int : __cast_unsigned_unsigned_long_long(V, #T) \
)

#define __hlzd(T, V) (T)_Generic(((T)0), \
	signed char : __cast_signed_signed_char(V, #T), \
	signed short int : __cast_signed_signed_short(V, #T), \
	signed int : __cast_signed_signed_int(V, #T), \
	signed long int : __cast_signed_signed_long(V, #T), \
	signed long long int : __cast_signed_signed_long_long(V, #T) \
)

__API_HEADER_END(c, nonnull, single)

#endif // __ZX_STD_IO_H
