/*!
 * @copyright
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT license.
 */
#include "0xc/sys/exit.h"

#include "0xc/std/api.h"
#include "0xc/std/array.h"
#include "0xc/sys/errno.h"

#include <errno.h>
#include <stdlib.h>

// MARK: Types
typedef int (*sysexit_t)(
	errno_t error
);

// MARK: Module Private
static int
_sysexit_c11(
		errno_t error)
{
	switch (error) {
	case 0:
		return EX_OK;
	case EDOM:
		return EX_DATAERR;
	case ERANGE:
		return EX_DATAERR;
	case EILSEQ:
		return EX_DATAERR;
	}
	return -1;
}

static int
_sysexit_posix(
		errno_t error)
{
	switch (error) {
	case EPERM:
		return EX_NOPERM;
	case ENOENT:
		return EX_NOINPUT;
	case ESRCH:
		return EX_UNAVAILABLE;
	case EINTR:
		return EX_TEMPFAIL;
	case EIO:
		return EX_IOERR;
	case ENXIO:
		return EX_UNAVAILABLE;
	case E2BIG:
		return EX_DATAERR;
	case ENOEXEC:
		return EX_DATAERR;
	case EBADF:
		return EX_SOFTWARE;
	case ECHILD:
		return EX_OSERR;
	case EAGAIN:
		return EX_TEMPFAIL;
	case ENOMEM:
		return EX_OSERR;
	case EACCES:
		return EX_NOPERM;
	case EFAULT:
		return EX_SOFTWARE;
	case EBUSY:
		return EX_TEMPFAIL;
	case EEXIST:
		return EX_CANTCREAT;
	case EXDEV:
		return EX_OSERR;
	case ENODEV:
		return EX_UNAVAILABLE;
	case ENOTDIR:
		return EX_NOINPUT;
	case EISDIR:
		return EX_NOINPUT;
	case EINVAL:
		return EX_DATAERR;
	case ENFILE:
		return EX_OSERR;
	case EMFILE:
		return EX_OSERR;
	case ENOTTY:
		return EX_UNAVAILABLE;
	case ETXTBSY:
		return EX_TEMPFAIL;
	case EFBIG:
		return EX_IOERR;
	case ENOSPC:
		return EX_CANTCREAT;
	case ESPIPE:
		return EX_IOERR;
	case EROFS:
		return EX_CANTCREAT;
	case EMLINK:
		return EX_CANTCREAT;
	case EPIPE:
		return EX_IOERR;
	case EDEADLK:
		return EX_TEMPFAIL;
	case ENAMETOOLONG:
		return EX_DATAERR;
	case ENOLCK:
		return EX_OSERR;
	case ENOSYS:
		return EX_UNAVAILABLE;
	case ENOTEMPTY:
		return EX_DATAERR;
	case ELOOP:
		return EX_DATAERR;
	case ENOMSG:
		return EX_PROTOCOL;
	case EIDRM:
		return EX_UNAVAILABLE;
	case EPROTO:
		return EX_PROTOCOL;
#if defined(EMULTIHOP)
	// pdclibc is missing this.
	case EMULTIHOP:
		return EX_PROTOCOL;
#endif
	case EBADMSG:
		return EX_PROTOCOL;
	case EOVERFLOW:
		return EX_DATAERR;
	case ECANCELED:
		return EX_TEMPFAIL;
	case EOWNERDEAD:
		return EX_OSERR;
	case ENOTRECOVERABLE:
		return EX_OSERR;
	case EINPROGRESS:
		return EX_TEMPFAIL;
	case EALREADY:
		return EX_TEMPFAIL;
	case ENOTSOCK:
		return EX_SOFTWARE;
	case EDESTADDRREQ:
		return EX_DATAERR;
	case EMSGSIZE:
		return EX_DATAERR;
	case EPROTOTYPE:
		return EX_PROTOCOL;
	case ENOPROTOOPT:
		return EX_PROTOCOL;
	case EPROTONOSUPPORT:
		return EX_PROTOCOL;
	case EOPNOTSUPP:
		return EX_UNAVAILABLE;
	case EAFNOSUPPORT:
		return EX_UNAVAILABLE;
	case EADDRINUSE:
		return EX_UNAVAILABLE;
	case EADDRNOTAVAIL:
		return EX_UNAVAILABLE;
	case ENETDOWN:
		return EX_UNAVAILABLE;
	case ENETUNREACH:
		return EX_NOHOST;
	case ENETRESET:
		return EX_TEMPFAIL;
	case ECONNABORTED:
		return EX_TEMPFAIL;
	case ECONNRESET:
		return EX_TEMPFAIL;
	case ENOBUFS:
		return EX_OSERR;
	case EISCONN:
		return EX_SOFTWARE;
	case ENOTCONN:
		return EX_SOFTWARE;
	case ETIMEDOUT:
		return EX_TEMPFAIL;
	case ECONNREFUSED:
		return EX_UNAVAILABLE;
	case EHOSTUNREACH:
		return EX_NOHOST;
	default:
		return -1;
	}
}

static int
_sysexit_posix_np(
		errno_t error)
{
	switch (error) {
#if defined(ENOTBLK)
	case ENOTBLK:
		return EX_NOINPUT;
#endif
#if defined(ENOSTR)
	case ENOSTR:
		return EX_UNAVAILABLE;
#endif
#if defined(ENODATA)
	case ENODATA:
		return EX_NOINPUT;
#endif
#if defined(ETIME)
	case ETIME:
		return EX_TEMPFAIL;
#endif
#if defined(ENOSR)
	case ENOSR:
		return EX_OSERR;
#endif
#if defined(ENOLINK)
	case ENOLINK:
		return EX_UNAVAILABLE;
#endif
#if defined(EWOULDBLOCK)
	case EWOULDBLOCK:
		return EX_TEMPFAIL;
#endif
#if defined(ESOCKTNOSUPPORT)
	case ESOCKTNOSUPPORT:
		return EX_UNAVAILABLE;
#endif
#if defined(EPFNOSUPPORT)
	case EPFNOSUPPORT:
		return EX_UNAVAILABLE;
#endif
#if defined(ESHUTDOWN)
	case ESHUTDOWN:
		return EX_UNAVAILABLE;
#endif
#if defined(ETOOMANYREFS)
	case ETOOMANYREFS:
		return EX_OSERR;
#endif
#if defined(EHOSTDOWN)
	case EHOSTDOWN:
		return EX_NOHOST;
#endif
#if defined(EDQUOT)
	case EDQUOT:
		return EX_CANTCREAT;
#endif
#if defined(ESTALE)
	case ESTALE:
		return EX_UNAVAILABLE;
#endif
#if defined(EREMOTE)
	case EREMOTE:
		return EX_UNAVAILABLE;
#endif
	default:
		return -1;
	}
}

#if defined(__FreeBSD__)
static int
_sysexit_freebsd(
		errno_t error)
{
	switch (error) {
	case EAUTH:
		return EX_NOPERM;
	case ENEEDAUTH:
		return EX_NOPERM;
	case EPROCLIM:
		return EX_OSERR;
	case EBADRPC:
		return EX_PROTOCOL;
	case ERPCMISMATCH:
		return EX_PROTOCOL;
	case EPROGUNAVAIL:
		return EX_UNAVAILABLE;
	case EPROGMISMATCH:
		return EX_PROTOCOL;
	case EPROCUNAVAIL:
		return EX_UNAVAILABLE;
	case EFTYPE:
		return EX_DATAERR;
	case EDOOFUS:
		return EX_SOFTWARE;
	case ENOTCAPABLE:
		return EX_NOPERM;
	case ECAPMODE:
		return EX_NOPERM;
	case EINTEGRITY:
		return EX_DATAERR;
	default:
		return -1;
	}
}
#endif // __FreeBSD__

#if defined(__APPLE__)
static int
_sysexit_darwin(
		errno_t error)
{
	switch (error) {
	case EAUTH:
		return EX_NOPERM;
	case ENEEDAUTH:
		return EX_NOPERM;
	case EPROCLIM:
		return EX_OSERR;
	case EBADRPC:
		return EX_PROTOCOL;
	case ERPCMISMATCH:
		return EX_PROTOCOL;
	case EPROGUNAVAIL:
		return EX_UNAVAILABLE;
	case EPROGMISMATCH:
		return EX_PROTOCOL;
	case EPROCUNAVAIL:
		return EX_UNAVAILABLE;
	case EFTYPE:
		return EX_DATAERR;
	case EPWROFF:
		return EX_UNAVAILABLE;
	case EDEVERR:
		return EX_IOERR;
	case EBADEXEC:
		return EX_DATAERR;
	case EBADARCH:
		return EX_DATAERR;
	case ESHLIBVERS:
		return EX_DATAERR;
	case EBADMACHO:
		return EX_DATAERR;
	case ENOPOLICY:
		return EX_NOPERM;
	case EQFULL:
		return EX_TEMPFAIL;
	default:
		return -1;
	}
}
#endif // __APPLE__

#if defined(__linux__)
static int
_sysexit_linux(
		errno_t error)
{
	switch (error) {
	case ECHRNG:
		return EX_DATAERR;
	case EL2NSYNC:
		return EX_OSERR;
	case EL3HLT:
		return EX_OSERR;
	case EL3RST:
		return EX_OSERR;
	case ELNRNG:
		return EX_DATAERR;
	case EUNATCH:
		return EX_UNAVAILABLE;
	case ENOCSI:
		return EX_UNAVAILABLE;
	case EL2HLT:
		return EX_OSERR;
	case EBADE:
		return EX_DATAERR;
	case EBADR:
		return EX_DATAERR;
	case EXFULL:
		return EX_OSERR;
	case ENOANO:
		return EX_UNAVAILABLE;
	case EBADRQC:
		return EX_DATAERR;
	case EBADSLT:
		return EX_DATAERR;
	case EBFONT:
		return EX_OSFILE;
	case ENONET:
		return EX_NOHOST;
	case ENOPKG:
		return EX_UNAVAILABLE;
	case EADV:
		return EX_PROTOCOL;
	case ESRMNT:
		return EX_OSERR;
	case ECOMM:
		return EX_PROTOCOL;
	case EDOTDOT:
		return EX_PROTOCOL;
	case ENOTUNIQ:
		return EX_DATAERR;
	case EBADFD:
		return EX_SOFTWARE;
	case EREMCHG:
		return EX_PROTOCOL;
	case ELIBACC:
		return EX_NOINPUT;
	case ELIBBAD:
		return EX_DATAERR;
	case ELIBSCN:
		return EX_DATAERR;
	case ELIBMAX:
		return EX_OSERR;
	case ELIBEXEC:
		return EX_DATAERR;
	case ERESTART:
		return EX_TEMPFAIL;
	case ESTRPIPE:
		return EX_IOERR;
	case EUCLEAN:
		return EX_OSFILE;
	case ENOTNAM:
		return EX_DATAERR;
	case ENAVAIL:
		return EX_UNAVAILABLE;
	case EISNAM:
		return EX_DATAERR;
	case EREMOTEIO:
		return EX_IOERR;
	case ENOMEDIUM:
		return EX_NOINPUT;
	case EMEDIUMTYPE:
		return EX_DATAERR;
	case ENOKEY:
		return EX_NOPERM;
	case EKEYEXPIRED:
		return EX_NOPERM;
	case EKEYREVOKED:
		return EX_NOPERM;
	case EKEYREJECTED:
		return EX_NOPERM;
	case ERFKILL:
		return EX_UNAVAILABLE;
	case EHWPOISON:
		return EX_IOERR;
	default:
		return -1;
	}
}
#endif // __linux__

// MARK: Public
int
syserror(
		errno_t error)
{
	static const sysexit_t exits[] = {
		&_sysexit_c11,
		&_sysexit_posix,
		&_sysexit_posix_np,
#if defined(__FreeBSD__)
		&_sysexit_freebsd,
#endif
#if defined(__APPLE__)
		&_sysexit_darwin,
#endif
#if defined(__linux__)
		&_sysexit_linux,
#endif
	};
	int result = -1;
	size_t i = 0;

	for (i = 0; i < countof(exits); i++) {
		sysexit_t f = exits[i];

		result = f(error);
		if (result >= 0) {
			return result;
		}
	}

	return EX_UNAVAILABLE;
}

void
sysexit(
		errno_t error)
{
	exit(syserror(error));
}

void
sysexit_check(
		errno_t error)
{
	if (error < 0 || error >= ELAST) {
		panicx("invalid errno value: %d", error);
	}

	if (!error) {
		return;
	}

	sysexit(error);
}
