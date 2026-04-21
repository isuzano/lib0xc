/*!
 * @copyright
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT license.
 */
#include "check.h"
#include "check_internal.h"
#include "check/operator.h"
#include "check/token.h"

#include "0xc/std/defs.h"
#include "0xc/std/io.h"
#include "0xc/std/string.h"
#include "0xc/sys/errno.h"
#include "0xc/sys/log.h"
#include "0xc/sys/panic.h"

#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

// MARK: Log Protocol
static const log_label_t *
_check_get_label(
		__unused context_t obj,
		__unused const log_stream_t *st,
		__unused log_label_t *lbl)
{
	return NULL;
}

const log_proto_t _log_proto_check_type = {
	.log_version = LOG_PROTO_VERSION,
	.log_get_label = &_check_get_label,
};

// MARK: Fatal Singleton
const check_t _check_fatal = {
	.log_proto = &_log_proto_check_type,
	.chk_name = "fatal",
};

// MARK: Module Private
static void
_check_result(
		const check_t *chk,
		const char *__cstr what,
		const check_operator_t *op,
		const check_token_t *token,
		const char *__cstr actual,
		const char *__cstr expected)
{
	switch (token->chktk_result) {
	case CHECK_RESULT_SUCCESS:
		zxinfox(chk, "%s: %s: actual = %s, expected %s %s",
				token->chktk_string, what, actual, op->chkop_token, expected);
		break;
	case CHECK_RESULT_FAIL:
		panicx("%s: %s: actual = %s, expected %s %s",
				token->chktk_string, what, actual, op->chkop_token, expected);
	default:
		panic_switch(token->chktk_result);
	}
}

// MARK: Internal
void
_checkuint(
		const check_t *chk,
		const char *__cstr fmt,
		uint64_t actual,
		uint64_t expected,
		...)
{
	const check_token_t *tok = CHECK_TOKEN_FAIL;
	const check_operator_t *op = CHECK_OPERATOR_EQ;
	va_list ap;
	char msg[256] = "";
	char lhs[64] = "";
	char rhs[64] = "";

	va_start(ap, expected);
	vssprintf(msg, fmt, ap);
	va_end(ap);

	ssprintf(lhs, "0x%llx", oxollu(actual));
	ssprintf(rhs, "0x%llx", oxollu(expected));

	if (actual == expected) {
		tok = CHECK_TOKEN_PASS;
	}

	_check_result(chk, cstr(msg), op, tok, cstr(lhs), cstr(rhs));
}

void
_checkint(
		const check_t *chk,
		const char *__cstr fmt,
		int64_t actual,
		int64_t expected,
		...)
{
	const check_token_t *tok = CHECK_TOKEN_FAIL;
	const check_operator_t *op = CHECK_OPERATOR_EQ;
	va_list ap;
	char msg[256] = "";
	char lhs[64] = "";
	char rhs[64] = "";

	va_start(ap, expected);
	vssprintf(msg, fmt, ap);
	va_end(ap);

	ssprintf(lhs, "%lld", oxolld(actual));
	ssprintf(rhs, "%lld", oxolld(expected));

	if (actual == expected) {
		tok = CHECK_TOKEN_PASS;
	}

	_check_result(chk, cstr(msg), op, tok, cstr(lhs), cstr(rhs));
}

void
_checkerrno(
		const check_t *chk,
		const char *__cstr fmt,
		errno_t actual,
		errno_t expected,
		...)
{
	const check_token_t *tok = CHECK_TOKEN_FAIL;
	const check_operator_t *op = CHECK_OPERATOR_EQ;
	va_list ap;
	char msg[256] = "";
	char lhs[64] = "";
	char rhs[64] = "";

	va_start(ap, expected);
	vssprintf(msg, fmt, ap);
	va_end(ap);

	ssprintf(lhs, "%s", symerror(actual));
	ssprintf(rhs, "%s", symerror(expected));

	if (actual == expected) {
		tok = CHECK_TOKEN_PASS;
	}

	_check_result(chk, cstr(msg), op, tok, cstr(lhs), cstr(rhs));
}

void
_checkcstr(
		const check_t *chk,
		const char *__cstr fmt,
		const char *__cstr actual,
		const char *__cstr expected,
		...)
{
	const check_token_t *tok = CHECK_TOKEN_FAIL;
	const check_operator_t *op = CHECK_OPERATOR_EQ;
	va_list ap;
	char msg[256] = "";

	va_start(ap, expected);
	vssprintf(msg, fmt, ap);
	va_end(ap);

	if (strcmp(actual, expected) == 0) {
		tok = CHECK_TOKEN_PASS;
	}
	_check_result(chk, cstr(msg), op, tok, actual, expected);
}

void
_checkptr(
		const check_t *chk,
		const char *__cstr fmt,
		const void *actual,
		const void *expected,
		...)
{
	const check_token_t *tok = CHECK_TOKEN_FAIL;
	const check_operator_t *op = CHECK_OPERATOR_EQ;
	va_list ap;
	char msg[256] = "";
	char lhs[64] = "";
	char rhs[64] = "";

	va_start(ap, expected);
	vssprintf(msg, fmt, ap);
	va_end(ap);

	ssprintf(lhs, "%p", actual);
	ssprintf(rhs, "%p", expected);

	if (actual == expected) {
		tok = CHECK_TOKEN_PASS;
	}

	_check_result(chk, cstr(msg), op, tok, cstr(lhs), cstr(rhs));
}

void
_checkmem(
		const check_t *chk,
		const char *__cstr fmt,
		const uint8_t *__counted_by(actual_len) actual,
		size_t actual_len,
		const uint8_t *__counted_by(expected_len) expected,
		size_t expected_len,
		...)
{
	const check_token_t *tok = CHECK_TOKEN_FAIL;
	const check_operator_t *op = CHECK_OPERATOR_EQ;
	va_list ap;
	char msg[256] = "";
	char lhs[32] = "";
	char rhs[32] = "";
	const char *__cstr lhs_zterm = NULL;
	const char *__cstr rhs_zterm = NULL;

	va_start(ap, expected_len);
	vssprintf(msg, fmt, ap);
	va_end(ap);

	ssprintmem(lhs, actual, actual_len);
	ssprintmem(rhs, expected, expected_len);

	do {
		if (actual_len != expected_len) {
			break;
		}
		if (memcmp(actual, expected, actual_len)) {
			break;
		}
		tok = CHECK_TOKEN_PASS;
	} while (0);

	lhs_zterm = cstr(lhs);
	rhs_zterm = cstr(rhs);
	_check_result(chk, cstr(msg), op, tok, lhs_zterm, rhs_zterm);
}

void
_checknull(
		const check_t *chk,
		const char *__cstr fmt,
		const void *actual,
		...)
{
	const check_token_t *tok = CHECK_TOKEN_FAIL;
	const check_operator_t *op = CHECK_OPERATOR_EQ;
	va_list ap;
	char msg[256] = "";
	char lhs[64] = "";
	char rhs[64] = "NULL";

	va_start(ap, actual);
	vssprintf(msg, fmt, ap);
	va_end(ap);

	ssprintf(lhs, "%p", actual);
	if (actual == NULL) {
		tok = CHECK_TOKEN_PASS;
	}

	_check_result(chk, cstr(msg), op, tok, cstr(lhs), cstr(rhs));
}

void
_checknonnull(
		const check_t *chk,
		const char *__cstr fmt,
		const void *actual,
		...)
{
	const check_token_t *tok = CHECK_TOKEN_FAIL;
	const check_operator_t *op = CHECK_OPERATOR_EQ;
	va_list ap;
	char msg[256] = "";
	char lhs[64] = "";
	char rhs[64] = "non-NULL";

	va_start(ap, actual);
	vssprintf(msg, fmt, ap);
	va_end(ap);

	ssprintf(lhs, "%p", actual);
	if (actual) {
		tok = CHECK_TOKEN_PASS;
	}

	_check_result(chk, cstr(msg), op, tok, cstr(lhs), cstr(rhs));
}
