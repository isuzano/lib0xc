/*!
 * @copyright
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT license.
 */
#include "token.h"

// MARK: Globals
const check_token_t _check_token_pass = {
	.chktk_string = "PASS",
	.chktk_result = CHECK_RESULT_SUCCESS,
};

const check_token_t _check_token_fail = {
	.chktk_string = "FAIL",
	.chktk_result = CHECK_RESULT_FAIL,
};
