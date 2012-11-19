#ifndef MAIN_H
#define MAIN_H

#define FAIL 0
#define OK   1
#define LENGTH(x)               (sizeof x / sizeof x[0])
typedef char result;

typedef unsigned short ushort;

#include <stdarg.h>
extern void die(const char *errstr, ...);

#endif
