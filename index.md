# Esoteric Assembler
An interpreter with assembly like syntax.

## Index
1. [Download links](#download-links)
2. [How to use?](#how-to-use)
3. [The test files](#the-test-files)
4. [Source files](#source-files)
5. [How to code?](#how-to-code)
6. [More notes](#more-notes)
7. [Delimiting characters](#delimiting-characters)
8. [Strings](#strings)
9. [List of all errors](#list-of-all-errors)
10. [List of all warnings](#list-of-all-warnings)
11. [How this interpreter works?](https://github.com/AvirukBasak/esoteric-assembly/blob/main/docs/WORKING.md)
12. [Contribute](https://github.com/AvirukBasak/esoteric-assembly/blob/main/docs/CONTRIBUTING.md)
13. [Code of conduct](https://github.com/AvirukBasak/esoteric-assembly/blob/main/docs/CODE_OF_CONDUCT.md)

## Download links
- Checkout out our latest release [here](https://github.com/AvirukBasak/esoteric-assembly/releases/tag/v2022.5.14). You'll find installation instructions there.
- Download a ZIP file from [here](https://github.com/AvirukBasak/esoteric-assembly/archive/main.zip).
- Or get an executable from [bin](https://github.com/AvirukBasak/esoteric-assembly/tree/main/bin).

## How to use?
- Execute as `asm path/to/file` where `asm` is the command (or the path to the binary executable).
- Enter `asm --help` or `asm -h` for help text.

## The test files
- Folder [Demo](https://github.com/AvirukBasak/esoteric-assembly/tree/main/Demo) contains codes for trial.
- Visit [ASM-Tests](https://github.com/AvirukBasak/ASM-Tests) repository for more test codes.

## Source files
- [libheaders.h](https://github.com/AvirukBasak/esoteric-assembly/blob/main/src/headers/libheaders.h) Declaration of stuffs from library headers.
- [headers.h](https://github.com/AvirukBasak/esoteric-assembly/blob/main/src/headers/headers.h) Declaration of project specific functions, types and macros.
- [global.c](https://github.com/AvirukBasak/esoteric-assembly/blob/main/src/global.c) Global variable declarations.
- [main.c](https://github.com/AvirukBasak/esoteric-assembly/blob/main/src/main.c) Main and CLI argument evaluation functions.
- [misc.c](https://github.com/AvirukBasak/esoteric-assembly/blob/main/src/misc.c) General functions.
- [input.c](https://github.com/AvirukBasak/esoteric-assembly/blob/main/src/input.c) File handling and input functions.
- [output.c](https://github.com/AvirukBasak/esoteric-assembly/blob/main/src/output.c) Contains functions centered around displaying text.
- [interpreter.c](https://github.com/AvirukBasak/esoteric-assembly/blob/main/src/interpreter.c) Interpreter functions.

## How to code?
```
Usage:
  asm [filepath]
  asm [OPTION]
  asm [OPTION] [filepath]

Options:              |
  -h, --help          | Display this message
  -l, --labels        | Display declared labels in tabular form
  -c, --console       | Console mode to execute codes from stdin
  -v, --version       | Display version information
  -d, --dev           | Developer mode to debug interpreter I/O

Operands:             | 
  %x                  | Indicates register 'x'
  &ad                 | Indicates address 'ad' of RAM
  $no                 | Decimal or hex literal 'no'
  ptr                 | RAM data pointer

Escape sequences:     | 
  \"                  | Quote escape
  \t                  | Tab escape
  \n                  | Newline feed escape
  \r                  | Carriage return escape
  \b or \<space>      | Space escape
  \<any character>    | Same as writing without '\'

Registers:            | 
  operand, x = a to d | For storage

RAM:                  | 
  Default 1 cell      | Address 0 i.e. `&0`
  Variable cells      | For storage (32 Bits per cell)

Number of RAM cells can be changed using opcode `ram`.

Mnemonics:            | 
  #!/path/to/asm      | Execute asm script directly from shell
  # comment           | Single line comment
  /*comment*/         | Multi line comment
  Label:              | Labels
  set  op1 op2        | Set op1 to value of op2
  add  op1 op2        | Add op2 to op1 and store in op1
  sub  op1 op2        | Subtract op2 from op1 and store in op1
  mul  op1 op2        | Multiply op1 by op2 and store in op1
  div  op1 op2        | Divide op1 by op2 and store in op1
  mod  op1 op2        | Mod op1 by op2 and store in op1
  ieq  op1 op2        | If operands are equal, set FLAG true
  ige  op1 op2        | If op1 is greater or equal, set FLAG true
  ile  op1 op2        | If op1 is lesser or equal, set FLAG true
  igt  op1 op2        | If op1 is greater, set FLAG true
  ilt  op1 op2        | If op1 is lesser, set FLAG true
  and  op1 op2        | Set op1 to AND value of operands
  or   op1 op2        | Set op1 to OR value of operands
  xor  op1 op2        | Set op1 to XOR value of operands
  com  opn            | Set operand to its 32 bit 1's complement
  jmp  lbl            | Jump (unconditional) to a line after lbl
  jit  lbl            | Jump if (FLAG) true to line after lbl
  jif  lbl            | Jump if (FLAG) false to line after lbl
  call lbl            | Call label as function
  calt lbl            | Call label as function if FLAG true
  calf lbl            | Call label as function if FLAG false
  ram  opn            | Resize total RAM to 'opn' int sized cells
  inp  opn            | Input to operand
  prn  opn            | Print operand as number
  prc  opn            | Print operand as char
  prs  "str"          | Print str as string
  nwl                 | Print new line character
  ret                 | Return from function
  hlp                 | Display help text (only console mode)
  end                 | End execution

NOTE: $20 will be parsed as decimal. For hex, use $0x20. This is
      not necessary for $0a. Same goes for &. Also, 'ptr' can be
      modified as $ptr and used as &ptr.
```

## More notes
1. Interpreter uses a top-to-bottom code interpretation.
2. All codes are read directly from file buffer instead of a string.
3. Jump and function calls are disabled in console mode.
4. Opcode `inv` exists but has been deprecated since `jif` got introduced alongside `jit`.
5. Although both `jumps` and `calls` operate on `labels`, using `ret` after a `jump` will terminate current function.
6. If `ret` is used without a `call`, program will simply quit.
7. Strings are supported as an operand only by `prs`.
6. `hlp` is activated only in `console` mode.
7. Not having `end` causes error `5`.

## Delimiting characters
Tabs, spaces, commas, newlines, carriage returns and 
semicolons are used to delimit parts of the code. In 
reality, these characters are ignored.

## Strings
```
1. "set" %a "$5"
```
The above code is valid. This code sets register `a` to decimal `5`.
<br>
```
2. set "%a $5"
```
The 2nd code is invalid.<br>
Output:
```
ERR> [LINE: 2] Invalid operand '%a $5' for opcode 'set'
```
The trouble is that the delimiting space b/w the two 
operands of `set` doesn't get ignored and the whole 
thing hence becomes a single operand.

## List of all errors
```
LBL    CODE    ERROR

E1     1       Too many or too few CLI arguments
E2     2       No file path entered
E3     3       Invalid option
E4     4       Can't read file
E5     5       Unexpected EOF
E6     6       Invalid new line escape seq
E7     7       Invalid carriage return escape seq
E8     8       Input exceeded limit or too long
E9     9       Invalid register name
E10    10      Invalid RAM address
E11    11      Address out of bounds
E12    12      Invalid literal
E13    13      Can't assign to literal
E14    14      Invalid operand
E15    15      Duplicate labels
E16    16      No such label
E17    17      Can't divide by zero
E18    18      Invalid decimal input
E19    19      Invalid opcode
E20    20      Failed to allocate memory
```

## List of all warnings
``` 
LBL     WARNING

W1      Dev mode
W2      Opcode is disabled
W3      Opcode is deprecated

```
