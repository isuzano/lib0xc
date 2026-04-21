/*!
 * @copyright
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT license.
 */
#include "0xc/sys/check.h"
#include "0xc/sys/unit.h"
#include "0xc/std/alloc.h"

#include "0xc/std/api.h"
#include "0xc/std/rand.h"

#include <stdint.h>
#include <stdlib.h>

#if __has_include(<malloc/malloc.h>)
#include <malloc/malloc.h>
#endif

#ifndef __OPEN_SOURCE__
#if __has_include("0xc/funos/std/rand.h")
#include "0xc/funos/std/rand.h"
#endif
#endif

#if __has_ptrcheck
#include "0xc/std/pointer.h"
#endif

// MARK: IWYU
DECL_FUNC_4IWYU(arc4random);

// MARK: Types
typedef struct _test_object {
	uint32_t a;
	uint64_t b;
	uint32_t c;
} test_object_t;

typedef struct _test_object_with_extra {
	uint32_t extra_size;
	uint8_t extra_data[__counted_by(extra_size)];
} test_object_with_extra_t;

// MARK: Test
static void
UNIT_TEST_FUNC(alloc)(
		__unused const unit_t *un)
{
	__unused void *__single p_check = NULL;
	__unused size_t p_size = 0;

	sdrand();
	do {
		void *__free __sized_by_or_null(24u) p = NULL;

		p = walloc(24u);
		checknonnull("walloc [static size]", p);

		p_check = p;
	} while (0);

#if __APPLE__
	p_size = malloc_size(p_check);
	checkuint("allocation freed", p_size, 0);
#endif

	do {
		size_t size = 1024;
		void *__free p = NULL;

		p = walloc(size);
		checknonnull("walloc [literal size]", p);
	} while (0);

	do {
		size_t size = arc4random() % 1024;
		void *__free p = NULL;

		p = walloc(size);
		checknonnull("walloc [random size]", p);
	} while (0);

	do {
		test_object_with_extra_t *__single __free obj = NULL;
		uint32_t extra = 128;
		__unused size_t memsize = 0;
		__unused size_t memsize_expected = sizeof(*obj) + extra;

		obj = objallocx(test_object_with_extra_t, extra_size, extra);
		checknonnull("objallocx", obj);
		checkuint("object sized correctly", obj->extra_size, extra);

#if __has_ptrcheck
		memsize = __pointer_memsize(size_t, obj);
		checkuint("allocation sized correctly", memsize, memsize_expected);
#endif
	} while (0);

	do {
		test_object_t *__free p = NULL;

		p = objalloc(test_object_t);
		checkuint("objalloc [p->a]", p->a, 0);
		checkuint("objalloc [p->b]", p->b, 0);
		checkuint("objalloc [p->c]", p->c, 0);
	} while (0);
}

UNIT_TEST(alloc);
