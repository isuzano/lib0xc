/*!
 * @copyright
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT license.
 *
 * @header
 * Check object.
 */
#ifndef __ZX_SYS_CHECK_INTERNAL_H
#define __ZX_SYS_CHECK_INTERNAL_H

#include "0xc/sys/log.h"
#include "check.h"

#include "0xc/std/api.h"

#include <stdlib.h>

__API_HEADER_BEGIN(c, nonnull, single)

// MARK: Object
struct _check {
	const log_proto_t *log_proto;
	const char *__cstr chk_name;
};
DECL_TYPE_4IWYU(check_t);

__API_HEADER_END(c, nonnull, single)

#endif // __ZX_SYS_CHECK_INTERNAL_H
