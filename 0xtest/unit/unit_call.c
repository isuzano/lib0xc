/*!
 * @copyright
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT license.
 */
#include "0xc/std/string.h"
#include "0xc/sys/check.h"
#include "0xc/sys/log.h"
#include "0xc/sys/unit.h"
#include "0xc/std/call.h"
#include "0xc/std/call_ext.h"

#include "0xc/std/api.h"
#include "0xc/std/int.h"
#include "0xc/std/struct.h"

#include <limits.h>
#include <stdint.h>
#include <stdlib.h>

// MARK: IWYU
DECL_TYPE_4IWYU(ssize_t);
DECL_FUNC_4IWYU(strlcpy);

// A call with zero arguments.
static int _test_call_void_invoked;

CALL_DECL(test_call_void);

CALL_DECL_RETURN(test_call_void);
CALL_DECL_FIELDS(test_call_void);

// A call with two scalar inline arguments.
CALL_DECL(test_call_inline,
	int arg1,
	int arg2
);
CALL_DECL_RETURN(test_call_inline,
	int arg2,
	int arg1
);
CALL_DECL_FIELDS(test_call_inline);

// A call with an array argument, which should be passed inline.
CALL_DECL(test_call_array,
	int arg1,
	int arg2,
	char arg3[64]
);
CALL_DECL_RETURN(test_call_array,
	char arg3[64],
	int arg2,
	int arg1
);
CALL_DECL_FIELDS(test_call_array);

// A call with a pointer argument, which should be censored. Note we lay out the
// pointers on either side of the inline integer arguments to increase the
// chance of catching bugs where censorship spills over.
CALL_DECL(test_call_clear,
	size_t arg0_cnt,
	uint8_t *arg0,
	int arg1,
	int arg2,
	uint8_t *arg4,
	size_t arg4_cnt
);
CALL_DECL_RETURN(test_call_clear,
	char arg3[64],
	int arg2,
	int arg1
);
CALL_DECL_FIELDS(test_call_clear,
	arg0, arg0_cnt, private,
	arg4, arg4_cnt, private
);

// A call with an extended parameter list.
CALL_DECL_EXT(test_call_ext,
	CALL_EXT_PARAMS(const unit_t *un, int argx),
	int arg1,
	int arg2
);
CALL_DECL_FIELDS(test_call_ext);
CALL_DECL_RETURN(test_call_ext,
	const unit_t *unx,
	int argx,
	int arg2,
	int arg1
);

// MARK: Test Calls
void
test_call_void(
		const void *__sized_by(argv_size) __argv,
		size_t argv_size,
		__unused void *_Nullable __sized_by(retv_size) __retv,
		__unused size_t retv_size)
{
	const call_argv_t(test_call_void) *argv = NULL;

	argv = call_argv_import(test_call_void, __argv, argv_size);
	checkuint("empty argv size correct", sizeof(*argv), 0);

	_test_call_void_invoked = 1;
}

void
test_call_inline(
		const void *__sized_by(argv_size) __argv,
		size_t argv_size,
		void *_Nullable __sized_by(retv_size) __retv,
		size_t retv_size)
{
	const call_argv_t(test_call_inline) *argv = NULL;
	call_retv_t(test_call_inline) *retv = NULL;

	argv = call_argv_import(test_call_inline, __argv, argv_size);
	retv = call_retv_import(test_call_inline, __retv, retv_size);

	retv->arg1 = argv->arg1;
	retv->arg2 = argv->arg2;
}

void
test_call_array(
		const void *__sized_by(argv_size) __argv,
		size_t argv_size,
		void *_Nullable __sized_by(retv_size) __retv,
		size_t retv_size)
{
	const call_argv_t(test_call_array) *argv = NULL;
	call_retv_t(test_call_array) *retv = NULL;

	argv = call_argv_import(test_call_array, __argv, argv_size);
	retv = call_retv_import(test_call_array, __retv, retv_size);

	retv->arg1 = argv->arg1;
	retv->arg2 = argv->arg2;
	strscpy(retv->arg3, argv->arg3);
}

void
test_call_clear(
		const void *__sized_by(argv_size) __argv,
		size_t argv_size,
		void *_Nullable __sized_by(retv_size) __retv,
		size_t retv_size)
{
	const call_argv_t(test_call_clear) *argv = NULL;
	call_retv_t(test_call_clear) *retv = NULL;

	argv = call_argv_import(test_call_clear, __argv, argv_size);
	retv = call_retv_import(test_call_clear, __retv, retv_size);

	retv->arg1 = argv->arg1;
	retv->arg2 = argv->arg2;
	strscpy(retv->arg3, "done!");

	checknull("arg0 censored", argv->arg0);
	checknull("arg4 censored", argv->arg4);
}

void
test_call_ext(
		const unit_t *un,
		int argx,
		const void *__sized_by(argv_size) __argv,
		size_t argv_size,
		void *_Nullable __sized_by(retv_size) __retv,
		size_t retv_size)
{
	const call_argv_t(test_call_ext) *argv = NULL;
	call_retv_t(test_call_ext) *retv = NULL;

	zxdebugx(un, "here");

	argv = call_argv_import(test_call_ext, __argv, argv_size);
	retv = call_retv_import(test_call_ext, __retv, retv_size);

	retv->unx = un;
	retv->argx = argx;
	retv->arg1 = argv->arg1;
	retv->arg2 = argv->arg2;
}

// MARK: Tests
static void
UNIT_TEST_FUNC(call_empty)(
		__unused const unit_t *un)
{
	// Set to false in case the test is run multiple times.
	_test_call_void_invoked = 0;

	zxcall(test_call_void, NULL);
	checkint("void call function invoked", _test_call_void_invoked, 1);
}

static void
UNIT_TEST_FUNC(call_inline)(
		__unused const unit_t *un)
{
	call_retv_t(test_call_inline) *retv = call_retv_new(test_call_inline,
		.arg1 = INT_MAX,
		.arg2 = INT_MIN,
	);

	zxcall(test_call_inline, retv, -42, 42);
	checkint("inline call result 1", retv->arg1, -42);
	checkint("inline call result 2", retv->arg2, 42);
}

static void
UNIT_TEST_FUNC(call_array)(
		__unused const unit_t *un)
{
	call_retv_t(test_call_array) *retv = call_retv_new(test_call_array,
		.arg1 = INT_MAX,
		.arg2 = INT_MIN,
		.arg3 = "xxxxxx",
	);
	const char *__cstr arg3_chk = cstr(retv->arg3);

	zxcall(test_call_array, retv, -64, 64, "frogblast the ventcore!");
	checkint("array call result 1", retv->arg1, -64);
	checkint("array call result 2", retv->arg2, 64);
	checkcstr("array call result 3", arg3_chk, "frogblast the ventcore!");
}

static void
UNIT_TEST_FUNC(call_pointer_censor)(
		__unused const unit_t *un)
{
	uint8_t *__free p1 = NULL;
	uint8_t *__free p2 = NULL;
	call_retv_t(test_call_clear) *retv = call_retv_new(test_call_clear,
		.arg1 = INT_MAX,
		.arg2 = INT_MIN,
		.arg3 = "xxxxxx",
	);
	const char *__cstr arg3_chk = cstr(retv->arg3);

	p1 = malloc(4);
	p2 = malloc(4);

	zxcall(test_call_clear, retv, 4, p1, -128, 8088, p2, 4);
	checkint("pointer call result 1", retv->arg1, -128);
	checkint("pointer call result 2", retv->arg2, 8088);
	checkcstr("pointer call result 3", arg3_chk, "done!");
}

static void
UNIT_TEST_FUNC(call_ext)(
		const unit_t *un)
{
	call_retv_t(test_call_ext) *retv = call_retv_new(test_call_ext,
		.arg1 = INT_MAX,
		.arg2 = INT_MIN,
	);

	zxcall_ext(test_call_ext, retv, NULL, CALL_EXT_PARAMS(un, 48), 96, 192);
	checkint("standard arg 1", retv->arg1, 96);
	checkint("standard arg 2", retv->arg2, 192);
	checkptr("extended arg 1", retv->unx, un);
	checkint("extended arg 2", retv->argx, 48);
}

UNIT_TEST(call_empty);
UNIT_TEST(call_inline);
UNIT_TEST(call_array);
UNIT_TEST(call_pointer_censor);
UNIT_TEST(call_ext);
