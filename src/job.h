/* Joseph Boudou
 * -------------
 * TP JobShop
 * Cours d'optimisation combinatoire
 * -------------
 * Structure de donnée pour les jobs
 */

#ifndef JOB_H
#define JOB_H

#include "main.h"

struct job;
typedef struct job * Job;



/* Constructors Destructors */

// Allocate a new job
extern Job job_create (ushort initial_size);

// Desallocate the job
extern result job_free (Job job);



/* Job getters/setters */

// Add an operation to the job
extern result job_add_op (Job job, ushort res, ushort duration);

// The ressource id of an arbitrary operation
extern ushort job_op_res (Job job, ushort op_id);


/* Scheduling */

/* Jobs facilitate scheduling by maintaining informations on which operations has been scheduled.
 * The curop iterator point to the next operation to be scheduled.
 */

// Mark all operation as unscheduled and set the current operation to the first one
extern result job_unschedule (Job job);

// Test if all operation as been scheduled (1=true, 0=false)
extern int job_is_scheduled (Job job);

// Mark the current operation as sheduled and set the minimum starting time of the next operation
extern result job_next_op (Job job, int next_minstart);

// The index of the next operation to schedule
extern ushort job_curop_position (Job job);

// The minimum starting time of the next operation to schedule (depends only on job's previous operation duration)
extern int job_curop_minstart (Job job);

// The ressource id of the next operation to schedule
extern ushort job_curop_res (Job job);

// The duration of the next operation to schedule
extern ushort job_curop_duration (Job job);

// Sum of the duration of unscheduled operations
extern int job_remaining_duration (Job job);

#endif
