/*!
 * @copyright
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT license.
 */
#include "buff.h"
#include "buff/type_internal.h"

#include "0xc/std/api.h"
#include "0xc/std/context.h"
#include "0xc/std/pointer.h"
#include "0xc/std/defs.h"
#include "0xc/std/int.h"
#include "0xc/std/string.h"
#include "0xc/std/struct.h"

#include "0xc/sys/errno.h"
#include "0xc/sys/fourcc.h"
#include "0xc/sys/log.h"
#include "0xc/sys/panic.h"

#include <errno.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

// MARK: Log Protocol
static const log_label_t *
_buff_get_label(
		context_t obj,
		__unused const log_stream_t *st,
		log_label_t *lbl)
{
	const buff_t *bf = __context_import(buff_t *, obj);

	if (bf->bf_name) {
		lbl->lbl_type = FOURCC('BUFF');
		lbl->lbl_name = bf->bf_name;
	} else if (bf->bf_parent) {
		lbl->lbl_type = FOURCC('CURS');
		lbl->lbl_name = bf->bf_parent->bf_name;
	} else if (bf->bf_lender) {
		lbl->lbl_type = FOURCC('LOAN');
		lbl->lbl_name = bf->bf_lender->bf_name;
	} else {
		lbl->lbl_type = FOURCC('BUFF');
		lbl->lbl_name = bf->bf_type->bft_name;
	}

	return lbl;
}

const log_proto_t _log_proto_buff = {
	.log_version = LOG_PROTO_VERSION,
	.log_get_label = &_buff_get_label,
};

// MARK: Module Private
buff_t *
_buff_init(
		buff_t *bf,
		const buff_type_t *type,
		const char *_Nullable __cstr name)
{
	struct_version_assert(buff_t, bf->bf_version, 0, BUFF_VERSION);
	bf->log_proto = LOG_PROTO_BUFF;
	bf->bf_type = type;
	bf->bf_name = NULL;
	bf->bf_parent = NULL;
	bf->bf_loanee = NULL;
	bf->bf_lender = NULL;
	bf->bf_p = NULL;
	bf->bf_len = 0;

	if (name) {
		strscpy(bf->bf_name_mem, name);
		bf->bf_name = cstr(bf->bf_name_mem);
	}

	return bf;
}

static buff_t *
_buff_get_cursor(
		const buff_t *bf,
		buff_t *bf_cursor,
		buff_t *bf_zcursor)
{
	ptrdiff_t off = 0;

	if (!bf_cursor) {
		bf_cursor = buff_cursor(bf_zcursor, bf, 0);
	}

	if (bf_cursor->bf_parent != bf) {
		panicx("cursor does not refer to buffer being traversed");
	}

	if (bf_cursor->bf_len > bf->bf_len) {
		panicx("cursor length exceeds parent buffer length: "
				"cursor len = %lu, parent len = %lu",
				bf_cursor->bf_len, bf->bf_len);
	}

	off = bf_cursor->bf_p - bf->bf_p;
	if (off < 0) {
		panicx("cursor points before start of parent buffer: off = %ld", off);
	}

	return bf_cursor;
}

static buff_t *
_buff_advance_cursor(
		buff_t *bf_cursor,
		size_t n)
{
	if (n > bf_cursor->bf_len) {
		panicx("attempt to advance cursor beyond buffer end: "
				"advance = %zu, remaining = %zu",
				n, bf_cursor->bf_len);
	}

	bf_cursor->bf_p += n;
	bf_cursor->bf_len -= n;
	return bf_cursor;
}

static errno_t
_buff_read_flexible_array_count(
		const buff_t *bf,
		const struct_field_t *fld,
		const buff_t *bf_obj,
		size_t *cnt)
{
	errno_t __posix_error error = -1;
	integer_value_t *iv = __new(integer_value_t);
	uintmax_t v_unsigned = 0;
	intmax_t v_signed = 0;
	size_t cnt_scope = 0;

	if (!fld) {
		error = ENOENT;
		zxinfono(bf, error, "no flexible array count field in object");
		goto __out;
	}

	zxdebugx(bf, "reading flexible array count field: "
			"obj = %s, object len = %zu, field = %s, field width = %zu, "
			"field off = %zu, max = %#llx",
			fld->sf_struct, bf_obj->bf_len, fld->sf_fld, fld->sf_size,
			fld->sf_off, oxollu(fld->sf_maxint));

	iv = struct_field_read_integer(fld, bf_obj->bf_p, bf_obj->bf_len, iv);
	if (!iv) {
		error = EOVERFLOW;
		zxerrno(bf, error, "failed to read flexible array count field: "
				"obj = %s, field = %s, field width = %zu",
				fld->sf_struct, fld->sf_fld, fld->sf_size);
		goto __out;
	}

	switch (fld->sf_size) {
	case sizeof(uint8_t):
		v_unsigned = iv->iv_int8.vu;
		v_signed = iv->iv_int8.v;
		break;
	case sizeof(uint16_t):
		v_unsigned = iv->iv_int16.vu;
		v_signed = iv->iv_int16.v;
		break;
	case sizeof(uint32_t):
		v_unsigned = iv->iv_int32.vu;
		v_signed = iv->iv_int32.v;
		break;
	case sizeof(uint64_t):
		v_unsigned = iv->iv_int64.vu;
		v_signed = iv->iv_int64.v;
		break;
	default:
		panic_switch(fld->sf_size);
	}

	switch (fld->sf_maxint) {
	case INT8_MAX:
	case INT16_MAX:
	case INT32_MAX:
	case INT64_MAX:
		if (v_signed < 0) {
			error = EDOM;
			zxerrno(bf, error, "flexible array length is negative: "
					"obj = %s, field = %s, length = %lld",
					fld->sf_struct, fld->sf_fld,
					oxolld(v_signed));
			goto __out;
		}

		cnt_scope = __cast_signed_unsigned(size_t, v_signed);
		break;
	default:
		cnt_scope = __cast_unsigned_unsigned(size_t, v_unsigned);
		break;
	}

	zxdebugx(bf, "read flexible array count: "
			"obj = %s, count field = %s, count = %zu, max = %#llx",
			fld->sf_struct, fld->sf_fld,  cnt_scope, oxollu(fld->sf_maxint));

	*cnt = cnt_scope;
	error = 0;

__out:
	return error;
}

static errno_t
_buff_read_flexible_array(
		const buff_t *bf,
		buff_t *bf_cursor,
		const buff_t *bf_obj,
		const struct_field_t *fla_fld,
		const struct_field_t *flacnt_fld,
		const void *__sized_by_or_null(*fla_len) *fla,
		size_t *fla_len)
{
	errno_t __posix_error error = -1;
	bool overflow = false;
	size_t cnt = 0;
	size_t elmsize = 0;
	size_t nbytes = 0;
	size_t fla_len_scope = 0;
	const void *__sized_by_or_null(fla_len_scope) fla_scope = NULL;

	error = _buff_read_flexible_array_count(bf, flacnt_fld, bf_obj, &cnt);
	switch (error) {
	case 0:
		break;
	case ENOENT:
		error = 0;
		goto __out;
	default:
		zxerrno(bf, error, "failed to read flexible array count");
		goto __out;
	}

	elmsize = fla_fld->sf_size;
	zxdebugx(bf, "reading flexible array: "
			"obj = %s, field = %s, element size = %zu, count = %zu",
			fla_fld->sf_struct, fla_fld->sf_fld, elmsize, cnt);

	overflow = __builtin_mul_overflow(elmsize, cnt, &nbytes);
	if (overflow) {
		error = EOVERFLOW;
		zxerrno(bf, error, "flexible array size overflow: "
				"obj = %s, field = %s, count = %zu, element size = %zu",
				fla_fld->sf_struct, fla_fld->sf_fld, cnt, elmsize);
		goto __out;
	}

	fla_scope = buff_read(bf, bf_cursor, nbytes);
	if (!__bounds_check_and_set(fla_scope, fla_len_scope, nbytes)) {
		error = EOVERFLOW;
		zxerrno(bf, error, "failed to read flexible array: length = %zu", cnt);
		goto __out;
	}

	error = 0;

__out:
	*fla = fla_scope;
	*fla_len = fla_len_scope;

	return error;
}

// MARK: Internal
void *_Nullable __single
_buff_map_object(
		const buff_t *bf,
		buff_t *_Nullable bf_cursor,
		const buff_object_descriptor_t *bod,
		size_t *n_mapped)
{
	size_t chk_len = 0;
	void *__sized_by(chk_len) chk = NULL;
	const void *__single obj = NULL;
	size_t n_read = 0;

	// Attempt to map zero bytes just to ensure that we can get mutable access
	// to the buffer.
	chk = buff_map(bf, bf_cursor, 0);
	if (!__bounds_check_and_affirm(chk, chk_len)) {
		zxerrx(bf, "buffer does not support mutable access");
		return NULL;
	}

	obj = _buff_read_object(bf, bf_cursor, bod, &n_read);
	if (!obj) {
		zxerrx(bf, "insufficient space in buffer for object map: "
				"obj = %s, obj size = %lu, remaining = %lu",
				bod->bfod_objname, bod->bfod_objsize, bf_cursor->bf_len);
		return NULL;
	}

	*n_mapped = n_read;
	return __DECONST(void *, obj);
}

const void *_Nullable __single
_buff_read_object(
		const buff_t *bf,
		buff_t *_Nullable bf_cursor,
		const buff_object_descriptor_t *bod,
		size_t *n_read)
{
	errno_t error = -1;
	buff_t bf_zcursor = BUFF_INIT;
	size_t obj_base_len = bod->bfod_objsize;
	const void *__sized_by_or_null(obj_base_len) obj_base = NULL;
	buff_t *__buff_destroy bf_obj = buff_new();
	size_t fla_len = 0;
	const void *__sized_by(fla_len) fla = NULL;
	size_t obj_total = 0;
	size_t obj_len = 0;
	const void *__sized_by(obj_len) obj = NULL;

	// We borrow from the cursor buffer since we need mutable access to the
	// lender buffer in order to record the loanee, and it points to where we
	// need to read the object from anyway.
	bf_cursor = _buff_get_cursor(bf, bf_cursor, &bf_zcursor);
	bf_obj = buff_borrow(bf_obj, bf_cursor);

	obj_base = buff_read(bf, bf_cursor, obj_base_len);
	if (!__bounds_check_and_affirm(obj_base, obj_base_len)) {
		zxerrx(bf, "insufficient space in buffer for object map: "
				"obj = %s, obj size = %lu, remaining = %lu",
				bod->bfod_objname, bod->bfod_objsize, bf_cursor->bf_len);
		return NULL;
	}

	obj_total = obj_base_len;

	error = _buff_read_flexible_array(bf, bf_cursor, bf_obj,
			bod->bfod_fla, bod->bfod_flacnt, &fla, &fla_len);
	if (error) {
		zxerrno(bf, error, "failed to read flexible array: "
				"obj = %s, count field = %s, count field width = %zu "
				"flexible array field = %s, flexible array element size = %zu",
				bod->bfod_objname, bod->bfod_flacnt->sf_fld,
				bod->bfod_flacnt->sf_size, bod->bfod_fla->sf_fld,
				bod->bfod_fla->sf_size);
		return NULL;
	}

	obj_total += fla_len;
	obj = __unsafe_forge(const void *, obj_base, bidi, obj_total);
	obj_len = obj_total;

	*n_read = obj_len;
	return __DECONST(void *, obj);
}

const void *__single
_buff_write_object(
		const buff_t *bf,
		buff_t *_Nullable bf_cursor,
		const buff_object_descriptor_t *bod,
		size_t *n_written)
{
	size_t obj_wire_len = 0;
	const void *__sized_by_or_null(obj_wire_len) obj_wire = NULL;
	size_t obj_len = bod->bfod_objsize;
	const void *__sized_by_or_null(obj_len) obj = bod->bfod_obj;

	obj_wire = buff_write(bf, bf_cursor, obj, obj_len);
	if (!__bounds_check_and_set(obj_wire, obj_wire_len, obj_len)) {
		return NULL;
	}

	*n_written = obj_wire_len;
	return obj_wire;
}

// MARK: Public
buff_t *
buff_init(
		buff_t *bf,
		const buff_type_t *type,
		const char *_Nullable __cstr name,
		void *__sized_by(*p_len) *const __single p,
		size_t *p_len)
{
	bf = _buff_init(bf, type, name);
	bf->bf_p = *p;
	bf->bf_len = *p_len;

	*p = NULL;
	*p_len = 0;
	return bf;
}

buff_t *
buff_init_wrap(
		buff_t *bf,
		const char *_Nullable __cstr name,
		const void *__sized_by(p_len) p,
		size_t p_len)
{
	bf = _buff_init(bf, BUFF_TYPE_NULL, name);
	bf->bf_p = p;
	bf->bf_len = p_len;

	return bf;
}

buff_t *
buff_borrow(
		buff_t *bf,
		buff_t *bf_lender)
{
	if (bf_lender->bf_loanee) {
		panicx("loanee still outstanding");
	}

	bf = _buff_init(bf, BUFF_TYPE_LOANEE, NULL);
	bf->bf_lender = bf_lender;
	bf->bf_p = bf_lender->bf_p;
	bf->bf_len = bf_lender->bf_len;

	bf_lender->bf_loanee = bf;
	return bf;
}

buff_t *
buff_cursor(
		buff_t *bf,
		const buff_t *bf_parent,
		size_t off)
{
	if (off > bf_parent->bf_len) {
		zxerrx(bf_parent, "offset exceeds parent buffer length: "
				"off = %zu, parent len = %zu",
				off, bf_parent->bf_len);
		return NULL;
	}

	bf->log_proto = LOG_PROTO_BUFF;
	bf->bf_type = BUFF_TYPE_CURSOR;
	bf->bf_parent = bf_parent;
	bf->bf_p = bf_parent->bf_p;
	bf->bf_len = bf_parent->bf_len;

	return _buff_advance_cursor(bf, off);
}

void *__sized_by_or_null(len)
buff_map(
		const buff_t *bf,
		buff_t *bf_cursor,
		size_t len)
{
	buff_t bf_zcursor = BUFF_INIT;
	size_t mutable_len = 0;
	size_t p_mutable_len = 0;
	void *__sized_by(p_mutable_len) p_mutable = NULL;

	bf_cursor = _buff_get_cursor(bf, bf_cursor, &bf_zcursor);
	if (len > bf_cursor->bf_len) {
		ptrdiff_t off = bf_cursor->bf_p - bf->bf_p;

		zxerrx(bf, "insufficient space in buffer for map: "
				"off = %lld, remaining = %lu, expected >= %lu",
				oxolld(off), bf_cursor->bf_len, len);
		return NULL;
	}

	p_mutable = buff_type_get_mutable(bf_cursor, &mutable_len);
	if (!__bounds_check_and_set(p_mutable, p_mutable_len, mutable_len)) {
		zxerrx(bf, "buffer type does not support mutable access: %s",
				bf->bf_type->bft_name);
		return NULL;
	}

	_buff_advance_cursor(bf_cursor, len);
	__bounds_affirm(p_mutable);
	p_mutable_len = len;

	return p_mutable;
}

const uint8_t *__sized_by_or_null(n)
buff_read(
		const buff_t *bf,
		buff_t *bf_cursor,
		size_t n)
{
	buff_t bf_zcursor = BUFF_INIT;
	__unused size_t p_len = 0;
	const void *__sized_by(p_len) p = NULL;

	bf_cursor = _buff_get_cursor(bf, bf_cursor, &bf_zcursor);
	if (n > bf_cursor->bf_len) {
		ptrdiff_t off = bf_cursor->bf_p - bf->bf_p;

		zxerrx(bf, "insufficient space in buffer for read: "
				"off = %lld, remaining = %lu, expected >= %lu",
				oxolld(off), bf_cursor->bf_len, n);
		return NULL;
	}

	p = bf_cursor->bf_p;
	__bounds_update(p_len, n);

	_buff_advance_cursor(bf_cursor, n);
	return p;
}

const uint8_t *_Nullable __sized_by_or_null(p_len)
buff_write(
		const buff_t *bf,
		buff_t *bf_cursor,
		const uint8_t *__sized_by(p_len) p,
		size_t p_len)
{
	buff_t bf_zcursor = BUFF_INIT;
	size_t mutable_len = 0;
	size_t p_mutablelen = 0;
	void *__sized_by(p_mutablelen) p_mutable = NULL;

	bf_cursor = _buff_get_cursor(bf, bf_cursor, &bf_zcursor);
	if (p_len > bf_cursor->bf_len) {
		ptrdiff_t off = bf_cursor->bf_p - bf->bf_p;

		zxerrx(bf, "insufficient space in buffer for write: "
				"off = %lld, remaining = %lu, expected >= %lu",
				oxolld(off), bf_cursor->bf_len, p_len);
		return NULL;
	}

	p_mutable = buff_type_get_mutable(bf_cursor, &mutable_len);
	if (!__bounds_check_and_set(p_mutable, p_mutablelen, mutable_len)) {
		zxerrx(bf, "buffer type does not support mutable access: %s",
				bf->bf_type->bft_name);
		return NULL;
	}

	memcpy(p_mutable, p, p_len);
	_buff_advance_cursor(bf_cursor, p_len);

	return p_mutable;
}

void
buff_destroy(
		buff_t **bfp)
{
	buff_t *bf = NULL;

	bf = *bfp;
	if (!bf) {
		return;
	}

	buff_type_destruct(bf);
	*bfp = NULL;
}
