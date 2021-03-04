# Contribute
Become a contributor to `Esoteric-Assembler`.

## Index
1. [Coding conventions](coding-conventions)
2. [Documentation](documentation)
3. [Creating functions](creating-functions)
4. [Global variables, macros and types](global-variables-macros-and-types)
5. [Source code functions](source-code-functions)

## Coding conventions

### Documentation
1. Most lines that don't follow a convention of `C` coding must be commented, even if you think its purpose is obvious.
2. Every function must have a multi-line comment before the declaration, specifying its purpose, arguments, and return value.
3. Single line comments should only be place after a line and must be aligned with all other S.L. comments.
4. Please don't put S.L. comments above or below your line of code.

### Creating functions
1. No hardcoding. If you realise you've hardcoded over 5 lines over 3 times, you better make a function for it.
2. Any function that is used by more than one file should rest in [global.c]().
3. Do not create a function if it exists already in any of the source files.

### Global variables, macros and types
1. All libraries (standard or third-party) must be included only in [global.c]().
2. No library is to be included in other files. [main.c]() can only have the source files.
3. All `MACROS`, `types` and `global variables` must be defined only in [global.c]().
4. Every variable of an inbuilt type must be initialised in [main.c]() in function `initialize`.

## Source code functions

### [global.c]()
1. `char *substr(char *str, int frm, int to)` Substring `str` from `frm` till `to`.
2. `void printHelp()` Prints the help text you get with `--help` argument.
3. `void quit(int exitcode)` Quits the program whith the exit code after closing any files if open.
4. 




