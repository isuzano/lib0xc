/*!
 * @copyright
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT license.
 *
 * @header
 * Calls which allow for additional arguments to be prepended to the parameter
 * list as explicit function parameters. These are intended to facilitate
 * designing APIs with call objects in which the implementation may pass
 * contextual information to the callee without requiring the caller to capture
 * this information in the call. Up to four additional scalar parameters are
 * supported.
 *
 * The extra parameters are escrowed via uint64_t. This is safe for any scalar
 * value whose width is less than or equal to a general-purpose register
 * (integers, pointers, enums). It is NOT safe for:
 *
 *     - floating-point types (float/double pass in FP registers, a uint64_t
 *       cast is a value conversion, not a bitwise reinterpretation, and the
 *       callee will look in the wrong register file)
 *     - structures or aggregates (may not fit in a register and have different
 *       calling convention rules)
 *
 * The callee function is invoked through a function pointer whose parameter
 * types are all uint64_t. This is an ABI-level type pun which is safe when all
 * extra parameters are GP-register-class scalars on architectures where the
 * register assignment depends only on position and register class (e.g. ARM64,
 * MIPS64).
 */
#ifndef __ZX_STD_CALL_EXT_H
#define __ZX_STD_CALL_EXT_H

#include <0xc/std/api.h>
#include <0xc/std/macros.h>
#include <0xc/std/struct.h>
#include <0xc/std/array.h>
#include <0xc/std/pointer.h>
#include <0xc/sys/log.h>

#include <stdint.h>
#include <stdlib.h>

__API_HEADER_BEGIN(c, nonnull, single)

// MARK: External Types
typedef struct _call call_t;
typedef struct _call_field_vector call_field_vector_t;

// MARK: Supporting Types
/*!
 * @typedef call_ext_params_t
 * An array of additional parameters to be passed to a call function declared
 * with @CALL_DECL_EXT and invoked with @stdcall_ext. Up to 4 additional scalar
 * parameters are supported.
 */
typedef uint64_t call_ext_params_t[4];

/*!
 * @typedef call_trap_t
 * The function signature for a call trap handler which is invoked by
 * @zxcall_ext. A call trap handler can be used to implement custom handling for
 * calls, e.g. to implement custom synchronization.
 *
 * @param call
 * The call object for the call being handled.
 *
 * @param fvec
 * The field vector for the call being handled, which is emitted by the
 * @CALL_DECL_FIELDS macro.
 *
 * @param extparams
 * The extended parameters passed to @zxcall_ext.
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
 * This will be NULL if the call function does not return any values.
 *
 * @param retv_size
 * The size of @retv in bytes. This will be zero if the call function does not
 * return any values.
 */
typedef void (*call_trap_t)(
	const call_t *call,
	const call_field_vector_t *fvec,
	const __typed_array(call_ext_params_t, _Nullable, extparams),
	void *__sized_by(argv_size) argv,
	size_t argv_size,
	void *_Nullable __sized_by(retv_size) retv,
	size_t retv_size
);

// MARK: Declarations
/*
 * _CALL_VOID_OR_ARGS expands to `void` when __VA_ARGS__ is empty, or to
 * __VA_ARGS__ otherwise. This ensures that zero-argument call declarations emit
 * a proper C prototype with `(void)` instead of `()`.
 */
#define _CALL_VOID_OR_ARGS_EXPAND(...) __VA_ARGS__
#define _CALL_VOID_OR_ARGS_IMPL_0x00() void
#define _CALL_VOID_OR_ARGS_IMPL_0x01(...) __VA_ARGS__
#define _CALL_VOID_OR_ARGS_IMPL_0x02(...) __VA_ARGS__
#define _CALL_VOID_OR_ARGS_IMPL_0x03(...) __VA_ARGS__
#define _CALL_VOID_OR_ARGS_IMPL_0x04(...) __VA_ARGS__
#define _CALL_VOID_OR_ARGS_IMPL_0x05(...) __VA_ARGS__
#define _CALL_VOID_OR_ARGS_IMPL_0x06(...) __VA_ARGS__
#define _CALL_VOID_OR_ARGS_IMPL_0x07(...) __VA_ARGS__
#define _CALL_VOID_OR_ARGS_IMPL_0x08(...) __VA_ARGS__
#define _CALL_VOID_OR_ARGS_IMPL_0x09(...) __VA_ARGS__
#define _CALL_VOID_OR_ARGS_IMPL_0x0a(...) __VA_ARGS__
#define _CALL_VOID_OR_ARGS_IMPL_0x0b(...) __VA_ARGS__
#define _CALL_VOID_OR_ARGS_IMPL_0x0c(...) __VA_ARGS__
#define _CALL_VOID_OR_ARGS_IMPL_0x0d(...) __VA_ARGS__
#define _CALL_VOID_OR_ARGS_IMPL_0x0e(...) __VA_ARGS__
#define _CALL_VOID_OR_ARGS_IMPL_0x0f(...) __VA_ARGS__
#define _CALL_VOID_OR_ARGS_IMPL_0x10(...) __VA_ARGS__
#define _CALL_VOID_OR_ARGS_PASTE(N, ...) \
	_CALL_VOID_OR_ARGS_IMPL_ ## N(__VA_ARGS__)
#define _CALL_VOID_OR_ARGS_DISPATCH(N, ...) \
	_CALL_VOID_OR_ARGS_PASTE(N, ##__VA_ARGS__)
#define _CALL_VOID_OR_ARGS(...) \
	_CALL_VOID_OR_ARGS_EXPAND( \
	_CALL_VOID_OR_ARGS_DISPATCH(VA_COUNT(__VA_ARGS__), ##__VA_ARGS__))

/*!
 * @macro CALL_DECL_EXT
 * A variant of @CALL_DECL which also declares additional parameters which are
 * passed directly to the call function rather than being captured in the call
 * object. This is useful for designing APIs which leverage call objects but
 * which also have a consistent context.
 *
 * @param F
 * The name of the function.
 *
 * @param EXT
 * The additional parameters to declare, wrapped in @CALL_EXT_PARAMS, e.g.
 * CALL_EXT_PARAMS(const crypto_t *crypto, int32_t algo). The count of
 * additional parameters is preserved and can be obtained with @_CALL_EXT_CNT.
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
 *	     additional parameters specified by @EXT and the call object parameter
 *	     list specified by @...
 *	   - A call object declaration for the function which must be defined via
 *	     @CALL
 */
#define CALL_DECL_EXT(F, EXT, ...) \
	_Static_assert( \
			sizeof(void *) <= sizeof(uint64_t), \
			"extended call parameters require pointer-width <= 64 bits"); \
	typedef struct call_name(F, _argv) { \
		DELIMIT_SEMICOLON(__VA_ARGS__); \
	} call_name(F, _argv_t); \
\
	_Pragma("GCC diagnostic push") \
	_Pragma("GCC diagnostic ignored \"-Wunused-parameter\"") \
	__unused static inline void \
	call_name(F, _type_check)(_CALL_VOID_OR_ARGS(__VA_ARGS__)) \
	{ \
	} \
	_Pragma("GCC diagnostic pop") \
\
	__export \
	void \
	F( \
		_CALL_EXT_DELIMIT(EXT) \
		const void *__sized_by(argv_size) argv, \
		size_t argv_size, \
		void *_Nullable __sized_by(retv_size) retv, \
		size_t retv_size); \
\
	static const call_t call_name(F, _call) = { \
		.c_version = CALL_VERSION, \
		.log_proto = LOG_PROTO_CALL, \
		.c_name = #F, \
		.c_func_ext_cnt = _CALL_EXT_CNT(EXT), \
		.c_func.cf_unsafe = (void *)F, \
	}

/*!
 * @macro CALL_EXT_PARAMS
 * Wraps the additional parameters for @CALL_DECL_EXT and @stdcall_ext in a
 * parenthesized tuple, preserving the parameter count for later query via
 * @_CALL_EXT_CNT.
 *
 * @param ...
 * The additional parameters.
 */
#define CALL_EXT_PARAMS(...) (__VA_ARGS__)

/*!
 * @macro CALL_EXT_NOPARAMS
 * An empty parameter tuple for @CALL_DECL_EXT and @stdcall_ext indicating no
 * additional parameters.
 */
#define CALL_EXT_NOPARAMS ()

// MARK: Public
/*!
 * @function call_prepare
 * Prepares a call by performing necessary handling for the fields in the call's
 * argument structure which require special handling, as specified by the call's
 * field vector. This function must be invoked by custom trap handlers.
 *
 * @param call
 * The call object for the call being prepared.
 *
 * @param fvec
 * The field vector for the call, which is emitted by the @CALL_DECL_FIELDS
 * macro.
 *
 * @param argv
 * A pointer to the call object's argument structure, which is emitted by the
 * @CALL_DECL macro. The structure type can be obtained with @call_t.
 *
 * @param argv_size
 * The size of @argv in bytes.
 */
__export __nonnull1 __nonnull2 __nonnull3
void
call_prepare(
	const call_t *call,
	const call_field_vector_t *fvec,
	void *__sized_by(argv_size) argv,
	size_t argv_size);

/*!
 * @function call_trap
 * Invokes the default call trap handler. This is intended for use by custom
 * trap handlers which need to invoke the call function after performing their
 * specific handling. This must be called after @call_prepare with the provided
 * argument vector.
 *
 * @param call
 * The call object for the call being invoked.
 *
 * @param extparams
 * The extended parameters.
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
 * This will be NULL if the call function does not return any values.
 *
 * @param retv_size
 * The size of @retv in bytes. This will be zero if the call function does not
 * return any values.
 */
__export __nonnull1 __nonnull2 __nonnull3
void
call_trap(
	const call_t *call,
	const __typed_array(call_ext_params_t, _Nullable, extparams),
	void *__sized_by(argv_size) argv,
	size_t argv_size,
	void *_Nullable __sized_by(retv_size) retv,
	size_t retv_size);

/*!
 * @macro zxcall_ext
 * A variant of @zxcall which also accepts additional parameters to pass
 * directly to the call function rather than being captured in the call object.
 * This is useful for designing APIs which leverage call objects but which also
 * have a consistent context.
 *
 * @param F
 * The name of the call function to call.
 *
 * @param RETV
 * A pointer to the call function's return value structure, or NULL if the call
 * function does not return any values.
 *
 * @param TRAP
 * The trap handler to use to invoke the call. This should be a function pointer
 * of type @call_trap_t. If the trap handler is NULL, the call will be invoked
 * using a default trap handler which simply invokes the call function directly.
 *
 * @param EXT
 * The additional parameters to pass to the call function. These parameters
 * should be wrapped in a call to @CALL_EXT_PARAMS, e.g. CALL_EXT_PARAMS(foo,
 * bar).
 *
 * @param ...
 * The argument list to pass to the call function.
 */
#define zxcall_ext(F, RETV, TRAP, EXT, ...) do { \
	call_trap_t trap_0xc = (TRAP) ?: &_0xcall; \
	__assert_type_compat( \
			typeof(call_name(F, _type_check)(__VA_ARGS__)), \
			void, \
			"call to " #F " not compatible with provided arguments"); \
	call_ext_params_t extparams_0xc = { \
		_CALL_EXT_ARRAY_INIT(EXT) \
	}; \
	call_argv_t(F) argv_0xc = { \
		__VA_ARGS__ \
	}; \
	call_retv_t(F) *retv_0xc = (RETV); \
\
	trap_0xc( \
			&call_name(F, _call), \
			&call_name(F, _fields), \
			extparams_0xc, \
			&argv_0xc, sizeof(argv_0xc), \
			retv_0xc, retv_0xc ? sizeof(call_retv_t(F)) : 0); \
} while (0)

/*!
 * @internal
 * Implementation details.
 */
#define _CALL_EXT_UNPAREN(...) __VA_ARGS__
#define _CALL_EXT_CNT(EXT) _CALL_EXT_CNT_IMPL(_CALL_EXT_UNPAREN EXT)
#define _CALL_EXT_CNT_IMPL(...) VA_COUNT(__VA_ARGS__)
#define _CALL_EXT_TRAIL_0x00()
#define _CALL_EXT_TRAIL_N() ,
#define _CALL_EXT_DELIMIT(EXT) \
	_CALL_EXT_UNPAREN EXT \
	CONCAT(_CALL_EXT_TRAIL_, \
		CONCAT(_DELIMIT_TRAIL_IS_NONEMPTY_, \
			_CALL_EXT_CNT(EXT)))()
#define _CALL_EXT_ARRAY_INIT(EXT) \
	CONCAT(_CALL_EXT_ARRAY_INIT_, \
		CONCAT(_DELIMIT_TRAIL_IS_NONEMPTY_, \
			_CALL_EXT_CNT(EXT)))(EXT)
#define _CALL_EXT_ARRAY_INIT_0x00(EXT) 0
#define _CALL_EXT_ARRAY_INIT_N(EXT) \
	_CALL_EXT_ARRAY_INIT_APPLY(_CALL_EXT_UNPAREN EXT)
#define _CALL_EXT_ARRAY_INIT_APPLY(...) \
	MAP_APPLY(_CALL_EXT_UINT64_T, __VA_ARGS__)
#define _CALL_EXT_UINT64_T(X) (uint64_t)(X),

typedef void (*call_function_ext1_t)(
	uint64_t v1,
	const void *__sized_by(argv_size) argv,
	size_t argv_size,
	void *_Nullable __sized_by(retv_size) retv,
	size_t retv_size
);

typedef void (*call_function_ext2_t)(
	uint64_t v1,
	uint64_t v2,
	const void *__sized_by(argv_size) argv,
	size_t argv_size,
	void *_Nullable __sized_by(retv_size) retv,
	size_t retv_size
);

typedef void (*call_function_ext3_t)(
	uint64_t v1,
	uint64_t v2,
	uint64_t v3,
	const void *__sized_by(argv_size) argv,
	size_t argv_size,
	void *_Nullable __sized_by(retv_size) retv,
	size_t retv_size
);

typedef void (*call_function_ext4_t)(
	uint64_t v1,
	uint64_t v2,
	uint64_t v3,
	uint64_t v4,
	const void *__sized_by(argv_size) argv,
	size_t argv_size,
	void *_Nullable __sized_by(retv_size) retv,
	size_t retv_size
);

__API_HEADER_END(c, nonnull, single)

#endif // __ZX_STD_CALL_EXT_H
