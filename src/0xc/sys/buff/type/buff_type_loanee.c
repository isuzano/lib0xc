/*!
 * @copyright
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT license.
 */
#include "0xc/sys/buff.h"
#include "0xc/sys/buff/type_internal.h"

#include "0xc/std/defs.h"
#include "0xc/std/pointer.h"

#include <stdlib.h>

// MARK: Callbacks
static void *__sized_by(*len)
_buff_loanee_get_mutable(
		const buff_t *bf,
		size_t *len)
{
	size_t p_len_mutable = 0;
	size_t p_len = 0;
	void *__sized_by(p_len) p = NULL;

	// The lender may be a cursor buffer with an adjusted pointer, so we need to
	// just use it as a check that mutable access is possible but then return
	// our own pointer.
	p = buff_type_get_mutable(bf->bf_lender, &p_len_mutable);
	if (!__bounds_check_and_set(p, p_len, p_len_mutable)) {
		return NULL;
	}

	*len = bf->bf_len;
	return __DECONST(void *, bf->bf_p);
}

static void
_buff_loanee_destruct(
		__unused const buff_t *bf,
		__unused void *__sized_by(len) p,
		__unused size_t len)
{
	bf->bf_lender->bf_loanee = NULL;
}

// MARK: Descriptor
const buff_type_t _buff_type_loanee = {
	.bft_name = "loanee",
	.bft_get_mutable = &_buff_loanee_get_mutable,
	.bft_destruct = &_buff_loanee_destruct,
};
