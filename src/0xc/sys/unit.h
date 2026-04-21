/*!
 * @copyright
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT license.
 *
 * @header
 * Unit test object. This interface allows a project to declare individual unit
 * tests which are run in an undefined order.
 */
#ifndef __ZX_SYS_UNIT_H
#define __ZX_SYS_UNIT_H

#include <0xc/std/api.h>
#include <0xc/std/pointer.h>
#include <0xc/sys/linker_set.h>
#include <0xc/sys/log.h>

#include <string.h>

__API_HEADER_BEGIN(c, nonnull, single)

// MARK: Forward Types
typedef struct _unit unit_t;

// MARK: Types
/*!
 * @typedef unit_test_runnable_t
 * A function which determines whether the unit test should be run.
 *
 * @param un
 * The unit test.
 *
 * @result
 * The callee is expected to return zero if the test should run. Otherwise, a
 * non-zero value should be returned.
 */
typedef int (*unit_test_runnable_t)(
	const unit_t *un
);

/*!
 * @typedef unit_function_t
 * The function signature for a unit test.
 *
 * @param un
 * The unit test being run.
 */
typedef void (*unit_function_t)(
	const unit_t *un
);

// MARK: Globals
/*!
 * @const LOG_PROTO_UNIT
 * The log protocol for the @unit_t object.
 */
__export
const log_proto_t _log_proto_unit;
#define LOG_PROTO_UNIT __CONSTANT(log_proto, unit)

/*!
 * @const units
 * The unit test linker set. Traverse with @UNIT_FOREACH,
 */
LINKER_SET_DECL(unit_t, units);

// MARK: Object
/*!
 * @typedef unit_t
 * A unit test.
 *
 * @field un_version
 * The version of the structure.
 *
 * @field log_proto
 * The log protocol.
 *
 * @field un_name
 * The name of the test.
 *
 * @field un_runnable
 * An optional function which determines whether the test should be run. If not
 * specified, the test is always run.
 *
 * @field un_do
 * The test function.
 */
struct _unit {
	struct_version_t un_version;
	const log_proto_t *log_proto;
	const char *__cstr un_name;
	unit_test_runnable_t _Nullable un_runnable;
	unit_function_t un_do;
};
#define UNIT_VERSION STRUCT_VERSION(0)

// MARK: Public
/*!
 * @function UNIT_TEST_FUNC
 * Expands to the name of a unit test function.
 *
 * @param N
 * The name of the unit test.
 *
 * @result
 * The name of the function.
 */
#define UNIT_TEST_FUNC(N) __unit_test_ ## N ## _do

/*!
 * @function UNIT_TEST
 * Declares and registers a unit test.
 *
 * @param N
 * The name of the unit test.
 *
 * @result
 * A new unit test of the given name.
 */
#define UNIT_TEST(N) \
	static const unit_t UNIT_TEST_NAME(N) = { \
		.un_version = UNIT_VERSION, \
		.log_proto = LOG_PROTO_UNIT, \
		.un_name = #N, \
		.un_do = &UNIT_TEST_FUNC(N), \
	}; \
	UNIT_REGISTER(UNIT_TEST_NAME(N))

/*!
 * @function UNIT_FOREACH
 * Iterates over all registered unit tests.
 *
 * @param UN
 * A pointer to a @unit_t object which will be set to the current test.
 *
 * @param UNN
 * A pointer to a 'unit_t *' which is used by the implementation.
 *
 * @example
 *
 *    const unit_t *__single un = NULL;
 *    const unit_t *__single *__unsafe_indexable unn = NULL;
 *
 *    LINKER_SET_FOREACH(un, units, unn) {
 *        unit_do(un);
 *    }
 */
#define UNIT_FOREACH(UN, UNN) \
	LINKER_SET_FOREACH(UN, units, UNN)

/*!
 * @function unit_do
 * Runs the unit test. Failing unit tests are expected to abort.
 *
 * @param un
 * The unit test.
 */
__export __nonnull1
void
unit_do(
	const unit_t *un);

/*!
 * @internal
 * Implementation details.
 */
#define UNIT_REGISTER(UN) LINKER_SET_ENTRY(units, UN);
#define UNIT_TEST_NAME(N) __unit_test_ ## N

__API_HEADER_END(c, nonnull, single)

#endif // __ZX_SYS_UNIT_H
