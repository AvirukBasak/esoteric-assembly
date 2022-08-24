/* This file contains:
 | All global variables
 */

extern struct TABLE *tab;               // this will allow us to access any label based on its cursor posn
extern int tabIndex;                    // index of last label in table
extern bool console;                    // console mode
extern bool input;                      // marks input, so that lineNo isn't upadated
extern bool printLbl;                   // labels mode
extern bool dev;                        // dev mode

extern FILE *file;                      // input file pointer

extern bool FLAG;                       // boolean flag
extern char opcode[65];                 // opcode of operation
extern int a, b, c, d;                  // registers
extern int *RAM;                        // RAM array
extern size_t ramSize;

extern int dataPtr;                     // RAM data ptr
extern int intBuffer;                   // this global variable stores numeric inputs (after '#')
extern int garbageBuffer;               // garbage buffer to initialise selOprnd () pointer on event of an invalid register or address value

extern unsigned int lineNo;             // lineNo counter
extern unsigned long int steps;         // steps counter
