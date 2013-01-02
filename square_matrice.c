#include "square_matrice.h"

#include <stdlib.h>

struct sq_mat {
  ushort size;
  schar * tab;
};

SqMat sqmat_create(ushort size) {
  if (size == 0) return NULL;

  SqMat sm = malloc(sizeof(struct sq_mat));
  if (sm == NULL) return NULL;

  sm->size = size;
  sm->tab  = calloc(size * size, sizeof(schar));
  if (sm->tab == NULL) {
    free(sm);
    return NULL;
  }

  for (int pos=0; pos < size * size; pos++)
    sm->tab[pos] = SM_NULL;

  return sm;
}


result sqmat_free(SqMat sm) {
  if (sm == NULL) return FAIL;

  free(sm->tab);
  free(sm);
  return OK;
}


schar sqmat_get(SqMat sm, ushort x, ushort y) {
  if (sm == NULL) die("NULL pointer for sqmat_get\n");
  if ((x >= sm->size) || (y >= sm->size)) die("index out of bounds for sqmat_get\n");

  return sm->tab[x + y * sm->size];
}


void sqmat_set(SqMat sm, ushort x, ushort y, schar v) {
  if (sm == NULL) die("NULL pointer for sqmat_set\n");
  if ((x >= sm->size) || (y >= sm->size)) die("index out of bounds for sqmat_set\n");

  sm->tab[x + y * sm->size] = v;
}


SqMat sqmat_add(SqMat a, SqMat b) {
  if ((a == NULL) || (b == NULL)) die("NULL pointer for sqmat_add\n");
  if (a->size != b->size) die("Sizes don't match for sqmat_add\n");

  ushort size = a->size;
  SqMat sm = sqmat_create(size);

  for (int pos=0; pos < size * size; pos++)
    sm->tab[pos] = a->tab[pos] + b->tab[pos];

  return sm;
}
