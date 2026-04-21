/*!
 * @copyright
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT license.
 */
#include "0xc/sys/check.h"
#include "0xc/sys/unit.h"
#include "0xc/std/io.h"

#include "0xc/std/defs.h"
#include "0xc/std/int.h"
#include "0xc/std/pointer.h"

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

// MARK: Test
static void
UNIT_TEST_FUNC(sprintf_check_and_consume_notrunc)(
		__unused const unit_t *un)
{
	static const char *expected =
		"Hello, world! Here is a number: ffffffffffffffff "
		"Maybe this will overflow, maybe it won't";
	char buff[128] = "";
	size_t len = sizeof(buff);
	char *p = buff;
	const char *__cstr p_chk = __unsafe_forge(char *, buff, zterm);
	int n = 0;

	n = snprintf(p, len, "Hello, %s!", "world");
	p = sprintf_check_and_consume(p, &len, n, NULL);

	n = snprintf(p, len, " Here is a number: %llx", ULLONG_MAX);
	p = sprintf_check_and_consume(p, &len, n, NULL);

	n = snprintf(p, len, " Maybe this will overflow,");
	p = sprintf_check_and_consume(p, &len, n, NULL);

	n = snprintf(p, len, " maybe it won't");
	p = sprintf_check_and_consume(p, &len, n, NULL);

	checkcstr("string safely truncated", p_chk, expected);
}

static void
UNIT_TEST_FUNC(sprintf_check_and_consume_trunc)(
		__unused const unit_t *un)
{
	static const char *expected =
		"Hello, world! Here is a number: ffffffffffffffff "
		"Maybe this wil";
	char buff[64] = "";
	size_t len = sizeof(buff);
	char *p = buff;
	const char *__cstr p_chk = NULL;
	int n = 0;

	n = snprintf(p, len, "Hello, %s!", "world");
	p = sprintf_check_and_consume(p, &len, n, NULL);

	n = snprintf(p, len, " Here is a number: %llx", ULLONG_MAX);
	p = sprintf_check_and_consume(p, &len, n, NULL);

	n = snprintf(p, len, " Maybe this will overflow,");
	p = sprintf_check_and_consume(p, &len, n, NULL);

	n = snprintf(p, len, " maybe it won't");
	p = sprintf_check_and_consume(p, &len, n, NULL);

	p_chk = __unsafe_null_terminated_from_indexable(buff);
	checkcstr("string safely truncated", p_chk, expected);
}

UNIT_TEST(sprintf_check_and_consume_trunc);
UNIT_TEST(sprintf_check_and_consume_notrunc);
