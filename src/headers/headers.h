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
#   ifndef _WIN32
#      define YEL     "\033[33m"
#      define RED     "\033[31m"
#      define GRN     "\033[32m"
#      define BLU     "\033[34m"
#      define INV     "\033[7;1m"
#      define RST     "\033[0m"
#   endif
#
#   ifdef _WIN32
#      define YEL     ""
#      define RED     ""
#      define GRN     ""
#      define BLU     ""
#      define INV     ""
#      define RST     ""
#   endif
#
#   ifdef EOF
#       undef EOF
#   endif
#   define EOF(c) (c == -1 || c == 255)

    struct TABLE {
        int cur;
        unsigned int line;
        char lbl[65];
    };

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
    void eof ();
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
