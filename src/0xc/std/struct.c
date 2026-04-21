/*!
 * @copyright
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT license.
 */
#include "struct.h"

#include "0xc/std/defs.h"
#include "0xc/std/int.h"
#include "0xc/std/pointer.h"
#include "0xc/sys/panic.h"

#include <stdint.h>
#include <stdlib.h>

// MARK: Internal
integer_value_t *
struct_field_read_integer(
		const struct_field_t *fld,
		const void *__sized_by(p_len) p,
		size_t p_len,
		integer_value_t *iv)
{
	size_t vp_len = 0;
	const void *__sized_by(vp_len) vp = NULL;

	if (!fld->sf_maxint) {
		panicx("field is not an integer type: "
				"obj = %s, field = %s",
				fld->sf_struct, fld->sf_fld);
	}

	vp = struct_field_get_untyped(fld, p, p_len);
	if (!__bounds_check_and_set(vp, vp_len, fld->sf_size)) {
		return NULL;
	}

	switch (fld->sf_size) {
	case sizeof(uint8_t):
		iv->iv_int8.vu = *(const uint8_t *)vp;
		return iv;
	case sizeof(uint16_t):
		iv->iv_int16.vu = *(const uint16_t *)vp;
		return iv;
	case sizeof(uint32_t):
		iv->iv_int32.vu = *(const uint32_t *)vp;
		return iv;
	case sizeof(uint64_t):
		iv->iv_int64.vu = *(const uint64_t *)vp;
		return iv;
	default:
		panic_switch(fld->sf_size);
	}

	__unreachable();
}
