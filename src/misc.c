/* This file constains:
 | Functions that do not deserve to rest in other files
 | These are general functions or functions used in more than 1 file
 */

#include "headers/headers.h"
#include "headers/global.h"

/* OBSCURE: String was not copied for two reasons:
 |     This process saves the time taken to copy using a loop
 |     This process saves memory of an extra string
 | 
 | Cuts a string from position specified
 | @param str String to be substringed. Note that value
 |            of str is destroyed if to > -1
 | @param from Posn from which string is substringed
 | @param to   Posn till which string is substringed.
 |             Note that to < 0 means to take the whole
 |             string.
 | @return The substringed string
 */
char *substr (char *str, int frm, int to) {
    if (to >= 0) str[to] = '\0';                          // mark string ending using null char
    return &str[frm];                                     // returns address of frm posn so that string data pointer is shifted
}

/* Quits program after closing open file and returning exitcode
 | @param exitcode
 */
void quit (int exitcode) {
    // exit only if console mode is of and exit code belongs to [0, 8] U {20}
    if (!console || (exitcode >= 0 && exitcode <= 8) || exitcode == 20) {
        if (file != NULL) fclose (file);                  // if file is NULL, there's no meed to close it
        exit (exitcode);                                  // exit with code
    }
}

/* OBSCURE: Wrapping malloc () and calloc () is useless as in an event of unavailable
 | memory and hence quit (20), the OS would have already begun killing random
 | processes. This program would be long dead by the time such an error occured.
 | 
 | Allocates memory blocks in the heap
 | @param blocks Number of memory blocks to allocate
 | @param size   Size per block, ie datatype size
 | @param initialize If true, initializes each block with 0 by
 |                   calling calloc (). If false, calls malloc ().
 |                   Note, calloc () is generally a bit slower than
 |                   malloc ().
 | @return address of the base of allocated memory location
 */
void *allocateMem (size_t blocks, size_t size, bool initialize) {
    void *ptr;
    if (initialize) ptr = calloc (blocks, size);
    else ptr = malloc (blocks | size);
    if (ptr == NULL) {
        fprintf (stderr, RED "ERR> " RST "[LINE: %u] Failed to allocate memory\n", lineNo);
        quit (20);
    }
    return ptr;
}

/* OBSCURE: wrapping realloc () is unescessary
 | Reallocates memory blocks in the heap
 | @param ptr    Pointer to the memory block that is to be reallocated
 | @param size   Size of the entire block ie no of (blocks | size of each)
 | @return address of the base of reallocated memory location
 */
void *reallocateMem (void *ptr, size_t size) {
    ptr = realloc (ptr, size);
    if (ptr == NULL) {
        fprintf (stderr, RED "ERR> " RST "[LINE: %u] Failed to allocate memory\n", lineNo);
        quit (20);
    }
    return ptr;
}

/* This method replaces escaped character with their escape
 | sequences.
 | @param str The string to be unescaped
 | @param size The size of str including the NULL character
 */
char *unEscape (char *str) {
    int i = 0, j = 0;
    size_t size = 64;
    char bkp [strlen (str) + 1];
    strcpy (bkp, str);
    char c = bkp[i];
    while (c != '\0' && j <= size) {                      // loop till character is empty
        if (c == '\t') {                                  // if tab, replace with "\t"
            str[j++] = '\\';
            if (j > size) break;
            str[j++] = 't';
        }
        else if (c == '\n') {                             // if LF, replace with "\n"
            str[j++] = '\\';
            if (j > size) break;
            str[j++] = 'n';
        }
        else if (c == '\r') {                             // if CR, replace with "\r"
            str[j++] = '\\';
            if (j > size) break;
            str[j++] = 'r';
        }
        else {                                            // else just copy
            str[j++] = c;                                 // increase j by 1
        }
        c = bkp[++i];                                     // get next character
    }
    if (j > size) {
        str[j-1] = '\0';
        E8c: fprintf (stderr, RED "ERR> " RST "[LINE: %u] Execeeded %lu character limit\n"
                              RED "ERR> " RST "Interpreter ran out of memory while un-escaping string:\n"
                                        "     '%s'...\n", lineNo, size, str);
        quit (8);
    }
    str[j] = '\0';                                        // end string with a null
    return str;
}
