/* This header contains 
 * All function declarations from each and every source files
 * Every pre-processor to std/third-party libraries
 * Every macro
 */

#ifndef HEADER
#define HEADER

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define YEL "\e[33m"
#define RED "\e[31m"
#define GRN "\e[32m"
#define BLU "\e[34m"
#define INV "\e[7;1m"
#define RST "\e[0m"

#undef EOF
#define EOF 255

// main.c
void initialize();
void evalOptions(int argsc, char **args, int indx);
int main(int argsc, char *args[]);

// global.c
char *substr(char *str, int frm, int to);
void quit(int exitcode);
void *allocateMem(size_t blocks, size_t size, bool initialize);
void *reallocateMem(void *ptr, size_t size);
char *unEscape(char *str);

// input.c
void openFile(char *path);
void eof(FILE *ptr);
signed short int readC(FILE *ptr);
bool isStrayChar(signed short int c);
void scanStr(FILE *ptr, char *str, unsigned int size);

// output.c
int print(FILE *ptr, const char *format, ...);
void printHelp(bool usage);
void prArray(char *s, unsigned int size);

// interpreter.c
int *selOprnd(char *oprnd, bool w);
void genJmpTable();
void gotoLabel(char *label);
void evaluate(char *opcode);
void interpret();

#endif
