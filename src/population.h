/* Joseph Boudou
 * -------------
 * TP JobShop
 * Cours d'optimisation combinatoire
 * -------------
 * Structure de données pour les populations (ensemble ordonné de solutions)
 */

#ifndef POPULATION_H
#define POPULATION_H

#define POP_SIZE 1024

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

// Append to dest as much plans as possible from src starting at pos.
// Returns the number of plans added.
extern ushort pop_copy_append(Population dest, Population src, ushort pos);

// Forget about the pop's plans (without deleting them)
extern void pop_reset(Population pop);

#endif // POPULATION_H
