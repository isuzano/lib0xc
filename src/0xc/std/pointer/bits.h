/*!
 * @copyright
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT license.
 *
 * @header
 * Tagged pointer specifications. Targets can instantiate pointer tag
 * specifications for the lower and upper bits of a pointer based entirely on
 * the number of bits available for tagging. All field derivations will
 * constant-fold.
 */
#ifndef __ZX_STD_POINTER_BITS_H
#define __ZX_STD_POINTER_BITS_H

#include "0xc/std/api.h"
#include "0xc/std/pointer.h"

#include <stdint.h>

__API_HEADER_BEGIN(c, nonnull, single);

#define POINTER_BIT_MASK_HI(N) \
	(uintptr_t)((1ull << (POINTER_WIDTH - (N))) - 1)
#define POINTER_BIT_MASK_LO(N) \
	(uintptr_t)((1ull << (N)) - 1)

// MARK: Types
typedef struct _pointer_bits {
	uint8_t pb_pointer_bits;
	uintptr_t pb_pointer_mask;
	uint8_t pb_tag_bits;
	uintptr_t pb_tag_mask;
} pointer_bits_t;

#define POINTER_BITS_HI(N) __strict_literal(pointer_bits_t){ \
	.pb_pointer_bits = POINTER_WIDTH - (N), \
	.pb_pointer_mask = POINTER_BIT_MASK_HI(N), \
	.pb_tag_bits = (N), \
	.pb_tag_mask = ~POINTER_BIT_MASK_HI(N), \
}

#define POINTER_BITS_LO(N) __strict_literal(pointer_bits_t){ \
	.pb_pointer_bits = POINTER_WIDTH - (N), \
	.pb_pointer_mask = ~POINTER_BIT_MASK_LO(N), \
	.pb_tag_bits = (N), \
	.pb_tag_mask = POINTER_BIT_MASK_LO(N), \
}

#define POINTER_ZBITS __strict_literal(pointer_bits_t){ \
	.pb_pointer_bits = POINTER_WIDTH, \
	.pb_pointer_mask = (uintptr_t)(~0ull), \
	.pb_tag_bits = 0, \
	.pb_tag_mask = (uintptr_t)0, \
}

#define POINTER_BITS_CHECK(N_HI, N_LO) \
	_Static_assert(N_HI + N_LO < POINTER_WIDTH, \
		"too many tag bits defined for pointer");

__API_HEADER_END(c, nonnull, single);

#endif // __ZX_STD_POINTER_BITS_H
