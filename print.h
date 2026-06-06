#pragma once

// This file defines some functions to make it easy to do formatted text
// without using printf style specifiers:
//
//     print(...) - print text
//     fprint(file, ...) - print text to file
//     String(...) - return an allocated string
//
// If you put `#define PRINT_COLOR 1` before the import, text will be printed
// with terminal colors.

#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef PRINT_COLOR
#define PRINT_COLOR 0
#endif

#define EVAL0(...) __VA_ARGS__
#define EVAL1(...) EVAL0(EVAL0(EVAL0(__VA_ARGS__)))
#define EVAL2(...) EVAL1(EVAL1(EVAL1(__VA_ARGS__)))
#define EVAL3(...) EVAL2(EVAL2(EVAL2(__VA_ARGS__)))
#define EVAL4(...) EVAL3(EVAL3(EVAL3(__VA_ARGS__)))
#define EVAL(...)  EVAL4(EVAL4(EVAL4(__VA_ARGS__)))

#define MAP_END(...)
#define MAP_OUT
#define MAP_COMMA ,

#define MAP_GET_END2() 0, MAP_END
#define MAP_GET_END1(...) MAP_GET_END2
#define MAP_GET_END(...) MAP_GET_END1
#define MAP_NEXT0(test, next, ...) next MAP_OUT

#define MAP_LIST_NEXT1(test, next) MAP_NEXT0(test, MAP_COMMA next, 0)
#define MAP_LIST_NEXT(test, next)  MAP_LIST_NEXT1(MAP_GET_END test, next)

#define MAP_LIST0(f, x, peek, ...) f(x) MAP_LIST_NEXT(peek, MAP_LIST1)(f, peek, __VA_ARGS__)
#define MAP_LIST1(f, x, peek, ...) f(x) MAP_LIST_NEXT(peek, MAP_LIST0)(f, peek, __VA_ARGS__)

#define MAP_LIST(f, ...) EVAL(MAP_LIST1(f, __VA_ARGS__, ()()(), ()()(), ()()(), 0))

// GCC lets you define macro-like functions which are always inlined and never
// compiled using this combination of flags. See: https://gcc.gnu.org/onlinedocs/gcc/Inline.html
#ifndef PRINT_FN
#define PRINT_FN extern inline __attribute__((gnu_inline, always_inline)) int
#endif

typedef struct {
    uint64_t n;
    bool no_prefix;
    bool uppercase;
    int digits;
} hex_format_t;
#define hex(x, ...) ((hex_format_t){.n=x, __VA_ARGS__})

typedef struct {
    uint64_t n;
    bool no_prefix;
    bool uppercase;
    int digits;
} oct_format_t;
#define oct(x, ...) ((oct_format_t){.n=x, __VA_ARGS__})

typedef struct {
    double n;
    int precision;
} num_format_t;
#define num_format(x, ...) ((num_format_t){.n=x, __VA_ARGS__})

typedef struct {
    const char *str;
} quoted_t;
#define quoted(s) ((quoted_t){s})

#if PRINT_COLOR
#define hl(s) "\033[35m" s "\033[m"
#else
#define hl(s) s
#endif
PRINT_FN _print_int(FILE *f, int64_t x) { return fprintf(f, hl("%ld"), x); }
PRINT_FN _print_uint(FILE *f, uint64_t x) { return fprintf(f, hl("%lu"), x); }
PRINT_FN _print_double(FILE *f, double x) { return fprintf(f, hl("%g"), x); }
PRINT_FN _print_pointer(FILE *f, void *p) { return fprintf(f, hl("%p"), p); }
PRINT_FN _print_bool(FILE *f, bool b) { return fputs(b ? hl("yes") : hl("no"), f); }
PRINT_FN _print_str(FILE *f, const char *s) { return fputs(s, f); }
PRINT_FN _print_char(FILE *f, char c) {
#if PRINT_COLOR
#define ESC(e) "\033[35m'\033[34;1m\\" e "\033[0;35m'\033[m"
#else
#define ESC(e) "'\\" e "'"
#endif
    const char *named[256] = {['\n']=ESC("n"), ['\t']=ESC("t"), ['\r']=ESC("r"),
        ['\033']=ESC("e"), ['\v']=ESC("v"), ['\a']=ESC("a"), ['\b']=ESC("b")};
    const char *name = named[(uint8_t)c];
    if (name != NULL)
        return fputs(name, f);
    else if (isprint(c))
        return fprintf(f, hl("'%c'"), c);
    else
        return fprintf(f, ESC("x%02X"), (uint8_t)c);
#undef ESC
}
PRINT_FN _print_quoted(FILE *f, quoted_t quoted) {
#if PRINT_COLOR
#define ESC(e) "\033[34;1m\\" e "\033[0;35m"
#else
#define ESC(e) "\\" e
#endif
    const char *named[256] = {['\n']=ESC("n"), ['\t']=ESC("t"), ['\r']=ESC("r"),
        ['\033']=ESC("e"), ['\v']=ESC("v"), ['\a']=ESC("a"), ['\b']=ESC("b")};
    int printed = fputs("\033[35m\"", f);
    for (const char *p = quoted.str; *p; p++) {
        const char *name = named[(uint8_t)*p];
        if (name != NULL) {
            printed += fputs(name, f);
        } else if (isprint(*p) || (uint8_t)*p > 0x7F) {
            printed += fputc(*p, f);
        } else {
            printed += fprintf(f, ESC("x%02X"), (uint8_t)*p);
        }
    }
    printed += fputs("\"\033[m", f);
#undef ESC
    return printed;
}
PRINT_FN _print_hex(FILE *f, hex_format_t hex) {
    return fprintf(f, hex.no_prefix ? (hex.uppercase ? hl("%0*lX") : hl("%0*lx")) : (hex.uppercase ? hl("0x%0*lX") : hl("%#0*lx")), hex.digits, hex.n);
}
PRINT_FN _print_oct(FILE *f, oct_format_t oct) {
    return fprintf(f, oct.no_prefix ? (oct.uppercase ? hl("%0*lO") : hl("%0*lo")) : (oct.uppercase ? hl("%#0*lO") : hl("%#0*lo")), oct.digits, oct.n);
}
PRINT_FN _print_num_format(FILE *f, num_format_t num) {
    return fprintf(f, hl("%.*lf"), num.precision, num.n);
}
#undef hl

#define _fprint1(f, x) _Generic((x), \
    char*: _print_str, \
    const char*: _print_str, \
    char: _print_char, \
    bool: _print_bool, \
    int64_t: _print_int, \
    int32_t: _print_int, \
    int16_t: _print_int, \
    int8_t: _print_int, \
    uint64_t: _print_uint, \
    uint32_t: _print_uint, \
    uint16_t: _print_uint, \
    uint8_t: _print_uint, \
    float: _print_double, \
    double: _print_double, \
    hex_format_t: _print_hex, \
    oct_format_t: _print_oct, \
    num_format_t: _print_num_format, \
    quoted_t: _print_quoted, \
    void*: _print_pointer)(f, x)

#define _print(x) _n += _fprint1(_printing, x)
#define _fprint(f, ...) ({ FILE *_printing = f; int _n = 0; MAP_LIST(_print, __VA_ARGS__); _n; })
#define fprint(f, ...) _fprint(f, __VA_ARGS__, "\n")
#define print(...) fprint(stdout, __VA_ARGS__)
#define fprint_inline(f, ...) _fprint(f, __VA_ARGS__)
#define print_inline(...) fprint_inline(stdout, __VA_ARGS__)
