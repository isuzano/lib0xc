/*!
 * @copyright
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT license.
 *
 * @header
 * Check operator. Operators are lightweight encapsulations around arithmetic
 * operators like '=' or '<='.
 */
#ifndef __ZX_SYS_CHECK_OPERATOR_H
#define __ZX_SYS_CHECK_OPERATOR_H

#include "0xc/std/api.h"

__API_HEADER_BEGIN(c, nonnull, single)

// MARK: Object
typedef struct _check_operator {
	const char *__cstr chkop_name;
	const char *__cstr chkop_token;
} check_operator_t;

// MARK: Globals
__private
const check_operator_t _check_operator_eq;
#define CHECK_OPERATOR_EQ (&_check_operator_eq)

__private
const check_operator_t _check_operator_ne;
#define CHECK_OPERATOR_NE (&_check_operator_ne)

__private
const check_operator_t _check_operator_gt;
#define CHECK_OPERATOR_GT (&_check_operator_gt)

__private
const check_operator_t _check_operator_lt;
#define CHECK_OPERATOR_LT (&_check_operator_lt)

__private
const check_operator_t _check_operator_ge;
#define CHECK_OPERATOR_GE (&_check_operator_ge)

__private
const check_operator_t _check_operator_le;
#define CHECK_OPERATOR_LE (&_check_operator_le)

__API_HEADER_END(c, nonnull, single)

#endif // __ZX_SYS_CHECK_OPERATOR_H
