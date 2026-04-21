# lib0xc Style Guide
This document describes the C coding style used throughout lib0xc. It is adapted
from the Darwin `style(3)` man page, with deviations to match the conventions
already established in this project. When in doubt, defer to the surrounding
code; consistency within the project trumps any rule written here.

## General Principles
### Diffs should be minimal
Humans ultimately review the code changes to lib0xc, and the style is
constructed to encourage diffs which are small, unambiguous, and presentable in
side-by-side review tools. This style explicitly does not rely on review tools
stripping out whitespace diffs and requires you to think about whitespace.

### Vertical space is a commodity
Scrolling vertically is easy. Do not be afraid to add lines if it makes a future
diff easier to read. One declaration per line, one initializer per line, one
logical step per line.

### Horizontal space is precious
Wrap source lines deliberately at 80 columns. Even in an era of ultra-widescreen
displays, 12-inch laptop screens still exist. and modern IDEs often surround the
code editor with additional functionality (project explorer, AI chat views,
terminals, etc.). Wrapping at 80 columns keeps the project source presentable
even in cramped editor environments and enables side-by-side review of the
naturally-wrapped code on more configurations. Soft-wrapped text is not a
substitute for conscious human craftsmanship.

Do not be afraid to leverage smaller helper functions, intermediary variables,
etc. toward the goal of making a line fit elegantly within the column limit.
Modern C compilers are very good at optimizing out these types of intermediary
constructions, so there's no performance extra credit for cramming a ton of
logic on a single line.

### Indent for scope, nothing else
Indentation indicates scope. Do not use it to align equals signs, `struct`
members, or comments across lines. lib0xc indents with **tabs** (rendered four
columns wide), and tabs appear only at the start of a line. Once code begins, no
further tabs appear. Spaces follow tabs only inside continuation lines (see
below) and inside strings or comments.

### Code is truth
A future maintainer (often you, six months later) should be able to read a
function and understand it without chasing tickets, chat threads, commit
messages, or passive-aggressive comments in pull requests. Linking to external
context is fine when it adds value, but the code itself must stand on its own.

Comments in the code should be the ultimate authority on what the code does and
why it exists.

### Each line should minimize entropy
Break compound expressions into atoms -- one declaration, one assignment, one
condition, etc. -- per line. When one atom changes, the diff should clearly show
only that atom changing. This is a lot more of an art than a science, but when
you force yourself to wrap lines at 80 columns, breaking up expressions
appropriately starts to become a lot more intuitive.

### Avoid lots of function parameters
If you've written a function, and over time you've found yourself adding more
and more parameters, you need to stop and realize that you're missing an object
context.

## Specific Guidelines
### Source file headers
Every source file begins with a HeaderDoc copyright block:

```c
/*!
 * @copyright
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT license.
 */
```

If the file has opening HeaderDoc (e.g. because it is a public header), merge
the file-level HeaderDoc into the same block as the copyright rather than
starting a second comment:

```c
/*!
 * @copyright
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT license.
 *
 * @header
 * Cursor object.
 */
#ifndef __ZX_CURSOR_H
#define __ZX_CURSOR_H
```

Header guards use the `__ZX_` prefix followed by the path components in upper-
case, separated by underscores.

### Column width and continuation lines
Wrap at 80 columns. When a statement must continue onto another line, indent the
continuation by **two additional tabs** so it cannot be confused with the body
that follows it. Operators stay on the line that introduces them, not on the
continuation line.

Good:

```c
if (foo && foo_baz &&
		foo_baz_bar) {
	exit(0);
}
```

Bad:

```c
if (foo && foo_baz &&
	foo_baz_bar) {
	exit(0);
}
```

### Function declarations and definitions
Function parameters always go on their own line. As the C language has grown
more extensions and annotations, parameter declarations have become more
complex. Simply forcing them to occupy their own line makes adding a new
parameter trivial to present as a diff.

The declaration's parameters should be indented one tab. The definition's
parameters should be indented two tabs to visually separate them from the
function body.

Function declarations should carry all appropriate annotations from
`0xc/std/defs.h`. Exported functions should be marked with `__export`, and
internal "private extern" functions should be marked with `__private`. If a
function returns a result, it should carry the `__warn_result` annotation unless
the result is truly optional and can be discarded. Any parameters which must be
non-`NULL` pointers should be marked with the `__nonnullN` attribute.

The function declaration should be the source of truth for all function
attribute annotations. Do not repeat annotations such as `__warn_result` in the
definition.

```c
__export __warn_result __nonnull1
CURSOR *
cmemopen(
	CURSOR *cur,
	void *__sized_by(p_len) _Nullable p,
	size_t p_len,
	const char *mode);

CURSOR *
cmemopen(
		CURSOR *cur,
		void *__sized_by(p_len) _Nullable p,
		size_t p_len,
		const char *mode)
{
	...
}
```

### Avoid prettifying alignment
Do not align assignments, struct members, or trailing comments with extra
whitespace. Re-aligning all columns when a new entry is longer than all previous
ones causes noisy diffs.

Good:

```c
enum foo {
	FOO = 1,
	BAR_BAZ = 3,
	BAZ_BATCH_BAD = 5,
	NEW_ENTRY_THAT_DOES_NOT_REQUIRE_REALIGNING_EVERYTHING_ELSE = 7
};
```

Bad:

```c
enum foo {
	FOO           = 1,
	BAR_BAZ       = 3,
	BAZ_BATCH_BAD = 5,
	NEW_ENTRY_THAT_REQUIRES_REALIGNING_EVERYTHING_ELSE = 7
};
```

There are exceptions such as flag enumerations, where aligning hex values aids
review, e.g.

```c
typedef enum {
	F_INIT = 0x00,
	F_FOO  = 0x01,
	F_BAR  = 0x02,
} flag_t;
```

### One blank line at a time
Use a single blank line to separate logical sections within a function or file.
Never use two or more in a row.

### Initialization
Use C99 designated initializers, one element per line, with a trailing comma on
the last element so adding a new entry is a one-line diff.

Good:

```c
struct my_point p = {
	.x = 1,
	.y = 0,
	.z = 1,
};
```

Bad:

```c
struct my_point p = { .x = 1, .y = 0, .z = 1 };
```

The lib0xc convention is to also expose a `<TYPE>_INIT` macro for any versioned
struct, defined alongside the type:

```c
#define CURSOR_INIT (CURSOR){ \
	.cur_version = CURSOR_VERSION, \
}
```

### Initialize variables and fail closed
Pre-declared variables are initialized to a value indicating failure. Success is
something the code must explicitly assert. For `errno_t` returns, that typically
means initializing to `-1`:

```c
errno_t error = -1;

error = vcprintf(cur, fmt, ap);
return error;
```

For complex aggregates you may omit the initializer if the next operation fully
populates the object, but keep that contract obvious to the reader. Even in
those cases, you should consider using `zstruct` or `zarray`.

### Types end in `_t`
Every `typedef` -- whether of a struct, union, enum, or scalar -- ends in `_t`
and is written entirely in lower case. Object-like structures are declared with
a leading-underscore tag and a same-name typedef:

```c
typedef struct _buff buff_t;

struct _buff {
	...
};
```

### Use explicitly-sized integer types
Prefer `int64_t`, `uint32_t`, `size_t`, and friends from `<stdint.h>` and
`<stddef.h>` over `int`, `long`, or `unsigned`. Use the bare types only when an
external API forces your hand.

### Prefix struct members
Members of any struct that is shared, exported, or persisted to disk must share
a short prefix derived from the type name. This avoids collisions with
preprocessor macros and makes it easy to grep for accesses.

```c
struct _buff {
	struct_version_t bf_version;
	const buff_type_t *bf_type;
	const uint8_t *__counted_by(bf_len) bf_p;
	size_t bf_len;
};
```

Common prefixes in lib0xc include `bf_` (buff), `cur_` (CURSOR), `sf_` (struct
field), and so on.

### Annotate bounds and nullability
lib0xc embraces clang's bounds-safety and nullability extensions. Use the
project's annotation macros on every pointer that crosses an API boundary:

- `__counted_by(N)`, `__sized_by(N)`, `__null_terminated`, `__cstr`, etc. to
  describe the memory that a pointer refers to
- `_Nullable`, `_Nonnull`, etc. for pointers that may be null
- `__API_HEADER_BEGIN(...)` / `__API_HEADER_END` to scope nonnull and pointer-
  class assumptions over an entire header

If a pointer is not annotated, the reader should be able to assume the default
established by the surrounding `__API_HEADER_BEGIN`.

### Use lib0xc's attribute macros, not raw clang attributes
Decorate API and private interface declarations with the project's macros:
`__export`, `__private`, `__warn_result`, `__nonnull1`, etc., `__noreturn`,
`__no_tail_call`, `__printflike`, etc. They expand correctly across the
platforms lib0xc supports. Do not sprinkle `__attribute__((...))` directly into
headers.

### Functions with no parameters take `void`
```c
void
foo(
	void);
```

An empty parameter list in C means "any arguments are accepted", which is almost
never what you want. This is addressed in C23.

### Function definitions on their own line
Place the return type and any attribute decorations on their own lines above the
function name. The opening brace of a definition sits on its own line. This
makes `^name` a reliable regex to find an implementation and makes diffs which
change the return type of a function smaller.

```c
__export __warn_result __nonnull1
CURSOR *
cmemopen(
		CURSOR *cur,
		void *__sized_by(p_len) _Nullable p,
		size_t p_len,
		const char *mode)
{
	...
}
```

### Distinguish exported and non-exported symbols
Internal functions, file-static helpers, and project-local globals are prefixed
with `_`. Anything reachable as API or SPI is not. Decorate exported
declarations with `__export` and internal-but-cross-file declarations with
`__private`.

```c
static errno_t
_ccheckmode(
	const CURSOR *cur,
	const char *__null_terminated mode);

__export __warn_result __nonnull1
CURSOR *
cmemopen(
	CURSOR *cur,
	void *__sized_by(p_len) _Nullable p,
	size_t p_len,
	const char *mode);
```

### Section headers with `MARK:`
Source files are generally organized into sections delineated with a `// MARK:`
comment. IDEs such as Xcode and various code browsers use these as outline
anchors; they also serve as a visual table of contents in plain text. Rough
section ordering:

- `// MARK: Macros`
- `// MARK: External Declarations`
- `// MARK: Supporting Types`
- `// MARK: Forward Declarations`
- `// MARK: Module Globals`
- `// MARK: Utilities`
- `// MARK: Internal`
- `// MARK: Private`
- `// MARK: Public`

The intent of this ordering is that each section is likely to consume the
content of the previous sections, e.g. forward function declarations for the
file are likelier to need to consume supporting `typedefs`, so the
`Supporting Types` section comes first.

Note that clang supports the `mark` pragma (`#pragma mark`) for declaring
outline anchors, but gcc does not, so avoid this pragma.

```c
// MARK: Utilities
static errno_t
_ccheckmode(...)
{
	...
}

// MARK: Public
CURSOR *
cmemopen(...)
{
	...
}
```

### Comments
Use `//` comments for everything inside function bodies and for short remarks.
Use `/*! ... */` HeaderDoc blocks for documenting types, macros, fields, and
functions in headers -- and for the file-level block at the top of a header or
source file.

Always put the comment on its own line, immediately above the code it describes.
Do not embed `/* ... */` comments inside expressions. Do not end a line of code
with a `//` comment. The one exception is a brief annotation on a preprocessor
directive identifying the matching opener -- e.g. `#endif // __FreeBSD__` or
`#else // __APPLE__` -- which aids navigation in long conditional blocks.

Document every public function with a HeaderDoc block:

```c
/*!
 * @function cmemopen
 * Opens a memory buffer as a cursor object.
 *
 * @param cur
 * A pointer to storage for the cursor object.
 *
 * @param p
 * A pointer to the memory buffer.
 *
 * @result
 * A cursor object which can be used to access the buffer.
 */
```

### Spaces around operators and parentheses
- No space around unary operators: `i++`, `*p`, `&x`.
- One space around binary and ternary operators: `a + b`, `c ? d : e`.
- One space between a control keyword and its `(`: `if (x)`, `while (x)`.
- No space between a function name and its `(`: `foo(x)`.
- No space immediately inside parentheses: `foo(x)` not `foo( x )`.

### Always brace control statements
`if`, `else`, `for`, `while`, and `do` always take braces, even for single-
statement bodies. Lack of braces has caused real security bugs and makes adding
a second statement riskier than it should be.

```c
if (foo) {
	bar();
}
```

Brace placement:

- The opening brace of a control statement goes on the same line as the
  statement (or the last continuation line of the statement)
- `else` sits between the closing and opening braces on a single line
- The opening brace of a function definition is on its own line
- The opening brace of a `struct`, `union`, or `enum` is on the declaration line
- Empty bodies use empty braces: `for (...; ...; ...) { }`

### `case` and `switch` share an indent level
All cases in a `switch` are in the same scope, so they share its indent. Avoid
braces around case bodies unless a local variable forces it. `switch` statements
should *always* have a `default` case. If nothing sane can be done in the
`default` case, use `panic_switch`.

```c
switch (cur->cur_mode[0]) {
case 'r':
case 'w':
	break;
default:
	panic_switch(cur->cur_mode[0]);
}
```

### `goto` is fine for cleanup
Use `goto` for sequential error handling -- it is the cleanest tool C provides.
The lib0xc convention is a single `__out:` label at the bottom of the function,
with each early failure jumping there:

```c
    errno_t error = -1;

    error = step1();
    if (error) {
	    goto __out;
    }

    error = step2();
    if (error) {
    	goto __out;
    }

    error = 0;

__out:
    return error;
```

Do not use `goto` as a looping construct. Compile with
`-Wsometimes-uninitialized` so the compiler catches paths that skip an
initialization.

### Declare all variables at top of scope
Because project style is to use `goto`, all variables should be declared at top-
of-scope to make skipping initialization impossible. Never lazily declare
variables within a scope. If you find that this is leading to overly-large
opening variable blocks, this is a good indication that you need to refactor
something.

### Reserve the ternary operator for trivial cases
Use `?:` only for short, unlikely-to-change expressions. If either branch needs
to wrap, use an `if`/`else` instead. The ternary may also be used in places
where the compiler must constant-fold the expression at compile time.

### Avoid booleans
Booleans too often hide behaviors across API boundaries such that the intent of
a function call is not fully captured in the calling context.

Bad:

```c
replace_spaces(str, false);
```

It is not obvious what the `false` implies until you hunt down the declaration
for `replace_spaces`:

```c
void
replace_spaces(
	char *str,
	bool and_tabs);
```

Better is to define a typed `enum` of behaviors:

```c
__typed_enum(replace_spaces_mode, uint8_t, closed,
	REPLACE_SPACES,
	REPLACE_TABS_TOO,
	_REPLACE_SPACES_MODE_CNT,
);
```

```c
replace_spaces(str, REPLACE_SPACES);
replace_spaces(str, REPLACE_TABS_TOO);
```

### Don't pointlessly validate inputs
If you control every caller of a function, do not check parameters defensively
-- the operating system, sanitizers, and `-fbounds-safety` already do a much
better job catching corruption than your code can. Reserve runtime input
validation for data that crosses a trust boundary (untrusted input, IPC, files
on disk).

### Abort on bad API inputs
When an API contract is violated by a caller you do not control, fail fast with
`panicx` (or another lib0xc panic primitive) and a message that describes what
went wrong. Returning an error code for programmer errors trains callers to
ignore them.

```c
if (number > (UINT8_MAX / 2)) {
	panicx("number given to foo_a_bar() too large: %u", number);
}
```

### Don't mingle POSIX return codes and `errno`
Some routines return the error directly; others return `-1` and set `errno`.
Reflect this in your variable names so the reader knows which convention is in
play:

- `errno_t error` -- holds a POSIX error code returned directly
- `int ret` -- holds a status that requires consulting `errno`

The same discipline applies to function return types: a function that returns a
POSIX error code directly returns `errno_t`, and a function that returns `-1` to
signal "consult `errno`" returns `int`. Do not use `int` as a stand-in for
`errno_t`, or vice versa -- the return type is the first thing a caller reads,
and it should immediately tell them which convention is in play.

Local variables holding a POSIX error code that will be returned to the caller
should be annotated with the `__posix_error` attribute (cf.
[`0xc/sys/errno.h`](src/0xc/sys/errno.h)). The attribute installs a cleanup hook
which traps if the variable does not hold a valid `errno` value at scope exit,
catching cases where a code path forgets to assign a result before returning. Do
not annotate intermediate `errno_t` locals that merely capture the result of a
sub-call and are not themselves returned. `__posix_error` is a variable
attribute only; it cannot be applied to a function declaration. The function's
return type (`errno_t`) is what communicates the contract to callers.

New lib0xc APIs return `errno_t`; they do not write to the global `errno`.

### Avoid complex `if` statements; return distinct error codes
Break compound conditions into separate checks so each failure mode gets its own
diff line and a meaningful error code. Most validation reads better as a
sequence of small guards than as one tangled Boolean.

```c
if (foo->f_int > 10 || foo->f_int < 5) {
	return ERANGE;
}

if (!foo->f_uaddr) {
	return EFAULT;
}

if (foo->f_state != FS_INITIALIZED) {
	return EBUSY;
}
```

### Use `sizeof()` on variables, not types
Pass an actual variable to `sizeof()` whenever possible. If the variable's type
changes, the size follows automatically. Remember that `sizeof()` of a pointer
or of an array parameter measures the pointer, not the pointee -- use `strlen`
or an explicit length for those cases.

### Always reference string length directly
Do not hard-code the length of a string literal. Use `strslen(s)` for character
arrays and `strlen(s)` or `strnlen(s)` for character pointers. Modern C
compilers will fold `strlen` of a constant into a constant.

### `NULL` is valid input to `free`
Don't guard a `free` call with a null check, and don't carry a separate
`did_alloc` flag. Use a single pointer that starts as `NULL` and is
unconditionally freed at the end. Or better yet, use `__free`.

### Destruction routines should zero the input pointer
If a pointer is given to a destruction routine, it should be given as a pointer
to a pointer, cf. `buff_destroy`. This makes use-after-free bugs much less
likely. Destruction routines should also be no-ops when the pointee is `NULL`,
which allows for unconditional calls to the destructor at the end of the
function body if the pointee is initialized to `NULL`.

### Preprocessor macros
- `#define`d constants and function-like macros that double-evaluate their
  arguments are written in `ALL_CAPS`
- Function-like macros that evaluate each argument exactly once may use
  lower-case names that mirror the function they replace
- Always parenthesize macro parameters in the expansion. Pass parameters using
  all-caps (e.g. `X`, `BUFF`) to avoid colliding with caller variables and to
  make parameter usage in the macro stand out
- Local variables declared in a macro expression should carry the `_0xc` suffix
  to avoid colliding with variables in the calling scope
- Prefer an inline function to a macro whenever the macro does not need the
  preprocessor's special powers (stringification, token pasting, lazy
  evaluation). When you must use a macro, hand the bulk of the logic off to a
  `static inline` helper with the `__inline_impl` attribute
- Preprocessor directives always start at column 0, regardless of how deeply
  nested the surrounding code is

### `#if` versus `#ifdef`
Prefer `#if FEATURE` over `#ifdef FEATURE`. The C standard makes `#if` evaluate
undefined identifiers as `0`, which lets a project disable a feature by setting
it to `0` rather than by un-defining it.

There are plenty of exceptions to this, notably macros which are conditionally-
defined by the compiler, where it will warn you if you attempt to check them
with `#if` (e.g. `__LP64__`). This guidance is mostly for macros that your code
itself defines and checks.

### Use typed `enum`s
`enum`s should be declared using `__typed_enum` or `__bitfld` to assign an
explicit type to the `enum` and allow `-Wassign-enum` on clang to verify that
assignments to a typed `enum` are within the enumerated range (even for
bitfields).

Normal `enum`s which do not represent bit fields should include a "count"
sentinel at the end of the initializer list, prefixed with an `_` character and
named with the `_CNT` suffix.

```c
__typed_enum(buff_type, uint8_t, closed,
	BUFF_TYPE_INVALID,
	BUFF_TYPE_STATIC,
	BUFF_TYPE_DYNAMIC,
	_BUFF_TYPE_CNT,
);
```

The macro takes the `enum`'s base name (without the `_t` suffix), the underlying
integer type, the `closed`/`open` extensibility tag, and the enumerators. It
produces both the `enum` and a typedef of the form `<NAME>_t`.

### Versioned structures
Public structures in lib0xc carry a `<prefix>_version` field whose type is
`struct_version_t`, initialized via the `<TYPE>_VERSION` macro and asserted on
entry to any constructor with `struct_version_assert`. Provide a matching
`<TYPE>_INIT` macro and, where appropriate, a `<type>_new()` stack allocator
macro.

`struct_version_t` is specifically chosen to be a `uint8_t` to make reading the
structure version from an untrusted blob of data very simple due to two
properties:

1. As long as there is a single byte in the blob, a version can be read from it
2. A single-byte integer can be read without swapping the byte order, even if
   the rest of the structure needs to be swapped

## See Also
- `style(3)` on Darwin -- the original inspiration for this guide.
- [`CONTRIBUTING.md`](CONTRIBUTING.md) -- how to land a change.
- The existing source under [`src/0xc/`](src/0xc/) -- the ground truth when this
  document and the code disagree.
