## /*!
## @file
## lib0xc is a set of libc extensions and additions designed to make memory-safe
## programming easier.
## */
ifneq ($(wildcard Makefile.internal),)
include Makefile.internal
else
include Makefile.public
endif
