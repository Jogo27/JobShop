#ifndef POPULATION_H
#define POPULATION_H

#define POP_SIZE 128

#include "main.h"
#include "plan.h"

struct population;
typedef struct population * Population;

extern Population pop_create();

// The destructor doesn't delete any plan
extern void pop_free(Population pop);


extern ushort pop_size(Population pop);

extern Plan pop_get(Population pop, ushort pos);


// Delete plan if the population is full
extern result pop_append(Population pop, Plan plan);

// Delete plans getting off the pop
extern result pop_insert_at(Population pop, Plan plan, ushort pos);

// Insert plan at its place (assume and keep the population sorted)
// Delete plans getting out the pop
extern result pop_insert(Population pop, Plan plan);


// Forget about the pop's plans (without deleting them)
extern void pop_reset(Population pop);

#endif // POPULATION_H
