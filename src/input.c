// opens a file
void openFile(char *path) {
    file = fopen(path, "r");     // r means read mode
    if (file == NULL) {
        E4: fprintf(stderr, RED "ERR> " RST "Can't read file '%s'\n", unEscape(path));
        fprintf(stderr, RED "ERR> " RST "Check if file path exists and has read permission\n");
        exit(4);
    }
}

//handles EOF encounters
void eof() {
    E5: fprintf(stderr, RED "ERR> " RST "[LINE: %u] Unexpected end of file\n", lineNo);
    quit(5);
}

bool isStrayChar(char c) {
    return (c == ' ' || 
            c == '\t'||
            c == ',' || 
            c == ';' );
}

/* All this to incorporate line numbers counting
 | It scans for strings and while doing so, if it finds 
 | a \n, it adds 1 to lineNo, and ignores spaces and tabs.
 | If EOF (int value -1) is found, it halts
 */
void scanStr(FILE *ptr, char *str, unsigned int size) {
    int i = 0;
    signed char c;
    bool quoted = false;
    bool escaped = false;
    /* This loop is for cases where there's stray spaces, indents, 
     | newlines before a string. It is to traverse thru those till 
     | a proper character is spotted.
     | 32 is space.
     | 13 is carriage return '\r' (used in Windows). In Windows, 
     | '\r' is followed by '\n'. Although UNIX like OS don't use 
     | '\r', prior to OS X, mac used '\r' only, not '\n'.
     | 10 is '\n' or newline character.
     | 9 is '\t'.
     */
     // VERY IMPORTANT WARNING: fgetc() will NOT update reader cursor once it reaches EOF.
    while ((c = fgetc(ptr)) == EOF || c == 13 || c == 10 || c == '/' || isStrayChar(c)) {
        // updates lineNo if newline is spotted
        if (c == 10) {
            // updates line no
            if (!input) ++lineNo;
            // setting prompt
            if (console && prompt) printf(GRN "asm> " RST);
            // gets next character
            c = fgetc(ptr);
            // quit if eof
            if (c == EOF) eof();
            // else if c is not 13, bring read cursor back at location of c
            else if (c != 13) ungetc(c, ptr);
        }
        else if (c == 13) {
            // updates line no
            if (!input) ++lineNo;
            // gets next character
            c = fgetc(ptr);
            // quit if eof
            if (c == EOF) eof();
            // else if c is not 10, bring read cursor back at location of c
            else if (c != 10) ungetc(c, ptr);
        }
        else if (c == '/') {
            if ((c = fgetc(ptr)) != '*') ungetc(c, ptr);
            else {
                do {
                    c = fgetc(ptr);
                    // updates lineNo if newline is spotted
                    if (c == 10) {
                        if (console) printf("com> ");
                        if ((c = fgetc(ptr)) != 13) ungetc(c, ptr);
                        if (!input) ++lineNo;
                     }
                     else if (c == 13) {
                        // updates lineNo if newline is spotted
                        if ((c = fgetc(ptr)) != 10) ungetc(c, ptr);
                        if (!input) ++lineNo;
                     }
                     else if (c == '*') {
                        if ((c = fgetc(ptr)) == '/') break;
                        else ungetc(c, ptr);
                     }
                     else if (c == EOF) eof();
                } while (1);
            }
        }
        else if (c == EOF) eof();
    }
    /* due to use of fgetc() in entry control mode, file pointer will 
     | spot a proper char befor exiting upper loop.
     | Eg: 
     | \n\n\nabc
     |       ^___fgetc() gets this value to check if its a stray value 
     | in upper loop and immediately updates file pointer to point b.
     | So doing fgetc() again fetches string from b. But string is abc.
     | So ungetc() is to move the file pointer to a, ie undo getc operation.
     */
    ungetc(c, ptr);
    while ((c = fgetc(ptr)) != EOF) {
        /* if quoted is true, it means string is within quotes and
         * so no stray character, newlines, spaces, etc are ignored
         * till a second closing quote is spotted
         */
        if ((c == 13 || c == 10 || isStrayChar(c)) && !quoted) {
            /* once read and found the char is a dilimiter, push it back
             * into the stream and break. when scanStr() is called next,
             * it automatically ignores this character using the first loop
             * before collecting the proper characters using this loop.
             */
            ungetc(c, ptr);
            break;
        }
        /* if comment started, push the two comment start characters
         * ie '/' and '*' into the stream and break, just like what's done
         * for delimiting characters.
         */
        else if (c == '/' && !quoted) {
            if ((c = fgetc(ptr)) == '*') {
                ungetc('*', ptr);
                ungetc('/', ptr);
                break;
            }
            else if (c == EOF) eof();
            else {
                // if not comment start, just push back the '/'
                ungetc(c, ptr);
                c = '/';
            }
        }
        /* if quote, invert current value of 'quoted' flag
         * this flag is like a switch that controls the reading
         * or ignoring of delimiters 
         */
        else if (c == '"') {
            quoted = !quoted;
            if (quoted) continue;
            // if quoted is made false, a string is read, so break
            else break;
        }
        // escape sequence for inputting delimiters
        else if (c == '\\') {
            escaped = true;
            c = fgetc(ptr);
            if (c == 'n') {
                // if index becomes equal to max size allowed for input
                if (i == size) {
                    str[i] = '\0';
                    E8a: fprintf(stderr, RED "ERR> " RST "[LINE: %u] Exceeded %u character input limit\n" RED "ERR> " RST "For '%s...'\n", lineNo, size, unEscape(substr(str, 0, 16)));
                    quit(8);
                }
                str[i++] = '\n';
                continue;
            }
            else if (c == 'r') c = '\r';
            else if (c == 't') c = '\t';
            else if (c == 'b') c = ' ';
            else if (c == 10 && !console) {
                E6: fprintf(stderr, RED "ERR> " RST "[LINE: %u-%u] Newline must be escaped with '\\n'\n", lineNo, lineNo + 1);
                quit(6);
            }
            else if (c == 13 && !console) {
                E7: fprintf(stderr, RED "ERR> " RST "[LINE: %u] Carriage return must be escaped with '\\r'\n", lineNo);
                quit(7);
            }
        }
        // if index becomes equal to max size allowed for input
        if (i == size) {
            str[i] = '\0';
            E8a: fprintf(stderr, RED "ERR> " RST "[LINE: %u] Exceeded %u character input limit\n" RED "ERR> " RST "For '%s...'\n", lineNo, size, unEscape(substr(str, 0, 16)));
            quit(8);
        }
        str[i++] = c;
        if (console && ( (c == 10 && quoted) || (c == 10 && escaped) )) {
            printf("nwl> ");
            quoted = true;
        }
        
        escaped = false;
    }
    // null char string terminator
    str[i] = '\0';
    /* print whatever is read in, prArray works only if 'dev' flag is on
     * see global.c
     */
    prArray(str, strlen(str));
}
