/*!
 * @copyright
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT license.
 *
 * @header
 * Shim to provide arc4random implementation for FunOS.
 */
#ifndef __ZX_TEST_STD_RAND_H
#define __ZX_TEST_STD_RAND_H

#include "0xc/std/defs.h"
#include "0xc/std/api.h"

#include <stdint.h>
#include <stdlib.h>

__API_HEADER_BEGIN(c, nonnull, single);

/*!
 * @function sdrand
 * Seeds the random number generator with a value derived from the current time.
 */
__export
void
sdrand(
	void);

#ifndef __OPEN_SOURCE__
/*!
 * @function arc4random
 * Returns a random 32-bit unsigned integer.
 *
 * @result
 * A random 32-bit unsigned integer.
 *
 * @discussion
 * This is a shim for the FunOS test target.
 */
__export __warn_result
uint32_t
arc4random(
	void);
#endif

/*!
 * @function rands
 * Fills the given buffer with random alphanumeric characters.
 *
 * @param s
 * The buffer to fill with random characters.
 *
 * @param s_len
 * The length of the buffer.
 *
 * @param n
 * The number of random characters to generate.
 *
 * @result
 * A pointer to the null-terminated string containing the random characters.
 */
__export __nonnull1
const char *__cstr
rands(
	char *__counted_by(s_len) s,
	__unused size_t s_len,
	size_t n);

__API_HEADER_END(c, nonnull, single);

#endif // __ZX_TEST_STD_RAND_H
