/*!
 * @copyright
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT license.
 *
 * @file
 * Buffer object that exposes ownership semantics and bounded access.
 */
#ifndef __ZX_SYS_BUFF_H
#define __ZX_SYS_BUFF_H

#include <0xc/std/api.h>
#include <0xc/std/struct.h>
#include <0xc/std/pointer.h>
#include <0xc/std/int.h>
#include <0xc/std/array.h>
#include <0xc/sys/log.h>
#include <0xc/sys/errno.h>

#include <stdlib.h>
#include <stdint.h>

// IWYU pragma: begin_exports
#include <0xc/sys/buff/type.h>
// IWYU pragma: end_exports

__API_HEADER_BEGIN(c, nonnull, single)

// MARK: Forward Types
typedef struct _buff buff_t;

// MARK: Object
/*!
 * @const BUFF_VERSION
 * The version of the @buff_t structure supported by the implementation.
 */
#define BUFF_VERSION STRUCT_VERSION(0)

/*!
 * @struct buff_t
 * A representation of a memory buffer. Even though the structure definition is
 * public, all fields should be treated as opaque. All interaction with this
 * object should be via the APIs in this header.
 *
 * @field bf_version
 * The version of the structure.
 *
 * @field log_proto
 * The log protocol.
 *
 * @field bf_type
 * The buffer type.
 *
 * @field bf_name
 * The name of the buffer.
 *
 * @field bf_parent
 * The parent buffer, if this is a cursor buffer.
 *
 * @field bf_lender
 * The buffer which has lent memory to this buffer.
 *
 * @field bf_loanee
 * The buffer which has taken a loan of this buffer's memory.
 *
 * @field bf_p
 * A pointer to the memory managed by the buffer.
 *
 * @field bf_len
 * The length of the memory referred to by @bf_p.
 */
struct _buff {
	struct_version_t __fldinit bf_version;
	const log_proto_t *log_proto;
	const buff_type_t *bf_type;
	const char *_Nullable __cstr bf_name;
	char bf_name_mem[64];
	const buff_t *_Nullable bf_parent;
	buff_t *_Nullable bf_lender;
	const buff_t *_Nullable bf_loanee;
	const uint8_t *__counted_by(bf_len) bf_p;
	size_t bf_len;
};
#define BUFF_INIT __strict_literal(buff_t){ \
	.bf_version = BUFF_VERSION, \
	.log_proto = LOG_PROTO_BUFF, \
	.bf_type = NULL, \
	.bf_name = NULL, \
	.bf_parent = NULL, \
	.bf_p = NULL, \
	.bf_len = 0, \
}

// MARK: Log Protocol
__private
const log_proto_t _log_proto_buff;
#define LOG_PROTO_BUFF __CONSTANT(log_proto, buff)

// MARK: Attributes
/*!
 * @const __buff_destroy
 * An attribute that can be attached to a @buff_t pointer which will cause it to
 * be destroyed at end-of-scope. This attribute forces the pointer to be a
 * single pointer when bounds safety is enabled.
 *
 * @example
 * buff_t *__buff_destroy bf = buff_new();
 */
#define __buff_destroy __single __cleanup(__buff_destroy_cleanup)

// MARK: Public
/*!
 * @function buff_new
 * Allocates a new buffer object with the appropriate version.
 *
 * @result
 * A pointer to the object. This object resides in automatic storage and must be
 * initialized with @buff_init or similar to be used.
 */
#define buff_new() __new(buff_t, \
	.bf_version = BUFF_VERSION, \
)

/*!
 * @function buff_static
 * Initialize a buffer object with a static memory region.
 *
 * @param NAME
 * The name of the buffer.
 *
 * @param ARR
 * The static memory region to manage with the buffer.
 *
 * @result
 * A buffer object initialized to manage the memory region.
 */
#define buff_static(NAME, ARR) { \
	.bf_version = BUFF_VERSION, \
	.log_proto = LOG_PROTO_BUFF, \
	.bf_type = BUFF_TYPE_NULL, \
	.bf_name = NAME, \
	.bf_parent = NULL, \
	.bf_p = ARR, \
	.bf_len = sizeof(ARR), \
}; \
__assert_static_array(ARR, "must initialize with a statically-allocated array")

/*!
 * @function buff_init
 * Transfer ownership of a memory region to a buffer object.
 *
 * @param bf
 * The buffer to initialize.
 *
 * @param type
 * The buffer type.
 *
 * @param name
 * The name of the buffer. May be NULL.
 *
 * @param p
 * A pointer to the pointer to the memory region. Will be set to NULL upon
 * return.
 *
 * @param p_len
 * A pointer to the length of the memory referred to by @p. Will be set to zero
 * upon return.
 *
 * @result
 * A pointer to the buffer. The caller must dispose of the buffer with
 * @buff_destroy when it is no longer needed.
 */
__export __warn_result __nonnull1 __nonnull2 __nonnull4
buff_t *
buff_init(
	buff_t *bf,
	const buff_type_t *type,
	const char *_Nullable __cstr name,
	void *_Nonnull __sized_by(*p_len) *const _Nonnull __single p,
	size_t *p_len);

/*!
 * @function buff_init_wrap
 * Wrap a memory region in a buffer.
 *
 * @param bf
 * The buffer to initialize.
 *
 * @param name
 * The name of the buffer. May be NULL.
 *
 * @param p
 * The pointer to the memory region.
 *
 * @param p_len
 * The length of the memory referred to by @p.
 *
 * @result
 * A pointer to the buffer.
 */
__export __nonnull1 __nonnull3
buff_t *
buff_init_wrap(
	buff_t *bf,
	const char *_Nullable __cstr name,
	const void *__sized_by(p_len) p,
	size_t p_len);

/*!
 * @function buff_borrow
 * Creates a buffer which aliases the memory of another buffer. The original
 * buffer may not be destroyed while the alias is in use.
 *
 * @param bf
 * The buffer to initialize.
 *
 * @param bf_lender
 * The original buffer whose memory is to be aliased.
 *
 * @result
 * A pointer to the alias buffer.
 */
__export __nonnull1 __nonnull2
buff_t *
buff_borrow(
	buff_t *bf,
	buff_t *bf_lender);

/*!
 * @function buff_cursor
 * Creates a cursor buffer at a given offset within a parent buffer.
 *
 * @param bf
 * The buffer to initialize.
 *
 * @param bf_parent
 * The buffer from which to create the slice.
 *
 * @param off
 * The offset into the parent buffer at which the cursor begins.
 *
 * @result
 * A pointer to the cursor buffer.
 */
__export __warn_result __nonnull1 __nonnull2
buff_t *_Nullable
buff_cursor(
	buff_t *bf,
	const buff_t *bf_parent,
	size_t off);

/*!
 * @function buff_cursor_static
 * Creates a cursor buffer at a given offset within a parent buffer, with the
 * cursor residing in static storage.
 *
 * @param BF_PARENT
 * The buffer from which to create the slice.
 *
 * @param ARR
 * The static memory region to track with the cursor buffer. This must be the
 * same region as the parent buffer's memory, but is required to be passed in to
 * ensure that the cursor buffer itself can reside in static storage.
 *
 * @result
 * A cursor buffer initialized with the given parent and offset, residing in
 * static storage.
 */
#define buff_cursor_static(BF_PARENT, ARR) { \
	.bf_version = BUFF_VERSION, \
	.log_proto = LOG_PROTO_BUFF, \
	.bf_type = BUFF_TYPE_CURSOR, \
	.bf_name = "cursor<" #BF_PARENT ">", \
	.bf_parent = (BF_PARENT), \
	.bf_p = (ARR), \
	.bf_len = sizeof(ARR), \
}; \
__assert_static_array(ARR, "must initialize with a statically-allocated array")

/*!
 * @function buff_map
 * Maps a region of the buffer for direct access.
 *
 * @param bf
 * The buffer.
 *
 * @param bf_cursor
 * The cursor buffer which tracks where the mapping should begin. Upon return,
 * the cursor's offset will be advanced by the size of the mapping.
 *
 * May be NULL, in which case the mapping begins at the start of the buffer.
 *
 * @param len
 * The length of the mapping.
 *
 * @result
 * A pointer to the mapped region, or NULL if the buffer does not have
 * sufficient space for the requested mapping.
 */
__export __warn_result __nonnull1
void *_Nullable __sized_by_or_null(len)
buff_map(
	const buff_t *bf,
	buff_t *_Nullable bf_cursor,
	size_t len);

/*!
 * @function buff_map_object
 * Maps an object of the given type from the buffer.
 *
 * @param BF
 * The buffer.
 *
 * @param BFC
 * The cursor buffer which tracks where the map should begin. Upon return, the
 * cursor's offset will be advanced by the size of the object mapped.
 *
 * May be NULL, in which case the object is mapped from the start of the buffer.
 *
 * @param T
 * The type of object to map.
 *
 * @result
 * A pointer to the object mapped from the buffer, or NULL if the buffer does
 * not have sufficient space for the requested object.
 */
#define buff_map_object(BF, BFC, T) (T *__single)({ \
	buff_object_descriptor_t bfod_0xc = { \
		.bfod_version = BUFF_OBJECT_DESCRIPTOR_VERSION, \
		.bfod_objname = #T, \
		.bfod_obj = NULL, \
		.bfod_objsize = sizeof(T), \
		.bfod_fla = NULL, \
		.bfod_flacnt = NULL, \
	}; \
	void *__single p_0xc = NULL; \
	size_t p_len_0xc = 0; \
\
	p_0xc = _buff_map_object((BF), (BFC), &bfod_0xc, &p_len_0xc); \
	if (p_0xc && (p_len_0xc != sizeof(T))) { \
		panicx("object map inconsistency: " \
				"type = %s, actual = %zu, expected = %lu", \
				#T, p_len_0xc, sizeof(T)); \
	} \
	p_0xc ? __forge_type(T *, p_0xc, p_len_0xc) : NULL; \
})

/*!
 * @function buff_map_object_flexible
 * Maps a variably-sized object from the buffer. The object is expected to
 * contain a length field which indicates the size of a flexible array at the
 * end of the object.
 *
 * @param BF
 * The buffer.
 *
 * @param BFC
 * The cursor buffer which tracks where the map should begin. Upon return, the
 * cursor's offset will be advanced by the size of the object mapped.
 *
 * May be NULL, in which case the object is mapped from the start of the buffer.
 *
 * @param T
 * The type of object to map.
 *
 * @param FLA
 * The field in the object which contains the the flexible array.
 *
 * @param FLACNT
 * The field in the object which contains the length of the flexible array.
 *
 * @param N
 * On return, will be set to the number of bytes mapped from the buffer.
 *
 * @result
 * A pointer to the object mapped from the buffer, or NULL if the buffer does
 * not contain the complete object.
 */
#define buff_map_object_flexible(BF, BFC, T, FLA, FLACNT, N) \
		(T *__sized_by(*(N)))({ \
	const buff_object_descriptor_t bfod_0xc = { \
		.bfod_version = BUFF_OBJECT_DESCRIPTOR_VERSION, \
		.bfod_objname = #T, \
		.bfod_obj = NULL, \
		.bfod_objsize = sizeof(T), \
		.bfod_fla = __strict_literal_addr(struct_field_t){ \
			.sf_struct = #T, \
			.sf_fld = #FLA, \
			.sf_off = fldoff(T, FLA), \
			.sf_size = sizeof(fldtypeof(T, FLA[0])), \
			.sf_maxint = 0, \
		}, \
		.bfod_flacnt = struct_field(T, FLACNT), \
	}; \
	void *__single p_0xc = NULL; \
\
	p_0xc = _buff_map_object((BF), (BFC), &bfod_0xc, (N)); \
	__unsafe_forge(T *, p_0xc, bidi, *(N)); \
})

/*!
 * @function buff_read
 * Reads a sequence of bytes from the buffer.
 *
 * @param bf
 * The buffer.
 *
 * @param bf_cursor
 * The cursor buffer which tracks where the read should begin. Upon return, the
 * cursor's offset will be advanced by the number of bytes read.
 *
 * May be NULL, in which case bytes are read from the start of the buffer.
 *
 * @param n
 * The number of bytes to read.
 *
 * @result
 * A pointer to the bytes read from the buffer, or NULL if the buffer does not
 * have sufficient space for the requested read.
 */
__export __warn_result __nonnull1
const uint8_t *_Nullable __sized_by_or_null(n)
buff_read(
	const buff_t *bf,
	buff_t *_Nullable bf_cursor,
	size_t n);

/*!
 * @function buff_read_object
 * Reads an object of the given type from the buffer.
 *
 * @param BF
 * The buffer.
 *
 * @param BFC
 * The cursor buffer which tracks where the read should begin. Upon return, the
 * cursor's offset will be advanced by the size of the object read.
 *
 * May be NULL, in which case the object is read from the start of the buffer.
 *
 * @param T
 * The type of object to read.
 *
 * @result
 * A pointer to the object read from the buffer, or NULL if the buffer does not
 * have sufficient space for the requested object.
 */
#define buff_read_object(BF, BFC, T) (const T *__single)({ \
	const buff_object_descriptor_t bfod_0xc = { \
		.bfod_version = BUFF_OBJECT_DESCRIPTOR_VERSION, \
		.bfod_objname = #T, \
		.bfod_obj = NULL, \
		.bfod_objsize = sizeof(T), \
		.bfod_fla = NULL, \
		.bfod_flacnt = NULL, \
	}; \
	const void *__single p_0xc = NULL; \
	size_t p_len_0xc = 0; \
\
	p_0xc = _buff_read_object((BF), (BFC), &bfod_0xc, &p_len_0xc); \
	if (p_0xc && (p_len_0xc != sizeof(T))) { \
		panicx("object read inconsistency: " \
				"type = %s, actual = %zu, expected = %lu", \
				#T, p_len_0xc, sizeof(T)); \
	} \
	p_0xc ? __forge_type(const T *, p_0xc, p_len_0xc) : NULL; \
})

/*!
 * @function buff_read_object_flexible
 * Reads a variably-sized object from the buffer. The object is expected to
 * contain a length field which indicates the size of a flexible array at the
 * end of the object.
 *
 * @param BF
 * The buffer.
 *
 * @param BFC
 * The cursor buffer which tracks where the read should begin. Upon return, the
 * cursor's offset will be advanced by the size of the object read.
 *
 * May be NULL, in which case the object is read from the start of the buffer.
 *
 * @param T
 * The type of object to read.
 *
 * @param FLA
 * The field in the object which contains the the flexible array.
 *
 * @param FLACNT
 * The field in the object which contains the length of the flexible array.
 *
 * @param N
 * On return, will be set to the number of bytes read from the buffer.
 *
 * @result
 * A pointer to the object read from the buffer, or NULL if the buffer does not
 * contain the complete object.
 */
#define buff_read_object_flexible(BF, BFC, T, FLA, FLACNT, N) \
		(const T *__sized_by(*(N)))({ \
	const buff_object_descriptor_t bfod_0xc = { \
		.bfod_version = BUFF_OBJECT_DESCRIPTOR_VERSION, \
		.bfod_objname = #T, \
		.bfod_obj = NULL, \
		.bfod_objsize = sizeof(T), \
		.bfod_fla = struct_field_flexible_array(T, FLA), \
		.bfod_flacnt = struct_field(T, FLACNT), \
	}; \
	const void *__single p_0xc = NULL; \
\
	p_0xc = _buff_read_object((BF), (BFC), &bfod_0xc, (N)); \
	__unsafe_forge(const T *, p_0xc, bidi, *(N)); \
})

/*!
 * @function buff_write
 * Writes a sequence of bytes to the buffer.
 *
 * @param bf
 * The buffer.
 *
 * @param bf_cursor
 * The cursor buffer which tracks where the write should begin. Upon return, the
 * cursor's offset will be advanced by the number of bytes written.
 *
 * May be NULL, in which case bytes are written to the start of the buffer.
 *
 * @param p
 * A pointer to the bytes to write.
 *
 * @param p_len
 * The number of bytes to write.
 *
 * @result
 * A pointer to the location in the buffer where the bytes were written, or NULL
 * if the buffer does not have sufficient space for the requested write.
 */
__export __warn_result __nonnull1 __nonnull3
const uint8_t *_Nullable __sized_by_or_null(p_len)
buff_write(
	const buff_t *bf,
	buff_t *_Nullable bf_cursor,
	const uint8_t *__sized_by(p_len) p,
	size_t p_len);

/*!
 * @function buff_write_object
 * Writes an object of the given type to the buffer.
 *
 * @param BF
 * The buffer.
 *
 * @param BFC
 * The cursor buffer which tracks where the write should begin. Upon return, the
 * cursor's offset will be advanced by the size of the object write.
 *
 * May be NULL, in which case the object is written to the start of the buffer.
 *
 * @param T
 * The type of object to write.
 *
 * @param OBJ
 * A pointer to the object to write.
 *
 * @result
 * A pointer to the object written into the buffer, or NULL if the buffer does
 * not have sufficient space for the requested object.
 */
#define buff_write_object(BF, BFC, T, OBJ) (const typeof(*OBJ) *__single)({ \
	buff_object_descriptor_t bfod_0xc = { \
		.bfod_version = BUFF_OBJECT_DESCRIPTOR_VERSION, \
		.bfod_objname = #T, \
		.bfod_obj = (OBJ), \
		.bfod_objsize = sizeof(T), \
		.bfod_fla = NULL, \
		.bfod_flacnt = NULL, \
	}; \
	const void *__single p_0xc = NULL; \
	size_t p_len_0xc = 0; \
\
	p_0xc = _buff_write_object((BF), (BFC), &bfod_0xc, &p_len_0xc); \
	if (p_0xc && (p_len_0xc != sizeof(T))) { \
		panicx("object write inconsistency: " \
				"type = %s, actual = %zu, expected = %lu", \
				#T, p_len_0xc, sizeof(T)); \
	} \
	p_0xc ? __forge_type(const T *, p_0xc, p_len_0xc) : NULL; \
})

/*!
 * @function buff_write_object_flexible
 * Writes a variably-sized object to the buffer. The object is expected to
 * contain a length field which indicates the size of a flexible array at the
 * end of the object.
 *
 * @param BF
 * The buffer.
 *
 * @param BFC
 * The cursor buffer which tracks where the write should begin. Upon return, the
 * cursor's offset will be advanced by the size of the object write.
 *
 * May be NULL, in which case the object is written to the start of the buffer.
 *
 * @param T
 * The type of object to write.
 *
 * @param FLA
 * The field in the object which contains the the flexible array.
 *
 * @param FLACNT
 * The field in the object which contains the length of the flexible array.
 *
 * @param OBJ
 * A pointer to the object to write.
 *
 * @param N
 * On return, will be set to the number of bytes written to the buffer.
 *
 * @result
 * A pointer to the object written into the buffer, or NULL if the buffer does
 * not have sufficient space for the requested object.
 */
#define buff_write_object_flexible(BF, BFC, T, FLA, FLACNT, OBJ, N) \
		(const T *__sized_by(*(N)))({ \
	size_t fla_len_0xc = flexsizeof(OBJ, FLA, FLACNT); \
	buff_object_descriptor_t bfod_0xc = { \
		.bfod_version = BUFF_OBJECT_DESCRIPTOR_VERSION, \
		.bfod_objname = #T, \
		.bfod_obj = (OBJ), \
		.bfod_objsize = fla_len_0xc, \
		.bfod_fla = struct_field_flexible_array(T, FLA), \
		.bfod_flacnt = struct_field(T, FLACNT), \
	}; \
	const void *__single p_0xc = NULL; \
\
	p_0xc = _buff_write_object((BF), (BFC), &bfod_0xc, (N)); \
	__unsafe_forge(const T *, p_0xc, bidi, *(N)); \
})

/*!
 * @function buff_destroy
 * Destroy a buffer.
 *
 * @param bfp
 * A pointer to the buffer to destroy. Will be set to NULL upon return. If the
 * buffer referred to by the pointer is NULL, then this is a no-op.
 */
__export __nonnull1
void
buff_destroy(
	buff_t *_Nullable *_Nonnull bfp);

// MARK: Internal
/*!
 * @internal
 * Implementation details. Do not call directly.
 */
typedef struct _buff_object_descriptor {
	struct_version_t bfod_version;
	const char *__cstr bfod_objname;
	const void *_Nullable __sized_by_or_null(bfod_objsize) bfod_obj;
	size_t bfod_objsize;
	const struct_field_t *_Nullable bfod_fla;
	const struct_field_t *_Nullable bfod_flacnt;
} buff_object_descriptor_t;
#define BUFF_OBJECT_DESCRIPTOR_VERSION STRUCT_VERSION(0)

__export __warn_result __nonnull1 __nonnull3 __nonnull4
const void *_Nullable __single
_buff_read_object(
	const buff_t *bf,
	buff_t *_Nullable bf_cursor,
	const buff_object_descriptor_t *bod,
	size_t *n_read);

__export __warn_result __nonnull1 __nonnull3 __nonnull4
const void *_Nullable __single
_buff_write_object(
	const buff_t *bf,
	buff_t *_Nullable bf_cursor,
	const buff_object_descriptor_t *bod,
	size_t *n_written);

__export __warn_result __nonnull1 __nonnull3
void *_Nullable __single
_buff_map_object(
	const buff_t *bf,
	buff_t *_Nullable bf_cursor,
	const buff_object_descriptor_t *bod,
	size_t *n_mapped);

__inline_impl __nonnull1
static inline void
__buff_destroy_cleanup(
		buff_t *_Nullable __single *_Nonnull __unsafe_indexable __p)
{
	buff_t *__single bf = *__p;
	buff_destroy(&bf);
}

__API_HEADER_END(c, nonnull, single)

#endif // __ZX_SYS_BUFF_H
