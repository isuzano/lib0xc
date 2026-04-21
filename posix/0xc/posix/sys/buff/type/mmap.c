/*!
 * @copyright
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT license.
 */
#include "0xc/sys/buff.h"
#include "0xc/sys/buff/type_internal.h"

#include "0xc/std/defs.h"
#include "0xc/sys/posixdefs.h"

#include <stdlib.h>
#include <sys/mman.h>

// MARK: Callbacks
static void *__sized_by(*len)
_buff_mmap_get_mutable(
		const buff_t *bf,
		size_t *len)
{
	*len = bf->bf_len;
	return __DECONST(void *, bf->bf_p);
}

static void
_buff_mmap_destruct(
		__unused const buff_t *bf,
		void *__sized_by(len) p,
		size_t len)
{
	int ret = -1;

	ret = munmap(p, len);
	posix_assert("munmap", ret);
}

// MARK: Descriptor
const buff_type_t _buff_type_mmap = {
	.bft_name = "posix mmap",
	.bft_get_mutable = &_buff_mmap_get_mutable,
	.bft_destruct = &_buff_mmap_destruct,
};
