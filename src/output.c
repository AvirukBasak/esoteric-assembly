// ouputs a string like printf
int print(FILE *ptr, const char *format, ...) {
    if (ptr == stderr) fprintf(stderr, "\r");
    va_list arg;
    int done;
    va_start(arg, format);
    done = vfprintf(ptr, format, arg);
    va_end(arg); 
    return done;
}

// prints asm help text
void printHelp(bool usage) {
    if (usage) {
        print (stdout, "Usage:\n");
        print (stdout, "  asm [filepath]\n  asm [OPTION]\n  asm [OPTION] [filepath]\n\n");
        print (stdout, "Options:\n");
        print (stdout, "  -h, --help          | Display this message\n");
        print (stdout, "  -l, --labels        | Display declared labels in tabular form\n");
        print (stdout, "  -c, --console       | Console mode to execute codes from stdin\n");
        print (stdout, "  -v, --version       | Display version information\n");
        print (stdout, "  -d, --dev           | Developer mode to debug interpreter I/O\n\n");
    }
    print (stdout, "Operands:             | \n");
    print (stdout, "  %%x                  | Indicates register 'x'\n");
    print (stdout, "  &ad                 | Indicates address 'ad' of RAM\n");
    print (stdout, "  $no                 | Decimal or hex literal 'no'\n");
    print (stdout, "  ptr                 | RAM data pointer\n\n");
    print (stdout, "Escape sequences:     | \n");
    print (stdout, "  \\\"                  | Quote escape\n");
    print (stdout, "  \\t                  | Tab escape\n");
    print (stdout, "  \\n                  | Newline feed escape\n");
    print (stdout, "  \\r                  | Carriage return escape\n");
    print (stdout, "  \\b or \\<space>      | Space escape\n");
    print (stdout, "  \\<any character>    | Same as writing without '\\'\n\n");
    print (stdout, "Registers:            | \n");
    print (stdout, "  operand, x = [a, d] | For storage\n\n");
    print (stdout, "RAM:                  | \n");
    print (stdout, "  (32 x 1048576) bits | For storage (4MB)\n\n");
    print (stdout, "Mnemonics:            | \n");
    print (stdout, "  /*comment*/         | Multi line comment\n");
    print (stdout, "  Label:              | Labels\n");
    print (stdout, "  set op1 op2         | Set op1 to value of op2\n");
    print (stdout, "  add op1 op2         | Add op2 to op1 and store in op1\n");
    print (stdout, "  sub op1 op2         | Subtract op2 from op1 and store in op1\n");
    print (stdout, "  mul op1 op2         | Multiply op1 by op2 and store in op1\n");
    print (stdout, "  div op1 op2         | Divide op1 by op1 and store in op1\n");
    print (stdout, "  mod op1 op2         | Mod op1 by op2 and store in op1\n");
    print (stdout, "  ieq op1 op2         | If operands are equal, set FLAG true\n");
    print (stdout, "  ige op1 op2         | If op1 is greater or equal, set FLAG true\n");
    print (stdout, "  ile op1 op2         | If op1 is lesser or equal, set FLAG true\n");
    print (stdout, "  igt op1 op2         | If op1 is greater, set FLAG true\n");
    print (stdout, "  ilt op1 op2         | If op1 is lesser, set FLAG true\n");
    print (stdout, "  and op1 op2         | Set op1 to AND value of operands\n");
    print (stdout, "  or  op1 op2         | Set op1 to OR value of operands\n");
    print (stdout, "  xor op1 op2         | Set op1 to XOR value of operands\n");
    print (stdout, "  com op              | Set operand to its 32 bit 1's complement\n");
    print (stdout, "  jmp lbl             | Jump (unconditional) to a line after lbl\n");
    print (stdout, "  jit lbl             | Jump if (FLAG) true to line after lbl\n");
    print (stdout, "  jif lbl             | Jump if (FLAG) false to line after lbl\n");
    print (stdout, "  call lbl            | Call label as function\n");
    print (stdout, "  calt lbl            | Call label as function if FLAG true\n");
    print (stdout, "  calf lbl            | Call label as function if FLAG false\n");
    print (stdout, "  inp op              | Input to operand\n");
    print (stdout, "  prn op              | Print operand as number\n");
    print (stdout, "  prc op              | Print operand as char\n");
    print (stdout, "  prs \"str\"           | Print str as string\n");
    print (stdout, "  nwl                 | Print new line character\n");
    print (stdout, "  ret                 | Return from function\n");
    print (stdout, "  hlp                 | Display help text (only console mode)\n");
    print (stdout, "  end                 | End execution\n\n");
    print (stdout, "NOTE: $20 will be parsed as decimal. For hex, use $0x20. This is\n");
    print (stdout, "      not necessary for $0a. Same goes for &. Also, 'ptr' can be\n");
    print (stdout, "      modified as $ptr and used as &ptr.\n");
}

/* prints a string as an array of ASCII values
 * @param s The string
 * @param size Length of the string
 */
void prArray(char *s, unsigned int size) {
    if (dev) {
        if (size == 0) {
            print (stdout, "<" BLU "NULL" RST ">\n");
            return;
        }
        else if (size > 8) {
            print (stdout, "<" BLU "LONG TOKEN, HIDDEN" RST ">\n");
            return;
        }
        char c;
        if (s[size - 1] == ':') print (stdout, "<" RED);
        else print (stdout, "<" GRN);
        for (int i = 0; i < size; i++) {
            c = s[i];
            if (c == 9) print (stdout, "\\t");
            else if (c == 10) print (stdout, "\\n");
            else if (c == 13) print (stdout, "\\r");
            else print (stdout, "%c", c);
        }
        print (stdout, RST ">\t\t{" YEL);
        for (int i = 0; i < size; i++) {
            print (stdout, " %d", s[i]);
        }
        print (stdout, RST " }\n");
    }
}
