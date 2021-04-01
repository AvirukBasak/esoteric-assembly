/* This file contains prototypes of every library function
 | used in the source. This is done to reduce size of the
 | object file and often helps reduce executable binary size
 | by a few kilobytes.
 */

// stdlib.h alternative
# ifdef size_t
#     undef size_t
# endif
# define size_t unsigned long;

void *malloc (size_t size);
void *calloc (size_t blocks, size_t size);
void *realloc (void *ptr, size_t size);
void free (void *ptr);

long strtol (const char *s, char **endptr, int base);

void exit (int exitcode);

// string.h alternative
size_t strlen (const char *s);
int strcmp (const char *s1, const char *s2);
char *strcpy (char *s1, const char *s2);

// stdbool.h alternative
#define bool _Bool
#define true 1
#define false 0
