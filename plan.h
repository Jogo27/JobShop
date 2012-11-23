#ifndef PLAN_H
#define PLAN_H

#include <stdio.h>

#include "main.h"
#include "job.h"
#include "ressource.h"


struct plan;
typedef struct plan * Plan;

extern Plan   plan_create(ushort nb_res, ushort nb_tasks);
extern result plan_free(Plan plan);

extern int plan_duration(Plan plan);

extern Ressource plan_get_ressource(Plan plan, ushort res_id);

extern result plan_schedule(Plan plan, Job job, ushort job_id);

extern void plan_output(Plan plan, FILE * stream);

#endif
