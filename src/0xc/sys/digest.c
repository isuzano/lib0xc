/*!
 * @copyright
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT license.
 */
#include "0xc/sys/digest.h"

#include "0xc/std/array.h"
#include "0xc/std/io.h"
#include "0xc/sys/panic.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

// MARK: Public
const digest_t *
digest_init(
		digest_t *d,
		const uint8_t *__counted_by(len) bytes,
		size_t len)
{
	if (len > sizeof(d->d_bytes)) {
		panic_overflow("digest length", len, sizeof(d->d_bytes));
	}

	memcpy(d->d_bytes, bytes, len);
	d->d_len = len;
	return d;
}

int
digest_compare(
		const digest_t *lhs,
		const digest_t *rhs)
{
	if (lhs->d_len != rhs->d_len) {
		return -1;
	}
	return memcmp(lhs->d_bytes, rhs->d_bytes, lhs->d_len);
}

void
digest_print(
		const digest_t *d,
		__typed_array(digest_string_t, _Nonnull, ds))
{
	snprintdgst(ds, d->d_bytes, d->d_len);
}
