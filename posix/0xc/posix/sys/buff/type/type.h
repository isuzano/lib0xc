/*!
 * @copyright
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT license.
 *
 * @header
 * POSIX buffer types.
 */
#ifndef __ZX_POSIX_SYS_BUFF_TYPE_TYPE_H
#define __ZX_POSIX_SYS_BUFF_TYPE_TYPE_H

#include <0xc/std/api.h>
#include <0xc/sys/buff/type.h>

#include <sys/mman.h>

__API_HEADER_BEGIN(c, nonnull, single)

/*!
 * @const BUFF_TYPE_MMAP
 * A buffer created by a call to mmap(2).
 */
__export
const buff_type_t _buff_type_mmap;
#define BUFF_TYPE_MMAP __CONSTANT(buff_type, mmap);

__API_HEADER_END(c, nonnull, single)

#endif // __ZX_POSIX_SYS_BUFF_TYPE_TYPE_H
