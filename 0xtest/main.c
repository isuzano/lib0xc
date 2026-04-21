/*!
 * @copyright
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT license.
 *
 * @file
 * Entry point for the POSIX host 0xtest executable.
 */
#include "0xc/std/defs.h"
#include "0xc/sys/unit.h"

int
main(
		__unused int argc,
		__unused const char *argv[__counted_by(argc)])
{
	const unit_t *__single un = NULL;
	const unit_t *__single *__unsafe_indexable unn = NULL;

	UNIT_FOREACH(un, unn) {
		unit_do(un);
	}

	return 0;
}
