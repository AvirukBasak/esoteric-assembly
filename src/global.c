/* This file contains:
 * Global variables
 * Every macro
 * Every pre-processor to std/third-party libraries
 * Any function used in multiple source files
 * Any function that is fit for none of the other files
 */

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
bool console;                    // console mode
bool prompt;                     // if true, prints asm> in console mode
bool input;                      // marks input, so that lineNo isn't upadated
bool printLbl;                   // labels mode
bool dev;                        // dev mode

FILE *file;                      // input file pointer

bool FLAG;                       // boolean flag
char opcode[65];                 // opcode of operation
int a, b, c, d;                  // registers
int RAM[1048576];                // RAM 4MB

int dataPtr;                     // RAM data ptr
int intBuffer;                   // this global variable stores numeric inputs (after '#')
int garbageBuffer;               // garbage buffer to initialise selOprnd() pointer on event of an invalid register or address value

unsigned int lineNo;             // lineNo counter
unsigned long int steps;         // steps counter

/* OBSCURE: String was not copied for two reasons:
 *     This process saves the time taken to copy using a loop
 *     This process saves memory of an extra string
 *
 * Cuts a string from position specified
 * @param str String to be substringed. Note that value
 *            of str is destroyed if to > -1
 * @param from Posn from which string is substringed
 * @param to   Posn till which string is substringed.
 *             Note that to < 0 means to take the whole
 *             string.
 * @return The substringed string
 */
char *substr(char *str, int frm, int to) {
    if (to >= 0) str[to] = '\0'; // mark string ending using null char
    return &str[frm];            // returns address of frm posn so that string data pointer is shifted
}

// prints asm help text
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

/* Quits program after closing open file and returning exitcode
 * @param exitcode
 */
void quit(int exitcode) {
    // exit only if console mode is of and exit code belongs to [0, 8] U {20}
    if (!console || (exitcode >= 0 && exitcode <= 8) || exitcode == 20) {
        if (file != NULL) fclose(file);            // if file is NULL, there's no meed to close it
        exit(exitcode);                            // exit with code
    }
}

/* OBSCURE: Wrapping malloc() and calloc() is useless as in an event of unavailable
 * memory and hence quit(20), the OS would have already begun killing random
 * processes. This program would be long dead by the time such an error occured.
 *
 * Allocates memory blocks in the heap
 * @param blocks Number of memory blocks to allocate
 * @param size   Size per block, ie datatype size
 * @param initialize If true, initializes each block with 0 by
 *                   calling calloc(). If false, calls malloc().
 *                   Note, calloc() is generally a bit slower than
 *                   malloc().
 * @return address of the base of allocated memory location
 */
void *allocateMem(size_t blocks, size_t size, bool initialize) {
    void *ptr;
    if (initialize) ptr = calloc(blocks, size);
    else ptr = malloc(blocks * size);
    if (ptr == NULL) {
        fprintf(stderr, RED "ERR> " RST "Failed to allocate memory\n");
        quit(20);
    }
    return ptr;
}

/* OBSCURE: wrapping realloc() is unescessary
 * Reallocates memory blocks in the heap
 * @param ptr    Pointer to the memory block that is to be reallocated
 * @param size   Size of the entire block ie no of (blocks * size of each)
 * @return address of the base of reallocated memory location
 */
void *reallocateMem(void *ptr, size_t size) {
    ptr = realloc(ptr, size);
    if (ptr == NULL) {
        fprintf(stderr, RED "ERR> " RST "Failed to allocate memory\n");
        quit(20);
    }
    return ptr;
}

/* prints a string as an array of ASCII values
 * @param s The string
 * @param size Length of the string
 */
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

/* OBSCURE WARNING: value returned by unEscape() must be used immediately or before 
 * doing another unEscape() or memory allocation as unEscape() returns a dangling
 * pointer. This is done to free up space so that the caller function can be bothered
 * only about implementation.
 * @param str The string to be unescaped
 * @return The unescaped string
 */
char *unEscape(char *str) {
    
    /* allocate memory to out so that it is existing in heap
     * outside the normal function stack frame. By doing so, it
     * can be accessed globally using the pointer.
     */
    char *out = allocateMem(1, sizeof(char), false);
    int iSize = 1, i = 0, j = 0;
    char c = str[i];
    while (c != '\0') {                  // loop till character is empty
        if (c == 9) {                    // if tab, replace with "\t"
            out[j++] = '\\';
            out = reallocateMem(out, (++iSize) * sizeof(char));
            out[j++] = 't';
            out = reallocateMem(out, (++iSize) * sizeof(char));
        }
        else if (c == 10) {              // if LF, replace with "\n"
            out[j++] = '\\';
            out = reallocateMem(out, (++iSize) * sizeof(char));
            out[j++] = 'n';
            out = reallocateMem(out, (++iSize) * sizeof(char));
        }
        else if (c == 13) {              // if CR, replace with "\r"
            out[j++] = '\\';
            out = reallocateMem(out, (++iSize) * sizeof(char));
            out[j++] = 'r';
            out = reallocateMem(out, (++iSize) * sizeof(char));
        }
        else {                           // else just copy
            out[j++] = c;                // increase j by 1
            out = reallocateMem(out, (++iSize) * sizeof(char));
        }
        c = str[++i];                    // get next character
    }
    out[j] = '\0';                       // end string with a null
    
    /* OBSCURE: free memory of out before returning.
     * this only marks the memory location of out unallocated
     * but doesn't overwrite it with 0. Be warned that this means
     * if out is referenced after doing another memory allocation,
     * the location of out may get overwritten by the newly allocated
     * value as it is already marked free
     */
    free(out);
    return out;
}
