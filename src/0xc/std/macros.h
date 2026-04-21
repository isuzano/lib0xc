/*!
 * @copyright
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT license.
 *
 * @header
 * Macro utilities.
 */
#ifndef __ZX_STD_MACROS_H
#define __ZX_STD_MACROS_H

// MARK: Definitions
/*!
 * @macro COMMA
 * A comma token.
 */
#if !defined (COMMA)
#define COMMA() ,
#endif

/*!
 * @macro SEMICOLON
 * A semicolon token.
 */
#if !defined (SEMICOLON)
#define SEMICOLON() ;
#endif

/*!
 * @macro NODELIM
 * A no-op delimiter that expands to nothing. This is useful as the trailing
 * delimiter argument to @DELIMIT when no trailing delimiter is desired.
 */
#if !defined (NODELIM)
#define NODELIM()
#endif

/*!
 * @function PAREN
 * Delimiter macro that expands to a parenthesis.
 *
 * @param WHICH
 * Either 'open' or 'close' to specify which parenthesis to expand to.
 *
 * @result
 * A left parenthesis token if WHICH is 'open', or a right parenthesis token if
 * WHICH is 'close'.
 */
#define PAREN(WHICH) \
		_PAREN_ ## WHICH()
#define PAREN_open() (
#define PAREN_close() )

// MARK: Operations
/*!
 * @macro PRAGMA
 * Emits a pragma directive.
 *
 * @param P
 * The pragma to emit.
 */
#if !defined(PRAGMA)
#define PRAGMA(P) _Pragma(#P)
#endif

/*!
 * @macro HALT
 * Emits a compile-time error with a message.
 *
 * @param M
 * The message to emit.
 */
#if !defined(HALT)
#define HALT(M) PRAGMA(GCC error ("" M))
#endif

/*!
 * @macro STRINGIFY
 * Converts a token to a string literal.
 *
 * @param S
 * The token to convert.
 *
 * @result
 * The string literal corresponding to @S.
 */
#if !defined(STRINGIFY)
#define _STRINGIFY(S) #S
#define STRINGIFY(S) _STRINGIFY(S)
#endif

/*!
 * @macro __STRINGIFY
 * Variant of @STRINGIFY that is less likely to collide with externally-defined
 * macros which may be broken.
 *
 * @param S
 * The token to convert.
 *
 * @result
 * The string literal corresponding to @S.
 */
#define _ZXSTRINGIFY(S) #S
#define ZXSTRINGIFY(S) _ZXSTRINGIFY(S)

/*!
 * @macro CONCAT
 * Concatenates two tokens.
 *
 * @param X
 * The first token.
 *
 * @param Y
 * The second token.
 *
 * @result
 * The concatenation of @X and @Y.
 */
#if !defined(_CONCAT)
#define _CONCAT(X, Y) X ## Y
#define CONCAT(X, Y) _CONCAT(X, Y)
#endif

/*!
 * @macro ZXCONCAT
 * Variant of @CONCAT that is less likely to collide with externally-defined
 * macros which may be broken.
 *
 * @param X
 * The first token.
 *
 * @param Y
 * The second token.
 *
 * @result
 * The concatenation of @X and @Y.
 */
#define _ZXCONCAT(X, Y) X ## Y
#define ZXCONCAT(X, Y) _ZXCONCAT(X, Y)

/*!
 * @function VA_COUNT
 * Counts the number of variadic arguments. Up to 48 arguments are supported.
 *
 * @param ...
 * 0 to 48 arguments to count.
 *
 * @result
 * The number of arguments passed.
 */
#if !defined(VA_COUNT)
#define _VA_COUNT_ARG_N( \
	_00, _01, _02, _03, _04, _05, _06, _07, _08, \
	_09, _10, _11, _12, _13, _14, _15, _16, \
	_17, _18, _19, _20, _21, _22, _23, _24, \
	_25, _26, _27, _28, _29, _30, _31, _32, \
	_33, _34, _35, _36, _37, _38, _39, _40, \
	_41, _42, _43, _44, _45, _46, _47, _48, \
	N, ...) N

#define _VA_COUNT_IMPL(...) \
	_VA_COUNT_ARG_N(__VA_ARGS__, \
			0x30, 0x2f, 0x2e, 0x2d, 0x2c, 0x2b, 0x2a, 0x29, \
			0x28, 0x27, 0x26, 0x25, 0x24, 0x23, 0x22, 0x21, \
			0x20, 0x1f, 0x1e, 0x1d, 0x1c, 0x1b, 0x1a, 0x19, \
			0x18, 0x17, 0x16, 0x15, 0x14, 0x13, 0x12, 0x11, \
			0x10, 0x0f, 0x0e, 0x0d, 0x0c, 0x0b, 0x0a, 0x09, \
			0x08, 0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01, \
			0x00)

#define VA_COUNT(...) _VA_COUNT_IMPL(_placeholder, ##__VA_ARGS__)
#endif

// MARK: Mapping
/*!
 * @function MAP
 * Maps each variadic argument to a pattern with optional tokens before and
 * after each argument.
 *
 * @param PRE
 * Tokens to prepend to each argument.
 *
 * @param POST
 * Tokens to append to each argument.
 *
 * @param ...
 * 0 to 24 arguments to map over.
 *
 * @example
 *
 *     MAP(function(, );, a, b, c)
 *     // expands to:
 *     //   function(a); function(b); function(c);
 */
#define MAP(PRE, POST, ...) \
	_MAP_EXPAND(_MAP_DISPATCH( \
		VA_COUNT(__VA_ARGS__), PRE, POST, ##__VA_ARGS__))

/*!
 * @function MAP_APPLY
 * Applies a function-like macro to each variadic argument.
 *
 * @param F
 * A function-like macro name. Each argument will be passed to F as F(Ai).
 *
 * @param ...
 * 0 to 24 arguments to apply F to.
 *
 * @example
 *
 *     #define DECL(x) int x;
 *     MAP_APPLY(DECL, a, b, c)
 *     // expands to:
 *     //   int a; int b; int c;
 */
#define MAP_APPLY(F, ...) \
	_MAP_APPLY_EXPAND(_MAP_APPLY_DISPATCH( \
		VA_COUNT(__VA_ARGS__), F, ##__VA_ARGS__))

// MARK: Delimiting
/*!
 * @function DELIMIT
 * Expands arguments with a custom delimiter between each and an optional
 * trailing delimiter after the last argument.
 *
 * @param DELIM
 * A function-like macro that expands to the delimiter token. Must be invocable
 * as DELIM() to produce the delimiter.
 *
 * @param TRAIL
 * A function-like macro that expands to the trailing delimiter token appended
 * after the last argument. When the argument list is empty, the trailing
 * delimiter is not emitted. Use @a NODELIM for no trailing delimiter.
 *
 * @param ...
 * 0 to 24 arguments to expand.
 *
 * @example
 *
 *     DELIMIT(SEMICOLON, NODELIM, int x, char y, float z)
 *     // expands to:
 *     //   int x ; char y ; float z
 *
 *     DELIMIT(COMMA, COMMA, int x, char *name)
 *     // expands to:
 *     //   int x , char *name ,
 *
 *     DELIMIT(COMMA, COMMA)
 *     // expands to nothing.
 */
#define DELIMIT(DELIM, TRAIL, ...) \
	_DELIMIT_EXPAND(_DELIMIT_DISPATCH( \
		VA_COUNT(__VA_ARGS__), DELIM, ##__VA_ARGS__)) \
	_DELIMIT_TRAIL(TRAIL, VA_COUNT(__VA_ARGS__))

/*!
 * @function DELIMIT_COMMA
 * Convenience macro to expand arguments with comma delimiters.
 *
 * @param ...
 * 0 to 24 arguments to expand with commas between them.
 *
 * @example
 *
 *     #define MY_ARGS int x, char *name, float value
 *     typedef void (*my_func_t)(DELIMIT_COMMA(MY_ARGS));
 *     // expands to:
 *     //   typedef void (*my_func_t)(int x , char *name , float value);
 */
#define DELIMIT_COMMA(...) DELIMIT(COMMA, NODELIM, ##__VA_ARGS__)

/*!
 * @function DELIMIT_SEMICOLON
 * Convenience macro to expand arguments with semicolon delimiters.
 *
 * @param ...
 * 0 to 24 arguments to expand with semicolons between them.
 *
 * @example
 *
 *     #define MY_ARGS int x, char *name, float value
 *     struct my_struct {
 *         DELIMIT_SEMICOLON(MY_ARGS);
 *     };
 *     // expands to struct fields: int x ; char *name ; float value ;
 */
#define DELIMIT_SEMICOLON(...) DELIMIT(SEMICOLON, NODELIM, ##__VA_ARGS__)

/*!
 * @internal
 * MAP implementation details.
 */
#define _MAP_EXPAND(...) __VA_ARGS__

#define _MAP_0x00(PRE, POST)
#define _MAP_0x01(PRE, POST, \
		A1) \
	PRE A1 POST
#define _MAP_0x02(PRE, POST, \
		A1, A2) \
	PRE A1 POST \
	PRE A2 POST
#define _MAP_0x03(PRE, POST, \
		A1, A2, A3) \
	PRE A1 POST \
	PRE A2 POST \
	PRE A3 POST
#define _MAP_0x04(PRE, POST, \
		A1, A2, A3, A4) \
	PRE A1 POST \
	PRE A2 POST \
	PRE A3 POST \
	PRE A4 POST
#define _MAP_0x05(PRE, POST, \
		A1, A2, A3, A4, A5) \
	PRE A1 POST \
	PRE A2 POST \
	PRE A3 POST \
	PRE A4 POST \
	PRE A5 POST
#define _MAP_0x06(PRE, POST, \
		A1, A2, A3, A4, A5, A6) \
	PRE A1 POST \
	PRE A2 POST \
	PRE A3 POST \
	PRE A4 POST \
	PRE A5 POST \
	PRE A6 POST
#define _MAP_0x07(PRE, POST, \
		A1, A2, A3, A4, A5, A6, A7) \
	PRE A1 POST \
	PRE A2 POST \
	PRE A3 POST \
	PRE A4 POST \
	PRE A5 POST \
	PRE A6 POST \
	PRE A7 POST
#define _MAP_0x08(PRE, POST, \
		A1, A2, A3, A4, A5, A6, A7, A8) \
	PRE A1 POST \
	PRE A2 POST \
	PRE A3 POST \
	PRE A4 POST \
	PRE A5 POST \
	PRE A6 POST \
	PRE A7 POST \
	PRE A8 POST
#define _MAP_0x09(PRE, POST, \
		A1, A2, A3, A4, A5, A6, A7, A8, A9) \
	PRE A1 POST \
	PRE A2 POST \
	PRE A3 POST \
	PRE A4 POST \
	PRE A5 POST \
	PRE A6 POST \
	PRE A7 POST \
	PRE A8 POST \
	PRE A9 POST
#define _MAP_0x0a(PRE, POST, \
		A1, A2, A3, A4, A5, A6, A7, A8, A9, A10) \
	PRE A1 POST \
	PRE A2 POST \
	PRE A3 POST \
	PRE A4 POST \
	PRE A5 POST \
	PRE A6 POST \
	PRE A7 POST \
	PRE A8 POST \
	PRE A9 POST \
	PRE A10 POST
#define _MAP_0x0b(PRE, POST, \
		A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11) \
	PRE A1 POST \
	PRE A2 POST \
	PRE A3 POST \
	PRE A4 POST \
	PRE A5 POST \
	PRE A6 POST \
	PRE A7 POST \
	PRE A8 POST \
	PRE A9 POST \
	PRE A10 POST \
	PRE A11 POST
#define _MAP_0x0c(PRE, POST, \
		A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12) \
	PRE A1 POST \
	PRE A2 POST \
	PRE A3 POST \
	PRE A4 POST \
	PRE A5 POST \
	PRE A6 POST \
	PRE A7 POST \
	PRE A8 POST \
	PRE A9 POST \
	PRE A10 POST \
	PRE A11 POST \
	PRE A12 POST
#define _MAP_0x0d(PRE, POST, \
		A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13) \
	PRE A1 POST \
	PRE A2 POST \
	PRE A3 POST \
	PRE A4 POST \
	PRE A5 POST \
	PRE A6 POST \
	PRE A7 POST \
	PRE A8 POST \
	PRE A9 POST \
	PRE A10 POST \
	PRE A11 POST \
	PRE A12 POST \
	PRE A13 POST
#define _MAP_0x0e(PRE, POST, \
		A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14) \
	PRE A1 POST \
	PRE A2 POST \
	PRE A3 POST \
	PRE A4 POST \
	PRE A5 POST \
	PRE A6 POST \
	PRE A7 POST \
	PRE A8 POST \
	PRE A9 POST \
	PRE A10 POST \
	PRE A11 POST \
	PRE A12 POST \
	PRE A13 POST \
	PRE A14 POST
#define _MAP_0x0f(PRE, POST, \
		A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15) \
	PRE A1 POST \
	PRE A2 POST \
	PRE A3 POST \
	PRE A4 POST \
	PRE A5 POST \
	PRE A6 POST \
	PRE A7 POST \
	PRE A8 POST \
	PRE A9 POST \
	PRE A10 POST \
	PRE A11 POST \
	PRE A12 POST \
	PRE A13 POST \
	PRE A14 POST \
	PRE A15 POST
#define _MAP_0x10(PRE, POST, \
		A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15, A16) \
	PRE A1 POST \
	PRE A2 POST \
	PRE A3 POST \
	PRE A4 POST \
	PRE A5 POST \
	PRE A6 POST \
	PRE A7 POST \
	PRE A8 POST \
	PRE A9 POST \
	PRE A10 POST \
	PRE A11 POST \
	PRE A12 POST \
	PRE A13 POST \
	PRE A14 POST \
	PRE A15 POST \
	PRE A16 POST
#define _MAP_0x11(PRE, POST, \
		A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15, A16, \
		A17) \
	PRE A1 POST \
	PRE A2 POST \
	PRE A3 POST \
	PRE A4 POST \
	PRE A5 POST \
	PRE A6 POST \
	PRE A7 POST \
	PRE A8 POST \
	PRE A9 POST \
	PRE A10 POST \
	PRE A11 POST \
	PRE A12 POST \
	PRE A13 POST \
	PRE A14 POST \
	PRE A15 POST \
	PRE A16 POST \
	PRE A17 POST
#define _MAP_0x12(PRE, POST, \
		A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15, A16, \
		A17, A18) \
	PRE A1 POST \
	PRE A2 POST \
	PRE A3 POST \
	PRE A4 POST \
	PRE A5 POST \
	PRE A6 POST \
	PRE A7 POST \
	PRE A8 POST \
	PRE A9 POST \
	PRE A10 POST \
	PRE A11 POST \
	PRE A12 POST \
	PRE A13 POST \
	PRE A14 POST \
	PRE A15 POST \
	PRE A16 POST \
	PRE A17 POST \
	PRE A18 POST
#define _MAP_0x13(PRE, POST, \
		A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15, A16, \
		A17, A18, A19) \
	PRE A1 POST \
	PRE A2 POST \
	PRE A3 POST \
	PRE A4 POST \
	PRE A5 POST \
	PRE A6 POST \
	PRE A7 POST \
	PRE A8 POST \
	PRE A9 POST \
	PRE A10 POST \
	PRE A11 POST \
	PRE A12 POST \
	PRE A13 POST \
	PRE A14 POST \
	PRE A15 POST \
	PRE A16 POST \
	PRE A17 POST \
	PRE A18 POST \
	PRE A19 POST
#define _MAP_0x14(PRE, POST, \
		A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15, A16, \
		A17, A18, A19, A20) \
	PRE A1 POST \
	PRE A2 POST \
	PRE A3 POST \
	PRE A4 POST \
	PRE A5 POST \
	PRE A6 POST \
	PRE A7 POST \
	PRE A8 POST \
	PRE A9 POST \
	PRE A10 POST \
	PRE A11 POST \
	PRE A12 POST \
	PRE A13 POST \
	PRE A14 POST \
	PRE A15 POST \
	PRE A16 POST \
	PRE A17 POST \
	PRE A18 POST \
	PRE A19 POST \
	PRE A20 POST
#define _MAP_0x15(PRE, POST, \
		A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15, A16, \
		A17, A18, A19, A20, A21) \
	PRE A1 POST \
	PRE A2 POST \
	PRE A3 POST \
	PRE A4 POST \
	PRE A5 POST \
	PRE A6 POST \
	PRE A7 POST \
	PRE A8 POST \
	PRE A9 POST \
	PRE A10 POST \
	PRE A11 POST \
	PRE A12 POST \
	PRE A13 POST \
	PRE A14 POST \
	PRE A15 POST \
	PRE A16 POST \
	PRE A17 POST \
	PRE A18 POST \
	PRE A19 POST \
	PRE A20 POST \
	PRE A21 POST
#define _MAP_0x16(PRE, POST, \
		A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15, A16, \
		A17, A18, A19, A20, A21, A22) \
	PRE A1 POST \
	PRE A2 POST \
	PRE A3 POST \
	PRE A4 POST \
	PRE A5 POST \
	PRE A6 POST \
	PRE A7 POST \
	PRE A8 POST \
	PRE A9 POST \
	PRE A10 POST \
	PRE A11 POST \
	PRE A12 POST \
	PRE A13 POST \
	PRE A14 POST \
	PRE A15 POST \
	PRE A16 POST \
	PRE A17 POST \
	PRE A18 POST \
	PRE A19 POST \
	PRE A20 POST \
	PRE A21 POST \
	PRE A22 POST
#define _MAP_0x17(PRE, POST, \
		A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15, A16, \
		A17, A18, A19, A20, A21, A22, A23) \
	PRE A1 POST \
	PRE A2 POST \
	PRE A3 POST \
	PRE A4 POST \
	PRE A5 POST \
	PRE A6 POST \
	PRE A7 POST \
	PRE A8 POST \
	PRE A9 POST \
	PRE A10 POST \
	PRE A11 POST \
	PRE A12 POST \
	PRE A13 POST \
	PRE A14 POST \
	PRE A15 POST \
	PRE A16 POST \
	PRE A17 POST \
	PRE A18 POST \
	PRE A19 POST \
	PRE A20 POST \
	PRE A21 POST \
	PRE A22 POST \
	PRE A23 POST
#define _MAP_0x18(PRE, POST, \
		A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15, A16, \
		A17, A18, A19, A20, A21, A22, A23, A24) \
	PRE A1 POST \
	PRE A2 POST \
	PRE A3 POST \
	PRE A4 POST \
	PRE A5 POST \
	PRE A6 POST \
	PRE A7 POST \
	PRE A8 POST \
	PRE A9 POST \
	PRE A10 POST \
	PRE A11 POST \
	PRE A12 POST \
	PRE A13 POST \
	PRE A14 POST \
	PRE A15 POST \
	PRE A16 POST \
	PRE A17 POST \
	PRE A18 POST \
	PRE A19 POST \
	PRE A20 POST \
	PRE A21 POST \
	PRE A22 POST \
	PRE A23 POST \
	PRE A24 POST

#define _MAP_DISPATCH_IMPL(N, PRE, POST, ...) \
	CONCAT(_MAP_, N)(PRE, POST, ##__VA_ARGS__)
#define _MAP_DISPATCH(N, PRE, POST, ...) \
	_MAP_DISPATCH_IMPL(N, PRE, POST, ##__VA_ARGS__)

/*!
 * @internal
 * MAP_APPLY implementation details.
 */
#define _MAP_APPLY_EXPAND(...) __VA_ARGS__

#define _MAP_APPLY_0x00(F, ...)
#define _MAP_APPLY_0x01(F, \
		A1) \
	F(A1)
#define _MAP_APPLY_0x02(F, \
		A1, A2) \
	F(A1) \
	F(A2)
#define _MAP_APPLY_0x03(F, \
		A1, A2, A3) \
	F(A1) \
	F(A2) \
	F(A3)
#define _MAP_APPLY_0x04(F, \
		A1, A2, A3, A4) \
	F(A1) \
	F(A2) \
	F(A3) \
	F(A4)
#define _MAP_APPLY_0x05(F, \
		A1, A2, A3, A4, A5) \
	F(A1) \
	F(A2) \
	F(A3) \
	F(A4) \
	F(A5)
#define _MAP_APPLY_0x06(F, \
		A1, A2, A3, A4, A5, A6) \
	F(A1) \
	F(A2) \
	F(A3) \
	F(A4) \
	F(A5) \
	F(A6)
#define _MAP_APPLY_0x07(F, \
		A1, A2, A3, A4, A5, A6, A7) \
	F(A1) \
	F(A2) \
	F(A3) \
	F(A4) \
	F(A5) \
	F(A6) \
	F(A7)
#define _MAP_APPLY_0x08(F, \
		A1, A2, A3, A4, A5, A6, A7, A8) \
	F(A1) \
	F(A2) \
	F(A3) \
	F(A4) \
	F(A5) \
	F(A6) \
	F(A7) \
	F(A8)
#define _MAP_APPLY_0x09(F, \
		A1, A2, A3, A4, A5, A6, A7, A8, A9) \
	F(A1) \
	F(A2) \
	F(A3) \
	F(A4) \
	F(A5) \
	F(A6) \
	F(A7) \
	F(A8) \
	F(A9)
#define _MAP_APPLY_0x0a(F, \
		A1, A2, A3, A4, A5, A6, A7, A8, A9, A10) \
	F(A1) \
	F(A2) \
	F(A3) \
	F(A4) \
	F(A5) \
	F(A6) \
	F(A7) \
	F(A8) \
	F(A9) \
	F(A10)
#define _MAP_APPLY_0x0b(F, \
		A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11) \
	F(A1) \
	F(A2) \
	F(A3) \
	F(A4) \
	F(A5) \
	F(A6) \
	F(A7) \
	F(A8) \
	F(A9) \
	F(A10) \
	F(A11)
#define _MAP_APPLY_0x0c(F, \
		A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12) \
	F(A1) \
	F(A2) \
	F(A3) \
	F(A4) \
	F(A5) \
	F(A6) \
	F(A7) \
	F(A8) \
	F(A9) \
	F(A10) \
	F(A11) \
	F(A12)
#define _MAP_APPLY_0x0d(F, \
		A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13) \
	F(A1) \
	F(A2) \
	F(A3) \
	F(A4) \
	F(A5) \
	F(A6) \
	F(A7) \
	F(A8) \
	F(A9) \
	F(A10) \
	F(A11) \
	F(A12) \
	F(A13)
#define _MAP_APPLY_0x0e(F, \
		A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14) \
	F(A1) \
	F(A2) \
	F(A3) \
	F(A4) \
	F(A5) \
	F(A6) \
	F(A7) \
	F(A8) \
	F(A9) \
	F(A10) \
	F(A11) \
	F(A12) \
	F(A13) \
	F(A14)
#define _MAP_APPLY_0x0f(F, \
		A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15) \
	F(A1) \
	F(A2) \
	F(A3) \
	F(A4) \
	F(A5) \
	F(A6) \
	F(A7) \
	F(A8) \
	F(A9) \
	F(A10) \
	F(A11) \
	F(A12) \
	F(A13) \
	F(A14) \
	F(A15)
#define _MAP_APPLY_0x10(F, \
		A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15, A16) \
	F(A1) \
	F(A2) \
	F(A3) \
	F(A4) \
	F(A5) \
	F(A6) \
	F(A7) \
	F(A8) \
	F(A9) \
	F(A10) \
	F(A11) \
	F(A12) \
	F(A13) \
	F(A14) \
	F(A15) \
	F(A16)
#define _MAP_APPLY_0x11(F, \
		A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15, A16, \
		A17) \
	F(A1) \
	F(A2) \
	F(A3) \
	F(A4) \
	F(A5) \
	F(A6) \
	F(A7) \
	F(A8) \
	F(A9) \
	F(A10) \
	F(A11) \
	F(A12) \
	F(A13) \
	F(A14) \
	F(A15) \
	F(A16) \
	F(A17)
#define _MAP_APPLY_0x12(F, \
		A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15, A16, \
		A17, A18) \
	F(A1) \
	F(A2) \
	F(A3) \
	F(A4) \
	F(A5) \
	F(A6) \
	F(A7) \
	F(A8) \
	F(A9) \
	F(A10) \
	F(A11) \
	F(A12) \
	F(A13) \
	F(A14) \
	F(A15) \
	F(A16) \
	F(A17) \
	F(A18)
#define _MAP_APPLY_0x13(F, \
		A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15, A16, \
		A17, A18, A19) \
	F(A1) \
	F(A2) \
	F(A3) \
	F(A4) \
	F(A5) \
	F(A6) \
	F(A7) \
	F(A8) \
	F(A9) \
	F(A10) \
	F(A11) \
	F(A12) \
	F(A13) \
	F(A14) \
	F(A15) \
	F(A16) \
	F(A17) \
	F(A18) \
	F(A19)
#define _MAP_APPLY_0x14(F, \
		A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15, A16, \
		A17, A18, A19, A20) \
	F(A1) \
	F(A2) \
	F(A3) \
	F(A4) \
	F(A5) \
	F(A6) \
	F(A7) \
	F(A8) \
	F(A9) \
	F(A10) \
	F(A11) \
	F(A12) \
	F(A13) \
	F(A14) \
	F(A15) \
	F(A16) \
	F(A17) \
	F(A18) \
	F(A19) \
	F(A20)
#define _MAP_APPLY_0x15(F, \
		A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15, A16, \
		A17, A18, A19, A20, A21) \
	F(A1) \
	F(A2) \
	F(A3) \
	F(A4) \
	F(A5) \
	F(A6) \
	F(A7) \
	F(A8) \
	F(A9) \
	F(A10) \
	F(A11) \
	F(A12) \
	F(A13) \
	F(A14) \
	F(A15) \
	F(A16) \
	F(A17) \
	F(A18) \
	F(A19) \
	F(A20) \
	F(A21)
#define _MAP_APPLY_0x16(F, \
		A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15, A16, \
		A17, A18, A19, A20, A21, A22) \
	F(A1) \
	F(A2) \
	F(A3) \
	F(A4) \
	F(A5) \
	F(A6) \
	F(A7) \
	F(A8) \
	F(A9) \
	F(A10) \
	F(A11) \
	F(A12) \
	F(A13) \
	F(A14) \
	F(A15) \
	F(A16) \
	F(A17) \
	F(A18) \
	F(A19) \
	F(A20) \
	F(A21) \
	F(A22)
#define _MAP_APPLY_0x17(F, \
		A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15, A16, \
		A17, A18, A19, A20, A21, A22, A23) \
	F(A1) \
	F(A2) \
	F(A3) \
	F(A4) \
	F(A5) \
	F(A6) \
	F(A7) \
	F(A8) \
	F(A9) \
	F(A10) \
	F(A11) \
	F(A12) \
	F(A13) \
	F(A14) \
	F(A15) \
	F(A16) \
	F(A17) \
	F(A18) \
	F(A19) \
	F(A20) \
	F(A21) \
	F(A22) \
	F(A23)
#define _MAP_APPLY_0x18(F, \
		A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15, A16, \
		A17, A18, A19, A20, A21, A22, A23, A24) \
	F(A1) \
	F(A2) \
	F(A3) \
	F(A4) \
	F(A5) \
	F(A6) \
	F(A7) \
	F(A8) \
	F(A9) \
	F(A10) \
	F(A11) \
	F(A12) \
	F(A13) \
	F(A14) \
	F(A15) \
	F(A16) \
	F(A17) \
	F(A18) \
	F(A19) \
	F(A20) \
	F(A21) \
	F(A22) \
	F(A23) \
	F(A24)

#define _MAP_APPLY_DISPATCH_IMPL(N, F, ...) \
	CONCAT(_MAP_APPLY_, N)(F, ##__VA_ARGS__)
#define _MAP_APPLY_DISPATCH(N, F, ...) \
	_MAP_APPLY_DISPATCH_IMPL(N, F, ##__VA_ARGS__)

/*!
 * @internal
 * DELIMIT implementation details.
 */
#define _DELIMIT_EXPAND(...) __VA_ARGS__

#define _DELIMIT_0x00(DELIM)
#define _DELIMIT_0x01(DELIM, \
		A1) \
	A1
#define _DELIMIT_0x02(DELIM, \
		A1, A2) \
	A1 DELIM() \
	A2
#define _DELIMIT_0x03(DELIM, \
		A1, A2, A3) \
	A1 DELIM() \
	A2 DELIM() \
	A3
#define _DELIMIT_0x04(DELIM, \
		A1, A2, A3, A4) \
	A1 DELIM() \
	A2 DELIM() \
	A3 DELIM() \
	A4
#define _DELIMIT_0x05(DELIM, \
		A1, A2, A3, A4, A5) \
	A1 DELIM() \
	A2 DELIM() \
	A3 DELIM() \
	A4 DELIM() \
	A5
#define _DELIMIT_0x06(DELIM, \
		A1, A2, A3, A4, A5, A6) \
	A1 DELIM() \
	A2 DELIM() \
	A3 DELIM() \
	A4 DELIM() \
	A5 DELIM() \
	A6
#define _DELIMIT_0x07(DELIM, \
		A1, A2, A3, A4, A5, A6, A7) \
	A1 DELIM() \
	A2 DELIM() \
	A3 DELIM() \
	A4 DELIM() \
	A5 DELIM() \
	A6 DELIM() \
	A7
#define _DELIMIT_0x08(DELIM, \
		A1, A2, A3, A4, A5, A6, A7, A8) \
	A1 DELIM() \
	A2 DELIM() \
	A3 DELIM() \
	A4 DELIM() \
	A5 DELIM() \
	A6 DELIM() \
	A7 DELIM() \
	A8
#define _DELIMIT_0x09(DELIM, \
		A1, A2, A3, A4, A5, A6, A7, A8, A9) \
	A1 DELIM() \
	A2 DELIM() \
	A3 DELIM() \
	A4 DELIM() \
	A5 DELIM() \
	A6 DELIM() \
	A7 DELIM() \
	A8 DELIM() \
	A9
#define _DELIMIT_0x0a(DELIM, \
		A1, A2, A3, A4, A5, A6, A7, A8, A9, A10) \
	A1 DELIM() \
	A2 DELIM() \
	A3 DELIM() \
	A4 DELIM() \
	A5 DELIM() \
	A6 DELIM() \
	A7 DELIM() \
	A8 DELIM() \
	A9 DELIM() \
	A10
#define _DELIMIT_0x0b(DELIM, \
		A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11) \
	A1 DELIM() \
	A2 DELIM() \
	A3 DELIM() \
	A4 DELIM() \
	A5 DELIM() \
	A6 DELIM() \
	A7 DELIM() \
	A8 DELIM() \
	A9 DELIM() \
	A10 DELIM() \
	A11
#define _DELIMIT_0x0c(DELIM, \
		A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12) \
	A1 DELIM() \
	A2 DELIM() \
	A3 DELIM() \
	A4 DELIM() \
	A5 DELIM() \
	A6 DELIM() \
	A7 DELIM() \
	A8 DELIM() \
	A9 DELIM() \
	A10 DELIM() \
	A11 DELIM() \
	A12
#define _DELIMIT_0x0d(DELIM, \
		A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13) \
	A1 DELIM() \
	A2 DELIM() \
	A3 DELIM() \
	A4 DELIM() \
	A5 DELIM() \
	A6 DELIM() \
	A7 DELIM() \
	A8 DELIM() \
	A9 DELIM() \
	A10 DELIM() \
	A11 DELIM() \
	A12 DELIM() \
	A13
#define _DELIMIT_0x0e(DELIM, \
		A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14) \
	A1 DELIM() \
	A2 DELIM() \
	A3 DELIM() \
	A4 DELIM() \
	A5 DELIM() \
	A6 DELIM() \
	A7 DELIM() \
	A8 DELIM() \
	A9 DELIM() \
	A10 DELIM() \
	A11 DELIM() \
	A12 DELIM() \
	A13 DELIM() \
	A14
#define _DELIMIT_0x0f(DELIM, \
		A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15) \
	A1 DELIM() \
	A2 DELIM() \
	A3 DELIM() \
	A4 DELIM() \
	A5 DELIM() \
	A6 DELIM() \
	A7 DELIM() \
	A8 DELIM() \
	A9 DELIM() \
	A10 DELIM() \
	A11 DELIM() \
	A12 DELIM() \
	A13 DELIM() \
	A14 DELIM() \
	A15
#define _DELIMIT_0x10(DELIM, \
		A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15, A16) \
	A1 DELIM() \
	A2 DELIM() \
	A3 DELIM() \
	A4 DELIM() \
	A5 DELIM() \
	A6 DELIM() \
	A7 DELIM() \
	A8 DELIM() \
	A9 DELIM() \
	A10 DELIM() \
	A11 DELIM() \
	A12 DELIM() \
	A13 DELIM() \
	A14 DELIM() \
	A15 DELIM() \
	A16
#define _DELIMIT_0x11(DELIM, \
		A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15, A16, \
		A17) \
	A1 DELIM() \
	A2 DELIM() \
	A3 DELIM() \
	A4 DELIM() \
	A5 DELIM() \
	A6 DELIM() \
	A7 DELIM() \
	A8 DELIM() \
	A9 DELIM() \
	A10 DELIM() \
	A11 DELIM() \
	A12 DELIM() \
	A13 DELIM() \
	A14 DELIM() \
	A15 DELIM() \
	A16 DELIM() \
	A17
#define _DELIMIT_0x12(DELIM, \
		A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15, A16, \
		A17, A18) \
	A1 DELIM() \
	A2 DELIM() \
	A3 DELIM() \
	A4 DELIM() \
	A5 DELIM() \
	A6 DELIM() \
	A7 DELIM() \
	A8 DELIM() \
	A9 DELIM() \
	A10 DELIM() \
	A11 DELIM() \
	A12 DELIM() \
	A13 DELIM() \
	A14 DELIM() \
	A15 DELIM() \
	A16 DELIM() \
	A17 DELIM() \
	A18
#define _DELIMIT_0x13(DELIM, \
		A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15, A16, \
		A17, A18, A19) \
	A1 DELIM() \
	A2 DELIM() \
	A3 DELIM() \
	A4 DELIM() \
	A5 DELIM() \
	A6 DELIM() \
	A7 DELIM() \
	A8 DELIM() \
	A9 DELIM() \
	A10 DELIM() \
	A11 DELIM() \
	A12 DELIM() \
	A13 DELIM() \
	A14 DELIM() \
	A15 DELIM() \
	A16 DELIM() \
	A17 DELIM() \
	A18 DELIM() \
	A19
#define _DELIMIT_0x14(DELIM, \
		A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15, A16, \
		A17, A18, A19, A20) \
	A1 DELIM() \
	A2 DELIM() \
	A3 DELIM() \
	A4 DELIM() \
	A5 DELIM() \
	A6 DELIM() \
	A7 DELIM() \
	A8 DELIM() \
	A9 DELIM() \
	A10 DELIM() \
	A11 DELIM() \
	A12 DELIM() \
	A13 DELIM() \
	A14 DELIM() \
	A15 DELIM() \
	A16 DELIM() \
	A17 DELIM() \
	A18 DELIM() \
	A19 DELIM() \
	A20
#define _DELIMIT_0x15(DELIM, \
		A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15, A16, \
		A17, A18, A19, A20, A21) \
	A1 DELIM() \
	A2 DELIM() \
	A3 DELIM() \
	A4 DELIM() \
	A5 DELIM() \
	A6 DELIM() \
	A7 DELIM() \
	A8 DELIM() \
	A9 DELIM() \
	A10 DELIM() \
	A11 DELIM() \
	A12 DELIM() \
	A13 DELIM() \
	A14 DELIM() \
	A15 DELIM() \
	A16 DELIM() \
	A17 DELIM() \
	A18 DELIM() \
	A19 DELIM() \
	A20 DELIM() \
	A21
#define _DELIMIT_0x16(DELIM, \
		A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15, A16, \
		A17, A18, A19, A20, A21, A22) \
	A1 DELIM() \
	A2 DELIM() \
	A3 DELIM() \
	A4 DELIM() \
	A5 DELIM() \
	A6 DELIM() \
	A7 DELIM() \
	A8 DELIM() \
	A9 DELIM() \
	A10 DELIM() \
	A11 DELIM() \
	A12 DELIM() \
	A13 DELIM() \
	A14 DELIM() \
	A15 DELIM() \
	A16 DELIM() \
	A17 DELIM() \
	A18 DELIM() \
	A19 DELIM() \
	A20 DELIM() \
	A21 DELIM() \
	A22
#define _DELIMIT_0x17(DELIM, \
		A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15, A16, \
		A17, A18, A19, A20, A21, A22, A23) \
	A1 DELIM() \
	A2 DELIM() \
	A3 DELIM() \
	A4 DELIM() \
	A5 DELIM() \
	A6 DELIM() \
	A7 DELIM() \
	A8 DELIM() \
	A9 DELIM() \
	A10 DELIM() \
	A11 DELIM() \
	A12 DELIM() \
	A13 DELIM() \
	A14 DELIM() \
	A15 DELIM() \
	A16 DELIM() \
	A17 DELIM() \
	A18 DELIM() \
	A19 DELIM() \
	A20 DELIM() \
	A21 DELIM() \
	A22 DELIM() \
	A23
#define _DELIMIT_0x18(DELIM, \
		A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15, A16, \
		A17, A18, A19, A20, A21, A22, A23, A24) \
	A1 DELIM() \
	A2 DELIM() \
	A3 DELIM() \
	A4 DELIM() \
	A5 DELIM() \
	A6 DELIM() \
	A7 DELIM() \
	A8 DELIM() \
	A9 DELIM() \
	A10 DELIM() \
	A11 DELIM() \
	A12 DELIM() \
	A13 DELIM() \
	A14 DELIM() \
	A15 DELIM() \
	A16 DELIM() \
	A17 DELIM() \
	A18 DELIM() \
	A19 DELIM() \
	A20 DELIM() \
	A21 DELIM() \
	A22 DELIM() \
	A23 DELIM() \
	A24

#define _DELIMIT_DISPATCH_IMPL(N, DELIM, ...) \
	CONCAT(_DELIMIT_, N)(DELIM, ##__VA_ARGS__)
#define _DELIMIT_DISPATCH(N, DELIM, ...) \
	_DELIMIT_DISPATCH_IMPL(N, DELIM, ##__VA_ARGS__)

#define _DELIMIT_TRAIL_0x00(TRAIL)
#define _DELIMIT_TRAIL_N(TRAIL) TRAIL()
#define _DELIMIT_TRAIL(TRAIL, N) \
	CONCAT(_DELIMIT_TRAIL_, \
		CONCAT(_DELIMIT_TRAIL_IS_NONEMPTY_, N))(TRAIL)
#define _DELIMIT_TRAIL_IS_NONEMPTY_0x00 0x00
#define _DELIMIT_TRAIL_IS_NONEMPTY_0x01 N
#define _DELIMIT_TRAIL_IS_NONEMPTY_0x02 N
#define _DELIMIT_TRAIL_IS_NONEMPTY_0x03 N
#define _DELIMIT_TRAIL_IS_NONEMPTY_0x04 N
#define _DELIMIT_TRAIL_IS_NONEMPTY_0x05 N
#define _DELIMIT_TRAIL_IS_NONEMPTY_0x06 N
#define _DELIMIT_TRAIL_IS_NONEMPTY_0x07 N
#define _DELIMIT_TRAIL_IS_NONEMPTY_0x08 N
#define _DELIMIT_TRAIL_IS_NONEMPTY_0x09 N
#define _DELIMIT_TRAIL_IS_NONEMPTY_0x0a N
#define _DELIMIT_TRAIL_IS_NONEMPTY_0x0b N
#define _DELIMIT_TRAIL_IS_NONEMPTY_0x0c N
#define _DELIMIT_TRAIL_IS_NONEMPTY_0x0d N
#define _DELIMIT_TRAIL_IS_NONEMPTY_0x0e N
#define _DELIMIT_TRAIL_IS_NONEMPTY_0x0f N
#define _DELIMIT_TRAIL_IS_NONEMPTY_0x10 N
#define _DELIMIT_TRAIL_IS_NONEMPTY_0x11 N
#define _DELIMIT_TRAIL_IS_NONEMPTY_0x12 N
#define _DELIMIT_TRAIL_IS_NONEMPTY_0x13 N
#define _DELIMIT_TRAIL_IS_NONEMPTY_0x14 N
#define _DELIMIT_TRAIL_IS_NONEMPTY_0x15 N
#define _DELIMIT_TRAIL_IS_NONEMPTY_0x16 N
#define _DELIMIT_TRAIL_IS_NONEMPTY_0x17 N
#define _DELIMIT_TRAIL_IS_NONEMPTY_0x18 N

#endif // __ZX_STD_MACROS_H
