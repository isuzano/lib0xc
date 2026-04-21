/*!
 * @copyright
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT license.
 */
#include "0xc/sys/check.h"
#include "0xc/sys/unit.h"
#include "0xc/sys/digest.h"

#include "0xc/std/defs.h"
#include "0xc/std/struct.h"
#include "0xc/std/pointer.h"
#include "0xc/std/io.h"

#include <stdint.h>

// MARK: Test
static void
UNIT_TEST_FUNC(digest)(
		__unused const unit_t *un)
{
	digest_t d = zstruct(digest_t);
	static const uint8_t bytes[] = {
		0x82, 0xa7, 0x99, 0xf4, 0xdd,
		0x9a, 0x16, 0x71, 0x18, 0x36,
		0x66, 0x79, 0x15, 0xe9, 0x5b,
		0x3c, 0x65, 0xab, 0xdf, 0xb0,
	};
	digest_string_t ds = "";
	digest_string_t ds_expected = "82a799f4dd9a16711836667915e95b3c65abdfb0";
	const char *__cstr ds_zterm = NULL;
	const char *__cstr ds_expected_zterm = NULL;

	digest_init(&d, bytes, sizeof(bytes));
	digest_print(&d, ds);

	ds_zterm = __forge(const char *, ds, zterm);
	ds_expected_zterm = __forge(const char *, ds_expected, zterm);
	checkcstr("digest string", ds_zterm, ds_expected_zterm);
}

UNIT_TEST(digest);
