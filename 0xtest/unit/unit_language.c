/*!
 * @copyright
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT license.
 */
#include "0xc/sys/check.h"
#include "0xc/sys/unit.h"
#include "0xc/std/defs.h"

#include <stdint.h>
#include <stdlib.h>

// MARK: Types
__bitfld(test_bitfield, uint32_t,
	TEST_BITFIELD_FLAG_A = 1 << 0,
	TEST_BITFIELD_FLAG_B = 1 << 1,
	TEST_BITFIELD_FLAG_C = 1 << 2,
);

// MARK: Map Tests
#define _MAP_COUNTER_CALL(V) _map_counter(&V);
#define _DECL_INT(V) int V = 42;
#define _BOGUS_DECL(V) unsigned int V = -27;

static void
_map_counter(
		int *v)
{
	(v[0])++;
}

// MARK: DELIMIT Tests
struct foo {
	DELIMIT_SEMICOLON(
		int a,
		int b,
		int c
	);
};

struct foo_empty {
	DELIMIT_SEMICOLON();
};

__unused static void
foofunc(DELIMIT_COMMA(
		__unused int a,
		__unused float b,
		__unused char c),
		__unused struct foo f)
{

}

// MARK: Test
static void
UNIT_TEST_FUNC(language)(
		__unused const unit_t *un)
{
	size_t cnt = VA_COUNT(1, 2, 3, 4, 5, 6, 7, 8);
	size_t zero = VA_COUNT();

	checkuint("VA_COUNT correct", cnt, 8);
	checkuint("VA_COUNT zero correct", zero, 0);
}

static void
UNIT_TEST_FUNC(map)(
		__unused const unit_t *un)
{
	// Verify that
	//
	//     MAP(int , = 0;, x, y, z)
	//
	// expands to
	//
	//     int x = 0; int y = 0; int z = 0;
	MAP(int , = 27;, x, y, z)
	checkint("MAP_APPLY _DECL_INT p", x, 27);
	checkint("MAP_APPLY _DECL_INT q", y, 27);
	checkint("MAP_APPLY _DECL_INT r", z, 27);
}

static void
UNIT_TEST_FUNC(map_apply)(
		__unused const unit_t *un)
{
	int cnt = 0;

	MAP_APPLY(_BOGUS_DECL);
	MAP_APPLY(_MAP_COUNTER_CALL, cnt, cnt, cnt, cnt, cnt)
	checkint("MAP_APPLY call count", cnt, 5);

	MAP_APPLY(_DECL_INT, p, q, r);
	checkint("MAP_APPLY _DECL_INT p", p, 42);
	checkint("MAP_APPLY _DECL_INT q", q, 42);
	checkint("MAP_APPLY _DECL_INT r", r, 42);
}

UNIT_TEST(language);
UNIT_TEST(map);
UNIT_TEST(map_apply);
