#ifndef MAIN_H
#define MAIN_H

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

#define FAIL 0
#define OK   1
typedef char result;

typedef unsigned short ushort;
typedef signed   char  schar;

#include <stdarg.h>
extern void die(const char *errstr, ...);

// Only print in "debug" mode
extern void debug(const char *format, ...);

// Print summary information (not in "debug" mode)
extern void info(const char *format, ...);

#endif
