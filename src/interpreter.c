// multi-type operand selector, selects b/w ram addr, reg or num and returns a pointer
int *selOprnd(char *oprnd, bool w) {
    
    // initialise with garbageBuffer so that if oprnd is invalid, this memory address is returned
    garbageBuffer = 0;
    int *ptr = &garbageBuffer;
    
    /* strtol() is string to long converter. atoi() use is not suggested because 
     | atoi doesn't check if the input string is actually a number and hence returns 
     | no errors. strtol() sets endptr to NULL if conversion doesn't happen.
     */
    char *endptr;
    
    if (oprnd[0] == '%') {
        if (!strcmp(oprnd, "%a")) ptr = &a;
        else if (!strcmp(oprnd, "%b")) ptr = &b;
        else if (!strcmp(oprnd, "%c")) ptr = &c;
        else if (!strcmp(oprnd, "%d")) ptr = &d;
        else {
            E9: print (stderr, RED "ERR> " RST "[LINE: %u] Invalid register name: '%s'\n", lineNo, unEscape(&oprnd[1]));
            quit(9);
            return ptr;
        }
    }
    else if (oprnd[0] == '&') {
        int ramIndex = 0;
        
        // checks if oprnd is ptr
        if (!strcmp(substr(oprnd, 1, -1), "ptr")) {
            ramIndex = dataPtr;
        }
        else {
            /* try converting from decimal
             * strtol(ptr, endptr, base), if conversion fails, endptr is set to null
             */
            ramIndex = (int)strtol(substr(oprnd, 1, -1), &endptr, 10);
            
            // if from dec conversion fails
            if (*endptr) {
                
                // try converting from hex
                ramIndex = (int)strtol(substr(oprnd, 1, -1), &endptr, 16);
                
                // if even that fails, surely the input number is invalid
                if (*endptr) {
                    E10: print (stderr, RED "ERR> " RST "[LINE: %u] Invalid address value: '%s'\n", lineNo, unEscape(substr(oprnd, 1, -1)));
                    quit(10);
                    return ptr;
                }
            }
        }
        if (ramIndex < 0 || ramIndex >= 1048576) {
            E11: print (stderr, RED "ERR> " RST "[LINE: %u] Address out of bounds for: '%s'\n", lineNo, unEscape(substr(oprnd, 1, -1)));
            quit(11);
            return ptr;
        }
        // ptr is assigned at the very end. before this, it had pointed to garbageBuffer
        ptr = &RAM[ramIndex];
    }
    else if (oprnd[0] == '$') {
        
        // if oprnd is ptr
        if (!strcmp(substr(oprnd, 1, -1), "ptr")) {
            ptr = &dataPtr;
        }
        else {
            // try converting from decimal
            intBuffer = (int)strtol(substr(oprnd, 1, -1), &endptr, 10);
            
            // if from dec conversion fails
            if (*endptr) {
                
                // try converting from hex
                intBuffer = (int)strtol(substr(oprnd, 1, -1), &endptr, 16);
                
                // if even that fails, surely the input number is invalid
                if (*endptr) {
                    E12: print (stderr, RED "ERR> " RST "[LINE: %u] Invalid literal value: '%s'\n", lineNo, unEscape(substr(oprnd, 1, -1)));
                    quit(12);
                    return ptr;
                }
            }
            // if writable flag disabled by calling function for safety reasons
            if (!w) {
                E13: print (stderr, RED "ERR> " RST "[LINE: %u] Cannot assign to literal\n", lineNo);
                quit(13);
                return ptr;
            }
            // ptr is assigned at the very end. before this, it had pointed to garbageBuffer
            ptr = &intBuffer;
        }
    }
    else {
        
        /* backing up unEscape(oprnd) as a dangling pointer can't be trusted
         * if this isn't done unEscape(oprnd) is overwritten by unEscape(opcode)
         * as the former becomes a dangling pointer, meaning its memory is already
         * freed
         */
        size_t size = strlen(unEscape(oprnd));
        char tmp[size];
        strcpy(tmp, unEscape(oprnd));
        E14: print (stderr, RED "ERR> " RST "[LINE: %u] Invalid operand: '%s' for opcode: '%s'\n", lineNo, tmp, unEscape(opcode));
        quit(14);
        return ptr;
    }
    return ptr;
}

// generate jump table
void genJmpTable() {
    if (dev) print (stdout, YEL "\nSCAN FILE FOR LABELS\n" RST);
    tabIndex = 0;
    do {
        scanStr(file, opcode, 64);              // input whatever is there
        if (opcode[strlen(opcode) - 1] == ':') {
            
            /* Memory reallocaton so that memory isn't overwritten by a growing array. 
             | if instead I had used calloc just once, arrays would've expanded, yes, but
             | expanded into where? In this case, cur[] would probably expand into line[] and
             | overwrite line nos. So realloc() is used to copy the current array, add one cell
             | to it and place it in a new location. This way, no memory is overwritten.
             */
            tab = reallocateMem(tab, (tabIndex + 1) * sizeof(struct TABLE));
            
            // storing label, line and cursor posn
            strcpy(tab[tabIndex].lbl, substr(opcode, 0, strlen(opcode) - 1));
            for (int i = 0; i < tabIndex; i++) {
                if (!strcmp(tab[tabIndex].lbl, tab[i].lbl)) {
                    E15: print (stderr, RED "ERR> " RST "[LINE: %u] Script contains duplicate of label: '%s'\n", lineNo, unEscape(tab[i].lbl));
                    quit(15);
                }
            }
            tab[tabIndex].cur = ftell(file);     // store it's cursor posn
            tab[tabIndex++].line = lineNo;       // store it's line
        }
    } while (strcmp("end", opcode));
    
    if (printLbl) {                              // print label table
        print (stdout, YEL "\nLBL\tCUR\tLINE\n" RST);
        
        int i = 0;
        for (; i < tabIndex; i++)
            print (stdout, "%s\t%d\t%u\n", tab[i].lbl, tab[i].cur, tab[i].line);
        if (i == 0)
            print (stdout, RED "No labels to print\n" RST);
        print (stdout, "\n");
    }
}

// jump to label
void gotoLabel(char *label) {
    unsigned int lineNoBackup = lineNo;
    
    for (int i = 0; i < tabIndex; i++) {         // traverse the table
        // if label is in table
        if (!strcmp(label, tab[i].lbl)) {        // if table found
            // set file cursor index to that label's cursor index
            fseek(file, tab[i].cur, SEEK_SET);   // set cursor to cursor pnt after label
            lineNo = tab[i].line;                // set lineNo to line after label
            return;
        }
    }
    E16: print (stderr, RED "ERR> " RST "[LINE: %u] No such label: '%s'\n", lineNoBackup, unEscape(label));
    quit(16);
}

// interprets commands
void evaluate(char *opcode) {
    char oprnd1[65];
    char oprnd2[65];
    unsigned int retCur;                         // backup file cursor index for 'ret'
    unsigned int retLineNo;                      // line number backup for 'ret'

    if (opcode[strlen(opcode) - 1] == ':');      // LABELS ignored

    else if (!strcmp(opcode, "jmp") || !strcmp(opcode, "jit") || !strcmp(opcode, "jif")) {
        
        scanStr(file, oprnd1, 64);
        if (console) {                           // loops and functions disabled when code is read from stdin (console)
            W2c: print (stdout, YEL "WRN> " RST "[LINE: %u] Opcode '%s' is disabled in console mode\n", lineNo, unEscape(opcode));
            return;
        }
        if (!strcmp(opcode, "jmp") || (!strcmp(opcode, "jit") && FLAG) || (!strcmp(opcode, "jif") && !FLAG)) {
            retLineNo = lineNo;                  // backup the line index for 'ret' opcode
            retCur = ftell(file);                // backup the cursor index for 'ret' opcode
            gotoLabel(oprnd1);                   // goto label, ie function declaration
        }
    }
    else if (!strcmp(opcode, "inv")) {           // INVERT_FLAG
        FLAG = !FLAG;
        
        W3: print (stdout, YEL "WRN> " RST "[LINE: %u] Opcode 'inv' is deprecated\n", lineNo);
        print (stdout, YEL "WRN> " RST "Use 'jit' i.e. JUMP_IF_TRUE or 'jif' i.e. JUMP_IF_FALSE\n");
    }
    else if (!strcmp(opcode, "set")) {           // SET_VALUE
        scanStr(file, oprnd1, 64);               // input 1st operand
        scanStr(file, oprnd2, 64);               // input 2nd operand
        // this method automatically selects if the value is a reg, address, num or invalid
        *selOprnd(oprnd1, 0) = *selOprnd(oprnd2, 1);
    }
    else if (!strcmp(opcode, "add")) {           // ADD
        scanStr(file, oprnd1, 64);
        scanStr(file, oprnd2, 64);
        *selOprnd(oprnd1, 0) += *selOprnd(oprnd2, 1);
    }
    else if (!strcmp(opcode, "sub")) {           // SUBTRACT
        scanStr(file, oprnd1, 64);
        scanStr(file, oprnd2, 64);
        *selOprnd(oprnd1, 0) -= *selOprnd(oprnd2, 1);
    }
    else if (!strcmp(opcode, "mul")) {           // MULTIPLY
        scanStr(file, oprnd1, 64);
        scanStr(file, oprnd2, 64);
        *selOprnd(oprnd1, 0) *= *selOprnd(oprnd2, 1);
    }
    else if (!strcmp(opcode, "div")) {           // DIVIDE
        scanStr(file, oprnd1, 64);
        scanStr(file, oprnd2, 64);
        if (!*selOprnd(oprnd2, 1)) {
            E17a: print (stderr, RED "ERR> " RST "[LINE: %u] Cannot divide by zero\n", lineNo);
            quit(17);
        }
        *selOprnd(oprnd1, 0) /= *selOprnd(oprnd2, 1);
    }
    else if (!strcmp(opcode, "mod")) {           // MOD
        scanStr(file, oprnd1, 64);
        scanStr(file, oprnd2, 64);
        if (!*selOprnd(oprnd2, 1)) {
            E17b: print (stderr, RED "ERR> " RST "[LINE: %u] Cannot divide by zero\n", lineNo);
            quit(17);
        }
        *selOprnd(oprnd1, 0) %= *selOprnd(oprnd2, 1);
    }
    else if (!strcmp(opcode, "and")) {           // AND
        scanStr(file, oprnd1, 64);
        scanStr(file, oprnd2, 64);
        *selOprnd(oprnd1, 0) &= *selOprnd(oprnd2, 1);
    }
    else if (!strcmp(opcode, "or")) {            // OR
        scanStr(file, oprnd1, 64);
        scanStr(file, oprnd2, 64);
        *selOprnd(oprnd1, 0) |= *selOprnd(oprnd2, 1);
    }
    else if (!strcmp(opcode, "xor")) {           // XOR
        scanStr(file, oprnd1, 64);
        scanStr(file, oprnd2, 64);
        *selOprnd(oprnd1, 0) ^= *selOprnd(oprnd2, 1);
    }
    else if (!strcmp(opcode, "com")) {           // [sizeof(int) * 8] Bit 1's complement
        scanStr(file, oprnd1, 64);
        *selOprnd(oprnd1, 0) = ~*selOprnd(oprnd1, 0);
    }
    else if (!strcmp(opcode, "ieq")) {           // IS_EQUAL?
        scanStr(file, oprnd1, 64);
        scanStr(file, oprnd2, 64);
        FLAG = *selOprnd(oprnd1, 0) == *selOprnd(oprnd2, 1);
    }
    else if (!strcmp(opcode, "igt")) {           // IS_GREATER_THAN?
        scanStr(file, oprnd1, 64);
        scanStr(file, oprnd2, 64);
        FLAG = *selOprnd(oprnd1, 0) > *selOprnd(oprnd2, 1);
    }
    else if (!strcmp(opcode, "ilt")) {           // IS_LESS_THAN?
        scanStr(file, oprnd1, 64);
        scanStr(file, oprnd2, 64);
        FLAG = *selOprnd(oprnd1, 0) < *selOprnd(oprnd2, 1);
    }
    else if (!strcmp(opcode, "ige")) {           // IS_GREATER_EQUAL?
        scanStr(file, oprnd1, 64);
        scanStr(file, oprnd2, 64);
        FLAG = *selOprnd(oprnd1, 0) >= *selOprnd(oprnd2, 1);
    }
    else if (!strcmp(opcode, "ile")) {           // IS_LESS_EQUAL?
        scanStr(file, oprnd1, 64);
        scanStr(file, oprnd2, 64);
        FLAG = *selOprnd(oprnd1, 0) <= *selOprnd(oprnd2, 1);
    }
    else if (!strcmp(opcode, "inp")) {           // INPUT (console, only numeric input)
        scanStr(file, oprnd1, 64);
        // checks if operand is valid, ie not a garbage
        if (selOprnd(oprnd1, 0) == &garbageBuffer)
            return;
        
        if (dev) print (stdout, YEL "\ninp> " RST);        // in dev mode, extra LF
        else if (console) {                      // inp prompt in console mode
            print (stdout, YEL "inp> " RST);
            prompt = false;                      // disable asm> prompt
        }
        input = true;                            // disables lineNo update
        scanStr(stdin, oprnd2, 64);              // input from console, NOT file
        input = false;                           // enables lineNo update and asm> prompt
        prompt = true;
        
        if (strlen(oprnd2) > 10) {               // an int is generally <= 10 digit long
            E8c: print (stderr, RED "ERR> " RST "Input too long\n");
            quit(8);
        }
            
        if (dev) print (stdout, "\n");                   // dev mode aesthetics
        
        char *endptr;
        *selOprnd(oprnd1, 0) = (int)strtol(oprnd2, &endptr, 10);
        if (*endptr) {
            E18: print (stderr, RED "ERR> " RST "Invalid decimal input: '%s'\n", unEscape(oprnd2));
            quit(18);
        }
    }
    else if (!strcmp(opcode, "prn")) {           // PRINT_NUM (print as number)
        scanStr(file, oprnd1, 64);
        // checks if operand is valid, ie not a garbage
        if (selOprnd(oprnd1, 1) == &garbageBuffer)
            return;
        
        if (dev) print (stdout, YEL "\nout> " RST);
        else if (console) print (stdout, YEL "out> " RST);
        
        print (stdout, "%d", *selOprnd(oprnd1, 1));
        
        if (dev) print (stdout, "\n\n");
        else if (console) print (stdout, "\n");
    }
    else if (!strcmp(opcode, "prc")) {           // PRINT_CHAR (print as character)
        scanStr(file, oprnd1, 64);
        // checks if operand is valid, ie not a garbage
        if (selOprnd(oprnd1, 1) == &garbageBuffer)
            return;
        
        if (dev) print (stdout, YEL "\nout> " RST);
        else if (console) print (stdout, YEL "out> " RST);
        
        print (stdout, "%c", *selOprnd(oprnd1, 1));
        
        if (dev) print (stdout, "\n\n");
        else if (console) print (stdout, "\n");
    }
    else if (!strcmp(opcode, "prs")) {           // PRINT_STRING
        scanStr(file, oprnd1, 64);
        
        if (dev) print (stdout, YEL "\nout> " RST);
        else if (console) print (stdout, YEL "out> " RST);
        
        print (stdout, "%s", oprnd1);
        
        if (dev || console) {
            if (oprnd1[strlen(oprnd1) - 1] != 10) print (stdout, INV "%%" RST "\n");
            if (!console) print (stdout, "\n");
        }
    }
    else if (!strcmp(opcode, "nwl")) {           // NEWLINE FEED
        if (dev || console) {
            if (dev) print (stdout, YEL "\nout> " BLU);
            else if (console) print (stdout, YEL "out> " BLU); 
            print (stdout, "NEW LINE" RST);
            if (dev) print (stdout, "\n\n");
            else if (console) print (stdout, "\n");
        }
        else print (stdout, "\n");
    }
    // END
    else if (!strcmp(opcode, "end")) return;
    // Invalid opcode
    else {
        E19: print (stderr, RED "ERR> " RST "[LINE: %u] Invalid opcode: '%s'\n", lineNo, unEscape(opcode));
        quit(19);
    }
}

void interpret() {
    unsigned int retLineNo = 1;
    unsigned int retCur = -1;
    do {
        scanStr(file, opcode, 64);
        if (!strcmp(opcode, "hlp")) {
            if (console) printHelp(false);
            else 
                W2a: print (stdout, YEL "WRN> " RST "[LINE: %u] Opcode 'hlp' is disabled without console mode\n", lineNo);
        }
        // call label as a function
        else if (!strcmp(opcode, "call") || !strcmp(opcode, "calt") || !strcmp(opcode, "calf")) {
            char label[65];
            scanStr(file, label, 64);
            if (console) {
                W2b: print (stdout, YEL "WRN> " RST "[LINE: %u] Opcode '%s' is disabled in console mode\n", lineNo, unEscape(opcode));
                continue;
            }
            if (!strcmp(opcode, "call") || (!strcmp(opcode, "calt") && FLAG) || (!strcmp(opcode, "calf") && !FLAG)) {
                retLineNo = lineNo;              // backup the line index for 'ret' opcode
                retCur = ftell(file);            // backup the cursor index for 'ret' opcode
                gotoLabel(label);                // goto label, ie function declaration
                interpret();                     // recursively call to execute code within function
                fseek(file, retCur, SEEK_SET);   // return to ret cursor index of file after jump instruction
                lineNo = retLineNo;
            }
        }
        else if (!strcmp(opcode, "ret")) {
            return;
        }
        else evaluate(opcode);
        ++steps;
    } while (strcmp("end", opcode));
}
