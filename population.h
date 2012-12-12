#ifndef POPULATION_H
#define POPULATION_H

#define POP_SIZE 128

#include "main.h"
#include "plan.h"

struct population;
typedef struct population * Population;

Population pop_create();

// The destructor doesn't delete any plan
Population pop_free(Population pop);


ushort pop_size(Population pop);

Plan pop_get(Population pop, ushort pos);


// Delete plan if the population is full
result pop_apppend(Population pop, Plan plan);

// Delete plans getting out the pop
result pop_insert_at(Population pop, Plan plan, ushort pos);

// Insert plan at its place (assume and keep the population sorted)
// Delete plans getting out the pop
result pop_insert(Population pop, Plan plan);


// Forget about the pop's plans (without deleting them)
void pop_reset(Population pop);

#endif // POPULATION_H
