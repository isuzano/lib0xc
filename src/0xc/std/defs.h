/*!
 * @copyright
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT license.
 *
 * @header
 * Base definitions for lib0xc.
 */
#ifndef __ZX_STD_DEFS_H
#define __ZX_STD_DEFS_H

// IWYU pragma: begin_exports
#include <0xc/std/macros.h>

#include <stdlib.h>
#include <stddef.h>

#if __has_include(<sys/cdefs.h>)
#include <sys/cdefs.h>
#endif
// IWYU pragma: end_exports

#if __STDC_VERSION__ < 201112L || !__GNUC__
#error "lib0xc requires gnuc11"
#endif

#ifndef __has_builtin
#define __has_builtin(X) 0
#endif
#ifndef __has_include
#define __has_include(X) 0
#endif
#ifndef __has_feature
#define __has_feature(X) 0
#endif
#ifndef __has_attribute
#define __has_attribute(X) 0
#endif
#ifndef __has_extension
#define __has_extension(X) 0
#endif
#ifndef __has_ptrcheck
#define __has_ptrcheck 0
#endif
// clang-17 doesn't appear to expose the c_fixed_enum extension for checking,
// but it's been around long enough that we can assume it's there when compiling
// with clang. Otherwise, we can check for it directly. This syntax was added to
// C in C23.
#if defined(__clang__) || __has_extension(c_fixed_enum) || __STDC_VERSION__ >= 202311L
#define __ZX_FIXED_ENUMS 1
#else
#define __ZX_FIXED_ENUMS 0
#endif

#if __GNUC__
#define __noreturn_0xgnuc __attribute__((__noreturn__))
#define __nothrow_0xgnuc __attribute__((__nothrow__))
#define __nonnull1_0xgnuc __attribute__((__nonnull__(1)))
#define __nonnull2_0xgnuc __attribute__((__nonnull__(2)))
#define __nonnull3_0xgnuc __attribute__((__nonnull__(3)))
#define __nonnull4_0xgnuc __attribute__((__nonnull__(4)))
#define __nonnull5_0xgnuc __attribute__((__nonnull__(5)))
#define __nonnull6_0xgnuc __attribute__((__nonnull__(6)))
#define __nonnull7_0xgnuc __attribute__((__nonnull__(7)))
#define __nonnull8_0xgnuc __attribute__((__nonnull__(8)))
#define __nonnull9_0xgnuc __attribute__((__nonnull__(9)))
#define __nonnull10_0xgnuc __attribute__((__nonnull__(10)))
#define __nonnull11_0xgnuc __attribute__((__nonnull__(11)))
#define __nonnull12_0xgnuc __attribute__((__nonnull__(12)))
#define __nonnull13_0xgnuc __attribute__((__nonnull__(13)))
#define __nonnull14_0xgnuc __attribute__((__nonnull__(14)))
#define __nonnull15_0xgnuc __attribute__((__nonnull__(15)))
#define __sentinel_0xgnuc __attribute__((__sentinel__))
#define __pure_0xgnuc __attribute__((__pure__))
#define __pure_const_0xgnuc __attribute__((__const__))
#define __warn_result_0xgnuc __attribute__((__warn_unused_result__))
#define __malloc_0xgnuc __attribute__((__malloc__))
#define __used_0xgnuc __attribute__((__used__))
#define __unused_0xgnuc __attribute__((__unused__))
#define __cold_0xgnuc __attribute__((__cold__))
#define __noinline_0xgnuc __attribute__((__noinline__))
#define __always_inline_0xgnuc __attribute__((__always_inline__))
#define __transparent_union_0xgnuc __attribute__((__transparent_union__))
#define __aligned_0xgnuc(n) __attribute__((__aligned__((n))))
#define __packed_0xgnuc __attribute__((__packed__))
#define __printflike_0xgnuc(x, y) __attribute__((__format__(printf,x,y)))
#define __export_0xgnuc extern __attribute__((__visibility__("default")))
#define __private_0xgnuc extern __attribute__((__visibility__("hidden")))
#define __expect_0xgnuc(X, V) __builtin_expect((X), (V))
#define __prevent_tail_call_optimization_0xgnuc() __asm__("")
#define __compiler_const_0xgnuc(expr) __builtin_constant_p(expr)
#define __unavail_0xgnuc(M) __attribute__((__unavailable__(M)))
#define __cleanup_0xgnuc(F) __attribute__((__cleanup__(F)))
#define __alloc_size_0xgnuc(N, ...) \
	__attribute__((alloc_size(N, ## __VA_ARGS__)))
#define __extension_0xgnuc __extension__
#else
#define __noreturn_0xgnuc
#define __nothrow_0xgnuc
#define __nonnull1_0xgnuc
#define __nonnull2_0xgnuc
#define __nonnull3_0xgnuc
#define __nonnull4_0xgnuc
#define __nonnull5_0xgnuc
#define __nonnull6_0xgnuc
#define __nonnull7_0xgnuc
#define __nonnull8_0xgnuc
#define __nonnull9_0xgnuc
#define __nonnull10_0xgnuc
#define __nonnull11_0xgnuc
#define __nonnull12_0xgnuc
#define __nonnull13_0xgnuc
#define __nonnull14_0xgnuc
#define __nonnull15_0xgnuc
#define __sentinel_0xgnuc
#define __pure_0xgnuc
#define __pure_const_0xgnuc
#define __warn_result_0xgnuc
#define __malloc_0xgnuc
#define __used_0xgnuc
#define __unused_0xgnuc
#define __cold_0xgnuc
#define __noinline_0xgnuc
#define __always_inline_0xgnuc
#define __transparent_union_0xgnuc
#define __aligned_0xgnuc(n)
#define __packed_0xgnuc
#define __printflike_0xgnuc
#define __export_0xgnuc
#define __private_0xgnuc
#define __expect_0xgnuc
#define __prevent_tail_call_optimization_0xgnuc()
#define __compiler_const_0xgnuc(EXPR)
#define __unavail_0xgnuc(M)
#define __cleanup_0xgnuc
#define __alloc_size_0xgnuc(N)
#define __extension_0xgnuc
#endif

#if !defined(__noreturn)
#define __noreturn __noreturn_0xgnuc
#endif

#if !defined(__nothrow)
#define __nothrow __nothrow_0xgnuc
#endif

#if !defined(__nonnull1)
#define __nonnull1 __nonnull1_0xgnuc
#endif

#if !defined(__nonnull2)
#define __nonnull2 __nonnull2_0xgnuc
#endif

#if !defined(__nonnull3)
#define __nonnull3 __nonnull3_0xgnuc
#endif

#if !defined(__nonnull4)
#define __nonnull4 __nonnull4_0xgnuc
#endif

#if !defined(__nonnull5)
#define __nonnull5 __nonnull5_0xgnuc
#endif

#if !defined(__nonnull6)
#define __nonnull6 __nonnull6_0xgnuc
#endif

#if !defined(__nonnull7)
#define __nonnull7 __nonnull7_0xgnuc
#endif

#if !defined(__nonnull8)
#define __nonnull8 __nonnull8_0xgnuc
#endif

#if !defined(__nonnull9)
#define __nonnull9 __nonnull9_0xgnuc
#endif

#if !defined(__nonnull10)
#define __nonnull10 __nonnull10_0xgnuc
#endif

#if !defined(__nonnull11)
#define __nonnull11 __nonnull11_0xgnuc
#endif

#if !defined(__nonnull12)
#define __nonnull12 __nonnull12_0xgnuc
#endif

#if !defined(__nonnull13)
#define __nonnull13 __nonnull13_0xgnuc
#endif

#if !defined(__nonnull14)
#define __nonnull14 __nonnull14_0xgnuc
#endif

#if !defined(__nonnull15)
#define __nonnull15 __nonnull15_0xgnuc
#endif

#if !defined(__sentinel)
#define __sentinel __sentinel_0xgnuc
#endif

#if !defined(__pure)
#define __pure __pure_0xgnuc
#endif

#if !defined(__pure_const)
#define __pure_const __pure_const_0xgnuc
#endif

#if !defined(__warn_result)
#define __warn_result __warn_result_0xgnuc
#endif

#if !defined(__malloc)
#define __malloc __malloc_0xgnuc
#endif

#if !defined(__used)
#define __used __used_0xgnuc
#endif

// Note that gcc and clang have differing opinions on where this can be placed
// on a declaration. Both are happen with either before or after, but gcc does
// not like it inline, e.g. clang is okay with:
//
//     char *__unused foo = NULL;
//
// but gcc is doesn't respect the attribute in that usage.
#if !defined(__unused)
#define __unused __unused_0xgnuc
#endif

#if !defined(__cold)
#define __cold __cold_0xgnuc
#endif

#if !defined(__noinline)
#define __noinline __noinline_0xgnuc
#endif

#if !defined(__always_inline)
#define __always_inline __always_inline_0xgnuc
#endif

#if !defined(__transparent_union)
#define __transparent_union __transparent_union_0xgnuc
#endif

#if !defined(__aligned)
#define __aligned(N) __aligned_0xgnuc(N)
#endif

#if !defined(__packed)
#define __packed __packed_0xgnuc
#endif

#if !defined(__printflike)
#define __printflike(X, Y) __printflike_0xgnuc(X, Y)
#endif

#if !defined(__export)
#define __export __export_0xgnuc
#endif

#if !defined(__private)
#define __private __private_0xgnuc
#endif

#if !defined(__inline_impl)
#define __inline_impl __unused __always_inline
#endif

#if !defined(__expect)
#define __expect(X, Y) __expect_0xgnuc(X, Y)
#endif

#if !defined(__prevent_tail_call_optimization)
#define __prevent_tail_call_optimization() \
	__prevent_tail_call_optimization_0xgnuc()
#endif

#if !defined(__compiler_const)
#define __compiler_const(EXPR) __compiler_const_0xgnuc(EXPR)
#endif

#if !defined(__unavail)
#define __unavail(M) __unavail_0xgnuc(M)
#endif

#if !defined(__ptrcheck_required)
#if __has_ptrcheck
#define __ptrcheck_required
#else
#define __ptrcheck_required __unavail("requires -fbounds-safety")
#endif
#endif

#if !__has_attribute(__cleanup)
#if __has_attribute(cleanup)
#define __cleanup(F) __cleanup_0xgnuc(F)
#else
#define __cleanup(F) __unavail("cleanup attribute not available")
#endif
#endif

#if __STDC_VERSION__ < 202311L && !__GNUC__
#define	typeof(X) HALT("typeof not available on this compiler")
#endif

#if __STDC_VERSION__ >= 202311L || __has_builtin(__typeof_unqual__)
#define __typeof_dequal(X) __typeof_unqual__(X)
#elif __GNUC__
#define __typeof_dequal(X) typeof(({ \
	__auto_type tmp_0xc = (X); \
	tmp_0xc; \
}))
#else
#define __typeof_dequal(X) \
	HALT("typeof_dequal not available on this compiler")
#endif

#define typeof_decl(X) __typeof_dequal({ \
	X, tmp_0xc; \
})

#if !defined(__alloc_size)
#if __has_attribute(__alloc_size)
#define __alloc_size(N, ...) __alloc_size_0xgnuc(N, ## __VA_ARGS__)
#else
#define __alloc_size(N, ...)
#endif
#endif

#if !defined(__no_tail_call)
#if __has_attribute(not_tail_called)
#define __no_tail_call __attribute__((__not_tail_called__))
#else
#define __no_tail_call
#endif
#endif

#if !defined(__callback)
#if __has_attribute(callback)
#define __callback(F, ...) __attribute__((__callback__(F, ## __VA_ARGS__)))
#else
#define __callback
#endif
#endif

#if !defined(__cdecl)
#if defined(__cplusplus)
#define __cdecl extern "C"
#else
#define __cdecl
#endif
#endif

#if !defined(__cstr)
#define __cstr __null_terminated
#endif

#if !(defined(__clang__) && __has_feature(nullability))
#define	_Nonnull
#define	_Nullable
#define	_Null_unspecified
#endif

#if defined(__clang__) && __has_feature(nullability)
#define	__NULLABILITY_PRAGMA_PUSH \
	_Pragma("clang diagnostic push") \
	_Pragma("clang diagnostic ignored \"-Wnullability-completeness\"")
#define	__NULLABILITY_PRAGMA_POP \
	_Pragma("clang diagnostic pop")
#else
#define	__NULLABILITY_PRAGMA_PUSH
#define	__NULLABILITY_PRAGMA_POP
#endif

#if !defined(__extension)
#define __extension __extension_0xgnuc
#endif

#if defined(__clang__)
#define __strict_literal(T) (T)
#define __strict_literal_addr(T) &(T)
#else
#define __strict_literal(T)
#define __strict_literal_addr(T) &(T)
#endif

#if !defined(__cplusplus) && !defined(__min_size)
#define __min_size(X) static (X)
#else
#define __min_size(X) (X)
#endif

// This doesn't come in until clang-20 or so.
#if __has_attribute(require_explicit_initialization) && !defined(__fldinit)
#define __fldinit __attribute__((require_explicit_initialization))
#else
#define __fldinit
#endif

#if !defined(__fallthrough)
#if defined(__cplusplus) && defined(__clang__)
#define __fallthrough() [[clang::fallthrough]]
#elif __has_attribute(fallthrough)
#define __fallthrough() __attribute__((__fallthrough__))
#else
#define __fallthrough()
#endif
#endif

#if !defined(__unreachable)
#if __has_builtin(__builtin_unreachable)
#define __unreachable() __builtin_unreachable()
#else
#define __unreachable() abort()
#endif
#endif

#if !defined(__enum_ext)
#if __has_attribute(enum_extensibility)
#define __enum_ext(EXT) __attribute__((enum_extensibility(EXT)))
#else
#define __enum_ext(EXT)
#endif
#endif

#if !defined(__enum_bitfld)
#if __has_attribute(flag_enum)
#define __enum_bitfld __attribute__((flag_enum))
#else
#define __enum_bitfld
#endif
#endif

#if !defined(__typed_enum)
#if __ZX_FIXED_ENUMS
#define __typed_enum(NAME, T, EXT, ...) \
	typedef enum __enum_ext(EXT) NAME : T { \
		__VA_ARGS__ \
	} NAME ## _t
#else
#define __typed_enum(NAME, T, EXT, ...) \
	enum __enum_ext(EXT) NAME { \
		__VA_ARGS__ \
	}; \
	typedef T NAME ## _t
#endif
#endif

#if !defined(__bitfld)
#if __ZX_FIXED_ENUMS
#define __bitfld(NAME, T, ...) \
	typedef enum __enum_ext(closed) __enum_bitfld NAME : T { \
		__VA_ARGS__ \
	} NAME ## _t
#else
#define __bitfld(NAME, T, ...) \
	enum __enum_ext(closed) __enum_bitfld NAME { \
		__VA_ARGS__ \
	}; \
	typedef T NAME ## _t
#endif
#endif

#if !defined(__dump_struct)
#if __has_builtin(__builtin_dump_struct)
#define __dump_struct(S, P, ...) \
	__builtin_dump_struct(S, P, ## __VA_ARGS__)
#else
#define __dump_struct(S, P, ...) \
	P(__VA_ARGS__, "%s", #S)
#endif
#endif

#if !defined(__classify_type)
#if __has_builtin(__builtin_classify_type)
#define __classify_type(T) __builtin_classify_type(T)
#define __type_class_check(T, CLASS) (__classify_type(T) == (CLASS))
#else
#define __classify_type(T) \
	HALT("type classification not available on this compiler")
#define __type_class_check(T, CLASS)  \
	HALT("type classification not available on this compiler")
#endif
#endif

#endif // __ZX_STD_DEFS_H
