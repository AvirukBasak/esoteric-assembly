# Contribute
Become a contributor to `Esoteric-Assembler`.

## Index
1. [GitHub conventions](#github-conventions)
2. [Documentation](#documentation)
3. [Creating functions](#creating-functions)
4. [Global variables, macros and types](#global-variables-macros-and-types)

## GitHub conventions
1. All identifier name should clearly convey its purpose.
2. Any new function description should be added to [CONTRIBUTE.md](https://github.com/AvirukBasak/Esoteric-Assembler/blob/main/docs/CONTRIBUTE.md) by the coder.
3. [README.md](https://github.com/AvirukBasak/Esoteric-Assembler/blob/main/README.md) and [CONTRIBUTE.md](https://github.com/AvirukBasak/Esoteric-Assembler/blob/main/docs/CONTRIBUTE.md) should be updated or checked for updates with every new commit.

## Documentation
1. Most lines that don't follow a convention of `C` coding must be commented, even if you think its purpose is obvious.
2. Important functions must have a multi-line comment before the declaration, specifying its purpose, arguments, and return value.
3. Single line comments should only be place after a line and must be aligned with all other S.L. comments.
4. Please don't put S.L. comments below your line of code.

## Creating functions
1. No hardcoding. If you realise you've hardcoded over 5 lines over 3 times, you better make a function for it.
2. Any function that is used by more than one file should rest in [global.c](https://github.com/AvirukBasak/Esoteric-Assembler/blob/main/src/global.c).
3. Do not create a function if it exists already in any of the source files.
4. This applies for even library functions.
5. For example, use `allocateMem()` for `malloc()` or `calloc()` and use `reallocateMem()` for `realloc()`.

## Global variables, macros and types
1. All libraries (standard or third-party) must be included only in [global.c](https://github.com/AvirukBasak/Esoteric-Assembler/blob/main/src/global.c).
2. No library is to be included in other files. [main.c](https://github.com/AvirukBasak/Esoteric-Assembler/blob/main/src/main.c) can only have the source files.
3. All `MACROS`, `types` and `global variables` must be defined only in [global.c](https://github.com/AvirukBasak/Esoteric-Assembler/blob/main/src/global.c).
4. Every variable of an inbuilt type must be initialised in [main.c](https://github.com/AvirukBasak/Esoteric-Assembler/blob/main/src/main.c) in function `initialize`.

## Other conventions
1. Use of `scanf()` and `gets()` for input is strictly forbidden.
2. Use `scanStr()` in [input.c](https://github.com/AvirukBasak/Esoteric-Assembler/blob/main/src/input.c) in association with `strtol()` for integer inputs.
3. You cannot immediately use any dynamic memory allocation method (or any method that uses it) after using `unEscape()` in [global.c](https://github.com/AvirukBasak/Esoteric-Assembler/blob/main/src/global.c).
4. You must use up or backup the value returned by `unEscape()` before doing so as it returns a `dangling pointer`.
5. This is done to free up memory so that `free()` is not used after every call to `unEscape()`.
