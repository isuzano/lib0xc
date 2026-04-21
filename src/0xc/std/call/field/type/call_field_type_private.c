/*!
 * @copyright
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT license.
 */
#include "0xc/std/call.h"

#include "0xc/std/api.h"
#include "0xc/std/struct.h"

#include <stdlib.h>
#include <string.h>

static void
_call_field_handler_private(
		__unused const struct_field_t *fld,
		__unused const struct_field_t *sf_assoc,
		void *__sized_by(p_len) p,
		size_t p_len)
{
	memset(p, 0, p_len);
}

const call_field_type_t _call_field_type_private = {
	.cfh_name = "private",
	.cfh_handler = &_call_field_handler_private,
};
