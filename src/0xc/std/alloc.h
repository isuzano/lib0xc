/*!
 * @copyright
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT license.
 *
 * @header
 * Memory allocation.
 */
#ifndef __ZX_STD_ALLOC_H
#define __ZX_STD_ALLOC_H

#include <0xc/std/api.h>
#include <0xc/std/io.h>
#include <0xc/std/type.h>
#include <0xc/std/pointer.h>
#include <0xc/sys/panic.h>

// IWYU pragma: begin_exports
#if __has_include(<alloca.h>)
#include <alloca.h>
#define __ZX_HAS_ALLOCA 1
#endif

#include <stdlib.h>
// IWYU pragma: end_exports

__API_HEADER_BEGIN(c, nonnull, single)

// MARK: Attributes
/*!
 * @define __free
 * When applied to a variable which refers to a heap allocation from malloc(3)
 * et al., this attribute causes the variable to be passed to a cleanup function
 * which calls free(3) on the variable's value when it goes out of scope. If the
 * variable's value does not refer to a valid heap allocation at the time of
 * scope exit, the behavior is undefined.
 *
 * @example
 *
 *     // `buf` will be automatically freed when it goes out of scope. If the
 *     // scope exits before `buf` is assigned, NULL will be passed to free(3),
 *     // which is a no-op, so this is safe.
 *     char *__free buf = NULL;
 *
 *     ...
 *
 *     buf = malloc(128)
 */
#define __free __cleanup(__cleanup_free)

// MARK: Public
/*!
 * @function objalloc
 * Allocates a zero-initialized object of the specified type.
 *
 * @param T
 * The type of object to allocate.
 *
 * @result
 * The new object. The caller is responsible for disposing of the allocation
 * when it is no longer needed.
 */
#define objalloc(T) (T *__single)({ \
	T *__single obj_0xc = NULL; \
\
	obj_0xc = calloc(1, sizeof(T)); \
	if (!obj_0xc) { \
		panicx("object allocation failed: " \
				"t = %s, size = %zu", \
				#T, sizeof(T)); \
	} \
	(obj_0xc); \
})

/*!
 * @function objallocx
 * Allocates an object of the specified type with extra space at the end.
 *
 * @param T
 * The type of object to allocate.
 *
 * @param F
 * The field in the object to store the extra size.
 *
 * @param X
 * The number of extra bytes to allocate at the end of the object. This
 * argument's type must be compatible with the type of the field specified by
 * @F.
 *
 * @result
 * The new object. The caller is responsible for disposing of the allocation
 * when it is no longer needed.
 *
 * @discussion
 * When bounds safety is enabled, the flexible array member at the end of @T
 * must be annotated with __counted_by using @F. The result of this macro must
 * be assigned to a single pointer. For example:
 *
 *     struct obj {
 *         uint32_t f_extra_size;
 *         uint8_t f_extra_data[__counted_by(f_extra_size)];
 *     }
 *
 *     struct obj *__single obj = objallocx(struct obj, f_extra_size, 128);
 */
#if __has_ptrcheck
#define objallocx(T, F, X) (T *__single)({ \
	size_t x_0xc = __assert_positive_and_get(size_t, X); \
	size_t n_0xc = sizeof(T) + x_0xc; \
	T *__indexable obj_0xc = NULL; \
\
	obj_0xc = calloc(1, n_0xc); \
	if (!obj_0xc) { \
		panicx("flexible object allocation failed: " \
				"type = %s, size = %zu, extra = %zu", \
				#T, sizeof(T), x_0xc); \
	} \
	obj_0xc->F = (X); \
	(T *__single)obj_0xc; \
})
#else
#define objallocx(T, F, X) (T *)({ \
	size_t x_0xc = __assert_positive_and_get(size_t, X); \
	size_t n_0xc = sizeof(T) + x_0xc; \
	T *obj_0xc = NULL; \
\
	obj_0xc = calloc(1, n_0xc); \
	if (!obj_0xc) { \
		panicx("flexible object allocation failed: " \
				"type = %s, size = %zu, extra = %zu", \
				#T, sizeof(T), x_0xc); \
	} \
	obj_0xc->F = (X); \
	(obj_0xc); \
})
#endif

/*!
 * @function objcalloc
 * Allocates a series of zero-initialized objects of the specified type.
 *
 * @param T
 * The type of object to allocate.
 *
 * @param N
 * The number of objects to allocate.
 *
 * @result
 * A pointer to the first object allocated. The caller is responsible for
 * disposing of the allocation when it is no longer needed.
 */
#define objcalloc(T, N) (T *__counted_by(N))({ \
	T *__counted_by(N) objs_0xc = NULL; \
\
	objs_0xc = calloc((N), sizeof(T)); \
	if (!objs_0xc) { \
		panicx("object allocation failed: " \
				"t = %s, cnt = %zu, size = %zu", \
				#T, oxozu(N), sizeof(T)); \
	} \
	(objs_0xc); \
})

/*!
 * @function walloc
 * Allocates heap memory such that the caller may assume the allocation is
 * successful.
 *
 * @param S
 * The size of the allocation.
 *
 * @result
 * The new allocation. The resulting memory is zero-initialized. The caller is
 * responsible for disposing of the allocation when it is no longer needed.
 *
 * @discussion
 * If the allocation size is a constant known to the compiler, the
 * implementation will abort the caller if the underlying allocation fails. The
 * rationale is that callers should only hard-code reasonably-sized allocations,
 * e.g. the size of a C structure. This same rationale is why the memory is
 * always zero-initialized -- the cost of zeroing small allocations is low.
 *
 * Otherwise, the implementation will yield for a target-defined period and
 * retry the allocation until successful.
 */
#define walloc(S) wcalloc(1, S)

/*!
 * @function wcalloc
 * Allocates a number of objects from heap memory such that the caller may
 * assume the allocation is successful.
 *
 * @param N
 * The count of objects to allocate.
 *
 * @param S
 * The size of each object.
 *
 * @result
 * The new allocation. The resulting memory is zero-initialized. The caller is
 * responsible for disposing of the allocation when it is no longer needed.
 *
 * @discussion
 * If both the object count and allocation size are constants known to the
 * compiler, the implementation will abort the caller if the underlying
 * allocation fails. The rationale is that callers should only hard-code a
 * reasonable number of reasonably-sized allocations, e.g. some number of C
 * structure.
 *
 * Otherwise, the implementation will yield for a target-defined period and
 * retry the allocation until successful.
 */
#define wcalloc(N, S) ({ \
	void *p_0xc = NULL; \
\
	_Static_assert(__compiler_const(S) && __compiler_const(N) ? \
			((N) * (S)) <= ZX_WALLOC_LIMIT : 1, \
			"static allocation size beyond target limit"); \
	if (__compiler_const(S) && __compiler_const(N)) { \
		p_0xc = _calloc_static(N, S); \
	} else { \
		p_0xc = _calloc_wait(N, S); \
	} \
	(p_0xc); \
})

/*!
 * @internal
 * Implementation details.
 */
// MARK: Internal
#if !defined(__ZX_HAS_ALLOCA)
#if __has_builtin(__builtin_alloca)
#define alloca(S) __builtin_alloca(S)
#else
#define alloca(S) HALT("alloca not available in this environment")
#endif
#endif

__export __no_tail_call
void
__waiting_for_memory(
	size_t s);

__inline_impl __warn_result __malloc __alloc_size(1, 2)
static inline void *__sized_by_or_null(n * s)
_calloc_static(
		size_t n,
		size_t s)
{
	void *p = NULL;

	p = calloc(n, s);
	if (!p) {
		panicx("allocation failure for static size: "
				"n = %zu, size = %zu", n, s);
	}
	return p;
}

__inline_impl __warn_result __malloc __alloc_size(1, 2)
static inline void *__sized_by_or_null(n * s)
_calloc_wait(
		size_t n,
		size_t s)
{
	void *p = NULL;

	while (!(p = calloc(n, s))) {
		__waiting_for_memory(n * s);
	}
	return p;
}

__inline_impl __nonnull1
static inline void
__cleanup_free(
		void *_Nonnull __unsafe_indexable __p)
{
	void **__single tp = __unsafe_forge(void **, __p, single);
	void *__unsafe_indexable p = *tp;
	free(p);
}

__API_HEADER_END(c, nonnull, single)

#endif // __ZX_STD_ALLOC_H
