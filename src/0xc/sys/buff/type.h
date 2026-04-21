/*!
 * @copyright
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT license.
 *
 * @file
 * Buffer types.
 */
#ifndef __ZX_SYS_BUFF_TYPE_H
#define __ZX_SYS_BUFF_TYPE_H

#include <0xc/std/api.h>

#include <stdlib.h>

__API_HEADER_BEGIN(c, nonnull, single)

/*!
 * @struct buff_type_t
 * An opaque type descriptor for a buffer.
 */
typedef struct _buff_type buff_type_t;

/*!
 * @const BUFF_TYPE_NULL
 * A buffer which has no explicit destructor, e.g. a stack buffer.
 */
__export
const buff_type_t _buff_type_null;
#define BUFF_TYPE_NULL __CONSTANT(buff_type, null)

/*!
 * @const BUFF_TYPE_LOANEE
 * A buffer which aliases memory owned by another buffer.
 */
__export
const buff_type_t _buff_type_loanee;
#define BUFF_TYPE_LOANEE __CONSTANT(buff_type, loanee)

/*!
 * @const BUFF_TYPE_CURSOR
 * A buffer which is a cursor tracking reads or writes through a parent buffer.
 */
__export
const buff_type_t _buff_type_cursor;
#define BUFF_TYPE_CURSOR __CONSTANT(buff_type, cursor)

/*!
 * @const BUFF_TYPE_MALLOC
 * A buffer created by an ANSI-conforming malloc(3) implementation.
 */
__export
const buff_type_t _buff_type_malloc;
#define BUFF_TYPE_MALLOC __CONSTANT(buff_type, malloc)

__API_HEADER_END(c, nonnull, single)

#endif // __ZX_SYS_BUFF_TYPE_H
