#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define YEL "\e[33m"
#define RED "\e[31m"
#define GRN "\e[32m"
#define BLU "\e[34m"
#define INV "\e[7;1m"
#define RST "\e[0m"

struct TABLE {
    int cur;
    unsigned int line;
    char lbl[65];
};
struct TABLE *tab;               // this will allow us to access any label based on its cursor posn
int tabIndex;                    // index of last label in table
bool console;
bool printLbl;
bool dev;

FILE *file;                      // input file pointer

bool FLAG;                       // boolean flag
char opcode[65];                 // opcode of operation
int a, b, c, d;                  // registers
int RAM[1048576];                // RAM 4MB

int dataPtr;                     // RAM data ptr
int intBuffer;                   // this global variable stores numeric inputs (after '#')
int garbageBuffer;               // garbage buffer to initialise return pointer on event of an invalid register or address value

unsigned int lineNo;             // lineNo counter
unsigned long int steps;         // steps counter

// cuts a string from position specified
char *substr(char *str, int frm, int to) {
    if (to >= 0) str[to] = '\0';
    return &str[frm];            // returns address of frm posn so that string data pointer is shifted
}

void printHelp() {
    printf("Usage:\n");
    printf("  asm [filepath]\n  asm [OPTION]\n  asm [OPTION] [filepath]\n\n");
    printf("Options:\n");
    printf("  -h, --help          | Display this message\n");
    printf("  -l, --labels        | Display declared labels in tabular form\n");
    printf("  -c, --console       | Console mode to execute codes from stdin\n");
    printf("  -v, --version       | Display version information\n");
    printf("  -d, --dev           | Developer mode to debug interpreter I/O\n\n");
    printf("Operands:             | \n");
    printf("  %%x                  | Indicates register 'x'\n");
    printf("  &ad                 | Indicates address 'ad' of RAM\n");
    printf("  $no                 | Decimal or hex literal 'no'\n");
    printf("  ptr                 | RAM data pointer\n\n");
    printf("Escape sequences:     | \n");
    printf("  \\\"                  | Quote escape\n");
    printf("  \\t                  | Tab escape\n");
    printf("  \\n                  | Newline feed escape\n");
    printf("  \\r                  | Carriage return escape\n");
    printf("  \\b or \\<space>      | Space escape\n");
    printf("  \\<any character>    | Same as writing without '\\'\n\n");
    printf("Registers:            | \n");
    printf("  operand, x = [a, d] | For storage\n\n");
    printf("RAM:                  | \n");
    printf("  (32 x 1048576) bits | For storage (4MB)\n\n");
    printf("Mnemonics:            | \n");
    printf("  /*comment*/         | Multi line comment\n");
    printf("  Label:              | Labels\n");
    printf("  set op1 op2         | Set op1 to value of op2\n");
    printf("  add op1 op2         | Add op2 to op1 and store in op1\n");
    printf("  sub op1 op2         | Subtract op2 from op1 and store in op1\n");
    printf("  mul op1 op2         | Multiply op1 by op2 and store in op1\n");
    printf("  div op1 op2         | Divide op1 by op1 and store in op1\n");
    printf("  mod op1 op2         | Mod op1 by op2 and store in op1\n");
    printf("  ieq op1 op2         | If operands are equal, set FLAG true\n");
    printf("  ige op1 op2         | If op1 is greater or equal, set FLAG true\n");
    printf("  ile op1 op2         | If op1 is lesser or equal, set FLAG true\n");
    printf("  igt op1 op2         | If op1 is greater, set FLAG true\n");
    printf("  ilt op1 op2         | If op1 is lesser, set FLAG true\n");
    printf("  and op1 op2         | Set op1 to AND value of operands\n");
    printf("  or  op1 op2         | Set op1 to OR value of operands\n");
    printf("  xor op1 op2         | Set op1 to XOR value of operands\n");
    printf("  com op              | Set operand to its 32 bit 1's complement\n");
    printf("  jmp lbl             | Jump (unconditional) to a line after lbl\n");
    printf("  jit lbl             | Jump if (FLAG) true to line after lbl\n");
    printf("  jif lbl             | Jump if (FLAG) false to line after lbl\n");
    printf("  call lbl            | Call label as function\n");
    printf("  calt lbl            | Call label as function if FLAG true\n");
    printf("  calf lbl            | Call label as function if FLAG false\n");
    printf("  inp op              | Input to operand\n");
    printf("  prn op              | Print operand as number\n");
    printf("  prc op              | Print operand as char\n");
    printf("  prs \"str\"           | Print str as string\n");
    printf("  nwl                 | Print new line character\n");
    printf("  ret                 | Return from function\n");
    printf("  end                 | End execution\n\n");
    printf("NOTE: $20 will be parsed as decimal. For hex, use $0x20. This is\n");
    printf("      not necessary for $0a. Same goes for &. Also, 'ptr' can be\n");
    printf("      modified as $ptr and used as &ptr.\n");
}

void prArray(char *s, unsigned int size) {
    if (dev) {
        if (size == 0) {
            printf("<" BLU "NULL" RST ">\n");
            return;
        }
        else if (size > 8) {
            printf("<" BLU "LONG TOKEN, HIDDEN" RST ">\n");
            return;
        }
        char c;
        if (s[size - 1] == ':') printf("<" RED);
        else printf("<" GRN);
        for (int i = 0; i < size; i++) {
            c = s[i];
            if (c == 9) printf("\\t");
            else if (c == 10) printf("\\n");
            else if (c == 13) printf("\\r");
            else printf("%c", c);
        }
        printf(RST ">\t\t{" YEL);
        for (int i = 0; i < size; i++) {
            printf(" %d", s[i]);
        }
        printf(RST " }\n");
    }
}

char *unEscape(char *str) {
    char *out = calloc(1, sizeof(char));
    int iSize = 1, i = 0, j = 0;
    char c = str[i];
    while (c != '\0') {
        if (c == 9) {
            out[j++] = '\\';
            out = realloc(out, (++iSize) * sizeof(char));
            out[j++] = 't';
            out = realloc(out, (++iSize) * sizeof(char));
        }
        else if (c == 10) {
            out[j++] = '\\';
            out = realloc(out, (++iSize) * sizeof(char));
            out[j++] = 'n';
            out = realloc(out, (++iSize) * sizeof(char));
        }
        else if (c == 13) {
            out[j++] = '\\';
            out = realloc(out, (++iSize) * sizeof(char));
            out[j++] = 'r';
            out = realloc(out, (++iSize) * sizeof(char));
        }
        else {
            out[j++] = c;
            out = realloc(out, (++iSize) * sizeof(char));
        }
        c = str[++i];
    }
    out[j] = '\0';
    return out;
}

void quit(int exitcode) {
    if (!console || exitcode == 5) {
        if (file != NULL) fclose(file);
        exit(exitcode);
    }
}
