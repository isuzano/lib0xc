/*!
 * @copyright
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT license.
 *
 * @header
 * FourCC object.
 */
#ifndef __ZX_SYS_FOURCC_H
#define __ZX_SYS_FOURCC_H

#include <0xc/std/api.h>
#include <0xc/std/int.h>
#include <0xc/std/struct.h>

#include <stdint.h>

#if !defined(__clang__) && defined(__GNUC__)
// GCC has an issue with pragmas being used within statements, so if this header
// gets included, just disable the warning globally.
#pragma GCC diagnostic ignored "-Wmultichar"
#endif

__API_HEADER_BEGIN(c, nonnull, single)

// MARK: Supporting Types
typedef union __aligned(4) _fourcc_union {
	uint32_t fccu_network;
	char fccu_string[4];
} fourcc_union_t;

// MARK: Object
typedef struct __aligned(4) _fourcc {
	uint32_t fcc_code;
	fourcc_union_t fcc_printable;
	char fcc_term;
} fourcc_t;

#define FOURCC_INIT __strict_literal(fourcc_t){ \
	.fcc_code = 0, \
	.fcc_printable.fccu_string = zstruct(char [4]), \
	.fcc_term = 0, \
}

// MARK: Public
/*!
 * @function FOURCC
 * Initializes a constant four-character code.
 *
 * @param V
 * The four-character constant.
 *
 * @discussion
 * The @__FOURCC variant is available in the event of a naming conflict with
 * @FOURCC.
 */
#if defined(__clang__)
#define FOURCC(V) \
	_Pragma("clang diagnostic push") \
	_Pragma("clang diagnostic ignored \"-Wfour-char-constants\"") \
		(fourcc_t){ \
			.fcc_code = (uint32_t)V, \
			.fcc_printable.fccu_network = h2n((uint32_t)V), \
			.fcc_term = 0, \
		} \
	_Pragma("clang diagnostic pop")

#define __FOURCC(V) \
	_Pragma("clang diagnostic push") \
	_Pragma("clang diagnostic ignored \"-Wfour-char-constants\"") \
		(fourcc_t){ \
			.fcc_code = (uint32_t)V, \
			.fcc_printable.fccu_network = h2n((uint32_t)V), \
			.fcc_term = 0, \
		} \
	_Pragma("clang diagnostic pop")

#define __FOURCC_U32(V) \
	_Pragma("clang diagnostic push") \
	_Pragma("clang diagnostic ignored \"-Wfour-char-constants\"") \
		(uint32_t)V \
	_Pragma("clang diagnostic pop")
#elif defined(__GNUC__)
#define FOURCC(V) \
		(fourcc_t){ \
			.fcc_code = (uint32_t)V, \
			.fcc_printable.fccu_network = h2n((uint32_t)V), \
			.fcc_term = 0, \
		}
#define __FOURCC(V) \
		(fourcc_t){ \
			.fcc_code = (uint32_t)V, \
			.fcc_printable.fccu_network = h2n((uint32_t)V), \
			.fcc_term = 0, \
		}

#define __FOURCC_U32(V) (uint32_t)V
#endif

/*!
 * @function fourcc_new
 * Allocates and initializes a new four-character code.
 *
 * @param V
 * The four-character constant.
 *
 * @result
 * A pointer to the object. This object resides in automatic storage.
 */
#if defined(__clang__)
#define fourcc_new(V) \
	_Pragma("clang diagnostic push") \
	_Pragma("clang diagnostic ignored \"-Wfour-char-constants\"") \
		__new(fourcc_t, \
			.fcc_code = (uint32_t)V, \
			.fcc_printable.fccu_network = h2n((uint32_t)V), \
			.fcc_term = 0, \
		) \
	_Pragma("clang diagnostic pop")
#elif defined(__GNUC__)
#define fourcc_new(V) \
		__new(fourcc_t, \
			.fcc_code = (uint32_t)V, \
			.fcc_printable.fccu_network = h2n((uint32_t)V), \
			.fcc_term = 0, \
		)
#endif

/*!
 * @function fourcc_init
 * Initializes a FourCC object.
 *
 * @param fcc
 * The FourCC object.
 *
 * @param v
 * The four-character code value.
 */
__export
const fourcc_t *
fourcc_init(
	fourcc_t *fcc,
	uint32_t v);

/*!
 * @function fourcc_get_string
 * Returns the C-string representation of the FourCC.
 *
 * @param fcc
 * The FourCC object.
 */
__export __warn_result
const char *__cstr
fourcc_get_string(
	const fourcc_t *fcc);

/*!
 * @function fourcc_compare
 * Checks whether a FourCC object matches a given value.
 *
 * @param fcc
 * The FourCC object.
 *
 * @param v
 * The four-character code value to check against.
 *
 * @result
 * Zero if the FourCC matches the value, or a nonzero value otherwise.
 */
__export __warn_result __nonnull1
int
_fourcc_compare(
	const fourcc_t *fcc,
	uint32_t v);
#define fourcc_compare(FCC, V) \
	_fourcc_compare((FCC), __FOURCC_U32(V))

__API_HEADER_END(c, nonnull, single)

#endif // __ZX_FOURCC_H
