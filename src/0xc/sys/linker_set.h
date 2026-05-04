/*!
 * @copyright
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT license.
 *
 * @file
 * Linker sets support for both ELF and Mach-O linkers.
 */
#ifndef __ZX_SYS_LINKER_SET_H
#define __ZX_SYS_LINKER_SET_H

#include <0xc/std/api.h>
#include <0xc/std/int.h>

__API_HEADER_BEGIN(c, nonnull, single)

// MARK: Definitions
#define _LINKER_SET_SECTION_NAME(SET) CONCAT(_LINKER_SET_PREFIX, SET)
#define _LINKER_SET_SECTION_NAME_CSTR(SET) \
	ZXSTRINGIFY(CONCAT(_LINKER_SET_PREFIX, SET))

#if defined(__ELF__)
/*!
 * @section
 * The ELF linker emits section anchors of the form __start_SECTION_NAME and
 * __stop_SECTION_NAME. These can be referred to via their symbol names, and the
 * addresses of the symbols will be the start and stop of the section,
 * respectively.
 */
#define _LINKER_SET_PREFIX set_
#define _LINKER_SET_PREFIX_CSTR "set_"
#define _LINKER_SET_SECTION_CSTR(SET) _LINKER_SET_SECTION_NAME_CSTR(SET)
#define _LINKER_SET_START_ATTR(SET)
#define _LINKER_SET_END_ATTR(SET)
#elif defined(__MACH__)
/*!
 * @section
 * The Mach-O linker emits section anchors of the form
 * section$start$SEGMENT-NAME$SECTION-NAME and
 * section$end$SEGMENT-NAME$SECTION-NAME. Unlike ELF, the segment matters, and
 * since the symbol name is not a legal C symbol name, we have to indirect
 * through __asm to get to it. We assign these to variables that are named like
 * the ELF variables just for consistency.
 *
 * Note that sections in Mach-O are conventionally prefixed with '__'.
 */
#define _LINKER_SET_PREFIX __set_
#define _LINKER_SET_SECTION_CSTR(SET) \
	"__DATA," _LINKER_SET_SECTION_NAME_CSTR(SET)
#define _LINKER_SET_MACH_SYMBOL(SET, WHICH) \
	"section$" #WHICH "$__DATA$" ZXSTRINGIFY(_LINKER_SET_SECTION_NAME(SET))
#define _LINKER_SET_ASM(SET, WHICH) \
	__asm(_LINKER_SET_MACH_SYMBOL(SET, WHICH))
#define _LINKER_SET_START_ATTR(SET) _LINKER_SET_ASM(SET, start)
#define _LINKER_SET_END_ATTR(SET) _LINKER_SET_ASM(SET, end)
#else
#error "unsupported ABI"
#endif

/*!
 * @section
 * clang supports applying the no_sanitize attribute to variables, but GCC does
 * not. For GCC, we rely on inline helper functions that are annotated (which is
 * allowed) to perform the pointer accesses.
 */
#if defined(__clang__)
#define _LINKER_SET_ATTR_NOSAN __attribute__((no_sanitize("address")))
#else
#define _LINKER_SET_ATTR_NOSAN
#endif

/*!
 * @section
 * When building with bounds safety enabled, linker sets are declared as zero-
 * length arrays to ensure that there are bounds attached to the array, which
 * allows them to be accessed so that unsafe pointers can be created from them.
 * Otherwise, the array is an incomplete type, which indicates to gcc on mips
 * that the array cannot be in the small data section, and so GP-relative
 * addressing is not used.
 */
#if __has_ptrcheck
#define _LINKER_SET_ARRAY_CNT 0
#else
#define _LINKER_SET_ARRAY_CNT
#endif

__attribute__((no_sanitize("address")))
static inline int
__linker_set_in_bounds(
		const void *__unsafe_indexable cursor,
		const void *__unsafe_indexable end)
{
	return cursor < end;
}

__attribute__((no_sanitize("address")))
static inline unsigned long
__linker_set_count(
		const void *__unsafe_indexable start,
		const void *__unsafe_indexable end)
{
	long diff = (const char *)end - (const char *)start;
	return __cast_signed_unsigned(unsigned long, diff) / sizeof(void *);
}

#define _LINKER_SET_START(SET) CONCAT(__start_, _LINKER_SET_SECTION_NAME(SET))
#define _LINKER_SET_END(SET) CONCAT(__stop_, _LINKER_SET_SECTION_NAME(SET))
#define _LINKER_SET_ATTR(SET) \
	__attribute__((used)) \
	__attribute__((section(_LINKER_SET_SECTION_CSTR(SET)))) \
	_LINKER_SET_ATTR_NOSAN
#define _LINKER_SET_MEMBER_NAME(SET, _sym) \
	CONCAT(_LINKER_SET_PREFIX, SET ## _member_ ## _sym)

// MARK: Public
/*!
 * @function LINKER_SET_DECL
 * Declares a linker set.
 *
 * @param T
 * The type of node in the set.
 *
 * @param SET
 * The name of the set.
 *
 * @discussion
 * The linker set boundaries are declared as arrays to prevent MIPS from using
 * GP-relative addressing.
 */
#define LINKER_SET_DECL(T, SET) \
	extern const T *_Nonnull __single \
			_LINKER_SET_START(SET)[_LINKER_SET_ARRAY_CNT] \
			_LINKER_SET_START_ATTR(SET); \
	extern const T *_Nonnull __single \
			_LINKER_SET_END(SET)[_LINKER_SET_ARRAY_CNT] \
			_LINKER_SET_END_ATTR(SET)

/*!
 * @function LINKER_SET_ENTRY
 * Adds a symbol to a linker set.
 *
 * @param SET
 * The name of the linker set.
 *
 * @param SYM
 * The symbol to add.
 */
#define LINKER_SET_ENTRY(SET, SYM) \
	_LINKER_SET_ATTR(SET) \
	static const typeof(&SYM) __single \
			_LINKER_SET_MEMBER_NAME(SET, SYM) = &(SYM)

/*!
 * @function LINKER_SET_FOREACH
 * Iterates over the contents of a linker set.
 *
 * @param ITER
 * A pointer to the current object in the iteration.
 *
 * @param SET
 * The name of the linker set.
 *
 * @param CURSOR
 * A cursor variable which points to the current iterator. This variable is not
 * consumed directly by the caller.
 *
 * @example
 * A linker set declared like so:
 *
 *     LINKER_SET_DECL(const struct foo, foos);
 *
 * can be traversed like so:
 *
 *     const struct foo *__single fi = NULL;
 *     const struct foo *__single *__unsafe_indexable fii = NULL;
 *
 *     LINKER_SET_FOREACH(fi, foos, fii) {
 *         fprintf(stdout, "%s\n", fi->f_name);
 *     }
 */
#define LINKER_SET_FOREACH(ITER, SET, CURSOR) \
	for ((CURSOR) = &_LINKER_SET_START(SET)[0]; \
			(__linker_set_in_bounds((CURSOR), &_LINKER_SET_END(SET)[0]) && \
				((ITER) = (CURSOR)[0])); \
			(CURSOR)++)

/*!
 * @function LINKER_SET_COUNT
 * Returns the number of entries in a linker set.
 *
 * @param SET
 * The name of the linker set.
 *
 * @result
 * The number of entries in the set.
 */
#define LINKER_SET_COUNT(SET) \
	__linker_set_count(_LINKER_SET_START(SET), _LINKER_SET_END(SET))

__API_HEADER_END(c, nonnull, single)

#endif // __ZX_SYS_LINKER_SET_H
