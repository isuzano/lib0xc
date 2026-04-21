/*!
 * @copyright
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT license.
 *
 * @header
 * Field vector for calls. Field vectors contain fields which require special
 * handling during a call, e.g. fields which refer to out-of-line memory.
 */
#ifndef __ZX_STD_CALL_FIELD_VECTOR_H
#define __ZX_STD_CALL_FIELD_VECTOR_H

#include "0xc/std/call.h"
#include "0xc/std/api.h"

#include <stdlib.h>

__API_HEADER_BEGIN(c, nonnull, single)

// MARK: Public
__private __nonnull1 __nonnull2 __nonnull3
void
call_field_vector_prepare(
	const call_field_vector_t *fvec,
	const call_t *call,
	void *__sized_by(argv_size) argv,
	size_t argv_size);

__API_HEADER_END(c, nonnull, single)

#endif // __ZX_STD_CALL_FIELD_VECTOR_H
