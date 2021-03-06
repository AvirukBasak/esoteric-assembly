/* This file contains:
 * Global variables
 * Any function used in multiple source files
 * Any function that is fit for none of the other files
 */

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
char lastChar;                   // last read char from file

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
        print (stderr, RED "ERR> " RST "Failed to allocate memory\n");
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
        print (stderr, RED "ERR> " RST "Failed to allocate memory\n");
        quit(20);
    }
    return ptr;
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
