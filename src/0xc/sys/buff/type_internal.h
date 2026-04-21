/*!
 * @copyright
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT license.
 *
 * @header
 * Buffer type descriptor.
 */
#ifndef __ZX_SYS_BUFF_TYPE_INTERNAL_H
#define __ZX_SYS_BUFF_TYPE_INTERNAL_H

#include "0xc/std/api.h"
#include "0xc/sys/buff.h"

#include <stdlib.h>

__API_HEADER_BEGIN(c, nonnull, single)

// MARK: Supporting Types
// This is marked as returning a single pointer because the bounds safety
// implementation doesn't recognize typedef'ed function parameters properly and
// will throw an error saying that we cannot use the dereference operator for
// the __sized_by() expression, since that is only supported on function
// parameters because it doesn't think that the function type parameter is a
// named parameter.
//
// So instead we just keep the function type as returning a single pointer and
// then forge a pointer of the appropriate length from that.
typedef void *_Nullable __single (*buff_type_get_mutable_t)(
	const buff_t *bf,
	size_t *len
);

typedef void (*buff_type_destructor_t)(
	const buff_t *bf,
	void *__sized_by(p_len) p,
	size_t p_len
);

// MARK: Object
struct _buff_type {
	const char *__cstr bft_name;
	buff_type_get_mutable_t bft_get_mutable;
	buff_type_destructor_t bft_destruct;
};
DECL_TYPE_4IWYU(buff_type_t);

// MARK: Public
__export __warn_result __nonnull1 __nonnull2
void *_Nullable __sized_by(*len)
buff_type_get_mutable(
	const buff_t *bf,
	size_t *len);

__export __nonnull1
void
buff_type_destruct(
	buff_t *bf);

__API_HEADER_END(c, nonnull, single)

#endif // __ZX_SYS_BUFF_TYPE_INTERNAL_H
