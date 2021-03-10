// prints asm help text
void printHelp (bool usage) {
    if (usage) {
        printf ("Usage:\n");
        printf ("  asm [filepath]\n  asm [OPTION]\n  asm [OPTION] [filepath]\n\n");
        printf ("Options:\n");
        printf ("  -h, --help          | Display this message\n");
        printf ("  -l, --labels        | Display declared labels in tabular form\n");
        printf ("  -c, --console       | Console mode to execute codes from stdin\n");
        printf ("  -v, --version       | Display version information\n");
        printf ("  -d, --dev           | Developer mode to debug interpreter I/O\n\n");
    }
    printf ("Operands:             | \n");
    printf ("  %%x                  | Indicates register 'x'\n");
    printf ("  &ad                 | Indicates address 'ad' of RAM\n");
    printf ("  $no                 | Decimal or hex literal 'no'\n");
    printf ("  ptr                 | RAM data pointer\n\n");
    printf ("Escape sequences:     | \n");
    printf ("  \\\"                  | Quote escape\n");
    printf ("  \\t                  | Tab escape\n");
    printf ("  \\n                  | Newline feed escape\n");
    printf ("  \\r                  | Carriage return escape\n");
    printf ("  \\b or \\<space>      | Space escape\n");
    printf ("  \\<any character>    | Same as writing without '\\'\n\n");
    printf ("Registers:            | \n");
    printf ("  operand, x = [a, d] | For storage\n\n");
    printf ("RAM:                  | \n");
    printf ("  (32 x 1048576) bits | For storage (4MB)\n\n");
    printf ("Mnemonics:            | \n");
    printf ("  /*comment*/         | Multi line comment\n");
    printf ("  Label:              | Labels\n");
    printf ("  set op1 op2         | Set op1 to value of op2\n");
    printf ("  add op1 op2         | Add op2 to op1 and store in op1\n");
    printf ("  sub op1 op2         | Subtract op2 from op1 and store in op1\n");
    printf ("  mul op1 op2         | Multiply op1 by op2 and store in op1\n");
    printf ("  div op1 op2         | Divide op1 by op1 and store in op1\n");
    printf ("  mod op1 op2         | Mod op1 by op2 and store in op1\n");
    printf ("  ieq op1 op2         | If operands are equal, set FLAG true\n");
    printf ("  ige op1 op2         | If op1 is greater or equal, set FLAG true\n");
    printf ("  ile op1 op2         | If op1 is lesser or equal, set FLAG true\n");
    printf ("  igt op1 op2         | If op1 is greater, set FLAG true\n");
    printf ("  ilt op1 op2         | If op1 is lesser, set FLAG true\n");
    printf ("  and op1 op2         | Set op1 to AND value of operands\n");
    printf ("  or  op1 op2         | Set op1 to OR value of operands\n");
    printf ("  xor op1 op2         | Set op1 to XOR value of operands\n");
    printf ("  com op              | Set operand to its 32 bit 1's complement\n");
    printf ("  jmp lbl             | Jump (unconditional) to a line after lbl\n");
    printf ("  jit lbl             | Jump if (FLAG) true to line after lbl\n");
    printf ("  jif lbl             | Jump if (FLAG) false to line after lbl\n");
    printf ("  call lbl            | Call label as function\n");
    printf ("  calt lbl            | Call label as function if FLAG true\n");
    printf ("  calf lbl            | Call label as function if FLAG false\n");
    printf ("  inp op              | Input to operand\n");
    printf ("  prn op              | Print operand as number\n");
    printf ("  prc op              | Print operand as char\n");
    printf ("  prs \"str\"           | Print str as string\n");
    printf ("  nwl                 | Print new line character\n");
    printf ("  ret                 | Return from function\n");
    printf ("  hlp                 | Display help text (only console mode)\n");
    printf ("  end                 | End execution\n\n");
    printf ("NOTE: $20 will be parsed as decimal. For hex, use $0x20. This is\n");
    printf ("      not necessary for $0a. Same goes for &. Also, 'ptr' can be\n");
    printf ("      modified as $ptr and used as &ptr.\n");
}

/* prints a string as an array of ASCII values
 * @param s The string
 * @param size Length of the string
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
            if (c == 9) printf ("\\t");
            else if (c == 10) printf ("\\n");
            else if (c == 13) printf ("\\r");
            else printf ("%c", c);
        }
        printf (RST ">\t\t{" YEL);
        for (int i = 0; i < size; i++) {
            printf (" %d", s[i]);
        }
        printf (RST " }\n");
    }
}
