/*!
 * @copyright
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT license.
 */
#include "0xc/sys/check.h"
#include "0xc/sys/unit.h"
#include "0xc/sys/posixdefs.h"

#include "0xc/std/defs.h"
#include "0xc/sys/errno.h"

#include <errno.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/unistd.h>

static void
UNIT_TEST_FUNC(posix_access_enoent)(
		__unused const unit_t *un)
{
	errno_t error = -1;

	error = posix(access("/nonexistent", F_OK));
	checkerrno("access nonexistent path", error, ENOENT);
}

static void
UNIT_TEST_FUNC(posix_access_success)(
		__unused const unit_t *un)
{
	errno_t error = -1;

	error = posix(access("/", F_OK));
	checkerrno("access root", error, 0);
}

static void
UNIT_TEST_FUNC(posix_switch_enoent)(
		__unused const unit_t *un)
{
	int ret = -1;
	errno_t error = -1;
	bool handled = false;

	ret = access("/nonexistent", F_OK);
	posix_switch (ret) {
	case 0:
		error = 0;
		handled = true;
		break;
	case ENOENT:
		error = ENOENT;
		handled = true;
		break;
	}

	checkerrno("switch access nonexistent", error, ENOENT);
	checkuint("switch handled", handled, true);
}

static void
UNIT_TEST_FUNC(posix_switch_success)(
		__unused const unit_t *un)
{
	int ret = -1;
	errno_t error = -1;
	bool handled = false;

	ret = access("/", F_OK);
	posix_switch (ret) {
	case 0:
		error = 0;
		handled = true;
		break;
	case ENOENT:
		error = ENOENT;
		handled = true;
		break;
	}

	checkerrno("switch access root", error, 0);
	checkuint("switch handled", handled, true);
}

static void
UNIT_TEST_FUNC(posix_acquire_success)(
		__unused const unit_t *un)
{
	fd_t fd = -1;
	errno_t error = -1;

	error = posix_acquire(dup(STDERR_FILENO), &fd);
	checkerrno("acquire dup stderr", error, 0);
	checkint("acquired fd valid", (fd >= 0), true);
	close(fd);
}

static void
UNIT_TEST_FUNC(posix_acquire_ebadf)(
		__unused const unit_t *un)
{
	fd_t fd = -1;
	errno_t error = -1;

	error = posix_acquire(dup(-1), &fd);
	checkerrno("acquire dup bad fd", error, EBADF);
	checkint("fd unchanged", fd, -1);
}

UNIT_TEST(posix_access_enoent);
UNIT_TEST(posix_access_success);
UNIT_TEST(posix_switch_enoent);
UNIT_TEST(posix_switch_success);
UNIT_TEST(posix_acquire_success);
UNIT_TEST(posix_acquire_ebadf);
