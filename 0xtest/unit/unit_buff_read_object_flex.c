/*!
 * @copyright
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT license.
 */
#include "0xc/sys/check.h"
#include "0xc/sys/unit.h"
#include "0xc/sys/buff.h"

#include "0xc/std/api.h"
#include "0xc/std/alloc.h"
#include "0xc/std/struct.h"
#include "0xc/std/int.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

// MARK: IWYU
DECL_TYPE_4IWYU(ssize_t);

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
UNIT_TEST_FUNC(buff_read_object_flex)(
		__unused const unit_t *un)
{
	buff_t *__buff_destroy bf = buff_new();
	object_flex_unsigned_t *__single obj = NULL;
	const object_flex_unsigned_t *__single obj_fromwire = NULL;
	uint16_t cnt = 24;
	size_t obj_size = 0;
	size_t n_total = 0;

	obj = objallocx(object_flex_unsigned_t, obj_data_cnt, cnt);
	obj->obj_f1 = 0x59;
	obj->obj_f2 = 0x67;
	memset(obj->obj_data, 'A', cnt);
	obj_size = flexsizeof(obj, obj_data, obj_data_cnt);

	bf = buff_init_wrap(bf, "flexible obj", obj, obj_size);
	obj_fromwire = buff_read_object_flexible(bf, NULL,
			object_flex_unsigned_t, obj_data, obj_data_cnt,
			&n_total);
	checknonnull("object", obj_fromwire);
	checkuint("total bytes read", n_total, obj_size);
	checkuint("flex data count", obj_fromwire->obj_data_cnt, cnt);

	checkmem("object data consistent",
			obj->obj_data, obj->obj_data_cnt,
			obj_fromwire->obj_data, obj_fromwire->obj_data_cnt);
}

static void
UNIT_TEST_FUNC(buff_read_object_flex_signed)(
		__unused const unit_t *un)
{
	buff_t *__buff_destroy bf = buff_new();
	object_flex_signed_t *__single obj = NULL;
	const object_flex_signed_t *__single obj_fromwire = NULL;
	int32_t cnt = 24;
	ssize_t cnt_signed = __cast_signed_signed(ssize_t, cnt);
	size_t obj_size = 0;
	size_t n_total = 0;

	obj = objallocx(object_flex_signed_t, obj_data_cnt, cnt);
	obj->obj_f1 = 0x59;
	obj->obj_f2 = 0x67;
	memset(obj->obj_data, 'B', 24);
	obj_size = flexsizeof(obj, obj_data, obj_data_cnt);

	bf = buff_init_wrap(bf, "flexible obj", obj, obj_size);
	obj_fromwire = buff_read_object_flexible(bf, NULL,
			object_flex_signed_t, obj_data, obj_data_cnt,
			&n_total);
	checknonnull("object", obj_fromwire);
	checkuint("total bytes read", n_total, obj_size);
	checkint("flex data count", obj_fromwire->obj_data_cnt, cnt_signed);

	checkmem("object data consistent",
			obj->obj_data, __cast_signed_unsigned(size_t, obj->obj_data_cnt),
			obj_fromwire->obj_data,
			__cast_signed_unsigned(size_t, obj_fromwire->obj_data_cnt));
}

static void
UNIT_TEST_FUNC(buff_read_object_flex_bogus)(
		__unused const unit_t *un)
{
	buff_t *__buff_destroy bf = buff_new();
	static char wire[1024] = "";
	object_flex_signed_t *__single obj = NULL;
	const object_flex_signed_t *__single obj_fromwire = NULL;
	const struct_field_t *cntfld = NULL;
	int32_t *cntmod = NULL;
	int32_t cnt = 24;
	uint32_t cnt_unsigned = __cast_signed_unsigned(uint32_t, cnt);
	size_t obj_size = 0;
	size_t n_total = 0;

	bf = buff_init_wrap(bf, NULL, wire, sizeof(wire));
	obj = objallocx(object_flex_signed_t, obj_data_cnt, cnt);
	obj->obj_f1 = 0x59;
	obj->obj_f2 = 0x67;
	memset(obj->obj_data, 'A', cnt_unsigned);
	obj_size = flexsizeof(obj, obj_data, obj_data_cnt);
	memcpy(wire, obj, obj_size);

	// Corrupt the count field.
	cntfld = struct_field(object_flex_signed_t, obj_data_cnt);
	cntmod = struct_field_get_mutable(int32_t *, cntfld, &wire[0], obj_size);
	*cntmod = 0xf00d;

	obj_fromwire = buff_read_object_flexible(bf, NULL,
			object_flex_signed_t, obj_data, obj_data_cnt,
			&n_total);
	checknull("excessive count not read", obj_fromwire);

	// Now set a negative value.
	*cntmod = -47;
	obj_fromwire = buff_read_object_flexible(bf, NULL,
			object_flex_signed_t, obj_data, obj_data_cnt,
			&n_total);
	checknull("negative count not read", obj_fromwire);
}

UNIT_TEST(buff_read_object_flex);
UNIT_TEST(buff_read_object_flex_signed);
UNIT_TEST(buff_read_object_flex_bogus);
