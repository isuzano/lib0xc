/*!
 * @copyright
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT license.
 */
#include "0xc/sys/check.h"
#include "0xc/sys/unit.h"
#include "0xc/std/limits.h"

#include "0xc/std/defs.h"
#include "0xc/std/pointer.h"

#include <stdint.h>
#include <stdlib.h>

// MARK: Test
static void
UNIT_TEST_FUNC(alignto_uint8)(
		__unused const unit_t *un)
{
	checkuint("alignto uint8_t(0, 4)", alignto((uint8_t)0, 4), 0);
	checkuint("alignto uint8_t(1, 4)", alignto((uint8_t)1, 4), 4);
	checkuint("alignto uint8_t(4, 4)", alignto((uint8_t)4, 4), 4);
	checkuint("alignto uint8_t(5, 8)", alignto((uint8_t)5, 8), 8);
}

static void
UNIT_TEST_FUNC(alignto_uint16)(
		__unused const unit_t *un)
{
	checkuint("alignto uint16_t(0, 16)", alignto((uint16_t)0, 16), 0);
	checkuint("alignto uint16_t(13, 16)", alignto((uint16_t)13, 16), 16);
	checkuint("alignto uint16_t(256, 256)", alignto((uint16_t)256, 256), 256);
	checkuint("alignto uint16_t(257, 256)", alignto((uint16_t)257, 256), 512);
}

static void
UNIT_TEST_FUNC(alignto_uint32)(
		__unused const unit_t *un)
{
	checkuint("alignto uint32_t(4095, 4096)",
			alignto((uint32_t)4095, 4096), 4096);
	checkuint("alignto uint32_t(4096, 4096)",
			alignto((uint32_t)4096, 4096), 4096);
	checkuint("alignto uint32_t(4097, 4096)",
			alignto((uint32_t)4097, 4096), 8192);
}

static void
UNIT_TEST_FUNC(alignto_uint64)(
		__unused const unit_t *un)
{
	checkuint("alignto uint64_t(0, 1)", alignto((uint64_t)0, 1), 0);
	checkuint("alignto uint64_t(7, 1)", alignto((uint64_t)7, 1), 7);
	checkuint("alignto uint64_t(1, 4096)", alignto((uint64_t)1, 4096), 4096);
}

static void
UNIT_TEST_FUNC(alignto_int16)(
		__unused const unit_t *un)
{
	checkint("alignto int16_t(0, 4)", alignto((int16_t)0, 4), 0);
	checkint("alignto int16_t(1, 4)", alignto((int16_t)1, 4), 4);
	checkint("alignto int16_t(4, 4)", alignto((int16_t)4, 4), 4);
	checkint("alignto int16_t(3, 8)", alignto((int16_t)3, 8), 8);
}

static void
UNIT_TEST_FUNC(alignto_int32)(
		__unused const unit_t *un)
{
	checkint("alignto int32_t(0, 16)", alignto((int32_t)0, 16), 0);
	checkint("alignto int32_t(1, 16)", alignto((int32_t)1, 16), 16);
	checkint("alignto int32_t(16, 16)", alignto((int32_t)16, 16), 16);
	checkint("alignto int32_t(17, 16)", alignto((int32_t)17, 16), 32);
}

static void
UNIT_TEST_FUNC(alignto_int64)(
		__unused const unit_t *un)
{
	checkint("alignto int64_t(0, 4096)", alignto((int64_t)0, 4096), 0);
	checkint("alignto int64_t(1, 4096)", alignto((int64_t)1, 4096), 4096);
	checkint("alignto int64_t(4096, 4096)",
			alignto((int64_t)4096, 4096), 4096);
}

static void
UNIT_TEST_FUNC(alignto_size)(
		__unused const unit_t *un)
{
	checkuint("alignto size_t(0, 8)", alignto((size_t)0, 8), 0);
	checkuint("alignto size_t(5, 8)", alignto((size_t)5, 8), 8);
	checkuint("alignto size_t(8, 8)", alignto((size_t)8, 8), 8);
}

static void
UNIT_TEST_FUNC(alignto_runtime)(
		__unused const unit_t *un)
{
	volatile uint32_t rt_align = 16;
	volatile size_t rt_page = 4096;

	checkint("alignto runtime(0, 16)", alignto(0, rt_align), 0);
	checkint("alignto runtime(1, 16)", alignto(1, rt_align), 16);
	checkint("alignto runtime(16, 16)", alignto(16, rt_align), 16);
	checkint("alignto runtime(17, 16)", alignto(17, rt_align), 32);

	checkint("alignto runtime(0, 4096)", alignto(0, rt_page), 0);
	checkint("alignto runtime(1, 4096)", alignto(1, rt_page), 4096);
	checkint("alignto runtime(4097, 4096)", alignto(4097, rt_page), 8192);
}

static void
UNIT_TEST_FUNC(chkalign_runtime)(
		__unused const unit_t *un)
{
	volatile uint32_t rt_align = 16;

	checkint("chkalign runtime(0, 16)", chkalign(0, rt_align), 1);
	checkint("chkalign runtime(16, 16)", chkalign(16, rt_align), 1);
	checkint("chkalign runtime(1, 16)", chkalign(1, rt_align), 0);
	checkint("chkalign runtime(15, 16)", chkalign(15, rt_align), 0);
}

static void
UNIT_TEST_FUNC(alignto_pointer)(
		__unused const unit_t *un)
{
	const uint8_t *__single p = NULL;
	const uint8_t *__single expected = NULL;
	const uint8_t *__single result = NULL;

	p = __unsafe_forge(const uint8_t *, 0x1000, single);
	expected = __unsafe_forge(const uint8_t *, 0x1000, single);
	result = __unsafe_forge(const uint8_t *, alignto(p, 4096), single);
	checkptr("alignto(0x1000, 4096)", result, expected);

	p = __unsafe_forge(const uint8_t *, 0x1001, single);
	expected = __unsafe_forge(const uint8_t *, 0x2000, single);
	result = __unsafe_forge(const uint8_t *, alignto(p, 4096), single);
	checkptr("alignto(0x1001, 4096)", result, expected);

	p = __unsafe_forge(const uint8_t *, 0x1fff, single);
	expected = __unsafe_forge(const uint8_t *, 0x2000, single);
	result = __unsafe_forge(const uint8_t *, alignto(p, 4096), single);
	checkptr("alignto(0x1fff, 4096)", result, expected);

	p = __unsafe_forge(const uint8_t *, 0x0, single);
	result = __unsafe_forge(const uint8_t *, alignto(p, 16), single);
	checkptr("alignto(0x0, 16)", result, NULL);

	p = __unsafe_forge(const uint8_t *, 0x7, single);
	expected = __unsafe_forge(const uint8_t *, 0x8, single);
	result = __unsafe_forge(const uint8_t *, alignto(p, 8), single);
	checkptr("alignto(0x7, 8)", result, expected);

	p = __unsafe_forge(const uint8_t *, 0x10, single);
	expected = __unsafe_forge(const uint8_t *, 0x10, single);
	result = __unsafe_forge(const uint8_t *, alignto(p, 16), single);
	checkptr("alignto(0x10, 16)", result, expected);
}

static void
UNIT_TEST_FUNC(alignto_pointer_runtime)(
		__unused const unit_t *un)
{
	volatile uint32_t rt_align = 16;
	volatile size_t rt_page = 4096;
	const uint8_t *__single p = NULL;
	const uint8_t *__single expected = NULL;
	const uint8_t *__single result = NULL;

	p = __unsafe_forge(const uint8_t *, 0x1001, single);
	expected = __unsafe_forge(const uint8_t *, 0x2000, single);
	result = __unsafe_forge(const uint8_t *, alignto(p, rt_page), single);
	checkptr("alignto runtime(0x1001, 4096)", result, expected);

	p = __unsafe_forge(const uint8_t *, 0x3, single);
	expected = __unsafe_forge(const uint8_t *, 0x10, single);
	result = __unsafe_forge(const uint8_t *, alignto(p, rt_align), single);
	checkptr("alignto runtime(0x3, 16)", result, expected);
}

static void
UNIT_TEST_FUNC(chkalign_pointer)(
		__unused const unit_t *un)
{
	const void *__single p = NULL;

	p = __unsafe_forge(const void *, 0x1000, single);
	checkuint("chkalign(0x1000, 4096)", chkalign(p, 4096), 1);
	checkuint("chkalign(0x1000, 16)", chkalign(p, 16), 1);

	p = __unsafe_forge(const void *, 0x1001, single);
	checkuint("chkalign(0x1001, 4096)", chkalign(p, 4096), 0);
	checkuint("chkalign(0x1001, 16)", chkalign(p, 16), 0);

	p = __unsafe_forge(const void *, 0x0, single);
	checkuint("chkalign(0x0, 16)", chkalign(p, 16), 1);

	p = __unsafe_forge(const void *, 0x10, single);
	checkuint("chkalign(0x10, 16)", chkalign(p, 16), 1);
	checkuint("chkalign(0x10, 32)", chkalign(p, 32), 0);

	p = __unsafe_forge(const void *, 0x7, single);
	checkuint("chkalign(0x7, 8)", chkalign(p, 8), 0);
	checkuint("chkalign(0x7, 4)", chkalign(p, 4), 0);
	checkuint("chkalign(0x7, 1)", chkalign(p, 1), 1);
}

static void
UNIT_TEST_FUNC(chkalign_pointer_runtime)(
		__unused const unit_t *un)
{
	volatile uint32_t rt_align = 16;
	volatile size_t rt_page = 4096;
	const void *__single p = NULL;

	p = __unsafe_forge(const void *, 0x1000, single);
	checkuint("chkalign runtime(0x1000, 4096)", chkalign(p, rt_page), 1);
	checkuint("chkalign runtime(0x1000, 16)", chkalign(p, rt_align), 1);

	p = __unsafe_forge(const void *, 0x1001, single);
	checkuint("chkalign runtime(0x1001, 4096)", chkalign(p, rt_page), 0);
	checkuint("chkalign runtime(0x1001, 16)", chkalign(p, rt_align), 0);
}

UNIT_TEST(alignto_uint8);
UNIT_TEST(alignto_uint16);
UNIT_TEST(alignto_uint32);
UNIT_TEST(alignto_uint64);
UNIT_TEST(alignto_int16);
UNIT_TEST(alignto_int32);
UNIT_TEST(alignto_int64);
UNIT_TEST(alignto_size);
UNIT_TEST(alignto_runtime);
UNIT_TEST(chkalign_runtime);
UNIT_TEST(chkalign_pointer);
UNIT_TEST(chkalign_pointer_runtime);
UNIT_TEST(alignto_pointer);
UNIT_TEST(alignto_pointer_runtime);
