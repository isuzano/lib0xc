/*!
 * @copyright
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT license.
 */
#include "0xc/sys/check.h"
#include "0xc/sys/unit.h"

#include "0xc/sys/exit.h"

#include "0xc/std/defs.h"

#include <errno.h>

static void
UNIT_TEST_FUNC(sysexit_c11)(
		__unused const unit_t *un)
{
	checkint("C11 success", syserror(0), EX_OK);
	checkint("C11 EDOM", syserror(EDOM), EX_DATAERR);
	checkint("C11 ERANGE", syserror(ERANGE), EX_DATAERR);
	checkint("C11 EILSEQ", syserror(EILSEQ), EX_DATAERR);
}

static void
UNIT_TEST_FUNC(sysexit_posix)(
		__unused const unit_t *un)
{
	checkint("POSIX EPERM", syserror(EPERM), EX_NOPERM);
	checkint("POSIX ENOENT", syserror(ENOENT), EX_NOINPUT);
	checkint("POSIX EINVAL", syserror(EINVAL), EX_DATAERR);
	checkint("POSIX EIO", syserror(EIO), EX_IOERR);
	checkint("POSIX ENOMEM", syserror(ENOMEM), EX_OSERR);
	checkint("POSIX EACCES", syserror(EACCES), EX_NOPERM);
	checkint("POSIX EEXIST", syserror(EEXIST), EX_CANTCREAT);
	checkint("POSIX ENOSPC", syserror(ENOSPC), EX_CANTCREAT);
	checkint("POSIX EAGAIN", syserror(EAGAIN), EX_TEMPFAIL);
	checkint("POSIX ETIMEDOUT", syserror(ETIMEDOUT), EX_TEMPFAIL);
	checkint("POSIX ECONNREFUSED", syserror(ECONNREFUSED), EX_UNAVAILABLE);
	checkint("POSIX EHOSTUNREACH", syserror(EHOSTUNREACH), EX_NOHOST);
	checkint("POSIX EPROTO", syserror(EPROTO), EX_PROTOCOL);
}

static void
UNIT_TEST_FUNC(sysexit_posix_np)(
		__unused const unit_t *un)
{
#if defined(ENOTBLK)
	checkint("POSIX NP ENOTBLK", syserror(ENOTBLK), EX_NOINPUT);
#endif
#if defined(ETIME)
	checkint("POSIX NP ETIME", syserror(ETIME), EX_TEMPFAIL);
#endif
#if defined(EDQUOT)
	checkint("POSIX NP EDQUOT", syserror(EDQUOT), EX_CANTCREAT);
#endif
#if defined(ESTALE)
	checkint("POSIX NP ESTALE", syserror(ESTALE), EX_UNAVAILABLE);
#endif
#if defined(EHOSTDOWN)
	checkint("POSIX NP EHOSTDOWN", syserror(EHOSTDOWN), EX_NOHOST);
#endif
}

#if defined(__APPLE__)
static void
UNIT_TEST_FUNC(sysexit_darwin)(
		__unused const unit_t *un)
{
	checkint("Darwin EFTYPE", syserror(EFTYPE), EX_DATAERR);
	checkint("Darwin EBADMACHO", syserror(EBADMACHO), EX_DATAERR);
	checkint("Darwin ENOPOLICY", syserror(ENOPOLICY), EX_NOPERM);
	checkint("Darwin EDEVERR", syserror(EDEVERR), EX_IOERR);
	checkint("Darwin EQFULL", syserror(EQFULL), EX_TEMPFAIL);
}
#endif

#if defined(__FreeBSD__)
static void
UNIT_TEST_FUNC(sysexit_freebsd)(
		__unused const unit_t *un)
{
	checkint("FreeBSD EDOOFUS", syserror(EDOOFUS), EX_SOFTWARE);
	checkint("FreeBSD ENOTCAPABLE", syserror(ENOTCAPABLE), EX_NOPERM);
	checkint("FreeBSD EINTEGRITY", syserror(EINTEGRITY), EX_DATAERR);
	checkint("FreeBSD EAUTH", syserror(EAUTH), EX_NOPERM);
}
#endif

#if defined(__linux__)
static void
UNIT_TEST_FUNC(sysexit_linux)(
		__unused const unit_t *un)
{
	checkint("Linux ECHRNG", syserror(ECHRNG), EX_DATAERR);
	checkint("Linux ENOKEY", syserror(ENOKEY), EX_NOPERM);
	checkint("Linux ERFKILL", syserror(ERFKILL), EX_UNAVAILABLE);
	checkint("Linux ENOMEDIUM", syserror(ENOMEDIUM), EX_NOINPUT);
	checkint("Linux EHWPOISON", syserror(EHWPOISON), EX_IOERR);
}
#endif

static void
UNIT_TEST_FUNC(sysexit_unknown)(
		__unused const unit_t *un)
{
	checkint("Unknown error", syserror(9999), EX_UNAVAILABLE);
}

UNIT_TEST(sysexit_c11);
UNIT_TEST(sysexit_posix);
UNIT_TEST(sysexit_posix_np);
UNIT_TEST(sysexit_unknown);

#if defined(__APPLE__)
UNIT_TEST(sysexit_darwin);
#endif

#if defined(__FreeBSD__)
UNIT_TEST(sysexit_freebsd);
#endif

#if defined(__linux__)
UNIT_TEST(sysexit_linux);
#endif
