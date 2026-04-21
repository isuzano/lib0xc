/*!
 * @copyright
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT license.
 */
#include "0xc/sys/buff.h"
#include "0xc/sys/buff/type_internal.h"

#include "0xc/std/defs.h"

#include <stdlib.h>

// MARK: Callbacks
static void *__sized_by(*len)
_buff_malloc_get_mutable(
		const buff_t *bf,
		size_t *len)
{
	*len = bf->bf_len;
	return __DECONST(void *, bf->bf_p);
}

static void
_buff_malloc_destruct(
		__unused const buff_t *bf,
		void *__sized_by(len) p,
		__unused size_t len)
{
	free(p);
}

// MARK: Descriptor
const buff_type_t _buff_type_malloc = {
	.bft_name = "ansi malloc",
	.bft_get_mutable = &_buff_malloc_get_mutable,
	.bft_destruct = &_buff_malloc_destruct,
};
