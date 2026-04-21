/*!
 * @copyright
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT license.
 */
#include "0xc/std/limits.h"
#include "0xc/sys/check.h"
#include "0xc/sys/unit.h"
#include "0xc/std/defs.h"
#include "0xc/sys/linker_set.h"

#include <stdint.h>
#include <stdlib.h>

// MARK: Types
typedef struct _linker_set_entry {
	uint16_t lse_value;
} linker_set_entry_t;

// MARK: Module Globals
LINKER_SET_DECL(linker_set_entry_t, test);

static const linker_set_entry_t _l0 = {
	.lse_value = 0x000f,
};
LINKER_SET_ENTRY(test, _l0);

static const linker_set_entry_t _l1 = {
	.lse_value = 0x00f0,
};
LINKER_SET_ENTRY(test, _l1);

static const linker_set_entry_t _l2 = {
	.lse_value = 0x0f00,
};
LINKER_SET_ENTRY(test, _l2);

static const linker_set_entry_t _l3 = {
	.lse_value = 0xf000,
};
LINKER_SET_ENTRY(test, _l3);

// MARK: Test
static void
UNIT_TEST_FUNC(linker_set)(
		__unused const unit_t *un)
{
	const linker_set_entry_t *__single ei = NULL;
	const linker_set_entry_t *__single *__unsafe_indexable eii = NULL;
	uint16_t actual = 0;
	uint16_t expected = INT_MAXOF(uint16_t);
	size_t cnt = LINKER_SET_COUNT(test);
	size_t cnt_expected = sizeof(uint16_t) * 2;

	LINKER_SET_FOREACH(ei, test, eii) {
		actual |= ei->lse_value;
	}

	checkuint("all set entries iterated", actual, expected);
	checkuint("linker set count correct", cnt, cnt_expected);
}
UNIT_TEST(linker_set);
