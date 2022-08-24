/* This file contains:
 | Functions related to console input
 */

#include "headers/headers.h"
#include "headers/global.h"

// opens a file
void openFile (char *path) {
    file = fopen (path, "rb");                                        // r means read mode
    if (file == NULL) {                                               // if NULL, means file not read
        E4: fprintf (stderr, RED "ERR> " RST "Can't read file '%s'\n"
                                 "     Check if file path exists and has read permission\n", path);
        quit (4);
    }
}

//handles EOF encounters
void eof () {
    E5b: fprintf (stderr, RED "ERR> " RST "[LINE: %u] Unexpected EOF encounter\n", lineNo);
    quit (5);
}

signed short int readC (FILE *ptr) {
    signed short int c;
    c = fgetc (ptr);
    if (ptr == stdin && EOF(c)) {
        c = '\n';
        printf ("\n");
        quit (0);
    }
    return c;
}

bool isStrayChar (signed short int c) {
    return (c == ' ' || 
            c == '\t'||
            c == ',' || 
            c == ';' ||
            c == '#' );
}

/* All this to incorporate line numbers counting
 | It scans for strings and while doing so, if it finds 
 | a \n, it adds 1 to lineNo, and ignores spaces and tabs.
 | If EOF (int value -1) is found, it halts
 */
void scanStr (FILE *ptr, char *str, unsigned int size) {
    int i = 0;
    signed short int c;
    bool quoted = false;
    bool escaped = false;
    
    /* This loop is for cases where there's stray spaces, indents, 
     | newlines before a string. It is to traverse thru those till 
     | a proper character is spotted.
     | 32 is space.
     | '\r' is carriage return '\r' (used in Windows). In Windows, 
     | '\r' is followed by '\n'. Although UNIX like OS don't use 
     | '\r', prior to OS X, mac used '\r' only, not '\n'.
     | '\n' is '\n' or newline character.
     | '\t' is '\t' or tab
     */
     // VERY IMPORTANT WARNING: readC () will NOT update reader cursor once it reaches EOF.
    while ( (c = readC (ptr)) == '\r' || c == '\n' || c == '/' || isStrayChar (c) || EOF(c)) {
         
        if (c == '\n') {                                               // updates lineNo if newline is spotted
            if (!input) {
                ++lineNo;                                              // updates line no
                if (console) printf (GRN "asm> " RST);                 // asm> code prompt
            }
            else {
                if (dev || console) printf (YEL "inp> " RST);          // inp> input prompt
            }
            c = readC (ptr);                                           // gets next character
            if (EOF(c)) eof ();                                        // quit if eof
            else if (c != '\r') ungetc (c, ptr);                       // else if c is not '\r', bring read cursor back at location of c 
        }
        
        else if (c == '\r') {
            if (!input) ++lineNo;                                      // updates line no
            c = readC (ptr);                                           // gets next character
            if (EOF(c)) eof ();                                        // quit if eof
            else if (c != '\n') ungetc (c, ptr);                       // else if c is not '\n', bring read cursor back at location of c
        }
        
        else if (c == '#') {                                           // checks if S.L. comment begins
            do {
                c = readC (ptr);                                       // read comment character but ignore it
                if (c == '\n') {
                    if (console) printf (GRN "asm> " RST);             // asm> code prompt
                    c = readC (ptr);                                   // gets the next character
                    if (EOF(c)) eof ();                                // quit if eof
                    else if (c != '\r') ungetc (c, ptr);               // else if c is not '\r', bring read cursor back at location of c
                    if (!input) ++lineNo;                              // update lineNo if 'input' flag isn't on. this flag indicates 'inp' opcode
                    break;
                }
                else if (c == '\r') {
                    c = readC (ptr);                                   // gets next character
                    if (EOF(c)) eof ();                                // quit if eof
                    else if (c != '\n') ungetc (c, ptr);               // else if c is not '\n', bring read cursor back at location of c
                    if (!input) ++lineNo;                              // updates line no
                    break;
                }
            } while (true);
        }
        
        else if (c == '/') {                                           // checks if comment begins
            if ( (c = readC (ptr)) != '*') ungetc (c, ptr);            // comment not begins, unget the *
            else {
                do {
                    c = readC (ptr);                                   // read comment character but ignore it
                    if (c == '\n') {
                        if (console) printf ("com> ");
                        if ( (c = readC (ptr)) != '\r') ungetc (c, ptr);
                        if (!input) ++lineNo;                          // update lineNo if 'input' flag isn't on. this flag indicates 'inp' opcode
                     }
                     else if (c == '\r') {
                         
                        if ( (c = readC (ptr)) != '\n') {              // updates lineNo if newline is spotted
                            ungetc (c, ptr);
                        }
                        if (!input) ++lineNo;
                     }
                     else if (c == '*') {                              // closing comments
                        if ( (c = readC (ptr)) == '/') break;
                        else ungetc (c, ptr);
                     }
                     else if (EOF(c)) eof ();
                } while (true);
            }
        }
        else if (EOF(c)) eof ();
    }
    
    /* due to use of readC () in entry control mode, file pointer will 
     | spot a proper char befor exiting upper loop.
     | Eg: 
     | \n\n\nabc
     |       ^___readC () gets this value to check if its a stray value 
     | in upper loop and immediately updates file pointer to point b.
     | So doing readC () again fetches string from b. But string is abc.
     | So ungetc () is to move the file pointer to a, ie undo getc operation.
     */
    ungetc (c, ptr);
    while ( (c = readC (ptr)) != -1 && c != 255) {
        
        /* if quoted is true, it means string is within quotes and
         | so no stray character, newlines, spaces, etc are ignored
         | till a second closing quote is spotted
         */
        if ( (c == '\r' || c == '\n' || isStrayChar (c)) && !quoted) {
            
            /* once read and found the char is a dilimiter, push it back
             | into the stream and break. when scanStr () is called next,
             | it automatically ignores this character using the first loop
             | before collecting the proper characters using this loop.
             */
            ungetc (c, ptr);
            break;
        }
        
        /* if single line comment started, push back the # character into stdin
         * and break. this means finishing reading the string.
         */
        else if (c == '#' && !quoted) {
            ungetc (c, ptr);
            break;
        }
        
        /* if comment started, push the two comment start characters
         | ie '/' and '*' into the stream and break, just like what's done
         | for delimiting characters.
         */
        else if (c == '/' && !quoted) {
            if ( (c = readC (ptr)) == '*') {
                ungetc ('*', ptr);
                ungetc ('/', ptr);
                break;
            }
            else if (EOF(c)) eof ();
            else {
                // if not comment start, just push back the '/'
                ungetc (c, ptr);
                c = '/';
            }
        }
        
        /* if quote, invert current value of 'quoted' flag
         | this flag is like a switch that controls the reading
         | or ignoring of delimiters 
         */
        else if (c == '"') {
            quoted = !quoted;
            continue;
        }
        // escape sequence for inputting delimiters
        else if (c == '\\') {
            
            escaped = true;
            c = readC (ptr);
            
            if (EOF(c)) eof ();
            else if (c == 'n') {
                // if index becomes equal to max size allowed for input
                if (i == size) {
                    str[i] = '\0';
                    E8a: fprintf (stderr, RED "ERR> " RST "[LINE: %u] Exceeded %u character input limit\n" RED "ERR> " RST "For '%s...'\n", lineNo, size, unEscape (substr (str, 0, 16)));
                    quit (8);
                }
                
                // this is done so that c is not set to '\n' otherwise nwl> prompt will get activated at label NWL for inp> "str\n"
                str[i++] = '\n';
                escaped = false;
                continue;
            }
            else if (c == 'r') c = '\r';
            else if (c == 't') c = '\t';
            else if (c == 'b') c = ' ';
            else if (c == '\n' && !console) {
                E6: fprintf (stderr, RED "ERR> " RST "[LINE: %u-%u] Newline must be escaped with '\\n'\n", lineNo, lineNo + 1);
                quit (6);
            }
            else if (c == '\r' && !console) {
                E7: fprintf (stderr, RED "ERR> " RST "[LINE: %u] Carriage return must be escaped with '\\r'\n", lineNo);
                quit (7);
            }
        }
        
        // if index becomes equal to max size allowed for input
        if (i == size) {
            str[i] = '\0';
            E8b: fprintf (stderr, RED "ERR> " RST "[LINE: %u] Exceeded %u character input limit\n" RED "ERR> " RST "For '%s...'\n", lineNo, size, unEscape (substr (str, 0, 16)));
            quit (8);
        }
        
        // writing into string
        str[i++] = (char) c;
        
        // if console mode is one and string is in quotes and enter is pressed, give new line prompt
        NWL: if (console && c == '\n' && (quoted || escaped)) {
            printf ("nwl> ");
        }
        escaped = false;
    }
    
    // null char string terminator
    str[i] = '\0';
    
    /* print whatever is read in, prArray works only if 'dev' flag is on
     | see global.c
     */
    prArray (str, strlen (str));
}
