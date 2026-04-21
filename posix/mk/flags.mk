# /*!
# @file
# Various flags needed for the POSIX target.
# */

# On both arm64 and x86_64 POSIX platforms, we can use the 16 high bits and 3
# low bits for pointer tagging. Due to laziness, we don't support LA57 on Ice
# Lake and later since there's no great way for determining it at runtime on
# Linux. On Darwin, no Macs ever shipped with those parts, so we don't care
# about that.
CFLAGS_ARM64 += -DPOINTER_TAG_BITS_HI=16 -DPOINTER_TAG_BITS_LO=3
CFLAGS_X86_64 += -DPOINTER_TAG_BITS_HI=16 -DPOINTER_TAG_BITS_LO=3

ifeq ($(UNAME),Darwin)
# Darwin has arm64e content, but it's not supported publicly. Still, just for
# the sake of completeness, we define pointer tag properties for it. arm64e uses
# all 16 upper bits for pointer authentication (PAC), so all we've got are the
# low 3 bits.
CFLAGS_ARM64E := -DPOINTER_TAG_BITS_HI=0 -DPOINTER_TAG_BITS_LO=3
endif
