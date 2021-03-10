/* This header contains
 | All function declarations from each and every source file
 | Necessary inclusions of std/third-party libraries
 | Every macro
 | 
 | Un-included libraries have their stuffs declared in libheader.h
 | NOTE that stdio.h is compulsory unless you want to suffer
 |      warnings during compilation. Another reason of keeping
 |      stdio.h was the struct FILE couldn't be defined in
 |      libheaders.h file.
 */

# ifndef HEADERS
#   define HEADERS
#
#   include <stdio.h>
#   include "libheaders.h"
#
#   define YEL "\e[33m"
#   define RED "\e[31m"
#   define GRN "\e[32m"
#   define BLU "\e[34m"
#   define INV "\e[7;1m"
#   define RST "\e[0m"
#
#   ifdef EOF
#       undef EOF
#   endif
#   define EOF(c) (c == -1 || c == 255)
#   
    // main.c
    void initialize ();
    void evalOptions (int argsc, char **args, int indx);
    int main (int argsc, char *args[]);
    
    // misc.c
    char *substr (char *str, int frm, int to);
    void quit (int exitcode);
    void *allocateMem (size_t blocks, size_t size, bool initialize);
    void *reallocateMem (void *ptr, size_t size);
    char *unEscape (char *str);
    
    // input.c
    void openFile (char *path);
    void eof (FILE *ptr);
    signed short int readC (FILE *ptr);
    bool isStrayChar (signed short int c);
    void scanStr (FILE *ptr, char *str, unsigned int size);

    // output.c
    void printHelp (bool usage);
    void prArray (char *s, unsigned int size);
    
    // interpreter.c
    bool ifReadOperandsAreGarbage (char *oprnd1, int size1, char *oprnd2, int size2);
    int *selOprnd (char *oprnd, bool w);
    void genJmpTable ();
    void gotoLabel (char *label);
    void evaluate (char *opcode);
    void interpret ();
    
# endif
