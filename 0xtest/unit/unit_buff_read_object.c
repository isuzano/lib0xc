/*!
 * @copyright
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT license.
 */
#include "0xc/sys/check.h"
#include "0xc/sys/unit.h"
#include "0xc/sys/buff.h"

#include "0xc/std/defs.h"
#include "0xc/std/array.h"

#include <stdint.h>
#include <stdlib.h>

// MARK: Types
typedef struct _bag_of_bytes {
	uint32_t bb_f1;
	uint32_t bb_f2;
	uint64_t bb_f3;
} bag_of_bytes_t;

// MARK: Module Globals
static const bag_of_bytes_t __buff[] = {
	[0] = {
		.bb_f1 = 0xdeadbeef,
		.bb_f2 = 0xfeedface,
		.bb_f3 = 0xd00df00d900,
	}, {
		.bb_f1 = 0xf1,
		.bb_f2 = 0xf2,
		.bb_f3 = 0xf3,
	}, {
		.bb_f1 = 0x12345678,
		.bb_f2 = 0x87654321,
		.bb_f3 = 0xfedcba987654321,
	}, {
		.bb_f1 = 12,
		.bb_f2 = 24,
		.bb_f3 = 48,
	},
};

// MARK: Test
static void
UNIT_TEST_FUNC(buff_read_object)(
		__unused const unit_t *un)
{
	buff_t *__buff_destroy bf = buff_new();
	buff_t *__buff_destroy bf_cursor = buff_new();
	const bag_of_bytes_t *__single bi = NULL;
	size_t i = 0;

	bf = buff_init_wrap(bf, "object", __buff, sizeof(__buff));
	bf_cursor = buff_cursor(bf_cursor, bf, 0);
	while ((bi = buff_read_object(bf, bf_cursor, bag_of_bytes_t))) {
		const bag_of_bytes_t *bi_expect = &__buff[i];

		checkuint("object field 1[%zu]", bi->bb_f1, bi_expect->bb_f1, i);
		checkuint("object field 2[%zu]", bi->bb_f2, bi_expect->bb_f2, i);
		checkuint("object field 3[%zu]", bi->bb_f3, bi_expect->bb_f3, i);
		i += 1;
	}

	checkuint("iterated all objects", i, countof(__buff));
}

UNIT_TEST(buff_read_object);
