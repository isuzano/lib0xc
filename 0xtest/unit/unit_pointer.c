/*!
 * @copyright
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT license.
 */
#include "0xc/sys/check.h"
#include "0xc/sys/unit.h"
#include "0xc/std/pointer.h"

#include "0xc/std/defs.h"
#include "0xc/std/alloc.h"

#include <stdlib.h>

// MARK: Test
static void
UNIT_TEST_FUNC(pointer)(
		__unused const unit_t *un)
{
	void *__free p = NULL;
	void *__unsafe_indexable p_chk_unsafe = NULL;
	void *__single p_chk = NULL;
	tagptr_t tp = TAGPTR_NULL;
	ptrtag_t tag = __pointer_get_max_taggable();
	ptrtag_t tag_chk = 0;

	p = malloc(128);
	checknonnull("allocated memory", p);

	tp = __pointer_tag_and_get(p, tag);
	p_chk_unsafe = __pointer_clr_tag_and_get(tp, &tag_chk);
	p_chk = __unsafe_forge(void *, p_chk_unsafe, single);
	checkptr("pointer tagged/untagged", p_chk, p);
	checkuint("tag extracted", tag_chk, tag);

	tp = __pointer_try_tag_and_get(p, tag + 1);
	p_chk = __unsafe_forge(void *, tp.__opaque, single);
	checkptr("pointer tag overflow avoided", p, p_chk);

	p_chk_unsafe = __pointer_clr_tag_and_get(tp, &tag_chk);
	p_chk = __unsafe_forge(void *, p_chk_unsafe, single);
	checkptr("pointer preserved", p_chk, p);
	checkuint("zero tag extracted", tag_chk, 0);

	tag /= 2;
	tp = __pointer_tag_and_get(p, tag);
	p_chk_unsafe = __pointer_clr_tag_and_get(tp, &tag_chk);
	p_chk = __unsafe_forge(void *, p_chk_unsafe, single);
	checkptr("pointer tagged/untagged", p_chk, p);
	checkuint("tag extracted", tag_chk, tag);
}

UNIT_TEST(pointer);
