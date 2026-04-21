/*!
 * @copyright
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT license.
 */
#include "call.h"
#include "call_ext.h"
#include "call/field/vector.h"

#include "0xc/std/api.h"
#include "0xc/std/array.h"
#include "0xc/std/context.h"
#include "0xc/sys/fourcc.h"
#include "0xc/sys/log.h"
#include "0xc/sys/panic.h"

#include <stdlib.h>

// MARK: Log Protocol
static const log_label_t *
_call_get_label(
		context_t obj,
		__unused const log_stream_t *stream,
		log_label_t *lbl)
{
	const call_t *call = __context_import(call_t *, obj);

	switch (call->c_func_ext_cnt) {
	case 0:
		lbl->lbl_type = FOURCC('CALL');
		break;
	case 1:
		lbl->lbl_type = FOURCC('CAL1');
		break;
	case 2:
		lbl->lbl_type = FOURCC('CAL2');
		break;
	case 3:
		lbl->lbl_type = FOURCC('CAL3');
		break;
	case 4:
		lbl->lbl_type = FOURCC('CAL4');
		break;
	default:
		panic_switch(call->c_func_ext_cnt);
	}

	lbl->lbl_name = call->c_name;

	return lbl;
}

const log_proto_t _log_proto_call = {
	.log_version = LOG_PROTO_VERSION,
	.log_get_label = &_call_get_label,
};

// MARK: Public
void
call_prepare(
		const call_t *call,
		const call_field_vector_t *fvec,
		void *__sized_by(argv_size) argv,
		size_t argv_size)
{
	call_field_vector_prepare(fvec, call, argv, argv_size);
}

void
call_trap(
		const call_t *call,
		const __typed_array(call_ext_params_t, _Nullable, extparams),
		void *__sized_by(argv_size) argv,
		size_t argv_size,
		void *_Nullable __sized_by(retv_size) retv,
		size_t retv_size)
{
	switch (call->c_func_ext_cnt) {
	case 0:
		call->c_func.cf_func(
				argv, argv_size,
				retv, retv_size);
		break;
	case 1:
		call->c_func.cf_ext1(
				extparams[0],
				argv, argv_size,
				retv, retv_size);
		break;
	case 2:
		call->c_func.cf_ext2(
				extparams[0], extparams[1],
				argv, argv_size,
				retv, retv_size);
		break;
	case 3:
		call->c_func.cf_ext3(
				extparams[0], extparams[1],
				extparams[2],
				argv, argv_size,
				retv, retv_size);
		break;
	case 4:
		call->c_func.cf_ext4(
				extparams[0], extparams[1],
				extparams[2], extparams[3],
				argv, argv_size,
				retv, retv_size);
		break;
	default:
		panic_switch(call->c_func_ext_cnt);
	}
}

// MARK: Implementation Details
void
_0xcall(
		const call_t *call,
		const call_field_vector_t *fvec,
		const __typed_array(call_ext_params_t, _Nullable, extparams),
		void *__sized_by(argv_size) argv,
		size_t argv_size,
		void *__sized_by(retv_size) retv,
		size_t retv_size)
{
	call_prepare(call, fvec, argv, argv_size);
	call_trap(call, extparams, argv, argv_size, retv, retv_size);
}
