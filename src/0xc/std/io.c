/*!
 * @copyright
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT license.
 */
#include "io.h"

#include "0xc/std/array.h"
#include "0xc/sys/panic.h"
#include "0xc/std/int.h"

#include <stdint.h>
#include <stdlib.h>

char *__sized_by(*len)
sprintf_check_and_consume(
		char *__sized_by(*len) s,
		size_t *len,
		int n,
		int *t)
{
	char *s_scope = s;
	size_t len_scope = *len;
	int len_signed = __cast_unsigned_signed(int, len_scope);
	size_t n_unsigned = 0;

	if (t && n >= 0) {
		*t += n;
	}

	if (n < 0 || n >= len_signed) {
		n = len_signed;
	}

	n_unsigned = __cast_signed_unsigned(size_t, n);
	s_scope += n_unsigned;
	len_scope -= n_unsigned;
	*len = len_scope;

	return s_scope;
}

void
snprintdgst(
		__typed_array(digest_string_t, _Nonnull, s),
		const uint8_t *__counted_by(len) dgst,
		size_t len)
{
	static const char hex[] = "0123456789abcdef";
	char *sp = s;
	size_t i = 0;

	if (len > DIGEST_BYTES_CNT) {
		panic_overflow("digest length", len, DIGEST_BYTES_CNT);
	}

	for (i = 0; i < len; i++) {
		uint8_t bi = dgst[i];
		uint8_t hi = ((bi & 0xf0) >> 4);
		uint8_t lo = (bi & 0x0f);

		sp[0] = hex[hi];
		sp[1] = hex[lo];
		sp[2] = 0;
		sp += 2;
	}
}

void
snprintmem(
		char *__counted_by(len) s,
		size_t len,
		const uint8_t *__counted_by(p_len) p,
		size_t p_len)
{
	static const char hex[] = "0123456789abcdef";
	size_t s_capacity = 0;
	char *sp = s;
	size_t i = 0;
	size_t n = p_len;

	switch (len) {
	case 0:
		// If the buffer is length 0, then we'll just wind up bailing out since
		// no loop iterations will happen.
		s_capacity = 0;
		break;
	case 1:
	case 2:
		// If the buffer is not large enough to hold the hex representation of a
		// single byte, then we want to at least put a null terminator in there,
		// so special case this since otherwise we'd return without touching the
		// buffer.
		s[0] = 0;
		return;
	default:
		// Account for the null terminator -- integer division in C truncates
		// toward zero, so odd numbers will round down and always leave the loop
		// with enough space to insert a terminator.
		s_capacity = ((len - 1) / 2);
	}

	if (s_capacity < p_len) {
		n = s_capacity;
	}

	for (i = 0; i < n; i++) {
		uint8_t bi = p[i];
		uint8_t hi = ((bi & 0xf0) >> 4);
		uint8_t lo = (bi & 0x0f);

		sp[0] = hex[hi];
		sp[1] = hex[lo];
		sp[2] = 0;
		sp += 2;
	}
}
