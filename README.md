# print - a simple C printing library

This project is a single-file header that defines some functions to make it easy to do formatted text
without using printf style specifiers:

```c
print(...) - print text
fprint(file, ...) - print text to file
String(...) - return an allocated string
```

If you put `#define PRINT_COLOR 1` before the import, text will be printed with
terminal colors.
