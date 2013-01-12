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

extern Plan   plan_create(Prob prob);
extern Plan   plan_clone(Plan plan);
extern result plan_free(Plan plan);


extern int plan_duration(Plan plan);

extern int plan_sum_makespan(Plan plan);

extern Ressource plan_get_ressource(Plan plan, ushort res_id);

extern int plan_equals(Plan plan_a, Plan plan_b);

// Function type to provide to handle newly created plan
typedef void (*process_new_plan)(Plan,void*);

// Neighbourhood functions

extern result plan_schedule(Plan plan, Prob prob, ushort job_id);


// For each neighbour plan, call the function
extern void plan_neighbourhood(Plan plan, Prob prob,
                        process_new_plan function, void * function_data);
extern void plan_neighbourhood_one(Plan plan, ushort res_id, Prob prob,
                        process_new_plan function, void * function_data);
extern void plan_neighbourhood_perm(Plan plan, ushort res_id, Prob prob,
                        process_new_plan function, void * function_data);

// Only compute neighbourhood for worse duration ressources
extern void plan_neighbourhood_worse(Plan plan, Prob prob,
                        process_new_plan function, void * function_data);

extern Plan plan_reduce_critical_path (Plan plan, Prob prob);

extern Plan plan_merge_res (Plan plan_a, Plan plan_b, Prob prob);

extern void plan_crossover_task (Plan plan_a, Plan plan_b, Prob prob,
                        Ressource * (*res_crossover)(Ressource,Ressource), process_new_plan function, void * function_data);

static inline void plan_crossover_task_onepoint (Plan plan_a, Plan plan_b, Prob prob,
                        process_new_plan function, void * function_data) {
  plan_crossover_task(plan_a, plan_b, prob, &res_crossover_onepoint, function, function_data);
}

static inline void plan_crossover_task_order (Plan plan_a, Plan plan_b, Prob prob,
                        process_new_plan function, void * function_data) {
  plan_crossover_task(plan_a, plan_b, prob, &res_crossover_order, function, function_data);
}

extern Plan plan_merge_task(Plan plan_a, Plan plan_b, Prob prob);


extern void plan_output(Plan plan, FILE * stream);

#endif
