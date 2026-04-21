/*!
 * @copyright
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT license.
 *
 * @header
 * Check tokens encapsulate a result and string associated with that result.
 */
#ifndef __ZX_SYS_CHECK_TOKEN_H
#define __ZX_SYS_CHECK_TOKEN_H

#include "0xc/std/api.h"

#include <stdint.h>

__API_HEADER_BEGIN(c, nonnull, single)

// MARK: Supporting Types
__typed_enum(check_result, uint32_t, closed,
	CHECK_RESULT_SUCCESS,
	CHECK_RESULT_FAIL,
);

// MARK: Object
typedef struct _check_token {
	const char *__cstr chktk_string;
	check_result_t chktk_result;
} check_token_t;

// MARK: Tokens
__private
const check_token_t _check_token_pass;
#define CHECK_TOKEN_PASS (&_check_token_pass)

__private
const check_token_t _check_token_fail;
#define CHECK_TOKEN_FAIL (&_check_token_fail)

__API_HEADER_END(c, nonnull, single)

#endif // __ZX_SYS_CHECK_TOKEN_H
