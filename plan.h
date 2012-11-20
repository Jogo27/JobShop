#ifndef PLAN_H
#define PLAN_H

#include <stdio.h>

#include "main.h"
#include "job.h"


struct plan;
typedef struct plan * Plan;

extern Plan   plan_create(int start, ushort nb_res, ushort nb_tasks);
extern result plan_free(Plan plan);

extern int plan_duration(Plan plan);

extern result plan_schedule(Plan plan, Job job, ushort job_id);

extern void plan_output(Plan plan, FILE * stream);

#endif
