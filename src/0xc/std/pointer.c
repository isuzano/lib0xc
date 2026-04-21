/*!
 * @copyright
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT license.
 */
#include "pointer.h"
#include "pointer/bits.h"

#include "0xc/std/api.h"
#include "0xc/std/int.h"
#include "0xc/std/io.h"

#include <stdint.h>
#include <stdlib.h>

// MARK: Preconditions
#if !defined(POINTER_TAG_BITS_HI)
#error "POINTER_TAG_BITS_HI must be defined"
#endif

#if !defined(POINTER_TAG_BITS_LO)
#error "POINTER_TAG_BITS_LO must be defined"
#endif

POINTER_BITS_CHECK(POINTER_TAG_BITS_HI, POINTER_TAG_BITS_LO);

// MARK: Module Globals
#if POINTER_TAG_BITS_HI > 0
static const pointer_bits_t _hi_bits = POINTER_BITS_HI(POINTER_TAG_BITS_HI);
#else
static const pointer_bits_t _hi_bits = POINTER_ZBITS;
#endif

#if POINTER_TAG_BITS_LO > 0
static const pointer_bits_t _lo_bits = POINTER_BITS_LO(POINTER_TAG_BITS_LO);
#else
static const pointer_bits_t _lo_bits = POINTER_ZBITS;
#endif

__always_inline
static inline const pointer_bits_t *
hibits(
		void)
{
	return &_hi_bits;
}

__always_inline
static inline const pointer_bits_t *
lobits(
		void)
{
	return &_lo_bits;
}

// MARK: Public
ptrtag_t
__pointer_get_max_taggable(void)
{
	size_t total = POINTER_TAG_BITS_HI + POINTER_TAG_BITS_LO;
	return (ptrtag_t)((1 << total) - 1);
}

tagptr_t
__pointer_tag_and_get(
		void *__unsafe_indexable _Nullable p,
		ptrtag_t tag)
{
	tagptr_t p_tagged = TAGPTR_NULL;
	tagptr_t p_untagged = {
		.__opaque = (uintptr_t)p,
	};

	p_tagged = __pointer_try_tag_and_get(p, tag);
	if (tag && p_tagged.__opaque == p_untagged.__opaque) {
		panicx("tag cannot be represented in pointer: tag = %#x, max tag = %#x",
				oxou(tag), oxou(__pointer_get_max_taggable()));
	}

	return p_tagged;
}

tagptr_t
__pointer_try_tag_and_get(
		void *__unsafe_indexable _Nullable p,
		ptrtag_t tag)
{
	const pointer_bits_t *hi_bits = hibits();
	const pointer_bits_t *lo_bits = lobits();
	uintptr_t p_uint = (uintptr_t)p;
	uintptr_t tag_lo = 0;
	uintptr_t tag_hi = 0;

	if (tag > __pointer_get_max_taggable()) {
		tag = 0;
	}

	// Split the tag into the components for the high bits and low bits. The
	// results will be directly masked into the pointer.
	tag_lo = (tag & lo_bits->pb_tag_mask);
	tag_hi = (tag >> lo_bits->pb_tag_bits);
	tag_hi <<= hi_bits->pb_pointer_bits;

	// Now mask off the existing tag bits in the pointer.
	p_uint &= lo_bits->pb_pointer_mask;
	p_uint &= hi_bits->pb_pointer_mask;
	p_uint |= tag_lo;
	p_uint |= tag_hi;

	return (tagptr_t){
		.__opaque = p_uint,
	};
}

void *__unsafe_indexable
__pointer_clr_tag_and_get(
		tagptr_t p,
		ptrtag_t *tag)
{
	const pointer_bits_t *hi_bits = hibits();
	const pointer_bits_t *lo_bits = lobits();
	uintptr_t p_uint = p.__opaque;
	uintptr_t tag_both = 0;
	uintptr_t tag_lo = 0;
	uintptr_t tag_hi = 0;
	uintptr_t tag_scope = 0;

	// First extract the tag bits from the pointer and reverse the operation
	// done in __pointer_tag_and_get.
	tag_both = (p_uint & (lo_bits->pb_tag_mask | hi_bits->pb_tag_mask));
	tag_lo = (tag_both & lo_bits->pb_tag_mask);
	tag_hi = (tag_both & hi_bits->pb_tag_mask);
	tag_hi >>= hi_bits->pb_pointer_bits;
	tag_hi <<= lo_bits->pb_tag_bits;
	tag_scope = (tag_lo | tag_hi);

	// Now mask off the tag bits from the pointer so it's safe to dereference.
	p_uint &= lo_bits->pb_pointer_mask;
	p_uint &= hi_bits->pb_pointer_mask;
	*tag = __cast_unsigned_unsigned(ptrtag_t, tag_scope);

	return (void *__unsafe_indexable)p_uint;
}
