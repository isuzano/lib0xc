/*!
 * @copyright
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT license.
 */
#include "0xc/sys/check.h"
#include "0xc/sys/unit.h"
#include "0xc/std/io.h"

#include "0xc/std/defs.h"
#include "0xc/sys/log.h"
#include "0xc/std/limits.h"

#include <limits.h>
#include <stdint.h>
#include <stdlib.h>

// MARK: Test
static void
UNIT_TEST_FUNC(hlzux)(
		__unused const unit_t *un)
{
	char s[64] = "";
	uint16_t u16 = 12;
	uint32_t u32 = 42;
	int64_t i64 = -35;
	size_t sz = 90;
	uint64_t chk = 0;
	int64_t chk_signed = 0;

	ssprintf(s, "hu = %hu, u = %u, lld = %lld, zu = %zu",
			oxohu(u16), oxou(u32), oxolld(i64), oxozu(sz));
	zxinfox(un, "%s", cstr(s));

	chk = INT_MAXOF(long);
	checkuint("long max", chk, LONG_MAX);

	chk_signed = INT_MINOF(long);
	checkint("long min", chk_signed, LONG_MIN);
}

UNIT_TEST(hlzux);
