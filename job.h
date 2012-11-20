#ifndef JOB_H
#define JOB_H

#include "main.h"

struct job;
typedef struct job * Job;

// Create a new job
extern Job job_create(int start, ushort initial_size);

// Delete the job
extern result job_free(Job job);


// Add a operation to the job
extern result job_add_op(Job job, ushort res, ushort duration);


// Mark all operation as unscheduled and set the current operation to the first one
extern result job_unschedule(Job job, int start);

// Mark the current operation as sheduled and set the minimum starting time of the next operation
extern result job_next_op(Job job, int start);

// Test if all operation as been scheduled (1=true, 0=false)
extern int job_is_scheduled(Job job);


// The index of the next operation to schedule
extern ushort job_curop_position(Job job);

// The minimum starting time of the next operation to schedule (depends only on job's previous operation duration)
extern int job_start(Job job);

// The ressource id of the next operation to schedule
extern ushort job_curop_res(Job job);

// The duration of the next operation to schedule
extern ushort job_curop_duration(Job job);

#endif
