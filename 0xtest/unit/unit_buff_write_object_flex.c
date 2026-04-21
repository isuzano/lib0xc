/*!
 * @copyright
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT license.
 */
#include "0xc/sys/check.h"
#include "0xc/sys/unit.h"
#include "0xc/sys/buff.h"

#include "0xc/std/defs.h"
#include "0xc/std/struct.h"
#include "0xc/std/int.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

// MARK: Types
typedef struct _object_flex_unsigned {
	uint32_t obj_f1;
	uint16_t obj_f2;
	uint16_t obj_data_cnt;
	uint8_t obj_data[__counted_by(obj_data_cnt)];
} object_flex_unsigned_t;

typedef struct _object_flex_signed {
	uint32_t obj_f1;
	uint16_t obj_f2;
	int32_t obj_data_cnt;
	uint8_t obj_data[__counted_by(obj_data_cnt)];
} object_flex_signed_t;

// MARK: Test
static void
UNIT_TEST_FUNC(buff_write_object_flex)(
		__unused const unit_t *un)
{
	object_flex_unsigned_t *__single obj = NULL;
	const object_flex_unsigned_t *__single obj_wire = NULL;
	uint16_t cnt = 48;
	static char wire[1024];
	buff_t *__buff_destroy bf_wire = buff_new();
	size_t obj_size = 0;
	size_t n_written = 0;

	obj = __newx(object_flex_unsigned_t, obj_data, obj_data_cnt, cnt);
	obj->obj_f1 = 0x59;
	obj->obj_f2 = 0x67;
	memset(obj->obj_data, 'C', cnt);
	obj_size = flexsizeof(obj, obj_data, obj_data_cnt);

	bf_wire = buff_init_wrap(bf_wire, "wire", wire, sizeof(wire));
	obj_wire = buff_write_object_flexible(bf_wire, NULL,
			object_flex_unsigned_t, obj_data, obj_data_cnt,
			obj, &n_written);
	checknonnull("object written", obj_wire);
	checkuint("bytes written", n_written, obj_size);
	checkuint("flex data count", obj_wire->obj_data_cnt, cnt);
	checkmem("object data consistent",
			obj->obj_data, obj->obj_data_cnt,
			obj_wire->obj_data, obj_wire->obj_data_cnt);
}

static void
UNIT_TEST_FUNC(buff_write_object_flex_signed)(
		__unused const unit_t *un)
{
	object_flex_signed_t *__single obj = NULL;
	const object_flex_signed_t *__single obj_wire = NULL;
	int32_t cnt = 48;
	uint32_t cnt_unsigned = __cast_signed_unsigned(uint32_t, cnt);
	static char wire[1024];
	buff_t *__buff_destroy bf_wire = buff_new();
	size_t obj_size = 0;
	size_t n_written = 0;

	obj = __newx(object_flex_signed_t, obj_data, obj_data_cnt, cnt);
	obj->obj_f1 = 0x59;
	obj->obj_f2 = 0x67;
	memset(obj->obj_data, 'C', cnt_unsigned);
	obj_size = flexsizeof(obj, obj_data, obj_data_cnt);

	bf_wire = buff_init_wrap(bf_wire, "wire", wire, sizeof(wire));
	obj_wire = buff_write_object_flexible(bf_wire, NULL,
			object_flex_signed_t, obj_data, obj_data_cnt,
			obj, &n_written);
	checknonnull("object written", obj_wire);
	checkuint("bytes written", n_written, obj_size);
	checkint("flex data count", obj_wire->obj_data_cnt, cnt);
	checkmem("object data consistent",
			obj->obj_data, __cast_signed_unsigned(size_t, obj->obj_data_cnt),
			obj_wire->obj_data,
			__cast_signed_unsigned(size_t, obj_wire->obj_data_cnt));
}

static void
UNIT_TEST_FUNC(buff_write_object_flex_bogus)(
		__unused const unit_t *un)
{
	object_flex_signed_t *__single obj = NULL;
	const object_flex_signed_t *__single obj_wire = NULL;
	const struct_field_t *cntfld = NULL;
	int32_t cnt = 48;
	uint32_t cnt_unsigned = __cast_signed_unsigned(uint32_t, cnt);
	int32_t *cntmod = NULL;
	static char wire[1024];
	buff_t *__buff_destroy bf_wire = buff_new();
	size_t obj_size = 0;
	size_t n_written = 0;

	obj = __newx(object_flex_signed_t, obj_data, obj_data_cnt, cnt);
	obj->obj_f1 = 0x59;
	obj->obj_f2 = 0x67;
	memset(obj->obj_data, 'C', cnt_unsigned);
	obj_size = flexsizeof(obj, obj_data, obj_data_cnt);

	cntfld = struct_field(object_flex_signed_t, obj_data_cnt);
	cntmod = struct_field_get_mutable(int32_t *, cntfld, obj, obj_size);
	*cntmod = 0xf00d;

	bf_wire = buff_init_wrap(bf_wire, "wire", wire, sizeof(wire));
	obj_wire = buff_write_object_flexible(bf_wire, NULL,
			object_flex_signed_t, obj_data, obj_data_cnt,
			obj, &n_written);
	checknull("overly large object not written", obj_wire);
}

UNIT_TEST(buff_write_object_flex);
UNIT_TEST(buff_write_object_flex_signed);
UNIT_TEST(buff_write_object_flex_bogus);
