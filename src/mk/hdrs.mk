## /*!
## @file
## Generic header list.
## */
HDRS_DIR_GEN := src
HDRS_GEN := \
	$(wildcard src/0xc/shim/*.h) \
	$(wildcard src/0xc/std/*.h) \
	$(wildcard src/0xc/sys/*.h) \
	$(wildcard src/0xc/sys/buff/*.h)
HDRS_GEN := $(filter-out %_internal.h,$(HDRS_GEN))
