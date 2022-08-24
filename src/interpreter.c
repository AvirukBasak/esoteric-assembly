/* This file contains:
 | Functions for code interpretation
 */

#include "headers/headers.h"
#include "headers/global.h"

/* Reads operands and checks if garbage
 | @param oprnd1, oprnd2 The operands to be read
 | @param size1, size2 The available string size
 | @return bool, true if garbage
 */
bool ifReadOperandsAreGarbage (char *oprnd1, int size1, char *oprnd2, int size2) {
    scanStr (file, oprnd1, size1);                        // input 1st operand
    bool writable = false;
    if (!strcmp (opcode, "ieq") ||
        !strcmp (opcode, "ige") ||
        !strcmp (opcode, "ile") ||
        !strcmp (opcode, "igt") ||
        !strcmp (opcode, "igt")  )
            writable = true;
    if (selOprnd (oprnd1, writable) == &garbageBuffer)
        return true;
    scanStr (file, oprnd2, size2);                        // input 2nd operand
    return selOprnd (oprnd2, true) == &garbageBuffer;
}

/* multi-type operand selector, selects b/w ram addr, reg or num and returns a pointer
 | @param oprnd     The operand in string
 | @param w         If the operand is writable (in case it is a literal, it shouldn't be)
 | @return     pointer to either RAM addr, reg, numBuffer (where literals are stored),
 |             ptr, or garbageBuffer (for invalid operand values)
 */
int *selOprnd (char *oprnd, bool w) {
    
    // initialise with garbageBuffer so that if oprnd is invalid, this memory address is returned
    garbageBuffer = 0;
    int *ptr = &garbageBuffer;
    
    /* strtol () is string to long converter. atoi () use is not suggested because 
     | atoi doesn't check if the input string is actually a number and hence returns 
     | no errors. strtol () sets endptr to NULL if conversion doesn't happen.
     */
    char *endptr;
    
    if (oprnd[0] == '%') {
        if (!strcmp (oprnd, "%a")) ptr = &a;
        else if (!strcmp (oprnd, "%b")) ptr = &b;
        else if (!strcmp (oprnd, "%c")) ptr = &c;
        else if (!strcmp (oprnd, "%d")) ptr = &d;
        else {
            E9: fprintf (stderr, RED "ERR> " RST "[LINE: %u] Invalid register name: '%s'\n", lineNo, unEscape (&oprnd[1]));
            quit (9);
            return ptr;
        }
    }
    else if (oprnd[0] == '&') {
        int ramIndex = 0;
        
        // checks if oprnd is ptr
        if (!strcmp (substr (oprnd, 1, -1), "ptr")) {
            ramIndex = dataPtr;
        }
        else {
            /* try converting from decimal
             | strtol (ptr, endptr, base), if conversion fails, endptr is set to null
             */
            ramIndex = (int)strtol (substr (oprnd, 1, -1), &endptr, 10);
            
            // if from dec conversion fails
            if (*endptr) {
                
                // try converting from hex
                ramIndex = (int)strtol (substr (oprnd, 1, -1), &endptr, 16);
                
                // if even that fails, surely the input number is invalid
                if (*endptr) {
                    E10: fprintf (stderr, RED "ERR> " RST "[LINE: %u] Invalid address value: '%s'\n", lineNo, unEscape (substr (oprnd, 1, -1)));
                    quit (10);
                    return ptr;
                }
            }
        }
        if (ramIndex < 0 || ramIndex >= ramSize) {
            E11: fprintf (stderr, RED "ERR> " RST "[LINE: %u] Address out of bounds for: '%s'\n", lineNo, unEscape (substr (oprnd, 1, -1)));
            quit (11);
            return ptr;
        }
        // ptr is assigned at the very end. before this, it had pointed to garbageBuffer
        ptr = &RAM[ramIndex];
    }
    else if (oprnd[0] == '$') {
        
        // if oprnd is ptr
        if (!strcmp (substr (oprnd, 1, -1), "ptr")) {
            ptr = &dataPtr;
        }
        else {
            // try converting from decimal
            intBuffer = (int)strtol (substr (oprnd, 1, -1), &endptr, 10);
            
            // if from dec conversion fails
            if (*endptr) {
                
                // try converting from hex
                intBuffer = (int)strtol (substr (oprnd, 1, -1), &endptr, 16);
                
                // if even that fails, surely the input number is invalid
                if (*endptr) {
                    E12: fprintf (stderr, RED "ERR> " RST "[LINE: %u] Invalid literal value: '%s'\n", lineNo, unEscape (substr (oprnd, 1, -1)));
                    quit (12);
                    return ptr;
                }
            }
            // if writable flag disabled by calling function for safety reasons
            if (!w) {
                E13: fprintf (stderr, RED "ERR> " RST "[LINE: %u] Cannot assign to literal\n", lineNo);
                quit (13);
                return ptr;
            }
            // ptr is assigned at the very end. before this, it had pointed to garbageBuffer
            ptr = &intBuffer;
        }
    }
    else {
        E14: fprintf (stderr, RED "ERR> " RST "[LINE: %u] Invalid operand: '%s' for opcode: '%s'\n", lineNo, unEscape (oprnd), unEscape (opcode));
        quit (14);
        return ptr;
    }
    return ptr;
}

// generate jump table
void genJmpTable () {
    if (dev) printf (YEL "\nSCAN FILE FOR LABELS\n" RST);
    tabIndex = 0;
    do {
        scanStr (file, opcode, 64);                        // input whatever is there
        if (opcode[strlen (opcode) - 1] == ':') {
            
            /* Memory reallocaton so that memory isn't overwritten by a growing array. 
             | if instead I had used calloc just once, arrays would've expanded, yes, but
             | expanded into where? In this case, cur[] would probably expand into line[] and
             | overwrite line nos. So realloc () is used to copy the current array, add one cell
             | to it and place it in a new location. This way, no memory is overwritten.
             */
            tab = reallocateMem (tab, (tabIndex + 1) * sizeof (struct TABLE));
            
            // storing label, line and cursor posn
            strcpy (tab[tabIndex].lbl, substr (opcode, 0, strlen (opcode) - 1));
            for (int i = 0; i < tabIndex; i++) {
                if (!strcmp (tab[tabIndex].lbl, tab[i].lbl)) {
                    E15: fprintf (stderr, RED "ERR> " RST "[LINE: %u] Script contains duplicate of label: '%s'\n", lineNo, unEscape (tab[i].lbl));
                    quit (15);
                }
            }
            tab[tabIndex].cur = ftell (file);             // store it's cursor posn
            tab[tabIndex++].line = lineNo;                // store it's line
        }
    } while (strcmp ("end", opcode));
    
    if (printLbl) {                                       // print label table
        if (tabIndex == 0) {
            printf (RED "\nNo labels to print\n" RST); 
        }
        else {
            printf (YEL "\nLBL\tCUR\tLINE\n" RST);
            int i = 0;
            for (; i < tabIndex; i++)
                printf ("%s\t%d\t%u\n", tab[i].lbl, tab[i].cur, tab[i].line);
        }
        printf ("\n");
    }
}

// jump to label
void gotoLabel (char *label) {
    unsigned int lineNoBackup = lineNo;
    
    for (int i = 0; i < tabIndex; i++) {                  // traverse the table
        if (!strcmp (label, tab[i].lbl)) {                // if lable is in table
            fseek (file, tab[i].cur, SEEK_SET);           // set cursor to cursor pnt after label
            lineNo = tab[i].line;                         // set lineNo to line after label
            return;
        }
    }
    E16: fprintf (stderr, RED "ERR> " RST "[LINE: %u] No such label: '%s'\n", lineNoBackup, unEscape (label));
    quit (16);
}

// interprets commands
void evaluate (char *opcode) {
    char oprnd1[65];
    char oprnd2[65];
    unsigned int retCur;                                  // backup file cursor index for 'ret'
    unsigned int retLineNo;                               // line number backup for 'ret'

    if (opcode[strlen (opcode) - 1] == ':');              // LABELS ignored

    else if (!strcmp (opcode, "jmp") || !strcmp (opcode, "jit") || !strcmp (opcode, "jif")) {
        
        scanStr (file, oprnd1, 64);
        if (console) {                                    // loops and functions disabled when code is read from stdin (console)
            W2c: printf (YEL "WRN> " RST "[LINE: %u] Opcode '%s' is disabled in console mode\n", lineNo, unEscape (opcode));
            return;
        }
        if (!strcmp (opcode, "jmp") || (!strcmp (opcode, "jit") && FLAG) || (!strcmp (opcode, "jif") && !FLAG)) {
            retLineNo = lineNo;                           // backup the line index for 'ret' opcode
            retCur = ftell (file);                        // backup the cursor index for 'ret' opcode
            gotoLabel (oprnd1);                           // goto label, ie function declaration
        }
    }
    else if (!strcmp (opcode, "inv")) {                   // INVERT_FLAG
        FLAG = !FLAG;
        
        W3: printf (YEL "WRN> " RST "[LINE: %u] Opcode 'inv' is deprecated\n"
                        "     Use 'jit' i.e. JUMP_IF_TRUE or 'jif' i.e. JUMP_IF_FALSE\n", lineNo);
    }
    else if (!strcmp (opcode, "set")) {                   // SET_VALUE
        if (ifReadOperandsAreGarbage (oprnd1, 64, oprnd2, 64))
            return;
        
        // this method selects from the values of a reg, address, num or invalid
        *selOprnd (oprnd1, false) = *selOprnd (oprnd2, true);
    }
    else if (!strcmp (opcode, "add")) {                   // ADD
        if (ifReadOperandsAreGarbage (oprnd1, 64, oprnd2, 64))
            return;
        *selOprnd (oprnd1, false) += *selOprnd (oprnd2, true);
    }
    else if (!strcmp (opcode, "sub")) {                   // SUBTRACT
        if (ifReadOperandsAreGarbage (oprnd1, 64, oprnd2, 64))
            return;
        *selOprnd (oprnd1, false) -= *selOprnd (oprnd2, true);
    }
    else if (!strcmp (opcode, "mul")) {                   // MULTIPLY
        if (ifReadOperandsAreGarbage (oprnd1, 64, oprnd2, 64))
            return;
        *selOprnd (oprnd1, false) *= *selOprnd (oprnd2, true);
    }
    else if (!strcmp (opcode, "div")) {                   // DIVIDE
        if (ifReadOperandsAreGarbage (oprnd1, 64, oprnd2, 64))
            return;
        if (!*selOprnd (oprnd2, true)) {
            E17a: fprintf (stderr, RED "ERR> " RST "[LINE: %u] Cannot divide by zero\n", lineNo);
            quit (17);
        }
        *selOprnd (oprnd1, false) /= *selOprnd (oprnd2, true);
    }
    else if (!strcmp (opcode, "mod")) {                   // MOD
        if (ifReadOperandsAreGarbage (oprnd1, 64, oprnd2, 64))
            return;
        if (!*selOprnd (oprnd2, true)) {
            E17b: fprintf (stderr, RED "ERR> " RST "[LINE: %u] Cannot divide by zero\n", lineNo);
            quit (17);
        }
        *selOprnd (oprnd1, false) %= *selOprnd (oprnd2, true);
    }
    else if (!strcmp (opcode, "and")) {                   // AND
        if (ifReadOperandsAreGarbage (oprnd1, 64, oprnd2, 64))
            return;
        *selOprnd (oprnd1, false) &= *selOprnd (oprnd2, true);
    }
    else if (!strcmp (opcode, "or")) {                    // OR
        if (ifReadOperandsAreGarbage (oprnd1, 64, oprnd2, 64))
            return;
        *selOprnd (oprnd1, false) |= *selOprnd (oprnd2, true);
    }
    else if (!strcmp (opcode, "xor")) {                   // XOR
        if (ifReadOperandsAreGarbage (oprnd1, 64, oprnd2, 64))
            return;
        *selOprnd (oprnd1, false) ^= *selOprnd (oprnd2, true);
    }
    else if (!strcmp (opcode, "com")) {                   // [sizeof (int) * 8] Bit 1's complement
        scanStr (file, oprnd1, 64);
        *selOprnd (oprnd1, false) = ~*selOprnd (oprnd1, false);
    }
    else if (!strcmp (opcode, "ieq")) {                   // IS_EQUAL?
        if (ifReadOperandsAreGarbage (oprnd1, 64, oprnd2, 64))
            return;
        FLAG = *selOprnd (oprnd1, true) == *selOprnd (oprnd2, true);
        if (console) printf (YEL "out> %s\n" RST, FLAG ? BLU "true" : RED "false");
    }
    else if (!strcmp (opcode, "igt")) {                   // IS_GREATER_THAN?
        if (ifReadOperandsAreGarbage (oprnd1, 64, oprnd2, 64))
            return;
        FLAG = *selOprnd (oprnd1, true) > *selOprnd (oprnd2, true);
        if (console) printf (YEL "out> %s\n" RST, FLAG ? BLU "true" : RED "false");
    }
    else if (!strcmp (opcode, "ilt")) {                   // IS_LESS_THAN?
        if (ifReadOperandsAreGarbage (oprnd1, 64, oprnd2, 64))
            return;
        FLAG = *selOprnd (oprnd1, true) < *selOprnd (oprnd2, true);
        if (console) printf (YEL "out> %s\n" RST, FLAG ? BLU "true" : RED "false");
    }
    else if (!strcmp (opcode, "ige")) {                   // IS_GREATER_EQUAL?
        if (ifReadOperandsAreGarbage (oprnd1, 64, oprnd2, 64))
            return;
        FLAG = *selOprnd (oprnd1, true) >= *selOprnd (oprnd2, true);
        if (console) printf (YEL "out> %s\n" RST, FLAG ? BLU "true" : RED "false");
    }
    else if (!strcmp (opcode, "ile")) {                   // IS_LESS_EQUAL?
        if (ifReadOperandsAreGarbage (oprnd1, 64, oprnd2, 64))
            return;
        FLAG = *selOprnd (oprnd1, true) <= *selOprnd (oprnd2, true);
        if (console) printf (YEL "out> %s\n" RST, FLAG ? BLU "true" : RED "false");
    }
    // RAM resize
    else if (!strcmp (opcode, "ram")) {
        scanStr (file, oprnd1, 64);
        if (selOprnd (oprnd1, true) == &garbageBuffer)    // checks if operand is valid, ie not a garbage
            return;
        
        RAM = reallocateMem (RAM, (size_t)*selOprnd (oprnd1, true) * sizeof (int));
        ramSize = *selOprnd (oprnd1, true);
    }
    else if (!strcmp (opcode, "inp")) {                   // INPUT (console, only numeric input)
    
        scanStr (file, oprnd1, 64);
        if (selOprnd (oprnd1, false) == &garbageBuffer)   // checks if operand is valid, ie not a garbage
            return;
        
        if (dev) printf ("\n");
        input = true;                                     // this disables line counter in scanStr ()
        scanStr (stdin, oprnd2, 64);                      // input from console, NOT file
        input = false;                                    // re-enables line counter
        if (strlen (oprnd2) > 10) {                       // an int is generally <= 10 digit long
            E8c: fprintf (stderr, RED "ERR> " RST "Input too long\n");
            quit (8);
        }
            
        if (dev) printf ("\n");                           // dev mode aesthetics
        
        char *endptr;
        *selOprnd (oprnd1, false) = (int)strtol (oprnd2, &endptr, 10);
        if (*endptr) {
            E18: fprintf (stderr, RED "ERR> " RST "Invalid decimal input: '%s'\n", unEscape (oprnd2));
            quit (18);
        }
    }
    else if (!strcmp (opcode, "prn")) {                   // PRINT_NUM (print as number)
    
        scanStr (file, oprnd1, 64);
        if (selOprnd (oprnd1, true) == &garbageBuffer)    // checks if operand is valid, ie not a garbage
            return;
        
        if (dev) printf (YEL "\nout> " RST);              // print dev mode output indicator
        else if (console) printf (YEL "out> " RST);
        
        printf ("%d", *selOprnd (oprnd1, true));          // prints the number
        
        if (dev) printf ("\n\n");
        else if (console) printf ("\n");
    }
    else if (!strcmp (opcode, "prc")) {                   // PRINT_CHAR (print as character)
    
        scanStr (file, oprnd1, 64);
        if (selOprnd (oprnd1, true) == &garbageBuffer)    // checks if operand is valid, ie not a garbage
            return;
        
        if (dev) printf (YEL "\nout> " RST);
        else if (console) printf (YEL "out> " RST);
        
        printf ("%c", *selOprnd (oprnd1, true));          // prints the character
        
        if (dev) printf ("\n\n");
        else if (console) printf ("\n");
    }
    else if (!strcmp (opcode, "prs")) {                   // PRINT_STRING
    
        scanStr (file, oprnd1, 64);
        
        if (dev) printf (YEL "\nout> " RST);
        else if (console) printf (YEL "out> " RST);
        
        printf ("%s", oprnd1);                            // prints the string 
        if (dev || console) {                             // prints an inverted % just like zsh if output doesn't end with a newline
            if (oprnd1[strlen (oprnd1) - 1] != '\n') printf (INV "%%" RST "\n");
            if (dev) printf ("\n");
        }
    }
    else if (!strcmp (opcode, "nwl")) {                   // NEWLINE FEED
        if (dev || console) {
            if (dev) printf (YEL "\nout> " BLU);
            else if (console) printf (YEL "out> " BLU); 
            printf ("NEW LINE" RST);
            if (dev) printf ("\n\n");
            else if (console) printf ("\n");
        }
        else printf ("\n");
    }
    // help text for console mode
    else if (!strcmp (opcode, "hlp")) {
        if (console) printHelp (false);
        else 
            W2a: printf (YEL "WRN> " RST "[LINE: %u] Opcode 'hlp' is disabled without console mode\n", lineNo);
    }
    // END
    else if (!strcmp (opcode, "end")) return;
    // Invalid opcode
    else {
        E19: fprintf (stderr, RED "ERR> " RST "[LINE: %u] Invalid opcode: '%s'\n", lineNo, unEscape (opcode));
        quit (19);
    }
}

void interpret () {
    unsigned int retLineNo = 1;
    unsigned int retCur = -1;
    do {
        scanStr (file, opcode, 64);
        // call label as a function
        if (!strcmp (opcode, "call") || !strcmp (opcode, "calt") || !strcmp (opcode, "calf")) {
            char label[65];
            scanStr (file, label, 64);
            if (console) {
                W2b: printf (YEL "WRN> " RST "[LINE: %u] Opcode '%s' is disabled in console mode\n", lineNo, unEscape (opcode));
                continue;
            }
            if (!strcmp (opcode, "call") || (!strcmp (opcode, "calt") && FLAG) || (!strcmp (opcode, "calf") && !FLAG)) {
                retLineNo = lineNo;                       // backup the line index for 'ret' opcode
                retCur = ftell (file);                    // backup the cursor index for 'ret' opcode
                gotoLabel (label);                        // goto label, ie function declaration
                interpret ();                             // recursively call to execute code within function
                fseek (file, retCur, SEEK_SET);           // return to ret cursor index of file after jump instruction
                lineNo = retLineNo;
            }
        }
        else if (!strcmp (opcode, "ret")) {
            return;
        }
        else evaluate (opcode);
        ++steps;
    } while (strcmp ("end", opcode));
}
