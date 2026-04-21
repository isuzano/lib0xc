/*!
 * @copyright
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT license.
 *
 * @header
 * Digest object.
 */
#ifndef __ZX_SYS_DIGEST_H
#define __ZX_SYS_DIGEST_H

#include <0xc/std/api.h>
#include <0xc/std/io.h>
#include <0xc/std/array.h>

#include <stddef.h>
#include <stdint.h>

__API_HEADER_BEGIN(c, nonnull, single)

// MARK: Object
typedef struct _digest {
	digest_bytes_t d_bytes;
	size_t d_len;
} digest_t;

// MARK: Public
/*!
 * @function digest_init
 * Initializes a digest object.
 *
 * @param d
 * The digest.
 *
 * @param bytes
 * The digest bytes.
 *
 * @param len
 * The length of the digest bytes.
 *
 * @result
 * A pointer to the digest object.
 */
__export __nonnull1 __nonnull2
const digest_t *
digest_init(
	digest_t *d,
	const uint8_t *__counted_by(len) bytes,
	size_t len);

/*!
 * @function digest_compare
 * Compares two digests.
 *
 * @param lhs
 * The digest on the left-hand side of the comparison.
 *
 * @param rhs
 * The digest on the right-hand side of the comparison.
 *
 * @result
 * If the two digests are equal, zero is returned. Otherwise, a non-zero value
 * is returned.
 */
__export __warn_result __nonnull1 __nonnull2
int
digest_compare(
	const digest_t *lhs,
	const digest_t *rhs);

/*!
 * @function digest_print
 * Prints the digest as a hexadecimal string.
 *
 * @param d
 * The digest.
 *
 * @param ds
 * The destination buffer.
 */
__export __nonnull1 __nonnull2
void
digest_print(
	const digest_t *d,
	__typed_array(digest_string_t, _Nonnull, ds));

__API_HEADER_END(c, nonnull, single)

#endif // __ZX_SYS_DIGEST_H
