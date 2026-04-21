/*!
 * @copyright
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT license.
 *
 * @header
 * Context pointer abstraction that leverages pointer tagging to add some amount
 * of safety to the pattern of casting a context pointer to and from 'void *'
 * across API boundaries. This safety is best effort, and if the pointer cannot
 * be tagged with the appropriate size information, the implementation will
 * simply not enforce the size check.
 *
 * @example
 * For a function
 *
 *     void
 *     dispatch_async_f(
 *         dispatch_queue_t dq,
 *         context_t ctxt,
 *         dispatch_function_t f);
 *
 * the call site would be implemented as
 *
 *     state_t s = {
 *         .field0 = 0,
 *         .field1 = 1,
 *         .field2 = 2,
 *     };
 *     context_t ctxt = __context_export(state_t *, &s);
 *     dispatch_async_f(dq, ctxt, &func);
 *
 * and the worker function would be implemented as
 *
 *     void
 *     func(context_t ctxt)
 *     {
 *         const state_t *s = __context_import(state_t *, ctxt);
 *         // do work
 *     }
 *
 * @example
 * Context objects can also be leveraged across API boundaries that do not
 * accept a context_t and instead accept a 'void *' context pointer.
 *
 * For a function
 *
 *     void
 *     dispatch_async_f(
 *         dispatch_queue_t dq,
 *         void *ctxt,
 *         dispatch_function_t f);
 *
 * the call site would be implemented as
 *
 *    state_t s = {
 * 	      .field0 = 0,
 *        .field1 = 1,
 *        .field2 = 2,
 *    };
 *
 *    void *__single ctxt = __context_export_unsafe(state_t *, &s);
 *    dispatch_async_f(dq, ctxt, &func);
 *
 * and the worker function would be implemented as
 *
 *    void
 *    func(void *__single ctxt)
 *    {
 *        const state_t *s = __context_import_unsafe(state_t *, ctxt);
 *        // do work
 *    }
 *
 * @discussion
 * A previous implementation attempt leveraged indexable bounds-checked pointers
 * explicitly and confined this API's usage to __ptrcheck being available.
 * However, it proved too awkward to construct APIs callable with both the C
 * pointer ABI and the wide pointer ABI used by indexable pointers. This
 * implementation is useable universally.
 */
#ifndef __ZX_STD_CONTEXT_H
#define __ZX_STD_CONTEXT_H

#include <0xc/std/api.h>
#include <0xc/std/int.h>
#include <0xc/std/io.h>
#include <0xc/std/limits.h>
#include <0xc/std/string.h>
#include <0xc/std/pointer.h>
#include <0xc/std/type.h>
#include <0xc/sys/panic.h>

#include <stdlib.h>

__API_HEADER_BEGIN(c, nonnull, single)

// MARK: Types
/*!
 * @typedef context_t
 * An intermediary representation of a context object.
 */
typedef tagptr_t context_t;

// MARK: Public
/*!
 * @function __context_export
 * Exports an object as a context object which carries information about the
 * object's size.
 *
 * @param T
 * The type of object that is to be exported, e.g. 'state_t *'.
 *
 * @param P
 * A pointer to the object to be exported.
 *
 * @result
 * The untyped pointer that can be passed as a context pointer.
 */
#define __context_export(T, P) \
	__context_wrap(__DECONST(void *, P), #T, sizeof((P)[0]))

/*!
 * @function __context_export_anonymous
 * Exports an object as a context object which carries information about the
 * object's size, but when the type name is not available.
 *
 * @param P
 * A pointer to the object to be exported.
 *
 * @result
 * The untyped pointer that can be passed as a context pointer.
 */
#define __context_export_anonymous(P) \
	__context_wrap(__DECONST(void *, P), NULL, sizeof((P)[0]))

/*!
 * @function __context_export_unsafe
 * Exports an object as an untyped pointer which carries information about the
 * object's size.
 *
 * @param T
 * The type of object that is to be exported, e.g. 'state_t *'.
 *
 * @param P
 * A pointer to the object to be exported.
 *
 * @result
 * An untyped single pointer that can be passed as a context pointer. This
 * pointer cannot be safely dereferenced.
 */
#define __context_export_unsafe(T, P) (void *__single)({ \
	context_t ctxt_0xc = TAGPTR_NULL; \
	ctxt_0xc = __context_wrap(__DECONST(void *, P), #T, sizeof((P)[0])); \
	__unsafe_forge(void *, ctxt_0xc.__opaque, single); \
})

/*!
 * @function __context_import
 * Imports a context object and verifies that the memory it refers to is of the
 * expected size for the intended object.
 *
 * @param T
 * The type of object that is to be imported, e.g. 'state_t *'.
 *
 * @param P
 * The @context_t object to import.
 *
 * @result
 * An immutable single pointer to the imported object. If the context object
 * does not refer to the expected number of bytes required by the object, the
 * implementation aborts.
 *
 * @discussion
 * When bounds checking is on, the length verification in this implementation is
 * technically superfluous. But it remains enabled because the error message
 * that is emitted is much clearer.
 */
#define __context_import(T, P) (const T __single)({ \
	void *__unsafe_indexable p_0xc = NULL; \
	p_0xc = __context_unwrap((P), #T, sizeof(((T)0)[0])); \
	__forge_type(const T, p_0xc, sizeof(((T)0)[0])); \
})

/*!
 * @function __context_import_mutable
 * Imports a context object and verifies that the memory referred to by its
 * untyped pointer is of the expected size for the intended object.
 *
 * @param T
 * The type of object that is to be imported, e.g. 'state_t *'.
 *
 * @param P
 * The @context_t object to import.
 *
 * @result
 * A mutable single pointer to the imported object. If the untyped pointer does
 * not refer to the expected number of bytes required by the object, the
 * implementation aborts.
 */
#define __context_import_mutable(T, P) (T __single)({ \
	__DECONST(T __single, __context_import(T, P)); \
})

/*!
 * @function __context_import_unsafe
 * Imports an object from an untyped single pointer produced by
 * @__context_export_unsafe.
 *
 * @param T
 * The type of object that is to be imported, e.g. 'state_t *'.
 *
 * @param P
 * An untyped single pointer produced by @__context_export_unsafe.
 *
 * @result
 * An immutable single pointer to the imported object. If the untyped pointer
 * does not refer to the expected number of bytes required by the object, the
 * implementation aborts.
 */
#define __context_import_unsafe(T, P) (const T __single)({ \
	context_t ctxt_0xc = { \
		.__opaque = (uintptr_t)P, \
	}; \
	void *__unsafe_indexable p_0xc = NULL; \
	p_0xc = __context_unwrap(ctxt_0xc, #T, sizeof(((T)0)[0])); \
	__unsafe_forge(const T, p_0xc, single); \
})

/*!
 * @function __context_import_unsafe_mutable
 * Imports an object from an untyped single pointer produced by
 * @__context_export_unsafe.
 *
 * @param T
 * The type of object that is to be imported, e.g. 'state_t *'.
 *
 * @param P
 * An untyped single pointer produced by @__context_export_unsafe.
 *
 * @result
 * A mutable single pointer to the imported object. If the untyped pointer does
 * not refer to the expected number of bytes required by the object, the
 * implementation aborts.
 */
#define __context_import_unsafe_mutable(T, P) (T __single)({ \
	__DECONST(T __single, __context_import_unsafe(T, P)); \
})

/*!
 * @internal
 * Implementation details.
 */
// MARK: Internal
__export __warn_result
context_t
__context_wrap(
	void *_Nullable p,
	const char *_Nullable type_name,
	size_t len);

__export __warn_result __nonnull2
void *__unsafe_indexable
__context_unwrap(
	context_t ctxt,
	const char *type_name,
	size_t len);

__API_HEADER_END(c, nonnull, single)

#endif // __ZX_STD_CONTEXT_H
