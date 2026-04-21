/*!
 * @copyright
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT license.
 *
 * The memhash() implementation below is derived from MurmurHash2 (MurmurHash64A
 * and MurmurHash64B) by Austin Appleby:
 *
 *     "MurmurHash2 was written by Austin Appleby, and is placed in the
 *      public domain. The author hereby disclaims copyright to this source
 *      code."
 *
 * Upstream: https://github.com/aappleby/smhasher
 */
#include "0xc/sys/hash.h"

#include "0xc/std/defs.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

_Pragma("GCC diagnostic push")
_Pragma("GCC diagnostic ignored \"-Wimplicit-fallthrough\"")
#ifdef __LP64__
static uint64_t
MurmurHash64A(
		const void *__sized_by(p_len) p,
		size_t p_len,
		uint64_t seed)
{
	const uint64_t m = 0xc6a4a7935bd1e995;
	const int r = 47;
	uint64_t h = seed ^ (p_len * m);
	const uint64_t * data = (const uint64_t *)p;
	const uint64_t * end = data + (p_len / 8);
	while(data != end) {
		uint64_t k = *data++;
		k *= m;
		k ^= k >> r;
		k *= m;
		h ^= k;
		h *= m;
	}
	const unsigned char * data2 = (const unsigned char *)data;
	switch(p_len & 7) {
	case 7: h ^= ((uint64_t)data2[6]) << 48;
	case 6: h ^= ((uint64_t)data2[5]) << 40;
	case 5: h ^= ((uint64_t)data2[4]) << 32;
	case 4: h ^= ((uint64_t)data2[3]) << 24;
	case 3: h ^= ((uint64_t)data2[2]) << 16;
	case 2: h ^= ((uint64_t)data2[1]) << 8;
	case 1: h ^= ((uint64_t)data2[0]);
			h *= m;
	};
	h ^= h >> r;
	h *= m;
	h ^= h >> r;
	return h;
}
#else // __LP64__
static uint64_t
MurmurHash64B(
		const void *__sized_by(p_len) p,
		size_t p_len,
		uint64_t seed)
{
	const uint32_t m = 0x5bd1e995;
	const int r = 24;

	uint32_t h1 = (uint32_t)(seed) ^ (uint32_t)p_len;
	uint32_t h2 = (uint32_t)(seed >> 32);

	const uint32_t * data = (const uint32_t *)p;
	size_t len = p_len;

	while(len >= 8) {
		uint32_t k1 = *data++;
		k1 *= m; k1 ^= k1 >> r; k1 *= m;
		h1 *= m; h1 ^= k1;
		len -= 4;

		uint32_t k2 = *data++;
		k2 *= m; k2 ^= k2 >> r; k2 *= m;
		h2 *= m; h2 ^= k2;
		len -= 4;
	}

	if(len >= 4) {
		uint32_t k1 = *data++;
		k1 *= m; k1 ^= k1 >> r; k1 *= m;
		h1 *= m; h1 ^= k1;
		len -= 4;
	}

	switch(len) {
	case 3: h2 ^= ((uint32_t)((const unsigned char *)data)[2]) << 16;
	case 2: h2 ^= ((uint32_t)((const unsigned char *)data)[1]) << 8;
	case 1: h2 ^= ((uint32_t)((const unsigned char *)data)[0]);
			h2 *= m;
	};

	h1 ^= h2 >> 18; h1 *= m;
	h2 ^= h1 >> 22; h2 *= m;
	h1 ^= h2 >> 17; h1 *= m;
	h2 ^= h1 >> 19; h2 *= m;

	uint64_t h = h1;
	h = (h << 32) | h2;
	return h;
}
#endif // __LP64__
_Pragma("GCC diagnostic pop")

uint64_t
memhash(
		const void *__sized_by(p_len) p,
		size_t p_len,
		uint64_t seed)
{
#ifdef __LP64__
	return MurmurHash64A(p, p_len, seed);
#else
	return MurmurHash64B(p, p_len, seed);
#endif
}

uint64_t
strhash(
		const char *__cstr s,
		uint64_t seed)
{
	const void *p_indexable = __null_terminated_to_indexable(s);
	return memhash(p_indexable, strlen(s), seed);
}
