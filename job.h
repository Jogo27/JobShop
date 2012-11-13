#ifndef JOB_H
#define JOB_H

struct job;
typedef struct job * Job;

// Create a new job
extern Job job_new(int start, unsigned short initial_size);

// Add a task to the job
extern int job_add_task(Job job, unsigned short res, unsigned short duration);

// The index of the next task to schedule
extern unsigned short job_cur_position(Job job);

// The minimum starting time of the next task to schedule (depends only on job's previous task duration)
extern int job_start(Job job);

// The ressource id of the next task to schedule
extern unsigned short job_cur_res(Job job);

// The duration of the next task to schedule
extern unsigned short job_cur_duration(Job job);

// Mark the current task as sheduled and set the minimum starting time of the next task
extern int job_next_task(Job job, int start);

// Delete the job
extern int job_free(Job job);

#endif
