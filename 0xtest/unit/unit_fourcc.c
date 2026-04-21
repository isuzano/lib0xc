/*!
 * @copyright
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT license.
 */
#include "0xc/sys/check.h"
#include "0xc/sys/unit.h"
#include "0xc/sys/fourcc.h"

#include "0xc/std/defs.h"

// MARK: Module Globals
static const fourcc_t _fcc_static = FOURCC('BNCH');

// MARK: Test
static void
UNIT_TEST_FUNC(fourcc)(
		__unused const unit_t *un)
{
	fourcc_t fcc = FOURCC('BORD');
	const char *__cstr cstr = NULL;

	cstr = fourcc_get_string(&fcc);
	checkcstr("four character code", cstr, "BORD");

	fourcc_init(&fcc, 'CHIP');
	cstr = fourcc_get_string(&fcc);
	checkcstr("four character code", cstr, "CHIP");

	cstr = fourcc_get_string(&_fcc_static);
	checkcstr("four character code", cstr, "BNCH");
}

UNIT_TEST(fourcc);
