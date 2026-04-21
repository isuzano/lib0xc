/*!
 * @copyright
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT license.
 */
#include "0xc/std/pointer.h"
#include "0xc/sys/check.h"
#include "0xc/sys/unit.h"
#include "0xc/sys/buff.h"

#include "0xc/std/defs.h"

#ifndef __OPEN_SOURCE__
#if __has_include("0xc/funos/sys/unit.h")
#include "0xc/funos/sys/unit.h"
#endif
#endif

#include <stdlib.h>
#include <stdint.h>

// MARK: Module Globals
static const uint8_t __data[] = {
	0xde, 0xad, 0xbe, 0xef, 0xfe, 0xed, 0xfa, 0xce,
	0x12, 0x34, 0x56, 0x78, 0x9a, 0xbc, 0xde, 0xf0,
};

// MARK: Test
static void
UNIT_TEST_FUNC(buff_static_init)(
		__unused const unit_t *un)
{
	static const buff_t bf = buff_static("test", __data);
	buff_t *__buff_destroy bf_cursor = buff_new();
	const uint8_t *__single p = NULL;

	checkuint("buffer length", bf.bf_len, sizeof(__data));

	// Verify that reading from the static buffer with a runtime cursor produces
	// a valid pointer.
	bf_cursor = buff_cursor(bf_cursor, &bf, 0);
	checknonnull("cursor", bf_cursor);

	p = buff_read(&bf, bf_cursor, sizeof(__data));
	checknonnull("read full buffer", p);

	// Verify that a subsequent read past the end returns NULL.
	p = buff_read(&bf, bf_cursor, 1);
	checkptr("read past end", p, NULL);
}

static void
UNIT_TEST_FUNC(buff_cursor_static_init)(
		__unused const unit_t *un)
{
	static const buff_t bf = buff_static("test", __data);
	static buff_t bf_cursor = buff_cursor_static(&bf, __data);
	size_t p_len = sizeof(__data) / 2;
	const uint8_t *__sized_by_or_null(p_len) p = NULL;

	checkuint("cursor length", bf_cursor.bf_len, sizeof(__data));

	// Read the first half through the static cursor.
	p = buff_read(&bf, &bf_cursor, p_len);
	__bounds_affirm(p_len);
	checknonnull("read first half", p);

	// Read the second half through the same static cursor.
	p = buff_read(&bf, &bf_cursor, p_len);
	__bounds_affirm(p_len);
	checknonnull("read second half", p);

	// Cursor should now be exhausted.
	p = buff_read(&bf, &bf_cursor, 1);
	__bounds_affirm(p_len);
	checkptr("read past end", p, NULL);
}

#ifndef __OPEN_SOURCE__
#if __has_include("0xc/funos/sys/unit.h")
UNIT_TEST_THREAD(buff_static_init);
UNIT_TEST_THREAD(buff_cursor_static_init);
#else
UNIT_TEST(buff_static_init);
UNIT_TEST(buff_cursor_static_init);
#endif
#else
UNIT_TEST(buff_static_init);
UNIT_TEST(buff_cursor_static_init);
#endif
