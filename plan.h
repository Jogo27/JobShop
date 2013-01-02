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
extern result plan_free(Plan plan);


extern int plan_duration(Plan plan);

extern Ressource plan_get_ressource(Plan plan, ushort res_id);

extern int plan_equals(Plan plan_a, Plan plan_b);


extern result plan_schedule(Plan plan, Prob prob, ushort job_id);


// For each neighbour plan, call the function
extern void plan_neighbourhood(Plan plan, Prob prob,
                        void (*function)(Plan,void *), void * function_data);
extern void plan_neighbourhood_one(Plan plan, ushort res_id, Prob prob,
                        void (*function)(Plan,void *), void * function_data);

// Only compute neighbourhood for worse duration ressources
extern void plan_neighbourhood_worse(Plan plan, Prob prob,
                        void (*function)(Plan,void *), void * function_data);

extern Plan plan_merge_res(Plan plan_a, Plan plan_b, Prob prob);
extern Plan plan_merge_task(Plan plan_a, Plan plan_b, Prob prob);


extern void plan_output(Plan plan, FILE * stream);

#endif
