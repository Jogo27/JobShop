#ifndef RESSOURCE_H
#define RESSOURCE_H

#include <stdio.h>

#include "main.h"
#include "job.h"

struct ressource;
typedef struct ressource * Ressource;

// Create a empty ressource
extern Ressource res_create(ushort initial_size);

// Hard copy
extern Ressource res_copy(Ressource res);

// Soft copy
extern Ressource res_clone(Ressource res);

extern result res_free(Ressource res);

extern int res_equals(Ressource res_a, Ressource res_b);

// Add a task
extern result res_add_task(Ressource res, Job job, ushort job_id);

// Next free moment
extern int    res_duration(Ressource res);

extern ushort res_max_task_id(Ressource res);

extern ushort res_task_job     (Ressource res, ushort task_id);
extern ushort res_task_duration(Ressource res, ushort task_id);
extern ushort res_task_start   (Ressource res, ushort task_id);
extern ushort res_task_jobstart(Ressource res, ushort task_id);

// Swap tasks a and b
extern int res_swap(Ressource res, ushort task_a_id, ushort task_b_id);

extern void res_output(Ressource res, FILE* stream);

#endif
