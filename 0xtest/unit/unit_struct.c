/*!
 * @copyright
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT license.
 */
#include "0xc/sys/check.h"
#include "0xc/sys/unit.h"
#include "0xc/std/struct.h"

#include "0xc/std/defs.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

// MARK: Types
typedef struct _test_struct {
	int a;
	int b;
	int c;
} test_struct_t;

typedef struct _flex_struct {
	int a;
	int b;
	size_t c;
	char d[__counted_by(c)];
} flex_struct_t;

// MARK: Test
static void
UNIT_TEST_FUNC(struct_new)(
		__unused const unit_t *un)
{
	test_struct_t *t = __new(test_struct_t,
		.a = 1,
		.b = 2,
		.c = 3
	);

	checkint("a", t->a, 1);
	checkint("b", t->b, 2);
	checkint("c", t->c, 3);
}

static void
UNIT_TEST_FUNC(struct_newx)(
		__unused const unit_t *un)
{
	uint8_t expected[] = "AAAAAAAAAAAAAAAAAAAA";
	flex_struct_t *__single t = NULL;

	t = __newx(flex_struct_t, d, c, sizeof(expected));
	t->a = 1;
	t->b = 2;
	memcpy(t->d, expected, sizeof(expected));

	checkint("a", t->a, 1);
	checkint("b", t->b, 2);
	checkuint("c", t->c, sizeof(expected));
	checkmem("d", t->d, t->c, expected, sizeof(expected));
}

UNIT_TEST(struct_new);
UNIT_TEST(struct_newx);
