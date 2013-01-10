/* Joseph Boudou
 * -------------
 * TP JobShop
 * Cours d'optimisation combinatoire
 * -------------
 * Structure de données pour l'ordonnancement d'une seule ressource
 */

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

// Low level version of res_add_task
extern result res_add_task_low(Ressource res, ushort job_id, int min_start, int duration);

// Next free moment
extern int    res_duration(Ressource res);

extern ushort res_max_task_id(Ressource res);

extern ushort res_task_job     (Ressource res, ushort task_id);
extern ushort res_task_duration(Ressource res, ushort task_id);
extern ushort res_task_start   (Ressource res, ushort task_id);
extern ushort res_task_jobstart(Ressource res, ushort task_id);

// Move task a at position b
extern int res_move(Ressource res, ushort task_a_id, ushort task_b_id);

// Create a random crossover which keeps the common partial ordering of tasks.
// The resulting ressource contains arbitrary jobstarts and durations (it has to be replayed).
extern Ressource res_crossover(Ressource res_a, Ressource res_b);

extern void res_output(Ressource res, FILE* stream);

#endif
