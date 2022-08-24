/* This file contains:
 | All global variables
 */

#include "headers/headers.h"
#include "headers/global.h"

struct TABLE *tab;               // this will allow us to access any label based on its cursor posn
int tabIndex;                    // index of last label in table
bool console;                    // console mode
bool input;                      // marks input, so that lineNo isn't upadated
bool printLbl;                   // labels mode
bool dev;                        // dev mode

FILE *file;                      // input file pointer

bool FLAG;                       // boolean flag
char opcode[65];                 // opcode of operation
int a, b, c, d;                  // registers
int *RAM;                        // RAM array
size_t ramSize;

int dataPtr;                     // RAM data ptr
int intBuffer;                   // this global variable stores numeric inputs (after '#')
int garbageBuffer;               // garbage buffer to initialise selOprnd () pointer on event of an invalid register or address value

unsigned int lineNo;             // lineNo counter
unsigned long int steps;         // steps counter
