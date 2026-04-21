/*!
 * @copyright
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT license.
 */
#include "0xc/sys/check.h"
#include "0xc/sys/unit.h"
#include "0xc/sys/hash.h"

#include "0xc/std/api.h"
#include "0xc/std/pointer.h"
#include "0xc/std/array.h"
#include "0xc/std/string.h"
#include "0xc/std/struct.h"
#include "0xc/sys/log.h"
#include "0xc/std/rand.h"

#include <stdlib.h>
#include <string.h>

#ifndef __OPEN_SOURCE__
#if defined(TARGET_0XFUN) && TARGET_0XFUN
#include "0xc/funos/sys/unit.h"
#endif
#endif



// MARK: IWYU
DECL_FUNC_4IWYU(arc4random);

// MARK: Types
typedef struct _test_node {
	HASH_ENTRY(_test_node) tn_hshe;
	char tn_k[16];
	size_t tn_klen;
} test_node_t;

typedef HASH(test, _test_node, 7) test_hash_t;

// MARK: Utilities
static void
hash_do(
		const unit_t *un,
		test_node_t n_arr[static 32],
		test_node_t n_arr2[static 32],
		size_t n_arr_cnt,
		test_hash_t *hash)
{
	size_t i = 0;
	test_node_t *tni = NULL;
	size_t break_after = 17;
	size_t found = 0;
	size_t traversed = 0;
	const char k_no[] = "AAAAAAAAAAAAAAAAAAAA";

	sdrand();
	checkuint("hash table size", countof(hash->h_buckets), 7);
	tni = HASH_LOOKUP(hash, k_no, strslen(k_no), tn_hshe);
	checknull("non-existent lookup returns null [empty table]", tni);

	for (i = 0; i < n_arr_cnt; i++) {
		test_node_t *n = &n_arr[i];
		char *k = __forge(char *, n->tn_k, bidi);
		const char *__cstr k_zterm = NULL;
		size_t k_len = arc4random() % fldsiz(test_node_t, tn_k);
		const test_node_t *chk = NULL;

		if (k_len < 8) {
			k_len = 8;
		}

		rands(k, fldsiz(test_node_t, tn_k), k_len);
		n->tn_klen = k_len + 1;
		HASH_INSERT(hash, n, tn_hshe);
		zxinfox(un, "inserted key: %s", k);

		chk = HASH_LOOKUP(hash, n->tn_k, n->tn_klen, tn_hshe);
		checkptr("node lookup returned pointer", chk, n);
		checkmem("node lookup", chk->tn_k, chk->tn_klen, n->tn_k, n->tn_klen);

		k_zterm = __unsafe_null_terminated_from_indexable(k);
		chk = HASH_LOOKUP_CSTR(hash, k_zterm, tn_hshe);
		checkptr("node cstr lookup returned pointer", chk, n);

		tni = HASH_LOOKUP(hash, k_no, strslen(k_no), tn_hshe);
		checknull("non-existent lookup returns null [post-insertion]", tni);
	}

	for (i = 0; i < n_arr_cnt; i++) {
		test_node_t *n = &n_arr[i];
		test_node_t *n2 = &n_arr2[i];

		*n2 = *n;
	}

	HASH_FOREACH(tni, hash, tn_hshe) {
		checknonnull("iterator pointer non-null", tni);
		zxinfox(un, "node = %s", cstr(tni->tn_k));

		for (i = 0; i < n_arr_cnt; i++) {
			test_node_t *tnii = &n_arr2[i];

			// Mark this node as found.
			if (strcmp(cstr(tnii->tn_k), cstr(tni->tn_k)) == 0) {
				found += 1;
				tnii->tn_k[0] = 0;
				break;
			}
		}
	}

	// Ensure that we traversed each node exactly once in the table.
	checkuint("found all nodes", found, n_arr_cnt);

	// Test breaks out of loops.
	i = 0;
	HASH_FOREACH(tni, hash, tn_hshe) {
		i += 1;
		if (i == break_after) {
			__hash_break(hash);
		}
	}

	checkuint("broke out of loop", i, break_after);

	// Now remove all the nodes.
	for (i = 0; i < n_arr_cnt; i++) {
		test_node_t *n = &n_arr[i];
		const test_node_t *chk = NULL;

		HASH_REMOVE(hash, n, _test_node, tn_hshe);
		chk = HASH_LOOKUP(hash, n->tn_k, n->tn_klen, tn_hshe);
		checknull("removed node: %s", chk, cstr(n->tn_k));
	}

	traversed = 0;
	HASH_FOREACH(tni, hash, tn_hshe) {
		traversed += 1;
	}

	checkuint("emptied table", traversed, 0);
}

// MARK: Test
#if TARGET_0XTEST
static void
UNIT_TEST_FUNC(hash)(
		__unused const unit_t *un)
{
	static test_node_t n_arr[32] = zarray(test_node_t, 32);
	static test_node_t n_arr2[32] = zarray(test_node_t, 32);
	static test_hash_t hash = HASH_INITIALIZER(
		_test_node,
		tn_k,
		tn_klen
	);

	hash_do(un, n_arr, n_arr2, countof(n_arr), &hash);
}

UNIT_TEST(hash);
#endif // TARGET_0XTEST

#ifndef __OPEN_SOURCE__
#if defined(TARGET_0XFUN) && TARGET_0XFUN
static void
UNIT_TEST_FUNC(hash_wu)(
		__unused const unit_t *un)
{
	static test_node_t n_arr[32] = zarray(test_node_t, 32);
	static test_node_t n_arr2[32] = zarray(test_node_t, 32);
	static test_hash_t hash = HASH_INITIALIZER(
		_test_node,
		tn_k,
		tn_klen
	);

	hash_do(un, n_arr, n_arr2, countof(n_arr), &hash);
}

static void
UNIT_TEST_FUNC(hash_thread)(
		__unused const unit_t *un)
{
	static test_node_t n_arr[32] = zarray(test_node_t, 32);
	static test_node_t n_arr2[32] = zarray(test_node_t, 32);
	static test_hash_t hash = HASH_INITIALIZER(
		_test_node,
		tn_k,
		tn_klen
	);

	hash_do(un, n_arr, n_arr2, countof(n_arr), &hash);
}

UNIT_TEST_WU(hash_wu);
UNIT_TEST_THREAD(hash_thread);
#endif // TARGET_0XFUN
#endif // __OPEN_SOURCE__
