#ifndef JOB_H
#define JOB_H

#include "main.h"

struct job;
typedef struct job * Job;

// Create a new job
extern Job job_create(int start, ushort initial_size);

// Add a operation to the job
extern result job_add_op(Job job, ushort res, ushort duration);

// The index of the next operation to schedule
extern ushort job_curop_position(Job job);

// The minimum starting time of the next operation to schedule (depends only on job's previous operation duration)
extern int job_start(Job job);

// The ressource id of the next operation to schedule
extern ushort job_curop_res(Job job);

// The duration of the next operation to schedule
extern ushort job_curop_duration(Job job);

// Mark the current operation as sheduled and set the minimum starting time of the next operation
extern result job_next_op(Job job, int start);

// Delete the job
extern result job_free(Job job);

#endif
