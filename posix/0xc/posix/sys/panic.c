/*!
 * @copyright
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT license.
 */
#include "0xc/sys/panic.h"
#include "0xc/std/defs.h"
#include "0xc/std/io.h"
#include "0xc/std/string.h"
#include "0xc/sys/errno.h"

#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>

// MARK: Public
void
panic(
		int error,
		const char *__cstr fmt,
		...)
{
	char m[1024] = "";
	va_list ap;

	va_start(ap, fmt);
	vssprintf(m, fmt, ap);
	va_end(ap);

	fprintf(stderr, "panic: %s: %d\n", cstr(m), error);
	__builtin_trap();
}

void
panicno(
		errno_t error,
		const char *__cstr fmt,
		...)
{
	char m[1024] = "";
	va_list ap;

	va_start(ap, fmt);
	vssprintf(m, fmt, ap);
	va_end(ap);

	fprintf(stderr, "panic: %s: %s\n", cstr(m), symerror(error));
	__builtin_trap();
}

void
panic0x(
		uint64_t error,
		const char *__cstr fmt,
		...)
{
	char m[1024] = "";
	va_list ap;

	va_start(ap, fmt);
	vssprintf(m, fmt, ap);
	va_end(ap);

	fprintf(stderr, "panic: %s: %#llx\n", cstr(m), (unsigned long long)error);
	__builtin_trap();
}

void
panicx(
		const char *__cstr fmt, ...)
{
	char m[1024] = "";
	va_list ap;

	va_start(ap, fmt);
	vssprintf(m, fmt, ap);
	va_end(ap);

	fprintf(stderr, "panic: %s\n", cstr(m));
	__builtin_trap();
}
