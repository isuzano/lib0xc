/*!
 * @copyright
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT license.
 *
 * @header
 * Utilities for annotating API headers.
 */
#ifndef __ZX_STD_API_H
#define __ZX_STD_API_H

// IWYU pragma: begin_exports
#include <0xc/platform.h>
#include <0xc/std/defs.h>
#include <stdlib.h>
#include <stdint.h>

#if __has_include(<ptrcheck.h>)
#include <ptrcheck.h>
#else
#include <0xc/shim/ptrcheck.h>
#endif
// IWYU pragma: end_exports

// MARK: External Declarations
__export __no_tail_call __noreturn __nonnull1 __printflike(1, 2)
void
panicx(
	const char *__cstr fmt,
	...);

// MARK: Header Annotations
/*!
 * @const __BEGIN_DECLS
 * @const __END_DECLS
 * Begin and end a section which disables C++ symbol name mangling.
 */
#if !defined(__BEGIN_DECLS) && defined(__cplusplus)
#define __BEGIN_DECLS extern "C" {
#define __END_DECLS	}
#else
#define __BEGIN_DECLS
#define __END_DECLS
#endif

/*!
 * @const __NULLABLE_ASSUME_NONNULL_BEGIN
 * @const __NULLABLE_ASSUME_NONNULL_END
 * Begin and end a section which assumes all pointers are non-NULL. Headers may
 * not be included within sections which have nullability assumptions.
 */
#if defined(__clang__) && __has_feature(assume_nonnull)
#define __NULLABLE_ASSUME_NONNULL_BEGIN _Pragma("clang assume_nonnull begin")
#define __NULLABLE_ASSUME_NONNULL_END _Pragma("clang assume_nonnull end")
#else
#define __NULLABLE_ASSUME_NONNULL_BEGIN
#define __NULLABLE_ASSUME_NONNULL_END
#endif

/*!
 * @const __PTRCHECK_ASSUME_SINGLE_BEGIN
 * @const __PTRCHECK_ASSUME_SINGLE_END
 * Begin and end a section which assumes all pointers use the single pointer
 * check ABI. Ending the section restores the assumption that pointers use the
 * unsafe-indexable ABI, which is the normal C pointer ABI.
 */
#define __PTRCHECK_ASSUME_SINGLE_BEGIN __ptrcheck_abi_assume_single()
#define __PTRCHECK_ASSUME_SINGLE_END __ptrcheck_abi_assume_unsafe_indexable()

/*!
 * @function __API_HEADER_BEGIN
 * @function __API_HEADER_END
 * Declares the start and end of an API header, respectively.
 *
 * @param SYMABI
 * The linkage ABI when compiling with a C++ compiler. Valid values are:
 *
 *     cxx         Use the C++ ABI, which mangles symbol names
 *     c           Use the C ABI and do not mangle symbol names
 *
 * @param NULLABILITY
 * The nullability assumption policy for all pointers enclosed in the section.
 * Valid values are:
 *
 *     nullable    Assume pointers are nullable unless otherwise specified
 *     nonnull     Assume pointers are non-null unless otherwise specified
 *     unspecified Assume pointers have nullability unspecified
 *
 * @param PTRABI
 * The pointer ABI assumption for all pointers enclosed in the section. Valid
 * values are:
 *
 *     single      Assume pointers use the single pointer ABI and are compatible
 *                 with the C pointer ABI unless otherwise specified
 *     bidi        Assume pointers use the bidirectionally-indexable ABI and are
 *                 incompatible with the C pointer ABI  unless otherwise
 *                 specified
 *     unsafe      Assume pointers use the C pointer ABI and are not bounds-
 *                 checked
 */
#define __API_HEADER_BEGIN(SYMABI, NULLABILITY, PTRABI) \
	__cpp_symabi_ ## SYMABI ## _begin() \
	__nullability_begin(NULLABILITY) \
	__ptrcheck_abi_begin(PTRABI)

#define __API_HEADER_END(SYMABI, NULLABILITY, PTRABI) \
	__cpp_symabi_ ## SYMABI ## _end() \
	__nullability_end(NULLABILITY) \
	__ptrcheck_abi_end(PTRABI)

// MARK: Declarations
/*!
 * @function __CONSTANT
 * Declare an exported constant of an opaque type. This constant follows strict
 * naming conventions related to its type. For a type 'object_type' and symbol
 * named 'foo', the emitted constant is of type 'object_type_t' and named
 * '_object_type_foo'.
 *
 * @param T
 * The type of the constant, not including the trailing '_t'.
 *
 * @param S
 * The name of the symbol which is to be prefixed with the base type @T.
 */
#define __CONSTANT_NAME(T, S) _ ## T ## _ ## S
#define __CONSTANT(T, S) (const T ## _t *)(&__CONSTANT_NAME(T, S))

// MARK: Include-What-You-Use Annotations
/*!
 * @function DECL_FUNC_4IWYU
 * Declares a function so that it can be found by IWYU. This is primarily useful
 * for functions whose actual declarations are in implementation headers that
 * are transitively included by public headers, but which IWYU cannot resolve
 * correctly.
 *
 * Use of this macro does not impact declarations on the function prototype,
 * e.g. attributes such as __printflike are still respected and do not need to
 * be repeated.
 */
#define DECL_FUNC_4IWYU(F) typeof(F) F

/*!
 * @function DECL_TYPE_4IWYU
 * Declares a type so that it can be found by IWYU. This is essentially the same
 * use-case as @DECL_FUNC_4IWYU, but for types.
 */
#define DECL_TYPE_4IWYU(T) typedef T T

// MARK: Structure Versioning
/*!
 * @typedef struct_version_t
 * A convenience type which encapsulates a version for a structure whose layout
 * is exported. This type is only a single byte wide and is therefore does not
 * require managing byte ordering.
 */
typedef uint8_t struct_version_t;

/*!
 * @function STRUCT_VERSION
 * Convenience macro to use a compile-time constant as a structure version.
 *
 * @param V
 * The version number. If this is not a compile-time constant, the expression
 * will evaluate an array with negative size, and compilation will fail with the
 * corresponding error.
 */
#define STRUCT_VERSION(V) \
	(struct_version_t)( \
		__compiler_const(V) ? \
			(V) : \
			sizeof( \
				struct { \
					char __check[__compiler_const(V) ? 1 : -1]; \
				} \
			) \
		)

/*!
 * @function struct_version_from_buffer
 * Reads a structure version from untyped memory. The version is expected to be
 * the first field in the structure.
 *
 * @param buff
 * A pointer to the memory which purports to hold the structure.
 *
 * @param len
 * The length of @buff.
 *
 * @result
 * The version. If the buffer was not large enough to accommodate the
 * struct_version_t type, the implementation will abort.
 */
__inline_impl __warn_result __nonnull1
static inline struct_version_t
struct_version_from_buffer(
		const void *__sized_by(len) buff,
		size_t len)
{
	if (len < sizeof(struct_version_t)) {
		panicx("zero byte buffer");
	}
	return ((const struct_version_t *)buff)[0];
}

/*!
 * @function struct_version_assert
 * Asserts that a structure version is at least a minimum version.
 *
 * @param T
 * The structure type.
 *
 * @param V
 * The version.
 *
 * @param MIN
 * The minimum supported version.
 *
 * @param MAX
 * The maximum supported version.
 */
#define struct_version_assert(T, V, MIN, MAX) ({ \
	if ((V) < (MIN)) { \
		panicx("unsupported structure version: " \
				"type = %s, actual = %hhu, expected >= %hhu", \
				#T, (V), (MIN)); \
	} \
	if ((V) > (MAX)) { \
		panicx("unsupported structure version: " \
				"type = %s, actual = %hhu, expected <= %hhu", \
				#T, (V), (MAX)); \
	} \
})

/*!
 * @internal
 * Implementation details.
 */
// MARK: Internal
#if !defined(__BEGIN_DECLS) && defined(__cplusplus)
#define __cpp_symabi_c_begin() extern "C" {
#define __cpp_symabi_c_end() }

#define __cpp_symabi_cxx_begin()
#define __cpp_symabi_cxx_end()
#else
#define __cpp_symabi_c_begin()
#define __cpp_symabi_c_end()

#define __cpp_symabi_cxx_begin()
#define __cpp_symabi_cxx_end()
#endif

#if defined(__clang__) && __has_feature(assume_nonnull)
#define __nullability_begin(N) PRAGMA(clang assume_ ## N begin)
#define __nullability_end(N) PRAGMA(clang assume_ ## N end)
#else
#define __nullability_begin(N)
#define __nullability_end(N)
#endif

#define __ptrcheck_abi_begin(ABI) __ptrcheck_abi_assume_ ## ABI ()
#define __ptrcheck_abi_end(ABI) __ptrcheck_abi_assume_unsafe_indexable()

#endif // __ZX_STD_API_H
