## /*!
## @file
## Generic source file list for the target. This is ingested by both the lib0xc
## target and the 0xtest target.
## */
SRC_GEN := \
	$(wildcard src/0xc/*.c) \
	$(wildcard src/0xc/std/*.c) \
	$(wildcard src/0xc/std/call/field/type/*.c) \
	$(wildcard src/0xc/std/call/field/*.c) \
	$(wildcard src/0xc/sys/*.c) \
	$(wildcard src/0xc/sys/buff/*.c) \
	$(wildcard src/0xc/sys/buff/type/*.c) \
	$(wildcard src/0xc/sys/check/*.c) \
	$(wildcard src/0xc/sys/log/*.c)
SRC_GEN := $(filter-out %_internal.h,$(SRC_GEN))
