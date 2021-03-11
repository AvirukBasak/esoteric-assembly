# Contribute
Become a contributor to `Esoteric-Assembler`.

### Index
1. [GitHub conventions](#github-conventions)
2. [Documentation](#documentation)
3. [Creating functions](#creating-functions)
4. [Library headers](#library-headers)
5. [Global variables, macros and types](#global-variables-macros-and-types)
6. [Coding conventions](#coding-conventions)
7. [Present ideas](#present-ideas)

### GitHub conventions
1. [README.md](https://github.com/AvirukBasak/Esoteric-Assembler/blob/main/README.md) and [CONTRIBUTE.md](https://github.com/AvirukBasak/Esoteric-Assembler/blob/main/docs/CONTRIBUTE.md) should be updated or checked for updates with every new commit.

### Documentation
1. Most lines should be commented, even if you think its purpose is obvious.
2. Important functions must have a multi-line comment before the declaration, specifying its purpose, arguments, and return value.
3. Single line comments if put on same line must be aligned with all other S.L. comments.
4. Please don't put S.L. comments below your line of code.

### Creating functions
1. No hardcoding. If you realise you've hardcoded over 5 lines over 3 times, you better make a function for it.
2. General functions should rest in [misc.c](https://github.com/AvirukBasak/Esoteric-Assembler/blob/main/src/misc.c).
3. Do not create a function if a similar one exists already in any of the source files.
4. This applies for even library functions.
5. For example, use `allocateMem()` for `malloc()` or `calloc()` and use `reallocateMem()` for `realloc()`.
6. All project functions must be declared in [headers.h](https://github.com/AvirukBasak/Esoteric-Assembler/blob/main/src/headers/headers.h).

### Library headers
1. Apart from library headers whose `non-inclusion` gives a `warning`, no other library header can be included.
2. This means `functions`, `typedef`s and `macro`s of headers viz `stdlib.h`, `stdbool.h`, `string.h`, etc. are to be declared.
3. These declarations must be put only in [libheaders.h](https://github.com/AvirukBasak/Esoteric-Assembler/blob/main/src/headers/libheaders.h) file.
4. Note that `stdio.h` is included as its non-inclusion gives warnings even if its contents are declared in [libheaders.h](https://github.com/AvirukBasak/Esoteric-Assembler/blob/main/src/headers/libheaders.h) file.
5. This is done to exclude extra stuff that gets included if a library header is included.
6. This in turn `reduces size` of object file and often the binary executable.


### Global variables, macros and types
1. All `macro`s, `typedef`s and `library headers` (standard or third-party) must be included only in [headers.h](https://github.com/AvirukBasak/Esoteric-Assembler/blob/main/src/headers.h).
2. No library is to be included in other files. [main.c](https://github.com/AvirukBasak/Esoteric-Assembler/blob/main/src/main.c) can only have the project specific header files.
3. All `global variables` must be defined only in [global.c](https://github.com/AvirukBasak/Esoteric-Assembler/blob/main/src/global.c).
4. Every variable of an inbuilt type must be initialised in [main.c](https://github.com/AvirukBasak/Esoteric-Assembler/blob/main/src/main.c) using function `initialize()`.

### Coding conventions
1. All identifier name should clearly convey its purpose.
2. Use of `scanf()` and `gets()` for input is strictly forbidden regardless of data type.
3. Use `scanStr()` defined in [input.c](https://github.com/AvirukBasak/Esoteric-Assembler/blob/main/src/input.c) in association with `strtol()` for integer inputs.
4. In the case of using an obscure technique, mention why it was used, precautions (and if possible, the alternative) in comments with the word `OBSCURE:` when starting the comment.
5. `OBSCURE` techniques are meant to be tricky to handle, even risky. Bug squashing in these can be very difficult.

### Present ideas
1. If you wanna present an idea (feature requests) or report a bug, create an issue with the proper template.
