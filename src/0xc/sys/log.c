/*!
 * @copyright
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT license.
 */
#include "log.h"

#include "0xc/std/api.h"
#include "0xc/std/array.h"
#include "0xc/std/cursor.h"
#include "0xc/std/defs.h"
#include "0xc/std/pointer.h"
#include "0xc/std/string.h"
#include "0xc/sys/errno.h"

#include <ctype.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

// MARK: IWYU
DECL_FUNC_4IWYU(strlcpy);
DECL_FUNC_4IWYU(strlcat);

// MARK: Module Private
static char *__cstr
_strchr_zterm(
		const char *__cstr s,
		int c)
{
	// Older clangs will complain about strchr returning a pointer type which is
	// incompatible with char *__null_terminated, even though strchr in the
	// Darwin Libc is properly annotated to return a null-terminated char
	// pointer. I suspect that this was just a bug that was fixed or something.
	// In any case, we can work around it with an unsafe forge without breaking
	// newer clangs.
	return __unsafe_forge(char *, strchr(s, c), zterm);
}

static const char *__cstr
_dump_field_get_type_cstr(
		const char *__cstr fmt,
		va_list ap)
{
	const char *pcnt = __unsafe_terminated_by_to_indexable(fmt);
	const char *__cstr pcnt_cstr = NULL;
	const char *__cstr type_cstr = NULL;
	const char *spec = NULL;
	size_t s_count = 0;

	while ((pcnt_cstr = _strchr_zterm(fmt, '%'))) {
		__unused size_t pcnt_len = 0;
		char ch = 0;
		const char *__unsafe_indexable type_unsafe = NULL;
		const char *__cstr type = NULL;

		// Skip literal %%.
		pcnt_len = strlen(pcnt_cstr) + 1;
		pcnt = __unsafe_forge(const char *, pcnt_cstr, bidi, pcnt_len);
		if (pcnt[1] == '%') {
			pcnt += 2;
			continue;
		}

		spec = &pcnt[1];
		while ((ch = spec[0])) {
			if (isalpha(ch)) {
				break;
			}

			spec += 1;
		}

		switch (ch) {
		case 0:
			return NULL;
		case 's':
			s_count++;
			__fallthrough();
		default:
			type_unsafe = va_arg(ap, const char *);
			type = __unsafe_forge(const char *, type_unsafe, zterm);
			break;
		}

		// The first '%s' is the indentation level, and the second is the type
		// name of the field.
		if (s_count == 2) {
			type_cstr = type;
			break;
		}

		pcnt = &spec[1];
	}

	return type_cstr;
}

static const char *__cstr
_dump_field_munge_fmt(
		__unused dump_field_context_t *ctxt,
		const char *__cstr type_cstr,
		const char *__cstr fmt,
		char fmt_new[static 64])
{
	size_t i = 0;
	char *__unsafe_indexable pcnt_unsafe = NULL;
	char *pcnt = NULL;
	char *spec = NULL;
	const char *__cstr hacks[] = {
		"uint8_t *",
		"unsigned char *",
	};
	char *__cstr fmt_new_cstr = __unsafe_forge(char *, fmt_new, zterm);

	// The type name is passed as the second %s argument by
	// __builtin_dump_struct.
	for (i = 0; i < countof(hacks); i++) {
		if (strstr(type_cstr, hacks[i])) {
			break;
		}
	}

	if (i == countof(hacks)) {
		return fmt;
	}

	strlcpy(fmt_new, fmt, 64);

	// Replace the last %...s specifier with %p. The last % in the format string
	// corresponds to the field value. Skip flags, field width, precision, and
	// length modifiers to find the conversion character. Preserve any trailing
	// characters (e.g. newline or closing quote).
	pcnt_unsafe = strrchr(fmt_new_cstr, '%');
	if (pcnt_unsafe) {
		char *__cstr pcnt_zterm = NULL;
		char ch = 0;
		char tail[64] = "";

		// Skip flags.
		pcnt_zterm = __unsafe_forge(char *, pcnt_unsafe, zterm);
		pcnt = __null_terminated_to_indexable(pcnt_zterm);
		spec = &pcnt[1];
		while ((ch = spec[0])) {
			switch (ch) {
			case '-':
			case '+':
			case ' ':
			case '#':
			case '0':
				spec += 1;
				continue;
			default:
				break;
			}

			break;
		}

		// Skip field width.
		while ((ch = spec[0])) {
			if (isdigit(ch) || ch == '*') {
				spec += 1;
				continue;
			}

			break;
		}

		// Skip precision.
		if (spec[0] == '.') {
			spec += 1;

			while ((ch = spec[0])) {
				if (isdigit(ch) || ch == '*') {
					spec += 1;
					continue;
				}

				break;
			}
		}

		// Skip length modifiers.
		while ((ch = spec[0])) {
			switch (ch) {
			case 'h':
			case 'l':
			case 'z':
			case 'j':
			case 't':
			case 'L':
			case 'q':
				spec += 1;
				continue;
			default:
				break;
			}

			break;
		}

		ch = spec[0];
		switch (ch) {
		case 's':
			// Replace the conversion specifier and strip flags, field width,
			// precision, and length modifiers -- none of which are meaningful
			// for %p. Copy the tail (from the specifier onward) into a separate
			// buffer so that strlcat does not receive overlapping
			// restrict-qualified pointers.
			strscpy(tail, spec);
			tail[0] = 'p';
			pcnt[1] = 0;
			strlcat(fmt_new, tail, 64);
			break;
		default:
			// Not a string specifier, so we don't know how to munge it.
			break;
		}
	}

	return __unsafe_null_terminated_from_indexable(fmt_new);
}

// MARK: Implementation Details
int
__dump_field(
		dump_field_context_t *ctxt,
        const char *__cstr fmt,
		...)
{
	int ret = -1;
	errno_t error = -1;
	va_list ap;
	va_list ap_copy;
	const char *__cstr type_cstr = NULL;
	const char *__cstr fmt_new = fmt;
	char fmt_new_mem[64] = "";

	va_start(ap, fmt);
	va_copy(ap_copy, ap);
	type_cstr = _dump_field_get_type_cstr(fmt, ap_copy);
	if (type_cstr) {
		fmt_new = _dump_field_munge_fmt(ctxt, type_cstr, fmt, fmt_new_mem);
	}
	va_end(ap_copy);

	// Restart ap from the beginning for the actual formatting.
	va_end(ap);

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
	va_start(ap, fmt);
	error = vcprintf(ctxt->dmp_cursor, fmt_new, ap);
	if (!error) {
		ret = 0;
	}
	va_end(ap);
#pragma GCC diagnostic pop

	return ret;
}
