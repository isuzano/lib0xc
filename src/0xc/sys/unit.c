/*!
 * @copyright
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT license.
 */
#include "0xc/sys/unit.h"

#include "0xc/std/defs.h"
#include "0xc/std/context.h"
#include "0xc/sys/fourcc.h"
#include "0xc/sys/log.h"

// MARK: Log Protocol
static const log_label_t *
_unit_get_label(
		context_t obj,
		__unused const log_stream_t *st,
		log_label_t *lbl)
{
	const unit_t *un = __context_import(unit_t *, obj);

	lbl->lbl_type = FOURCC('TEST');
	lbl->lbl_name = un->un_name;
	return lbl;
}

const log_proto_t _log_proto_unit = {
	.log_version = LOG_PROTO_VERSION,
	.log_get_label = &_unit_get_label,
};

// MARK: Public
void
unit_do(
		const unit_t *un)
{
	if (un->un_runnable && un->un_runnable(un)) {
		zxinfox(un, "test not runnable; skipping");
		return;
	}

	zxinfox(un, "running unit test");
	un->un_do(un);
}
