/*!
 * @copyright
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT license.
 */
#include "0xc/sys/check.h"
#include "0xc/sys/unit.h"
#include "0xc/std/cursor.h"

#include "0xc/std/defs.h"
#include "0xc/std/pointer.h"
#include "0xc/std/int.h"
#include "0xc/sys/errno.h"

#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

// MARK: Test
static void
UNIT_TEST_FUNC(cursor_trunc)(
		__unused const unit_t *un)
{
	static const char *expected =
		"Hello, world! Here is a number: ffffffffffffffff "
		"Maybe this wil";
	char buff[64] = "";
	const char *__cstr p_chk = __unsafe_forge(char *, buff, zterm);
	CURSOR *__single cur = cursor_new();
	int eob = -1;
	errno_t error = -1;

	cur = cbuffopen(cur, buff, "w");
	error = cprintf(cur, "Hello, %s!", "world");
	checkint("printed string segment 1", error, 0);

	eob = ceob(cur);
	checkint("not at end of buffer after segment 1", eob, 0);

	error = cprintf(cur, " Here is a number: %llx", ULLONG_MAX);
	checkint("printed string segment 2", error, 0);

	eob = ceob(cur);
	checkint("not at end of buffer after segment 2", eob, 0);

	error = cprintf(cur, " Maybe this will overflow,");
	checkint("truncated string segment 3", error, EOVERFLOW);

	eob = ceob(cur);
	checkint("end of buffer reached", eob, EOF);

	error = cprintf(cur, " maybe it won't");
	checkint("truncated string segment 4", error, EOVERFLOW);

	eob = ceob(cur);
	checkint("end of buffer reached", eob, EOF);

	checkcstr("string safely truncated", p_chk, expected);
	cclose_check(&cur, EOF);

	checkptr("cursor zero'ed", cur, NULL);
}

static void
UNIT_TEST_FUNC(cursor_notrunc)(
		__unused const unit_t *un)
{
	static const char *expected =
		"Hello, world! Here is a number: ffffffffffffffff "
		"Maybe this will overflow, maybe it won't";
	char buff[128] = "";
	const char *__cstr p_chk = __unsafe_forge(char *, buff, zterm);
	CURSOR *__single cur = cursor_new();
	errno_t error = -1;

	cur = cbuffopen(cur, buff, "w");
	error = cprintf(cur, "Hello, %s!", "world");
	checkint("printed string segment 1", error, 0);

	error = cprintf(cur, " Here is a number: %llx", ULLONG_MAX);
	checkint("printed string segment 2", error, 0);

	error = cprintf(cur, " Maybe this will overflow,");
	checkint("printed string segment 3", error, 0);

	error = cprintf(cur, " maybe it won't");
	checkint("printed string segment 4", error, 0);

	checkcstr("full string printed", p_chk, expected);
	cclose_check(&cur, 0);

	checkptr("cursor zero'ed", cur, NULL);
}

static void
UNIT_TEST_FUNC(cursor_mode)(
		__unused const unit_t *un)
{
	char buff[64] = "";
	CURSOR cur_mem = CURSOR_INIT;
	CURSOR *__single cur = NULL;
	errno_t error = -1;

	cur = cbuffopen(&cur_mem, buff, "r");
	error = cprintf(cur, "This should fail");
	checkint("writing to read-only cursor fails", error, EACCES);

	cclose(&cur);

	cur = cbuffopen(&cur_mem, buff, "r+");
	error = cprintf(cur, "This should succeed");
	checkint("writing to read-write cursor succeeds", error, 0);

	cclose(&cur);

	cur = cbuffopen(&cur_mem, buff, "w");
	error = cprintf(cur, "This should succeed");
	checkint("writing to write-only cursor succeeds", error, 0);

	cclose(&cur);

	cur = cbuffopen(&cur_mem, buff, "w+");
	error = cprintf(cur, "This should succeed");
	checkint("writing to write-read cursor succeeds", error, 0);

	cclose(&cur);
}

UNIT_TEST(cursor_trunc);
UNIT_TEST(cursor_notrunc);
UNIT_TEST(cursor_mode);
