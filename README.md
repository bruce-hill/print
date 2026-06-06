# print - a simple C printing library

This project is a single-file header that defines some functions to make it easy to do formatted text
without using printf style specifiers. For example:

```c
int x = 42;
print("My number is ", x, " and that's ", hex(x), " in hex");
```

## API

```c
print(...) // print text with a trailing newline
print_inline(...) // print text without a trailing newline

fprint(file, ...) // print text to a file with a trailing newline
fprint_inline(file, ...) // print text to a file without a trailing newline
```

If you put `#define PRINT_COLOR 1` before the import, text will be printed with
terminal colors.

## Allocating strings

Additionally, if you are using the Boehm Garbage Collector, you can use
`#include "gcprint.h"` to print to an allocated string.

```c
char *s = String(...); // return an allocated string
```
