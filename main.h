#ifndef MAIN_H
#define MAIN_H

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

#define FAIL 0
#define OK   1
typedef char result;

typedef unsigned short ushort;

#include <stdarg.h>
extern void die(const char *errstr, ...);

#endif
