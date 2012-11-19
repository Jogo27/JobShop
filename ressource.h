#ifndef RESSOURCE_H
#define RESSOURCE_H

#include "main.h"

struct ressource;
typedef struct ressource * Ressource;

// Create a empty ressource
extern Ressource res_create(int start, ushort initial_size);

// Add a task
extern result res_add_task(Ressource res, ushort job, ushort op, int min_start, ushort duration);

// Next free moment
extern int    res_start(Ressource res);

extern ushort res_curtask_job(Ressource res);
extern ushort res_curtask_op(Ressource res);
extern ushort res_curtask_start(Ressource res);
extern ushort res_curtask_duration(Ressource res);
extern ushort res_curtask_jobstart(Ressource res);

extern result res_next_task(Ressource res);
extern result res_rewind_task(Ressource res);

extern result res_delete(Ressource res);

#endif
