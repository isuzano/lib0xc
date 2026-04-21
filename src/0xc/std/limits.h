/*!
 * @copyright
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT license.
 *
 * @header
 * Type limits.
 */
#ifndef __ZX_STD_LIMITS_H
#define __ZX_STD_LIMITS_H

#include <0xc/std/api.h>
#include <0xc/std/defs.h>
#include <0xc/std/int.h>
#include <0xc/sys/panic.h>

// IWYU pragma: begin_exports
#include <limits.h>
#include <stdint.h>
// IWYU pragma: end_exports

__API_HEADER_BEGIN(c, nonnull, single)

/*!
 * @const SSIZE_MAX
 * The maximum value of a ssize_t.
 */
#if !defined(SSIZE_MAX)
#define SSIZE_MAX INT_MAXOF(ssize_t)
#endif

/*!
 * @const SSIZE_MIN
 * The minimum value of a ssize_t.
 */
#if !defined(SSIZE_MIN)
#define SSIZE_MIN INT_MINOF(ssize_t)
#endif

/*!
 * @function INT_MAXOF
 * Returns the maximum value for an integer type.
 *
 * @param T
 * The integer type.
 *
 * @result
 * The maximum value for the given integer type.
 */
#define INT_MAXOF(T) _Generic((T)0, \
	signed char : CHAR_MAX, \
	signed short : SHRT_MAX, \
	signed int : INT_MAX, \
	signed long : LONG_MAX, \
	signed long long : LLONG_MAX, \
	unsigned char : UCHAR_MAX, \
	unsigned short : USHRT_MAX, \
	unsigned int : UINT_MAX, \
	unsigned long : ULONG_MAX, \
	unsigned long long : ULLONG_MAX \
)

/*!
 * @function INT_MINOF
 * Returns the minimum value for an integer type.
 *
 * @param T
 * The integer type.
 *
 * @result
 * The minimum value for the given integer type.
 */
#define INT_MINOF(T) _Generic((T)0, \
	char : CHAR_MIN, \
	short : SHRT_MIN, \
	int : INT_MIN, \
	long : LONG_MIN, \
	long long : LLONG_MIN, \
	_Generic_match_unsigned_int(0) \
)

/*!
 * @function WIDTHOF
 * Returns the width in bits of a type.
 *
 * @param T
 * The type.
 *
 * @result
 * The width in bits of the type.
 */
#define WIDTHOF(T) (sizeof(T) * CHAR_BIT)

// MARK: Public
/*!
 * @function alignto
 * Aligns a value up to the nearest multiple of a specified alignment.
 *
 * @param VAL
 * The value to align. May be an integer or pointer type.
 *
 * @param ALIGN
 * The alignment to align to. Must be a power of two.
 *
 * @result
 * The smallest value greater than or equal to @VAL that is a multiple of
 * @ALIGN.
 */
#define alignto(VAL, ALIGN) ({ \
	_Static_assert(sizeof(uintptr_t) <= sizeof(uintmax_t), \
			"uintmax_t must be at least as wide as uintptr_t"); \
	if (!__compiler_const(ALIGN) && ((ALIGN) & ((ALIGN) - 1)) != 0) { \
		panicx("alignment must be a power of two: %llx", \
				(unsigned long long)(ALIGN)); \
	} \
	_Static_assert(!__compiler_const(ALIGN) || \
			(((ALIGN) & ((ALIGN) - 1)) == 0), \
			"alignment must be a power of two"); \
	_Generic((VAL), \
		_Generic_match_unsigned_int(__alignto_uint), \
		_Generic_match_signed_int(__alignto_int), \
		default: __alignto_ptr \
	)(VAL, (uintmax_t)(ALIGN)); \
})

/*!
 * @function alignto_unsafe
 * Variant of @alignto which does not verify that the alignment is a power of
 * two at runtime. This macro is useable at file scope since it does not contain
 * any statements.
 *
 * @param VAL
 * The value to align.
 *
 * @param ALIGN
 * The alignment to align to.
 *
 * @result
 * The smallest value greater than or equal to @VAL that is a multiple of
 * @ALIGN, assuming that @ALIGN is a power of two.
 */
#define alignto_unsafe(VAL, ALIGN) \
	((((VAL) + ((ALIGN) - 1)) & ~((ALIGN) - 1)))

/*!
 * @function chkalign
 * Checks if a value is aligned to a specified alignment.
 *
 * @param VAL
 * The value to check.
 *
 * @param ALIGN
 * The alignment to check against. Must be a power of two.
 *
 * @result
 * True if @VAL is a multiple of @ALIGN, false otherwise.
 */
#define chkalign(VAL, ALIGN) ({ \
	_Static_assert(sizeof(uintptr_t) <= sizeof(uintmax_t), \
			"uintmax_t must be at least as wide as uintptr_t"); \
	_Static_assert(!__compiler_const(ALIGN) || \
			(((ALIGN) & ((ALIGN) - 1)) == 0), \
			"alignment must be a power of two"); \
	if (!__compiler_const(ALIGN) && ((ALIGN) & ((ALIGN) - 1)) != 0) { \
		panicx("alignment must be a power of two: %llx", \
				(unsigned long long)(ALIGN)); \
	} \
	_Generic((VAL), \
		_Generic_match_unsigned_int(__chkalign_uint), \
		_Generic_match_signed_int(__chkalign_int), \
		default: __chkalign_ptr \
	)(VAL, (uintmax_t)(ALIGN)); \
})

/*!
 * @internal
 * Implementation details.
 */
#define __alignto_arith(VAL, ALIGN) \
	(((VAL) + ((ALIGN) - 1)) & ~((ALIGN) - 1))

__inline_impl __warn_result
static inline uintmax_t
__alignto_uint(
		uintmax_t val,
		uintmax_t align)
{
	return __alignto_arith(val, align);
}

__inline_impl __warn_result
static inline intmax_t
__alignto_int(
		intmax_t val,
		uintmax_t align)
{
	uintmax_t val_unsigned = __cast_signed_unsigned(uintmax_t, val);
	return (intmax_t)__alignto_arith(val_unsigned, align);
}

__inline_impl __warn_result
static inline void *__unsafe_indexable
__alignto_ptr(
		const void *__unsafe_indexable _Nullable val,
		uintmax_t align)
{
	uintmax_t aligned = 0;
	uintptr_t aligned_addr = 0;

	aligned = __alignto_arith((uintptr_t)val, align);
	aligned_addr = __cast_unsigned_unsigned(uintptr_t, aligned);
	return (void *__unsafe_indexable)aligned_addr;
}
#define __chkalign_arith(VAL, ALIGN) \
	(((VAL) & ((ALIGN) - 1)) == 0)

__inline_impl __warn_result
static inline _Bool
__chkalign_uint(
		uintmax_t val,
		uintmax_t align)
{
	return __chkalign_arith(val, align);
}

__inline_impl __warn_result
static inline _Bool
__chkalign_int(
		intmax_t val,
		uintmax_t align)
{
	uintmax_t val_unsigned = __cast_signed_unsigned(uintmax_t, val);
	return __chkalign_arith(val_unsigned, align);
}

__inline_impl __warn_result
static inline _Bool
__chkalign_ptr(
		const void *__unsafe_indexable _Nullable val,
		uintmax_t align)
{
	return __chkalign_arith((uintptr_t)val, align);
}
__API_HEADER_END(c, nonnull, single)

#endif // __ZX_STD_LIMITS_H
