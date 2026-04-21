/*!
 * @copyright
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT license.
 *
 * @file
 * Macros for dealing with the C type system.
 */
#ifndef __ZX_STD_TYPE_H
#define	__ZX_STD_TYPE_H

#include <0xc/std/api.h>

__API_HEADER_BEGIN(c, nonnull, single)

// MARK: Public
/*!
 * @function __chk_type_compat
 * Returns whether two types are compatible.
 *
 * @param T1
 * The first type.
 *
 * @param T2
 * The second type.
 *
 * @result
 * Whether the two types are compatible.
 */
#define __chk_type_compat(T1, T2) \
	__builtin_types_compatible_p(T1, T2)

/*!
 * @function __chk_type_unsigned
 * Returns whether a type is an unsigned integer type.
 *
 * @param T
 * The type to check.
 *
 * @result
 * Non-zero if the type is an unsigned integer type, zero otherwise.
 */
#define __chk_type_unsigned(T) (_Generic((T)0, \
	_Generic_match_unsigned_int(1), \
	_Generic_match_signed_int(0) \
))

/*!
 * @function __assert_type_compat
 * Asserts that two types are compatible.
 *
 * @param T1
 * The first type.
 *
 * @param T2
 * The second type.
 *
 * @param M
 * The assertion message.
 */
#define __assert_type_compat(T1, T2, M) \
	_Static_assert(__chk_type_compat(T1, T2), M)

/*!
 * @function __assert_compiler_const
 * Asserts that a variable is a compile-time constant.
 *
 * @param V
 * The variable.
 *
 * @param M
 * The assertion message.
 */
#define __assert_compiler_const(V, M) \
	_Static_assert(__compiler_const(V), M)

/*!
 * @function __assert_positive_and_get
 * Asserts that a value is positive and returns it as the requested unsigned
 * type.
 *
 * @param T
 * The desired type of the result.
 *
 * @param V
 * The value to check.
 *
 * @result
 * The value @V if it is positive.
 */
#define __assert_positive_and_get(T, V) (T)({ \
_Pragma("GCC diagnostic push") \
_Pragma("GCC diagnostic ignored \"-Wtype-limits\"") \
	typeof(V) v_0xc = (V); \
	int un_0xc = __chk_type_unsigned(typeof(v_0xc)); \
\
	if (!un_0xc && v_0xc < 0) { \
		panicx("value must be non-negative: %lld", (long long)v_0xc); \
	} \
\
	(T)(V); \
_Pragma("GCC diagnostic pop") \
})

/*!
 * @macro _Generic_match_signed_int
 * Emits matching arms for all signed integer types for use in a _Generic
 * selection.
 *
 * @param E
 * The expression to use for the matching arms.
 *
 * @result
 * Matching arms for all signed integer types with the expression @E.
 */
#define _Generic_match_signed_int(E) \
	signed char : E, \
	signed short int : E, \
	signed int : E, \
	signed long int : E, \
	signed long long int : E

/*!
 * @macro _Generic_match_unsigned_int
 * Emits matching arms for all unsigned integer types for use in a _Generic
 * selection.
 *
 * @param E
 * The expression to use for the matching arms.
 *
 * @result
 * Matching arms for all unsigned integer types with the expression @E.
 */
#define _Generic_match_unsigned_int(E) \
	unsigned char : E, \
	unsigned short int : E, \
	unsigned int : E, \
	unsigned long int : E, \
	unsigned long long int : E

/*!
 * @function __DEQUALIFY
 * Removes qualifiers from a type.
 *
 * @param T
 * The resulting type.
 *
 * @param Q
 * The qualifier to cast away.
 *
 * @param V
 * The variable.
 *
 * @result
 * The variable @V cast to the type @T.
 */
#if !defined(__DEQUALIFY)
#if defined(__clang__)
#define __DEQUALIFY(T, Q, V) \
	_Pragma("GCC diagnostic push") \
	_Pragma("GCC diagnostic ignored \"-Wcast-qual\"") \
	((T)(V)) \
	_Pragma("GCC diagnostic pop")
#elif defined(__GNUC__)
#define __DEQUALIFY(T, Q, V) \
	((T)(uintptr_t)(V))
#endif // defined(__clang__)
#endif

/*!
 * @function __DECONST
 * Casts away the 'const' qualifier. This macro can be used in such instances to
 * make these call sites more easily discoverable.
 *
 * @param T
 * The resulting type.
 *
 * @param V
 * The variable.
 *
 * @result
 * The variable @V cast to the type @T.
 */
#if !defined(__DECONST)
#define __DECONST(T, V) __DEQUALIFY(T, const, V)
#endif

__API_HEADER_END(c, nonnull, single)

#endif // __ZX_STD_TYPE_H
