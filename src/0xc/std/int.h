/*!
 * @copyright
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT license.
 *
 * @header
 * Integer conversions.
 */
#ifndef __ZX_STD_INT_H
#define __ZX_STD_INT_H

#include <0xc/std/api.h>
#include <0xc/std/type.h>
#include <0xc/sys/panic.h>

/*!
 * @include
 * We'd expect that exporting stdint.h via IWYU would correctly propagate for
 * uses of standard integer types, e.g. uint32_t, but it appears that IWYU
 * doesn't handle this case properly. So to ensure that files including this
 * header get the necessary definitions, we export stdio.h, which appears to do
 * the trick.
 *
 * Unfortunately, we cannot include stddef.h in this to give access to size_t
 * because IWYU appears to have a hard-coded rule that prevents re-exporting
 * that (and other) fundamental C types.
 *
 * We don't use DECL_TYPE_4IWYU here because IWYU will flag this header as
 * unused if that's the only thing being brought in (since it really feels that
 * size_t should come from elsewhere).
 */
// IWYU pragma: begin_exports
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <limits.h>

#if __has_include(<sys/types.h>)
#include <sys/types.h>
DECL_TYPE_4IWYU(ssize_t);
#endif
// IWYU pragma: end_exports

__API_HEADER_BEGIN(c, nonnull, single)

// MARK: Shims
#if !defined(_SSIZE_T) && \
	!defined(_SSIZE_T_DEFINED) && \
	!defined(_SSIZE_T_DECLARED)
typedef long ssize_t;
#endif

// MARK: Types
/*!
 * @internal integer_union_t
 * @internal integer_standard_union_t(T)
 * Internal implementation details.
 */
#define integer_union_t(T) \
	union { \
		T v; \
		unsigned T vu; \
	}

#define integer_standard_union_t(T) \
	union { \
		T v; \
		u ## T vu; \
	}

/*!
 * @typedef char_union_t
 * @typedef short_union_t
 * @typedef int_union_t
 * @typedef long_union_t
 * @typedef long_long_union_t
 * Union types for accessing integer values as either signed or unsigned.
 *
 * @field v
 * The signed integer value.
 *
 * @field vu
 * The unsigned integer value.
 */
typedef integer_union_t(char) char_union_t;
typedef integer_union_t(short) short_union_t;
typedef integer_union_t(int) int_union_t;
typedef integer_union_t(long) long_union_t;
typedef integer_union_t(long long) long_long_union_t;
typedef integer_standard_union_t(int8_t) int8_union_t;
typedef integer_standard_union_t(int16_t) int16_union_t;
typedef integer_standard_union_t(int32_t) int32_union_t;
typedef integer_standard_union_t(int64_t) int64_union_t;

/*!
 * @typedef integer_value_t
 * A union type for accessing integer values of various sizes.
 *
 * @field iv_char
 * The 8-bit integer value.
 *
 * @field iv_short
 * The short integer value.
 *
 * @field iv_int
 * The standard integer value.
 *
 * @field iv_long
 * The platform-sized integer value.
 *
 * @field iv_long_long
 * The largest integer value.
 *
 * @field iv_int8
 * The 8-bit integer value.
 *
 * @field iv_int16
 * The 16-bit integer value.
 *
 * @field iv_int32
 * The 32-bit integer value.
 *
 * @field iv_int64
 * The 64-bit integer value.
 */
typedef union _integer_value {
	char_union_t iv_char;
	short_union_t iv_short;
	int_union_t iv_int;
	long_union_t iv_long;
	long_long_union_t iv_long_long;
	int8_union_t iv_int8;
	int16_union_t iv_int16;
	int32_union_t iv_int32;
	int64_union_t iv_int64;
} integer_value_t;

/*!
 * @typedef byte_order_t
 * An enumeration representing byte ordering.
 *
 * @const BYTE_ORDER_BIG
 * Big endian byte ordering (also known as network byte ordering).
 *
 * @const BYTE_ORDER_LITTLE
 * Little endian byte ordering.
 */
__typed_enum(byte_order, uint8_t, closed,
	BYTE_ORDER_BIG,
	BYTE_ORDER_LITTLE,
	_BYTE_ORDER_CNT,
);

/*!
 * @const BYTE_ORDER_HOST
 * The byte ordering of the host system.
 */
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#define BYTE_ORDER_HOST BYTE_ORDER_BIG
#elif __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#define BYTE_ORDER_HOST BYTE_ORDER_LITTLE
#else
#error "unknown host byte order"
#endif

/*!
 * @const BYTE_ORDER_ANTI_HOST
 * The opposite of the byte ordering of the host system.
 */
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#define BYTE_ORDER_ANTI_HOST BYTE_ORDER_LITTLE
#elif __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#define BYTE_ORDER_ANTI_HOST BYTE_ORDER_BIG
#else
#error "unknown host byte order"
#endif

// MARK: Public
/*!
 * @function __cast_unsigned_unsigned
 * Cast from an unsigned integer value to a lower-width unsigned integer value.
 *
 * @param T
 * The integer type to cast to.
 *
 * @param V
 * The value to cast.
 *
 * @result
 * The input value @V as a @T.
 */
#define __cast_unsigned_unsigned(T, V) _Generic(((T)0), \
	unsigned char : __cast_unsigned_unsigned_char, \
	unsigned short int : __cast_unsigned_unsigned_short, \
	unsigned int : __cast_unsigned_unsigned_int, \
	unsigned long int : __cast_unsigned_unsigned_long, \
	unsigned long long int : __cast_unsigned_unsigned_long_long \
)(V, #T)

/*!
 * @function __cast_unsigned_signed
 * Cast from an unsigned integer value to a signed integer value.
 *
 * @param T
 * The integer type to cast to.
 *
 * @param V
 * The value to cast.
 *
 * @result
 * The input value @V as a @T.
 */
#define __cast_unsigned_signed(T, V) _Generic(((T)0), \
	char : __cast_unsigned_signed_char, \
	short int : __cast_unsigned_signed_short, \
	int : __cast_unsigned_signed_int, \
	long int : __cast_unsigned_signed_long, \
	long long int : __cast_unsigned_signed_long_long \
)(V, #T)

/*!
 * @function __cast_signed_signed
 * Cast from a signed integer value to a lower-width signed integer value.
 *
 * @param T
 * The integer type to cast to.
 *
 * @param V
 * The value to cast.
 *
 * @result
 * The input value @V as a @T.
 */
#define __cast_signed_signed(T, V) _Generic(((T)0), \
	char : __cast_signed_signed_char, \
	short int : __cast_signed_signed_short, \
	int : __cast_signed_signed_int, \
	long int : __cast_signed_signed_long, \
	long long int : __cast_signed_signed_long_long \
)(V, #T)

/*!
 * @function __cast_signed_unsigned
 * Cast from a signed integer value to an unsigned integer value.
 *
 * @param T
 * The integer type to cast to.
 *
 * @param V
 * The value to cast.
 *
 * @result
 * The input value @V as a @T.
 */
#define __cast_signed_unsigned(T, V) _Generic(((T)0), \
	unsigned char : __cast_signed_unsigned_char, \
	unsigned short int : __cast_signed_unsigned_short, \
	unsigned int : __cast_signed_unsigned_int, \
	unsigned long int : __cast_signed_unsigned_long, \
	unsigned long long int : __cast_signed_unsigned_long_long \
)(V, #T)

// MARK: Byte Ordering
/*!
 * @function __swap_byte_order
 * Swaps the byte ordering of an integer of defined width.
 *
 * @param V
 * The integer to swap.
 *
 * @result
 * The swapped integer.
 *
 * @discussion
 * If the __builtin_bswap*() family of intrinsics are not available, the
 * implementation will substitute them with its own implementations. These
 * implementations cannot be const-folded by the compiler, and therefore they
 * cannot be used to statically initialize global objects.
 */
#define __swap_byte_order(V) \
	(typeof(V))_Generic((V), \
		uint8_t : (uint8_t)(V), \
		uint16_t : __builtin_bswap16((uint16_t)V), \
		uint32_t : __builtin_bswap32((uint32_t)V), \
		uint64_t : __builtin_bswap64((uint64_t)V), \
		int8_t : (uint8_t)(V), \
		int16_t : __builtin_bswap16((uint16_t)V), \
		int32_t : __builtin_bswap32((uint32_t)V), \
		int64_t : __builtin_bswap64((uint64_t)V) \
	)

/*!
 * @function n2h
 * Swaps the integer from network byte ordering to host byte ordering.
 *
 * @param V
 * The integer.
 *
 * @result
 * The integer represented in host byte ordering.
 */
#if defined(__LITTLE_ENDIAN__)
#define n2h(V) __swap_byte_order(V)
#else
#define n2h(V) (V)
#endif

/*!
 * @function h2n
 * Swaps the integer from host byte ordering to network byte ordering.
 *
 * @param V
 * The integer.
 *
 * @result
 * The integer represented in network byte ordering.
 */
#if defined(__LITTLE_ENDIAN__)
#define h2n(V) __swap_byte_order(V)
#else
#define h2n(V) (V)
#endif

/*!
 * @function n2u
 * Swaps the given integer from network byte ordering to little endian byte
 * ordering.
 *
 * @param V
 * The integer.
 *
 * @result
 * The integer represented in little endian byte ordering.
 */
#if defined(__BIG_ENDIAN__)
#define n2u(V) __swap_byte_order(V)
#else
#define n2u(V) (V)
#endif

/*!
 * @function h2u
 * Swaps the given integer from host byte ordering to little endian byte
 * ordering.
 *
 * @param V
 * The integer.
 *
 * @result
 * The integer represented in little endian byte ordering.
 */
#if defined(__BIG_ENDIAN__)
#define h2u(V) __swap_byte_order(V)
#else
#define h2u(V) (V)
#endif

/*!
 * @function u2h
 * Swaps the given integer from little endian byte ordering to host byte
 * ordering.
 *
 * @param V
 * The integer.
 *
 * @result
 * The integer represented in host byte ordering.
 */
#if defined(__BIG_ENDIAN__)
#define u2h(V) __swap_byte_order(V)
#else
#define u2h(V) (V)
#endif

/*!
 * @function memtoh
 * Interprets the given memory according to the specified byte ordering and
 * returns the resulting integer.
 *
 * @param T
 * The integer type to interpret the memory as.
 *
 * @param O
 * The byte ordering of the memory indicated by @P.
 *
 * @param P
 * The memory.
 *
 * @param L
 * The length of the memory referred to by @P.
 *
 * @result
 * The integer represented in host byte ordering.
 */
#define memtoh(T, O, P, L) (T)({ \
	T v_0xc = 0; \
\
	if ((L) < sizeof(T)) { \
		panicx("insufficient memory to interpret '" #T "': " \
				"length = %zu, expected >= %zu", \
				(L), sizeof(T)); \
	} \
\
	v_0xc = *(const T *)(P); \
	switch (O) { \
	case BYTE_ORDER_BIG: \
		v_0xc = n2h(v_0xc); \
		break; \
	case BYTE_ORDER_LITTLE: \
		v_0xc = u2h(v_0xc); \
		break; \
	default: \
		panic_switch(O); \
	} \
	(v_0xc); \
})

/*!
 * @internal
 * Implementation details.
 */
// MARK: Internal
__inline_impl __pure_const __warn_result __nonnull2
static inline unsigned char
__cast_unsigned_unsigned_char(
		uintmax_t v,
		const char *__cstr type)
{
	if (v > UCHAR_MAX) {
		panic_overflow(type, v, UCHAR_MAX);
	}
	return (unsigned char)v;
}

__inline_impl __pure_const __warn_result __nonnull2
static inline unsigned short int
__cast_unsigned_unsigned_short(
		uintmax_t v,
		const char *__cstr type)
{
	if (v > USHRT_MAX) {
		panic_overflow(type, v, USHRT_MAX);
	}
	return (unsigned short int)v;
}

__inline_impl __pure_const __warn_result __nonnull2
static inline unsigned int
__cast_unsigned_unsigned_int(
		uintmax_t v,
		const char *__cstr type)
{
	if (v > UINT_MAX) {
		panic_overflow(type, v, UINT_MAX);
	}
	return (unsigned int)v;
}

__inline_impl __pure_const __warn_result __nonnull2
static inline unsigned long int
__cast_unsigned_unsigned_long(
		uintmax_t v,
		const char *__cstr type)
{
	if (v > ULONG_MAX) {
		panic_overflow(type, v, ULONG_MAX);
	}
	return (unsigned long int)v;
}

__inline_impl __pure_const __warn_result __nonnull2
static inline unsigned long long int
__cast_unsigned_unsigned_long_long(
		uintmax_t v,
		const char *__cstr type)
{
	if (v > ULLONG_MAX) {
		panic_overflow(type, v, ULLONG_MAX);
	}
	return (unsigned long long int)v;
}

// MARK: -
__inline_impl __pure_const __warn_result __nonnull2
static inline char
__cast_unsigned_signed_char(
		uintmax_t v,
		const char *__cstr type)
{
	if (v > CHAR_MAX) {
		panic_overflow(type, v, CHAR_MAX);
	}
	return (char)v;
}

__inline_impl __pure_const __warn_result __nonnull2
static inline short
__cast_unsigned_signed_short(
		uintmax_t v,
		const char *__cstr type)
{
	if (v > SHRT_MAX) {
		panic_overflow(type, v, SHRT_MAX);
	}
	return (short int)v;
}

__inline_impl __pure_const __warn_result __nonnull2
static inline int
__cast_unsigned_signed_int(
		uintmax_t v,
		const char *__cstr type)
{
	if (v > INT_MAX) {
		panic_overflow(type, v, INT_MAX);
	}
	return (int)v;
}

__inline_impl __pure_const __warn_result __nonnull2
static inline long int
__cast_unsigned_signed_long(
		uintmax_t v,
		const char *__cstr type)
{
	if (v > LONG_MAX) {
		panic_overflow(type, v, LONG_MAX);
	}
	return (long)v;
}

__inline_impl __pure_const __warn_result __nonnull2
static inline long long int
__cast_unsigned_signed_long_long(
		uintmax_t v,
		const char *__cstr type)
{
	if (v > LLONG_MAX) {
		panic_overflow(type, v, LLONG_MAX);
	}
	return (long long int)v;
}

// MARK: -
__inline_impl __pure_const __warn_result __nonnull2
static inline char
__cast_signed_signed_char(
		intmax_t v,
		const char *__cstr type)
{
	if (v > CHAR_MAX) {
		panic_overflow(type, v, CHAR_MAX);
	}
	if (v < CHAR_MIN) {
		panic_underflow(type, v, CHAR_MIN);
	}
	return (char)v;
}

__inline_impl __pure_const __warn_result __nonnull2
static inline short int
__cast_signed_signed_short(
		intmax_t v,
		const char *__cstr type)
{
	if (v > SHRT_MAX) {
		panic_overflow(type, v, SHRT_MAX);
	}
	if (v < SHRT_MIN) {
		panic_underflow(type, v, SHRT_MIN);
	}
	return (short int)v;
}

__inline_impl __pure_const __warn_result __nonnull2
static inline int
__cast_signed_signed_int(
		intmax_t v,
		const char *__cstr type)
{
	if (v > INT_MAX) {
		panic_overflow(type, v, INT_MAX);
	}
	if (v < INT_MIN) {
		panic_underflow(type, v, INT_MIN);
	}
	return (int)v;
}

__inline_impl __pure_const __warn_result __nonnull2
static inline long int
__cast_signed_signed_long(
		intmax_t v,
		const char *__cstr type)
{
	if (v > LONG_MAX) {
		panic_overflow(type, v, LONG_MAX);
	}
	if (v < LONG_MIN) {
		panic_underflow(type, v, LONG_MIN);
	}
	return (long int)v;
}

__inline_impl __pure_const __warn_result __nonnull2
static inline long long int
__cast_signed_signed_long_long(
		intmax_t v,
		const char *__cstr type)
{
	if (v > LLONG_MAX) {
		panic_overflow(type, v, LLONG_MAX);
	}
	if (v < LLONG_MIN) {
		panic_underflow(type, v, LLONG_MIN);
	}
	return (long long int)v;
}

// MARK: -
__inline_impl __pure_const __warn_result __nonnull2
static inline unsigned char
__cast_signed_unsigned_char(
		intmax_t v,
		const char *__cstr type)
{
	if (v > CHAR_MAX) {
		panic_overflow(type, v, CHAR_MAX);
	}
	if (v < 0) {
		panic_underflow(type, v, 0);
	}
	return (unsigned char)v;
}

__inline_impl __pure_const __warn_result __nonnull2
static inline unsigned short int
__cast_signed_unsigned_short(
		intmax_t v,
		const char *__cstr type)
{
	if (v > SHRT_MAX) {
		panic_overflow(type, v, SHRT_MAX);
	}
	if (v < 0) {
		panic_underflow(type, v, 0);
	}
	return (unsigned short)v;
}

__inline_impl __pure_const __warn_result __nonnull2
static inline unsigned int
__cast_signed_unsigned_int(
		intmax_t v,
		const char *__cstr type)
{
	if (v > INT_MAX) {
		panic_overflow(type, v, INT_MAX);
	}
	if (v < 0) {
		panic_underflow(type, v, 0);
	}
	return (unsigned int)v;
}

__inline_impl __pure_const __warn_result __nonnull2
static inline unsigned long int
__cast_signed_unsigned_long(
		intmax_t v,
		const char *__cstr type)
{
	if (v > LONG_MAX) {
		panic_overflow(type, v, LONG_MAX);
	}
	if (v < 0) {
		panic_underflow(type, v, 0);
	}
	return (unsigned long)v;
}

__inline_impl __pure_const __warn_result __nonnull2
static inline unsigned long long int
__cast_signed_unsigned_long_long(
		intmax_t v,
		const char *__cstr type)
{
	if (v < 0) {
		panic_underflow(type, v, 0);
	}
	return (unsigned long long)v;
}

__inline_impl __pure_const __warn_result
static inline uint8_t
__swap_u8(
		uint8_t v)
{
	return v;
}

#if !__has_builtin(__builtin_bswap16)
#define swap_union_t(T) \
	union { \
		T v; \
		uint8_t v_addressible[sizeof(T)]; \
	}

__inline_impl __pure_const __warn_result
static inline uint16_t
__builtin_bswap16(
		uint16_t v)
{
	size_t i = 0;
	size_t j = sizeof(v) - 1;
	swap_union_t(uint16_t) v_orig = {
		.v = v,
	};
	swap_union_t(uint16_t) v_swapped = {
		.v = v,
	};

	for (i = 0; i < sizeof(v); i++, j--) {
		v_swapped.v_addressible[j] = v_orig.v_addressible[i];
	}
	return v_swapped.v;
}
#endif

#if !__has_builtin(__builtin_bswap32)
__inline_impl __pure_const __warn_result
static inline uint32_t
__builtin_bswap32(
		uint32_t v)
{
	size_t i = 0;
	size_t j = sizeof(v) - 1;
	swap_union_t(uint32_t) v_orig = {
		.v = v,
	};
	swap_union_t(uint32_t) v_swapped = {
		.v = v,
	};

	for (i = 0; i < sizeof(v); i++, j--) {
		v_swapped.v_addressible[j] = v_orig.v_addressible[i];
	}
	return v_swapped.v;
}
#endif

#if !__has_builtin(__builtin_bswap64)
__inline_impl __pure_const __warn_result
static inline uint64_t
__builtin_bswap64(
		uint64_t v)
{
	size_t i = 0;
	size_t j = sizeof(v) - 1;
	swap_union_t(uint64_t) v_orig = {
		.v = v,
	};
	swap_union_t(uint64_t) v_swapped = {
		.v = v,
	};

	for (i = 0; i < sizeof(v); i++, j--) {
		v_swapped.v_addressible[j] = v_orig.v_addressible[i];
	}
	return v_swapped.v;
}
#endif

__API_HEADER_END(c, nonnull, single)

#endif // __ZX_STD_INT_H
