/*!
 * @copyright
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT license.
 */
#include "vector.h"
#include "0xc/std/call.h"
#include "0xc/std/pointer.h"
#include "0xc/std/struct.h"
#include "0xc/sys/log.h"

#include <stdlib.h>

// MARK: Module Private
static void
_call_field_vector_prepare_field(
		const call_field_t *cf,
		const call_t *call,
		void *__sized_by(argv_size) argv,
		size_t argv_size,
		size_t idx)
{
	const struct_field_t *fld = &cf->cf_fld;
	const struct_field_t *fld_assoc = &cf->cf_assocfld;
	size_t p_len = 0;
	void *__sized_by(p_len) p = NULL;

	zxdebugx(call, "preparing field[%zu]: %s->%s<%d, +%zu, %zu>",
			idx, fld->sf_struct, fld->sf_fld, fld->sf_class,
			fld->sf_off, fld->sf_size);

	p = struct_field_get_untyped_mutable(fld, argv, argv_size);
	if (!__bounds_check_and_set(p, p_len, fld->sf_size)) {
		panicx("field argument out of bounds: "
				"field index = %zu, struct = %s, field = %s, "
				"off = %zu, obj size = %zu",
				idx, fld->sf_struct, fld->sf_fld, fld->sf_off, argv_size);
	}

	cf->cf_type->cfh_handler(fld, fld_assoc, p, p_len);
}

// MARK: Public
void
call_field_vector_prepare(
		const call_field_vector_t *fvec,
		const call_t *call,
		void *__sized_by(argv_size) argv,
		size_t argv_size)
{
	size_t i = 0;

	for (i = 0; i < fvec->cfv_cnt; i++) {
		const call_field_t *cf = &fvec->cfv_desc[i];
		_call_field_vector_prepare_field(cf, call, argv, argv_size, i);
	}
}
