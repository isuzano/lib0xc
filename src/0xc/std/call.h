/*!
 * @copyright
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT license.
 *
 * @header
 * Call objects capture a function's parameter list as a typed structure,
 * allowing for flexible function calling patterns. The functions associated
 * with call objects have a consistent type signature, which allows them to be
 * called from a common caller function, enumerated in lists, etc.
 *
 * Unlike closures in other programming languages (or the blocks extension for C
 * and Objective-C in clang), lib0xc call objects only capture state; they do
 * not permit nested function declarations. In other words, a call object allows
 * a caller to capture a function's parameters in a structure which can then be
 * passed to a function with a known parameter list.
 */
#ifndef __ZX_STD_CALL_H
#define __ZX_STD_CALL_H

#include <0xc/std/api.h>
#include <0xc/std/macros.h>
#include <0xc/std/struct.h>
#include <0xc/std/array.h>
#include <0xc/std/call_ext.h>
#include <0xc/std/pointer.h>
#include <0xc/sys/log.h>

#include <stdint.h>
#include <stdlib.h>

__API_HEADER_BEGIN(c, nonnull, single)

// MARK: Log Protocol
__export
const log_proto_t _log_proto_call;
#define LOG_PROTO_CALL __CONSTANT(log_proto, call)

// MARK: Supporting Types
/*!
 * @typedef call_function_t
 * The function signature for a callable object function.
 *
 * @param argv
 * A pointer to the call object's argument structure, which is emitted by the
 * @CALL_DECL macro. The structure type can be obtained with @call_t.
 *
 * @param argv_size
 * The size of @argv in bytes.
 *
 * @param retv
 * A pointer to the call object's return value structure. This is a structure
 * which the call function can write to in order to return values to the caller.
 *
 * @param retv_size
 * The size of @retv in bytes.
 */
typedef void (*call_function_t)(
	const void *__sized_by(argv_size) argv,
	size_t argv_size,
	void *_Nullable __sized_by(retv_size) retv,
	size_t retv_size
);

/*!
 * @typedef call_function_ext_t
 * A union of all callable function prototypes. Extended calls support up to
 * four additional scalar parameters (cf. 0xc/std/call_ext.h).
 *
 * @field cf_unsafe
 * The function pointer without a specific type.
 *
 * @field cf_func
 * The function pointer for a non-extended call.
 *
 * @field cf_ext1
 * The function pointer for an extended call with one additional parameter.
 *
 * @field cf_ext2
 * The function pointer for an extended call with two additional parameters.
 *
 * @field cf_ext3
 * The function pointer for an extended call with three additional parameters.
 *
 * @field cf_ext4
 * The function pointer for an extended call with four additional parameters.
 */
typedef union _call_function_ext {
	void *cf_unsafe;
	call_function_t cf_func;
	call_function_ext1_t cf_ext1;
	call_function_ext2_t cf_ext2;
	call_function_ext3_t cf_ext3;
	call_function_ext4_t cf_ext4;
} call_function_ext_t;

/*!
 * @typedef call_field_handler_t
 * The function signature for a call field handler which is invoked by @stdcall
 * when it encounters a field which has been marked for handling.
 *
 * @param sf
 * The descriptor for the field.
 *
 * @param sf_assoc
 * The descriptor for the associated field. If no field is associated, this will
 * be NULL.
 *
 * @param p
 * The structure object.
 *
 * @param p_len
 * The length of the memory referred to by @p.
 */
typedef void(*call_field_handler_t)(
	const struct_field_t *sf,
	const struct_field_t *_Nullable sf_assoc,
	void *__sized_by(p_len) p,
	size_t p_len
);

/*!
 * @struct call_field_type_t
 * A descriptor for typed fields specified in @CALL_DECL_FIELDS.
 *
 * @field cfh_name
 * The name of the type.
 *
 * @field cfh_handler
 * The handler function.
 */
typedef struct _call_field_type {
	const char *__cstr cfh_name;
	call_field_handler_t cfh_handler;
} call_field_type_t;

// MARK: Field Handlers
/*!
 * @const CALL_FIELD_TYPE_PRIVATE
 * A field which is cleared before being passed to the callee.
 */
__export
const call_field_type_t _call_field_type_private;
#define CALL_FIELD_HANDLER_PRIVATE __CONSTANT(call_field_type, private)

// MARK: Object
/*!
 * @struct call_t
 * A call object structure.
 *
 * @field c_version
 * The structure version.
 *
 * @field log_proto
 * The log protocol to use for logging from the call function.
 *
 * @field c_name
 * The name of the call.
 *
 * @field c_func_ext_cnt
 * The number of additional parameters specified by @CALL_DECL_EXT, or zero if
 * the call was declared with @CALL_DECL.
 *
 * @field c_func
 * The call function to invoke.
 */
#define CALL_VERSION STRUCT_VERSION(0)
typedef struct _call {
	struct_version_t c_version;
	const log_proto_t *log_proto;
	const char *__cstr c_name;
	size_t c_func_ext_cnt;
	call_function_ext_t c_func;
} call_t;

// MARK: Declarations
/*!
 * @macro CALL_DECL
 * Declares a function which can accept a call object as its parameter list.
 *
 * @param F
 * The name of the function.
 *
 * @param ...
 * The parameter list of the function, which will be captured as a call object.
 * Up to 16 parameters can be specified.
 *
 * @result
 * Emits the following declarations:
 *
 *     - A structure whose type can be obtained using @call_t(F) with fields
 * 	     corresponding to the parameter list
 *	   - A function prototype for the function name specified by @F with the
 *       call object parameter list specified by @...
 *	   - A call object declaration for the function which must be defined via
 *	     @CALL
 */
#define CALL_DECL(F, ...) CALL_DECL_EXT(F, CALL_EXT_NOPARAMS, __VA_ARGS__)

/*!
 * @macro CALL_DECL_RETURN
 * Declares the return value structure for a call function.
 *
 * @param F
 * The name of the call.
 *
 * @param ...
 * The return value list of the function. Up to 16 return values can be
 * specified.
 *
 * @result
 * Emits a structure with fields corresponding to the return value list. The
 * type of this structure can be obtained using @call_t(F) with the suffix
 * '_retv'.
 */
#define CALL_DECL_RETURN(F, ...) \
	typedef struct call_name(F, _retv) { \
		DELIMIT_SEMICOLON(__VA_ARGS__); \
	} call_name(F, _retv_t)

/*!
 * @macro CALL_DECL_FIELDS
 * Declares the call arguments which require special handling. Each field
 * declaration is a triple of (field, associated field, handler name).
 *
 * This macro must be specified regardless of whether the call has fields
 * requiring special handling. If a call has no such fields, this macro should
 * be invoked with an empty variadic argument list.
 *
 * @param F
 * The name of the call.
 *
 * @param ...
 * Triples of (field, associated field, type name) names. The total number of
 * variadic arguments must be a multiple of three. Up to 16 triples (48
 * arguments) can be specified.
 *
 * The type name refers to a @call_field_type_t constant provided by the
 * library, e.g. "private" refers to @CALL_FIELD_HANDLER_PRIVATE.
 *
 * Whether an associated field needs to be specified depends on the handler. For
 * example, the "private" handler does not require an associated field, so in
 * this case the second item in the triple can be filled with any placeholder
 * value, e.g. the field itself.
 *
 * @example
 *
 *     CALL_DECL(my_func,
 *         uint32_t flags,
 *         void *buf,
 *         size_t buf_cnt,
 *         char *name,
 *         uint32_t name_cnt
 *     );
 *
 *     CALL_DECL_FIELDS(my_func,
 *         buf, buf_cnt, private,
 *         name, name_cnt, private
 *     );
 */
#define CALL_DECL_FIELDS(F, ...) \
	_Static_assert( \
			VA_COUNT(__VA_ARGS__) / 3 <= countof_unsafe( \
				fldof(call_field_vector_t, cfv_desc)), \
				"too many field triples"); \
	__unused static const call_field_vector_t \
	call_name(F, _fields) = { \
		.cfv_desc = { \
			_CALL_FIELD_TRIPLES(F, __VA_ARGS__) \
		}, \
		.cfv_cnt = VA_COUNT(__VA_ARGS__) / 3, \
	}

/*!
 * @macro call_argv_t
 * Obtains the type of a call object for a function declared with
 * @CALL_DECL.
 *
 * @param F
 * The name of the call.
 *
 * @result
 * The type of the call object for @F, which is the structure type emitted by
 * @CALL_DECL.
 */
#define call_argv_t(F) call_name(F, _argv_t)

/*!
 * @macro call_argv_import
 * Imports a call object's argument structure from the pointer and length passed
 * as the argument vector to a @call_function_t function.
 *
 * @param F
 * The name of the call.
 *
 * @param P
 * A pointer to the argument vector.
 *
 * @param L
 * The length of the argument vector.
 *
 * @result
 * The type of the call function's return value structure, which is the
 * structure type obtained by @call_argv_t.
 */
#define call_argv_import(F, P, L) \
	__forge_type(const call_argv_t(F) *, P, L)

/*!
 * @macro call_retv_t
 * Obtains the type of a call function's return value structure for a function
 * declared with @CALL_DECL and @CALL_DECL_RETURN.
 *
 * @param F
 * The name of the call.
 *
 * @result
 * The type of the call function's return value structure, which is the
 * structure type emitted by @CALL_DECL_RETURN.
 */
#define call_retv_t(F) call_name(F, _retv_t)

/*!
 * @macro call_retv_new
 * Creates an instance of the return value structure for a call function.
 *
 * @param F
 * The name of the call.
 *
 * @param ...
 * The initial field values for the return value structure.
 *
 * @result
 * A pointer to an instance of the call function's return value structure, which
 * is the structure type emitted by @CALL_DECL_RETURN. The memory referred to by
 * this pointer will be freed when the calling scope exits.
 */
#define call_retv_new(F, ...) __new(call_retv_t(F), ## __VA_ARGS__)

/*!
 * @macro call_retv_import
 * Imports a call function's return value structure from the pointer and length
 * passed as the return value vector to a @call_function_t function.
 *
 * @param F
 * The name of the call.
 *
 * @param P
 * A pointer to the return value vector.
 *
 * @param L
 * The length of the return value vector.
 *
 * @result
 * The type of the call function's return value structure, which is the
 * structure type obtained by @call_retv_t.
 */
#define call_retv_import(F, P, L) \
	__forge_type(call_retv_t(F) *, P, L)

/*!
 * @macro zxcall
 * Calls a function declared with @CALL_DECL using the provided argument list.
 *
 * @param F
 * The name of the call function to call.
 *
 * @param RETV
 * A pointer to the call function's return value structure, or NULL if the call
 * function does not return any values. If non-NULL, the caller is expected to
 * have declared the return value structure using @CALL_DECL_RETURN and to pass
 * a pointer to an instance of this structure.
 *
 * @param ...
 * The argument list to pass to the call function.
 */
#define zxcall(F, RETV, ...) \
	zxcall_ext(F, RETV, NULL, CALL_EXT_NOPARAMS, __VA_ARGS__)

/*!
 * @internal
 * Implementation details.
 */
#define call_name(F, N) F ## _call ## N

typedef struct _call_field {
	struct_field_t cf_fld;
	struct_field_t cf_assocfld;
	const call_field_type_t *cf_type;
} call_field_t;

typedef struct _call_field_vector {
	call_field_t cfv_desc[16];
	size_t cfv_cnt;
} call_field_vector_t;

#define _CALL_FIELD_TRIPLES(F, ...) \
	CONCAT(_CALL_FIELD_, VA_COUNT(__VA_ARGS__))(F, __VA_ARGS__)

#define _CALL_FIELD_DESC(F, FLD, ASSOC, HANDLER) { \
	.cf_fld = STRUCT_FIELD_INIT(call_argv_t(F), FLD), \
	.cf_assocfld = STRUCT_FIELD_INIT(call_argv_t(F), ASSOC), \
	.cf_type = &_call_field_type_ ## HANDLER, \
},

#define _CALL_FIELD_0x00(F, ...)
#define _CALL_FIELD_0x03(F, FLD, ASSOC, HANDLER) \
	_CALL_FIELD_DESC(F, FLD, ASSOC, HANDLER)
#define _CALL_FIELD_0x06(F, FLD, ASSOC, HANDLER, ...) \
	_CALL_FIELD_DESC(F, FLD, ASSOC, HANDLER) \
	_CALL_FIELD_0x03(F, __VA_ARGS__)
#define _CALL_FIELD_0x09(F, FLD, ASSOC, HANDLER, ...) \
	_CALL_FIELD_DESC(F, FLD, ASSOC, HANDLER) \
	_CALL_FIELD_0x06(F, __VA_ARGS__)
#define _CALL_FIELD_0x0c(F, FLD, ASSOC, HANDLER, ...) \
	_CALL_FIELD_DESC(F, FLD, ASSOC, HANDLER) \
	_CALL_FIELD_0x09(F, __VA_ARGS__)
#define _CALL_FIELD_0x0f(F, FLD, ASSOC, HANDLER, ...) \
	_CALL_FIELD_DESC(F, FLD, ASSOC, HANDLER) \
	_CALL_FIELD_0x0c(F, __VA_ARGS__)
#define _CALL_FIELD_0x12(F, FLD, ASSOC, HANDLER, ...) \
	_CALL_FIELD_DESC(F, FLD, ASSOC, HANDLER) \
	_CALL_FIELD_0x0f(F, __VA_ARGS__)
#define _CALL_FIELD_0x15(F, FLD, ASSOC, HANDLER, ...) \
	_CALL_FIELD_DESC(F, FLD, ASSOC, HANDLER) \
	_CALL_FIELD_0x12(F, __VA_ARGS__)
#define _CALL_FIELD_0x18(F, FLD, ASSOC, HANDLER, ...) \
	_CALL_FIELD_DESC(F, FLD, ASSOC, HANDLER) \
	_CALL_FIELD_0x15(F, __VA_ARGS__)
#define _CALL_FIELD_0x1b(F, FLD, ASSOC, HANDLER, ...) \
	_CALL_FIELD_DESC(F, FLD, ASSOC, HANDLER) \
	_CALL_FIELD_0x18(F, __VA_ARGS__)
#define _CALL_FIELD_0x1e(F, FLD, ASSOC, HANDLER, ...) \
	_CALL_FIELD_DESC(F, FLD, ASSOC, HANDLER) \
	_CALL_FIELD_0x1b(F, __VA_ARGS__)
#define _CALL_FIELD_0x21(F, FLD, ASSOC, HANDLER, ...) \
	_CALL_FIELD_DESC(F, FLD, ASSOC, HANDLER) \
	_CALL_FIELD_0x1e(F, __VA_ARGS__)
#define _CALL_FIELD_0x24(F, FLD, ASSOC, HANDLER, ...) \
	_CALL_FIELD_DESC(F, FLD, ASSOC, HANDLER) \
	_CALL_FIELD_0x21(F, __VA_ARGS__)
#define _CALL_FIELD_0x27(F, FLD, ASSOC, HANDLER, ...) \
	_CALL_FIELD_DESC(F, FLD, ASSOC, HANDLER) \
	_CALL_FIELD_0x24(F, __VA_ARGS__)
#define _CALL_FIELD_0x2a(F, FLD, ASSOC, HANDLER, ...) \
	_CALL_FIELD_DESC(F, FLD, ASSOC, HANDLER) \
	_CALL_FIELD_0x27(F, __VA_ARGS__)
#define _CALL_FIELD_0x2d(F, FLD, ASSOC, HANDLER, ...) \
	_CALL_FIELD_DESC(F, FLD, ASSOC, HANDLER) \
	_CALL_FIELD_0x2a(F, __VA_ARGS__)
#define _CALL_FIELD_0x30(F, FLD, ASSOC, HANDLER, ...) \
	_CALL_FIELD_DESC(F, FLD, ASSOC, HANDLER) \
	_CALL_FIELD_0x2d(F, __VA_ARGS__)

__export __nonnull1 __nonnull2 __nonnull3
void
_0xcall(
	const call_t *call,
	const call_field_vector_t *fvec,
	const __typed_array(call_ext_params_t, _Nullable, extparams),
	void *__sized_by(argv_size) argv,
	size_t argv_size,
	void *_Nullable __sized_by(retv_size) retv,
	size_t retv_size);

__API_HEADER_END(c, nonnull, single)

#endif // __ZX_STD_CALL_H
