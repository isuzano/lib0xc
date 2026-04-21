/*!
 * @copyright
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT license.
 *
 * @header
 * Simple hash table in the style of the BSD queue(3) macros.
 */
#ifndef __ZX_SYS_HASH_H
#define __ZX_SYS_HASH_H

#include <0xc/std/api.h>
#include <0xc/std/array.h>
#include <0xc/std/pointer.h>
#include <0xc/std/defs.h>
#include <0xc/std/api.h>
#include <0xc/std/type.h>
#include <0xc/std/struct.h>
#include <0xc/sys/panic.h>
#include <0xc/sys/queue.h>

#include <stdint.h>
#include <stdlib.h>

__API_HEADER_BEGIN(c, nonnull, single)

// MARK: Hash Routines
/*!
 * @function memhash
 * A simple memory hash implementation using MurmurHash2.
 *
 * @param p
 * A pointer to the bytes to hash.
 *
 * @param p_len
 * The length of the buffer referred to by @p.
 *
 * @param seed
 * The seed value for the hash. May be zero, in which case the hash will be
 * unseeded.
 *
 * @result
 * The hash.
 *
 * @discussion
 * This hash is suitable for use cases such as choosing a bucket in a hash
 * table. It is not suitable for cryptographic measurements.
 */
__export __warn_result __nonnull1
uint64_t
memhash(
	const void *__sized_by(p_len) p,
	size_t p_len,
	uint64_t seed);

/*!
 * @function strhash
 * A variant of @memhash that operates on a null-terminated C string.
 *
 * @param s
 * The string.
 *
 * @param seed
 * The seed value for the hash. May be zero, in which case the hash will be
 * unseeded.
 *
 * @result
 * The hash.
 *
 * @discussion
 * See discussion for @memhash.
 */
__export __warn_result __nonnull1
uint64_t
strhash(
	const char *__cstr s,
	uint64_t seed);

// MARK: Hash Table
/*!
 * @function HASH
 * Declares a hash table consisting of nodes of the given type.
 *
 * @param NAME
 * The name of the resulting struct type. May be left blank.
 *
 * @param T
 * The type of node in the hash table omitting the 'struct' tag, e.g. for a node
 * type of 'struct foo', just 'foo'.
 *
 * @param CNT
 * The number of buckets in the hash table.
 *
 * @result
 * The hash table type.
 */
#define HASH(NAME, T, CNT) \
	struct NAME { \
		SLIST_HEAD(, T) h_buckets[CNT]; \
		unsigned long __fldinit h_koff; \
		unsigned long __fldinit h_klenoff; \
		unsigned long __fldinit h_klenlen; \
	}

/*!
 * @function HASH_INITIALIZER
 * Initializes the hash table with the properties of the given type and field.
 *
 * @param T
 * The type of node in the hash table.
 *
 * @param HASHFLD
 * The field within @T that stores the data to be hashed. This field's raw value
 * is used to perform the hash, i.e. a pointer type is not "followed". This data
 * should be stored in-band with the object.
 *
 * @param LENFLD
 * The field within @T that stores the length of the data to be hashed.
 *
 * @result
 * The initialized hash table.
 */
#define HASH_INITIALIZER(T, HF, LF) { \
	.h_koff = fldoff(struct T, HF), \
	.h_klenoff = fldoff(struct T, LF), \
	.h_klenlen = fldsiz(struct T, LF), \
}

/*!
 * @function HASH_INIT
 * Initializes an existing hash table with the properties of the given type and
 * field.
 *
 * @param H
 * The hash table.
 *
 * @param T
 * The type of node in the hash table.
 *
 * @param HF
 * The field within @T that stores the data to be hashed. This field's raw value
 * is used to perform the hash, i.e. a pointer type is not "followed". This data
 * should be stored in-band with the object.
 *
 * @param LF
 * The field within @T that stores the length of the data to be hashed.
 */
#define HASH_INIT(H, T, HF, LF) do { \
	for (size_t i_0xc = 0; i_0xc < countof((H)->h_buckets); i_0xc++) { \
		SLIST_INIT(&((H)->h_buckets[i_0xc])); \
	} \
	(H)->h_koff = fldoff(struct T, HF); \
	(H)->h_klenoff = fldoff(struct T, LF); \
	(H)->h_klenlen = fldsiz(struct T, LF); \
} while (0)

/*!
 * @function HASH_ENTRY
 * Declares a hash table entry in the object.
 *
 * @param T
 * The type of node.
 *
 * @result
 * The hash table entry.
 */
#define HASH_ENTRY(T) \
	SLIST_ENTRY(T)

/*!
 * @function HASH_INSERT
 * Inserts a node into the hash table.
 *
 * @param HASH
 * The hash table.
 *
 * @param NODE
 * The node to insert.
 *
 * @param FLD
 * The hash entry field of the node type.
 */
#define HASH_INSERT(HASH, NODE, FLD) do { \
	uint64_t mah_bucket = _HASH_BUCKET_NODE((HASH), (NODE)); \
	SLIST_INSERT_HEAD(&((HASH)->h_buckets[mah_bucket]), (NODE), FLD); \
} while (0)

/*!
 * @function HASH_LOOKUP
 * Looks up the node for a key in the hash table.
 *
 * @param HASH
 * The hash table.
 *
 * @param K
 * The key.
 *
 * @param K_LEN
 * The length of @K.
 *
 * @param FLD
 * The hash entry field of the node type.
 *
 * @result
 * If the node is found, it is returned. Otherwise, NULL is returned.
 */
#define HASH_LOOKUP(HASH, K, K_LEN, FLD) ({ \
	uint64_t mah_bucket_0xc = _HASH_BUCKET((HASH), (K), (K_LEN)); \
	__typeof_dequal((HASH)->h_buckets[0].slh_first) n_0xc = NULL; \
\
	SLIST_FOREACH(n_0xc, &((HASH)->h_buckets[mah_bucket_0xc]), FLD) { \
		const uint8_t *k_0xc = _HASH_KEY(HASH, n_0xc); \
		size_t klen_0xc = _HASH_KEYLEN(HASH, n_0xc); \
\
		if (klen_0xc == (K_LEN)) { \
			if (memcmp(k_0xc, K, klen_0xc) == 0) { \
				break; \
			} \
		} \
	} \
	(n_0xc); \
})

/*!
 * @function HASH_LOOKUP_CSTR
 * Looks up the node for a key which is a C string in the hash table. The null
 * terminator is included in the hash, and therefore the node's length must
 * reflect the total size of the string, including the terminator.
 *
 * @param HASH
 * The hash table.
 *
 * @param K
 * The key. This must be a null-terminated C string marked with the __cstr or
 * __null_terminated annotation if bounds safety is enabled.
 *
 * @param FLD
 * The hash entry field of the node type.
 *
 * @result
 * If the node is found, it is returned. Otherwise, NULL is returned.
 */
#define HASH_LOOKUP_CSTR(HASH, K, FLD) ({ \
	const char *__cstr k_cstr_0xc = (K); \
	const char *k_indexable_0xc = __unsafe_null_terminated_to_indexable(K); \
	HASH_LOOKUP(HASH, k_indexable_0xc, strlen(k_cstr_0xc) + 1, FLD); \
})

/*!
 * @function HASH_REMOVE
 * Removes the node from the hash table.
 *
 * @param HASH
 * The hash table.
 *
 * @param NODE
 * The node to remove.
 *
 * @param T
 * The type of node in the hash table.
 *
 * @param FLD
 * The hash entry field of the node type.
 */
#define HASH_REMOVE(HASH, NODE, T, FLD) do { \
	uint64_t mah_bucket_0xc = _HASH_BUCKET_NODE((HASH), (NODE)); \
	SLIST_REMOVE(&((HASH)->h_buckets[mah_bucket_0xc]), (NODE), T, FLD); \
} while (0)

/*!
 * @function HASH_FOREACH
 * Iterates the hash table.
 *
 * @param VAR
 * The variable which stores the current node.
 *
 * @param HASH
 * The hash table.
 *
 * @param FLD
 * The hash entry field of the node type.
 */
#define HASH_FOREACH(VAR, HASH, FLD) \
	for (size_t i_0xc = 0; i_0xc < countof((HASH)->h_buckets); i_0xc++) \
		SLIST_FOREACH(VAR, &((HASH)->h_buckets[i_0xc]), FLD)

/*!
 * @function HASH_FOREACH_SAFE
 * Iterates the hash table and permits removal of a node.
 *
 * @param VAR
 * The variable which stores the current node.
 *
 * @param HASH
 * The hash table.
 *
 * @param FLD
 * The hash entry field of the node type.
 *
 * @param TVAR
 * The variable which stores deletion state for the current node.
 */
#define HASH_FOREACH_SAFE(VAR, HASH, FLD, TVAR) \
	for (size_t i_0xc = 0; i_0xc < countof((HASH)->h_buckets); i_0xc++) \
		SLIST_FOREACH_SAFE(VAR, &((HASH)->h_buckets[i_0xc]), FLD, TVAR)

/*!
 * @function __hash_break
 * Breaks out of a hash table iteration. Hash table iterations expand to nested
 * loops, so the C 'break' statement will only break out of the inner loop.
 *
 * @param HASH
 * The hash table being iterated.
 */
#define __hash_break(HASH) \
	i_0xc = countof((HASH)->h_buckets); \
	break

// MARK: Internal
/*!
 * @internal
 * Implementation details.
 */
#define _HASH_KEY(HASH, NODE) ({ \
	const uint8_t *p_0xc = __forge_fromtype(uint8_t *, NODE); \
	 (p_0xc + (HASH)->h_koff); \
})

#define _HASH_KEYLEN(HASH, NODE) ({ \
	const uint8_t *p_0xc = __forge_fromtype(uint8_t *, NODE); \
	const uint8_t *p_klen_0xc = p_0xc + (HASH)->h_klenoff; \
	uint64_t klen_0xc = 0; \
\
	switch ((HASH)->h_klenlen) { \
	case 1: \
		klen_0xc = *(const uint8_t *)p_klen_0xc; \
		break; \
	case 2: \
		klen_0xc = *(const uint16_t *)p_klen_0xc; \
		break; \
	case 4: \
		klen_0xc = *(const uint32_t *)p_klen_0xc; \
		break; \
	case 8: \
		klen_0xc = *(const uint64_t *)p_klen_0xc; \
		break; \
	default: \
		panic_switch((HASH)->h_klenlen); \
	} \
	(klen_0xc); \
})

#define _HASH_BUCKET(HASH, K, K_LEN) ({ \
	uint64_t h_0xc = memhash(K, K_LEN, 0); \
	(h_0xc % countof((HASH)->h_buckets)); \
})

#define _HASH_BUCKET_NODE(HASH, NODE) ({ \
	const uint8_t *k_0xc = _HASH_KEY(HASH, NODE); \
	uint64_t klen_0xc = _HASH_KEYLEN(HASH, NODE); \
	_HASH_BUCKET(HASH, k_0xc, klen_0xc); \
})

__API_HEADER_END(c, nonnull, single)

#endif // __ZX_SYS_HASH_H
