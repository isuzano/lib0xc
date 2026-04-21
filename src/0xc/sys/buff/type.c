/*!
 * @copyright
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT license.
 */
#include "type_internal.h"

#include "0xc/sys/buff.h"
#include "0xc/std/api.h"
#include "0xc/std/pointer.h"

#include <stdlib.h>

// MARK: Public
void *__sized_by(*len)
buff_type_get_mutable(
		const buff_t *bf,
		size_t *len)
{
	const buff_type_t *type = bf->bf_type;
	size_t p_mutable_len = 0;
	void *__single p_mutable = NULL;
	size_t p_len = 0;
	void *__sized_by(p_len) p = NULL;

	if (bf->bf_parent) {
		type = bf->bf_parent->bf_type;
	}

	p_mutable = type->bft_get_mutable(bf, &p_mutable_len);
	if (!p_mutable) {
		return NULL;
	}

	p = __unsafe_forge(void *, p_mutable, bidi, p_mutable_len);
	p_len = p_mutable_len;
	*len = p_len;

	return p;
}

void
buff_type_destruct(
		buff_t *bf)
{
	size_t p_len_hack = 0;
	size_t p_len = 0;
	void *__sized_by(p_len) p_mutable = NULL;

	if (bf->bf_loanee) {
		panicx("cannot destroy buffer with outstanding loanee");
	}

	p_mutable = buff_type_get_mutable(bf, &p_len_hack);
	if (__bounds_check_and_set(p_mutable, p_len, p_len_hack)) {
		bf->bf_type->bft_destruct(bf, p_mutable, p_len);
	}
}
