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



/* Constructors Destructors */

/* Ressources have reference count to be cheaply copyed (cloned). A cloned ressource can't be
 * modified until res_prepare_for_write is called on it.
 */

// Create a empty ressource
extern Ressource res_create (ushort initial_size);

// Hard copy
extern Ressource res_copy (Ressource res);

// Soft copy
extern Ressource res_clone (Ressource res);

// Eventualy turn a soft copy to a hard one
extern void res_prepare_for_write (Ressource * res);

// Desallocate a ressource (unless it has been cloned)
extern result res_free (Ressource res);



/* Ressource Getters and Setters */

// Ressources are equals if their job sequence are equals
extern int res_equals (Ressource res_a, Ressource res_b);

// Add a task (size is increased if needed)
extern result res_add_task (Ressource res, Job job, ushort job_id);

// Low level version of res_add_task (use with care)
extern result res_add_task_low (Ressource res, ushort job_id, int min_start, int duration);

// Move task a at position b
extern int res_move (Ressource res, ushort task_a_id, ushort task_b_id);

// Next free moment
extern int    res_duration (Ressource res);

// Output the sequence of job identifier separated by spaces
extern void res_output (Ressource res, FILE* stream);



/* Tasks Getters and Setters */

// task_id of the last task
extern ushort res_max_task_id (Ressource res);

// Job identifier of the task
extern ushort res_task_job     (Ressource res, ushort task_id);

// Duration of the task
extern ushort res_task_duration (Ressource res, ushort task_id);

// Planned start time of the task
extern ushort res_task_start   (Ressource res, ushort task_id);

// Minimum start time the corresponding operation could have been started considering only the
// preceding operation of the same job
extern ushort res_task_jobstart (Ressource res, ushort task_id);



/* Crossover */

// Classic one point crossover algorithm on tasks
// The resulting ressource has to be replayed
extern Ressource * res_crossover_onepoint (Ressource res_a, Ressource res_b);

// Create two random crossovers which keeps the common partial ordering of tasks.
// The resulting ressource contains arbitrary jobstarts and durations (it has to be replayed).
extern Ressource * res_crossover_order (Ressource res_a, Ressource res_b);

#endif
