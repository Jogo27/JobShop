#ifndef SQUARE_MATRICE_H 
#define SQUARE_MATRICE_H 

#include <limits.h>
#include "main.h"

#define SM_NULL SCHAR_MIN

struct sq_mat;
typedef struct sq_mat * SqMat;

extern SqMat sqmat_create(ushort size);

extern result sqmat_free(SqMat sm);

extern schar sqmat_get(SqMat sm, ushort x, ushort y);

extern void sqmat_set(SqMat sm, ushort x, ushort y, schar v);

extern SqMat sqmat_add(SqMat a, SqMat b);

#endif // SQUARE_MATRICE_H 
