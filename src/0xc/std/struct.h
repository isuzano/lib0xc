/*!
 * @copyright
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT license.
 *
 * @file
 * Structure macros.
 */
#ifndef __ZX_STD_STRUCT_H
#define __ZX_STD_STRUCT_H

#include <0xc/std/api.h>
#include <0xc/std/io.h>
#include <0xc/std/limits.h>
#include <0xc/std/array.h>
#include <0xc/std/int.h>
#include <0xc/std/pointer.h>
// IWYU pragma: begin_exports
#include <0xc/std/alloc.h>
#include <0xc/std/type.h>
#include <0xc/sys/panic.h>
// IWYU pragma: end_exports

#include <stdlib.h>
#include <stdint.h>
// IWYU pragma: begin_exports
#include <string.h>
// IWYU pragma: end_exports

__API_HEADER_BEGIN(c, nonnull, single)

// MARK: Supporting Types
/*!
 * @struct struct_field_t
 * A description of a field in a structure.
 *
 * @field sf_struct
 * The name of the structure type.
 *
 * @field sf_fld
 * The name of the field.
 *
 * @field sf_off
 * The offset of the field in bytes from the start of the structure.
 *
 * @field sf_size
 * The size of the field in bytes.
 *
 * @field sf_maxint
 * The maximum integer value that can be represented by the field, if it is an
 * integer type.
 *
 * @field sf_class
 * The type classification of the field.
 */
typedef struct _struct_field {
	const char *__cstr __fldinit sf_struct;
	const char *__cstr __fldinit sf_fld;
	size_t __fldinit sf_off;
	size_t __fldinit sf_size;
	uintmax_t sf_maxint;
	int sf_class;
} struct_field_t;

#define STRUCT_FIELD_INIT(STRUCT, FLD) __strict_literal(struct_field_t){ \
	.sf_struct = #STRUCT, \
	.sf_fld = #FLD, \
	.sf_off = fldoff(STRUCT, FLD), \
	.sf_size = fldsiz(STRUCT, FLD), \
	.sf_maxint = _Generic(fldof(STRUCT, FLD), \
		unsigned char : UCHAR_MAX, \
		unsigned short: USHRT_MAX, \
		unsigned int: UINT_MAX, \
		unsigned long : ULONG_MAX, \
		unsigned long long : ULLONG_MAX, \
		signed char: SCHAR_MAX, \
		signed short: SHRT_MAX, \
		signed int: INT_MAX, \
		signed long : LONG_MAX, \
		signed long long : LLONG_MAX, \
		default: 0 \
	), \
	.sf_class = __classify_type(fldof(STRUCT, FLD)), \
}

// MARK: Structure Fields
/*!
 * @function struct_field
 * Returns a pointer to a structure field descriptor.
 *
 * @param T
 * The structure type.
 *
 * @param FLD
 * The field name.
 *
 * @result
 * The pointer to the resulting structure field descriptor.
 */
#if defined(__clang__)
#define struct_field(T, FLD) \
	&(STRUCT_FIELD_INIT(T, FLD))
#else
#define struct_field(T, FLD) \
	&((const struct_field_t)STRUCT_FIELD_INIT(T, FLD))
#endif

/*!
 * @function struct_field_flexible_array
 * Returns a pointer to a structure field descriptor for a flexible array
 * member.
 *
 * @param T
 * The structure type.
 *
 * @param FLD
 * The field name.
 *
 * @result
 * The pointer to the resulting structure field descriptor. The size of the
 * flexible array field is the size of a single element; the actual size of the
 * array must be determined at runtime.
 */
#define struct_field_flexible_array(T, FLD) &(struct_field_t){ \
	.sf_struct = #T, \
	.sf_fld = #FLD, \
	.sf_off = fldoff(T, FLD), \
	.sf_size = fldsiz(T, FLD[0]), \
	.sf_maxint = 0, \
}

/*!
 * @function struct_field_get
 * Returns a pointer to the given field in a structure.
 *
 * @param T
 * The type of the returned pointer, e.g. for a field of type 'uint32_t',
 * 'uint32_t *'.
 *
 * @param SF
 * The structure field descriptor.
 *
 * @param P
 * A pointer to the structure.
 *
 * @param L
 * The length of the memory referred to by @P.
 *
 * @result
 * A pointer to the field in the structure.
 */
#define struct_field_get(T, SF, P, L) (const T __single)({ \
	size_t len_required_0xc = (SF)->sf_off + (SF)->sf_size; \
	size_t p_len_0xc = 0; \
	const char *__sized_by(p_len_0xc) p_0xc = 0; \
\
	if (len_required_0xc > (L)) { \
		panicx("insufficient length for field: " \
				"struct = %s, field = %s, requested type = %s, " \
				"off = %zu, size = %zu, required >= %zu", \
				(SF)->sf_struct, (SF)->sf_fld, #T, (SF)->sf_off, \
				(SF)->sf_size, len_required_0xc); \
	} \
\
	if (sizeof(((T)0)[0]) != (SF)->sf_size) { \
		panicx("field size mismatch: " \
				"struct = %s, field = %s, requested type = %s, " \
				"actual size = %zu, expected size = %zu", \
				(SF)->sf_struct, (SF)->sf_fld, #T, \
				sizeof(((T)0)[0]), (SF)->sf_size); \
	} \
\
	p_0xc = __unsafe_forge(const char *, P, bidi, L); \
	__bounds_update(p_len_0xc, L); \
\
	(p_0xc + (SF)->sf_off); \
})

/*!
 * @function struct_field_get_mutable
 * Returns a mutable pointer to the given field in a structure.
 *
 * @param T
 * The type of the returned pointer, e.g. for a field of type 'uint32_t',
 * 'uint32_t *'.
 *
 * @param SF
 * The structure field descriptor.
 *
 * @param P
 * A pointer to the structure.
 *
 * @param L
 * The length of the memory referred to by @P.
 *
 * @result
 * A pointer to the field in the structure.
 */
#define struct_field_get_mutable(T, SF, P, L) __DECONST(T __single, ({ \
	struct_field_get(T, SF, P, L); \
}))

/*!
 * @function struct_field_get_value
 * Returns the value of the given field in a structure.
 *
 * @param T
 * The type of the returned value, e.g. 'uint32_t' for a field of type
 * 'uint32_t'.
 *
 * @param SF
 * The structure field descriptor.
 *
 * @param P
 * A pointer to the structure.
 *
 * @param L
 * The length of the memory referred to by @P.
 *
 * @result
 * The value of the field in the structure.
 */
#define struct_field_get_value(T, SF, P, L) \
	(struct_field_get(T *, SF, P, L))[0]

/*!
 * @function struct_field_get_untyped
 * Variant of @struct_field_get which returns a pointer to the beginning of the
 * field within the provided structure.
 *
 * @param SF
 * The structure field descriptor.
 *
 * @param P
 * A pointer to the structure.
 *
 * @param L
 * The length of the memory referred to by @P.
 *
 * @result
 * A pointer to the field in the structure or NULL if the field is out of bounds
 * of the provided memory.
 */
#define struct_field_get_untyped(SF, P, L) ({ \
	size_t len_required_0xc = (SF)->sf_off + (SF)->sf_size; \
	__unused size_t p_len_0xc = 0; \
	const char *__sized_by(p_len_0xc) p_0xc = 0; \
	size_t p_fld_len_0xc = 0; \
	const char *__sized_by(p_fld_len_0xc) p_fld_0xc = NULL; \
\
	p_0xc = __unsafe_forge(const char *, P, bidi, L); \
	p_len_0xc = L; \
\
	if ((L) >= len_required_0xc) { \
		p_fld_0xc = &(p_0xc)[(SF)->sf_off]; \
		__bounds_update(p_fld_len_0xc, (SF)->sf_size); \
	} \
\
	__unsafe_forge(const void *, p_fld_0xc, bidi, p_fld_len_0xc); \
})

/*!
 * @function struct_field_get_untyped_mutable
 * Variant of @struct_field_get_untyped which returns a pointer to the beginning
 * of the field within the provided structure.
 *
 * @param SF
 * The structure field descriptor.
 *
 * @param P
 * A pointer to the structure.
 *
 * @param L
 * The length of the memory referred to by @P.
 *
 * @result
 * A pointer to the field in the structure or NULL if the field is out of bounds
 * of the provided memory.
 */
#define struct_field_get_untyped_mutable(SF, P, L) ({ \
	__DECONST(void *, struct_field_get_untyped(SF, P, L)); \
})

/*!
 * @function struct_field_get_unsafe
 * Variant of @struct_field_get which does not perform any bounds or size checks
 * on the provided structure or field descriptor. This is primarily intended to
 * initialized sized-by pointers to fields in structures without side-effects,
 * which bounds safety does not support.
 *
 * @param T
 * The type of the returned pointer, e.g. for a field of type 'uint32_t',
 * 'uint32_t *'.
 *
 * @param SF
 * The structure field descriptor.
 *
 * @param P
 * A pointer to the structure.
 *
 * @param L
 * The length of the memory referred to by @P.
 *
 * @result
 * A sized pointer referring to the specified field.
 */
#define struct_field_get_unsafe(T, SF, P, L) \
	__unsafe_forge(T, \
			__unsafe_forge(char *, P, bidi, L) + SF->sf_off, \
			bidi, SF->sf_size)

/*!
 * @function struct_field_read_integer
 * Reads an integer field from a structure into an integer value union.
 *
 * @param fld
 * The structure field descriptor.
 *
 * @param p
 * A pointer to the structure.
 *
 * @param p_len
 * The length of the memory referred to by @p.
 *
 * @param iv
 * A pointer to the integer value union in which to store the resulting value.
 *
 * @result
 * A pointer to the integer value union, or NULL if the field could not be read.
 */
__export __warn_result __nonnull1 __nonnull2 __nonnull4
integer_value_t *_Nullable
struct_field_read_integer(
	const struct_field_t *fld,
	const void *__sized_by(p_len) p,
	size_t p_len,
	integer_value_t *iv);

/*!
 * @function struct_field_get_unsigned_int
 * Returns the value of an unsigned integer field in a structure.
 *
 * @param SF
 * The structure field descriptor.
 *
 * @param P
 * A pointer to the structure.
 *
 * @param L
 * The length of the memory referred to by @P.
 *
 * @result
 * The value of the unsigned integer field in the structure.
 */
#define struct_field_get_unsigned_int(SF, P, L) (uintmax_t)({ \
	uintmax_t v_0xc = 0; \
	integer_value_t *iv_0xc = NULL; \
	integer_value_t ivmem_0xc = {0}; \
\
	switch ((SF)->sf_maxint) { \
	case INT_MAXOF(uint8_t): \
	case INT_MAXOF(uint16_t): \
	case INT_MAXOF(uint32_t): \
	case INT_MAXOF(uint64_t): \
		break; \
	default: \
		panicx("field is not an unsigned integer type: " \
				"obj = %s, field = %s, max = %llu", \
				(SF)->sf_struct, (SF)->sf_fld, oxollu((SF)->sf_maxint)); \
	} \
\
	iv_0xc = struct_field_read_integer((SF), (P), (L), &ivmem_0xc); \
	switch ((SF)->sf_size) { \
	case sizeof(uint8_t): \
		v_0xc = iv_0xc->iv_int8.vu; \
		break; \
	case sizeof(uint16_t): \
		v_0xc = iv_0xc->iv_int16.vu; \
		break; \
	case sizeof(uint32_t): \
		v_0xc = iv_0xc->iv_int32.vu; \
		break; \
	case sizeof(uint64_t): \
		v_0xc = iv_0xc->iv_int64.vu; \
		break; \
	default: \
		panic_switch((SF)->sf_size); \
	} \
\
	(v_0xc); \
})

/*!
 * @function struct_field_get_signed_int
 * Returns the value of a signed integer field in a structure.
 *
 * @param SF
 * The structure field descriptor.
 *
 * @param P
 * A pointer to the structure.
 *
 * @param L
 * The length of the memory referred to by @P.
 *
 * @result
 * The value of the signed integer field in the structure.
 */
#define struct_field_get_signed_int(SF, P, L) (intmax_t)({ \
	intmax_t v_0xc = 0; \
	integer_value_t *iv_0xc = NULL; \
	integer_value_t ivmem_0xc = {0}; \
\
	switch ((SF)->sf_maxint) { \
	case INT_MAXOF(int8_t): \
	case INT_MAXOF(int16_t): \
	case INT_MAXOF(int32_t): \
	case INT_MAXOF(int64_t): \
		break; \
	default: \
		panicx("field is not a signed integer type: " \
				"obj = %s, field = %s, max = %lld", \
				(SF)->sf_struct, (SF)->sf_fld, oxollu((SF)->sf_maxint)); \
	} \
\
	iv_0xc = struct_field_read_integer((SF), (P), (L), &ivmem_0xc); \
	switch ((SF)->sf_size) { \
	case sizeof(int8_t): \
		v_0xc = iv_0xc->iv_int8.v; \
		break; \
	case sizeof(int16_t): \
		v_0xc = iv_0xc->iv_int16.v; \
		break; \
	case sizeof(int32_t): \
		v_0xc = iv_0xc->iv_int32.v; \
		break; \
	case sizeof(int64_t): \
		v_0xc = iv_0xc->iv_int64.v; \
		break; \
	default: \
		panic_switch((SF)->sf_size); \
	} \
\
	(v_0xc); \
})

// MARK: Public
/*!
 * @function zstruct
 * Initializes storage for a structure.
 *
 * @param T
 * The type for which to declare storage.
 *
 * @result
 * The initializer for the type.
 */
#define zstruct(T) __strict_literal(T){}

/*!
 * @function __new
 * Creates a new structure of the specified type and initializes it with the
 * specified initializer list.
 *
 * @param T
 * The type of structure to create, e.g. `struct foo`.
 *
 * @param ...
 * The initializer list. This may be empty, in which case the resulting
 * structure will be zero-initialized on C23+.
 *
 * @result
 * A pointer to the resulting structure. The memory referred to by this pointer
 * will be freed when the calling scope exits.
 *
 * @discussion
 * This macro enables using a cleanup attribute on a pointer to an object which
 * has a destructor for resources it holds (rather than its own memory). For
 * example, the @buff_t object has a destructor which frees the memory the
 * buffer manages, but the buffer object itself is typically allocated on the
 * stack. Using this macro allows creating such an object in automatic storage
 * without having to declare a separate 'buff_t' and 'buff_t *'.
 *
 * Since the resulting pointer points to an object with the same lifetime as the
 * pointer itself, we don't have to worry about e.g. a cleanup handler dealing
 * with a pointer to bogus stack memory. As long as the pointer is valid, the
 * object it points to is also valid.
 */
#if !defined(__cplusplus)
#define __new(T, ...) (T *__single)&((T){ \
	__VA_ARGS__ \
})
#else
#define __new(...) HALT("__new not available on this compiler")
#endif

/*!
 * @function __newx
 * Variant of @__new which creates a new structure of the specified type with a
 * flexible array member.
 *
 * @param T
 * The type of structure to create, e.g. `struct foo`.
 *
 * @param F
 * The flexible array member field of @T.
 *
 * @param CF
 * The flexible array count member field of @T. This field will be set to the
 * value provided in @N.
 *
 * @param N
 * The count of items in the flexible array member.
 *
 * @result
 * A pointer to the resulting structure. The memory referred to by this pointer
 * will be freed when the calling function returns.
 *
 * @discussion
 * This macro intentionally omits an initializer list parameter because bounds
 * safety forbids passing structures with flexible array members by value, and
 * therefore such structures cannot be initialized with designated initializers.
 */
#if !defined(__cplusplus)
#define __newx(T, F, CF, N) (T *__single)({ \
	size_t n_0xc = __assert_positive_and_get(size_t, N); \
	void *__unsafe_indexable p_unsafe_0xc = NULL; \
	size_t p_len_0xc = sizeof(T) + spanof(fldtypeof(T, F[0]), n_0xc); \
	void *__sized_by_or_null(p_len_0xc) p_0xc = NULL; \
	size_t obj_len_0xc = 0; \
	T *__sized_by(obj_len_0xc) obj_0xc = NULL; \
\
	p_unsafe_0xc = alloca(p_len_0xc); \
	p_0xc = __unsafe_forge(void *, p_unsafe_0xc, bidi, p_len_0xc); \
	__bounds_affirm(p_len_0xc); \
	memset(p_0xc, 0, p_len_0xc); \
\
	obj_0xc = p_0xc; \
	__bounds_update(obj_len_0xc, p_len_0xc); \
	obj_0xc->CF = (N); \
	obj_0xc; \
})
#else
#define __newx(...) \
	HALT("__newx not available on this compiler")
#endif

/*!
 * @function fldof
 * Expands to an expression whose resulting type is that of the specified field
 * in a structure type.
 *
 * @param T
 * The structure type.
 *
 * @param F
 * The field name.
 *
 * @result
 * The field in the structure. This can only be used for directives such as
 * sizeof, which do not evaluate their arguments.
 */
#define fldof(T, F) \
	(((T *)0)->F)

/*!
 * @function fldtypeof
 * Returns the type of the field.
 *
 * @param T
 * The structure type.
 *
 * @param F
 * The field name.
 *
 * @result
 * The type of the field.
 */
#define	fldtypeof(T, F) \
	typeof(fldof(T, F))

/*!
 * @function fldoff
 * Returns the offset in bytes of a field in a structure.
 *
 * @param T
 * The structure type.
 *
 * @param F
 * The field name.
 *
 * @result
 * The offset in bytes.
 */
#define	fldoff(T, F) \
	__builtin_offsetof(T, F)

/*!
 * @function fldsiz
 * Returns the size in bytes of a field in a structure.
 *
 * @param T
 * The structure type.
 *
 * @param F
 * The field name.
 *
 * @result
 * The size in bytes.
 */
#define	fldsiz(T, F) \
	(sizeof(fldof(T, F)))

/*!
 * @function fldbase
 * Returns the base of a struct from a given address and field.
 *
 * @param T
 * The structure type.
 *
 * @param F
 * The field name.
 *
 * @param ADDR
 * The address of the field referred to by @F.
 *
 * @result
 * The address of the structure containing the field.
 */
#define	fldbase(T, F, ADDR) \
	((T *)((char *)(ADDR) - fldoff(T, F)))

/*!
 * @function flexsizeof
 * Returns the size in bytes of a structure with a flexible array member.
 *
 * @param OBJ
 * A pointer to the structure.
 *
 * @param FLA
 * The flexible array member field name.
 *
 * @param FLACNT
 * The field name which contains the count of items in the flexible array.
 *
 * @result
 * The size in bytes of the structure including the flexible array member.
 */
#define flexsizeof(OBJ, FLA, FLACNT) ({ \
	size_t cnt_0xc = __assert_positive_and_get(size_t, (OBJ)->FLACNT); \
	(sizeof(*OBJ) + spanof(fldtypeof(typeof(*(OBJ)), FLA[0]), cnt_0xc)); \
})

/*!
 * @function structcmp
 * Compares two structures for equality. This check is a simple byte comparison
 * of the two structure arguments.
 *
 * @param A
 * The first structure.
 *
 * @param B
 * The second structure.
 *
 * @result
 * Zero if the structures are equal, non-zero otherwise.
 *
 * @discussion
 * This macro includes both a type compatibility check and a size check
 * deliberately since there are cases where compatible types may have different
 * sizes, e.g. 'int []' and 'int [5]' are compatible. The size check ensures
 * that attempting to use an incomplete type will result in a compiler error,
 * since sizeof() cannot be applied to incomplete types.
 *
 * This macro is only generally available on C23 or later since it relies on the
 * ability to easily initialize the memory occupied by a structure to all
 * zeroes, which which is the behavior specified by C23 for any partially-
 * initialized structure (and includes padding bytes).
 *
 * Prior to C23, this macro is only available for structures which have no
 * padding bytes.
 */
#define structcmp(A, B) ({ \
	__assert_packed_struct_pre_c23(typeof(*(A)), structcmp); \
	__assert_type_compat(typeof(*(A)), typeof(*(B)), \
			"structure arguments must be compatible types"); \
	if (sizeof(*(A)) != sizeof(*(B))) { \
		panicx("size mismatch: %zu != %zu", sizeof(*(A)), sizeof(*(B))); \
	} \
	memcmp((A), (B), sizeof(*(A))); \
})

/*!
 * @internal
 * Implementation details.
 */
#if __STDC_VERSION__ < 202311L
#define __assert_packed_struct_pre_c23(T, API) \
	_Static_assert(_Alignof(T) == 1, \
			#API " requires struct type with no padding constraints")
#else
#define __assert_packed_struct_pre_c23(T, API)
#endif

__API_HEADER_END(c, nonnull, single)

#endif // __ZX_STD_STRUCT_H
