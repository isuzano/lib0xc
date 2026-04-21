/*!
 * @copyright
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT license.
 */
#include "operator.h"

// MARK: Globals
const check_operator_t _check_operator_eq = {
	.chkop_name = "equal",
	.chkop_token = "=",
};

const check_operator_t _check_operator_ne = {
	.chkop_name = "not equal",
	.chkop_token = "!=",
};

const check_operator_t _check_operator_gt = {
	.chkop_name = "greater than",
	.chkop_token = ">",
};

const check_operator_t _check_operator_lt = {
	.chkop_name = "less than",
	.chkop_token = "<",
};

const check_operator_t _check_operator_ge = {
	.chkop_name = "greater than or equal",
	.chkop_token = ">=",
};

const check_operator_t _check_operator_le = {
	.chkop_name = "less than or equal",
	.chkop_token = "<=",
};
