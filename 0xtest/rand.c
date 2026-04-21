/*!
 * @copyright
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT license.
 *
 * @file
 * Provides arc4random for FunOS.
 */
#include "0xc/std/api.h"
#include "0xc/std/string.h"

#include <stdint.h>
#include <stdlib.h>
#include <time.h>

// MARK: IWYU
DECL_FUNC_4IWYU(arc4random);

// MARK: External Declarations
#ifndef __OPEN_SOURCE__
extern long
_funos_libc_random(
	void);
#endif

// MARK: Public
void
sdrand(
		void)
{
#ifndef __OPEN_SOURCE__
#if TARGET_0XTEST
	time_t t = 0;
	unsigned int t_seed = 0;

	t = time(NULL);
	t_seed = __cast_signed_unsigned(unsigned int, t);

	srand(t_seed);
#endif
#endif
}

#ifndef __OPEN_SOURCE__
#if defined(TARGET_0XFUN) && TARGET_0XFUN && !PLATFORM_POSIX
uint32_t
arc4random(
		void)
{
	integer_value_t v;

	v.iv_long.v = _funos_libc_random();
	return v.iv_int32.vu;
}
#endif
#endif

char *__cstr
rands(
		char *__counted_by(s_len) s,
		__unused size_t s_len,
		size_t n)
{
	const char map[] =
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz"
		"0123456789";
	const size_t map_cnt = strslen(map);
	size_t i = 0;

	for (i = 0; i < n; i++) {
		s[i] = map[arc4random() % map_cnt];
	}

	s[n] = 0;
	return __unsafe_null_terminated_from_indexable(s);
}
