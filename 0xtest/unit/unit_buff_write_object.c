/*!
 * @copyright
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT license.
 */
#include "0xc/sys/check.h"
#include "0xc/sys/unit.h"
#include "0xc/sys/buff.h"

#include "0xc/std/defs.h"
#include "0xc/std/alloc.h"
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
UNIT_TEST_FUNC(buff_write_object)(
		__unused const unit_t *un)
{
	size_t p_len = 0;
	void *__sized_by(p_len) p = NULL;
	buff_t *__buff_destroy bf = buff_new();
	buff_t *__buff_destroy bf_cursor = buff_new();
	const bag_of_bytes_t *__single bi = NULL;
	size_t bi_expect_cnt = 0;
	const bag_of_bytes_t *__counted_by(bi_expect_cnt) bi_expect = NULL;
	size_t i = 0;

	p = walloc(sizeof(__buff));
	p_len = sizeof(__buff);
	bi_expect = p;
	bi_expect_cnt = countof(__buff);

	bf = buff_init(bf, BUFF_TYPE_MALLOC, "writable", &p, &p_len);
	checknonnull("buffer", bf);

	bf_cursor = buff_cursor(bf_cursor, bf, 0);
	for (i = 0; i < bi_expect_cnt; i++) {
		const bag_of_bytes_t *bi = &__buff[i];
		const bag_of_bytes_t *bip = NULL;

		bip = buff_write_object(bf, bf_cursor, bag_of_bytes_t, bi);
		checkptr("object written to expected location", bip, &bi_expect[i]);
	}

	i = 0;
	bf_cursor = buff_cursor(bf_cursor, bf, 0);
	while ((bi = buff_read_object(bf, bf_cursor, bag_of_bytes_t))) {
		checkuint("object field 1", bi->bb_f1, bi_expect[i].bb_f1);
		checkuint("object field 2", bi->bb_f2, bi_expect[i].bb_f2);
		checkuint("object field 3", bi->bb_f3, bi_expect[i].bb_f3);
		i += 1;
	}

	checkuint("iterated all objects", i, bi_expect_cnt);
}

UNIT_TEST(buff_write_object);
