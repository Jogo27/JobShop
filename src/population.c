/* Joseph Boudou
 * -------------
 * TP JobShop
 * Cours d'optimisation combinatoire
 * -------------
 * Structure de données pour les populations (ensemble ordonné de solutions)
 */

#include "population.h"

#include <stdlib.h>
#include <string.h>

struct population {
  ushort size;
  Plan   plans[POP_SIZE];
};

Population pop_create() {
  Population pop = malloc(sizeof(struct population));
  if (pop != NULL) pop->size = 0;
  return pop;
}


// The destructor doesn't delete any plan
void pop_free(Population pop) {
  if (pop == NULL) die("NULL pop for pop_free\n");
  free(pop);
}


ushort pop_size(Population pop) {
  if (pop == NULL) die("NULL pop for pop_size\n");
  return pop->size;
}


Plan pop_get(Population pop, ushort pos) {
  if ((pop == NULL) || (pos >= pop->size)) die("Wrong arg for pop_get\n");
  return pop->plans[pos];
}


// Delete plan if the population is full
result pop_append(Population pop, Plan plan) {
  if (pop == NULL) die("NULL pop for pop_append\n");
  if (plan == NULL) return FAIL;
  if (pop->size >= POP_SIZE) {
    plan_free(plan);
    return FAIL;
  }

  pop->plans[pop->size] = plan;
  pop->size += 1;
  return OK;
}


result pop_insert_at(Population pop, Plan plan, ushort pos) {
  if ((pop == NULL) || (pos > pop->size)) die("Wrong arg for pop_insert_at\n");
  if (plan == NULL) return FAIL;

  ushort size_to_move;
  if (pop->size >= POP_SIZE) {
    plan_free(pop_get(pop, POP_SIZE - 1));
    size_to_move = POP_SIZE - 1 - pos;
  }
  else {
    size_to_move = pop->size - pos;
    pop->size += 1;
  }
  memmove(&pop->plans[pos+1], &pop->plans[pos], size_to_move * sizeof(Plan));
  pop->plans[pos] = plan;
  return OK;
}


// Insert plan at its place (assume and keep the population sorted)
// Delete plans getting out the pop
result pop_insert(Population pop, Plan plan) {
  if (pop == NULL) die("NULL pop for pop_append\n");
  if (plan == NULL) return FAIL;

  if (pop->size == 0) return pop_append(pop, plan);

  int duration = plan_duration(plan);
  int duration_m;

  ushort b = 0;
  duration_m = plan_duration(pop->plans[0]);
  if (duration  <  duration_m) {
    return pop_insert_at(pop, plan, 0);
  }
  if (duration == duration_m) {
    if (plan_equals(plan, pop->plans[0])) {
      plan_free(plan);
      return FAIL;
    }
    else
      return pop_insert_at(pop, plan, 0);
  }

  ushort e = pop->size - 1;
  duration_m = plan_duration(pop->plans[e]);
  if (duration  >  duration_m) {
    return pop_append(pop, plan);
  }
  if (duration == duration_m) {
    if (plan_equals(plan, pop->plans[e])) {
      plan_free(plan);
      return FAIL;
    }
    else
      return pop_insert_at(pop, plan, e);
  }


  while ((e - b) > 1) {
    ushort m = (e + b) / 2;
    duration_m = plan_duration(pop->plans[m]);
    if (duration_m < duration) {
      b = m;
    }
    else if (duration_m > duration) {
      e = m;
    }
    else if (plan_equals(plan, pop->plans[m])) {
      plan_free(plan);
      return FAIL;
    }
    else {
      return pop_insert_at(pop, plan, m);
    }
  }
  return pop_insert_at(pop, plan, e);
}


ushort pop_copy_append(Population dest, Population src, ushort pos) {
  if ((dest == NULL) || (src == NULL)) die("NULL population for pop_copy_append\n");

  ushort nb_copy = MIN( (POP_SIZE - dest->size), (src->size - pos) );
  memcpy(&dest->plans[dest->size], &src->plans[pos], nb_copy * sizeof(Plan));
  dest->size += nb_copy;
  return nb_copy;
}

// Forget about the pop's plans (without deleting them)
void pop_reset(Population pop) {
  if (pop == NULL) die("NULL pop for pop_reset\n");
  pop->size = 0;
}
