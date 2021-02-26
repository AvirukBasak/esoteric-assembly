/* Author: CodesterClub
 * Version: 2021.3.1
 * Description: Another assembly simulator
 */

#include "global.c"
#include "input.c"
#include "interpreter.c"

void initialize() {
    tab = NULL;
    tabIndex = 0;
    printLbl = false;
    dev = false;
    file = NULL;
    FLAG = false;
    a = b = c = d = 0;
    dataPtr = 0;
    intBuffer = 0;
    lineNo = 1;
}

void evalOptions(char **args, int indx) {
    // If indx argument is help
    if (!strcmp(args[indx], "--help") || !strcmp(args[indx], "-h")) {
        printHelp();
        exit(0);
    }
    // if indx argument is version
    else if (!strcmp(args[indx], "--version") || !strcmp(args[indx], "-v")) {
        printf("Package: assembly-simulator\n");
        printf("Version: 2021.3.1\n");
        printf("Command: asm\n");
        exit(0);
    }
    // if it is dev
    else if (!strcmp(args[indx], "--dev") || !strcmp(args[indx], "-d")) {
        /* incase dev is 1st argument, ie indx is 1, indx 2 may be left empty
         * in that case this error msg is printed
         */
        if (args[2] == NULL) {
            E1: fprintf(stderr, RED "E2> No file path entered\n" RST);
            exit(2);
        }
        // make sure dev isn't enabled accidentally
        W1: printf(YEL "W1> DEVELOPER (DEBUG) MODE\n" RST);
        printf("  - This mode is for debugging the interpreter and not the asm\n    script.\n");
        printf("  - This mode prints every token the interpreter reads in.\n");
        printf("  - Label table is printed in this mode.\n");
        printf("  - I/O prompts of asm script will look ugly so removing them is\n    recommended.\n");
        printf(YEL "W1> Enter 'n' for normal execution. Enable debugger? (y/n) " RST);
        char s[8];
        scanStr(stdin, s, 8);
        if (s[0] == 'y') {
            printLbl = true;
            dev = true;
        }
    }
    // if indx arg is labels
    else if (!strcmp(args[indx], "--labels") || !strcmp(args[indx], "-l")) {
        // check if file path is not empty
        if (args[2] == NULL) {
            E2: fprintf(stderr, RED "E2> No file path entered\n" RST);
            exit(2);
        }
        // set label table printing flag on
        printLbl = true;
    }
    else {
        // if arg is invalid
        E3: fprintf(stderr, RED "E3> Invalid option: '%s'\n", args[indx]);
        fprintf(stderr, "E3> Use --help or -h for help text\n" RST);
        exit(3);
    }
}

/* args[] is used to pass command line arguments to program
 | For eg:
 | ~/$ ass /path/to/file.ass
 | This is an execution of binary file ass with file path as 
 | an argument. Automatically, the shell counts no. of arguments, 
 | and assigns "ass"  to args[0] and "/path/to/file.c"  to args[1].
 | *args[] is a DDA of chars or SDA of strings. (Note how *a is same 
 | as a[0], so *(*a) or *a[0] is same as (a[0])[0].
 */
int main(int argsc, char *args[]) {
    // initialize variables
    initialize();
    int arg = 1;
    /* If 1st argument (a string) is empty.
     | 0th argument is the executive command itself.
     */
    if(argsc == 1) {
        printf("Usage:\n");
        printf("  asm [filepath]\n  asm [OPTION]\n  asm [OPTION] [filepath]\n\n");
        printf("Options:\n");
        printf("  -h, --help       Display extended help text\n");
        printf("  -l, --labels     Display declared labels in tabular form\n");
        printf("  -v, --version    Display version information\n");
        printf("  -d, --dev        Developer mode to debug interpreter I/O\n");
        return 0;
    }
    // If 0th char of 1st argument is a '-', ie 1st arg is option
    else if (argsc == 2) {
        if (args[1][0] == '-') {
            evalOptions(args, 1);
            arg = 2;
        }
    }
    else if (argsc == 3) {
        if (args[2][0] == '-') {
            evalOptions(args, 2);
        }
        else if (args[1][0] == '-') {
            evalOptions(args, 1);
            arg = 2;
        }
    }
    else {
        fprintf(stderr, RED "E1> Too many arguments\n" RST);
        exit(1);
    }
    // open file
    openFile(args[arg]);
    // generate jump table
    genJmpTable();
    // goto file beginning
    fseek(file, 0, SEEK_SET);
    lineNo = 1;
    // scan the file for codes and interpret them
    if (dev) printf(YEL "SCAN FILE FOR CODES\n" RST);
    interpret();
    if (dev) printf(GRN "\nEXECUTION COMPLETE\n" RST);
    // close file
    quit(0);
}
