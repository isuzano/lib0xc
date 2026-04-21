# lib0xc
A set of C standard library-adjacent APIs for safer systems programming. While C
cannot be made completely type- and bounds-safe at the language level, its
prevailing uses can be made much safer than they are today.

## Goals
"Make C safer" is a nebulous and amorphous goal, and it is more apt as a
programming language design statement than a modest set of utilities. With that
in mind, lib0xc has the following concrete goals.

### `-Wall -Wextra -Werror -Weverywhere`
That last one isn't real, but still, lib0xc's goal is to make it possible for
projects to turn on as many warnings as possible and to fail to build if code
introduces new warnings. Often, certain high-value warnings are disabled because
a project wants to, e.g.

1. build with `-Werror`, and
2. avoid clumsy workarounds for things like portability

Concerns like this are frontmost in lib0xc's API design.

### Familiar, Easy to Adopt
lib0xc's APIs are deliberately named and designed to *look* like functions
they'd replace from the standard library and be drop-in replacements where
appropriate.

### Embrace Static Bounds
None of lib0xc's APIs assume that an allocator is available (with the exception
of APIs which provide utility *specifically for* allocations). Many of lib0xc's
APIs are designed to be used with fixed-sized data structures (e.g. `struct`s or
array types) and assert that size information for a particular argument is
available at compile-time.

To achieve this, lib0xc leans heavily on the C preprocessor to expose its API
surface. Many of its APIs are macros. While not a panacea, restricting code to
using fixed-size objects (and avoiding dynamic allocations) makes C generally
much safer.

### Support `-fbounds-safety`
lib0xc's API surface fully embraces the
[clang bounds safety](https://aka.ms/clang-bounds-safety) extensions, which
leverage macros that can safely expand to nothing to indicate the bounds of
memory referred to by pointers, making them source-compatible with existing C
code.

### Codify, Document, Test
Many of the APIs that lib0xc exposes have existed in various forms in the
industry, perhaps for decades. lib0xc does not claim to have brilliantly
conceived of the idea behind every API it exposes. Instead, it seeks to provide
codified representations of these patterns which are well-documented and
thoroughly-tested.

### Embiggen C's Pit of Success
Related to the previous goal, the patterns encapsulated by lib0xc APIs should
have a large "pit of success", that is, they are easier to use properly than
they are to mis-use. Many of the C language's liabilities stem from poorly-
designed API contracts, and where such APIs have lingered, lib0xc seeks to offer
better thought-out replacements.

## Components
### Standard Library Extensions (`0xc/std/`)
| Module | Standard Library Analogue | Description |
|--------|---------------------------|-------------|
| **alloc.h** | n/a | Typed allocation, automatic cleanup |
| **call.h** | n/a | Deferred function invocation |
| **context.h** | n/a | Bounds-checked context pointer |
| **cursor.h** | `FILE *` | Allocation-free, in-memory input/output stream |
| **int.h** | `stdint.h` | Safe integer conversions |
| **io.h** | `stdio.h` | Formatted output utilities |
| **pointer.h** | n/a | Useful macros for clang `-fbounds-safety` |
| **string.h** | `string.h` | Static variants of string functions |
| **struct.h** | n/a | Structure reflection and addressing |
| **array.h** | n/a | Utilities for array types |
| **type.h** | n/a | Type compatibility checks and compiler constant utilities |
| **limits.h** | `limits.h` | Min/max value utilities for integer types |

### Systems Programming Utilities (`0xc/sys/`)
| Module | POSIX Analogue |Description |
|--------|----------------|------------|
| **buff.h** | n/a | Bounded buffer encapsulation |
| **log.h** | `syslog.h` | Object-oriented logging with simplified levels |
| **hash.h** | `sys/queue.h` | BSD `queue.h` macro-style hash table |
| **digest.h** | n/a | Digest object |
| **fourcc.h** | n/a | Four-character codes |
| **errno.h** | `errno.h` | POSIX error utilities |
| **exit.h** | `sysexits.h` | `sysexits(3)` mappings to errno codes |
| **queue.h** | n/a | BSD `queue(3)` macros with bounds-safety annotations |
| **linker_set.h** | n/a | Unified, bounds-safe linker set for ELF and Mach-O |
| **check.h** | n/a | Simple unit test check functions |
| **unit.h** | n/a | Test harness with auto-discovery via linker sets |

## Examples
### Bounds-tracked formatting with `CURSOR`
```c
#include <0xc/std/cursor.h>

char buf[256];
CURSOR cur;
cbuffopen(&cur, buf, "w");
cprintf(&cur, "hello %s", "world");  // remaining space tracked automatically
```

### Bounds-checked context pointers
```c
#include <0xc/std/context.h>

struct my_state state;
context_t ctx = __context_export(struct my_state *, &state);

// Size is verified on import — mismatched type sizes will trap:
struct my_state *s = __context_import(struct my_state *, ctx);
```

### Safe integer conversions
```c
#include <0xc/std/int.h>

// Traps at runtime on overflow instead of silently truncating:
size_t n = __cast_signed_unsigned(size_t, file_stat.st_size);
```

### Portable printf format specifiers
```c
#include <0xc/std/io.h>

uint32_t v32 = 42;
uint64_t v64 = 100;
printf("%u %lu\n", oxou(v32), oxolu(v64));  // no PRIu32/PRIu64 needed
```

## Requirements
- C11 with GNU extensions (`-std=gnu11`)
- **clang** or **gcc** (clang recommended for `-fbounds-safety` support)
- GNUMake >= 3.81
- Supported platforms: **macOS** (arm64, x86_64), **Linux** (arm64, x86_64)

## Building
Build the POSIX static library:

```bash
make lib
```

This produces `build/public/lib0xc.a` for the host platform.

## Testing
Run the full test suite (POSIX platforms):

```bash
make test
```

Tests are organized per-module and use the `sys/unit.h` API for registration and
the `sys/check.h` API for individual test case assertions.

## Project Structure
```
src/              Portable source and headers (included by all targets)
  0xc/std/        Standard library extension headers
  0xc/std/call    Call implementation
  0xc/std/pointer Pointer tagging implementation
  0xc/sys/        Systems programming utility headers
  0xc/sys/buff    Buffer object implementation and headers
  0xc/sys/check   Test case assertion implementation
  0xc/sys/log     Logging API implementation
posix/            POSIX target (macOS, Linux)
0xtest/           Test suite
  unit/           Per-module unit tests
```

Each target directory contains a `mk/` directory with its build files and a
`0xc/` directory with any target-specific headers. Generally speaking, the `src`
directory's structure mirrors that of the final header hierarchy, with source
files living alongside their corresponding headers.

## New Platforms
lib0xc can be adopted to new runtime environments and is not strictly tied to a
POSIX-like environment. In order to build a lib0xc library for a new target, you
need to provide the following:

### Allocation Implementation
`std/alloc.h` expects an implementation of `__waiting_for_memory`. Otherwise,
lib0xc assumes that `malloc` is provided by the host, if it is present at all.
If it is not present, then simply defined a stub for `__waiting_for_memory` and
avoid using the `std/alloc.h` header entirely.

### Panic Implementations
`sys/panic.h` expects the following implementations:

- `panic`: Panic with an associated integer value
- `panicno`: Panic with an error number
- `panic0x`: Panic with an associated integer value, expressed as hexadecimal
- `panicx`: Panic with only a message

### Buffer Types
Buffer types specific for the target platform can be enumerated in a platform-
specific header in `sys/buff/type`. For example, the `BUFF_TYPE_MMAP` type is
specific to the POSIX library and encapsulates information for how to free the
underlying memory.

### Log Streams
`sys/log.h` expects its error streams to be implemented as `log_stream_t`
objects. Error streams encapsulate the implementations for writing to the
platform's output device. For example, in the POSIX library, the streams are
mapped to the standard C standard output streams (standard input, output,
error) and are written to via `vdprintf`.

### Platform Header
The platform header is expected to be in `0xc/platform.h` and installed within
the public header hierarchy. This header contains publicly-visible configuration
parameters, currently:

- `ZX_WALLOC_LIMIT`: The maximum limit at `walloc` will wait for memory
- `ZX_LOG_LEVEL`: The platform's log level; this can be different based on the
build variant of the library (e.g. the debug variant may have a higher level
than the release/production variant)

## Contributing
This project welcomes contributions and suggestions. See
[CONTRIBUTING.md](CONTRIBUTING.md) for details, including the Microsoft
Contributor License Agreement (CLA) requirement.

This project has adopted the
[Microsoft Open Source Code of Conduct](https://aka.ms/msosscoc). For more
information see the [Code of Conduct FAQ](https://aka.ms/msosscoc/faq) or
contact [opencode@microsoft.com](mailto:opencode@microsoft.com) with any
additional questions or comments.

## Security
For instructions on reporting security vulnerabilities, see
[SECURITY.md](SECURITY.md). Please do not report security issues through public
GitHub issues.

## Trademarks
This project may contain trademarks or logos for projects, products, or
services. Authorized use of Microsoft trademarks or logos is subject to and must
follow [Microsoft's Trademark & Brand Guidelines](https://aka.ms/tm/guidelines).
Use of Microsoft trademarks or logos in modified versions of this project must
not cause confusion or imply Microsoft sponsorship. Any use of third-party
trademarks or logos is subject to those third-party's policies.

## License
Copyright (c) Microsoft Corporation.

Licensed under the [MIT License](LICENSE.txt).
