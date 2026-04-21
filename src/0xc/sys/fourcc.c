/*!
 * @copyright
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT license.
 */
#include "0xc/sys/fourcc.h"

#include "0xc/std/defs.h"
#include "0xc/std/int.h"
#include "0xc/std/pointer.h"

#include <stdint.h>

// MARK: Public
const fourcc_t *
fourcc_init(
		fourcc_t *fcc,
		uint32_t v)
{
	fcc->fcc_code = v;
	fcc->fcc_printable.fccu_network = h2n(v);
	fcc->fcc_term = 0;

	return fcc;
}

const char *
fourcc_get_string(
		const fourcc_t *fcc)
{
	const char *__cstr p = NULL;

	p = __forge(const char *, fcc->fcc_printable.fccu_string, zterm);
	return p;
}

int
_fourcc_compare(
		const fourcc_t *fcc,
		uint32_t v)
{
	if (fcc->fcc_code == v) {
		return 0;
	}
	return 1;
}
