/*!
 * @copyright
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT license.
 *
 * @header
 * Utilities for mapping error numbers to process exit codes. Inspired by
 * FreeBSD's sysexits(3).
 */
#ifndef __ZX_SYS_EXIT_H
#define __ZX_SYS_EXIT_H

#include <0xc/std/api.h>

// IWYU pragma: begin_exports
#include <0xc/sys/errno.h>

#if __has_include(<sysexits.h>)
#include <sysexits.h>
#else
#include <0xc/shim/sysexits.h>
#endif
// IWYU pragma: end_exports

__API_HEADER_BEGIN(c, nonnull, single);

// MARK: Public
/*!
 * @function syserror
 * Returns an appropriate exit code from sysexits(3) for a given error number.
 *
 * @param error
 * The error number.
 *
 * @result
 * The appropriate exit code for the given error number.
 */
__export __warn_result
int
syserror(
	errno_t error);

/*!
 * @function sysexit
 * Exits the process with an appropriate exit code from sysexits(3) for the
 * error number.
 *
 * @param error
 * The error number.
 *
 * @discussion
 * This function does not return.
 */
__export __noreturn
void
sysexit(
	errno_t error);

/*!
 * @function sysexit_check
 * Checks the error number and exits the process with an appropriate exit code
 * from sysexits(3) if the error number is non-zero.
 *
 * @param error
 * The error number.
 */
__export
void
sysexit_check(
	errno_t error);

__API_HEADER_END(c, nonnull, single);

#endif // __ZX_SYS_EXIT_H
