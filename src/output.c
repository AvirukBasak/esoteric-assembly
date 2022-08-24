/* This file contains:
 | Functions related to console output
 */

#include "headers/headers.h"
#include "headers/global.h"

// prints asm help text
void printHelp (bool usage) {
    if (usage) printf (
        "Usage:\n"
        "  asm [filepath]\n"
        "  asm [OPTION]\n"
        "  asm [OPTION] [filepath]\n\n"
        "Options:              |\n"
        "  -h, --help          | Display this message\n"
        "  -l, --labels        | Display declared labels in tabular form\n"
        "  -c, --console       | Console mode to execute codes from stdin\n"
        "  -v, --version       | Display version information\n"
        "  -d, --dev           | Developer mode to debug interpreter I/O\n\n");
    printf (
        "Operands:             | \n"
        "  %%x                  | Indicates register 'x'\n"
        "  &ad                 | Indicates address 'ad' of RAM\n"
        "  $no                 | Decimal or hex literal 'no'\n"
        "  ptr                 | RAM data pointer\n\n"
        "Escape sequences:     | \n"
        "  \\\"                  | Quote escape\n"
        "  \\t                  | Tab escape\n"
        "  \\n                  | Newline feed escape\n"
        "  \\r                  | Carriage return escape\n"
        "  \\b or \\<space>      | Space escape\n"
        "  \\<any character>    | Same as writing without '\\'\n\n"
        "Registers:            | \n"
        "  operand, x = [a, d] | For storage\n\n"
        "RAM:                  | \n"
        "  Default 1 cell      | Address 0 i.e. `&0`\n"
        "  Variable cells      | For storage (32 Bits per cell)\n\n"
        "Number of RAM cells can be changed using opcode `ram`.\n\n"
        "Mnemonics:            | \n"
        "  #!/path/to/asm      | Execute asm script directly from shell\n"
        "  # comment           | Single line comment\n"
        "  /* comment */       | Multi line comment\n"
        "  Label:              | Labels\n"
        "  set  op1 op2        | Set op1 to value of op2\n"
        "  add  op1 op2        | Add op2 to op1 and store in op1\n"
        "  sub  op1 op2        | Subtract op2 from op1 and store in op1\n"
        "  mul  op1 op2        | Multiply op1 by op2 and store in op1\n"
        "  div  op1 op2        | Divide op1 by op1 and store in op1\n"
        "  mod  op1 op2        | Mod op1 by op2 and store in op1\n"
        "  ieq  op1 op2        | If operands are equal, set FLAG true\n"
        "  ige  op1 op2        | If op1 is greater or equal, set FLAG true\n"
        "  ile  op1 op2        | If op1 is lesser or equal, set FLAG true\n"
        "  igt  op1 op2        | If op1 is greater, set FLAG true\n"
        "  ilt  op1 op2        | If op1 is lesser, set FLAG true\n"
        "  and  op1 op2        | Set op1 to AND value of operands\n"
        "  or   op1 op2        | Set op1 to OR value of operands\n"
        "  xor  op1 op2        | Set op1 to XOR value of operands\n"
        "  com  op             | Set operand to its 32 bit 1's complement\n"
        "  jmp  lbl            | Jump (unconditional) to a line after lbl\n"
        "  jit  lbl            | Jump if (FLAG) true to line after lbl\n"
        "  jif  lbl            | Jump if (FLAG) false to line after lbl\n"
        "  call lbl            | Call label as function\n"
        "  calt lbl            | Call label as function if FLAG true\n"
        "  calf lbl            | Call label as function if FLAG false\n"
        "  ram  opn            | Resize total RAM to 'opn' int cells\n"
        "  inp  opn            | Input to operand\n"
        "  prn  opn            | Print operand as number\n"
        "  prc  opn            | Print operand as char\n"
        "  prs  \"str\"          | Print str as string\n"
        "  nwl                 | Print new line character\n"
        "  ret                 | Return from function\n"
        "  hlp                 | Display help text (only console mode)\n"
        "  end                 | End execution\n\n"
        "NOTE: $20 will be parsed as decimal. For hex, use $0x20. This is\n"
        "      not necessary for $0a. Same goes for &. Also, 'ptr' can be\n"
        "      modified as $ptr and used as &ptr.\n");
}

/* prints a string as an array of ASCII values
 | @param s The string
 | @param size Length of the string
 */
void prArray (char *s, unsigned int size) {
    if (dev) {
        if (size == 0) {
            printf ("<" BLU "NULL" RST ">\n");
            return;
        }
        else if (size > 8) {
            printf ("<" BLU "LONG TOKEN, HIDDEN" RST ">\n");
            return;
        }
        char c;
        if (s[size - 1] == ':') printf ("<" RED);
        else printf ("<" GRN);
        for (int i = 0; i < size; i++) {
            c = s[i];
            if (c == '\t') printf ("\\t");
            else if (c == '\n') printf ("\\n");
            else if (c == '\r') printf ("\\r");
            else printf ("%c", c);
        }
        printf (RST ">\t\t{" YEL);
        for (int i = 0; i < size; i++) {
            printf (" %d", s[i]);
        }
        printf (RST " }\n");
    }
}
