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



/* Constructors Destructors */

// Allocate a new (empty) population
extern Population pop_create ();

// The destructor doesn't delete any plan
extern void pop_free (Population pop);



/* Getters Setters */

// Number of plan in the population
extern ushort pop_size (Population pop);

// Retrieve a given plan
extern Plan pop_get (Population pop, ushort pos);

// Append to dest as much plans as possible from src starting at pos.
// Returns the number of plans added.
extern ushort pop_copy_append (Population dest, Population src, ushort pos);

// Forget about the pop's plans (without desallocating them)
extern void pop_reset (Population pop);


/* The following setters desallocate the plan if it can't be added to the population.
 * They desallocate any plan pushed out of the population too.
 */

// Add the plan assuming its makespan is worse than any other in the population
extern result pop_append (Population pop, Plan plan);

// Add the plan at the given position without checking its makespan
extern result pop_insert_at (Population pop, Plan plan, ushort pos);

// Add the plan at the position corresponding to its makespan (keep the population sorted)
extern result pop_insert (Population pop, Plan plan);


#endif // POPULATION_H
