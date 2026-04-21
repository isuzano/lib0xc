/*!
 * @copyright
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT license.
 *
 * @header
 * Logging macro support for objects. These macros differ from more traditional
 * logging systems in several ways:
 *
 *     1. All logs are associated with an object, and therefore objects which
 *        log must implement a protocol to distinguish their messages
 *     2. The decision whether to log a given line is made at build-time, based
 *        on the ZX_LOG_LEVEL macro
 *     3. There are only three types of log messages: error, info, and debug.
 *        There is a separate "debug2" type for specialized cases, and it is not
 *        fundamentally different from the "debug" level. These levels are
 *        represented as log streams.
 *
 *        STREAM   ZX_LOG_LEVEL   DESCRIPTION
 *        error    0              Error messages which record failures that
 *                                should always be presented in the log. A
 *                                successful cycle of a project's state machine
 *                                should result in zero error messages
 *        info     1              Informational messages that are primarily
 *                                useful to administrators, QA engineers, and
 *                                humans not generally familiar with the
 *                                codebase
 *        debug    2              Debugging messages which are primarily useful
 *                                to engineers working on the project
 *        debug2   3              Same as debug, but used for messages that are
 *                                on hot paths and may disrupt critical
 *                                performance characteristics of the project.
 *                                These messages should only be enabled on
 *                                special builds
 */
#ifndef __ZX_SYS_LOG_H
#define __ZX_SYS_LOG_H

#include <0xc/std/api.h>
#include <0xc/std/int.h>
#include <0xc/std/struct.h>
#include <0xc/sys/errno.h>

#include <stddef.h>
#include <stdarg.h>

// IWYU pragma: begin_exports
#include <0xc/std/context.h>
#include <0xc/std/cursor.h>
#include <0xc/sys/fourcc.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
// IWYU pragma: end_exports

__API_HEADER_BEGIN(c, nonnull, single)

// MARK: Forward Types
typedef struct _log_stream log_stream_t;

// MARK: Globals
/*!
 * @const LOG_STREAM_ERR
 * A stream of error messages that indicates a failure in the program's
 * operations.
 */
__export
const log_stream_t _log_stream_err;
#define LOG_STREAM_ERR __CONSTANT(log_stream, err)

/*!
 * @const LOG_STREAM_INFO
 * A stream of informative messages intended to give a human "flow chart"-level
 * insight into the program's actions. This is useful to humans who are not
 * familiar with the implementation of the project, but are familiar with the
 * basic design and information flow.
 */
__export
const log_stream_t _log_stream_info;
#define LOG_STREAM_INFO __CONSTANT(log_stream, info)

/*!
 * @const LOG_STREAM_DEBUG
 * A stream of debug messages intended to give a human insight into the
 * program's basic logic. This is useful to humans who are familiar with the
 * implementation of the project.
 */
__export
const log_stream_t _log_stream_debug;
#define LOG_STREAM_DEBUG __CONSTANT(log_stream, debug)

/*!
 * @const LOG_STREAM_DEBUG2
 * Same as @LOG_STREAM_DEBUG, but reserved for messages that may be on hot
 * paths, and so emitting them may cause critical performance characteristics of
 * the program to change substantially. This stream exists so that such messages
 * can be turned on in select builds of the program, but otherwise are not
 * present in the normal debug stream.
 */
__export
const log_stream_t _log_stream_debug2;
#define LOG_STREAM_DEBUG2 __CONSTANT(log_stream, debug2)

// MARK: Supporting Types
/*!
 * @const LOG_LABEL_VERSION
 * The version of the @log_label_t structure supported by the implementation.
 */
#define LOG_LABEL_VERSION STRUCT_VERSION(0)

/*!
 * @struct log_label_t
 * A structure describing an object's log stream labels.
 *
 * @field lbl_version
 * The version of the structure. Initialize to @LOG_LABEL_VERSION.
 *
 * @field lbl_type
 * A four-character code identifying the object type.
 *
 * @field lbl_name
 * The name of the object that should be present in messages logged by it.
 *
 * @field lbl_storage
 * Optional storage for a dynamically-constructed label. If the callee wishes to
 * dynamically construct a label, it may point @lbl_name to this storage.
 */
typedef struct _log_label {
	struct_version_t lbl_version;
	fourcc_t lbl_type;
	const char *__cstr lbl_name;
	char lbl_storage[__cstr 64];
} log_label_t;

// MARK: Protocol
/*!
 * @typedef log_proto_get_label_t
 * A function which returns label information for an object.
 *
 * @param obj
 * A context object which refers to the object. The callee can resolve to the
 * actual object type via @__context_import.
 *
 * @param stream
 * The log stream which is requesting label information.
 *
 * @param label
 * The label object the callee is to populate.
 *
 * @result
 * The callee is expected to return the pointer to @label if a log label should
 * be included. Otherwise, the callee may return NULL.
 */
typedef const log_label_t *_Nullable (*log_proto_get_label_t)(
	context_t obj,
	const log_stream_t *stream,
	log_label_t *label
);

/*!
 * @const LOG_PROTO_VERSION
 * The version of the @log_proto_t structure supported by the implementation.
 */
#define LOG_PROTO_VERSION STRUCT_VERSION(0)

/*!
 * @struct log_proto_t
 * The log protocol structure. Conforming objects are expected to include a
 * pointer to their protocol structure in a field named 'log_proto'.
 *
 * @field log_version
 * The version of the log protocol implemented in the structure.
 *
 * @field log_get_label
 * The function to retrieve label information.
 *
 * @example
 *
 *     static const log_proto_t __crypto_log = {
 *         .log_version = LOG_PROTO_VERSION,
 *         .log_get_label = &_crypto_cryptor_get_label,
 *     }
 *
 *     struct crypto_cryptor {
 *         const log_proto_t *log_proto;
 *         ...
 *     };
 *
 *     struct crypto_cryptor cryp = {
 *         .log_proto = &__crypto_log,
 *         ...
 *     };
 */
typedef struct _log_proto {
	struct_version_t log_version;
	log_proto_get_label_t log_get_label;
} log_proto_t;

// MARK: Public
// MARK: - Error
/*!
 * @function zxerr
 * Logs an error message with an associated result code.
 *
 * @param OBJ
 * The object which is the subject of the message.
 *
 * @param CODE
 * The result code associated with the message.
 *
 * @param FMT
 * The printf(3)-like format string for the message.
 *
 * @param ...
 * The arguments corresponding to @FMT.
 */
#define zxerr(OBJ, CODE, FMT, ...) \
	zxlog((OBJ), (CODE), LOG_STREAM_ERR, FMT, ## __VA_ARGS__)

/*!
 * @function zxerr0x
 * Variant of @zxerr that logs the code as a hexadecimal number.
 *
 * @param OBJ
 * The object which is the subject of the message.
 *
 * @param CODE
 * The result code associated with the message.
 *
 * @param FMT
 * The printf(3)-like format string for the message.
 *
 * @param ...
 * The arguments corresponding to @FMT.
 */
#define zxerr0x(OBJ, CODE, FMT, ...) \
	zxlog0x((OBJ), (CODE), LOG_STREAM_ERR, FMT, ## __VA_ARGS__)

/*!
 * @function zxerrno
 * Variant of @zxerr that logs the symbolic representation of the POSIX error
 * code.
 *
 * @param OBJ
 * The object which is the subject of the message.
 *
 * @param CODE
 * The result code associated with the message.
 *
 * @param FMT
 * The printf(3)-like format string for the message.
 *
 * @param ...
 * The arguments corresponding to @FMT.
 */
#define zxerrno(OBJ, CODE, FMT, ...) \
	zxlogno((OBJ), (CODE), LOG_STREAM_ERR, FMT, ## __VA_ARGS__)

/*!
 * @function zxerrx
 * Logs an error message.
 *
 * @param OBJ
 * The object which is the subject of the message.
 *
 * @param FMT
 * The printf(3)-like format string for the message.
 *
 * @param ...
 * The arguments corresponding to @FMT.
 */
#define zxerrx(OBJ, FMT, ...) \
	zxlogx((OBJ), LOG_STREAM_ERR, FMT, ## __VA_ARGS__)

/*!
 * @function zxerrthis
 * Variant of @zxerrx that logs the subject object's fields.
 *
 * @param OBJ
 * The object which is the subject of the message.
 *
 * @param N
 * The size of the buffer to use for the object's description. This must be an
 * integer constant expression.
 *
 * @param FMT
 * The printf(3)-like format string for the message.
 *
 * @param ...
 * The arguments corresponding to @FMT.
 *
 * @discussion
 * If the compiler supports __builtin_dump_struct, its output will be appended
 * to the log message. Otherwise, the address of @OBJ will be printed.
 */
#define zxerrthis(OBJ, N, FMT, ...) \
	zxlogthis((OBJ), N, LOG_STREAM_ERR, FMT, ## __VA_ARGS__)

// MARK: - Info
/*!
 * @function zxinfo
 * Logs an info message with an associated result code.
 *
 * @param OBJ
 * The object which is the subject of the message.
 *
 * @param CODE
 * The result code associated with the message.
 *
 * @param FMT
 * The printf(3)-like format string for the message.
 *
 * @param ...
 * The arguments corresponding to @FMT.
 */
#if ZX_LOG_LEVEL >= 1
#define zxinfo(OBJ, CODE, FMT, ...) \
	zxlog((OBJ), (CODE), LOG_STREAM_INFO, FMT, ## __VA_ARGS__)
#else
#define zxinfo(...)
#endif

/*!
 * @function zxinfo0x
 * Variant of @zxinfo that logs the result code as a hexadecimal number.
 *
 * @param OBJ
 * The object which is the subject of the message.
 *
 * @param CODE
 * The result code associated with the message.
 *
 * @param FMT
 * The printf(3)-like format string for the message.
 *
 * @param ...
 * The arguments corresponding to @FMT.
 */
#if ZX_LOG_LEVEL >= 1
#define zxinfo0x(OBJ, CODE, FMT, ...) \
	zxlog0x((OBJ), (CODE), LOG_STREAM_INFO, FMT, ## __VA_ARGS__)
#else
#define zxinfo0x(...)
#endif

/*!
 * @function zxinfono
 * Variant of @zxinfo that logs the symbolic representation of the POSIX error
 * code.
 *
 * @param OBJ
 * The object which is the subject of the message.
 *
 * @param CODE
 * The result code associated with the message.
 *
 * @param FMT
 * The printf(3)-like format string for the message.
 *
 * @param ...
 * The arguments corresponding to @FMT.
 */
#if ZX_LOG_LEVEL >= 1
#define zxinfono(OBJ, CODE, FMT, ...) \
	zxlogno((OBJ), (CODE), LOG_STREAM_INFO, FMT, ## __VA_ARGS__)
#else
#define zxinfono(...)
#endif

/*!
 * @function zxinfox
 * Logs an info message.
 *
 * @param OBJ
 * The object which is the subject of the message.
 *
 * @param FMT
 * The printf(3)-like format string for the message.
 *
 * @param ...
 * The arguments corresponding to @FMT.
 */
#if ZX_LOG_LEVEL >= 1
#define zxinfox(OBJ, FMT, ...) \
	zxlogx((OBJ), LOG_STREAM_INFO, FMT, ## __VA_ARGS__)
#else
#define zxinfox(...)
#endif

/*!
 * @function zxinfothis
 * Variant of @zxinfox that logs the subject object's fields.
 *
 * @param OBJ
 * The object which is the subject of the message.
 *
 * @param N
 * The size of the buffer to use for the object's description. This must be an
 * integer constant expression.
 *
 * @param FMT
 * The printf(3)-like format string for the message.
 *
 * @param ...
 * The arguments corresponding to @FMT.
 *
 * @discussion
 * If the compiler supports __builtin_dump_struct, its output will be appended
 * to the log message. Otherwise, the address of @OBJ will be printed.
 */
#if ZX_LOG_LEVEL >= 1
#define zxinfothis(OBJ, N, FMT, ...) \
	zxlogthis((OBJ), N, LOG_STREAM_INFO, FMT, ## __VA_ARGS__)
#else
#define zxinfothis(...)
#endif

// MARK: - Debug
/*!
 * @function zxdebug
 * Logs a debug message with an associated result code.
 *
 * @param OBJ
 * The object which is the subject of the message.
 *
 * @param CODE
 * The result code associated with the message.
 *
 * @param FMT
 * The printf(3)-like format string for the message.
 *
 * @param ...
 * The arguments corresponding to @FMT.
 */
#if ZX_LOG_LEVEL >= 2
#define zxdebug(OBJ, CODE, FMT, ...) \
	zxlog((OBJ), (CODE), LOG_STREAM_DEBUG, FMT, ## __VA_ARGS__)
#else
#define zxdebug(...)
#endif

/*!
 * @function zxdebug0x
 * Variant of @zxdebug that logs the code as a hexadecimal number.
 *
 * @param OBJ
 * The object which is the subject of the message.
 *
 * @param CODE
 * The result code associated with the message.
 *
 * @param FMT
 * The printf(3)-like format string for the message.
 *
 * @param ...
 * The arguments corresponding to @FMT.
 */
#if ZX_LOG_LEVEL >= 2
#define zxdebug0x(OBJ, CODE, FMT, ...) \
	zxlog0x((OBJ), (CODE), LOG_STREAM_DEBUG, FMT, ## __VA_ARGS__)
#else
#define zxdebug0x(...)
#endif

/*!
 * @function zxdebugno
 * Variant of @zxdebug that logs the symbolic representation of the POSIX error
 * code.
 *
 * @param OBJ
 * The object which is the subject of the message.
 *
 * @param CODE
 * The result code associated with the message.
 *
 * @param FMT
 * The printf(3)-like format string for the message.
 *
 * @param ...
 * The arguments corresponding to @FMT.
 */
#if ZX_LOG_LEVEL >= 2
#define zxdebugno(OBJ, CODE, FMT, ...) \
	zxlogno((OBJ), (CODE), LOG_STREAM_DEBUG, FMT, ## __VA_ARGS__)
#else
#define zxdebugno(...)
#endif

/*!
 * @function zxdebugx
 * Logs a debug message.
 *
 * @param OBJ
 * The object which is the subject of the message.
 *
 * @param FMT
 * The printf(3)-like format string for the message.
 *
 * @param ...
 * The arguments corresponding to @FMT.
 */
#if ZX_LOG_LEVEL >= 2
#define zxdebugx(OBJ, FMT, ...) \
	zxlogx((OBJ), LOG_STREAM_DEBUG, FMT, ## __VA_ARGS__)
#else
#define zxdebugx(...)
#endif

/*!
 * @function zxdebugthis
 * Variant of @zxdebugx that logs the subject object's fields.
 *
 * @param OBJ
 * The object which is the subject of the message.
 *
 * @param N
 * The size of the buffer to use for the object's description. This must be an
 * integer constant expression.
 *
 * @param FMT
 * The printf(3)-like format string for the message.
 *
 * @param ...
 * The arguments corresponding to @FMT.
 *
 * @discussion
 * If the compiler supports __builtin_dump_struct, its output will be appended
 * to the log message. Otherwise, the address of @OBJ will be printed.
 */
#if ZX_LOG_LEVEL >= 2
#define zxdebugthis(OBJ, N, FMT, ...) \
	zxlogthis((OBJ), N, LOG_STREAM_DEBUG, FMT, ## __VA_ARGS__)
#else
#define zxdebugthis(...)
#endif

// MARK: - Debug2
/*!
 * @function zxdebug2
 * Logs a hot path debug message with an associated result code.
 *
 * @param OBJ
 * The object which is the subject of the message.
 *
 * @param CODE
 * The result code associated with the message.
 *
 * @param FMT
 * The printf(3)-like format string for the message.
 *
 * @param ...
 * The arguments corresponding to @FMT.
 */
#if ZX_LOG_LEVEL >= 3
#define zxdebug2(OBJ, CODE, FMT, ...) \
	zxlog((OBJ), (CODE), LOG_STREAM_DEBUG2, FMT, ## __VA_ARGS__)
#else
#define zxdebug2(...)
#endif

/*!
 * @function zxdebug20x
 * Variant of @zxdebug2 that logs the result code as a hexadecimal number.
 *
 * @param OBJ
 * The object which is the subject of the message.
 *
 * @param CODE
 * The result code associated with the message.
 *
 * @param FMT
 * The printf(3)-like format string for the message.
 *
 * @param ...
 * The arguments corresponding to @FMT.
 */
#if ZX_LOG_LEVEL >= 3
#define zxdebug20x(OBJ, CODE, FMT, ...) \
	zxlog0x((OBJ), (CODE), LOG_STREAM_DEBUG2, FMT, ## __VA_ARGS__)
#else
#define zxdebug20x(...)
#endif

/*!
 * @function zxdebug2no
 * Variant of @zxdebug2 that logs the symbolic representation of the POSIX error
 * code.
 *
 * @param OBJ
 * The object which is the subject of the message.
 *
 * @param CODE
 * The result code associated with the message.
 *
 * @param FMT
 * The printf(3)-like format string for the message.
 *
 * @param ...
 * The arguments corresponding to @FMT.
 */
#if ZX_LOG_LEVEL >= 3
#define zxdebug2no(OBJ, CODE, FMT, ...) \
	zxlogno((OBJ), (CODE), LOG_STREAM_DEBUG2, FMT, ## __VA_ARGS__)
#else
#define zxdebug2no(...)
#endif

/*!
 * @function zxdebug2x
 * Logs a hot path debug message.
 *
 * @param OBJ
 * The object which is the subject of the message.
 *
 * @param FMT
 * The printf(3)-like format string for the message.
 *
 * @param ...
 * The arguments corresponding to @FMT.
 */
#if ZX_LOG_LEVEL >= 3
#define zxdebug2x(OBJ, FMT, ...) \
	zxlogx((OBJ), LOG_STREAM_DEBUG2, FMT, ## __VA_ARGS__)
#else
#define zxdebug2x(...)
#endif

/*!
 * @function zxdebug2this
 * Variant of @zxdebug2x that logs the subject object's fields.
 *
 * @param OBJ
 * The object which is the subject of the message.
 *
 * @param N
 * The size of the buffer to use for the object's description. This must be an
 * integer constant expression.
 *
 * @param FMT
 * The printf(3)-like format string for the message.
 *
 * @param ...
 * The arguments corresponding to @FMT.
 *
 * @discussion
 * If the compiler supports __builtin_dump_struct, its output will be appended
 * to the log message. Otherwise, the address of @OBJ will be printed.
 */
#if ZX_LOG_LEVEL >= 3
#define zxdebug2this(OBJ, N, FMT, ...) \
	zxlogthis((OBJ), (N), LOG_STREAM_DEBUG2, FMT, ## __VA_ARGS__)
#else
#define zxdebug2this(...)
#endif

// MARK: - Auto
/*!
 * @function zxauto
 * Logs a message as either error or debug, depending on whether the result code
 * given indicates success.
 *
 * @param OBJ
 * The object which is the subject of the message.
 *
 * @param CODE
 * The result code associated with the message. If this value is zero, the
 * message is logged as a debug message. Otherwise, it is logged as an error.
 *
 * @param FMT
 * The printf(3)-like format string for the message.
 *
 * @param ...
 * The arguments corresponding to @FMT.
 */
#define zxauto(OBJ, CODE, FMT, ...) \
	if ((CODE)) { \
		zxerr((OBJ), (CODE), FMT, ## __VA_ARGS__); \
	} else { \
		zxdebugx((OBJ), FMT ": success", ## __VA_ARGS__); \
	}

/*!
 * @function zxauto0x
 * Variant of @zxauto which logs the result code as a hexadecimal number.
 *
 * @param OBJ
 * The object which is the subject of the message.
 *
 * @param CODE
 * The result code associated with the message. If this value is zero, the
 * message is logged as a debug message. Otherwise, it is logged as an error.
 *
 * @param FMT
 * The printf(3)-like format string for the message.
 *
 * @param ...
 * The arguments corresponding to @FMT.
 */
#define zxauto0x(OBJ, CODE, FMT, ...) \
	if ((CODE)) { \
		zxerr0x((OBJ), (CODE), FMT, ## __VA_ARGS__); \
	} else { \
		zxdebugx((OBJ), FMT ": success", ## __VA_ARGS__); \
	}

/*!
 * @function zxautono
 * Variant of @zxauto which logs the symbolic representation of the POSIX error
 * code.
 *
 * @param OBJ
 * The object which is the subject of the message.
 *
 * @param CODE
 * The result code associated with the message. If this value is zero, the
 * message is logged as a debug message. Otherwise, it is logged as an error.
 *
 * @param FMT
 * The printf(3)-like format string for the message.
 *
 * @param ...
 * The arguments corresponding to @FMT.
 */
#define zxautono(OBJ, CODE, FMT, ...) \
	if ((CODE)) { \
		zxerrno((OBJ), (CODE), FMT, ## __VA_ARGS__); \
	} else { \
		zxdebugx((OBJ), FMT ": success", ## __VA_ARGS__); \
	}

// MARK: Internal
/*!
 * @internal
 * Implementation details. Note that @__zxlog is written to minimize stack usage
 * when optimizations are disabled, hence the lack of intermediary variables in
 * the implementation.
 */
__typed_enum(log_stream_scheme, uint8_t, closed,
	LOG_STREAM_SCHEME_LOCK,
	LOG_STREAM_SCHEME_LINE,
);

#define __zxlog(OBJ, STR, TRLFMT, TRLARG_WCOMMA, FMT, ...) do { \
	context_t ctxt_0xc = __context_export_anonymous(OBJ); \
	log_label_t *lbl_0xc = __new(log_label_t, \
		.lbl_version = LOG_LABEL_VERSION, \
	); \
	const char *__cstr type_0xc = NULL; \
\
	if ((OBJ)->log_proto->log_get_label(ctxt_0xc, STR, lbl_0xc)) { \
		type_0xc = fourcc_get_string(&lbl_0xc->lbl_type); \
	} \
\
	switch (log_stream_get_scheme(STR)) { \
	case LOG_STREAM_SCHEME_LOCK: \
		log_stream_lock(STR); \
		log_stream_write(STR, lbl_0xc, "%s", log_stream_get_prefix(STR)); \
\
		if (type_0xc) { \
			log_stream_write(STR, lbl_0xc, "%s<%s>: ", \
					lbl_0xc->lbl_name, type_0xc); \
		} \
		log_stream_write(STR, lbl_0xc, FMT, ## __VA_ARGS__); \
_Pragma("GCC diagnostic push") \
_Pragma("GCC diagnostic ignored \"-Wformat-zero-length\"") \
		log_stream_write(STR, lbl_0xc, TRLFMT TRLARG_WCOMMA); \
_Pragma("GCC diagnostic pop") \
		log_stream_write(STR, lbl_0xc, "%c", log_stream_get_terminator(STR)); \
		log_stream_unlock(STR); \
		break; \
	case LOG_STREAM_SCHEME_LINE: \
		if (type_0xc) { \
			log_stream_write(STR, lbl_0xc, "%s%s<%s>: " \
					FMT TRLFMT "%c", \
					log_stream_get_prefix(STR), lbl_0xc->lbl_name, \
					type_0xc, ## __VA_ARGS__ TRLARG_WCOMMA, \
					log_stream_get_terminator(STR)); \
		} else { \
			log_stream_write(STR, lbl_0xc, "%s" FMT TRLFMT "%c", \
					log_stream_get_prefix(STR), ## __VA_ARGS__ \
					TRLARG_WCOMMA, log_stream_get_terminator(STR)); \
		} \
		break; \
	default: \
		panic_switch(log_stream_get_scheme(STR)); \
	} \
} while (0)

#define zxlog(OBJ, CODE, STR, FMT, ...) \
	__zxlog(OBJ, STR, ": %d", COMMA() (CODE), FMT, ## __VA_ARGS__)

#define zxlog0x(OBJ, CODE, STR, FMT, ...) \
	__zxlog(OBJ, STR, ": %#x", COMMA() (CODE), FMT, ## __VA_ARGS__)

#define zxlogno(OBJ, CODE, STR, FMT, ...) \
	__zxlog(OBJ, STR, ": %s", COMMA() (symerror(CODE)), FMT, ## __VA_ARGS__)

#define zxlogx(OBJ, STR, FMT, ...) \
	__zxlog(OBJ, STR, "", , FMT, ## __VA_ARGS__)

typedef struct _dump_field_context {
	CURSOR *dmp_cursor;
} dump_field_context_t;

__export __printflike(2, 3)
int
__dump_field(
	dump_field_context_t *ctxt,
	const char *__cstr fmt,
	...);

// This is a bit ridiculous. __builtin_dump_struct is amazing, but it does
// possibly the dumbest thing when it encounters an unsigned character pointer
// by treating it as a C string and attempting to print it as such. So if you've
// just got raw bytes, you'll get garbage. If you've got ASan or bounds checking
// enabled, you'll potentially run off the end of the the buffer because the
// formatter is looking for a null terminator and not paying attention to the
// bounds of the field. While it cannot tell the difference between e.g. char *
// and int8_t *, it certainly does know the difference between signed and
// unsigned integers, so it should really just treat unsigned character pointers
// as opaque pointers and print their addresses, like it does for void *.
//
// So we have a custom formatter that intercepts the format string from
// __builtin_dump_struct and sniffs out the type of each field. If it sees an
// unsigned character pointer, it replaces the corresponding 's' conversion
// specifier in the format string with a 'p' specifier so that just the pointer
// gets printed for the field.
//
// This relies on the calling convention between __builtin_dump_struct and its
// formatter, which is not guaranteed, but in practice has been stable across
// compiler versions. And there are only so many ways it makes sense to print a
// C structure and its fields.
#define zxlogthis(OBJ, N, STR, FMT, ...) do { \
	char buff_0xc[N] = #OBJ; \
	CURSOR *__single cursor_0xc = cursor_new(); \
	__unused dump_field_context_t ctxt_0xc = { \
		.dmp_cursor = NULL, \
	}; \
\
	ctxt_0xc.dmp_cursor = cbuffopen(cursor_0xc, buff_0xc, "w"); \
	__dump_struct(OBJ, __dump_field, &ctxt_0xc); \
	__zxlog(OBJ, STR, ": %s", COMMA() (buff_0xc), FMT, ## __VA_ARGS__); \
	cclose(&cursor_0xc); \
} while (0)

__export __warn_result __nonnull1
log_stream_scheme_t
log_stream_get_scheme(
	const log_stream_t *st);

__export __warn_result __nonnull1
const char *
log_stream_get_prefix(
	const log_stream_t *st);

__export __warn_result __nonnull1
char
log_stream_get_terminator(
	const log_stream_t *st);

__export __nonnull1
void
log_stream_lock(
	const log_stream_t *st);

__export __nonnull1
void
log_stream_unlock(
	const log_stream_t *st);

__export __nonnull1 __nonnull3 __printflike(3, 4)
void
log_stream_write(
	const log_stream_t *st,
	const log_label_t *_Nullable label,
	const char *__cstr fmt,
	...);

__API_HEADER_END(c, nonnull, single)

#endif // __ZX_SYS_LOG_STREAM_H
