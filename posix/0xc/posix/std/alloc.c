/*!
 * @copyright
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT license.
 */
#include "0xc/std/alloc.h"
#include "0xc/std/api.h"

#include <stdlib.h>
#include <time.h>

// MARK: IWYU
DECL_FUNC_4IWYU(nanosleep);

// MARK: Internal
void
__waiting_for_memory(
		__unused size_t s)
{
	const struct timespec ts = {
		.tv_sec = 0,
		.tv_nsec = 500 * 1000,
	};
	nanosleep(&ts, NULL);
}
