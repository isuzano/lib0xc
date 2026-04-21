/*!
 * @copyright
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT license.
 *
 * @header
 * Bounds-safety macros for use with the clang bounds-safety feature. More
 * information about bounds safety can be found here:
 *
 *     https://tinyurl.com/bounds-safety
 */
#ifndef __ZX_STD_POINTER_H
#define __ZX_STD_POINTER_H

#include <0xc/std/api.h>
#include <0xc/std/int.h>
#include <0xc/std/array.h>
#include <0xc/std/io.h>
#include <0xc/std/limits.h>

#include <stdint.h>

__API_HEADER_BEGIN(c, nonnull, single)

// MARK: Definitions
/*!
 * @define POINTER_WIDTH
 * The width of a pointer in bits.
 */
#define POINTER_WIDTH WIDTHOF(void *)

// MARK: Types
/*!
 * @typedef tagptr_t
 * An opaque tagged pointer type.
 */
typedef struct _tagptr {
	uintptr_t __opaque;
} tagptr_t;
#define TAGPTR_NULL __strict_literal(tagptr_t){ \
	.__opaque = 0, \
}

/*!
 * @typedef ptrtag_t
 * A pointer tag.
 */
typedef uint32_t ptrtag_t;

// MARK: Public
/*!
 * @function __pointee_assignable
 * Expands to an assignable pointee which may be referred to by a pointer which
 * may be NULL.
 *
 * @param VP
 * The pointer.
 *
 * @example
 *
 *     const void *
 *     func(size_t *_Nullable len)
 *     {
 *         void *p = ...
 *         size_t p_len = ...
 *
 *         ...
 *
 *         __pointee_assignable(len) = p_len;
 *         return p;
 *     }
 */
#define __pointee_assignable(VP) if (VP) *(VP)

/*!
 * @function __bounds_update
 * Updates the bounds associated with a pointer without modifying the pointer
 * itself.
 *
 * @param V
 * The length variable.
 *
 * @param L
 * The new length value.
 */
#if __has_ptrcheck
#define __bounds_update(V, L) \
	(V) = (L)
#else
#define __bounds_update(V, L) \
	(void)((V) = (L))
#endif

/*!
 * @function __bounds_affirm
 * Affirms the bounds of a pointer without modifying the pointer itself. This is
 * useful for __counted_by_or_null and __sized_by_or_null pointers which still
 * require bounds updates to be tracked even when the size does not change.
 *
 * @param V
 * The length variable.
 */
#if __has_ptrcheck
#define __bounds_affirm(V) \
	(V) = (V)
#else
#define __bounds_affirm(V) \
	(void)((V) = (V))
#endif

/*!
 * @function __bounds_check_and_affirm
 * Checks the result of a function call which returns a __sized_by_or_null or
 * __counted_by_or_null pointer, and if successful, affirms the size of the
 * memory indicated by the pointer.
 *
 * @param P
 * The pointer returned from the function. This pointer must be either a
 * __sized_by_or_null or __counted_by_or_null pointer which holds the intended
 * allocation size.
 *
 * @param V
 * The variable associated with the pointer via __sized_by_or_null or
 * __counted_by_or_null. This variable must have held the intended size of the
 * memory region previously.
 *
 * @result
 * The pointer.
 *
 * @discussion
 * This macro enables the following pattern:
 *
 *     size_t p_len = 24;
 *     void *__sized_by_or_null(p_len) p = NULL;
 *
 *     p = malloc(p_len);
 *     if (!__bounds_check_and_affirm(p, p_len)) {
 *         // handle error
 *     }
 *
 * This avoids an explicit self-assignment of the bounds-tracking variable after
 * the function call, which is somewhat unaesthetic.
 */
#define __bounds_check_and_affirm(P, V) \
	(__bounds_update(V, V), (P))

/*!
 * @function __bounds_check_and_set
 * Checks the result of a function call which returns a bounded pointer, and if
 * successful, updates the bounds-tracking variable with the provided size.
 *
 * @param P
 * The pointer returned from the function. This pointer must be either a
 * __sized_by or __counted_by pointer.
 *
 * @param V
 * The variable associated with the pointer via __sized_by or __counted_by.
 *
 * @param S
 * The size to set.
 *
 * @result
 * The pointer.
 *
 * @discussion
 * This macro enables the following pattern:
 *
 *     size_t to_read = 24;
 *     size_t p_len = 0;
 *     void *__sized_by(p_len) p = NULL;
 *
 *     p = read_bytes(to_read);
 *     if (!__bounds_check_and_set(p, p_len, to_read)) {
 *         // handle error
 *     }
 *
 * This avoids an explicit size assignment of the bounds-tracking variable after
 * the function call, which is somewhat unaesthetic.
 */
#define __bounds_check_and_set(P, V, S) \
	(__bounds_update(V, S), (P))

/*!
 * @function __pointer_memsize
 * Returns the size in bytes of the memory region covered by a bounded pointer.
 *
 * @param T
 * The type of the resulting size, e.g. 'size_t'.
 *
 * @param P
 * The bounded pointer.
 *
 * @result
 * The size in bytes of the memory region covered by the bounded pointer.
 *
 * @discussion
 * This macro is only available when bounds safety is enabled.
 */
#if __has_ptrcheck
#define __pointer_memsize(T, P) (T)({ \
	ptrdiff_t lo_0xc = 0; \
	ptrdiff_t hi_0xc = 0; \
\
	lo_0xc = (ptrdiff_t)__ptr_lower_bound(P); \
	hi_0xc = (ptrdiff_t)__ptr_upper_bound(P); \
\
	__cast_signed_unsigned(T, hi_0xc - lo_0xc); \
})
#endif

/*!
 * @function __forge
 * Convenience function that performs safe forge operations to produce a bounded
 * pointer. These operations must take place on statically-allocated array
 * objects.
 *
 * @param T
 * The type of the pointer to forge, e.g. 'const char *'.
 *
 * @param P
 * The address from which to forge the bounded pointer.
 *
 * @param ABI
 * The ABI of the pointer to forge. Valid values are:
 *
 *     bidi       A bidirectionally-indexable pointer. Bounds will encapsulate
 *                the size of the object specified by P
 *     single     A pointer to a single object of type T
 *     term       A pointer to memory which is terminated by a pointer with a
 *                specific value. When this ABI is specified, the extra argument
 *                must be the terminating value, e.g. a zero byte
 *     zterm      A pointer to memory which is terminated by a zero value
 *
 * @param ...
 * Extra arguments required by the ABI.
 */
#define __forge(T, P, ABI, ...) ({ \
	__assert_static_array(P, "argument must be statically allocated array"); \
	__forge_abi_ ## ABI(T, P, ## __VA_ARGS__); \
})

/*!
 * @function __forge_type
 * Convenience function that forges a single pointer from a pointer and length.
 * The given memory region must be of the exact size required by the type of
 * pointer being forged, or the implementation will abort.
 *
 * @param T
 * The type of the pointer to forge, e.g. 'struct foo *'.
 *
 * @param P
 * The address from which to forge the single pointer.
 *
 * @param N
 * The length of the memory referred to by @P.
 *
 * @result
 * A new single pointer of the requested type.
 */
#define __forge_type(T, P, N) ({ \
	if ((N) != sizeof(((T)0)[0])) { \
		panicx("cannot forge '%s' from pointer: " \
				"actual length = %llu, expected length = %zu", \
				#T, oxollu(N), sizeof(((T)0)[0])); \
	} \
	__unsafe_forge(T, P, single); \
})

/*!
 * @function __forge_type_try
 * Convenience function that attempts to forge a single pointer to an object
 * from a pointer and length.
 *
 * @param T
 * The type of the pointer to forge, e.g. 'struct foo *'.
 *
 * @param P
 * The address from which to forge the single pointer.
 *
 * @param N
 * The length of the memory referred to by @P.
 *
 * @result
 * A new single pointer of the requested type, or NULL if the memory region is
 * not large enough to hold the object.
 */
#define __forge_type_try(T, P, N) ({ \
	((N) >= sizeof(((T)0)[0])) ? \
		__unsafe_forge(T, P, single) : \
		NULL; \
})

/*!
 * @function __forge_fromtype
 * Convenience function that forges a bidirectionally-indexable pointer from an
 * object.
 *
 * @param T
 * The type of the pointer to forge, e.g. 'uint8_t *'.
 *
 * @param P
 * The address from which to forge the pointer.
 *
 * @result
 * A new bidirectionally-indexable pointer which covers the object's bounds.
 */
#define __forge_fromtype(T, P) ({ \
	__unsafe_forge(T, P, bidi, sizeof(*P)); \
})

/*!
 * @function __unsafe_forge
 * Convenience function that performs unsafe-forge operations to produce a
 * bounded pointer. This should only be used when the bounds of the source
 * memory are not knowable to the compiler.
 *
 * @param T
 * The type of the pointer to forge, e.g. 'const char *'.
 *
 * @param P
 * The address from which to forge the bounded pointer.
 *
 * @param ABI
 * The ABI of the pointer to forge. Valid values are:
 *
 *     bidi       A bidirectionally-indexable pointer. When this ABI is
 *                specified, the extra argument must be an integer which
 *                contains the length of the memory
 *     single     A pointer to a single object of type T
 *     term       A pointer to memory which is terminated by a pointer with a
 *                specific value. When this ABI is specified, the extra argument
 *                must be the terminating value, e.g. a zero byte
 *     zterm      A pointer to memory which is terminated by a zero value
 *
 * @param ...
 * Extra arguments required by the ABI.
 *
 * @result
 * A new bounded pointer of the requested type.
 */
#define __unsafe_forge(T, P, ABI, ...) \
	__unsafe_forge_abi_ ## ABI(T, P, ## __VA_ARGS__)

/*!
 * @function __unsafe_forge_aligned
 * Performs an unsafe-forge operation on an aligned address. The address is
 * first aligned up to the specified alignment boundary, and then a bounded
 * pointer is forged from the result.
 *
 * @param T
 * The type of the pointer to forge, e.g. 'const char *'.
 *
 * @param P
 * The address from which to forge the bounded pointer.
 *
 * @param ABI
 * The ABI of the pointer to forge, cf. @__unsafe_forge.
 *
 * @param ALIGN
 * The alignment to which the address should be rounded up. Must be a power of
 * two.
 *
 * @param ...
 * Extra arguments required by the ABI.
 */
#define __unsafe_forge_aligned(T, P, ABI, ALIGN, ...) \
	__unsafe_forge(T, alignto((P), (ALIGN)), ABI, ## __VA_ARGS__)

/*!
 * @function __pointer_get_max_taggable
 * Returns the maximum tag value that may be encoded in a tagged pointer.
 *
 * @result
 * The maximum tag value that may be encoded in a tagged pointer.
 */
__export __warn_result
ptrtag_t
__pointer_get_max_taggable(
	void);

/*!
 * @function __pointer_tag_and_get
 * Adds a tag to a pointer.
 *
 * @param p
 * The pointer to which the tag should be added.
 *
 * @param tag
 * The tag to add to the pointer. A pointer with a zero tag is indistinguishable
 * from an untagged pointer.
 *
 * @result
 * The pointer with the added tag represented as an integer. This pointer cannot
 * be safely dereferenced until the tag is removed via
 * @__pointer_clr_tag_and_get. If the tag could not be represented in the
 * available tag bits, the implementation will abort.
 */
__export __warn_result
tagptr_t
__pointer_tag_and_get(
	void *__unsafe_indexable _Nullable p,
	ptrtag_t tag);

	/*!
 * @function __pointer_try_tag_and_get
 * Adds a tag to a pointer.
 *
 * @param p
 * The pointer to which the tag should be added.
 *
 * @param tag
 * The tag to add to the pointer. A pointer with a zero tag is indistinguishable
 * from an untagged pointer.
 *
 * @result
 * The pointer with the added tag represented as an integer. This pointer cannot
 * be safely dereferenced until the tag is removed via
 * @__pointer_clr_tag_and_get. If the tag could not be represented in the
 * available tag bits, a zero tag is encoded.
 */
__export __warn_result
tagptr_t
__pointer_try_tag_and_get(
	void *__unsafe_indexable _Nullable p,
	ptrtag_t tag);

/*!
 * @function __pointer_clr_tag_and_get
 * Removes a tag from a pointer and returns the resulting pointer and tag value.
 *
 * @param p
 * The pointer from which the tag should be removed.
 *
 * @param tag
 * On return, contains the tag that was removed from the pointer.
 *
 * @result
 * The untagged pointer which may be safely dereferenced.
 */
__export __warn_result __nonnull2
void *__unsafe_indexable
__pointer_clr_tag_and_get(
	tagptr_t p,
	ptrtag_t *tag);

/*!
 * @internal
 * Implementation details.
 */
// MARK: Internal
#define __forge_abi_bidi(T, P) \
	__unsafe_forge_bidi_indexable(T, P, sizeof(P))

#define __forge_abi_single(T, P) \
	__unsafe_forge_bidi_indexable(T, P, sizeof(P[0]))

#define __forge_abi_term(T, P, Z) \
	__unsafe_forge_terminated_by(T, P, Z)

#define __forge_abi_zterm(T, P) \
	__unsafe_forge_null_terminated(T, P)

#define __unsafe_forge_abi_bidi(T, P, L) \
	__unsafe_forge_bidi_indexable(T, P, L)

#define __unsafe_forge_abi_single(T, P) \
	__unsafe_forge_single(T, P)

#define __unsafe_forge_abi_term(T, P, Z) \
	__unsafe_forge_terminated_by(T, P, Z)

#define __unsafe_forge_abi_zterm(T, P) \
	__unsafe_forge_null_terminated(T, P)

__API_HEADER_END(c, nonnull, single)

#endif // __ZX_STD_POINTER_H
