/*!
 * @copyright
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT license.
 */
#include "context.h"

#include "0xc/std/defs.h"
#include "0xc/std/int.h"
#include "0xc/std/io.h"
#include "0xc/std/pointer.h"
#include "0xc/sys/panic.h"

#include <stdint.h>
#include <stdlib.h>

// MARK: Public
context_t
__context_wrap(
		void *_Nullable p,
		__unused const char *_Nullable type_name,
		size_t len)
{
	ptrtag_t len32 = __cast_unsigned_unsigned(ptrtag_t, len);
	return __pointer_try_tag_and_get(p, len32);
}

void *__unsafe_indexable
__context_unwrap(
		context_t ctxt,
		const char *type_name,
		size_t len)
{
	void *__unsafe_indexable _Nullable p = NULL;
	uint32_t len32 = __cast_unsigned_unsigned(uint32_t, len);
	uint32_t tag = 0;

	p = __pointer_clr_tag_and_get(ctxt, &tag);
	if (tag && tag != len32) {
		panicx("cannot import context pointer of type '%s': "
				"actual length = %zu, expected length = %zu",
				type_name, oxozu(tag), len);
	}

	return p;
}
