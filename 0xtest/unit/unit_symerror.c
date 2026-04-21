/*!
 * @copyright
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT license.
 */
#include "0xc/sys/check.h"
#include "0xc/sys/unit.h"
#include "0xc/sys/errno.h"

#include "0xc/std/defs.h"

#include <errno.h>

static void
UNIT_TEST_FUNC(symerror_c11)(
		__unused const unit_t *un)
{
	checkcstr("C11 EDOM", symerror(EDOM), "EDOM");
	checkcstr("C11 ERANGE", symerror(ERANGE), "ERANGE");
	checkcstr("C11 EILSEQ", symerror(EILSEQ), "EILSEQ");
}

static void
UNIT_TEST_FUNC(symerror_posix)(
		__unused const unit_t *un)
{
	checkcstr("POSIX EPERM", symerror(EPERM), "EPERM");
	checkcstr("POSIX ENOENT", symerror(ENOENT), "ENOENT");
	checkcstr("POSIX EINVAL", symerror(EINVAL), "EINVAL");
	checkcstr("POSIX ECONNREFUSED", symerror(ECONNREFUSED), "ECONNREFUSED");
}

static void
UNIT_TEST_FUNC(symerror_posix_np)(
		__unused const unit_t *un)
{
#if defined(ENOTBLK)
	checkcstr("POSIX NP ENOTBLK", symerror(ENOTBLK), "ENOTBLK");
#endif
#if defined(ENOSTR)
	checkcstr("POSIX NP ENOSTR", symerror(ENOSTR), "ENOSTR");
#endif
#if defined(ETIME)
	checkcstr("POSIX NP ETIME", symerror(ETIME), "ETIME");
#endif
#if defined(EDQUOT)
	checkcstr("POSIX NP EDQUOT", symerror(EDQUOT), "EDQUOT");
#endif
#if defined(ESTALE)
	checkcstr("POSIX NP ESTALE", symerror(ESTALE), "ESTALE");
#endif
}

#if defined(__APPLE__)
static void
UNIT_TEST_FUNC(symerror_darwin)(
		__unused const unit_t *un)
{
	checkcstr("Darwin EFTYPE", symerror(EFTYPE), "EFTYPE");
	checkcstr("Darwin EBADMACHO", symerror(EBADMACHO), "EBADMACHO");
	checkcstr("Darwin ENOPOLICY", symerror(ENOPOLICY), "ENOPOLICY");
}
#endif

#if defined(__FreeBSD__)
static void
UNIT_TEST_FUNC(symerror_freebsd)(
		__unused const unit_t *un)
{
	checkcstr("FreeBSD EDOOFUS", symerror(EDOOFUS), "EDOOFUS");
	checkcstr("FreeBSD ENOTCAPABLE", symerror(ENOTCAPABLE), "ENOTCAPABLE");
	checkcstr("FreeBSD EINTEGRITY", symerror(EINTEGRITY), "EINTEGRITY");
}
#endif

#if defined(__linux__)
static void
UNIT_TEST_FUNC(symerror_linux)(
		__unused const unit_t *un)
{
	checkcstr("Linux ECHRNG", symerror(ECHRNG), "ECHRNG");
	checkcstr("Linux ENOKEY", symerror(ENOKEY), "ENOKEY");
	checkcstr("Linux ERFKILL", symerror(ERFKILL), "ERFKILL");
}
#endif

static void
UNIT_TEST_FUNC(symerror_unknown)(
		__unused const unit_t *un)
{
	checkcstr("Unknown error", symerror(9999), "E??");
}

UNIT_TEST(symerror_c11);
UNIT_TEST(symerror_posix);
UNIT_TEST(symerror_posix_np);
UNIT_TEST(symerror_unknown);

#if defined(__APPLE__)
UNIT_TEST(symerror_darwin);
#endif

#if defined(__FreeBSD__)
UNIT_TEST(symerror_freebsd);
#endif

#if defined(__linux__)
UNIT_TEST(symerror_linux);
#endif
