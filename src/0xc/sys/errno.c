/*!
 * @copyright
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT license.
 */
#include "0xc/sys/errno.h"

#include "0xc/std/array.h"
#include "0xc/std/defs.h"

#include <errno.h>
#include <stdlib.h>

// MARK: Types
typedef const char *__cstr (*symerror_t)(
	errno_t error
);

// MARK: Module Private
static const char *__cstr
_symerror_c11(
		errno_t error)
{
	switch (error) {
	case 0:
		return "EOK";
	case EDOM:
		return "EDOM";
	case ERANGE:
		return "ERANGE";
	case EILSEQ:
		return "EILSEQ";
	}
	return NULL;
}

static const char *__cstr
_symerror_posix(
		errno_t error)
{
	switch (error) {
	case EPERM:
		return "EPERM";
	case ENOENT:
		return "ENOENT";
	case ESRCH:
		return "ESRCH";
	case EINTR:
		return "EINTR";
	case EIO:
		return "EIO";
	case ENXIO:
		return "ENXIO";
	case E2BIG:
		return "E2BIG";
	case ENOEXEC:
		return "ENOEXEC";
	case EBADF:
		return "EBADF";
	case ECHILD:
		return "ECHILD";
	case EAGAIN:
		return "EAGAIN";
	case ENOMEM:
		return "ENOMEM";
	case EACCES:
		return "EACCES";
	case EFAULT:
		return "EFAULT";
	case EBUSY:
		return "EBUSY";
	case EEXIST:
		return "EEXIST";
	case EXDEV:
		return "EXDEV";
	case ENODEV:
		return "ENODEV";
	case ENOTDIR:
		return "ENOTDIR";
	case EISDIR:
		return "EISDIR";
	case EINVAL:
		return "EINVAL";
	case ENFILE:
		return "ENFILE";
	case EMFILE:
		return "EMFILE";
	case ENOTTY:
		return "ENOTTY";
	case ETXTBSY:
		return "ETXTBSY";
	case EFBIG:
		return "EFBIG";
	case ENOSPC:
		return "ENOSPC";
	case ESPIPE:
		return "ESPIPE";
	case EROFS:
		return "EROFS";
	case EMLINK:
		return "EMLINK";
	case EPIPE:
		return "EPIPE";
	case EDEADLK:
		return "EDEADLK";
	case ENAMETOOLONG:
		return "ENAMETOOLONG";
	case ENOLCK:
		return "ENOLCK";
	case ENOSYS:
		return "ENOSYS";
	case ENOTEMPTY:
		return "ENOTEMPTY";
	case ELOOP:
		return "ELOOP";
	case ENOMSG:
		return "ENOMSG";
	case EIDRM:
		return "EIDRM";
	case EPROTO:
		return "EPROTO";
#if defined(EMULTIHOP)
	// pdclibc is missing this.
	case EMULTIHOP:
		return "EMULTIHOP";
#endif
	case EBADMSG:
		return "EBADMSG";
	case EOVERFLOW:
		return "EOVERFLOW";
	case ECANCELED:
		return "ECANCELED";
	case EOWNERDEAD:
		return "EOWNERDEAD";
	case ENOTRECOVERABLE:
		return "ENOTRECOVERABLE";
	case EINPROGRESS:
		return "EINPROGRESS";
	case EALREADY:
		return "EALREADY";
	case ENOTSOCK:
		return "ENOTSOCK";
	case EDESTADDRREQ:
		return "EDESTADDRREQ";
	case EMSGSIZE:
		return "EMSGSIZE";
	case EPROTOTYPE:
		return "EPROTOTYPE";
	case ENOPROTOOPT:
		return "ENOPROTOOPT";
	case EPROTONOSUPPORT:
		return "EPROTONOSUPPORT";
	case EOPNOTSUPP:
		return "EOPNOTSUPP";
	case EAFNOSUPPORT:
		return "EAFNOSUPPORT";
	case EADDRINUSE:
		return "EADDRINUSE";
	case EADDRNOTAVAIL:
		return "EADDRNOTAVAIL";
	case ENETDOWN:
		return "ENETDOWN";
	case ENETUNREACH:
		return "ENETUNREACH";
	case ENETRESET:
		return "ENETRESET";
	case ECONNABORTED:
		return "ECONNABORTED";
	case ECONNRESET:
		return "ECONNRESET";
	case ENOBUFS:
		return "ENOBUFS";
	case EISCONN:
		return "EISCONN";
	case ENOTCONN:
		return "ENOTCONN";
	case ETIMEDOUT:
		return "ETIMEDOUT";
	case ECONNREFUSED:
		return "ECONNREFUSED";
	case EHOSTUNREACH:
		return "EHOSTUNREACH";
	default:
		return NULL;
	}
}

static const char *__cstr
_symerror_posix_np(
		errno_t error)
{
	switch (error) {
#if defined(ENOTBLK)
	case ENOTBLK:
		return "ENOTBLK";
#endif
#if defined(ENOSTR)
	case ENOSTR:
		return "ENOSTR";
#endif
#if defined(ENODATA)
	case ENODATA:
		return "ENODATA";
#endif
#if defined(ETIME)
	case ETIME:
		return "ETIME";
#endif
#if defined(ENOSR)
	case ENOSR:
		return "ENOSR";
#endif
#if defined(ENOLINK)
	case ENOLINK:
		return "ENOLINK";
#endif
#if defined(EWOULDBLOCK)
	case EWOULDBLOCK:
		return "EWOULDBLOCK";
#endif
#if defined(ESOCKTNOSUPPORT)
	case ESOCKTNOSUPPORT:
		return "ESOCKTNOSUPPORT";
#endif
#if defined(EPFNOSUPPORT)
	case EPFNOSUPPORT:
		return "EPFNOSUPPORT";
#endif
#if defined(ESHUTDOWN)
	case ESHUTDOWN:
		return "ESHUTDOWN";
#endif
#if defined(ETOOMANYREFS)
	case ETOOMANYREFS:
		return "ETOOMANYREFS";
#endif
#if defined(EHOSTDOWN)
	case EHOSTDOWN:
		return "EHOSTDOWN";
#endif
#if defined(EDQUOT)
	case EDQUOT:
		return "EDQUOT";
#endif
#if defined(ESTALE)
	case ESTALE:
		return "ESTALE";
#endif
#if defined(EREMOTE)
	case EREMOTE:
		return "EREMOTE";
#endif
	default:
		return NULL;
	}
}

#if defined(__FreeBSD__)
static const char *__cstr
_symerror_freebsd(
		errno_t error)
{
	switch (error) {
	case EAUTH:
		return "EAUTH";
	case ENEEDAUTH:
		return "ENEEDAUTH";
	case EPROCLIM:
		return "EPROCLIM";
	case EBADRPC:
		return "EBADRPC";
	case ERPCMISMATCH:
		return "ERPCMISMATCH";
	case EPROGUNAVAIL:
		return "EPROGUNAVAIL";
	case EPROGMISMATCH:
		return "EPROGMISMATCH";
	case EPROCUNAVAIL:
		return "EPROCUNAVAIL";
	case EFTYPE:
		return "EFTYPE";
	case EDOOFUS:
		return "EDOOFUS";
	case ENOTCAPABLE:
		return "ENOTCAPABLE";
	case ECAPMODE:
		return "ECAPMODE";
	case EINTEGRITY:
		return "EINTEGRITY";
	default:
		return NULL;
	}
}
#endif // __FreeBSD__

#if defined(__APPLE__)
static const char *__cstr
_symerror_darwin(
		errno_t error)
{
	switch (error) {
	case EAUTH:
		return "EAUTH";
	case ENEEDAUTH:
		return "ENEEDAUTH";
	case EPROCLIM:
		return "EPROCLIM";
	case EBADRPC:
		return "EBADRPC";
	case ERPCMISMATCH:
		return "ERPCMISMATCH";
	case EPROGUNAVAIL:
		return "EPROGUNAVAIL";
	case EPROGMISMATCH:
		return "EPROGMISMATCH";
	case EPROCUNAVAIL:
		return "EPROCUNAVAIL";
	case EFTYPE:
		return "EFTYPE";
	case EPWROFF:
		return "EPWROFF";
	case EDEVERR:
		return "EDEVERR";
	case EBADEXEC:
		return "EBADEXEC";
	case EBADARCH:
		return "EBADARCH";
	case ESHLIBVERS:
		return "ESHLIBVERS";
	case EBADMACHO:
		return "EBADMACHO";
	case ENOPOLICY:
		return "ENOPOLICY";
	case EQFULL:
		return "EQFULL";
	default:
		return NULL;
	}
}
#endif // __APPLE__

#if defined(__linux__)
static const char *__cstr
_symerror_linux(
		errno_t error)
{
	switch (error) {
	case ECHRNG:
		return "ECHRNG";
	case EL2NSYNC:
		return "EL2NSYNC";
	case EL3HLT:
		return "EL3HLT";
	case EL3RST:
		return "EL3RST";
	case ELNRNG:
		return "ELNRNG";
	case EUNATCH:
		return "EUNATCH";
	case ENOCSI:
		return "ENOCSI";
	case EL2HLT:
		return "EL2HLT";
	case EBADE:
		return "EBADE";
	case EBADR:
		return "EBADR";
	case EXFULL:
		return "EXFULL";
	case ENOANO:
		return "ENOANO";
	case EBADRQC:
		return "EBADRQC";
	case EBADSLT:
		return "EBADSLT";
	case EBFONT:
		return "EBFONT";
	case ENONET:
		return "ENONET";
	case ENOPKG:
		return "ENOPKG";
	case EADV:
		return "EADV";
	case ESRMNT:
		return "ESRMNT";
	case ECOMM:
		return "ECOMM";
	case EDOTDOT:
		return "EDOTDOT";
	case ENOTUNIQ:
		return "ENOTUNIQ";
	case EBADFD:
		return "EBADFD";
	case EREMCHG:
		return "EREMCHG";
	case ELIBACC:
		return "ELIBACC";
	case ELIBBAD:
		return "ELIBBAD";
	case ELIBSCN:
		return "ELIBSCN";
	case ELIBMAX:
		return "ELIBMAX";
	case ELIBEXEC:
		return "ELIBEXEC";
	case ERESTART:
		return "ERESTART";
	case ESTRPIPE:
		return "ESTRPIPE";
	case EUCLEAN:
		return "EUCLEAN";
	case ENOTNAM:
		return "ENOTNAM";
	case ENAVAIL:
		return "ENAVAIL";
	case EISNAM:
		return "EISNAM";
	case EREMOTEIO:
		return "EREMOTEIO";
	case ENOMEDIUM:
		return "ENOMEDIUM";
	case EMEDIUMTYPE:
		return "EMEDIUMTYPE";
	case ENOKEY:
		return "ENOKEY";
	case EKEYEXPIRED:
		return "EKEYEXPIRED";
	case EKEYREVOKED:
		return "EKEYREVOKED";
	case EKEYREJECTED:
		return "EKEYREJECTED";
	case ERFKILL:
		return "ERFKILL";
	case EHWPOISON:
		return "EHWPOISON";
	default:
		return NULL;
	}
}
#endif // __linux__

// MARK: Public
const char *__cstr
symerror(
		errno_t error)
{
	static const symerror_t syms[] = {
		&_symerror_c11,
		&_symerror_posix,
		&_symerror_posix_np,
#if defined(__FreeBSD__)
		&_symerror_freebsd,
#endif
#if defined(__APPLE__)
		&_symerror_darwin,
#endif
#if defined(__linux__)
		&_symerror_linux,
#endif
	};
	const char *__cstr result = NULL;
	size_t i = 0;

	for (i = 0; i < countof(syms); i++) {
		symerror_t f = syms[i];

		result = f(error);
		if (result) {
			return result;
		}
	}

	return "E??";
}
