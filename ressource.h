#ifndef RESSOURCE_H
#define RESSOURCE_H

#include <stdio.h>

#include "main.h"

struct ressource;
typedef struct ressource * Ressource;

// Create a empty ressource
extern Ressource res_create(ushort initial_size);

// Hard copy
extern Ressource res_copy(Ressource res);

// Soft copy
extern Ressource res_clone(Ressource res);

extern result res_free(Ressource res);

// Add a task
extern result res_add_task(Ressource res, ushort job, ushort op, int min_start, ushort duration);

// Next free moment
extern int    res_duration(Ressource res);

extern ushort res_max_task_id(Ressource res);

extern ushort res_task_job     (Ressource res, ushort task_id);
extern ushort res_task_op      (Ressource res, ushort task_id);
extern ushort res_task_start   (Ressource res, ushort task_id);
extern ushort res_task_duration(Ressource res, ushort task_id);
extern ushort res_task_jobstart(Ressource res, ushort task_id);


extern void res_output(Ressource res, FILE* stream);

#endif
