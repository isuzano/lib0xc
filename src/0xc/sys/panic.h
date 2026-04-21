/*!
 * @copyright
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT license.
 *
 * @file
 * Panic implementation.
 */
#ifndef __ZX_SYS_PANIC_H
#define __ZX_SYS_PANIC_H

#include <0xc/std/api.h>
#include <0xc/sys/errno.h>

// IWYU pragma: begin_exports
#include <0xc/std/type.h>
// IWYU pragma: end_exports

#include <stdint.h>

__API_HEADER_BEGIN(c, nonnull, single)

// MARK: Public
/*!
 * @function panic
 * Declaration of panic implementation provided by the environment which aborts
 * the program after recording a panic message with an integer error number.
 *
 * @param error
 * The error code to record in the panic string.
 *
 * @param fmt
 * The printf(3)-like format string for the message.
 *
 * @param ...
 * The arguments corresponding to @fmt.
 */
__export __no_tail_call __noreturn __nonnull2 __printflike(2, 3)
void
panic(
	int error,
	const char *__cstr fmt,
	...);

/*!
 * @function panicno
 * Declaration of panic implementation provided by the environment which aborts
 * the program after recording a panic message with a POSIX error number.
 *
 * @param error
 * The POSIX error number to record in the panic string.
 *
 * @param fmt
 * The printf(3)-like format string for the message.
 *
 * @param ...
 * The arguments corresponding to @fmt.
 */
__export __no_tail_call __noreturn __nonnull2 __printflike(2, 3)
void
panicno(
	errno_t error,
	const char *__cstr fmt,
	...);

/*!
 * @function panic0x
 * Declaration of panic0x implementation provided by the environment which
 * aborts the program after recording a panic message with a hexadecimal error
 * code.
 *
 * @param error
 * The hexadecimal error code to record in the panic string.
 *
 * @param fmt
 * The printf(3)-like format string for the message.
 *
 * @param ...
 * The arguments corresponding to @fmt.
 */
__export __no_tail_call __noreturn __nonnull2 __printflike(2, 3)
void
panic0x(
	uint64_t error,
	const char *__cstr fmt,
	...);

/*!
 * @function panicx
 * Declaration of panicx implementation provided by the environment which aborts
 * the program after recording a panic message with no associated error code.
 *
 * @param fmt
 * The printf(3)-like format string for the message.
 *
 * @param ...
 * The arguments corresponding to @fmt.
 */
__export __no_tail_call __noreturn __nonnull1 __printflike(1, 2)
void
panicx(
	const char *__cstr fmt,
	...);

/*!
 * @function panic_switch
 * Panic on an unhandled switch() case.
 *
 * @param V
 * The value which was not handled.
 */
#define panic_switch(V) ({ \
	if (__chk_type_unsigned(typeof(V))) { \
		panicx("unhandled switch value: %#llx", (unsigned long long)(V)); \
	} else { \
		panicx("unhandled switch value: %lld", (long long)(V)); \
	} \
})

/*!
 * @function panic_overflow
 * Panic on an integer overflow.
 *
 * @param WHAT
 * A description of the value, e.g. "array count".
 *
 * @param V
 * The value.
 *
 * @param MAX
 * The maximum expected value.
 */
#define panic_overflow(WHAT, V, MAX) do { \
	if (__chk_type_unsigned(typeof(V))) { \
		panicx("'%s' overflow: type = %s, actual = %#llx, expected <= %#llx", \
				#V, WHAT, \
				(unsigned long long)(V), \
				(unsigned long long)(MAX)); \
	} else { \
		panicx("'%s' overflow: type = %s, actual = %lld, expected <= %lld", \
				#V, WHAT, \
				(long long)(V), \
				(long long)(MAX)); \
	} \
} while (0)

/*!
 * @function panic_underflow
 * Panic on an integer underflow.
 *
 * @param WHAT
 * A description of the value, e.g. "array count".
 *
 * @param V
 * The value.
 *
 * @param MIN
 * The minimum expected value.
 */
#define panic_underflow(WHAT, V, MIN) do { \
	if (__chk_type_unsigned(typeof(V))) { \
		panicx("'%s' underflow: type = %s, actual = %#llx, expected >= %#llx", \
				#V, WHAT, \
				(unsigned long long)(V), \
				(unsigned long long)(MIN)); \
	} else { \
		panicx("'%s' underflow: type = %s, actual = %lld, expected >= %lld", \
				#V, WHAT, \
				(long long)(V), \
				(long long)(MIN)); \
	} \
} while (0)

/*!
 * @function panic_align
 * Panic on a misaligned value.
 *
 * @param WHAT
 * A description of the value, e.g. "pointer to header".
 *
 * @param V
 * The value.
 *
 * @param ALIGN
 * The expected alignment of the value.
 */
#define panic_align(WHAT, V, ALIGN) do { \
	if (!__compiler_const(ALIGN) && ((ALIGN) & ((ALIGN) - 1)) != 0) { \
		panicx("alignment must be a power of two: %llx", \
				(unsigned long long)(ALIGN)); \
	} \
	_Static_assert(!__compiler_const(ALIGN) || \
			(((ALIGN) & ((ALIGN) - 1)) == 0), \
			"alignment must be a power of two"); \
	panicx("'%s' misaligned: " \
			"type = %s, actual = %#llx, expected alignment = %#llx", \
			#V, WHAT, (unsigned long long)(V), (unsigned long long)(ALIGN)); \
} while (0)

/*!
 * @function assert_zero
 * Asserts that a given return code is zero.
 *
 * @param WHAT
 * A description of the assertion, e.g. "memory apertures are consistent".
 *
 * @param V
 * The value on which to assert.
 */
#define assert_zero(WHAT, V) do { \
	if ((V)) { \
		panic((V), "assertion failed: %s", (WHAT)); \
	} \
} while (0)

__API_HEADER_END(c, nonnull, single)

#endif // __ZX_SYS_PANIC_H
