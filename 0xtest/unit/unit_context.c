/*!
 * @copyright
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT license.
 */
#include "0xc/sys/check.h"
#include "0xc/sys/unit.h"
#include "0xc/std/context.h"

#include "0xc/std/defs.h"

#include <stdint.h>

// MARK: Types
typedef void (*context_test_t)(
	const unit_t *un,
	context_t ctxt
);

typedef void (*context_test_unsafe_t)(
	const unit_t *un,
	void *__single ctxt
);

typedef struct _test_context {
	uint16_t a;
	uint32_t b;
	uint64_t c;
} test_context_t;

// MARK: Module Globals
static test_context_t _t = {
	.a = 0x66,
	.b = 0x5678,
	.c = 0xabdef,
};

// MARK: Test
__unused static void
context_test_fail(
		__unused const unit_t *un,
		context_t ctxt)
{
	const uint64_t *t = __context_import(uint64_t *, ctxt);
	checkuint("foo", *t, 1);
}

static void
context_test(
		__unused const unit_t *un,
		context_t ctxt)
{
	const test_context_t *t = __context_import(test_context_t *, ctxt);
	checkuint("t.a", t->a, _t.a);
	checkuint("t.b", t->b, _t.b);
	checkuint("t.c", t->c, _t.c);
}

static void
context_test_tramp(
		const unit_t *un,
		context_t ctxt,
		context_test_t f)
{
	f(un, ctxt);
}

__unused static void
context_test_unsafe_fail(
		__unused const unit_t *un,
		void *__single ctxt)
{
	const uint64_t *t = __context_import_unsafe(uint64_t *, ctxt);
	checkuint("foo", *t, 1);
}

static void
context_test_unsafe(
		__unused const unit_t *un,
		void *__single ctxt)
{
	const test_context_t *t = __context_import_unsafe(test_context_t *, ctxt);
	checkuint("t.a", t->a, _t.a);
	checkuint("t.b", t->b, _t.b);
	checkuint("t.c", t->c, _t.c);
}

static void
context_test_unsafe_tramp(
		const unit_t *un,
		void *__single ctxt,
		context_test_unsafe_t f)
{
	f(un, ctxt);
}

static void
UNIT_TEST_FUNC(context)(
		const unit_t *un)
{
	context_t ctxt = __context_export(test_context_t, &_t);
	void *__single ctxt_unsafe = __context_export_unsafe(test_context_t *, &_t);

	context_test_tramp(un, ctxt, &context_test);
	context_test_unsafe_tramp(un, ctxt_unsafe, context_test_unsafe);
}

__unused static void
UNIT_TEST_FUNC(context_fail)(
		const unit_t *un)
{
	context_t ctxt = __context_export(test_context_t, &_t);
	void *__single ctxt_unsafe = __context_export_unsafe(test_context_t *, &_t);

	context_test_tramp(un, ctxt, &context_test_fail);
	context_test_unsafe_tramp(un, ctxt_unsafe, &context_test_unsafe_fail);
}

UNIT_TEST(context);
