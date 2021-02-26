// multi-type operand selector, selects b/w ram addr, reg or num and returns a pointer
int *selOprnd(char *oprnd, bool w) {
    int *ptr;
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
            E9: fprintf(stderr, RED "E9> [LINE: %u] Invalid register name: '%s'\n" RST, lineNo, &oprnd[1]);
            quit(9);
        }
    }
    else if (oprnd[0] == '&') {
        int ramIndex = 0;
        // strtol(ptr, endptr, base), if conversion fails, endptr is set to null
        if (!strcmp(substr(oprnd, 1, -1), "ptr")) {
            ramIndex = dataPtr;
        }
        else {
            // try converting from decimal
            ramIndex = (int)strtol(substr(oprnd, 1, -1), &endptr, 10);
            // if from dec conversion fails
            if (*endptr) {
                // try converting from hex
                ramIndex = (int)strtol(substr(oprnd, 1, -1), &endptr, 16);
                // if even that fails, surely the input number is invalid
                if (*endptr) {
                    E10: fprintf(stderr, RED "E10> [LINE: %u] Invalid address value: '%s'\n" RST, lineNo, substr(oprnd, 1, -1));
                    quit(10);
                }
            }
        }
        if (ramIndex < 0 || ramIndex >= 1048576) {
            E11: fprintf(stderr, RED "E11> [LINE: %u] Address out of bounds for: '%s'\n" RST, lineNo, substr(oprnd, 1, -1));
            quit(11);
        }
        ptr = &RAM[ramIndex];
    }
    else if (oprnd[0] == '$') {
        // if ptr is being modified
        if (!strcmp(substr(oprnd, 1, -1), "ptr")) {
            ptr = &dataPtr;
        }
        else {
            // if writable flag disabled by calling function for safety reasons
            if (!w) {
                E12: fprintf(stderr, RED "E12> [LINE: %u] Cannot assign to literal\n" RST, lineNo);
                quit(12);
            }
            // try converting from decimal
            intBuffer = (int)strtol(substr(oprnd, 1, -1), &endptr, 10);
            // if from dec conversion fails
            if (*endptr) {
                // try converting from hex
                intBuffer = (int)strtol(substr(oprnd, 1, -1), &endptr, 16);
                // if even that fails, surely the input number is invalid
                if (*endptr) {
                    E13: fprintf(stderr, RED "E13> [LINE: %u] Invalid literal value: '%s'\n" RST, lineNo, substr(oprnd, 1, -1));
                    quit(13);
                }
            }
        }
        ptr = &intBuffer;
    }
    else {
        E14: fprintf(stderr, RED "E14> [LINE: %u] Invalid operand: '%s' for opcode: '%s'\n" RST, lineNo, oprnd, opcode);
        quit(14);
    }
    return ptr;
}

// generate jump table
void genJmpTable() {
    if (dev) printf(YEL "\nSCAN FILE FOR LABELS\n" RST);
    tabIndex = 0;
    do {
        scanStr(file, opcode, 64);              // input whatever is there
        if (opcode[strlen(opcode) - 1] == ':') {
            /* Memory reallocaton so that memory isn't overwritten by a growing array. 
             | if insted I had used calloc just once in main() by keeping in mind that calloc
             | automatically expands memory, arrays would've expanded dynamically, yes. But 
             | expanded into where? In this case, cur[] would expand into line[] and overwrite 
             | line numbers. So realloc() is used to copy the current array, add one cell to it 
             | and place it in a new location. This way, no memory is overwritten.
             */
            tab = realloc(tab, (tabIndex + 1) * sizeof(struct TABLE));
            // storing label, line and cursor posn
            strcpy(tab[tabIndex].lbl, substr(opcode, 0, strlen(opcode) - 1));
            for (int i = 0; i < tabIndex; i++) {
                if (!strcmp(tab[tabIndex].lbl, tab[i].lbl)) {
                    E15: fprintf(stderr, RED "E15> [LINE: %u] Script contains duplicate of label: '%s'\n" RST, lineNo, tab[i].lbl);
                    quit(15);
                }
            }
            tab[tabIndex].cur = ftell(file);     // store it's cursor posn
            tab[tabIndex++].line = lineNo;       // store it's line
        }
    } while (strcmp("end", opcode));
    if (printLbl) {
        printf(YEL "\nLBL\tCUR\tLINE\n" RST);
        int i = 0;
        for (; i < tabIndex; i++) {
            printf("%s\t%d\t%u\n", tab[i].lbl, tab[i].cur, tab[i].line);
        }
        if (i == 0) printf(RED ">> No labels to print\n" RST);
        printf("\n");
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
    E16: fprintf(stderr, RED "E16> [LINE: %u] No such label: '%s'\n" RST, lineNoBackup, label);
    quit(16);
}

// interprets commands
void evaluate(char *opcode) {
    char oprnd1[65];
    char oprnd2[65];
    unsigned int retCur;                         // backup file cursor index for 'ret'
    unsigned int retLineNo;                      // line number backup for 'ret'
    if (opcode[strlen(opcode) - 1] == ':');      // LABELS
    else if (!strcmp(opcode, "jit")) {           // JUMP_IF_TRUE (variable FLAG true)
        scanStr(file, oprnd1, 64);               // read label value
        if(FLAG) {
            retLineNo = lineNo;                  // backup the line index for 'ret' opcode
            retCur = ftell(file);                // backup the cursor index for 'ret' opcode
            gotoLabel(oprnd1);
        }
    }
    else if (!strcmp(opcode, "jif")) {           // JUMP_IF_FALSE (variable FLAG false)
        scanStr(file, oprnd1, 64);               // read label value
        if(!FLAG) {
            retLineNo = lineNo;                  // backup the line index for 'ret' opcode
            retCur = ftell(file);                // backup the cursor index for 'ret' opcode
            gotoLabel(oprnd1);
        }
    }
    else if (!strcmp(opcode, "jmp")) {           // JUMP (unconditional)
        scanStr(file, oprnd1, 64);               // read label value
        retLineNo = lineNo;                      // backup the line index for 'ret' opcode
        retCur = ftell(file);                    // backup the cursor index for 'ret' opcode
        gotoLabel(oprnd1);
    }
    else if (!strcmp(opcode, "inv")) {           // INVERT_FLAG
        FLAG = !FLAG;
        W2: printf(YEL "W2> [LINE: %u] Opcode 'inv' is deprecated\n", lineNo);
        printf("W2> Use 'jit' i.e. JUMP_IF_TRUE or 'jif' i.e. JUMP_IF_FALSE\n" RST);
    }
    else if (!strcmp(opcode, "set")) {           // SET_VALUE
        scanStr(file, oprnd1, 64);
        scanStr(file, oprnd2, 64);
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
            E17: fprintf(stderr, RED "E17> [LINE: %u] Cannot divide by zero\n" RST, lineNo);
            quit(17);
        }
        *selOprnd(oprnd1, 0) /= *selOprnd(oprnd2, 1);
    }
    else if (!strcmp(opcode, "mod")) {           // MOD
        scanStr(file, oprnd1, 64);
        scanStr(file, oprnd2, 64);
        if (!*selOprnd(oprnd2, 1)) {
            fprintf(stderr, RED "E17> [LINE: %u] Cannot divide by zero\n" RST, lineNo);
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
    else if (!strcmp(opcode, "com")) {           // 32 Bit 1's complement
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
        if (dev) printf(YEL "\nINP> " RST);
        scanStr(stdin, oprnd2, 64);              // input from console, NOT file
        if (strlen(oprnd2) > 10) {
            fprintf(stderr, RED "E8> Input too long\n");
            quit(8);
        }
        if (dev) printf("\n");
        char *endptr;
        *selOprnd(oprnd1, 0) = (int)strtol(oprnd2, &endptr, 10);
        if (*endptr) {
            E18: fprintf(stderr, RED "E18> Invalid decimal input: '%s'\n" RST, oprnd2);
            quit(18);
        }
    }
    else if (!strcmp(opcode, "prn")) {           // PRINT_NUM (print as number)
        scanStr(file, oprnd1, 64);
        if (dev) printf(YEL "\nOUT> " RST);
        printf("%d", *selOprnd(oprnd1, 0));
        if (dev) printf("\n\n");
    }
    else if (!strcmp(opcode, "prc")) {           // PRINT_CHAR (print as character)
        scanStr(file, oprnd1, 64);
        if (dev) printf(YEL "\nOUT> " RST);
        printf("%c", *selOprnd(oprnd1, 0));
        if (dev) printf("\n\n");
    }
    else if (!strcmp(opcode, "prs")) {           // PRINT_STRING
        scanStr(file, oprnd1, 64);
        if (dev) printf(YEL "\nOUT> " RST);
        printf("%s", oprnd1);
        if (dev) {
            if (oprnd1[strlen(oprnd1) - 1] != 10) printf(INV "%%" RST "\n");
            printf("\n");
        }
    }
    else if (!strcmp(opcode, "nwl")) {           // NEWLINE FEED
        if (!dev) printf("\n");
        else {
            printf(YEL "\nOUT> " BLU);
            printf("NEW LINE\n\n" RST);
        }
    }
    // END
    else if (!strcmp(opcode, "end")) return;
    // Invalid opcode
    else {
        E19: fprintf(stderr, RED "E19> [LINE: %u] Invalid opcode: '%s'\n" RST, lineNo, opcode);
        quit(19);
    }
}

void interpret() {
    unsigned int retLineNo = 1;
    unsigned int retCur = -1;
    do {
        scanStr(file, opcode, 64);
        // call label as a function
        if (!strcmp(opcode, "call") || !strcmp(opcode, "calt") || !strcmp(opcode, "calf")) {
            char label[65];
            scanStr(file, label, 64);
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
    } while (strcmp("end", opcode));
}
