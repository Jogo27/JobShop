/* Joseph Boudou
 * -------------
 * TP JobShop
 * Cours d'optimisation combinatoire
 * -------------
 * Structure de données pour les solutions
 */

#ifndef PLAN_H
#define PLAN_H

#include <stdio.h>

#include "main.h"
#include "job.h"
#include "prob.h"
#include "ressource.h"


struct plan;
typedef struct plan * Plan;

// Function handling newly created plan
typedef void (*process_new_plan) (Plan,void*);



/* Constructors/Destructors */

// Allocate a brand new plan
extern Plan   plan_create (Prob prob);

// Return a copy of plan in which ressources are clones of plan's ressources
extern Plan   plan_clone (Plan plan);

// Desallocate
extern result plan_free (Plan plan);



/* Plan Getters/Setters */

// Plans are equals if their ressources are equals
extern int plan_equals (Plan plan_a, Plan plan_b);

// Retrieve the given ressource
extern Ressource plan_get_ressource (Plan plan, ushort res_id);

// Add to the correct ressource a task corresponding to the "current" operation of the job job_id
// from prob. Then update prob.
extern result plan_schedule (Plan plan, Prob prob, ushort job_id);

// Cmax: the makespan of the complete plan
extern int plan_duration (Plan plan);

// The sum of the makespans of every ressource in the plan
extern int plan_sum_makespan (Plan plan);

// Output the plan, one line per ressource
extern void plan_output (Plan plan, FILE * stream);



/* Neighbourhood functions */

// All admissible adjacent transpositions of every ressources
extern void plan_neighbourhood (Plan plan, Prob prob,
                        process_new_plan function, void * function_data);

// All admissible adjacent transpositions of one ressources
extern void plan_neighbourhood_one (Plan plan, ushort res_id, Prob prob,
                        process_new_plan function, void * function_data);

// All admissible transpositions of every ressources
extern void plan_neighbourhood_perm (Plan plan, ushort res_id, Prob prob,
                        process_new_plan function, void * function_data);

// All admissible adjacent transpositions of the longest makespan ressources
extern void plan_neighbourhood_worse (Plan plan, Prob prob,
                        process_new_plan function, void * function_data);

// One transposition on the critical path
extern Plan plan_reduce_critical_path (Plan plan, Prob prob);



/* Crossover functions */

// Private function
extern void plan_crossover_task (Plan plan_a, Plan plan_b, Prob prob,
                        Ressource * (*res_crossover) (Ressource,Ressource), process_new_plan function, void * function_data);

// Classical one point crossover on every (non-equal) ressource
static inline void plan_crossover_task_onepoint (Plan plan_a, Plan plan_b, Prob prob,
                        process_new_plan function, void * function_data) {
  plan_crossover_task(plan_a, plan_b, prob, &res_crossover_onepoint, function, function_data);
}

// Stochastic crossover preserving tasks' partial ordering common to both parents
static inline void plan_crossover_task_order (Plan plan_a, Plan plan_b, Prob prob,
                        process_new_plan function, void * function_data) {
  plan_crossover_task(plan_a, plan_b, prob, &res_crossover_order, function, function_data);
}


#endif
