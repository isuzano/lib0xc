/*!
 * @copyright
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT license.
 *
 * @header
 * Test platform definitions. This header satisfies the include directive in
 * 0xc/std/api.h.
 */
#ifndef __ZX_TEST_PLATFORM_H
#define __ZX_TEST_PLATFORM_H

#define ZX_WALLOC_LIMIT 2048

#if !defined(ZX_LOG_LEVEL)
#define ZX_LOG_LEVEL 2
#endif

#endif // __ZX_TEST_PLATFORM_H
