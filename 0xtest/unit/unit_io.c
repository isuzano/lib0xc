/*!
 * @copyright
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT license.
 */
#include "0xc/sys/check.h"
#include "0xc/sys/unit.h"
#include "0xc/std/io.h"

#include "0xc/std/cursor.h"
#include "0xc/std/defs.h"
#include "0xc/std/pointer.h"
#include "0xc/std/string.h"
#include "0xc/sys/log.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

// MARK: Definitions
#if __has_ptrcheck
#define __SINGLE "__single"
#define __TERMINATED_BY_ZERO " __terminated_by(0)"
#else
#define __SINGLE ""
#define __TERMINATED_BY_ZERO ""
#endif

#define type_clang_cstr(T) _Generic((T)0, \
	const char * : "const char *" __SINGLE __TERMINATED_BY_ZERO, \
	const unsigned char * : "const unsigned char *" __SINGLE, \
	unsigned int : "unsigned int", \
	uint64_t : "uint64_t", \
	int32_t : "int32_t", \
	bool : "bool" \
)

// uint8_t is unsigned char -- _Generic cannot distinguish them.
#define type_cstr_cbytes "const uint8_t *" __SINGLE
#define type_cstr_mbytes "uint8_t *" __SINGLE

// MARK: Test
static void
UNIT_TEST_FUNC(snprintmem)(
		__unused const unit_t *un)
{
	static const uint8_t bytes[] = {
		0x82, 0xa7, 0x99, 0xf4, 0xdd,
		0x9a, 0x16, 0x71, 0x18, 0x36,
		0x66, 0x79, 0x15, 0xe9, 0x5b,
		0x3c, 0x65, 0xab, 0xdf, 0xb0,
	};
	digest_string_t ds = "";
	digest_string_t ds_expected = "82a799f4dd9a16711836667915e95b3c65abdfb0";

	ssprintmem(ds, bytes, sizeof(bytes));
	checkcstr("byte string",
			__forge(const char *, ds, zterm),
			__forge(const char *, ds_expected, zterm));
}

// MARK: dump_field Test
typedef struct _dump_field_test {
	const char *str;
	const unsigned char *bytes;
	const uint8_t *cbytes;
	uint8_t *mbytes;
	unsigned int uint;
	uint64_t u64;
	int32_t i32;
	bool flag;
} dump_field_test_t;

static void
UNIT_TEST_FUNC(dump_field)(
		__unused const unit_t *un)
{
	static const char hello[] = "hello";
	static const unsigned char data[] = {
		0xde, 0xad,
	};
	static const uint8_t cdata[] = {
		0xca, 0xfe,
	};
	static uint8_t mdata[] = {
		0xba, 0xbe,
	};
	__unused dump_field_test_t obj = {
		.str = hello,
		.bytes = data,
		.cbytes = cdata,
		.mbytes = mdata,
		.uint = 42,
		.u64 = 0x123456789abcdef0ULL,
		.i32 = -7,
		.flag = true,
	};
	static char actual[1024] = "";
	static char expected[1024] = "";
	const char *__cstr actual_cstr = __forge(const char *, actual, zterm);
	CURSOR *__single cur = cursor_new();
	__unused dump_field_context_t ctxt = {
		.dmp_cursor = NULL,
	};

	ctxt.dmp_cursor = cbuffopen(cur, actual, "w");
	__dump_struct(&obj, __dump_field, &ctxt);

	// Build the expected output independently. When __builtin_dump_struct is
	// available, it produces a full struct dump with per-field formatting. The
	// original format for string fields is "%.32s"; the munge replaces it with
	// "%p", stripping the precision and length modifiers. When the builtin is
	// not available, the fallback just prints the stringified variable name.
#if __has_builtin(__builtin_dump_struct)
	ssprintf(expected,
			"struct _dump_field_test {\n"
			"  %s str = \"hello\"\n"
			"  %s bytes = \"%p\"\n"
			"  %s cbytes = \"%p\"\n"
			"  %s mbytes = \"%p\"\n"
			"  unsigned int uint = 42\n"
			"  uint64_t u64 = 1311768467463790320\n"
			"  int32_t i32 = -7\n"
			"  bool flag = 1\n"
			"}\n",
			type_clang_cstr(const char *),
			type_clang_cstr(const unsigned char *), (const void *)data,
			type_cstr_cbytes, (const void *)cdata,
			type_cstr_mbytes, (const void *)mdata);
#else
	ssprintf(expected, "&obj");
#endif

	checkcstr("dump_field output", actual_cstr, cstr(expected));
	cclose(&cur);
}

UNIT_TEST(snprintmem);
UNIT_TEST(dump_field);
