/*!
 * @copyright
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT license.
 *
 * @file
 * Array macros.
 */
#ifndef __ZX_STD_ARRAY_H
#define	__ZX_STD_ARRAY_H

#include <0xc/std/api.h>
#include <0xc/std/type.h>
#include <0xc/std/limits.h>

__API_HEADER_BEGIN(c, nonnull, single)

// MARK: Public
/*!
 * @function __static_array
 * Returns whether a variable is a statically-allocated array.
 *
 * @param A
 * The array.
 *
 * @result
 * Whether @A is a statically-allocated array.
 *
 * @discussion
 * This implementation relies on checking that the type of the first array
 * member is not assignable. For the argument
 *
 *     char s[1];
 *
 * '&s[0] is not an assignable type, since array types are not assignable.
 * Whereas 's' decays to 'char *', which is an assignable type. So if the two
 * types are not assignable, then we have a static array.
 */
#define __chk_static_array(A) \
	(!__chk_type_compat(typeof(A), typeof(&(A)[0])))

/*!
 * @function __assert_static_array
 * Asserts that a variable is a statically-allocated array.
 *
 * @param A
 * The array.
 *
 * @param M
 * The assertion message.
 */
#define __assert_static_array(A, M) \
	_Static_assert(__chk_static_array(A), M)

/*!
 * @function __typed_array
 * Declares a function parameter which is associated with a typedef'ed array and
 * includes the 'static' qualifier on its length.
 *
 * @param T
 * The type which defines the array.
 *
 * @param A
 * Additional attributes to be applied to the array (e.g. _Nullable).
 *
 * @param N
 * The name of the parameter.
 *
 * @example
 * Unfortunately, the static keyword on array parameters cannot be used in
 * typedefs. This means that we can't do something like:
 *
 *     typedef char string_t[static 32];
 *
 * and then use 'string_t' as a function parameter. We have to do this:
 *
 *     typedef char string_t[32]
 *
 * and then
 *
 *     void
 *     func(char s[static 32]);
 *
 * This gets clumsy because then we have to define an extra constant for the
 * length to keep consistency between the two, without guaranteeing consistency
 * of the array base type. This macro lets us instead do
 *
 *     void
 *     func(__typed_array(string_t, _Nonnull, s));
 *
 * and keep consistency between both, albeit somewhat clunkily.
 */
#if !defined(__typed_array)
#define __typed_array(T, A, N) \
	typeof(((T){0})[0]) N[A __min_size(sizeof(T) / sizeof(((T){0})[0]))]
#endif

/*!
 * @function __array_bytes
 * Returns the number of bytes in an array of a given type, alignment, and
 * count.
 *
 * @param T
 * The type of the array elements.
 *
 * @param A
 * The alignment of each elment in the array.
 *
 * @param N
 * The number of elements in the array.
 */
#define __array_bytes(T, A, N) (alignto_unsafe(sizeof(T), A) * (N))

/*!
 * @function zarray
 * Initializes an array with elements of the specified type to zero.
 *
 * @param T
 * The type of element in the array.
 *
 * @param N
 * The number of elements in the array.
 *
 * @result
 * An array whose elements are of type @T and whose count is @N, initialized to
 * zero.
 */
#if __clang__
#define zarray(T, N) (T [N]){0}
#else
#define zarray(T, N) {0}
#endif

/*!
 * @function countof
 * Returns the number of elements in a statically-allocated array.
 *
 * @param A
 * The array.
 *
 * @result
 * The number of elements in @A.
 */
#define countof(A) ({ \
	__assert_static_array(A, "array must be statically allocated"); \
	(sizeof(A) / sizeof(A[0])); \
})

/*!
 * @function countof_unsafe
 * Returns the number of elements in a statically-allocated array without
 * verifying that the array is statically allocated.
 *
 * @param A
 * The array.
 *
 * @result
 * The number of elements in @A.
 *
 * @discussion
 * This variant of @countof should only be used in scopes where _Static_assert
 * cannot be used.
 */
#define countof_unsafe(A) \
	(sizeof(A) / sizeof(A[0]))

/*!
 * @function lastof
 * Returns the last index of a statically-allocated array.
 *
 * @param A
 * The array.
 *
 * @result
 * The last index of the array.
 */
#define lastof(A) \
	(countof(A) - 1)

/*!
 * @function spanof
 * Returns the size of an array of type @T with @N elements.
 *
 * @param T
 * The type of the array.
 *
 * @param N
 * The number of elements in the array.
 *
 * @result
 * The size of the array.
 */
#define spanof(T, N) \
	sizeof(T) * (N)

__API_HEADER_END(c, nonnull, single)

#endif // __ZX_STD_ARRAY_H
